
#ifndef MAP_H_
#define MAP_H_
#include "tm.h"

typedef struct map_node{
	tm_obj key;
	tm_obj val;
	struct map_node* next;
}map_node;


typedef struct tm_map
{
	int marked;
	int len;
	int cap;
	int cur; // next 函数记录当前处理的node个数
	int last_node_offset; // next 函数 ， 上一个node对应的nodes偏移量
	map_node* last_node; // next 函数中记录上一个node
	map_node** nodes;
}tm_map;

/* 公开接口
 *
 */
tm_obj map_new(tm_vm* tm);
void map_free(tm_vm*tm, tm_map* map);
void map_set(tm_vm* tm, tm_map* map, tm_obj key, tm_obj val);
int map_iget(tm_vm* tm, tm_map* map, tm_obj *des, tm_obj key);
int map_inext(tm_vm* tm, tm_map* map, tm_obj *key, tm_obj* val);
void map_print(tm_map* map);
#endif
