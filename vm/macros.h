
#ifndef macros_h
#define macros_h
#include "tm.h"

#define strequals(a, b) a == b || strcmp(a,b) == 0

#define get_val(obj) (obj).value
#define get_num(obj) get_val(obj).dv
#define get_double(obj) get_val(obj).dv
#define get_int(obj) (int) get_num(obj)
#define get_long(obj) get_val(obj).lv
#define get_str(obj) (obj.value.str)->value
#define get_file(obj) (obj.value.stream)->fp
#define get_stream(obj) (obj.value.stream)->fp
#define get_str_len(obj) obj.value.str->len
#define str_len get_str_len
#define get_func(obj) (obj.value.func)

#define get_list(obj) get_val(obj).list
#define get_list_len(obj) get_list(obj)->len
#define get_dict(obj) get_val(obj).dict
#define get_dict_len(obj) get_dict(obj)->len
#define get_mod(obj) get_val(obj).mod

#define get_nodes(obj)  get_list(obj)->nodes
#define get_keys(obj)   get_dict(obj)->keys
#define get_vals(obj)   get_dict(obj)->values


#define list_len(obj)  get_list(obj)->len
#define dict_len(obj)  get_dict_len(obj)

#define ptr_addr( ptr ) (long) (ptr) / sizeof(char*)

#define S(s) str_new(s,0)
#define N(n) tm_number(n)


/* for instruction read */
#define next_char( s ) *s++
#define next_byte( s ) *s++
/* gcc process ++ from right to left */
#define next_short( s ) (((*s) << 8) + *(s+1));s+=2;

/* for math */
#define max(a, b) (a) > (b) ? (a) : (b)

#endif


