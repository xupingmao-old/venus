#include "tm.h"

void _tm_print(tm_obj o, int depth){
	depth--;
	if( depth < 0 )
		return;
	switch( o.type ){
	case TM_STR:
		{
			int i;int len = str_len(o);
			char *s = get_str(o);
			for(i = 0; i < len; i++){
				if(s[i] == '\0'){
					putchar('\\');
					putchar('0');
				}else{
					putchar(s[i]);
				}
			}
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
			_tm_print(list->nodes[i], depth);
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
			_tm_print(k, depth);
			putchar(':');
			_tm_print(v, depth);
			if( i != len -1 ) putchar(',');
		}
		putchar('}');
		break;
	}
	// case TM_DCT:dict_print(get_dict(o));break;
	case TM_FNC:
		if( get_func(o)->self.type != TM_NON){
			printf("<method %x>", get_func(o));
		}else{
			printf("<function %x>", o.value.func);
		}
		break;
	case TM_NON:
		printf("None");break;
	}
}

void cprint(tm_obj o){
	_tm_print(o, 4);
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
		tm_print(p);
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