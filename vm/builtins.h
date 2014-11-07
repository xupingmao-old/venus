
#ifndef builtins_h
#define builtins_h
#include "tm.h"

void cprint(tm_obj v);
void cprintln(tm_obj v);
void cprintln_show_special(tm_obj o);
tm_obj tm_print( tm_arguments params);
tm_obj _tm_format(char* fmt, va_list ap, int appendln);
tm_obj tm_format(char*fmt, ...);
void _tm_printf(char* fmt, va_list ap);
void tm_printf(char* fmt, ...);
void tm_printf_only_type(char* fmt, ...);
tm_obj tm_int( tm_arguments p);
tm_obj tm_float(tm_arguments p);
tm_obj _obj_info(tm_obj o);
tm_obj _tm_type(tm_obj o);
tm_obj _tm_chr(int n);

tm_obj _tm_str(tm_obj obj);
tm_obj btm_str( tm_arguments a);

int _tm_len(tm_obj o);
tm_obj tm_len( tm_arguments p);

tm_obj blt_add_type_method(tm_arguments p);

#endif
