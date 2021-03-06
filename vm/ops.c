#include "tm.h"


/*tm_obj type_method(tm_vm* tm, tm_obj self, tm_obj k){
	if( k.type != TM_STR){
		tm_raise("type_method: require str");
	}
	tm_obj fnc;
	tm_obj methods;
	switch( self.type ){
	case TM_STR:{
		fnc = tm_get( tm->str_methods, k);break;
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



void tm_set( tm_obj self, tm_obj k, tm_obj v){
	switch( self.type ){
	case TM_LST:
	{
        if( TM_NUM != k.type){
            tm_raise("tm_set(), expect a number but see @", _tm_type(k));
        }
		int n = get_num(k);
		list_set( get_list(self), n, v);
	}return;
	case TM_DCT: dict_set( get_dict(self), k, v);return;
	}
	tm_raise(" tm_set: @[@] = @", self, k, v );
}

tm_obj tm_get(tm_obj self, tm_obj k){
	tm_obj v;
	switch( self.type){
		case TM_STR:{
			if( k.type == TM_NUM ){
				int n = get_num(k);		
				if (n < 0) n += get_str_len(self);	
				if( n >= get_str_len(self) || n < 0)
					tm_raise("tm_get: index overflow");
				//tm_printf("str = @, len = @, index = @\n", self,number_new(get_str_len(self)),  k);
				unsigned char c = get_str(self)[n];
                //printf("c = %d\n", c);
				return __chars__[c];
			}else if( dict_iget( get_dict(str_class), k , &v) ){
				return method_new(v, self);
			}
		}
		case TM_LST: {
			if( k.type == TM_NUM ){
				return list_get( self.value.list, get_num(k));
			}else if( dict_iget(get_dict(list_class), k, &v) ){
				return method_new( v, self);
			}
			break;
		}
		case TM_DCT:
			if( dict_iget( get_dict(self), k, &v)){
				return v;
			}else if(dict_iget(get_dict(dict_class), k, &v)){
				return method_new( v, self);
			}
			break;
		case TM_FNC:
            /*
			if( k.type == TM_STR && strequals(get_str(k), "code")){
				return get_func( self )->code;
			}*/
            break;
	}
    // tm_printf_only_type("@", self);
	// cprintln(self);
	tm_raise("tm_get: keyError @, self = @ ", _tm_type(k), _tm_type(self) );
	return obj_none;
}


tm_obj tm_sub( tm_obj a, tm_obj b){
	if( a.type == b.type ){
		if( a.type == TM_NUM){
			return tm_number(a.value.num - b.value.num);
		}
	}
	tm_raise("tm_sub: can not sub @ and @", a, b);
}

tm_obj tm_add(  tm_obj a, tm_obj b){
	if( a.type == b.type ){
		switch( a.type ){
			case TM_NUM:
				get_num(a) += get_num(b);
				return a;
			case TM_STR:
			{
				char* sa = get_str(a);
				char* sb = get_str(b);
				int la = get_str_len(a);
				int lb = get_str_len(b);
				if( la == 0){
					return b;
				}
				if( lb == 0){
					return a;
				}
				int len = la + lb;
				tm_obj des = str_new(NULL, len);
				char*s = get_str(des);
				memcpy(s,      sa, la);
				memcpy(s + la, sb, lb);
				return des;
			}
			case TM_LST:
			{
				return list_join(get_list(a), get_list(b) );
			}
		}
	}
    tm_raise("tm_add: can not add %t and %t", (a),(b));
	return obj_none;
}


