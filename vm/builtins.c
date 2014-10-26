#include "tm.h"

void _tm_print(tm_obj o, int depth, int show_special){
	depth--;
	if( depth < 0 )
		return;
	switch( o.type ){
	case TM_STR:
		{
			int i;int len = str_len(o);
			char *s = get_str(o);
			if(show_special){
				putchar('"');
				for(i = 0; i < len; i++){
					if(s[i] == '\0'){
						printf("\\0");
					}else if( s[i] == '\r'){
						printf("\\r");
					}else if( s[i] == '\n'){
						printf("\\n");
					}else if( s[i] == '\t'){
						printf("\\t");
					}else{
						putchar(s[i]);
					}
				}
				putchar('"');
			}else
				printf("%s", s);
		}
		break;
	case TM_NUM:
		printf("%g", get_double(o));
		break;
	case TM_LST:
	{
		int i = 0;
		tm_list* list = get_list(o);
		printf("[");
		for(i = 0; i < list->len; i++){
			_tm_print(list->nodes[i], depth, show_special);
			if( i+1 != list->len){
				putchar(',');
			}
		}
		printf("]");
		break;
	}
	case TM_DCT:
	{
		int i = 0;
		tm_obj k,v;
		tm_dict* dict = get_dict( o );
		int len = dict->len;
		putchar('{');
		for(i = 0; dict_inext(dict, &k, &v); i++){
			_tm_print(k, depth, show_special);
			putchar(':');
			_tm_print(v, depth, show_special);
			if( i != len -1 ) putchar(',');
		}
		putchar('}');
		break;
	}
	// case TM_DCT:dict_print(get_dict(o));break;
	case TM_FNC:
		if( get_func(o)->self.type != TM_NON){
			printf("<method %p ",get_func(o));
		}else{
			printf("<function %p ",get_func(o));
		}
		if( get_func(o)->name.type != TM_NON){
			cprint(get_func(o)->name);
		}
		printf(">");
		break;
	case TM_MOD:
		printf("<module %p>",get_mod(o));
		break;
	case TM_NON:
		printf("None");break;
	}
}

void cprint(tm_obj o){
	_tm_print(o, 4, 0);
}

void cprintln_show_special(tm_obj o){
	_tm_print(o, 4, 1);
	puts("");
}

void cprintln(tm_obj o){
	cprint(o);
	puts("");
}


tm_obj tm_print(tm_obj params){
	int i = 0;
	tm_list* list = get_list(params);
	for(i = 0; i < list->len; i++ ){
		cprint(list->nodes[i]);
		if( i + 1 != list->len){
			putchar(' ');
		}
	}
	putchar('\n');
	return tm->none;
}

tm_obj _tm_format(char* fmt, va_list ap){
	int i;
	int len = strlen(fmt);
	tm_obj nstr = str_new("", 0);
	int  templ = 0;
	char* start = fmt;
	for(i = 0; i < len; i++){
		if( fmt[i] == '@' ){
			if( templ > 0){
				tm_obj txt = str_new(start, templ);
				nstr = tm_add(nstr, txt);
				templ = 0;
			}
			tm_obj v = va_arg(ap, tm_obj);
			nstr = tm_add(nstr, _tm_str(v));
			start = fmt + i + 1;
		}else{
			templ++;
		}
	}
	if( templ > 0){
		tm_obj txt = str_new(start, templ);
		nstr = tm_add(nstr, txt);
	}
	return nstr;
}

tm_obj tm_format(char* fmt, ...){
	va_list a; 
	va_start(a,fmt);
	tm_obj  v = _tm_format(fmt, a);
	va_end(a);
	return v;
}
void _tm_printf(char* fmt, va_list ap){
	int i;
	int len = strlen(fmt);
	char*s = fmt;
	for(i = 0; i < len; i++){
		if( s[i] == '@' ){
			tm_obj v = va_arg(ap, tm_obj);
			cprint(v);
		}else{
			putchar(s[i]);
		}
	}
}

