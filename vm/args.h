
#ifndef args_h
#define args_h
#include "tm.h"


tm_obj tm_args( int n, ...);

inline
tm_obj get_arg(tm_obj params, int pos, int type){
	tm_list* list = params.value.list;
	int n = list->len;
	if ( pos >= n ){
		tm_raise("get_arg: index overflow");
	}
	tm_obj v = list->nodes[pos];
	if ( type == -1 ){
		return v;
	}
	if( v.type != type){
		tm_raise("get_arg: TypeError");
	}
	return v;
}

#endif
