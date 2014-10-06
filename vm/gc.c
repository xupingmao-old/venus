
#include "tm.h"

#define GC_CONSTANS_LEN 10

void gc_init( ){
	tm->allocated_mem = 0;

	int init_size = 100;
	tm->root = _list_new( init_size );
	tm->black = _list_new(init_size);
	tm->white = _list_new(init_size);
	tm->all = _list_new(init_size);
	// tm->strings = map_new_();
	// tm->strings = _map_new();
}

/*inline
tm_obj gc_do_string(tm_vm* tm, tm_obj k){
	int len = str_len(k);
	tm_dict* dict = tm->strings;
	if( len <= GC_CONSTANS_LEN ){
		int p = dict_find(tm, dict, k);
		if( p != -1 ){
			obj_free(tm, k);
			return dict->values->nodes[p];
		}else{
			list_append( tm, dict->keys, k);
			list_append( tm, dict->values, tm->none);
		}
	}
	list_append(tm, tm->all, k);
	return k;
}
*/
tm_obj gc_track( tm_obj v){
	switch( v.type ){
	case TM_NUM:
		return v;
	case TM_STR:
		v.value.str->marked = 1;
		break;
	case TM_LST:
		v.value.list->marked = 1;
		break;
/*	case TM_DCT:
		v.value.dict->marked = 1;
		break;*/
	case TM_MAP:
		v.value.map->marked = 1;
		break;
	}
	list_append(tm->all, v);
	return v;
}

void gc_mark(tm_obj v){
	switch(v.type){
		case TM_NUM:return;
		case TM_STR:{
			v.value.str->marked = 1;
			break;
		}
		case TM_LST:{
			tm_list* list;
			int n = list->len;
			int i;for(i = 0; i < n; i++){
				gc_mark(list->nodes[i]);
			}
			break;
		case TM_MAP:{
			tm_obj k,v;
			while( map_inext(v.value.map,&k, &v)){
				gc_mark(k);
				gc_mark(v);
			}
			break;
		}
		}
	}
}

void gc_full(){
	// mark all object 0
	int n,i;
	n = tm->all->len;
	tm_obj* nodes = tm->all->nodes;
	for(i = 0; i < n; i++){
		nodes[i].value.marked = 0;
	}
	tm_obj root = obj_new(TM_LST, tm->root);
	gc_mark(root);
}

void gc_info(){
	puts("===================vm infomation===============");
	printf("objsize=%d\n", sizeof( tm_obj ));

	puts("===================gc infomation===============");
	printf("\nallocated_mem: %d\n", tm->allocated_mem);
	printf("total object num: %d\n", tm->all->len);
}

void gc_free(){

#if DEBUG_GC
	puts("===================vm infomation===============");
	printf("objsize=%d\n", sizeof( tm_obj ));

	puts("===================gc infomation===============");
	printf("\nallocated_mem: %d\n", tm->allocated_mem);
	printf("total object num: %d\n", tm->all->len);

	tm_obj b;
	b.type = TM_LST;
	b.value.list = tm->all;
	cprint(b);
#endif
	tm_list* all = tm->all;
	int i;
	for(i = 0; i < all->len; i++){
		obj_free(all->nodes[i]);
	}

	list_free( tm->root);
	list_free( tm->all);
	list_free( tm->black );
	list_free( tm->white);
	// map_free( tm->strings);

#if DEBUG_GC
	printf("\nafter gc , allocated_mem: %d\n", tm->allocated_mem);
#endif
}
