
/**
* HashMap实现，采用链路分离法
* @Author xupingmao <578749341@qq.com>
*/


/**
* less interface is better than more,
* because many interface with similar function will confuse the users.
*/

#include "tm.h"

/** new hashdict instance, with initial allocated size set to 7.
*   better not similar to computer binary size ( as to say 2 ), 
*   such as 2, 10 etc.
*/
tm_dict* _dict_new(){
	int size = 7;
	tm_dict * dict = tm_alloc(sizeof( tm_dict));
	dict->nodes = tm_alloc(sizeof(dict_node*) * size );
	dict_node** nodes = dict->nodes;
	// to mark that the node is not allocated.
	int i;for(i = 0; i < size; i++){
		nodes[i] = NULL;
	}
	dict->len = 0;
	dict->cap = size;
	dict_iter_init( dict);
	return dict;
}

tm_obj dict_new(){
	tm_obj o;
	o.type = TM_DCT;
	get_dict( o ) = _dict_new();
	return gc_track( o);
}

// reset hashdict, do not need to allocate new block.
void dict_set_node( tm_dict* dict, dict_node* des){
	int hash = dict_index( des->key, dict->cap);
	/* in a dict, there will never be two keys with same value,
    so we do not need to check the key value */
	dict_node* node = dict->nodes[hash];
    
    /*
    dict_node** node = dict->nodes + hash;
	while ( (*node) != NULL ){
		node = &((*node)->next);
	}
	des->next = NULL;
	*node = des;
    */

    /* bellow version is more readable than that one */
    
    /* reset next value of the node */
    des->next = NULL;
    /* check the node in the nodes list first */
	if( node == NULL ){
		dict->nodes[hash] = des;
	}
	else{
        /* find a node whose next is null */
		while( node->next != NULL ){
			node = node->next;
		}
		node->next = des;
	}
}

void dict_check_size(tm_dict* dict){
	if( dict->len < dict->cap )
		return;
	int osize = dict->cap;
	int i;
	int nsize = osize * 3 - 9;
	dict->cap = nsize;
	// store old nodes to rehash the table
	dict_node** nodes = dict->nodes;
	dict->nodes = tm_alloc( nsize * sizeof( dict_node* ));
	// reset new nodes
	for(i = 0; i < nsize; i++){
		dict->nodes[i] = NULL;
	}
    /* rehash nodes */
	for(i = 0; i < osize; i++){
		dict_node* node = nodes[i];
		while( node != NULL ){
			dict_node* temp = node->next;
			dict_set_node( dict, node);
			node = temp;
		}
	}
	// free old nodes.
	tm_free(nodes, osize * sizeof(dict_node*));
}


void dict_iter_init(tm_dict* dict ){
	dict->cur = 0;
	dict->last_node = NULL;
	dict->last_node_offset = 0;
}

/**
 * iterate dictionary nodes
 * returns 1 if find one, else 0
 */
int dict_inext(tm_dict* dict, tm_obj* key, tm_obj *val){
	dict_node** nodes = dict->nodes;
	int cur = dict->cur;
    /* already iterate over */
	if( cur >= dict->len){
		dict_iter_init(dict);
		return 0;
	}
	dict_node* node;
	if( dict->last_node == NULL ){
		node = nodes[0];
	}else
		node = dict->last_node->next;
	while( node == NULL ){
		dict->last_node_offset ++;
		node = nodes[ dict->last_node_offset ];
	}
	*key = node->key;
	*val = node->val;
	dict->cur++;
	dict->last_node = node;
	return 1;
}

int dict_index( tm_obj obj , int size){
	switch( obj.type ){
		case TM_STR:{
			int i = 0;
			int len = obj.value.str->len;
			char* val = obj.value.str->value;
			switch( len ){
				case 0: return 0;
				case 1: return (val[0] ) & (size-1);
				default: return (val[0] + val[1]) & ( size -1 );
			}
		}break;
		case TM_NUM:
			return (int)get_num(obj) % size;
	}
	return 0;
}

void dict_free_node(dict_node* node){
	if( node == NULL ){
		return;
	}
	dict_free_node(node->next );
	tm_free(node, sizeof(dict_node));
}

void dict_free(tm_dict* dict){
#if DEBUG_GC
    int old = tm->allocated_mem;
    printf("free dict %p ...\n",dict );
#endif

	int cap = dict->cap;
	int i;
	for(i = 0; i < cap; i++){
		dict_free_node( dict->nodes[i]);
	}
	tm_free(dict->nodes, sizeof(dict_node*) * cap);
	tm_free(dict, sizeof(tm_dict));
#if DEBUG_GC
    int _new = tm->allocated_mem;
    printf("dict free , from %d to %d, free %d B\n", old, _new, old-_new);
#endif

}

void dict_print(tm_dict* dict){
	int i;int size = dict->cap;
	dict_node** nodes = dict->nodes;
	for(i = 0; i < size;i++){
		dict_node* node = nodes[i];
		if( node == NULL ){
			printf("dict[%d]=NULL\n", i);
		}else{
			printf("dict[%d]=", i);
			while( node != NULL){
				cprint(node->key);putchar(':');
				cprint(node->val);
				node = node->next;
				if( node != NULL)
					printf(" --> ");
			}
			putchar('\n');
		}
	}
}

void dict_set(tm_dict* dict, tm_obj key, tm_obj val){
	dict_check_size( dict);
	int hash = dict_index( key, dict->cap);
	dict_node** nodes = dict->nodes;
	dict_node* node = nodes[hash];
	dict_node** pre = nodes + hash;
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
	dict->len++;
	node = tm_alloc(sizeof(dict_node));
	node->key = key;
	node->val = val;
	node->next = NULL;
	*pre = node;
}


int dict_iget(tm_dict* dict,tm_obj key,tm_obj *des){
	int hash = dict_index( key, dict->cap);
	dict_node* node = dict->nodes[hash];
	while( node != NULL ){
		if(tm_equals(node->key, key)){
			*des = node->val;
			return 1;
		}
		node = node->next;
	}
	return 0;
}

tm_obj dict_keys( tm_dict* dict){
	tm_obj list = list_new( dict->len );
	dict_iter_init(dict);
	tm_obj k,v;
	while( dict_inext(dict, &k, &v ) ){
		list_append( get_list(list), k);
	}
	return list;
}

tm_obj bdict_keys( tm_arguments p){
	tm_obj dict = get_arg(p, 0, TM_DCT);
	return dict_keys(get_dict(dict));
}
