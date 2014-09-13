
#include "tm.h"

tm_obj tm_arg1(tm_vm* tm, tm_obj v1){
	tm_obj list = list_new_n(tm, 1);
	list_push(tm, list.value.list, v1);
	return list;
}

tm_obj tm_arg2(tm_vm* tm, tm_obj v1, tm_obj v2){
	tm_obj list = list_new_n(tm, 2);
	list_push(tm, list.value.list, v1);
	list_push(tm, list.value.list, v2);
	return list;
}

void tm_nargs( tm_vm* tm, int n, ...){
	list_clear(tm, tm->params);
	va_list a; va_start(a,n);
	int i;
	for (i=0; i<n; i++) {
		list_push(tm, tm->params,va_arg(a,tm_obj));
	}
	va_end(a);
}
