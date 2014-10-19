#include "tm.h"


tm_obj func_new(tm_obj mod,
		tm_obj code,
		tm_obj self, 
		tm_obj (*native_func)( tm_obj) ){
  tm_obj func;
  func.type = TM_FNC;
  tm_func* f= tm_alloc(sizeof(tm_func));
  f->mod = mod;
  f->code = code;
  f->pc = NULL;
  f->native_func = native_func;
  /*	if( native_func != NULL ){
	f->native_func = native_func;
	// check if the func is native by f->native_func == NULL?
	// f->fnc_type = TM_NATIVE;
	}else{
	f->native_func = NULL;
	// f->fnc_type = TM_FNC;
	}*/
  f->self = self;
  func.value.func = f;
  return gc_track(func);
}

tm_obj method_new(tm_obj _fnc, tm_obj self){
  tm_func* fnc = get_func(_fnc);
  return func_new( fnc->mod, fnc->code, self, fnc->native_func);
}

tm_obj class_new( tm_obj clazz ){
  tm_dict* cl = get_dict(clazz);
  tm_obj k,v;
  cl->cur = 0;
  tm_obj instance = dict_new();
  while( dict_inext( cl, &k, &v)){
    if( v.type == TM_FNC){
      tm_set( instance, k, method_new(v, instance));
    }
  }
  return instance;
}

void func_free( tm_func* func){
  // the references will be tracked by gc collecter
#if DEBUG_GC
  printf("free function , free %d B\n", sizeof(tm_func));
#endif
  tm_free(func, sizeof(tm_func));
}

tm_obj module_new( tm_obj file , tm_obj name, tm_obj code){
  tm_module *mod = tm_alloc( sizeof(tm_module));
  mod->file = file;
  mod->name = name;
  mod->code = code;
  mod->tags = NULL;
  mod->tagsize = 0;
  mod->checked = 0;
  mod->constants = tm->none;
  mod->globals = dict_new();
  tm_obj obj;
  obj.type = TM_MOD;
  obj.value.mod = mod;
  return gc_track( obj );
}

void module_free( tm_module* mod ){
/*  obj_free( mod->file );
  obj_free( mod->name );
  obj_free( mod->code );
  obj_free( mod->constants );
  obj_free( mod->globals );*/
#if DEBUG_GC
  printf("free module , free %d B\n", sizeof(tm_module) );
#endif
  if( mod->tags != NULL ){
    tm_free(mod->tags, mod->tagsize * sizeof(char*) );
  }
  tm_free( mod, sizeof( tm_module ));
}


