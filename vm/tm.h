

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
#define USE_NON_PARAM 1

int enable_debug = 0;

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>
#include <time.h>
#include <sys/stat.h>
#include <math.h>

#define tm_inline inline
typedef char instruction;

#define TM_STR 1
#define TM_NUM 2
#define TM_LST 3
#define TM_DCT 4
#define TM_FNC 5
#define TM_STREAM 6 // stream
#define TM_NON 7
#define TM_END 8 // end of list or dict
#define TM_MOD 9

#define TM_NATIVE 1
#define TM_METHOD 3
#define TM_NATIVE_METHOD 4

#define MAX_FILE_SIZE 1024 * 1024 * 5 // max file size loaded into memery


typedef struct tm_str{
	int marked;
	int len;
	int stype; /* 字符串类型 */
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
	tm_obj (*native_func)( tm_obj);
}tm_func;


#define OBJ_SIZE sizeof( tm_obj )

typedef struct tm_frame
{
	tm_obj locals[256];
	tm_obj *stack;
	tm_obj *top; // current stack top;
	tm_obj *last_pc;
	char* last_code;
    unsigned char* code;
    unsigned char** tags;
	int stacksize;
	int maxlocals;
	int maxstack;
	// tm_obj globals;
    /* object allocated during this frame, 
    during gc, new objects in current frame will marked not used,
    but new objects ahead current frame will be marked as used.*/
    tm_obj new_objs; 
    tm_obj params;
	// tm_obj file; // file name
	// tm_obj code; // byte code
	tm_obj ex; // exception info
	tm_obj line; // current line
	tm_obj fnc;
	unsigned char* jmp; // catch/except position
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
	/* tm_list* black;
	tm_list* white;
	tm_dict* strings; */

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
//tm_obj empty_list;
tm_obj g_arguments;
#include "object.h"

tm_inline
tm_obj tm_number(double v){
	tm_obj o;
	o.type = TM_NUM;
	o.value.dv = v;
	return o;
}

struct tm_check_result_st{
    int len;
    int maxlocals;
    int maxstack;
    unsigned char* pc;
};


void cprint(tm_obj v);
void cprintln(tm_obj v);
void cprintln_show_special(tm_obj o);
tm_obj tm_print( tm_obj params);
tm_obj _tm_format(char* fmt, va_list ap, int appendln);
tm_obj tm_format(char*fmt, ...);
void _tm_printf(char* fmt, va_list ap);
void tm_printf(char* fmt, ...);
void tm_printf_only_type(char* fmt, ...);
tm_obj tm_int( tm_obj p);
tm_obj tm_float(tm_obj p);
tm_obj _obj_info(tm_obj o);
tm_obj _tm_type(tm_obj o);
tm_obj tm_system(tm_obj p);
tm_obj _tm_chr(int n);

tm_obj blt_add_type_method(tm_obj p);


#define number_new tm_number

#include "macros.h"
#include "core.h"
#include "gc.h"
#include "ops.h"
#include "instruction.h"

#ifndef TM_TEST
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
#include "secure.c"
#include "frame.c"
#endif

#endif

