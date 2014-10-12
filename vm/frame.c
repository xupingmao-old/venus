/**

   date : 2014-9-2
**/

#include "tm.h"
#include "instruction.h"

tm_frame* frame_new( tm_func* func){
  tm->cur++;
  tm_frame* f = tm->frames + tm->cur;
  f->code = func->code;
  //f->mod = func->mod;
  return f;
}

// 一个模块的常量
tm_obj* push_constant(tm_obj mod, tm_obj v){
  tm_obj constants = tm_get( mod, string_new("__constants__", 0));
  int i = list_index( get_list(constants), v);
  if( i == -1 ){
    list_append(get_list(constants), v);
  }
  return get_list(constants)->nodes;
}

tm_obj* get_constants(tm_obj mod){
  tm_obj __constants__ = string_new("__constants__", 0);
  tm_map* m = get_map(mod);
  tm_obj v;
  if( map_iget(m, mod, &v) ){
  }else{
    v = list_new(20);
    list_append( get_list(v), tm->none);
    tm_set(mod, __constants__, v);
  }
  return get_list(v)->nodes;
}


void print_ins(int ins, tm_obj v){
  if( ins < 0) return;
  switch(ins){
  case NEW_STRING: tm_printf("NEW_STRING \"@\"\n", v);break;
  case NEW_NUMBER: tm_printf("NEW_NUMBER @\n", v);break;
  case LOAD_CONSTANT: tm_printf("LOAD_CONSTANT @\n", v);break;
  case LOAD_GLOBAL: tm_printf("LOAD_GLOBAL @\n", v);break;
  case STORE_GLOBAL: tm_printf("STORE_GLOBAL @\n", v);break;
  case TM_EOP: tm_printf("EOP\n");break;
  }
}


#define next_char( s ) *s++
#define next_byte( s ) *s++
#define next_short( s ) ((int) (*s++) << 8) + (int) (*s++)
#define read_number( v, s) v = *(double*)s; s+=sizeof(double);
#define TM_PUSH( x ) *(++top) = x
#define TM_POP() *(top--)
#define TM_TOP() *top

#define CASE( code, body ) case code :  body ; break;

#define PRINT_INS 1

tm_obj get_func_code(char* s){
  int len = 0;
  char* code = s;
  while(1){
    switch(next_char(s)){
    case ADD:
    case SUB:
    case MUL:
    case DIV:
    case MOD:
    case LOAD_PARAMS:
    case GT:
    case LT:
    case GTEQ:
    case LTEQ:
    case EQEQ:
    case NOTEQ:
    case GET:
    case SET:
      len++;
      break;
    case STORE_LOCAL:
    case LOAD_LOCAL:
      next_char(s);
      len+=2;
      break;
    case LOAD_CONSTANT:
    case LOAD_GLOBAL:
    case STORE_GLOBAL:
      next_short(s);
      len+=3;
      break;
    case TM_EOF:
      len++;
      goto ret;
    }
  }
 ret:
  return string_new(code, len);
}


#define restore_frame   f = tm->frames + tm->cur;	\
  top = f->last_pc;					\
  s = f->last_code;

#define save_frame    f->last_pc = top;		\
  f->last_code = s;

tm_obj tm_eval( tm_obj mod ){
  tm_obj __code__ = string_new("__code__", 0);
  tm_obj g = mod;
  tm_obj code = tm_get(mod, __code__);
  char* s = get_str(code);
  // constants will be built in modules.
  // get constants from function object.
  tm_obj* constants = get_constants(mod);

  tm_frame* f = tm->frames + tm->cur;

  f->file = tm_get(mod, string_new("__file__", 0));
  tm_obj* locals = f->locals;
  register tm_obj* top = f->stack;

  tm_obj x, k, v;
  tm_obj params, func;
  tm_func* _func;
  tm_obj ret = tm->none;
  int i;

 start:
  switch( next_char(s) ){
  case NEW_NUMBER: {
    double d;
    read_number( d, s);
    v = tm_number(d);
    constants = push_constant( mod , v);
#if PRINT_INS
    printf("NEW_NUMBER %g\n", d);
#endif
  }goto start;
  case NEW_STRING: {
    int len = next_short( s );
    v = string_new( s, len);
    s+=len;
    constants = push_constant( mod, v);
#if PRINT_INS
    tm_printf("NEW_STRING \"@\"\n", v);
#endif
  }goto start;
  case LOAD_CONSTANT: {
    i = next_short( s );
    TM_PUSH( constants[ i ] );
#if PRINT_INS 
    tm_printf("LOAD_CONSTANT @\n", constants[i]);
#endif
    goto start;
  }
  case LOAD_LOCAL: {
    i = next_char(s);
    TM_PUSH( locals[i] );
    goto start;
  }
  case LOAD_GLOBAL: {
    i = next_short(s);
    k = constants[ i ];
    if( map_iget( get_map(tm->builtins), k, &v)){
      // already in v;
    }else{
      v = tm_get(mod, k);
    }
    TM_PUSH( v );
#if PRINT_INS
    tm_printf("LOAD_GLOBAL @\n", v);
#endif
    goto start;
  }
  case STORE_LOCAL:{
    i = next_char( s );
    locals[i] = TM_POP();
  }break;
  case STORE_GLOBAL:{
    i = next_short( s );
    k = constants[ i ];
    x = TM_POP();
    tm_set( g, k, x );
#if PRINT_INS
    tm_printf("STORE_GLOBAL @\n", k);
#endif
    goto start;
  }
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
  case CALL: {
    i = next_byte( s );
#if PRINT_INS
    printf("CALL %d\n", i);
#endif
    params = list_new(i);
    tm_list* _p = get_list(params);
    tm_obj* j;
    // if i == 1, just top is OK
    for(j = top - i + 1; j <= top; j++){
      list_append(_p, *j);
    }
    top-=i;
    func = TM_POP();
    goto call_func;
    func_ret:
    TM_PUSH(ret);
    goto start;
  }break;
  case ADD:{
#if PRINT_INS
    puts("ADD");
#endif
    x = TM_POP();
    v = TM_TOP();
    TM_TOP() = tm_add(x,v);
    goto start;
  }
  case GET:{
    x = TM_POP();
    k = TM_POP();
    v = tm_get(x, k);
    TM_PUSH(v);
    goto start;
  }
  case POP:{
    TM_POP();
#if PRINT_INS
    puts("POP");
#endif
    goto start;
  }
  case TM_DEF:{
    tm_obj code = get_func_code(s);
    s+= get_str_len(code);
#if PRINT_INS
    printf("TM_DEF %d\n",i);
#endif
    func = func_new(mod, code,tm->none, NULL);
    TM_PUSH(func);
    goto start;
  }
  case RETURN:{
#if PRINT_INS
    puts("RETURN");
#endif
    ret = TM_POP();
    tm->cur--;
    restore_frame;
    goto func_ret;
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
  save_frame;
  // new frame
