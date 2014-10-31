
#include "tm.h"


tm_inline
tm_obj get_arg(tm_obj params, int pos, int type){
	tm_list* list = params.value.list;
	int n = list->len;
	if ( pos >= n ){
		tm_raise("get_arg: index overflow");
	}
	tm_obj v = list->nodes[pos];
	if ( type < 0 ){
		return v;
	}
	if( v.type != type){
		tm_raise("get_arg: TypeError see @", _obj_info(v));
	}
	return v;
}


tm_inline
int get_int_arg(tm_obj v){
	if( v.type != TM_NUM){
		tm_raise( "get_int_arg:@ is not a number", v );
	}
	return (int)get_num(v);
}

void* tm_alloc( size_t size)
{
	if( size <=  0){
		tm_raise("tm_alloc(), you allocate a memory block with size <= 0!");
		return NULL;
	}
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
	if( o == NULL )
		return;
#if DEBUG_GC
	printf("%d -> %d , -%d\n", tm->allocated_mem, tm->allocated_mem - size, size);
#endif
	// if (size == 0 ){
	// 	tm_raise("tm_free(), you free a block of size 0!");
	// }
	free(o);
	tm->allocated_mem -= size;
}

void tm_raise(char* fmt, ...)
{
	va_list a; 
	va_start(a,fmt);
	tm->frames[tm->cur].ex = _tm_format(fmt, a, 1);
	va_end(a);
	longjmp(tm->buf, 1);
}

inline
tm_obj obj_new( int type , void * value){
	tm_obj o;
	o.type = type;
	switch( type ){
		case TM_NUM: o.value.num = *(double*)value;break;
		case TM_STR: o.value.str = value;break;
		case TM_LST: o.value.list = value;break;
		case TM_DCT: o.value.dict = value;break;
		case TM_MOD: get_mod(o) = value;break;
		case TM_FNC: get_func(o) = value;break;
		case TM_NON: break;
		default: tm_raise("obj_new: not supported type @", number_new(type));
	}
	return o;
}

void obj_free( tm_obj o){
	switch( o.type ){
	case TM_STR: str_free( o.value.str);break;
	case TM_LST: list_free( o.value.list);break;
	case TM_DCT: dict_free(o.value.dict);break;
	case TM_FNC: func_free( o.value.func);break;
	case TM_MOD: module_free( o.value.mod );break;
	}
}

void tm_log(char* type, char* fmt, ...){
    static int log_open = 0;
    static FILE *fp;
    if( !log_open ){
        fp = fopen("log.log", "a");
        log_open = 1;
    }
    if( strcmp(type, "close") == 0){
        log_open = 0;
        fclose(fp);
        fp = NULL;
    }else {
        fwrite(type, 1, strlen(type), fp);
    }
}