int tm_eq(tm_obj a, tm_obj b){
	if( a.type != b.type ) return 0;
	switch( a.type ){
		case TM_NUM:
			return get_num(a) == get_num(b);
		case TM_STR:
		{
			char* sa = get_str(a);
			char* sb = get_str(b);
			return sa == sb || ( get_str_len(a) == get_str_len(b) && 
				strncmp(sa, sb, get_str_len(a)) == 0 );
		}
		case TM_LST:
		{
			if( get_list(a) == get_list(b)) return 1;
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
		case TM_NON:return 1;
		default: tm_raise("tm_eq(): not supported type %d", a.type);
	}
	return 0;
}

tm_obj t_tm_equals(tm_obj a, tm_obj b){
	return number_new( tm_eq(a,b));
}

tm_obj tm_not_equals( tm_obj a, tm_obj b){
	return number_new(!tm_eq(a,b));
}


#define tm_comp( fnc_name, op) tm_obj fnc_name(tm_obj a, tm_obj b) {      \
	if( a.type != b.type )                             \
		tm_raise( #fnc_name"(): can not compare [@] and [@]", _obj_info(a), _obj_info(b));                 \
	switch(a.type){                      \
		case TM_NUM: return number_new( get_num(a) op get_num(b) ); \
		case TM_STR: return  number_new(strcmp( get_str(a) , get_str(b)) op 0); \
		default : tm_raise(#fnc_name"() not support yet"); \
	}                           \
}

#define tm_comp2( fnc_name, op) int fnc_name(tm_obj a, tm_obj b) {      \
	if( a.type != b.type )                             \
		tm_raise( #fnc_name"(): can not compare [@] and [@]", _obj_info(a), _obj_info(b));                 \
	switch(a.type){                      \
		case TM_NUM: return  get_num(a) op get_num(b); \
		case TM_STR: return  strcmp( get_str(a) , get_str(b)) op 0; \
		default : tm_raise(#fnc_name"() not support yet"); \
	}                           \
    return 0;\
}

tm_comp( tm_lt, < );
tm_comp( tm_gt, >);
tm_comp( tm_lteq, <=);
tm_comp( tm_gteq, >=);

tm_comp2( tm_bool_lt, <);
tm_comp2( tm_bool_gt, >);
tm_comp2( tm_bool_lteq, <= );
tm_comp2( tm_bool_gteq, >= );
#define tm_bool_eqeq tm_eq
#define tm_bool_noteq !tm_eq

tm_obj tm_mul( tm_obj a, tm_obj b){
	if( a.type == b.type && a.type == TM_NUM){
		return number_new( get_num(a) * get_num(b) );
	}
	if( a.type == TM_NUM && b.type == TM_STR){
		tm_obj temp = a;
		a = b;
		b = temp;
	}
	if( a.type == TM_STR && b.type == TM_NUM){
		if( get_str_len(a) == 0) return a;
		tm_obj str = str_new(NULL, get_int(b) * get_str_len(a) );
		char* s = get_str(str);
		int i;for(i = 0; i < get_str_len(str) ; i+= get_str_len(a)){
			memcpy( s + i , get_str(a), get_str_len(a));
		}
		return str;
	}
	tm_raise("tm_mul: can not mul  @ and @", a,b );
	return obj_none;
}

tm_obj tm_div( tm_obj a, tm_obj b){
	if( a.type == b.type && a.type == TM_NUM){
		return number_new( get_num(a) / get_num(b) );
	}
	tm_raise("tm_div: can not div  @ and @", a,b );
	return obj_none;
}

tm_obj tm_mod( tm_obj a, tm_obj b){
	if( a.type == b.type && a.type == TM_NUM){
		return number_new((long)get_num(a) %  (long)get_num(b) );
	}
	tm_raise("tm_mod: can not mod  @ and @", a,b );
	return obj_none;
}


int _tm_has( tm_obj a, tm_obj b ){
	switch( a.type ){
		case TM_LST:{
			return ( list_index( get_list(a), b) != -1 );
		}
		case TM_STR:{
			if( b.type != TM_STR) return 0;
			return _str_find( a.value.str, b.value.str, 0) != -1;
		}
		case TM_DCT:{
			tm_obj v;
			return ( dict_iget(get_dict(a), b, &v) );
		}
	}
	return 0;
}


tm_obj tm_has(tm_obj a, tm_obj b){
	return number_new( _tm_has(a, b));
}


tm_obj _tm_not( tm_obj o){
	switch( o.type ){
		case TM_NUM:
			if( get_num(o) ) return obj_false;
			return obj_true;
		case TM_STR:
			if( get_str_len(o) > 0) return obj_false;
			return obj_true;
		case TM_NON:
			return obj_true;
	}
	return obj_false;
}
#define tm_in(k , x) tm_has(x, k)
#define tm_notin( k, x) _tm_not( tm_has(x,k))

int _tm_bool( tm_obj v){
	switch( v.type ){
		case TM_NUM:return get_num(v) != 0;
		case TM_NON:return 0;
		case TM_STR:return get_str_len(v) > 0;
		case TM_LST:return get_list_len(v) > 0;
		case TM_DCT:return get_dict_len(v) > 0;
	}
	return 0;
}

tm_obj tm_and( tm_obj a, tm_obj b){
	return number_new(_tm_bool(a) && _tm_bool(b));
}

tm_obj tm_or( tm_obj a, tm_obj b){
	return number_new(_tm_bool(a) || _tm_bool(b));
}

tm_obj tm_not( tm_obj o){
	if( _tm_bool(o)) return obj_false;
	return obj_true;
}

tm_obj tm_neg(tm_obj o){
	if( o.type == TM_NUM){
		get_num(o) = -get_num(o);
		return o;
	}
	tm_raise("tm_neg: can not handle @", o);
	return obj_none;
}

int tm_iter( tm_obj self, tm_obj *k){
    tm_obj v;
    int idx = get_num( *k );
	if( idx >= _tm_len(self))  return 0;
    if( self.type == TM_DCT ){
		if( idx == 0) dict_iter_init(get_dict(self));
        return dict_inext(get_dict(self), k, &v);
	}
    *k = tm_get(self, *k);
	return 1;
}
