
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
	map_node** nodes;
}tm_map;


tm_map* map_new(tm_vm* tm);
void map_set(tm_vm* tm, tm_map* map, tm_obj key, tm_obj val);
int map_iget(tm_vm* tm, tm_map* map, tm_obj *des, tm_obj key);

#endif