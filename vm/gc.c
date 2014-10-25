
#include "tm.h"

#define GC_CONSTANS_LEN 10

void gc_init( ){
	tm->allocated_mem = 0;

	int init_size = 100;
	tm->root = _list_new( init_size );
	tm->black = _list_new(init_size);
	tm->white = _list_new(init_size);
	tm->all = _list_new(init_size);
	// tm->strings = dict_new_();
	// tm->strings = _dict_new();
}
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
	case TM_DCT:
		v.value.dict->marked = 1;
		break;
	case TM_MOD:
		v.value.mod->marked = 1;
		break;
	}
	list_append(tm->all, v);
	return v;
}

void gc_mark(tm_obj o){
	if( o.value.marked )
		return;
	switch(o.type){
		case TM_NUM:return;
		case TM_STR:{
			o.value.str->marked = 1;
			break;
		}
		case TM_LST:{
				get_list(o)->marked = 1;
				tm_list* list = get_list(o);
				int n = list->len;
				int i;for(i = 0; i < n; i++){
					gc_mark(list->nodes[i]);
				}
			}
			break;
		case TM_DCT:{
			get_dict(o)->marked = 1;
			tm_obj k,v;
			dict_iter_init(get_dict(o));
			while( dict_inext(get_dict(o),&k, &v)){
				gc_mark(k);
				gc_mark(v);
			}
			break;
		}
		case TM_FNC:
			get_func(o)->marked = 1;
			gc_mark(get_func(o)->code);
			gc_mark(get_func(o)->mod);
			gc_mark(get_func(o)->self);
			break;
		case TM_MOD:
			gc_mark(get_mod(o)->code);
			gc_mark(get_mod(o)->name);
			gc_mark(get_mod(o)->file);
			gc_mark(get_mod(o)->constants);
			gc_mark(get_mod(o)->globals);
			break;
	}
}

void gc_full(){
	int n,i;
	// mark vm core
	gc_mark(tm->builtins);
	gc_mark(tm->modules);
	for(i = tm->cur; i >= 0; i--){
		tm_frame* f = tm->frames + i;
		int maxlocals = f->maxlocals;
		int j;for(j = 0; j < maxlocals; j++){
			gc_mark(f->locals[j]);
		}
	}

	// mark all object 0
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
	printf("objsize=%d\n",(int) sizeof( tm_obj ));

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

	// tm_obj b = obj_new(TM_LST, tm->all);
	// cprint(b);
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
	// dict_free( tm->strings);

#if DEBUG_GC
	printf("\nafter gc , allocated_mem: %d\n", tm->allocated_mem);
#endif
}
