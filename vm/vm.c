#include "tm.h"
/*
void constants_init(){
	tm->none = obj_new(TM_NON, NULL);
	obj_none = obj_new(TM_NON, NULL);
	int i;
	for(i = 0; i < 256; i++){
		char s[2] = {i, '\0'};
		tm->chars[i] = str_new(s, 1);
	}
}*/

#define register_constant( v) list_append(get_list(tm->root), (v));
void reg_builtin(char* name, tm_obj v){
	tm_obj key = str_new(name, strlen(name));
	tm_set( tm->builtins, key, v);
}

void reg_builtins(){
	/* constants */

    tm->root = list_new(100);
    tm->builtins = dict_new();
    tm->modules = dict_new();
    obj_true = number_new(1);
    obj_false = number_new(0);
    obj_none.type = TM_NON;
    obj__init__ = str_new("__init__", -1);
    obj__main__ = str_new("__main__", -1);
    obj__name__ = str_new("__name__", -1);
    obj_mod_ext = str_new("_pyc", -1);
    obj_star = str_new("*", -1);

    /* set module boot */
    tm_set( tm->modules, str_new("boot", -1), dict_new());
    // tm_set( tm->modules, str_new("tokenize_pyc", -1), dict_new());
    /* init chars , will never collected until last */
    int i;
    for(i = 0; i < 256; i++){
        unsigned char s[2] = {i, '\0'};
        __chars__[i] = str_new(s, 1);
        register_constant( __chars__[i]);
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
        {"exit", tm_exit},
        {0, 0}
    };
    for(i = 0; builtins[i].name != 0; i++){
        reg_builtin(builtins[i].name, func_new(obj_none, obj_none, obj_none, builtins[i].func));
    };
    tm_set( tm->builtins, str_new("tm", -1), number_new(1));
    tm_set( tm->builtins, str_new("True", -1), number_new(1));
    tm_set( tm->builtins, str_new("False",-1), number_new(0));
    tm_set( tm->builtins, str_new("__builtins__", -1), tm->builtins);

    /* build str class */
    str_class = dict_new();
   	static struct __builtin str_class_fnc_list[] = {
   		{"replace", str_replace},
   		{"find", str_find},
   		{"substring", str_substring},
   		{"upper", str_upper},
   		{"lower", str_lower},
        {"split", str_split},
   		{0,0}
   	};
   	for(i = 0; str_class_fnc_list[i].name != 0 ; i++){
   		tm_set( str_class, str_new(str_class_fnc_list[i].name, -1), 
   			func_new(obj_none, obj_none, obj_none, str_class_fnc_list[i].func));
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
        tm_obj name = str_new(list_class_fnc_list[i].name, -1);
        tm_obj fnc = func_new(obj_none, obj_none, obj_none, list_class_fnc_list[i].func);
        get_func(fnc)->name = name;
        tm_set( list_class, name, fnc);
   	}

   	/* build dict class */
   	dict_class = dict_new();
   	static struct __builtin dict_class_fnc_list[] = {
   		{"keys", bdict_keys},
   		{0,0}
   	};
   	for(i = 0; dict_class_fnc_list[i].name != 0 ; i++){
   		tm_set( dict_class, str_new(dict_class_fnc_list[i].name, -1), 
   			func_new(obj_none, obj_none, obj_none, dict_class_fnc_list[i].func));
   	}
    
    register_constant( str_class);
    register_constant( list_class);
    register_constant( dict_class);
    register_constant( tm->builtins);
    register_constant( tm->modules);
    register_constant( obj__init__);
	register_constant( obj__main__);
    register_constant( obj__name__);
    register_constant( obj_mod_ext);
    register_constant( obj_star);
}

