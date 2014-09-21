#include "tm.h"


tm_obj tm_c_call(tm_vm* tm, char* mod, char* func, tm_obj params){
	tm_obj m = tm->none;
	if( mod != NULL ){
		m = string_new(tm, mod);
		m = tm_get(tm, tm->modules, m);
	}else{
		m = tm->builtins;
	}
	if( m.type == TM_NON ){
		tm_raise(tm);
	}
	tm_obj fname = string_new(tm, func);
	tm_obj f0 = tm_get(tm, m, fname);
	return tm_call(tm, f0, params);
}

void constants_init(tm_vm* tm){
	tm->none.type = TM_NON;
	tm->none_str = S("None");
	tm->empty_str = S("");

	// init chars;
	// ��Щ�ַ�ᱻ��������Ϊ�ַ����ԭ��
	int i;
	for(i = 0; i < 256; i++){
		char *s = tm_alloc(tm, 2);
		s[0] = i;
		s[1] = '\0';
		// will be tracked by tm->strings;
		tm->chars[i] = string_new_(tm, s, 1);
	}
}

void builtins_method_init(tm_vm* tm){
	tm->string_methods = map_new(tm);
	tm->list_methods = map_new(tm);
	struct nfl{
		char* name;
		tm_obj (*native_func) (tm_vm*, tm_obj);
	} native_string_methods[] = {
			{ "substr", string_substr },
			{ "find" , string_find },
			{ "replace" , string_replace },
			{ "upper", string_upper },
			{0, 0}
	},
	native_list_methods[] = {
			{"push", _list_push},
			{"pop", _list_pop},
			{"insert", _list_insert},
			{0, 0}
	};
	int i = 0;
	for( i = 0; native_string_methods[i].name ; i++){
		tm_obj name = string_new(tm, native_string_methods[i].name);
		tm_obj func = native_method_new(tm, native_string_methods[i].native_func);
		tm_set( tm->string_methods, name, func);
	}

	for( i = 0; native_list_methods[i].name; i++){
		tm_obj name = string_new(tm, native_list_methods[i].name);
		tm_obj method = native_method_new(tm, native_list_methods[i].native_func);
		tm_set( tm->list_methods, name, method);
	}

	list_push(tm, tm->root, tm->string_methods);
	list_push(tm, tm->root, tm->list_methods);
	tm_obj o;
	o.type = TM_LST;
	get_list(o) = tm->root;
	_tm_print( o);
}

void builtins_init(tm_vm* tm){
	tm->builtins = dict_new(tm);
	struct nfl{
		char* name;
		tm_obj (*native_func) (tm_vm*, tm_obj);
	} native_func_list[] = {
			{ "print", tm_print },
			{ "sleep" , tm_sleep },
			{ "open" , stream_open },
			{ "input", tm_input },
			{0, 0}
	};
	int i = 0;
	for( i = 0; native_func_list[i].name ; i++){
		tm_obj name = string_new(tm, native_func_list[i].name);
		tm_obj func = native_func_new(tm, native_func_list[i].native_func);
		tm_set( tm->builtins, name, func);
	}
//	tm_print(tm, tm_arg1(tm, tm->builtins));
}

void modules_init(tm_vm*tm){
	tm->modules = dict_new(tm);
}

int tm_init(int argc, char* argv[]){
	tm = malloc( sizeof(tm_vm) );
	if( tm == NULL ){
		puts("vm init fail");
		return -1;
	}
	gc_init(tm);
	constants_init(tm);
	builtins_init(tm);
	builtins_method_init(tm);
	modules_init(tm);

	test_map(tm);

	tm->cur = 0;

	//test(tm);
	gc_free(tm);
	return 0;
}

int main(int argc, char* argv[]){
	return tm_init(argc, argv);
}

tm_obj tm_eval( tm_vm* tm){
	return tm->none;
}
