
/**

HashMap实现，采用链路分离法


**/


// private functions
/*
    接口暴露原则，越少越好
*/
int map_index( tm_obj obj, int size);
void map_check_size( tm_map* map);
tm_map* map_new_(tm_vm* tm);
void map_iter_init(tm_map* map);

#include "tm.h"

// 新的映射容器, 默认size=7 , 最好跟计算机常用大小不一致,
tm_map* map_new_( tm_vm *tm){
	int size = 7;
	tm_map * map = tm_alloc( tm, sizeof( tm_map));
	map->nodes = tm_alloc( tm, sizeof(map_node*) * size );
	map_node** nodes = map->nodes;
	// 标记对象未使用
	int i;for(i = 0; i < size; i++){
		nodes[i] = NULL;
	}
	map->len = 0;
	map->cap = size;
	map_iter_init( map);
	return map;
}

tm_obj map_new(tm_vm* tm){
	tm_obj o;
	o.type = TM_MAP;
	get_map( o ) = map_new_(tm);
	return gc_track(tm, o);
}

void map_set( tm_vm* tm,tm_map* map, tm_obj key, tm_obj val){
	map_check_size( map);
	int hash = map_index( key, map->cap);
	map_node** nodes = map->nodes;
	map_node* node = nodes[hash];
	map_node** pre = nodes + hash;
	// 如果刚好键值相等，赋值
	while( node != NULL ){
		if( tm_equals(node->key, key) ){
			node->val = val;
			return;
		}
		pre = &(node->next);
		node = node->next;
	}
	// 新增一个节点
	map->len++;
	node = tm_alloc(tm, sizeof(map_node));
	node->key = key;
	node->val = val;
	node->next = NULL;
	*pre = node;
}

// 重置hashmap, 不需要新的内存
void map_set_node( tm_map* map, map_node* des){
	int hash = map_index( des->key, map->cap);
	// 同一个map中不可能有相同的key，直接向下遍历即可
	map_node** node = map->nodes + hash;
	while ( (*node) != NULL ){
		node = &((*node)->next);
	}
	des->next = NULL;
	*node = des;

	/*
	map_node* node = map->nodes[hash];
	if( node == NULL ){
		map->nodes[hash] = des;
	}
	else{
		map_node* temp = node;
		node = node->next;
		while( node != NULL ){
			temp = node; node = node->next;
		}
		des->next = NULL;
		temp->next = des;
	}*/
}

void map_check_size(tm_map* map){
	if( map->len < map->cap )
		return;
	int osize = map->cap;
	int i;
	int nsize = osize * 3 - 9;
	map->cap = nsize;
	// 先临时存储之前的nodes
	map_node** nodes = map->nodes;
	map->nodes = tm_alloc(tm,  nsize * sizeof(map_node*) );
	// 重置新的nodes
	for(i = 0; i < nsize; i++){
		map->nodes[i] = NULL;
	}
	// 重新排列hashmap的KV
	// 按照之前的size重排
	for(i = 0; i < osize; i++){
		map_node* node = nodes[i];
		while( node != NULL ){
			map_node* temp = node->next;
			map_set_node( map, node);
			node = temp;
		}
	}
	// 释放老的内存
	tm_free( tm, nodes, osize * sizeof(map_node*));
}

int map_iget(tm_vm* tm, tm_map* map,tm_obj *des, tm_obj key){
	int hash = map_index( key, map->cap);
	map_node* node = map->nodes[hash];
	while( node != NULL ){
		if(tm_equals(node->key, key)){
			*des = node->val;
			return 1;
		}
		node = node->next;
	}
	return 0;
}

void map_iter_init(tm_map* map ){
	map->cur = 0;
	map->last_node = NULL;
	map->last_node_offset = 0;
}

/**
 * 遍历map对象
 * 如果能够遍历返回1
 * 不能遍历返回0
 */
int map_inext(tm_vm* tm, tm_map* map, tm_obj* key, tm_obj *val){
	map_node** nodes = map->nodes;
	int cur = map->cur;
	if( cur >= map->len){
		map_iter_init(map);
		return 0;
	}
	map_node* node;
	if( map->last_node == NULL ){
		node = nodes[0];
	}else
		node = map->last_node->next;
	while( node == NULL ){
		// 当前为空，说明之前的散列格子里的对象已经遍历完了
		map->last_node_offset ++;
		node = nodes[ map->last_node_offset ];
	}
	*key = node->key;
	*val = node->val;
	map->cur++;
	map->last_node = node;
	return 1;
}

int map_index( tm_obj obj , int size){
	switch( obj.type ){
		case TM_STR:{
			int i = 0;
			int len = obj.value.str->len;
			char* val = obj.value.str->value;
			int step = len / size + 1;
			int hash = val[0] + len;
			/*for( i = 0; i < len; i += step){
				hash += val[i] << 2;
			}*/
			return hash & (size - 1);
		}
		case TM_NUM:
			return (int)get_num(obj) % size;
		case TM_LST:
			return ptr_addr( get_list(obj) ) % size;
		case TM_DCT:
			return ptr_addr( get_dict(obj) ) % size;
	}
	return 0;
}

void map_free_node( tm_vm*tm, map_node* node){
	if( node == NULL ){
		return;
	}
	map_free_node(tm, node->next );
	tm_free(tm, node, sizeof(map_node));
}

void map_free(tm_vm *tm, tm_map* map){
	int cap = map->cap;
	int i;
	for(i = 0; i < cap; i++){
		map_free_node( tm, map->nodes[i] );
	}
}

void map_print(tm_map* map){
	int i;int size = map->cap;
	map_node** nodes = map->nodes;
	for(i = 0; i < size;i++){
		map_node* node = nodes[i];
		if( node == NULL ){
			printf("map[%d]=NULL\n", i);
		}else{
			printf("map[%d]=", i);
			while( node != NULL){
				cprint(node->key);putchar(':');
				cprint(node->val);
				node = node->next;
				printf(" --> ");
			}
			putchar('\n');
		}
	}
}