void load_bultin_module(char* fname, unsigned char* s, int codelen){
  tm_obj mod_name = str_new(fname, strlen(fname));
  tm_obj code = str_new(s, codelen) ;
  tm_obj mod = module_new(mod_name, mod_name , code );
  tm_obj fnc = func_new(mod, code, obj_none, NULL);
  get_func(fnc)->pc = get_str(code);
  get_func(fnc)->name = obj__main__;
  tm_eval( fnc , obj_none);
  // tm_set( tm->modules, mod_name, get_mod(mod)->globals);
}

void frames_init(){
	int i;
	for(i = 0; i < FRAMES_COUNT; i++){
		tm_frame* f = tm->frames + i;
		f->stacksize = 1000;
		f->stack = tm_alloc(f->stacksize * sizeof(tm_obj));
        f->new_objs = list_new(20);
		f->ex = obj_none;
		f->file = obj_none;
		f->line = obj_none;
        f->globals = obj_none;
		f->func_name = obj_none;
		f->maxlocals = 0;
		f->jmp = 0;
		f->maxlocals = 0;
        int j;for(j = 0; j < f->stacksize; j++){
          f->stack[j].type = TM_NON;
        }
        for(j = 0; j < 256; j++){
          f->locals[j].type = TM_NON;
        }
	}
    // printf("frame init done");
	tm->cur = 0;
}

void frames_free(){
	int i;
	for(i = 0; i < FRAMES_COUNT; i++){
		tm_frame*f = tm->frames + i ;
#if LIGHT_DEBUG_GC
        printf("free frame %d: %p\n", i, f);
#endif
		tm_free(f->stack, f->stacksize * sizeof(tm_obj));
		// f->ex, f->file will handled by gc
	}
}

void
print_usage(){
	puts("usage tmvm [file]");
}

int tm_run(int argc, char* argv[]){
    int rs = setjmp(tm->buf);
	if(  rs == 0 ){
        /**
            normal phase
        */
        tm->cur = -1;
		gc_init();
        puts("gc init done !");
		tm_obj p = list_new(argc);
		int i;for(i = 1; i < argc; i++){
			tm_obj arg = str_new(argv[i], strlen(argv[i]));
			list_append( get_list(p), arg);
		}
        
        reg_builtins();
        reg_builtin("ARGV", p);
        
        puts("builtins init done!");
        frames_init();
        puts("frames init done!");
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

            load_bultin_module("tokenize", tokenize_pyc, sizeof(tokenize_pyc));  
            // puts("tokenize done");
            // cprintln(tm->modules);
            load_bultin_module("parse", parse_pyc, sizeof(parse_pyc));  
            // cprintln(tm->modules);
            // puts("parse done");
            load_bultin_module("instruction", instruction_pyc, sizeof(instruction_pyc));  
            // puts("instruction done");
            load_bultin_module("encode", encode_pyc, sizeof(encode_pyc));  
            // puts("encode done");
            tm_obj mod = module_new(mod_name, obj__main__ , code );
            tm_obj fnc = func_new(mod, code, obj_none, NULL);
            get_func(fnc)->pc = get_str(code);
            get_func(fnc)->name = obj__main__;
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
	}else if(rs == 1) {
        /**
            handle exceptions
        */
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
        // print locals in the current frame
        tm_obj temp = build_list( 10, tm->frames[tm->cur].locals);
        cprintln( temp );
	}else if( rs == 2){
        // normal exit
    }
}

int tm_init(int argc, char* argv[]){
	tm = malloc( sizeof(tm_vm) );
	if( tm == NULL ){
		fprintf(stderr, "vm init fail");
		return -1;
	}

	tm_run(argc, argv);
#if LIGHT_DEBUG_GC
    puts("free frames ...");
#endif
	frames_free();
#if LIGHT_DEBUG_GC
    puts("free gc objects ...");
#endif
	gc_free();
#if LIGHT_DEBUG_GC
    puts("free tm ...");
#endif
	free(tm);
	return 0;
}

int main(int argc, char* argv[]){
	return tm_init(argc, argv);
}
