
#include "tm.h"

void* tm_alloc( size_t size)
{
	void* block = malloc( size );
#if DEBUG_GC
	printf("%d -> %d , +%d\n", tm->allocated_mem, tm->allocated_mem + size, size);
#endif
	if( block == NULL )
	{
		tm_raise("tm_alloc: fail to alloc memory block");
	}
	tm->allocated_mem += size;
	return block;
}

void* tm_realloc( void* o, size_t osize, size_t nsize)
{
	void* block = tm_alloc( nsize );
	memcpy(block, o, osize);
	tm_free(o, osize);
	return block;
}

void tm_free(void* o, size_t size){
#if DEBUG_GC
	printf("%d -> %d , -%d\n", tm->allocated_mem, tm->allocated_mem - size, size);
#endif
	free(o);
	tm->allocated_mem -= size;
}

void tm_raise(char* fmt, ...)
{
	va_list a; 
	va_start(a,fmt);
	// 添加\n
	int len = strlen(fmt);
	char*s = tm_alloc(len + 2);
	memcpy(s, fmt, len);
	s[len] = '\n';
	s[len+1] = '\0';
	tm->frames[tm->cur].ex = _tm_format(s, a);
	// 释放资源
	tm_free(s, len + 2);
	va_end(a);
	longjmp(tm->buf, 1);
}

tm_obj obj_new( int type , void * value){
	tm_obj o;
	o.type = type;
	switch( type ){
		case TM_STR: o.value.str = value;break;
		case TM_LST: o.value.list = value;break;
		case TM_MAP: o.value.map = value;break;
	}
	return o;
}

void obj_free( tm_obj o){
	switch( o.type ){
	case TM_STR: string_free( o.value.str);break;
	case TM_LST: list_free( o.value.list);break;
	case TM_MAP: map_free(o.value.map);break;
	/*case TM_DCT: dict_free(  tm, o.value.dict);break;*/
	case TM_FNC: func_free( o.value.func);break;
	}
}
