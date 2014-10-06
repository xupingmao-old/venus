

#ifndef tm_h
#define tm_h

#define DEBUG_GC 0

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>

typedef struct tm_string{
	int marked;
	int len;
	int inHeap; // 0: value is static , 1: value is in heap;
	char *value;
}tm_string;

typedef struct tm_list
{
	int marked;
	int len;
	int cap;
	int cur;
	int nodesize;
	struct tm_obj* nodes;
}tm_list;

typedef struct tm_dict
{
	int marked;
	tm_list* keys;
	tm_list* values;
}tm_dict;


typedef union tm_value
{
	int marked;
	double dv;
	double num;
	int iv;
	long lv;
	struct tm_string* str;
	struct tm_list* list;
/*	struct tm_dict* dict;*/
	struct tm_stream* stream;
	struct tm_func* func;
	struct tm_map* map;
}tm_value;


typedef struct tm_obj
{
	int type; // marked, type, others
	tm_value value;
}tm_obj;

typedef struct tm_stream
{
	tm_obj name;
	FILE* fp;
}tm_stream;

typedef struct tm_vm tm_vm;
typedef struct tm_func
{
	int marked;
	int fnc_type;
	tm_obj self;
	tm_obj globals;
	tm_obj code; // string
	tm_obj (*native_func)( tm_obj);
}tm_func;


#define OBJ_SIZE sizeof( tm_obj )

typedef struct tm_frame
{
	tm_obj locals[256];
	tm_obj *stack;
	tm_obj globals;
	tm_obj constants; 
	tm_obj mod; // module
	tm_obj code; // byte code
	tm_obj ex; // exception info
	int cur; 
	int jmp; // catch/except position
}tm_frame;

#include "map.h"

typedef struct tm_vm
{
	char* version;
	tm_obj error;
	tm_list* exception_stack;
	
	jmp_buf buf; // 异常栈
	tm_obj *stack;
	tm_obj *top;

	int cur; // current frame
	tm_frame frames[256];

	tm_obj chars[256];

	tm_obj none;
	tm_obj none_str;
	tm_obj empty_str;
	tm_obj end;

	tm_obj string_methods;
	tm_obj list_methods;

	tm_obj modules;
	tm_obj builtins;
	int steps;

	tm_list* root;
	tm_list* all;
	tm_list* black;
	tm_list* white;
	tm_map* strings;

	int allocated_mem;
	int used_mem;

}tm_vm;

/**
 * 系统就一个全局变量tm， 也就是虚拟机的structure
 */
tm_vm* tm;

#include "constants.h"
#include "object.h"

inline
tm_obj tm_number(double v){
	tm_obj o;
	o.type = TM_NUM;
	o.value.dv = v;
	return o;
}

#include "macros.h"
#include "core.h"
#include "builtins.h"
#include "gc.h"
#include "ops.h"
#include "instruction.h"
#include "args.h"

#include "string.c"
#include "list.c"
#include "func.c"
#include "core.c"
#include "builtins.c"
#include "ops.c"
#include "gc.c"
#include "stream.c"
#include "map.c"
#include "test.c"
#include "frame.c"

#endif

