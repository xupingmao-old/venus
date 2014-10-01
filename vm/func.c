#include "tm.h"


tm_obj func_new(unsigned char* code, int len, tm_obj self, tm_obj globals, tm_obj (*native_func)( tm_obj) ){
	tm_obj func;
	func.type = TM_FNC;
	tm_func* f= tm_alloc(sizeof(tm_func));
	if( code != NULL ){
		f->code = string_new( code, len);
	}else{
		f->code = tm->none;
	}
	if( native_func != NULL ){
		f->native_func = native_func;
		f->fnc_type = TM_NATIVE;
	}else{
		f->native_func = NULL;
		f->fnc_type = TM_FNC;
	}
	f->self = self;
	f->globals = globals;
	func.value.func = f;
	return gc_track(func);
}

void func_free( tm_func* func){
	obj_free( func->self);
	obj_free( func->globals);
	obj_free( func->code);
#if DEBUG_GC
	printf("free function , free %d B\n", sizeof(tm_func));
#endif
	tm_free(func, sizeof(tm_func));
}


