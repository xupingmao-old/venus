
#include "tm.h"


tm_inline
tm_obj get_arg(tm_obj params, int pos, int type){
    if( TM_LST != params.type) tm_raise("get_arg(): expect argument to be list ,but see %t", params);
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
char* get_str_arg(tm_obj p, int pos){
    tm_obj str = get_arg(p, pos, TM_STR);
    return get_str(str);
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
        tm_raise("tm_alloc(), you allocate a memory block of size %d!", size);
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
        default: tm_raise("obj_new: not supported type %d", type);
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
int log_state = 0;
void enable_log(){
    log_state = 100;
}

void disable_log(){
    log_state = 0;
}

void set_log_state(int state){
    switch( state ){
        case -1: log_state = -1;break;
        case 2: 
            if( log_state == -1) {
                // log_state = 1;
            }else{
                log_state = 100;
            }break;
    }
}

void tm_log(char* type, char* fmt, ...){
    static int log_open = 0;
    static FILE *fp;
    if( 100 != log_state )return;
    // puts(fmt);
    if( !log_open ){
        // puts(fmt);
        fp = fopen("log.log", "rb+");
        if( fp == NULL ){
            fp = fopen("log.log", "w");
            if( fp == NULL)
                tm_raise("tm_log(), can not load log file log.log");
            fclose(fp);
            fp = fopen("log.log", "rb+");
            if( fp == NULL)
                tm_raise("tm_log(), can not load log file log.log");
        }
        fseek(fp, 0, SEEK_SET);
        log_open = 1;
    }
    va_list ap;
    va_start(ap, fmt);
    if( strcmp(type, "close") == 0){
        log_open = 0;
        if(fp != NULL)
            fclose(fp);
        fp = NULL;
        va_end(ap);
        return;
    }else if( strstr(" frame new cond info", type) != NULL){
        va_end(ap);
        return;
    }
    else {
        fwrite(type, 1, strlen(type), fp);
        fwrite("  --  ", 1, 6, fp);
        // vfprintf(fp, fmt, ap);
        tm_obj str = _tm_format(fmt, ap, 1);
        fwrite(get_str(str), 1, get_str_len(str), fp);
    }
    va_end(ap);
    // cprint(_tm_format(fmt, ap, 1));
}

#define ACTIVE_LOG 0
#if ACTIVE_LOG
    #define tm_log0(type, fmt) tm_log(type, fmt)
    #define tm_log1(type, fmt, arg0) tm_log(type, fmt, arg0)
    #define tm_log2(type, fmt, arg0, arg1) tm_log(type, fmt, arg0, arg1)
    #define tm_log3(type, fmt, arg0, arg1, arg2) tm_log(type, fmt, arg0, arg1, arg2)
    #define close_log() tm_log("close", "");
#else 
    #define tm_log0(type, fmt) ;
    #define tm_log1(type, fmt, arg0) ;
    #define tm_log2(type, fmt, arg0, arg1) ;
    #define tm_log3(type, fmt, arg0, arg1, arg2) ;
    #define close_log() tm_log("close", "");
    #define enable_log() ;
    #define disable_log() ;
#endif
#define get_char(n) __chars__[n]
