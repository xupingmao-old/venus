
#include "tm.h"

tm_obj str_new(char *s , int size)
{
	tm_str* str = tm_alloc( sizeof( tm_str ));
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
		if( size == 0 ){
			str->value = "";
			str->len = 0;
		}else{
			str->len = strlen(s);
			str->value = s;
		}
	}
	tm_obj v;
	v.type = TM_STR;
	v.value.str = str;
	return gc_track(v);
}

void str_free( tm_str *str){
#if DEBUG_GC
int old = tm->allocated_mem;
printf("free string %p...\n", str);
#endif
	if( str->inHeap ){
		tm_free( str->value, str->len + 1);
	}
	tm_free(str, sizeof(tm_str));
#if DEBUG_GC
int _new = tm->allocated_mem;
printf("free string , %d => %d, freed %d B\n", old, _new, old - _new);
#endif
}

int _str_find( tm_str* s1, tm_str* s2, int start){
	char* ss1 = s1->value;
	char* ss2 = s2->value;
	char* p = strstr( ss1 + start, ss2);
	if( p == NULL ) return -1;
	// printf("%d[%s -> %s]\n",start,ss1 + start, p );
	return p - ss1;
}


tm_obj str_find( tm_obj params){
	tm_obj self = get_arg( params, 0, TM_STR);
	tm_obj str  = get_arg( params, 1, TM_STR);
	return number_new( _str_find(self.value.str, str.value.str, 0));
}

tm_obj _str_substring( tm_str* str, int start, int end){
	start = start >= 0 ? start : start + str->len;
	end = end >= 0 ? end : end + str->len;
	int max_end = str->len - 1;
	end = max_end < end ? max_end : end;
	int len = end - start + 1;
	if( len <= 0)
		return str_new("", 0);
	tm_obj new_str = str_new( NULL, len);
	char* s = get_str(new_str);
	int i;for(i = start; i <= end; i++){
		*(s++) = str->value[i];
	}
	return new_str;
}

tm_obj str_substring(tm_obj params){
	tm_obj self = get_arg( params, 0, TM_STR);
	tm_obj start = get_arg(params, 1, TM_NUM);
	tm_obj end = get_arg( params, 2, TM_NUM);
	return _str_substring(self.value.str, get_num(start), get_num(end) );
}

tm_obj str_upper(tm_obj params){
	tm_obj self = get_arg(params, 0, TM_STR);
	int i;
	char*s = get_str(self);
	int len = str_len(self);
	tm_obj nstr = str_new(NULL, len);
	char*news = get_str(nstr);
	for(i = 0; i < len; i++){
		if ( s[i] >= 'a' && s[i] <= 'z'){
			news[i] = s[i] + 'A' - 'a';
		}else{
			news[i] = s[i];
		}
	}
	return nstr;
}

tm_obj str_isupper(tm_obj params){
	tm_obj self = get_arg( params, 0, TM_STR);
	int i;
	for(i = 0; i < get_str_len(self); i++){
		char c = get_str(self)[i];
		if( c >= 'A' && c <= 'Z' ){

		}else{
			return obj_false;
		}
	}
	return obj_true;
}

tm_obj str_lower( tm_obj params){
	tm_obj self = get_arg(params, 0, TM_STR);
	int i;
	char*s = get_str(self);
	int len = str_len(self);
	tm_obj nstr = str_new(NULL, len);
	char*news = get_str(nstr);
	for(i = 0; i < len; i++){
		if ( s[i] >= 'A' && s[i] <= 'Z'){
			news[i] = s[i] + 'a' - 'A';
		}else{
			news[i] = s[i];
		}
	}
	return nstr;
}


tm_obj str_replace(tm_obj params){
	tm_obj self = get_arg(  params, 0, TM_STR);
	tm_obj src = get_arg( params, 1, TM_STR);
	tm_obj des = get_arg( params, 2, TM_STR);

	tm_obj nstr = str_new("", 0);
	int pos = _str_find( self.value.str, src.value.str, 0);
	int lastpos = 0;
	// puts("step0");
	// cprintln_show_special(src);
	// cprintln_show_special(des);
	while (pos != -1 && pos < get_str_len(self)){
		// printf("%d\n",pos );
		if( pos != 0)
			nstr = tm_add(nstr, _str_substring(self.value.str, lastpos, pos - 1));
		nstr = tm_add(nstr, des);
		// Sleep(1000);
		lastpos = pos + get_str_len(src);
		pos = _str_find( self.value.str, src.value.str, lastpos);
		// printf("lastpos = %d\n", lastpos);
	}
	// puts("step1");
	nstr = tm_add( nstr, _str_substring(self.value.str, lastpos, -1 ));
	// puts("step2");
	// cprintln_show_special(self);
	// cprintln_show_special(nstr);
	// cprintln(nstr);
	return nstr;
}

tm_obj str_split(tm_obj params){
	tm_obj self = get_arg( params, 0, TM_STR);
	tm_obj pattern = get_arg(params, 1, TM_STR);
	if( get_str_len(pattern) == 0){
		/* currently return none */
		return obj_none;
	}
	int pos = _str_find( self.value.str, pattern.value.str, 0);
	int lastpos = 0;
	tm_obj nstr = str_new("", 0);
	tm_obj list = list_new(10);
	while ( pos > 0 && pos < get_str_len(self)){
		tm_obj str = _str_substring(self.value.str, lastpos, pos - 1);
		list_append( get_list(list), str);
		lastpos = pos + get_str_len(pattern);
		pos = _str_find(self.value.str, pattern.value.str, lastpos);
	}
	list_append( get_list(list), _str_substring(self.value.str, lastpos, -1));
	return list;
}


/*void find_test(char*self, char* src, int p){
	tm_str s0, s1;
	s0.value = self;s1.value = src;
	printf("%s.find(%s) = %d \n", self, src, _str_find(&s0, &s1, p));
}

int main(){
	find_test("wwhello", "he", 0);
	find_test("wwhello", "he", 2);
	return 0;
}*/
