
#include "tm.h"

tm_obj string_new(tm_vm* tm, char *s )
{
	tm_string* str = tm_alloc( tm, sizeof( tm_string ));
	str->value = s;
	str->len = strlen(s);
	str->inHeap = 0;
	tm_obj v;
	v.type = TM_STR;
	v.value.str = str;
	return gc_track(tm, v);
}

// 不建议使用， 内容没有确定， gc无法判断能不能删除。
tm_obj string_alloc( tm_vm* tm, int len){
	tm_string* str = tm_alloc( tm, sizeof(tm_string) + len + 1);
	str->len = len;
	str->inHeap = 1;
	tm_obj v;
	v.type = TM_STR;
	v.value.str = str;
/*
#if DEBUG_GC
	printf("string_new\n");
#endif
*/
	return gc_track(tm, v);
}

tm_obj string_new_(tm_vm* tm, char *s, int len )
{
	tm_string* str = tm_alloc( tm, sizeof( tm_string ));
	str->value = s;
	str->len = len;
	str->inHeap = 1;
	tm_obj v;
	v.type = TM_STR;
	v.value.str = str;
/*
#if DEBUG_GC
	printf("string_new\n");
#endif
*/
	return gc_track(tm, v);
}


void string_free( tm_vm* tm, tm_string *str){
/*
#if DEBUG_GC
	if( str->inHeap)
		printf("free string: '%s', free %d B\n", str->value, str->len + 1 + sizeof(tm_string));
	else
		printf("free static string: '%s', free %d B\n", str->value, sizeof(tm_string));
#endif
*/
	if( str->inHeap ){
		tm->allocated_mem -= str->len + 1;
		free( str->value);
	}
	free(str);
	tm->allocated_mem -= sizeof(tm_string);
}


tm_obj string_new2(tm_vm* tm, char *s, tm_obj v){
	tm_obj a = string_new(tm, s);
	tm_obj b = tm_str(tm, v);
	return tm_add(tm, a, b);
}

tm_obj string_find(tm_vm* tm, tm_obj params){
	tm_obj self = get_arg(tm, params, 0, TM_STR);
	tm_obj str  = get_arg(tm, params, 1, TM_STR);
	char* self_v = get_str(self);
	char* str_v  = get_str(self);
	char* p = strstr(self_v, str_v);
	if( p == NULL ){
		return tm_number(-1);
	}
	return tm_number(self_v - p);
}

tm_obj string_substr(tm_vm* tm, tm_obj params){
	tm_obj self = get_arg(tm, params, 0, TM_STR);
	tm_obj start = get_arg(tm, params, 1, TM_NUM);
	tm_obj len = get_arg(tm, params, 2, TM_NUM);

	int start_i = get_num(start);
	int len_i = get_num(len);

	int osize = get_str_len(self);
	start_i = start_i > 0 ? start_i : start_i + osize;

	if( start_i >= osize || start_i < 0 ){
		return tm->empty_str;
	}

	int nsize = get_num( len );
	if( nsize <= 0)
		return tm->empty_str;

	tm_obj nstr = string_alloc(tm, nsize);
	memcpy( get_str(nstr), get_str(self) + start_i, nsize);
	get_str(nstr)[nsize-1] = '\0';
	return nstr;
}

tm_obj string_get(tm_vm* tm, tm_string* str, int n){
	if( n < 0 || n >= str->len){
		_tm_raise("string_get: index overflow");
	}
	return tm->chars[str->value[n]];
}

tm_obj string_upper(tm_vm* tm, tm_obj params){
	tm_obj self = get_arg(tm,params, 0, TM_STR);
	int i;
	char*s = get_str(self);
	int len = str_len(self);
	char* news = tm_alloc( tm, len + 1);
	for(i = 0; i < len; i++){
		if ( s[i] >= 'a' && s[i] <= 'a'){
			news[i] = s[i] + 'A' - 'a';
		}else{
			news[i] = s[i];
		}
	}
	news[len] = '\0';
	return string_new_(tm, s, len);
}

tm_obj string_replace(tm_vm* tm, tm_obj params){
	tm_obj self = get_arg( tm, params, 0, TM_STR);
	tm_obj src = get_arg( tm, params, 1, TM_STR);
	tm_obj des = get_arg( tm, params, 2, TM_STR);

	char* src_s = get_str(src);
	char* des_s = get_str(des);

	int sl = str_len(src);
	int dl = str_len(des);

	char* self_s = get_str(self);

	int len = str_len( self );
	char* nstr = tm_alloc( tm, len + 1);

	int i;
	char*s = nstr;
	int cap = len;
	int nlen = 0;
	for(i = 0; i < len; i++){
		if( nlen >= len){
			cap = cap * 3 / 2 + dl;
			free(nstr);
			nstr = tm_alloc( tm, cap + 1);
			s = nstr + nlen;
		}
		if( strncmp( s, src_s, sl) == 0){
			memcpy(s, des_s, dl);
			s+= dl;
			nlen+=dl;
		}else{
			s++;
			nlen++;
		}
	}
	*s = '\0';
	return string_new_(tm, nstr, nlen);
}
