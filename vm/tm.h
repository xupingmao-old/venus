

#ifndef tm_h
#define tm_h

#define DEBUG_GC 1

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
	int len;
	int cap;
	int cur;
	int marked;
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
	double dv;
	int iv;
	long lv;
	struct tm_string* str;
	struct tm_list* list;
	struct tm_dict* dict;
	struct tm_stream* stream;
	struct tm_func* func;
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
	tm_obj self;
	tm_obj globals;
	char* code;
	tm_obj (*native_func)(tm_vm*, tm_obj);
}tm_func;


#define OBJ_SIZE sizeof( tm_obj )

typedef struct tm_frame
{
	tm_list* params;

	tm_obj locals[256];
	tm_obj globals;
	tm_obj code;
	int cur;
	int jmp;

}tm_frame;

typedef struct tm_vm
{
	char* version;
	tm_obj error;
	tm_list* exception_stack;
	
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

	tm_list* params;

	tm_obj modules;
	tm_obj builtins;
	int steps;

	tm_list* root;
	tm_list* all;
	tm_list* black;
	tm_list* white;
	tm_dict* strings;

	int allocated_mem;
	int used_mem;

}tm_vm;


#include "object.h"
#include "macros.h"
#include "constants.h"
#include "core.h"
#include "args.h"
#include "builtins.h"
#include "gc.h"
#include "ops.h"
#include "instruction.h"

inline
tm_obj tm_number(double v){
	tm_obj o;
	o.type = TM_NUM;
	o.value.dv = v;
	return o;
}


#include "string.c"
#include "list.c"
#include "dict.c"
#include "func.c"
#include "core.c"
#include "args.c"
#include "builtins.c"
#include "ops.c"
#include "gc.c"
#include "stream.c"
#include "test.c"

#endif

