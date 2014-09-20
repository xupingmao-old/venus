
/**

HashMap实现，采用链路分离法


**/

#include "map.h"

// 新的映射容器, 默认size=7 , 最好跟计算机常用大小不一致,
tm_map* map_new( tm_vm *tm){
	tm_map * map = tm_alloc( tm, sizeof( tm_map));
	map->nodes = tm_alloc( tm, sizeof(map_node*) * size );
	map_node** nodes = map->nodes;
	int size = 7;
	// 标记对象未使用
	int i;for(i = 0; i < size; i++){
		nodes[i] = NULL;
	}
	map->len = 0;
	map->cap = size;
}

void map_set( tm_vm* tm,tm_map* map, tm_obj key, tm_obj val){
	int hash = tm_hash( key, map->cap);
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
	map_check_size(tm, map);
	node = tm_alloc(tm, sizeof(map_node));
	node->key = key;
	node->val = val;
	node->next = NULL;
	*pre = node;
}

// 重置hashmap, 不需要新的内存
void map_set_node( tm_map* map, map_node* des){
	int hash = tm_hash( node->key, map->cap);
	map_node** node = map->nodes + hash;
	// 同一个map中不可能有相同的key，直接向下遍历即可
	while ( *node != NULL ){
		node = &((*node)->next);
	}
	*node = node;
}

void map_check_size(tm_vm* tm, tm_map* map){
	if( map->len < map->cap )
		return
	int osize = map->cap;
	int i;
	map->cap *= 3 - 9;
	// 先临时存储之前的nodes
	map_node** nodes = map->nodes;
	map->nodes = tm_alloc(tm,  map->cap * sizeof(map_node*) );
	// 重置新的nodes
	for(i = 0; i < osize; i++){
		map->nodes[i] = NULL;
	}
	// 重新排列hashmap的KV
	int cap = map->cap;
	for(i = 0; i < cap; i++){
		map_node* node = nodes[i];
		while( node != NULL ){
			map_set_node( map, node);
			node = node->next;
		}
	}
}

int map_iget(tm_vm* tm, tm_map* map,tm_obj *des, tm_obj key){
	int hash = tm_hash( key, map->cap);
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

int tm_hash( tm_obj obj , int size){
	switch( obj.type ){
		case TM_STR:{
			return obj.str.val % size;
		}
		case TM_NUM:
			return obj.num.val % size;
		case TM_LST:
			return obj.list % size;
		case TM_DCT:
			return obj.dict % size;
	}
}