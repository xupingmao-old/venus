#include "tm.h"

tm_obj _tm_call(tm_vm* tm, tm_obj func, tm_obj params){
	return tm->none;
}


tm_obj tm_call( tm_obj func, tm_obj params){
	tm_func* f = func.value.func;
	// check if function is a method ?
	if( f->self.type != TM_NON){
		list_insert( get_list(params), 0, f->self);
		tm_frame* frame = frame_new(f);
		//tm_eval(frame);
	}
	if( f->native_func != NULL ){
		return f->native_func(params);
	}
	return tm->none;
}

/*tm_obj type_method(tm_vm* tm, tm_obj self, tm_obj k){
	if( k.type != TM_STR){
		tm_raise("type_method: require string");
	}
	tm_obj fnc;
	tm_obj methods;
	switch( self.type ){
	case TM_STR:{
		fnc = tm_get( tm->string_methods, k);break;
	case TM_LST:
		fnc = tm_get(tm, tm->list_methods, k);break;
	default:
		tm_raise("type_method: type has no such method");
	}
	fnc.value.func->self = self;
	return fnc;
	}
}
*/

tm_obj _tm_str(  tm_obj a){
	switch( a.type ){
	case TM_STR:
		return a;
	case TM_NUM:
	{
		char s[20];
		sprintf(s, "%g", get_num(a));
		return string_new(s, strlen(s));
	}
	case TM_LST:
		return string_new("<list>", 0);
	case TM_MAP:
		return string_new("<dict>", 0);
	case TM_FNC:
		return string_new("<function>", 0);
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
			return string_new(get_str(o), len);
		}
	case TM_LST:
		{
			tm_obj list = list_new( list_len(o));
			int i;
			for(i = 0; i < list_len(o); i++){
				list_append( get_list(o), o.value.list->nodes[i]);
			}
			return list;
		}
	}
	return tm->none;
}

tm_obj _tm_len(tm_obj o){
	switch(o.type){
	case TM_STR:return tm_number( get_str_len(o) );
	case TM_LST:return tm_number( list_len(o));
	case TM_MAP:return tm_number( map_len(o));
	}
	tm_raise("tm_len: @ has no attribute len", o);
	return tm->none;
}

tm_obj tm_len(tm_obj p){
	tm_obj o = get_arg(p, 0, -1);
	return _tm_len(o);
}

inline
int tm_get_int(tm_obj v){
	if( v.type != TM_NUM){
		tm_raise( "tm_get_int:@ is not a number", v );
	}
	return (int)v.value.dv;
}

void tm_set( tm_obj self, tm_obj k, tm_obj v){
	switch( self.type ){
	case TM_LST:
	{
		int n = tm_get_int( k);
		list_set( self.value.list, n, v);
	}
	break;
	case TM_MAP: map_set(self.value.map, k, v);break;
	}
}

tm_obj tm_get(tm_obj self, tm_obj k){
	tm_obj v;
	switch( self.type){
		case TM_LST: {
			int n = tm_get_int(k);
			return list_get( self.value.list, n);
		}
		case TM_MAP:
			if( map_iget(self.value.map, k, &v)){
				return v;
			} goto error;
			break;
		case TM_STR: {
			int kt = k.type;
			if( k.type == TM_STR ){
				int n = tm_get_int(k);
				return string_get(self.value.str, n);
			}
		}
		case TM_FNC:
			if( k.type == TM_STR && strequals(get_str(k), "code")){
				return get_func( self )->code;
			}
	}
	error:
	tm_raise("tm_get: keyError " );
	return tm->none;
}

tm_obj tm_sub( tm_obj a, tm_obj b){
	if( a.type != b.type){
		tm_raise("tm_sub: can not add @ and @", a, b);
	}
	if( a.type == TM_NUM){
		return tm_number(a.value.num - b.value.num);
	}
	tm_raise("tm_sub: can not sub @ and @", a, b);
}

tm_obj tm_add(  tm_obj a, tm_obj b){
	if( a.type != b.type ){
		tm_raise("tm_add: different types : @ @", a, b);
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
			tm_obj des = string_new(NULL, len);
			char*s = get_str(des);
			memcpy(s,      sa, la);
			memcpy(s + la, sb, lb);
			return des;
		}
		case TM_LST:
		{
			tm_list* list = list_join(get_list(a), get_list(b) );
			a.value.list = list;
			return gc_track(a);
		}
	}
	tm_raise("tm_add: can not add @ and @", a, b);
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
