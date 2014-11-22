
#ifndef MAP_H_
#define MAP_H_
#include "tm.h"

typedef struct dict_node{
	tm_obj key;
	tm_obj val;
	struct dict_node* next;
}dict_node;


typedef struct tm_dict
{
	int marked;
	int len;
	int cap;
	int cur; // next 函数记录当前处理的node个数
	int last_node_offset; // next 函数 ， 上一个node对应的nodes偏移量
	dict_node* last_node; // next 函数中记录上一个node
	dict_node* _nodes[7];
    dict_node** nodes;
}tm_dict;

/**
* public interfaces
*/
tm_obj dict_new();
tm_dict* _dict_new();
void dict_free(tm_dict* dict);
void dict_set( tm_dict* dict, tm_obj key, tm_obj val);
int dict_iget(tm_dict* dict, tm_obj key, tm_obj *des);
dict_node* dict_get_node(tm_dict* dict, tm_obj key);
int dict_inext(tm_dict* dict, tm_obj *key, tm_obj* val);
void dict_print(tm_dict* dict);
void dict_iter_init(tm_dict* dict);
#endif
