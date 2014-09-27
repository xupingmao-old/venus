
#include "tm.h"


tm_obj tm_args( int n, ...){
	tm_obj params = list_new(n);
	va_list a; va_start(a,n);
	int i;
	for (i=0; i<n; i++) {
		list_push(get_list(params),va_arg(a,tm_obj));
	}
	va_end(a);
	return params;
}
