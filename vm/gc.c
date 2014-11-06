
#include "tm.h"

#define GC_CONSTANS_LEN 10
#define GC_REACHED_SIGN 1
#define GC_MARKED(o) (o).value.gc->marked

void gc_init( ){
	tm->allocated_mem = 0;
	tm->gc_limit = 1024 * 8; // set 8k to see gc process

	int init_size = 100;
	tm->all = _list_new(init_size);

	// tm->black = _list_new(init_size);
	// tm->white = _list_new(init_size);
	// tm->strings = dict_new_();
	// tm->strings = _dict_new();
}
tm_obj gc_track( tm_obj v){
	switch( v.type ){
	case TM_NUM:
	case TM_NON:
		return v;
	case TM_STR:
		v.value.str->marked = GC_REACHED_SIGN;
		break;
	case TM_LST:
		get_list(v)->marked = GC_REACHED_SIGN;
		break;
	case TM_DCT:
		get_dict(v)->marked = GC_REACHED_SIGN;
		break;
	case TM_MOD:
		get_mod(v)->marked = GC_REACHED_SIGN;
		break;
    case TM_FNC:
        get_func(v)->marked = GC_REACHED_SIGN;
        break;
    default:
        tm_raise("gc_track(), not supported type %d", v.type);
        return v;
	}
	list_append(tm->all, v);
    
    if(tm->cur >= 0 ){
        list_append( get_list(tm->frames[tm->cur].new_objs), v );
    }
	return v;
}
/*
*/

void gc_mark(tm_obj o){
	if( o.type == TM_NUM || o.type == TM_NON)
        return;
//    tm_log("mark", "mark object @", _tm_type(o));
    /*
	if( GC_REACHED_SIGN == o.value.gc->marked )
		return;*/
    // tm_printf_only_type("mark object @\n", o);
	switch(o.type){
		case TM_STR:{
            if( o.value.str->marked ) return;
			o.value.str->marked = GC_REACHED_SIGN;
			break;
		}
		case TM_LST:{
                if( get_list(o)->marked ) return;
				get_list(o)->marked = GC_REACHED_SIGN;
				tm_list* list = get_list(o);
				int i;for(i = 0; i < list->len; i++){
					gc_mark(list->nodes[i]);
				}
			}
			break;
		case TM_DCT:{
            if( get_dict(o)->marked ) return;
			get_dict(o)->marked = GC_REACHED_SIGN;
			tm_obj k,v;
			dict_iter_init(get_dict(o));
			while( dict_inext(get_dict(o),&k, &v)){
				gc_mark(k);
				gc_mark(v);
			}
			break;
		}
		case TM_FNC:
            if( get_func(o)->marked ) return;
			get_func(o)->marked = GC_REACHED_SIGN;
			// gc_mark(get_func(o)->code);
			gc_mark(get_func(o)->mod);
			gc_mark(get_func(o)->self);
			gc_mark(get_func(o)->name);
			break;
		case TM_MOD:
            if( get_mod(o)->marked ) return;
			get_mod(o)->marked = GC_REACHED_SIGN;
			gc_mark(get_mod(o)->code);
			gc_mark(get_mod(o)->name);
			gc_mark(get_mod(o)->file);
			gc_mark(get_mod(o)->constants);
			gc_mark(get_mod(o)->globals);
			break;
         default: tm_raise("gc_mark(), unknown object type %d", o.type);
	}
}

void gc_mark_frames(){
    int i;
	for(i = 0; i < FRAMES_COUNT ; i++){
		tm_frame* f = tm->frames+i;
        tm_log("mark", "mark frame %d", i);
		gc_mark(f->new_objs);
        gc_mark(f->globals);
        gc_mark(f->constants);
	}
}

void gc_clean(){
	int n,i;
	// tm->black->len = 0;
    
    tm_list* temp = _list_new( 200 );
    tm_list* all = tm->all;
	for(i = 0; i < all->len; i++){
        if ( GC_MARKED(tm->all->nodes[i]) ){
            list_append( temp, all->nodes[i]);
        }else{
            // tm_printf("free @\n", _tm_type(all->nodes[i]));
            obj_free(all->nodes[i]);
            // nodes[i].value.gc = NULL;
        }
	}
    list_free( tm->all );
    tm->all = temp;
}

#define MARK(v) \
    switch( v.type ){  \
    case TM_STR: \
        v.value.str->marked = 0;\
        break;\
    case TM_LST:\
        get_list(v)->marked = 0;\
        break;\
    case TM_DCT:\
        get_dict(v)->marked = 0;\
        break;\
    case TM_MOD:\
        get_mod(v)->marked = 0;\
        break;\
    case TM_FNC:\
        get_func(v)->marked = 0;\
        break;\
    }

/**
* mark and sweep garbage collection
* 
* TODO maybe we can mark the object with different value to 
* recognize the gc type of the object.
*/
void gc_full(tm_obj ret){
    int i;
	long t1,t2;
	t1 = clock();
	// mark all object except new , to 0
	for(i = 0; i < tm->all->len; i++){
        /*
		if( 1 == nodes[i].value.gc->marked){
			// nodes[i].value.gc->marked = -1;
		}else{
			nodes[i].value.gc->marked = 0;
		}*/
        GC_MARKED(tm->all->nodes[i]) = 0;
        // MARK(nodes[i]);

	}
#if LIGHT_DEBUG_GC
	int old = tm->allocated_mem, _new;
#endif

    tm_log0("gc", "full gc start ...");

    tm_log("gc", "mark ret %t", ret);
    gc_mark(ret);

    tm_log0("gc", "mark root");
	gc_mark(tm->root);

	gc_mark_frames();
	gc_clean();
	t2 = clock();
#if LIGHT_DEBUG_GC
	_new = tm->allocated_mem;
    tm_log("gc", "full gc , elapsed time %ld, %d = > %d , freed %d B\n",t2-t1, old, _new, old- _new );
#endif
}

void gc_info(){
	puts("===================vm infomation===============");
	printf("objsize=%d\n",(int) sizeof( tm_obj ));

	puts("===================gc infomation===============");
	printf("\nallocated_mem: %d\n", tm->allocated_mem);
	printf("total object num: %d\n", tm->all->len);
}

void gc_free(){
	tm_list* all = tm->all;
	int i;
	for(i = 0; i < all->len; i++){
		obj_free(all->nodes[i]);
	}

	list_free( tm->all);
	// list_free( tm->black );
	// list_free( tm->white);
	// dict_free( tm->strings);

#if DEBUG_GC
	printf("\nafter gc , allocated_mem: %d\n", tm->allocated_mem);
#endif
#if !PRODUCT
    if( tm->allocated_mem != 0 ){
        printf("\n**memory leak happens***\n");
    }
#endif
    // close_log();
}
