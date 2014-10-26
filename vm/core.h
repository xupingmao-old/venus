#ifndef core_h
#define core_h
#include "tm.h"

void* tm_alloc( size_t size);
void* tm_realloc(void* o, size_t osize, size_t nsize);
void tm_free(void* o, size_t size);

tm_obj obj_new(int type, void* value);
void obj_free(tm_obj obj);
void tm_raise(char*fmt , ...);

#endif
