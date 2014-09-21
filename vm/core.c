
#include "tm.h"

void* tm_alloc( tm_vm* tm, size_t size)
{
	void* block = malloc( size );
	if( block == NULL )
	{
		tm->error = string_new(tm, "tm_alloc: fail to alloc memory block");
		tm_raise(tm);
	}
	tm->allocated_mem += size;
	return block;
}

void* tm_realloc( tm_vm* tm, void* o, size_t osize, size_t nsize)
{
	void* block = realloc( o, nsize);
	if( block == NULL ){
		tm->error = string_new(tm, "tm_realloc:fail to realloc memory block");
		tm_raise(tm);
	}
	tm->allocated_mem += nsize - osize;
	return block;
}

void tm_free( tm_vm* tm, void* o, size_t size){
	free(o);
	tm->allocated_mem -= size;
}

void tm_raise(tm_vm* tm)
{
	if( tm->error.type != TM_NON )
	{
		_tm_print(tm->error);
		exit(-1);
		list_push( tm, tm->exception_stack, tm->error);
	}
}

tm_obj obj_new( int type, void* value)
{
	tm_obj v;
	switch( type ){
	case TM_STR:
		v.type = TM_STR;
		v.value.str = value;
		return v;
	case TM_DCT:
		v.type = TM_DCT;
		v.value.dict = value;
		return v;
	}
}

void obj_free(tm_vm* tm, tm_obj o){
	switch( o.type ){
	case TM_STR: string_free(tm, o.value.str);break;
	case TM_LST: list_free(  tm, o.value.list);break;
	case TM_DCT: dict_free(  tm, o.value.dict);break;
	case TM_USER_FNC:
	case TM_NATIVE_FNC:
	case TM_METHOD:
	case TM_NATIVE_METHOD:
	case TM_FNC: func_free(  tm, o.value.func);break;
	case TM_MAP: map_free(tm, o.value.map);break;
	}
}
