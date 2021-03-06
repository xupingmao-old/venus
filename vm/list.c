
#include "tm.h"

tm_list* _list_new( int cap ){
	tm_list* list = tm_alloc( sizeof(tm_list));
	list->len = 0;
	if( cap <= 0) cap = 2;
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

/* build list of length n from object list */
tm_obj as_list(int n, ...){
	tm_obj list = list_new(n);
	va_list a; va_start(a,n);
	int i;
	for (i=0; i<n; i++) {
		list_append(get_list(list),va_arg(a,tm_obj));
	}
	va_end(a);
	return list;
}

/* build list from tm_obj arrays */
tm_obj build_list( int n, tm_obj* items){
	tm_obj obj = list_new(n);
	int i;for(i = 0; i < n; i++){
		list_append( get_list(obj), items[i]);
	}
	return obj;
}


void list_free(tm_list* list){
#if DEBUG_GC
    printf("free list %p ...\n", list);
	int old = tm->allocated_mem;
#endif
	tm_free(list->nodes, list->cap * sizeof(tm_obj));
	tm_free(list, sizeof(tm_list));
#if DEBUG_GC
    int _new = tm->allocated_mem;
	printf("free list  %d => %d, freed: %d B\n", old,_new, old - _new);
#endif
}


tm_obj list_get(tm_list* list, int n)
{
	if( n < 0 )
	 n += list->len;
	if( n >= list->len || n < 0)
	{
		tm_raise("list_get: index overflow, @[@]", _tm_type(obj_new(TM_LST, list)), number_new(n));
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
	if( list->len >= list->cap  )
	{
		int ocap = list->cap;
		list->cap += ocap / 2 + 1;
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
		for(; i > n && i >= 0; i -- ){
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


/*
	join two list into one;
*/

tm_obj list_join(tm_list* list1, tm_list*list2){
	int newl = list1->len + list2->len;
	tm_obj newlist = list_new( newl );
	tm_list* list = get_list(newlist);
	list->len = newl;
	int list1_nodes_size = list1->len * OBJ_SIZE;
	memcpy( list->nodes, list1->nodes, list1_nodes_size);
	memcpy( list->nodes + list1->len, list2->nodes, list2->len * OBJ_SIZE);
	return newlist;
}

// belows are builtin methods
//

tm_obj blt_list_join(tm_obj params)
{
	tm_obj l1 = get_arg(params, 0, TM_LST);
	tm_obj l2 = get_arg(params, 1, TM_LST);
	tm_list*  list1 = get_list(l1);
	tm_list*  list2 = get_list(l2);
	return list_join( list1, list2 );
}


tm_obj blist_append( tm_obj params){
	tm_obj self = get_arg( params, 0, TM_LST);
	tm_obj v = get_arg( params , 1, -1);
	list_append( get_list(self), v);
	return obj_none;
}

tm_obj blist_pop(tm_obj params){
	tm_obj self = get_arg(params, 0, TM_LST);
	return list_pop(get_list(self));
}
tm_obj blist_insert(tm_obj params){
	tm_obj self = get_arg(params, 0, TM_LST);
	tm_obj idx = get_arg(params, 1, TM_NUM);
	int n = get_num(idx) ;
	tm_obj v = get_arg( params, 2, -1);
	list_insert( get_list(self), n, v );
	return self;
}

tm_obj blist_extend( tm_obj params){
	tm_obj self = get_arg( params, 0, TM_LST);
	tm_obj des = get_arg( params, 0, TM_LST);
	int i;
	for(i = 0; i < list_len(des) ; i++){
		list_append( get_list(self), list_nodes(des)[i]);
	}
	return obj_none;
}

tm_obj blist_index( tm_obj params){
	tm_obj self = get_arg( params, 0, TM_LST);
	tm_obj v = get_arg( params, 1, -1);
	return number_new( list_index(get_list(self), v));
}

tm_obj blist_reverse(tm_obj params){
	tm_obj self = get_arg(params, 0, TM_LST);
    int start = 0, end = list_len( self ) -1 ;
    while ( end > start ) {
        tm_obj temp = list_nodes(self)[start];
        list_nodes(self)[start] = list_nodes(self)[end];
        list_nodes(self)[end] = temp;
        end--;start++;
    }
	return obj_none;
}

#define tm_append( list, v) list_append( get_list(list), v);

