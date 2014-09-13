#ifndef core_h
#define core_h
#include "tm.h"

void* tm_alloc( tm_vm* tm, size_t size);
void* tm_realloc( tm_vm* tm, void* o, size_t osize, size_t nsize);
tm_obj obj_new(int type, void* value);
void obj_free(tm_vm* tm, tm_obj obj);
void tm_raise(tm_vm* tm);

void string_free(tm_vm*tm, tm_string* s);
void list_free(tm_vm* tm, tm_list* list);
void dict_free(tm_vm* tm, tm_dict* dict);

#endif
