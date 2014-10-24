

#ifndef tm_h
#define tm_h

#define DEBUG_GC 0
#define PRINT_INS 1
#define PRINT_INS_CONST 0

int enable_debug = 0;

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>

typedef char instruction;

typedef struct tm_str{
	int marked;
	int len;
	int inHeap; // 0: value is static , 1: value is in heap;
	char *value;
}tm_str;

typedef struct tm_list
{
	int marked;
	int len;
	int cap;
	int cur;
	int nodesize;
	struct tm_obj* nodes;
}tm_list;


typedef union tm_value
{
	int marked;
	double dv;
	double num;
	int iv;
	long lv;
	struct tm_str* str;
	struct tm_list* list;
	struct tm_stream* stream;
	struct tm_func* func;
	struct tm_dict* dict;
	struct tm_module* mod;
}tm_value;


typedef struct tm_obj
{
	int type; // marked, type, others
	tm_value value;
}tm_obj;

typedef struct tm_module
{
	int marked;
	unsigned char** tags;
	int tagsize;
	int checked;
	tm_obj globals;
	tm_obj constants;
	tm_obj code;
	tm_obj file;
	tm_obj name;
}tm_module;

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
	instruction* pc;
	tm_obj self;
	tm_obj mod; // module, includes global, constants, etc.
	tm_obj code; // string
	tm_obj (*native_func)( tm_obj);
}tm_func;


#define OBJ_SIZE sizeof( tm_obj )

typedef struct tm_frame
{
	tm_obj locals[256];
	tm_obj *stack;
	tm_obj *last_pc;
	char* last_code;
	int stacksize;
	int maxlocals;
	tm_obj globals;
	tm_obj constants; 
	tm_obj file; // file name
	tm_obj code; // byte code
	tm_obj ex; // exception info
	tm_obj line; // current line
	int jmp; // catch/except position
}tm_frame;

#include "dict.h"

#define FRAMES_COUNT 256

typedef struct tm_vm
{
	char* version;
	tm_obj error;
	tm_list* exception_stack;
	
	jmp_buf buf; // 异常栈
	tm_obj *stack;
	tm_obj *top;

	int cur; // current frame
	tm_frame frames[FRAMES_COUNT];

	tm_obj chars[256];

	tm_obj none;
	tm_obj string_methods;
	tm_obj list_methods;

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

/**
 * global virtual machine
 */
tm_vm* tm;

tm_obj str_class;
tm_obj list_class;
tm_obj dict_class;

tm_obj obj_true;
tm_obj obj_false;
tm_obj obj_none;
tm_obj obj__init__;

#include "constants.h"
#include "object.h"

inline
tm_obj tm_number(double v){
	tm_obj o;
	o.type = TM_NUM;
	o.value.dv = v;
	return o;
}

#define number_new tm_number

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
#include "dict.c"
#include "test.c"
#include "secure.c"
#include "frame.c"

#endif

