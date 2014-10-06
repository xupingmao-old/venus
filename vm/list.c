
#include "tm.h"

tm_list* _list_new( int cap ){
	tm_list* list = tm_alloc( sizeof(tm_list));
	list->len = 0;
	list->cap = cap;
	list->nodes = tm_alloc( OBJ_SIZE * list->cap);
	list->cur = 0;
	return list;
}

tm_obj list_new(int cap){
	tm_obj v;
	v.type = TM_LST;
	v.value.list = _list_new(cap);
	return gc_track(v);
}


void list_free(tm_list* list){
#if DEBUG_GC
	int old = tm->allocated_mem;
#endif
	tm_free(list->nodes, list->cap * sizeof(tm_obj));
	tm_free(list, sizeof(tm_list));
#if DEBUG_GC
	printf("free list %x, from %d to %d, freed: %d\n", list,
			old, tm->allocated_mem,
			old - tm->allocated_mem);
#endif
}


tm_obj list_get(tm_list* list, int n)
{
	if( n < 0 )
	 n += list->len;
	if( n >= list->len || n < 0)
	{
		tm_raise("list_get: index overflow");
	}
	return list->nodes [n];
}

void list_set(tm_list* list, int n, tm_obj val)
{
	if( n < 0 ){
		n += list->len;
	}
	if( n >= list->len || n < 0)
	{
		tm_raise( "list_set: index overflow");
	}
	list->nodes[n] = val;
}

inline
void _list_check_cap( tm_list* list){
	if( list->len >= list->cap )
	{
		int ocap = list->cap;
		list->cap = ocap * 3 / 2 + 1;
		list->nodes = tm_realloc( list->nodes, sizeof(tm_obj) * ocap,
			sizeof(tm_obj) * list->cap);
#if DEBUG_GC
		printf("resize list: from %d to %d\n", OBJ_SIZE * ocap, OBJ_SIZE * list->cap);
#endif
	}
}

void list_append(tm_list* list, tm_obj obj)
{
	_list_check_cap( list);
	list->nodes[ list->len ] = obj;
	list->len++;
}

/**
	insert 
	after node at index of *n*
*/
void list_insert(tm_list* list, int n, tm_obj obj){
	_list_check_cap(list);
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

int list_index( tm_list* list, tm_obj v){
	int i;int len = list->len;
	tm_obj* nodes = list->nodes;
	for(i = 0; i < len;i++){
		if( tm_equals(nodes[i], v)){
			return i;
		}
	}
	return -1;
}

tm_obj list_pop(tm_list* list)
{
	tm_obj obj = list_get(list, -1 );
	list->len--;
	return obj;
}

int list_next(tm_list* list, tm_obj* des){
	if( list->cur < list->len){
		*des = list_get(list, list->cur);
		list->cur++;
		return 1;
	}
	list->cur = 0;
	return 0;
}


tm_list* list_join(tm_list* list1, tm_list*list2){
	return NULL;
}

// belows are builtin methods
//

tm_obj blt_list_join(tm_obj params)
{
	tm_obj l1 = get_arg(params, 0, TM_LST);
	tm_obj l2 = get_arg(params, 1, TM_LST);
	tm_list*  list1 = get_list(l1);
	tm_list*  list2 = get_list(l2);
	int len = list1->len + list2->len;
	int list1_nodes_size = list1->len * OBJ_SIZE;
	tm_obj des = list_new( len );
	tm_list* list = get_list(des);
	memcpy( list->nodes, list1->nodes, list1_nodes_size);
	memcpy( list->nodes + list1_nodes_size, list2->nodes, list2->len * OBJ_SIZE);
	return des;
}


tm_obj blt_push( tm_obj params){
	tm_obj self = get_arg( params, 0, TM_LST);
	tm_obj v = get_arg( params , 1, -1);
	list_append( get_list(self), v);
	return tm->none;
}

tm_obj blt_list_pop(tm_obj params){
	tm_obj self = get_arg(params, 0, TM_LST);
	return list_pop(get_list(self));
}
tm_obj blt_list_insert(tm_obj params){
	tm_obj self = get_arg(params, 0, TM_LST);
	tm_obj idx = get_arg(params, 1, TM_NUM);
	int n = idx.value.dv;
	tm_obj v = get_arg( params, 2, -1);
	list_insert( get_list(self), n, v );
	return self;
}
