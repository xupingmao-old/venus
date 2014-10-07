
#include "tm.h"

tm_obj string_new(char *s , int size)
{
	tm_string* str = tm_alloc( sizeof( tm_string ));
	if( size > 0 ){
		str->inHeap = 1;
		str->value = tm_alloc( size + 1);
		str->len = size;
		if( s != NULL ){
			memcpy(str->value, s, size);
		}
		str->value[size] = '\0'; 
	}else{
		str->inHeap = 0;
		str->len = strlen(s);
		str->value = s;
	}
	tm_obj v;
	v.type = TM_STR;
	v.value.str = str;
	return gc_track(v);
}

void string_free( tm_string *str){
	if( str->inHeap ){
		tm_free( str->value, str->len + 1);
	}
	tm_free(str, sizeof(tm_string));
}


tm_obj string_find( tm_obj params){
	tm_obj self = get_arg( params, 0, TM_STR);
	tm_obj str  = get_arg( params, 1, TM_STR);
	char* self_v = get_str(self);
	char* str_v  = get_str(self);
	char* p = strstr(self_v, str_v);
	if( p == NULL ){
		return tm_number(-1);
	}
	return tm_number(self_v - p);
}


tm_obj blt_string_substring(tm_obj params){
	tm_obj self = get_arg( params, 0, TM_STR);
	tm_obj start = get_arg(params, 1, TM_NUM);
	tm_obj end = get_arg( params, 2, TM_NUM);

	int start_i = get_num(start);
	int end_i = get_num(end);

	int len = end_i - start_i + 1;
	if( len < 0 )
		tm_raise("substring: index overflow");

	tm_obj nstr = string_new(NULL, len);
	int src_len = get_str_len(self);
	char*src = get_str(self);
	char*s = get_str(nstr);

	int i, j;for(i = start_i, j = 0; i < src_len && i <= end_i;i++, j++){
		s[j] = src[i];
	}
	s[j] = '\0';
	return nstr;
}

tm_obj string_get(tm_string* str, int n){
	if( n < 0 || n >= str->len){
		tm_raise("string_get: index overflow");
	}
	return tm->chars[str->value[n]];
}

tm_obj string_upper(tm_obj params){
	tm_obj self = get_arg(params, 0, TM_STR);
	int i;
	char*s = get_str(self);
	int len = str_len(self);
	tm_obj nstr = string_new(NULL, len);
	char*news = get_str(nstr);
	for(i = 0; i < len; i++){
		if ( s[i] >= 'a' && s[i] <= 'z'){
			news[i] = s[i] + 'A' - 'a';
		}else{
			news[i] = s[i];
		}
	}
	news[len] = '\0';
	return nstr;
}

/*
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
	char* nstr = tm_alloc(len + 1);

	int i;
	char*s = nstr;
	int cap = len;
	int nlen = 0;
	for(i = 0; i < len; i++){
		if( nlen >= len){
			cap = cap * 3 / 2 + dl;
			free(nstr);
			nstr = tm_alloc(cap + 1);
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
	return string_new_(nstr, nlen);
}
*/