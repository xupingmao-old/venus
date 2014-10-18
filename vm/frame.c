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
tm_obj* push_constant(tm_module *mod, tm_obj v){
  tm_obj cs = mod->constants;
  int i = list_index( get_list(cs), v);
  if( i == -1 ){
    list_append(get_list(cs), v);
  }
  return get_list(cs)->nodes;
}

tm_obj* get_constants(tm_module *mod){
  tm_obj constants = mod->constants;
  tm_obj v;
  if( constants.type == TM_NON){
    v = list_new(20);
    list_append( get_list(v), tm->none);
    mod->constants = v;
  }
  return get_list(v)->nodes;
}


#define read_number( v, s) v = *(double*)s; s+=sizeof(double);
#define TM_PUSH( x ) *(++top) = x
#define TM_POP() *(top--)
#define TM_TOP() *top

#define CASE( code, body ) case code :  body ; break;

tm_obj tm_def(tm_module* mod, char* s){
  int len = code_check( mod, s, 1);
  tm_obj _code = str_new(s , len);
 return func_new(mod, _code, tm->none, NULL);
}

#if PRINT_INS
#define TM_OP( OP_CODE, OP_STR, OP_FUNC ) case OP_CODE: x = TM_POP();\
  v = TM_TOP();\
  puts(OP_STR);  \
  TM_TOP() = OP_FUNC(v, x);\
  goto start;
#else
#define TM_OP( OP_CODE, OP_STR, OP_FUNC ) case OP_CODE: x = TM_POP();\
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
  s = f->last_code;   

#define save_frame()    f->last_pc = top;   \
  f->last_code = s;


#define new_frame()   tm->cur++;\
  f = tm->frames + tm->cur;\
  top = f->stack;                   \
  locals = f->locals;


tm_obj tm_eval( tm_module* mod ){
  tm_obj globals = mod->globals;
  tm_obj code = mod->code;
  unsigned char* s = get_str(code);
  // constants will be built in modules.
  // get constants from function object.
  tm_frame* f = tm->frames + tm->cur;
  f->file = mod->file;
  tm_obj* locals = f->locals;
  register tm_obj* top = f->stack;

  tm_obj* constants = get_constants(mod);
  tm_obj x, k, v;
  tm_obj params, func;
  tm_func* _func;
  tm_obj ret = tm->none;
  int i;

 start:
  switch( next_byte(s) ) {

  case NEW_NUMBER: {
    double d;
    read_number( d, s);
    v = tm_number(d);
    constants = push_constant( mod , v);
#if PRINT_INS
    printf("NEW_NUMBER %g\n", d);
#endif
    goto start;
  }

  case NEW_STRING: {
    int len = next_short( s );
    v = str_new( s, len);
    s+=len;
    constants = push_constant( mod, v);
#if PRINT_INS
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
    tm_printf("LOAD_CONSTANT [@] @\n",number_new(i), constants[i]);
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
    k = constants[ i ];
    if( dict_iget( get_dict(tm->builtins), k, &v)){
      // already in v;
    }else{
      v = tm_get(globals, k);
    }
    TM_PUSH( v );
#if PRINT_INS
    tm_printf("LOAD_GLOBAL [@] @ = @\n", number_new(i), k, v);
#endif
    goto start;
  }

  case STORE_GLOBAL:{
    i = next_short( s );
    k = constants[ i ];
    x = TM_POP();
#if PRINT_INS
    tm_printf("STORE_GLOBAL [@] @\n", number_new(i), k);
#endif
    tm_set( globals, k, x );
    goto start;
  }

  case LIST:{
    i = next_byte(s);
    #if PRINT_INS
        printf("LIST %d\n", i);
    #endif
    LOAD_LIST( params, i );
    TM_PUSH( params );
    goto start;
  }

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

  TM_OP( ADD, "ADD", tm_add);
  TM_OP( SUB, "SUB", tm_sub);
  TM_OP( MUL, "MUL", tm_mul);
  TM_OP( DIV, "DIV", tm_div);
  TM_OP( MOD, "MOD", tm_mod);
  TM_OP( GET, "GET", tm_get);
  
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
    
  case CALL: {
    i = next_byte( s );
#if PRINT_INS
    printf("CALL %d\n", i);
#endif
    LOAD_LIST(params, i);
    func = TM_POP();
    goto call_func;
    func_ret:
    TM_PUSH(ret);
    goto start;
  }break;

  case LOAD_PARAMS:{
#if PRINT_INS
    tm_printf("LOAD_PARAMS @\n", params);
#endif
    int len = list_len(params);
    for(i = 0; i < len; i++){
      locals[i] = get_list(params)->nodes[i];
    }
    goto start;
  }

  case TM_DEF:{
    func = tm_def(mod, s);
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
    restore_frame();
    goto func_ret;
  }

  case TAG:{
    i = next_short(s);
#if PRINT_INS
    printf("TAG %d\n", i);
#endif
    // TODO store_tag(mod, i, s);
    goto func_ret;
  }

  case POP_JUMP_ON_TRUE:{
    i = next_short(s);
#if PRINT_INS
    printf("POP_JUMP_ON_TRUE %d\n", i);
#endif
    if( tm_bool( TM_POP() )){
      s = mod->tags[i];
    }
    goto start;
  }

  case POP_JUMP_ON_FALSE:{
    i = next_short(s);
#if PRINT_INS
    printf("POP_JUMP_ON_FALSE %d\n", i);
#endif
    if( !tm_bool( TM_POP() )){
      s = mod->tags[i];
    }
    goto start;
  }

  case TM_EOF:{
#if PRINT_INS
    puts("TM_EOF");
#endif
    goto start;
  }

  case TM_EOP:{
#if PRINT_INS
    puts("TM_EOP");
#endif
    goto end;
  }

  default:
#if PRINT_INS
  puts("BAD INSTRUCTION");
#endif
  goto end;
}
  // cprintln(mod);

 call_func:

  _func = get_func(func);
  if( _func->self.type != TM_NON){
    list_insert(get_list(params),0, _func->self);
  }
  if( _func->native_func != NULL ){
    ret = _func->native_func(params);
    goto func_ret;
  }
  // user function
  // save current state
  save_frame();
  // new frame
  new_frame();
  s = get_str(get_func(func)->code);
  goto func_ret;

  end:

  tm->cur = 0;
  return ret;
}
