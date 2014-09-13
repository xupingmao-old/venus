#include "tm.h"

tm_func* _func_new(tm_vm* tm){
#if DEBUG_GC
	printf("func_new\n");
#endif
	tm_func* func = tm_alloc(tm, sizeof(tm_func));
	return func;
}

tm_obj native_func_new(tm_vm* tm, tm_obj (*native_func)(tm_vm*, tm_obj) ){
	tm_obj func;
	func.type = TM_NATIVE_FNC;
	tm_func* f= tm_alloc(tm, sizeof(tm_func));
	f->native_func = native_func;
	f->self = tm->none;
	f->globals = tm->none;
	func.value.func = f;
	return gc_track(tm, func);
}

void func_free(tm_vm* tm, tm_func* func){
	obj_free(tm, func->self);
	obj_free(tm, func->globals);
#if DEBUG_GC
	printf("free function , free %d B\n", sizeof(tm_func));
#endif
	free(func);
	tm->allocated_mem -= sizeof(tm_func);
}

tm_obj method_new(tm_vm* tm, tm_obj self){
	tm_obj func;
	func.type = TM_METHOD;
	tm_func* f = tm_alloc(tm, sizeof(tm_func));
	f->self = self;
	f->native_func = NULL;
	f->globals = tm->none;
	func.value.func = f;
	return gc_track(tm, func);
}

tm_obj native_method_new(tm_vm* tm, tm_obj (*native_func)(tm_vm*, tm_obj) ){
	tm_obj func;
	func.type = TM_NATIVE_METHOD;
	tm_func* f = tm_alloc(tm, sizeof(tm_func));
	f->self = tm->none;
	f->native_func = native_func;
	f->globals = tm->none;
	func.value.func = f;
	return gc_track(tm, func);
}



