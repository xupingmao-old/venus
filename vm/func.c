#include "tm.h"


tm_obj func_new(tm_vm* tm, tm_obj (*native_func)( tm_obj) ){
	tm_obj func;
	func.type = TM_FNC;
	tm_func* f= tm_alloc(sizeof(tm_func));
	if( native_func != NULL ){
		f->native_func = native_func;
		f->fnc_type = TM_NATIVE;
	}else{
		f->native_func = NULL;
		f->fnc_type = TM_FNC;
	}
	f->self = tm->none;
	f->globals = tm->none;
	func.value.func = f;
	return gc_track(func);
}

void func_free( tm_func* func){
	obj_free( func->self);
	obj_free( func->globals);
#if DEBUG_GC
	printf("free function , free %d B\n", sizeof(tm_func));
#endif
	tm_free(func, sizeof(tm_func));
}


