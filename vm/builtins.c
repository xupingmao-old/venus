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
		printf("%lf", get_double(o));
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
	/*case TM_MAP:
	{
		int i = 0;
		tm_obj k,v;
		tm_map* map = get_map( o );
		putchar('{');
		while( map_inext(map, &k, &v)){
			_tm_print(k, depth);
			putchar(':');
			_tm_print(v, depth);
			putchar(',');
		}
		putchar('}');
		break;
	}*/
	case TM_MAP:map_print(get_map(o));break;
	case TM_FNC:
		printf("<function %x>", o.value.func);
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
	tm_obj nstr = string_new("", 0);
	for(i = 0; i < len; i++){
		if( fmt[i] == '@' ){
			tm_obj v = va_arg(ap, tm_obj);
			nstr = tm_add(nstr, _tm_str(v));
		}else{
			nstr = tm_add(nstr, string_new( &fmt[i], 1));
		}
	}
	return nstr;
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
	return string_new(buf, strlen(buf));
}
