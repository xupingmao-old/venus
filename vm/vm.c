#include "tm.h"

void constants_init(){
	tm->none = obj_new(TM_NON, NULL);
	obj_none = obj_new(TM_NON, NULL);
	int i;
	for(i = 0; i < 256; i++){
		char s[2] = {i, '\0'};
		tm->chars[i] = str_new(s, 1);
	}
}


void reg_builtin(char* name, tm_obj v){
	tm_obj key = str_new(name, strlen(name));
	tm_set( tm->builtins, key, v);
}

void reg_builtins(){
	/* constants */

	obj_true = number_new(1);
	obj_false = number_new(0);
	obj_none.type = TM_NON;
    obj__init__ = str_new("__init__", -1);
    obj__main__ = str_new("__main__", -1);

    /* init chars , will never collected until last */
    int i;
    for(i = 0; i < 256; i++){
        unsigned char s[2] = {i, '\0'};
        __chars__[i] = str_new(s, 1);
    }
	

    struct __builtin {
        char* name;
        tm_obj (*func) ( tm_obj );
    };
    static struct __builtin builtins[] = {
        {"load", tm_load},
        {"save", tm_save},
        {"print", tm_print},
        {"input", tm_input},
        {"str", btm_str},
        {"int", tm_int},
        {"float", tm_float},
        {"range", tm_range},
		{"import", tm_import},
        {"globals", tm_globals},
        {"len", tm_len},
        {0, 0}
    };
    for(i = 0; builtins[i].name != 0; i++){
        reg_builtin(builtins[i].name, func_new(obj_none, tm->none, tm->none, builtins[i].func));
    };
    tm_set( tm->builtins, str_new("tm", -1), number_new(1));
    tm_set( tm->builtins, str_new("True", -1), number_new(1));
    tm_set( tm->builtins, str_new("False",-1), number_new(0));

    /* build str class */
    str_class = dict_new();
   	static struct __builtin str_class_fnc_list[] = {
   		{"replace", str_replace},
   		{"find", str_find},
   		{"substring", str_substring},
   		{"upper", str_upper},
   		{"lower", str_lower},
   		{0,0}
   	};
   	for(i = 0; str_class_fnc_list[i].name != 0 ; i++){
   		tm_set( str_class, str_new(str_class_fnc_list[i].name, -1), 
   			func_new(obj_none, tm->none, tm->none, str_class_fnc_list[i].func));
   	}

   	/* build list class */
   	list_class = dict_new();
   	static struct __builtin list_class_fnc_list[] = {
   		{"append", blist_append},
        {"pop", blist_pop},
        {"insert", blist_insert},
        {"extend", blist_extend},
        {"index", blist_index},
   		{0,0}
   	};
   	for(i = 0; list_class_fnc_list[i].name != 0 ; i++){
   		tm_set( list_class, str_new(list_class_fnc_list[i].name, -1), 
   			func_new(obj_none, tm->none, tm->none, list_class_fnc_list[i].func));
   	}

   	/* build dict class */
   	dict_class = dict_new();
   	static struct __builtin dict_class_fnc_list[] = {
   		{"keys", bdict_keys},
   		{0,0}
   	};
   	for(i = 0; dict_class_fnc_list[i].name != 0 ; i++){
   		tm_set( dict_class, str_new(dict_class_fnc_list[i].name, -1), 
   			func_new(obj_none, tm->none, tm->none, dict_class_fnc_list[i].func));
   	}
}

void frames_init(){
	int i;
	for(i = 0; i < FRAMES_COUNT; i++){
		tm_frame* f = tm->frames + i;
		f->stacksize = 200;
		f->stack = tm_alloc(f->stacksize * sizeof(tm_obj));
		f->ex = obj_none;
		f->file = obj_none;
		f->line = obj_none;
        f->globals = obj_none;
        f->func_name = obj_none;
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
        tm->modules = dict_new();
		frames_init();

		tm_obj p = list_new(argc);
		// init argv;
        // first vm itself, second srcfile
		int i;for(i = 1; i < argc; i++){
			tm_obj arg = str_new(argv[i], strlen(argv[i]));
			list_append( get_list(p), arg);
		}
        
        reg_builtins();
		reg_builtin("ARGV", p);
		// cprintln(tm_add(str_new("fdsaf",-1), str_new("fdsaf",-1)));
		// tm_printf("hello, @", str_new("34234@", -1));
		if( argc >= 2){
			char* fname = argv[1];
			if( strcmp(argv[1], "-d") == 0){
				enable_debug = 1;
				fname = argv[2];
			}
			tm_obj code = _load(fname);
			// printf("load file %s\n", fname);
			// cprintln(code);
			tm_obj mod_name = str_new(fname, strlen(fname));
			tm->frames[tm->cur].file = mod_name;
			tm_obj mod = module_new(mod_name, str_new("__main__", -1) , code );

            tm_set( get_mod(mod)->globals, str_new("__name__", -1), str_new("__main__", -1));
            tm_set( get_mod(mod)->globals, str_new("tm", -1), number_new(1));
            tm_obj fnc = func_new(mod, code, tm->none, NULL);
            get_func(fnc)->pc = get_str(code);
            get_func(fnc)->name = obj__main__;
            tm->cur--; // to use the first frame;
			tm_eval( fnc , obj_none);
			// cprintln(mod);
            // tm_obj res = _tm_call( "token", "do_tokenize", as_list(1, str_new("print(\"hello,world\"", -1)));
            // cprintln(res);
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
		/* catch exceptions */
		int i;
		int cur = tm->cur;
		printf("Traceback (most recent call last):\n");
		// 返回上一帧
		for(i = cur; i >= 0; i-- ){
			tm_frame* f = tm->frames + i;
			if( f->jmp == 0 ){
				tm_printf("  File \"@\": in @ , @\n", f->file, f->func_name, f->line);
			}
		}
		tm_printf("Exception: @", tm->frames[tm->cur].ex);
        tm_obj temp = build_list( 10, tm->frames[tm->cur].locals);
        cprintln( temp );
        // cprintln( tm->frames[cur].globals);
        // cprintln( tm->builtins);
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
