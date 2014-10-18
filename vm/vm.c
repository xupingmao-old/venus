#include "tm.h"

/*
tm_obj tm_c_call(tm_vm* tm, char* mod, char* func, tm_obj params){
	tm_obj m = tm->none;
	if( mod != NULL ){
		m = str_new(tm, mod);
		m = tm_get(tm, tm->modules, m);
	}else{
		m = tm->builtins;
	}
	if( m.type == TM_NON ){
		tm_raise(tm);
	}
	tm_obj fname = str_new(tm, func);
	tm_obj f0 = tm_get(tm, m, fname);
	return tm_call(tm, f0, params);
}
*/

void constants_init(){
	tm->none = obj_new(TM_NON, NULL);
	int i;
	for(i = 0; i < 256; i++){
		unsigned char s[2] = {i, '\0'};
		tm->chars[i] = str_new(s, 1);
	}
}
/*
void builtins_method_init(){
	tm->str_methods = dict_new();
	tm->list_methods = dict_new();
	struct nfl{
		char* name;
		tm_obj (*native_func) (tm_vm*, tm_obj);
	} native_str_methods[] = {
			{ "substr", str_substr },
			{ "find" , str_find },
			{ "replace" , str_replace },
			{ "upper", str_upper },
			{0, 0}
	},
	native_list_methods[] = {
			{"push", _list_push},
			{"pop", _list_pop},
			{"insert", _list_insert},
			{0, 0}
	};
	int i = 0;
	for( i = 0; native_str_methods[i].name ; i++){
		tm_obj name = str_new(tm, native_str_methods[i].name);
		tm_obj func = native_method_new(tm, native_str_methods[i].native_func);
		tm_set( tm->str_methods, name, func);
	}

	for( i = 0; native_list_methods[i].name; i++){
		tm_obj name = str_new(tm, native_list_methods[i].name);
		tm_obj method = native_method_new(tm, native_list_methods[i].native_func);
		tm_set( tm->list_methods, name, method);
	}

	list_push(tm, tm->root, tm->str_methods);
	list_push(tm, tm->root, tm->list_methods);
	tm_obj o;
	o.type = TM_LST;
	get_list(o) = tm->root;
	_tm_print( o);
}

void builtins_init(){
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
		tm_obj name = str_new(tm, native_func_list[i].name);
		tm_obj func = native_func_new(tm, native_func_list[i].native_func);
		tm_set( tm->builtins, name, func);
	}
//	tm_print(tm, tm_arg1(tm, tm->builtins));
}

void modules_init(){
	tm->modules = dict_new(tm);
}*/

void reg_builtin(char* name, tm_obj v){
	tm_obj key = str_new(name, strlen(name));
	tm_set( tm->builtins, key, v);
}

void reg_builtins(){
    struct __builtin {
        char* name;
        tm_obj (*func) ( tm_obj );
    };
    static struct __builtin builtins[] = {
        {"load", tm_load},
        {"save", tm_save},
        {"print", tm_print},
        {"str", btm_str},
        {"int", tm_int},
        {"float", tm_float},
        {0, 0}
    };
    int i;for(i = 0; builtins[i].name != 0; i++){
        reg_builtin(builtins[i].name, func_new(NULL, tm->none, tm->none, builtins[i].func));
    };

    /* build str class */
    str_class = dict_new();
   	static struct __builtin str_class_fnc_list[] = {
   		{"replace", str_replace},
   		{"find", str_find},
   		{0,0}
   	};
   	for(i = 0; str_class_fnc_list[i].name != 0 ; i++){
   		tm_set( str_class, str_new(str_class_fnc_list[i].name, -1), 
   			func_new(NULL, tm->none, tm->none, str_class_fnc_list[i].func));
   	}
}

void frames_init(){
	int i;
	for(i = 0; i < FRAMES_COUNT; i++){
		tm_frame* f = tm->frames + i;
		f->stacksize = 100;
		f->stack = tm_alloc(f->stacksize * sizeof(tm_obj));
		f->ex = tm->none;
		f->file = tm->none;
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
		// f->ex, f->file will handled by gc
	}
}

void
print_usage(){
	puts("usage tmvm [file]");
}

int tm_run(int argc, char* argv[]){
	if(  setjmp(tm->buf) == 0 ){
	// 真正要执行的代码,发生异常之后返回setjmp的地方
		//test_dict();
		gc_init();
		constants_init();
		tm->builtins = dict_new();
		frames_init();

		tm_obj p = list_new(argc);
		// init argv;
		int i;for(i = 0; i < argc; i++){
			tm_obj arg = str_new(argv[i], strlen(argv[i]));
			list_append( get_list(p), arg);
		}
        
        reg_builtins();
		reg_builtin("argv", p);
		// cprintln(tm_add(str_new("fdsaf",-1), str_new("fdsaf",-1)));
		// tm_printf("hello, @", str_new("34234@", -1));
		if( argc == 2){
			char* fname = argv[1];
			tm_obj code = _load(fname);
			// printf("load file %s\n", fname);
			// cprintln(code);
			tm_obj mod_name = str_new(fname, strlen(fname));
			tm->frames[tm->cur].file = mod_name;
			tm_obj mod = module_new(mod_name, str_new("__main__", -1) , code );
			tm_eval( mod.value.mod );
			// cprintln(mod);
		}else {
			print_usage();
		}
		// cprintln(tm->builtins);
		// tm_obj n = number_new(213.34);
		// _tm_len(n);
		// tm->cur = 0;
		// tm_obj v = obj_new(TM_LST, tm->all);
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
				tm_printf("  File \"@\": @", f->file, f->ex);
			}
		}
		tm_printf("Exception: @", tm->frames[tm->cur].ex);
	}
}

int tm_init(int argc, char* argv[]){
	tm = malloc( sizeof(tm_vm) );
	if( tm == NULL ){
		fprintf(stderr, "vm init fail");
		return -1;
	}
	tm_run(argc, argv);
	frames_free();
	gc_free();
	free(tm);
	return 0;
}

int main(int argc, char* argv[]){
	return tm_init(argc, argv);
}
