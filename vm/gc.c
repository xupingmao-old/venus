
#include "tm.h"

#define GC_CONSTANS_LEN 10

void gc_init( tm_vm* tm){
	tm->allocated_mem = 0;

	tm->root = list_new_(tm);
	tm->black = list_new_(tm);
	tm->white = list_new_(tm);
	tm->all = list_new_(tm);
	tm->strings = dict_new_(tm);
}

inline
tm_obj gc_do_string(tm_vm* tm, tm_obj k){
	int len = str_len(k);
	tm_dict* dict = tm->strings;
	if( len <= GC_CONSTANS_LEN ){
		int p = dict_find(tm, dict, k);
		if( p != -1 ){
			obj_free(tm, k);
			return dict->values->nodes[p];
		}else{
			list_push( tm, dict->keys, k);
			list_push( tm, dict->values, tm->none);
		}
	}
	list_push(tm, tm->all, k);
	return k;
}

tm_obj gc_track(tm_vm* tm, tm_obj v){
	switch( v.type ){
	case TM_NUM:
		return v;
	case TM_STR:
		return gc_do_string(tm, v);
	case TM_LST:
		v.value.list->marked = 1;
		break;
	case TM_DCT:
		v.value.dict->marked = 1;
		break;
	}
	list_push(tm, tm->all, v);
	return v;
}


void gc_free(tm_vm* tm){

#if DEBUG_GC
	puts("===================vm infomation===============");
	printf("objsize=%d\n", sizeof( tm_obj ));

	puts("===================gc infomation===============");
	printf("\nallocated_mem: %d\n", tm->allocated_mem);
	printf("total object num: %d\n", tm->all->len);

	tm_obj b;
	b.type = TM_LST;
	b.value.list = tm->all;
	_tm_print(b);
#endif
	tm_list* all = tm->all;
	int i;
	for(i = 0; i < all->len; i++){
		obj_free(tm, all->nodes[i]);
	}

	list_free( tm, tm->root);
	list_free( tm, tm->all);
	list_free( tm, tm->black );
	list_free( tm, tm->white);
	dict_free( tm, tm->strings);

#if DEBUG_GC
	printf("\nafter gc , allocated_mem: %d\n", tm->allocated_mem);
#endif
}
