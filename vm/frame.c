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
tm_obj* define_constant(tm_obj mod, tm_obj v){
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
	struct tm_check_result_st rs = code_check( mod, s, 1 );
	tm_obj fnc = func_new(mod, obj_none, NULL);
	get_func(fnc)->pc = rs.pc;
	get_func(fnc)->maxlocals = rs.maxlocals;
    get_func(fnc)->maxstack = rs.maxstack;
	struct tm_def_st def ;
    def.fnc = fnc;
    def.len = rs.len;
    return def;
}

#define TM_OP( OP_CODE, OP_FUNC ) case OP_CODE: \
  tm_log0("ins", #OP_CODE"");\
  *(top-1) = OP_FUNC(*(top-1), *top);--top;\
  break;

#define TM_OP2( OP_CODE, OP_FUNC) case OP_CODE:\
    if( !OP_FUNC( *(top-1), *top) ){ \
        s = tags[i]; top-=2; continue; \
    }; top-=2; break;

// if i == 1, j = top;
// if i == 0, j = top + 1;
#define LOAD_LIST(params, i)  params = list_new(i);\
    tm_list* _p = get_list(params); \
    tm_obj* j; \
    for(j = top - i + 1; j <= top; j++){  \
      list_append(_p, *j); \
    }  \
    top-=i;
#define LOAD_LIST2(p_, i)\
    tm_list*_p = get_list((p_));\
    tm_obj*j;for(j = top - i + 1; j <= top; j++){\
        list_append(_p, *j);\
    } top-=i;
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
  
tm_frame* build_frame(tm_obj fnc){
    tm->cur++;
    tm_frame* f = tm->frames+tm->cur;
    
    tm_log1("stack", "enter function %o", fnc);
    
    /* check if frame overflow */
    if( tm->cur >= FRAMES_COUNT)
        tm_raise("tm_eval: frame overflow");

    tm_obj mod = get_func(fnc)->mod;
    f->code = get_func(fnc)->pc;
    if( ! get_mod(mod)->checked ){
        code_check( mod, f->code, 0);
        // printf("code check done\n");
    }
    // constants will be built in modules.
    // get constants from function object.
    // f->file = get_mod(mod)->file;
    f->fnc = fnc;
    f->tags = get_fnc_mod_ptr(fnc)->tags;
    list_len(f->new_objs) = 0;
    return f;
}

void printInst(int code, int val, tm_obj* constants){
    if( code == LOAD_CONSTANT){
        tm_printf("LOAD_CONSTANT @\n", constants[val]);
    }else if( code == LOAD_GLOBAL){
        tm_printf("LOAD_GLOBAL @\n", constants[val]);
    }else if( code == STORE_GLOBAL){
        tm_printf("STORE_GLOBAL @\n", constants[val]);
    }else if( code == JUMP){
        printf("JUMP %d\n", val);
    }else if( code == UP_JUMP ){
        printf("UP_JUMP %d\n", val);
    }else{
        printf("%d,%d\n", code, val);
    }
}

tm_obj tm_eval( tm_obj fnc, tm_obj params ){
    tm_frame* f = build_frame(fnc);
    tm_obj* locals = f->locals;
    tm_obj* top = f->stack;
    tm_obj* constants = get_fnc_constants_nodes(fnc);
    tm_obj globals = get_fnc_globals(fnc);
    unsigned char* s = f->code;
    unsigned char** tags = f->tags;
    // f->top = top;
  
    top[0] = obj_none;
  
    tm_obj x, k, v;
    tm_obj ret = obj_none;
  
    int i, ins;
  

while(1){
  ins = s[0];
  i = (s[1] << 8) + s[2];
  // printInst(ins, i, constants);
  switch( ins ) {

  case NEW_NUMBER: {
    double d = atof(s+3);
    s+=i;
    v = tm_number(d);
    constants = define_constant( get_fnc_mod(fnc), v);
    tm_log1("new", "NEW_NUMBER %f", d);
    break;
  }

  case NEW_STRING: {
    v = str_new( s+3, i);
    s+=i;
    constants = define_constant(get_fnc_mod(fnc), v);
    tm_log2("new", "NEW_STRING [%d] @", i, v);
    break;
  }

  case LOAD_CONSTANT: {
    TM_PUSH( constants[ i ] );
    tm_log2("ins", "LOAD_CONSTANT [%d] @", i, constants[i]);
    break;
  }

  case LOAD_LOCAL: {
    tm_log1("ins", "LOAD_LOCAL %d", i);
    TM_PUSH( locals[i] );
    break;
  }

  case STORE_LOCAL:
    locals[i] = TM_POP();
    tm_log1("ins", "STORE_LOCAL %d", i);
    break;

  case LOAD_GLOBAL: {
    tm_log2("ins", "LOAD_GLOBAL [%d] @", (i), constants[i]);
    k = constants[ i ];
    if( dict_iget( get_dict(tm->builtins), k, &v)){
    }else{
      v = tm_get(globals, k);
    }
    TM_PUSH( v );
    break;
  }

  case STORE_GLOBAL:{
    k = constants[ i ];
    x = TM_POP();
    tm_set( globals, k, x );
    tm_log2("ins", "STORE_GLOBAL [%d] @", i, k);
    break;
  }

  case LIST:{
    tm_log1("ins", "LIST %d", i);
    TM_PUSH( list_new(i) );
    break;
  }

  case LIST_APPEND:
    tm_log0("ins", "LIST_APPEND");
    v = TM_POP();
    x = TM_TOP();
    if( x.type != TM_LST){
      tm_raise("tm_eval: LIST_APPEND expect a list but see %t", x);
    }
    list_append(get_list(x), v);
    break;
  
  case DICT_SET:
    tm_log0("ins", "DICT_SET");
    v = TM_POP();
    k = TM_POP();
    x = TM_TOP();
    if( x.type != TM_DCT){
        tm_raise("tm_eval(): DICT_SET expect a dict but see %t", x);
    }
    tm_set(x, k, v);
    break;

  case DICT:{
    tm_log1("ins", "DICT %d", i);
    tm_obj dict;
    LOAD_DICT( dict, i);
    TM_PUSH( dict );
    break;
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
  TM_OP( AND, tm_and);
  TM_OP( OR, tm_or);
  
  TM_OP2( LT_JUMP_ON_FALSE, tm_bool_lt );
  TM_OP2( GT_JUMP_ON_FALSE, tm_bool_gt );
  TM_OP2( LTEQ_JUMP_ON_FALSE, tm_bool_lteq);
  TM_OP2( GTEQ_JUMP_ON_FALSE, tm_bool_gteq);
  TM_OP2( EQEQ_JUMP_ON_FALSE, tm_bool_eqeq);
  TM_OP2( NOTEQ_JUMP_ON_FALSE, tm_bool_noteq);

  
  case SET:
    k = TM_POP();
    x = TM_POP();
    v = TM_POP();
    tm_set(x, k, v);
    tm_log0("ins", "SET");
    break;

  case POP:{
    TM_POP();
    tm_log0("ins", "POP");
    break;
  }

  case NOT:
    TM_TOP() = tm_not(TM_TOP());
    break;

  case NEG:
    TM_TOP() = tm_neg(TM_TOP());
    break;
    
  case CALL: {
    list_len(f->params) = 0;
    LOAD_LIST2(f->params, i);
    tm_obj func = TM_POP();
    // f->top = top;
    TM_PUSH( _tm_call(func, f->params));
    break;
  }break;

  case LOAD_PARAMS:{
    tm_log1("ins2", "LOAD_PARAMS %l", params);
    for(i = 0; i < list_len(params); i++){
      locals[i] = list_nodes(params)[i];
    }
    break;
  }

  case TM_FOR:{
    // jmp = next_short(s);
    k = *top;
    x = *(top-1);
    tm_log1("ins", "TM_FOR %d", i);
    if( tm_iter( x, &k) ){
      get_num(*top) += 1;
      // cprintln( x );
      TM_PUSH( k );
      // cprintln(v);
      break;
    }else{
      //s += i * 3;
      s = tags[i];
      continue;
    }
    break;
  }

  case TM_DEF:{
    struct tm_def_st def = tm_def(get_fnc_mod(fnc), s+3);
    get_func(def.fnc)->name = constants[i];
    s+= def.len;
    tm_log1("ins", "TM_DEF %d", def.len);
    TM_PUSH(def.fnc);
    break;
  }

  case RETURN:{
    tm_log0("stack", "RETURN");
    ret = TM_POP();
    goto end;
  }

  case TAG:{
    tm_log1("info","TAG %d", i);
    // TODO store_tag(mod, i, s);
    break;
  }

  case TAGSIZE:{
    tm_log1("info", "TAGSIZE %d", i);
    break;
  }

  case POP_JUMP_ON_TRUE:{
    tm_log1("cond", "POP_JUMP_ON_TRUE %d", i);
    if( _tm_bool( TM_POP() )){
      s = tags[i];
      // s += i * 3;
      continue;
    }
    break;
  }
  

  case POP_JUMP_ON_FALSE:{
    tm_log1("cond", "POP_JUMP_ON_FALSE %d", i);
    if( !_tm_bool( TM_POP() )){
      s = tags[i];
      //s += i * 3;
      continue;
    }
    break;
  }

  case JUMP_ON_TRUE:{
    tm_log1("cond", "JUMP_ON_TRUE %d", i);
    if( _tm_bool( TM_TOP() )){
      // s += i * 3;
      s = tags[i];
      continue;
    }
    break;
  }

  case JUMP_ON_FALSE:{
    tm_log1("cond", "JUMP_ON_FALSE %d", i);
    if( !_tm_bool( TM_TOP() )){
      // s += i * 3;
      s = tags[i];
      continue;
    }
    break;
  }

  case JUMP:
    tm_log1("cond", "JUMP %d", i);
    // s += (i+1) * 3;
    s = tags[i];
    continue;

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
  
  case SETJUMP:{
    f->jmp = tags[i];
    break;
  }
  
  /*
  case UP_JUMP:{
    s-=i*3;
    continue;
  }*/

  default:
    tm_raise("BAD INSTRUCTION, %d\n  globals() = \n@", ins,get_fnc_globals(f->fnc));
  goto end;
}
  s+=3;
}

  end:
    tm_log1("stack", "leave function %o", fnc);
    if( top != f->stack ) {
        printf("func_name = %s, count = %d\n", get_fnc_name(f->fnc) , (int)( top - f->stack));
    }
    if( tm->allocated_mem > tm->gc_limit){
        list_len(f->new_objs) = 0;
        gc_full(ret);
        tm->gc_limit = tm->allocated_mem * 2;
    }
    tm->cur--;

    return ret;
}
