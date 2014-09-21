
#include "tm.h"

#define __list_new( tm, size) \
	tm_list* list = tm_alloc(tm, sizeof(tm_list) ); \
	list->len = 0;\
	list->cap = size;\
	list->nodes = tm_alloc(tm, sizeof(tm_obj) * size);\
	list->cur = 0;

tm_list* list_new_(tm_vm* tm)
{
#if DEBUG_GC
	int old = tm->allocated_mem;
#endif
	__list_new(tm, 2);
#if DEBUG_GC
	printf("list_new: from %d to %d, allocated: %d\n", old, tm->allocated_mem,
			tm->allocated_mem - old);
#endif
	return list;
}

tm_obj list_new(tm_vm* tm){
	tm_list* list = list_new_(tm);
	tm_obj v;
	v.type = TM_LST;
	v.value.list = list;
	return gc_track(tm, v);
}

tm_obj list_new_n(tm_vm* tm, int cap){
	__list_new(tm, cap);
	tm_obj v;
	v.type = TM_LST;
	v.value.list = list;
	return gc_track(tm, v);
}

void list_free( tm_vm* tm, tm_list* list){
#if DEBUG_GC
	int old = tm->allocated_mem;
#endif
	tm_free(tm, list->nodes, list->cap * sizeof(tm_obj));
	tm_free(tm, list, sizeof(list));
#if DEBUG_GC
	printf("free list %x, from %d to %d, freed: %d\n", list,
			old, tm->allocated_mem,
			old - tm->allocated_mem);
#endif
}

void list_clear(tm_vm* tm, tm_list* list){
	list->len = 0;
}

tm_obj list_get(tm_vm* tm, tm_list* list, int n)
{
	if( n < 0 )
	 n += list->len;
	if( n >= list->len || n < 0)
	{
		tm->error = string_new(tm, "list_get: index overflow ");
		tm_raise( tm);
	}
	return list->nodes [n];
}

void list_set(tm_vm* tm, tm_list* list, int n, tm_obj val)
{
	if( n < 0 ){
		n += list->len;
	}
	if( n >= list->len || n < 0)
	{
		tm->error = string_new(tm, "list_set: index overflow");
		tm_raise(tm);
	}
	list->nodes[n] = val;
}

inline
void _list_check_cap(tm_vm*tm, tm_list* list){
	if( list->len >= list->cap )
	{
		int ocap = list->cap;
		list->cap = ocap * 3 / 2 + 1;
		list->nodes = tm_realloc( tm, list->nodes, sizeof(tm_obj) * ocap,
			sizeof(tm_obj) * list->cap);
#if DEBUG_GC
		printf("resize list: from %d to %d\n", OBJ_SIZE * ocap, OBJ_SIZE * list->cap);
#endif
	}
}

void list_push(tm_vm* tm, tm_list* list, tm_obj obj)
{
	_list_check_cap(tm, list);
	list->nodes[ list->len ] = obj;
	list->len++;
}

void list_insert(tm_vm* tm, tm_list* list, int n, tm_obj obj){
	_list_check_cap(tm, list);
	if( n < 0) n += list->len;
	if( n < list->len || n < 0){
		int i = list->len;
		for(; i > n; i -- ){
			list->nodes[i] = list->nodes[i-1];
		}
		list->nodes[n] = obj;
		list->len++;
	}else{
		list->nodes[list->len] = obj;
		list->len++;
	}
}


void list_push_str(tm_vm* tm, tm_list* list, char* v){
	list_push(tm, list, string_new(tm, v));
}

tm_obj list_pop(tm_vm* tm, tm_list* list)
{
	tm_obj obj = list_get(tm, list, -1 );
	list->len--;
	return obj;
}

tm_obj list_top( tm_vm* tm, tm_list* list){
	tm_obj v = list_get(tm, list, -1);
	return v;
}

void list_start( tm_vm* tm, tm_list* list){
	list->cur = 0;
}

void list_end( tm_vm* tm, tm_list* list){
	list->cur = 0;
}

tm_obj list_next(tm_vm* tm, tm_list* list){
	if( list->cur < list->len){
		tm_obj v = list_get(tm, list, list->cur);
		list->cur++;
		return v;
	}
	list->cur = 0;
	return tm->end;
}

tm_list* list_join(tm_vm* tm, tm_list* list1, tm_list *list2)
{
	tm_list* list = list_new_(tm);
	int len = list1->len + list2->len;
	int list1_nodes_size = list1->len * OBJ_SIZE;
	list->nodes = tm_alloc( tm, len * OBJ_SIZE);
	memcpy( list->nodes, list1->nodes, list1_nodes_size);
	memcpy( list->nodes + list1_nodes_size, list2->nodes, list2->len * OBJ_SIZE);
	return list;
}


tm_obj _list_push( tm_vm* tm, tm_obj params){
	tm_obj self = get_arg( tm,params, 0, TM_LST);
	tm_obj v = get_arg_(tm, params , 1);
	list_push( tm, get_list(self), v);
	return tm->none;
}

tm_obj _list_pop(tm_vm* tm,tm_obj params){
	tm_obj self = get_arg(tm, params, 0, TM_LST);
	return list_pop(tm, get_list(self));
}
tm_obj _list_insert(tm_vm* tm, tm_obj params){
	tm_obj self = get_arg(tm, params, 0, TM_LST);
	tm_obj idx = get_arg(tm, params, 1, TM_NUM);
	int n = idx.value.dv;
	tm_obj v = get_arg_(tm, params, 2);
	list_insert( tm, get_list(self),n, v );
	return self;
}

tm_obj _list_top(tm_vm* tm, tm_obj params){
	tm_obj self = get_arg(tm, params, 0, TM_LST);
	return list_top(tm, get_list(self));
}
