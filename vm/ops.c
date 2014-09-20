#include "tm.h"

tm_obj _tm_call(tm_vm* tm, tm_obj func, tm_obj params){
	return tm->none;
}


tm_obj tm_call( tm_vm* tm, tm_obj func, tm_obj params){
	tm_func* f = func.value.func;
	switch(func.type){
	case TM_NATIVE_FNC:
		return f->native_func(tm, params);
	case TM_USER_FNC:
		return _tm_call(tm, func, params);
	case TM_METHOD:
		list_insert(tm, get_list(params), 0, func.value.func->self);
		return _tm_call(tm, func, params);
	case TM_NATIVE_METHOD:
		list_insert(tm, get_list(params), 0, func.value.func->self);
		return f->native_func(tm, params);
	default:
		_tm_raise("tm_call: not callable");
	}
	return tm->none;
}

tm_obj type_method(tm_vm* tm, tm_obj self, tm_obj k){
	if( k.type != TM_STR){
		_tm_raise("type_method: require string");
	}
	tm_obj fnc;
	tm_obj methods;
	switch( self.type ){
	case TM_STR:{
		fnc = tm_get( tm, tm->string_methods, k);break;
	case TM_LST:
		fnc = tm_get(tm, tm->list_methods, k);break;
	default:
		_tm_raise("type_method: type has no such method");
	}
	fnc.value.func->self = self;
	return fnc;
	}
}


tm_obj tm_str( tm_vm* tm, tm_obj a){
	switch( a.type ){
	case TM_STR:
		return a;
	case TM_NUM:
	{
		char* s = tm_alloc( tm, 20);
		sprintf(s, "%g", get_num(a));
		return string_new_(tm, s, strlen(s));
	}
	case TM_LST:
		return string_new(tm, "<list>");
	case TM_DCT:
		return string_new(tm, "<dict>");
	case TM_USER_FNC:
	case TM_NATIVE_FNC:
		return string_new(tm, "<function>");
	case TM_METHOD:
		return string_new(tm, "<method>");
	}
	return tm->none_str;
}

tm_obj tm_copy(tm_vm* tm, tm_obj o){
	switch( o.type ){
	case TM_NUM:
		return o;
	case TM_STR:
		{
			int len = str_len(o);
			char* s = tm_alloc( tm, len + 1);
			memcpy(s, get_str(o), len);
			s[len] = '\0';
			return string_new_(tm, s, len);
		}
	case TM_LST:
		{
			tm_obj list = list_new(tm);
			int i;
			for(i = 0; i < list_len(o); i++){
				list_push( tm, list.value.list, o.value.list->nodes[i]);
			}
			return list;
		}
	}
	return tm->none;
}

tm_obj tm_len(tm_vm* tm, tm_obj p){
	tm_obj o = list_get(tm , get_list(p), 0 );
	switch(o.type){
	case TM_STR:return tm_number( get_str_len(o) );
	case TM_LST:return tm_number( list_len(o));
	case TM_DCT:return tm_number( dict_len(o));
	}
	_tm_raise("tm_len: %o has no len()");
	return tm->none;
}

__inline__
int tm_get_int(tm_vm* tm, tm_obj v){
	if( v.type != TM_NUM){
		tm->error = string_new2(tm, "tm_get_int: not a number ==> ", v);
		tm_raise( tm );
	}
	return (int)v.value.dv;
}

void tm_set( tm_vm* tm, tm_obj self, tm_obj k, tm_obj v){
	switch( self.type ){
	case TM_LST:
	{
		int n = tm_get_int( tm, k);
		list_set( tm, self.value.list, n, v);
	}
	break;
	case TM_DCT:
		dict_set(tm, self.value.dict, k, v);
		break;
	}
}

tm_obj tm_get(tm_vm* tm, tm_obj self, tm_obj k){
	switch( self.type){
		case TM_LST: {
			int n = tm_get_int(tm, k);
			return list_get(tm, self.value.list, n);
		}
		case TM_DCT:
			return dict_get(tm, self.value.dict, k);
		case TM_STR: {
			int kt = k.type;
			if( k.type == TM_STR ){
				int n = tm_get_int(tm, k);
				return string_get(tm, self.value.str, n);
			}
			return type_method(tm, self, k);
		}
	}
	return tm->none;
}

tm_obj tm_add( tm_vm* tm, tm_obj a, tm_obj b){
	if( a.type != b.type ){
		tm->error = string_new(tm, "tm_add: different types");
		tm_raise(tm);
	}
	switch( a.type ){
	case TM_NUM:
		a.value.dv += b.value.dv;
		return a;
	case TM_STR:
	{
		char* sa = get_str(a);
		char* sb = get_str(b);
		int la = get_str_len(a);
		int lb = get_str_len(b);
		int len = la + lb;
		char* s = tm_alloc( tm, len + 1);
		memcpy(s,      sa, la);
		memcpy(s + la, sb, lb);
		s[ len ] = '\0';
		return string_new_( tm, s, len);
	}
	case TM_LST:
	{
		tm_list* list = list_join(tm, get_list(a), get_list(b) );
		a.value.list = list;
		return gc_track(tm, a);
	}
	}
	_tm_raise("tm_add: type can not add");
	return tm->none;
}

int tm_eq(tm_obj a, tm_obj b){
	if( a.type != b.type ) return 0;
	switch( a.type ){
	case TM_NUM:
		return a.value.dv == b.value.dv;
	case TM_STR:
	{
		char* sa = get_str(a);
		char* sb = get_str(b);
		return sa == sb || strcmp(sa, sb) == 0;
	}
	case TM_LST:
	{
		int i;
		int len = get_list(a)->len;
		tm_obj* nodes1 = get_list(a)->nodes;
		tm_obj* nodes2 = get_list(b)->nodes;
		for(i = 0; i < len; i++){
			if( !tm_eq(nodes1[i], nodes2[i]) ){
				return 0;
			}
		}
		return 1;
	}
	}
	return 0;
}

#define tm_equals tm_eq