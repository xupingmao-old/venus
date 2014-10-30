/**

   date : 2014-9-2
**/

#include "tm.h"
#include "instruction.h"

typedef union tm_opcode {
  char* str;
  double* number;
  struct opcode{
    char type;
    int value;
  } opcode;
}tm_opcode;


// 一个模块的常量
tm_obj* push_constant(tm_obj mod, tm_obj v){
    tm_obj cs = get_mod(mod)->constants;
    int i = list_index( get_list(cs), v);
    if( i == -1 ){
        list_append(get_list(cs), v);
    }
    return get_list(cs)->nodes;
}

tm_obj _tm_call( tm_obj fnc, tm_obj params){
    // cprintln(params);
    // cprintln(fnc);
	if( fnc.type == TM_FNC){
        if( get_func(fnc)->self.type != TM_NON) {
            list_insert( get_list(params), 0, get_func(fnc)->self);
        }
        if( get_func(fnc)->native_func != NULL ){
            return get_func(fnc)->native_func(params);
        }
        return tm_eval(fnc, params);
    }else if( fnc.type == TM_DCT){
        fnc = class_new(fnc);
        if( _tm_has(fnc, obj__init__)){
            tm_obj f = tm_get(fnc, obj__init__);
            list_insert( get_list(params), 0, fnc);
            tm_eval(f , params);
        }
        return fnc;
    }
}

tm_obj tm_call( char* mod, char* fnc, tm_obj params){
  tm_obj m ;
  m = tm_get(tm->modules, str_new(mod, strlen(mod)));
  tm_obj f = tm_get( m, str_new(fnc, strlen(fnc)));
  return _tm_call(f, params);
}

tm_obj* get_constants(tm_obj mod){
  tm_obj constants = get_mod(mod)->constants;
  return get_list(constants)->nodes;
}


#define read_number( v, s) v = *(double*)s; s+=sizeof(double);
#define TM_PUSH( x ) *(++top) = (x); /*if( top >= f->stack + 100) tm_raise("stack overflow");*/
#define TM_POP() *(top--)
#define TM_TOP() (*top)

#define CASE( code, body ) case code :  body ; break;

tm_obj tm_def(tm_obj mod, char* s){
	int maxlocals = 0;
	int maxstack = 0;
	int len = code_check( mod, s, 1, &maxlocals, &maxstack);
	tm_obj code = str_new(s , len);
	tm_obj fnc = func_new(mod, code, obj_none, NULL);
	get_func(fnc)->pc = s;
	get_func(fnc)->maxlocals = maxlocals;
  get_func(fnc)->maxstack = maxstack;
	return fnc;
}

