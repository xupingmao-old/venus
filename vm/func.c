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

void func_free( tm_func* func){
  // the references will be tracked by gc collecter
#if DEBUG_GC
  printf("free function , free %d B\n", sizeof(tm_func));
#endif
  tm_free(func, sizeof(tm_func));
}


