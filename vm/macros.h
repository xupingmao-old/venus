
#ifndef macros_h
#define macros_h
#include "tm.h"

#define strequals(a, b) a == b || strcmp(a,b) == 0

#define get_val(obj) obj.value
#define get_num(obj) get_val(obj).dv
#define get_double(obj) get_val(obj).dv
#define get_int(obj) get_val(obj).iv
#define get_long(obj) get_val(obj).lv
#define get_str(obj) (obj.value.str)->value
#define get_file(obj) (obj.value.stream)->fp
#define get_stream(obj) (obj.value.stream)->fp
#define get_str_len(obj) obj.value.str->len
#define str_len get_str_len
#define get_func(obj) (obj.value.func)

#define get_list(obj) get_val(obj).list
#define get_dict(obj) get_val(obj).dict
#define get_map(obj) get_val(obj).map

#define get_nodes(obj)  get_list(obj)->nodes
#define get_keys(obj)   get_dict(obj)->keys
#define get_vals(obj)   get_dict(obj)->values


#define list_len(obj)  get_list(obj)->len
#define dict_len(obj)  get_keys(obj)->len
#define map_len( obj)  get_map(obj)->len

#define _tm_raise( s ) tm->error = string_new(tm, s); tm_raise(tm);


#define ptr_addr( ptr ) (long) (ptr) / sizeof(char*)

#define S(s) string_new(s,0)
#define N(n) tm_number(n)

#endif