#if PRINT_INS
#define TM_OP( OP_CODE, OP_FUNC ) case OP_CODE: x = TM_POP();\
  v = TM_TOP();\
  puts(#OP_CODE"");  \
  TM_TOP() = OP_FUNC(v, x);\
  goto start;
#else
#define TM_OP( OP_CODE, OP_FUNC ) case OP_CODE: x = TM_POP();\
  v = TM_TOP();\
  TM_TOP() = OP_FUNC(v, x);\
  goto start;
#endif

// if i == 1, j = top;
// if i == 0, j = top + 1;
#define LOAD_LIST(params, i)  params = list_new(i); \
    tm_list* _p = get_list(params); \
    tm_obj* j; \
    for(j = top - i + 1; j <= top; j++){  \
      list_append(_p, *j); \
    }  \
    top-=i;

// if n == 0, j = top + 1;
// if n == 1, j = top - 1 ,
#define LOAD_DICT( dict , n ) dict = dict_new();        \
    tm_obj*j;                                          \
    for(j = top - n * 2 + 1; j < top; j+=2){          \
      dict_set( get_dict(dict), *j, *(j+1));             \
    }                                                  \
    top -= n * 2;

#define restore_frame()   tm->cur --;\
  f = tm->frames + tm->cur; \
  top = f->last_pc;         \
  s = f->last_code;         \
  locals = f->locals;

#define save_frame()    f->last_pc = top;   \
  f->last_code = s;


#define new_frame()   tm->cur++;\
  f = tm->frames + tm->cur;\
  top = f->stack;                   \
  locals = f->locals;

tm_obj tm_eval( tm_obj fnc, tm_obj params ){

    tm->cur++;
/* check if frame overflow */
  if( tm->cur >= FRAMES_COUNT)
    tm_raise("tm_eval: frame overflow");

  tm_obj mod = get_func(fnc)->mod;
  tm_obj globals = get_mod(mod)->globals;
  tm_obj code = get_mod(mod)->code;
  unsigned char* s = get_func(fnc)->pc;
  // constants will be built in modules.
  // get constants from function object.
  tm_frame* f = tm->frames + tm->cur;
  f->file = get_mod(mod)->file;
  f->globals = globals;
  f->func_name = get_func(fnc)->name;
  f->maxlocals = get_func(fnc)->maxlocals;
  f->maxstack  = get_func(fnc)->maxstack;
  f->constants = get_mod(mod)->constants;
  tm_obj* locals = f->locals;
  tm_obj* top = f->stack;
  
  top[0] = obj_none;
  // get_list(f->new_objs)->len = 0;
  
  if( f->maxlocals > 200) {
    cprintln(fnc);
  }

  tm_obj* constants = get_constants(mod);
  tm_obj x, k, v;
  tm_obj func;
  tm_obj ret = obj_none;
  tm_obj templist;
  
  int i, ins, jmp;

  /* will optimize later */
  /*
  for(i = 0; i < 256; i++){
    locals[i].type = TM_NON;
  }
  for(i = 0; i < 256; i++){
    top[i].type = TM_NON;
  }*/

  if( ! get_mod(mod)->checked ){
    code_check( mod, s, 0, &i, &jmp);
  }

//  cprintln_show_special(params);
  unsigned char** tags = get_mod(mod)->tags;
  

 start:
  ins = next_byte(s);
/*
if( enable_debug ){
  char c = getch();
  if(c == 'd'){
    cprintln( f->globals);
    tm_printf("x = @\n", x);
    tm_printf("v = @\n", v);
    tm_printf("k = @\n", k);
  }else if ( c == 'l'){
    tm_obj temp = build_list( 10, locals);
    cprintln( temp );
  }else if (c == 'g'){
    cprintln( f->globals );
  }else if (c == 'q'){
    tm_raise("quit");
  }
}*/
  switch( ins ) {

  case NEW_NUMBER: {
    double d;
    read_number( d, s);
    v = tm_number(d);
    constants = push_constant( mod , v);
#if PRINT_INS_CONST
    printf("NEW_NUMBER %g\n", d);
#endif
    goto start;
  }

  case NEW_STRING: {
    int len = next_short( s );
    v = str_new( s, len);
    s+=len;
    constants = push_constant( mod, v);
#if PRINT_INS_CONST
    // tm_printf("NEW_STRING \"@\"\n", v);
    printf("NEW_STRING [%d] ", len);
    puts(get_str(v));
#endif
    goto start;
  }

  case LOAD_CONSTANT: {
    i = next_short( s );
    TM_PUSH( constants[ i ] );
#if PRINT_INS 
    tm_printf("LOAD_CONSTANT [@] ",number_new(i));
    cprintln_show_special(constants[i]);
#endif
    goto start;
  }

  case LOAD_LOCAL: {
    i = next_char(s);
    #if PRINT_INS
        printf("LOAD_LOCAL %d\n", i);
    #endif
    TM_PUSH( locals[i] );
    goto start;
  }

  case STORE_LOCAL:
    i = next_char( s );
    locals[i] = TM_POP();
    #if PRINT_INS
        printf("STORE_LOCAL %d\n", i);
    #endif
    goto start;

  case LOAD_GLOBAL: {
    i = next_short(s);
#if PRINT_INS
    // tm_printf("LOAD_GLOBAL [@] @\n", number_new(i), constants[i]);
#endif
    // cprintln( get_mod(mod)->constants);
    k = constants[ i ];
    // puts("step1");
    if( dict_iget( get_dict(tm->builtins), k, &v)){
      // already in v;
      // puts("step2");
    }else{
      // cprintln(globals);
      // cprintln(k);
      // puts("step3");
      // cprintln(globals);
      // cprintln(k);
      v = tm_get(globals, k);
      // cprintln(v);
      // puts("step4");
    }
    // puts("step5");
    TM_PUSH( v );
#if PRINT_INS
    tm_printf("LOAD_GLOBAL [@] @  \n", number_new(i), k);
#endif
    goto start;
  }

  case STORE_GLOBAL:{
    i = next_short( s );
    k = constants[ i ];
    x = TM_POP();
    tm_set( globals, k, x );
#if PRINT_INS
    tm_printf("STORE_GLOBAL [@] @\n", number_new(i), k);
#endif
    goto start;
  }

  case LIST:{
    i = next_byte(s);
    #if PRINT_INS
        printf("LIST %d\n", i);
    #endif
    LOAD_LIST( templist, i );
    TM_PUSH( templist );
    goto start;
  }

  case LIST_APPEND:
    #if PRINT_INS
      printf("LIST_APPEND\n");
    #endif
    v = TM_POP();
    x = TM_TOP();
    if( x.type != TM_LST){
      tm_raise("tm_eval: LIST_APPEND expect a list but see @", x);
    }
    list_append(get_list(x), v);
    goto start;

  case DICT:{
    i = next_byte(s);
    #if PRINT_INS
      printf("DICT %d\n", i);
    #endif
    tm_obj dict;
    LOAD_DICT( dict, i);
    TM_PUSH( dict );
    goto start;
  }

  TM_OP( ADD, tm_add);
  TM_OP( SUB, tm_sub);
  TM_OP( MUL, tm_mul);
  TM_OP( DIV, tm_div);
  TM_OP( MOD, tm_mod);
  TM_OP( GET, tm_get);
  TM_OP( EQEQ, t_tm_equals);
  TM_OP( NOTEQ, tm_not_equals);
  TM_OP( LT, tm_lt);
  TM_OP( LTEQ,  tm_lteq);
  TM_OP( GT, tm_gt);
  TM_OP( GTEQ, tm_gteq);
  TM_OP( IN, tm_in);
  TM_OP( NOTIN, tm_notin);
  TM_OP(AND, tm_and);
  TM_OP(OR, tm_or);
  
  case SET:
    k = TM_POP();
    x = TM_POP();
    v = TM_POP();
    tm_set(x, k, v);
#if PRINT_INS
    puts("SET");
#endif
    goto start;

  case POP:{
    TM_POP();
#if PRINT_INS
    puts("POP");
#endif
    goto start;
  }

  case NOT:
    TM_TOP() = tm_not(TM_TOP());
    goto start;

  case NEG:
    TM_TOP() = tm_neg(TM_TOP());
    goto start;
    
  case CALL: {
    i = next_byte( s );
#if PRINT_INS
    printf("CALL %d\n", i);
#endif
    LOAD_LIST(params, i);
    func = TM_POP();
    f->top = top;
    TM_PUSH( _tm_call(func, params));
    goto start;
  }break;

  case LOAD_PARAMS:{
#if PRINT_INS_CONST
    tm_printf("LOAD_PARAMS @\n", params);
#endif
    int len = list_len(params);
    for(i = 0; i < len; i++){
      locals[i] = get_list(params)->nodes[i];
    }
#if PRINT_INS
    puts("PARAMS END");
#endif
    goto start;
  }

  case TM_FOR:{
    jmp = next_short(s);
    k = *top;
    x = *(top-1);
#if PRINT_INS
    printf("TM_FOR %d\n", jmp);
#endif
    if( tm_iter( x, k, &v) ){
      get_num(*top) += 1;
      // cprintln( x );
      TM_PUSH( v );
      // cprintln(v);
      goto start;
    }else{
      s = tags[jmp];
    }
    goto start;
  }

  case TM_DEF:{
    i = next_short(s);
    func = tm_def(mod, s);
    get_func(func)->name = constants[i];
    tm_obj code = get_func(func)->code;
    s+= get_str_len(code);
#if PRINT_INS
    printf("TM_DEF %d\n",get_str_len(code));
#endif
    TM_PUSH(func);
    goto start;
  }

  case RETURN:{
#if PRINT_INS
    puts("RETURN");
#endif
    ret = TM_POP();
    goto end;
  }

  case TAG:{
    i = next_short(s);
#if PRINT_INS
    printf("TAG %d\n", i);
#endif
    // TODO store_tag(mod, i, s);
    goto start;
  }

  case TAGSIZE:{
    i = next_short(s);
#if PRINT_INS
    printf("TAGSIZE %d\n", i);
#endif
    goto start;
  }

  case POP_JUMP_ON_TRUE:{
    i = next_short(s);
#if PRINT_INS
    printf("POP_JUMP_ON_TRUE %d\n", i);
#endif
    if( _tm_bool( TM_POP() )){
      s = tags[i];
    }
    goto start;
  }

  case POP_JUMP_ON_FALSE:{
    // print_tags(get_mod(mod));
    i = next_short(s);
#if PRINT_INS
    // cprintln(TM_TOP());
    printf("POP_JUMP_ON_FALSE %d\n", i);
#endif
    if( !_tm_bool( TM_POP() )){
      s = tags[i];
    }
    goto start;
  }

  case JUMP_ON_TRUE:{
    i = next_short(s);
#if PRINT_INS
    printf("JUMP_ON_TRUE %d\n", i);
#endif
    if( _tm_bool( TM_TOP() )){
      s = tags[i];
    }
    goto start;
  }

  case JUMP_ON_FALSE:{
    // print_tags(get_mod(mod));
    i = next_short(s);
#if PRINT_INS
    // cprintln(TM_TOP());
    printf("JUMP_ON_FALSE %d\n", i);
#endif
    if( !_tm_bool( TM_TOP() )){
      s = tags[i];
    }
    goto start;
  }

  case JUMP:
    i = next_short(s);
#if PRINT_INS
    printf("JUMP %d\n", i);
#endif
    s = tags[i];
    goto start;

  case TM_EOF:{
#if PRINT_INS
    puts("TM_EOF");
#endif
    // restore_frame();
    ret = obj_none;
    goto end;
  }

  case TM_EOP:{
#if PRINT_INS
    puts("TM_EOP");
#endif
    // restore_frame();
    ret = obj_none;
    goto end;
  }

  default:
    cprintln(f->globals);
    tm_raise("BAD INSTRUCTION, @\n", number_new(ins));
  goto end;
}
  // cprintln(mod);

/* call_func:

  if( func.type == TM_DCT ){
    ret = class_new( func );
    if( _tm_has(ret, str_new("__init__", -1)) ){

    }
    goto func_ret;
  }
  if( get_func(func)->self.type != TM_NON){
    list_insert(get_list(params),0, get_func(func)->self);
  }
  if( get_func(func)->native_func != NULL ){
    ret = get_func(func)->native_func(params);
    goto func_ret;
  }
  // user function
  // save current state
  save_frame();
  // new frame
  new_frame();
  s = get_func(func)->pc;
  goto start;*/

  end:
  if( top != f->stack ) {
    printf("func_name = %s, count = %d\n", get_str(f->func_name) , (int)( top - f->stack));
    // tm_raise("top stack leaks");
  }
    if( tm->allocated_mem > tm->gc_limit){
        tm->gc_limit += 1024;
        tm_printf("full gc at @\n", f->func_name);
        // gc_mark(params);
        gc_full(ret);
        // int i;
        // for(i = 0; i < tm->cur; i++){
        // 	tm_frame* f = tm->frames+i;
        // 	printf("func_name = %s, maxstack = %d, maxlocals = %d\n", get_str(f->func_name),
        // 	f->maxstack, f->maxlocals );
        // }
    }
    tm->cur--;
    // get_list(f->new_objs)->len = 0;
    return ret;
}
