
#ifndef args_h
#define args_h
#include "tm.h"

tm_obj tm_arg1(tm_vm* tm, tm_obj v1);
tm_obj tm_arg2(tm_vm* tm, tm_obj v1, tm_obj v2);


inline
tm_obj get_arg(tm_vm* tm, tm_obj params, int pos, int type){
	tm_list* list = params.value.list;
	int n = list->len;
	if ( pos >= n ){
		tm->error = string_new(tm, "get_arg: index overflow");
		tm_raise(tm);
	}
	tm_obj v = list->nodes[pos];
	if( v.type != type){
		tm->error = string_new(tm, "get_arg: TypeError");
		tm_raise(tm);
	}
	return v;
}

inline
tm_obj get_arg_(tm_vm* tm, tm_obj params, int pos){
	tm_list* list = params.value.list;
	int n = list->len;
	if ( pos >= n ){
		tm->error = string_new(tm, "get_arg: index overflow");
		tm_raise(tm);
	}
	return list->nodes[pos];
}


#endif
