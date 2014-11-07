

#ifndef tm_h
#define tm_h
// #pragma pack(4)
#define DEBUG_GC 0
#define PRINT_INS 0
#define PRINT_INS_CONST 0
#define EVAL_DEBUG 1
#define DEBUG_INS 1
#define LIGHT_DEBUG_GC 0
#define LOG_LEVEL 1

int enable_debug = 0;

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>
#include <time.h>
#include <sys/stat.h>

#define tm_inline inline
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

struct  _gc_obj
{
	int marked;
	/* data */
};

typedef union tm_value
{
	double dv;
	double num;
	int iv;
	long lv;
	void* ptr;
	struct tm_str* str;
	struct tm_list* list;
	struct tm_stream* stream;
	struct tm_func* func;
	struct tm_dict* dict;
	struct tm_module* mod;
	struct _gc_obj* gc;
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
	// tm_obj name;
}tm_module;

typedef struct tm_stream
{
	int marked;
	tm_obj name;
	FILE* fp;
}tm_stream;

/*
typedef struct tm_arguments{
    int len;
    tm_obj *nodes;
}tm_arguments;
*/

typedef tm_obj tm_arguments;
typedef struct tm_vm tm_vm;
typedef struct tm_func
{
	int marked;
	int fnc_type;
	int maxlocals;
	int maxstack;
	instruction* pc;
	tm_obj self;
	tm_obj mod; // module, includes global, constants, etc.
	// tm_obj code; // string
	tm_obj name;
	tm_obj (*native_func)( tm_arguments);
}tm_func;

#define OBJ_SIZE sizeof( tm_obj )

typedef struct tm_frame
{
	tm_obj locals[256];
	tm_obj *stack;
	tm_obj *top; // current stack top;
	tm_obj *last_pc;
	char* last_code;
	int stacksize;
	int maxlocals;
	int maxstack;
	tm_obj globals;
	tm_obj constants; 
    tm_obj new_objs; // object allocated during this frame, reject.
	tm_obj file; // file name
	tm_obj code; // byte code
	tm_obj ex; // exception info
	tm_obj line; // current line
	tm_obj func_name;
	int jmp; // catch/except position
}tm_frame;

#include "dict.h"

#define FRAMES_COUNT 128

typedef struct tm_vm
{
	char* version;
	jmp_buf buf; // 异常栈

	int cur; // current frame
	tm_frame frames[FRAMES_COUNT];

	tm_obj modules;
	tm_obj builtins;
	tm_obj root;
	int steps;

	tm_list* all;
	tm_list* black;
	tm_list* white;
	tm_dict* strings;

	int allocated_mem;
	int gc_limit;

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
tm_obj obj__main__;
tm_obj obj__name__;
tm_obj obj_mod_ext;
tm_obj __chars__[256];
tm_obj g_arguments;
#include "constants.h"
#include "object.h"
#include "macros.h"

tm_inline
tm_obj tm_number(double v){
	tm_obj o;
	o.type = TM_NUM;
	get_num(o) = v;
	return o;
}

#define number_new tm_number

#include "core.h"
#include "builtins.h"
#include "gc.h"
#include "ops.h"
#include "instruction.h"

#include "compile.c"
#include "core.c"
#include "string.c"
#include "list.c"
#include "func.c"
#include "builtins.c"
#include "ops.c"
#include "gc.c"
#include "stream.c"
#include "dict.c"
#include "test.c"
#include "secure.c"
#include "frame.c"

#endif

