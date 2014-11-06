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
	if( fnc.type == TM_FNC){
        if( get_func(fnc)->self.type != TM_NON) {
            list_insert( get_list(params), 0, get_func(fnc)->self);
        }
        if( get_func(fnc)->native_func != NULL ){
            return get_func(fnc)->native_func(params);
        }
        return tm_eval(fnc, params);
    }else if( fnc.type == TM_DCT){
      tm_log1("stack", "new instance %t", fnc);
        fnc = class_new(fnc);
        tm_log0("stack", "class new done");
        if( _tm_has(fnc, obj__init__)){
            tm_obj f = tm_get(fnc, obj__init__);
            _tm_call(f, params);
        }
        tm_log1("stack", "instance %t built", fnc);
        return fnc;
    }
    tm_raise("tm_call(), not callable , type = %d", fnc.type);
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
#define TM_PUSH( x ) *(++top) = (x); /*if( top - f->stack >= 50) tm_raise("stack overflow");*/
#define TM_POP() *(top--)
#define TM_TOP() (*top)

#define CASE( code, body ) case code :  body ; break;

struct tm_def_st{
    tm_obj fnc;
    int len;
};
struct tm_def_st tm_def(tm_obj mod, char* s){
	int maxlocals = 0;
	int maxstack = 0;
	int len = code_check( mod, s, 1, &maxlocals, &maxstack);
	tm_obj fnc = func_new(mod, obj_none, NULL);
	get_func(fnc)->pc = s;
	get_func(fnc)->maxlocals = maxlocals;
    get_func(fnc)->maxstack = maxstack;
	struct tm_def_st def ;
    def.fnc = fnc;
    def.len = len;
    return def;
}

