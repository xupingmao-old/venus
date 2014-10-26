
#ifndef builtins_h
#define builtins_h
#include "tm.h"

void cprint(tm_obj v);
void cprintln(tm_obj v);
void cprintln_show_special(tm_obj o);
tm_obj tm_print( tm_obj params);
tm_obj _tm_format(char* fmt, va_list ap);
tm_obj tm_format(char*fmt, ...);
void _tm_printf(char* fmt, va_list ap);
void tm_printf(char* fmt, ...);
tm_obj tm_int( tm_obj p);
tm_obj tm_float(tm_obj p);
tm_obj _obj_info(tm_obj o);

#endif
