#include "tm.h"

/*
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
*/

void constants_init(){
	tm->none = obj_new(TM_NON, NULL);
	tm->none_str = string_new("None", 0);
	tm->empty_str = string_new("", 0);
	int i;
	for(i = 0; i < 256; i++){
		char s[2] = {i, '\0'};
		tm->chars[i] = string_new(s, 1);
	}
}
/*
void builtins_method_init(){
	tm->string_methods = map_new();
	tm->list_methods = map_new();
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

void builtins_init(){
	tm->builtins = map_new(tm);
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

void modules_init(){
	tm->modules = map_new(tm);
}*/

void reg_builtin(char* name, tm_obj v){
	tm_obj key = string_new(name, strlen(name));
	tm_set( tm->builtins, key, v);
}

int tm_run(){
	if(  setjmp(tm->buf) == 0 ){
	// 真正要执行的代码,发生异常之后返回setjmp的地方
		//test_map();
		cprintln(tm->builtins);
		tm_obj n = number_new(213.34);
		_tm_len(n);
		tm->cur = 0;
		tm_obj v = obj_new(TM_LST, tm->all);
		//tm_raise("tm->list = @", v);
	}else{
	// 发生了异常，返回捕捉异常的帧
		int i;
		int cur = tm->cur;
		printf("Traceback (most recent call last):\n");
		// 返回上一帧
		for(i = cur; i >= 0; i-- ){
			tm_frame* f = tm->frames + i;
			if( f->jmp == 0 ){
				tm_printf("  File \"@\": @", f->mod, f->ex);
			}
		}
		tm_printf("Exception: @", tm->frames[tm->cur].ex);
	}
}

void frames_init(){
	int i;
	for(i = 0; i < FRAMES_COUNT; i++){
		tm_frame* f = tm->frames + i;
		f->stacksize = 10;
		f->stack = tm_alloc(f->stacksize * sizeof(tm_obj));
		f->ex = tm->none;
		f->mod = tm->none;
		f->jmp = 0;
		f->maxlocals = 0;
	}
	tm->cur = 0;
}

void frames_free(){
	int i;
	for(i = 0; i < FRAMES_COUNT; i++){
		tm_frame*f = tm->frames + i;
		tm_free(f->stack, f->stacksize * sizeof(tm_obj));
		// f->ex, f->mod will handled by gc
	}
}

int tm_init(int argc, char* argv[]){
	tm = malloc( sizeof(tm_vm) );
	if( tm == NULL ){
		fprintf(stderr, "vm init fail");
		return -1;
	}
	gc_init();
	constants_init();
	tm->builtins = map_new();
	frames_init();

	tm_obj p = list_new(argc);
	// init argv;
	int i;for(i = 0; i < argc; i++){
		tm_obj arg = string_new(argv[i], strlen(argv[i]));
		list_append( get_list(p), arg);
	}
	reg_builtin("argv", p);

	tm_run();
	frames_free();
	gc_free();
	free(tm);
	return 0;
}

int main(int argc, char* argv[]){
	return tm_init(argc, argv);
}