void tm_printf(char* fmt, ...){
	va_list a; 
	va_start(a,fmt);
	_tm_printf(fmt, a);
	va_end(a);
}

tm_obj tm_sleep( tm_obj p){
	int i = 0;
	tm_obj time = get_arg(p, 0, TM_NUM);
	int t = get_num(time);
#ifdef _WINDOWS_H
	Sleep(t);
#else
	sleep(t);
#endif
	return tm->none;
}

tm_obj tm_input(tm_obj p){
	int i = 0;
	if( list_len(p) > 0){
		for(i = 0; i < list_len(p); i++){
			cprint(get_list(p)->nodes[i]);
		}
	}
	char buf[2048];
	fgets(buf, 2048, stdin);
	return str_new(buf, strlen(buf));
}

tm_obj tm_int(tm_obj p){
	tm_obj v = get_arg(p, 0, -1);
	if( v.type == TM_NUM ){
		return number_new( (int) get_num(v) );
	}else if( v.type == TM_STR ){
		return number_new( (int)atof(get_str(v)) );
	}
	tm_raise("tm_int: @ can not parse to int ", v);
	return tm->none;
}

tm_obj tm_float( tm_obj p){
	tm_obj v = get_arg(p, 0, -1);
	if( v.type == TM_NUM ){
		return v;
	}else if( v.type == TM_STR ){
		return number_new( atof(get_str(v)));
	}
	tm_raise("tm_float: @ can not parse to float", v);
	return tm->none;
}

tm_obj tm_range( tm_obj p){
	tm_obj N = get_arg(p, 0, TM_NUM );
	int n = (int) get_num( N );
	if( n < 0 ){
		tm_raise("tm_range: range size must be bigger than zero");
	}

	tm_obj list = list_new(n);
	int i = 0;
	for(i = 0; i < n; i++){
		list_append( get_list(list), number_new(i));
	}
	return list;
}

tm_obj _merge(tm_obj des, tm_obj src){
	if( des.type != src.type || des.type != TM_DCT)
		tm_raise("merge: can not merge @ and @", des, src);
	tm_obj k,v;
	dict_iter_init(get_dict(src));
	while( dict_inext(get_dict(src), &k, &v)){
		tm_set( des, k, v);
	}
	return des;
}

/* import file */
tm_obj tm_import( tm_obj p){
	tm_obj name = get_arg(p, 0, TM_STR);
	tm_obj arg1 = get_arg(p, 1, TM_STR);
	tm_obj mod;
	// puts("call import");
	// name = tm_add( name, obj_mod_ext);
	if(_tm_has( tm->modules, name) ) {
		// puts("enter");
		// cprint(tm->modules);
	 	mod = tm_get( tm->modules, name);
	}else{
		mod = dict_new();
		// printf("new a module with empty value\n");
	}
	if( tm_eq(arg1, obj_star) ){
		// printf("import *\n");
		_merge(tm->frames[tm->cur].globals,  mod);
	}
	return obj_none;
}


/* get globals */
tm_obj tm_globals(tm_obj p){
	return tm->frames[tm->cur].globals;
}

/* get object type */
tm_obj _tm_type( tm_obj o){
	switch( o.type ){
	case TM_NUM: return str_new("number", -1);
	case TM_STR: return str_new("string", -1);
	case TM_DCT: return str_new("dict", -1);
	case TM_LST: return str_new("list", -1);
	case TM_FNC: return str_new("function", -1);
	case TM_NON: return str_new("None", -1);
	}
	return str_new("unknown",-1);
}

/* return the type and value of a object */
tm_obj _obj_info(tm_obj o){
	tm_obj str = _tm_type(o);
	tm_obj com = str_new(":", -1);
	str = tm_add(str, com);
	return tm_add(str, _tm_str(o));
}

tm_obj tm_exit( tm_obj p){
	longjmp(tm->buf, 2);
	return obj_none;
}
