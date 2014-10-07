
#ifndef args_h
#define args_h
#include "tm.h"


inline
tm_obj get_arg(tm_obj params, int pos, int type){
	tm_list* list = params.value.list;
	int n = list->len;
	if ( pos >= n ){
		tm_raise("get_arg: index overflow");
	}
	tm_obj v = list->nodes[pos];
	if ( type < 0 ){
		return v;
	}
	if( v.type != type){
		tm_raise("get_arg: TypeError");
	}
	return v;
}

tm_obj tm_args( int n, ...){
	tm_obj params = list_new(n);
	va_list a; va_start(a,n);
	int i;
	for (i=0; i<n; i++) {
		list_append(get_list(params),va_arg(a,tm_obj));
	}
	va_end(a);
	return params;
}


#endif