#define TM_OP( OP_CODE, OP_FUNC ) case OP_CODE: x = TM_POP();\
  v = TM_TOP();\
  tm_log0("ins", #OP_CODE"");\
  TM_TOP() = OP_FUNC(v, x);\
  goto start;

// if i == 1, j = top;
// if i == 0, j = top + 1;
#define LOAD_LIST(params, i)  params = list_new(i);\
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

    tm_log1("stack", "enter function %o", fnc);
/* check if frame overflow */
  // if( tm->cur >= FRAMES_COUNT)
  //   tm_raise("tm_eval: frame overflow");

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
  // f->top = top;
  
  top[0] = obj_none;
  list_len(f->new_objs) = 0;
  
  if( f->maxlocals > 200)
  tm_log2("locals", "in %t, locals over %d", fnc, 200);

  tm_obj* constants = get_constants(mod);
  tm_obj x, k, v;
  tm_obj func;
  tm_obj ret = obj_none;
  tm_obj templist;
  
  int i, ins, jmp;

  if( ! get_mod(mod)->checked ){
    code_check( mod, s, 0, &i, &jmp);
  }

  unsigned char** tags = get_mod(mod)->tags;
  

 start:
  ins = next_byte(s);
  switch( ins ) {

  case NEW_NUMBER: {
    double d;
    read_number( d, s);
    v = tm_number(d);
    constants = push_constant( mod , v);
    tm_log1("new", "NEW_NUMBER %f", d);
    goto start;
  }

  case NEW_STRING: {
    int len = next_short( s );
    v = str_new( s, len);
    s+=len;
    constants = push_constant( mod, v);
    tm_log2("new", "NEW_STRING [%d] @", len, v);
    goto start;
  }

  case LOAD_CONSTANT: {
    i = next_short( s );
    TM_PUSH( constants[ i ] );
    tm_log2("ins", "LOAD_CONSTANT [%d] @", i, constants[i]);
    goto start;
  }

  case LOAD_LOCAL: {
    i = next_char(s);
    tm_log1("ins", "LOAD_LOCAL %d", i);
    TM_PUSH( locals[i] );
    goto start;
  }

  case STORE_LOCAL:
    i = next_char( s );
    locals[i] = TM_POP();
    tm_log1("ins", "STORE_LOCAL %d", i);
    goto start;

  case LOAD_GLOBAL: {
    i = next_short(s);
    tm_log2("ins", "LOAD_GLOBAL [%d] @", (i), constants[i]);
    k = constants[ i ];
    if( dict_iget( get_dict(tm->builtins), k, &v)){
    }else{
      v = tm_get(globals, k);
    }
    TM_PUSH( v );
    goto start;
  }

  case STORE_GLOBAL:{
    i = next_short( s );
    k = constants[ i ];
    x = TM_POP();
    tm_set( globals, k, x );
    tm_log2("ins", "STORE_GLOBAL [%d] @", i, k);
    goto start;
  }

  case LIST:{
    i = next_byte(s);
    tm_log1("ins", "LIST %d", i);
    TM_PUSH( list_new(i) );
    goto start;
  }

  case LIST_APPEND:
    tm_log0("ins", "LIST_APPEND");
    v = TM_POP();
    x = TM_TOP();
    if( x.type != TM_LST){
      tm_raise("tm_eval: LIST_APPEND expect a list but see %t", x);
    }
    list_append(get_list(x), v);
    goto start;
  
  case DICT_SET:
    tm_log0("ins", "DICT_SET");
    v = TM_POP();
    k = TM_POP();
    x = TM_TOP();
    if( x.type != TM_DCT){
        tm_raise("tm_eval(): DICT_SET expect a dict but see %t", x);
    }
    tm_set(x, k, v);
    goto start;

  case DICT:{
    i = next_byte(s);
    tm_log1("ins", "DICT %d", i);
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
    tm_log0("ins", "SET");
    goto start;

  case POP:{
    TM_POP();
    tm_log0("ins", "POP");
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
    tm_log1("ins", "CALL %d", i);
    /* set a global `arguments to store arguments */
    // list_len(g_arguments) = 0;
    /* if( 0 == i) { params.type = TM_NON; }
    else { LOAD_LIST(params, i); } */
    LOAD_LIST(params, i);
    func = TM_POP();
    // f->top = top;
    TM_PUSH( _tm_call(func, params));
    goto start;
  }break;

  case LOAD_PARAMS:{
    if( TM_LST != params.type ){
      // tm_raise("tm_eval(), expect params to be list, but see %t", params);
      goto start;
    }
    tm_log1("ins2", "LOAD_PARAMS %l", params);
    for(i = 0; i < list_len(params); i++){
      locals[i] = list_nodes(params)[i];
    }
    // list_len(params) = 0;
    goto start;
  }

  case TM_FOR:{
    jmp = next_short(s);
    k = *top;
    x = *(top-1);
    tm_log1("ins", "TM_FOR %d", jmp);
    if( tm_iter( x, &k) ){
      get_num(*top) += 1;
      // cprintln( x );
      TM_PUSH( k );
      // cprintln(v);
      goto start;
    }else{
      s = tags[jmp];
    }
    goto start;
  }

  case TM_DEF:{
    i = next_short(s);
    struct tm_def_st def = tm_def(mod, s);
    get_func(def.fnc)->name = constants[i];
    s+= def.len;
    tm_log1("ins", "TM_DEF %d", def.len);
    TM_PUSH(def.fnc);
    goto start;
  }

  case RETURN:{
    tm_log0("stack", "RETURN");
    ret = TM_POP();
    goto end;
  }

  case TAG:{
    i = next_short(s);
    tm_log1("info","TAG %d", i);
    // TODO store_tag(mod, i, s);
    goto start;
  }

  case TAGSIZE:{
    i = next_short(s);
    tm_log1("info", "TAGSIZE %d", i);
    goto start;
  }

  case POP_JUMP_ON_TRUE:{
    i = next_short(s);
    tm_log1("cond", "POP_JUMP_ON_TRUE %d", i);
    if( _tm_bool( TM_POP() )){
      s = tags[i];
    }
    goto start;
  }

  case POP_JUMP_ON_FALSE:{
    i = next_short(s);
    tm_log1("cond", "POP_JUMP_ON_FALSE %d", i);
    if( !_tm_bool( TM_POP() )){
      s = tags[i];
    }
    goto start;
  }

  case JUMP_ON_TRUE:{
    i = next_short(s);
    tm_log1("cond", "JUMP_ON_TRUE %d", i);
    if( _tm_bool( TM_TOP() )){
      s = tags[i];
    }
    goto start;
  }

  case JUMP_ON_FALSE:{
    i = next_short(s);
    tm_log1("cond", "JUMP_ON_FALSE %d", i);
    if( !_tm_bool( TM_TOP() )){
      s = tags[i];
    }
    goto start;
  }

  case JUMP:
    i = next_short(s);
    tm_log1("cond", "JUMP %d", i);
    s = tags[i];
    goto start;

  case TM_EOF:{
    tm_log0("stack", "TM_EOF");
    // restore_frame();
    ret = obj_none;
    goto end;
  }

  case TM_EOP:{
    tm_log0("stack", "TM_EOP");
    // restore_frame();
    ret = obj_none;
    goto end;
  }

  default:
    tm_raise("BAD INSTRUCTION, %d\n  globals() = \n@", ins,f->globals );
  goto end;
}

  end:
    tm_log1("stack", "leave function %o", fnc);
    if( top != f->stack ) {
        printf("func_name = %s, count = %d\n", get_str(f->func_name) , (int)( top - f->stack));
    }
    if( tm->allocated_mem > tm->gc_limit){
        tm->gc_limit += 1024 * 1024; // increase 1M
        gc_full(ret);
    }
    tm->cur--;
    list_len(f->new_objs) = 0;
    return ret;
}
