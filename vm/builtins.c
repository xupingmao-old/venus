#include "tm.h"

void __tm_print(tm_obj o, int depth){
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
			__tm_print(list->nodes[i], depth);
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
		tm_list* keys = get_keys(o);
		tm_list* vals = get_vals(o);
		printf("{");
		for(i = 0; i < keys->len; i++){
			__tm_print(keys->nodes[i], depth);
			putchar(':');
			__tm_print(vals->nodes[i], depth);
			if( i+1 != keys->len){
				putchar(',');
			}
		}
		printf("}");
		break;
	case TM_FNC:
	case TM_USER_FNC:
	case TM_NATIVE_FNC:
		printf("<function %x>", o.value.func);
		break;
	case TM_METHOD:
	case TM_NATIVE_METHOD:
		printf("<method %x>", o.value.func);
		break;
	case TM_NON:
		printf("None");break;

	case TM_MAP:
		map_print(o.value.map);break;
	}
	}
}

void cprint(tm_obj o){
	__tm_print(o, 4);
}

void _tm_print(tm_obj o){
	__tm_print(o, 4);
	putchar('\n');
}


tm_obj tm_print(tm_vm* tm, tm_obj params){
	int i = 0;
	tm_list* list = get_list(params);
	for(i = 0; i < list->len; i++ ){
		_tm_print(list->nodes[i]);
	}
	putchar('\n');
	return tm->none;
}

tm_obj tm_sleep(tm_vm* tm, tm_obj p){
	int i = 0;
	tm_obj time = get_arg(tm, p, 0, TM_NUM);
	int t = get_num(time);
#ifdef _WINDOWS_H
	Sleep(t);
#else
	sleep(t);
#endif
	return tm->none;
}

tm_obj tm_input(tm_vm* tm, tm_obj p){
	int i = 0;
	if( list_len(p) > 0){
		tm_print(tm, p);
	}
	char buf[2048];
	fgets(buf, 2048, stdin);
	int len = strlen(buf);
	char* s = tm_alloc(tm, len+1);
	strcpy(s, buf);
	return string_new_(tm, s, len);
}
