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

void push_const(tm_obj *des, int type , ...){
    va_list ap;
    va_start(ap, type);
    switch(type){
        case TM_STR:{
            char*s = va_arg(ap, char*);
            *des = str_new( s, strlen(s));
            break;
        }
        case TM_NON:
            des->type = TM_NON;
            va_end(ap);
            return;
        case TM_NUM:{
            double dval = va_arg(ap, double);
            *des = number_new(dval);
            va_end(ap);
            return;
        }
        case TM_DCT:
            *des = dict_new();
            break;
        case TM_LST:
            *des = list_new(0);
            break;
        case -1:
            *des = va_arg(ap, tm_obj);
            break;
        default:
            tm_raise("push_const(), not supported type %d", type);
    }
    list_append( get_list(tm->root), *des);
    va_end(ap);
}

void reg_builtins(){
  /* constants */

    tm->root = list_new(100);

    push_const(&(tm->builtins), TM_DCT);
    push_const(&(tm->modules), TM_DCT);
    push_const(&str_class, TM_DCT);
    push_const(&list_class, TM_DCT);
    push_const(&dict_class, TM_DCT);

    push_const(&obj_true, TM_NUM, 1.0);
    push_const(&obj_false, TM_NUM, 0.0);
    push_const(&obj_none, TM_NON);

    push_const(&obj__init__, TM_STR, "__init__");
    push_const(&obj_mod_ext, TM_STR, "_pyc");
    push_const(&obj__name__, TM_STR, "__name__");
    push_const(&obj__main__, TM_STR, "__main__");
    // push_const(&empty_list, -1, list_new(1));
    // push_const(&g_arguments, TM_LST);

    /* set module boot */
    tm_set( tm->modules, str_new("boot", -1), dict_new());
    // tm_set( tm->modules, str_new("tokenize_pyc", -1), dict_new());
    /* init chars , will never collected until last */
    int i;
    for(i = 0; i < 256; i++){
        unsigned char s[2] = {i, '\0'};
        push_const(&__chars__[i], -1, str_new(s, 1));
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
        {"load_module", load_module},
        {"get_last_frame_globals", get_last_frame_globals},
        {"globals", tm_globals},
        {"len", tm_len},
        {"exit", tm_exit},
        {"istype", tm_istype},
        // {"makesure", tm_makesure},
        {"chr", tm_chr},
        {"ord", tm_ord},
        {"dir", tm_dir},
        {"code8", tm_code8},
        {"code16", tm_code16},
        {"codeF", tm_codeF},
        {"exists", tm_exists},
        {"mtime", tm_mtime},
        {"clock", tm_clock},
        /* this function is super function , it can add method to type class */
        {"add_type_method", tm_add_type_method},
        {"_raise", btm_raise},
        {"raise", btm_raise},
        {NULL, 0}
    };
    for(i = 0; builtins[i].name != NULL; i++){
        tm_obj name = str_new(builtins[i].name, -1);
        tm_obj fnc = func_new(obj_none,obj_none, builtins[i].func);
        get_func(fnc)->name = name;
        tm_set(tm->builtins, name, fnc);
    };
    tm_set( tm->builtins, str_new("tm", -1), number_new(1));
    tm_set( tm->builtins, str_new("True", -1), number_new(1));
    tm_set( tm->builtins, str_new("False",-1), number_new(0));
    tm_set( tm->builtins, str_new("__builtins__", -1), tm->builtins);
    tm_set( tm->builtins, str_new("__modules__", -1), tm->modules);
    tm_set( tm->builtins, str_new("__objectsize__", -1), number_new(sizeof(tm_obj)));

    /* build str class */
     static struct __builtin str_class_fnc_list[] = {
       {"replace", str_replace},
       {"find", str_find},
       {"substring", str_substring},
       {"upper", str_upper},
       {"lower", str_lower},
       {"split", str_split},
       {"join", str_join},
       {0,0}
     };
     for(i = 0; str_class_fnc_list[i].name != 0 ; i++){
        tm_obj name = str_new(str_class_fnc_list[i].name, -1);
        tm_obj fnc = func_new(obj_none,obj_none, str_class_fnc_list[i].func);
        get_func(fnc)->name = name;
        tm_set( str_class, name, fnc);
     }

     /* build list class */
     static struct __builtin list_class_fnc_list[] = {
        {"append", blist_append},
        {"pop", blist_pop},
        {"insert", blist_insert},
        {"extend", blist_extend},
        {"index", blist_index},
        {"reverse", blist_reverse},
        {0,0}
    };
    for(i = 0; list_class_fnc_list[i].name != 0 ; i++){
        tm_obj name = str_new(list_class_fnc_list[i].name, -1);
        tm_obj fnc = func_new(obj_none,  obj_none, list_class_fnc_list[i].func);
        get_func(fnc)->name = name;
        tm_set( list_class, name, fnc);
     }

     /* build dict class */
     static struct __builtin dict_class_fnc_list[] = {
       {"keys", bdict_keys},
       {0,0}
     };
     for(i = 0; dict_class_fnc_list[i].name != 0 ; i++){
        tm_obj name = str_new(dict_class_fnc_list[i].name, -1);
        tm_obj fnc = func_new(obj_none,obj_none, dict_class_fnc_list[i].func);
        get_func(fnc)->name = name;
        tm_set( dict_class, name, fnc);
     }
}

void load_bultin_module(char* fname, unsigned char* s, int codelen){
  tm_obj mod_name = str_new(fname, strlen(fname));
  tm_obj code = str_new(s, codelen) ;
  tm_obj mod = module_new(mod_name, mod_name , code );
  tm_obj fnc = func_new(mod, obj_none, NULL);
  get_func(fnc)->pc = get_str(code);
  get_func(fnc)->name = obj__main__;
  tm_eval( fnc , obj_none);
  // tm_set( tm->modules, mod_name, get_mod(mod)->globals);
}

void frames_init(){
  int i;
  for(i = 0; i < FRAMES_COUNT; i++){
    tm_frame* f = tm->frames + i;
    f->stacksize = 100;
    f->stack = tm_alloc(f->stacksize * sizeof(tm_obj));
    tm_log3("frame", "alloc frame %d: %p, stack = %p", i, f , f->stack);
    f->new_objs = list_new(2);
    f->ex = obj_none;
    // f->file = obj_none;
    f->line = obj_none;
    // f->globals = obj_none;
    f->fnc = obj_none;
    // f->constants = obj_none;
    f->maxlocals = 0;
    f->jmp = NULL;
    f->maxlocals = 0;
  }
  tm->cur = 0;
}

void frames_free(){
  int i;
  for(i = 0; i < FRAMES_COUNT; i++){
    tm_frame*f = tm->frames + i ;
    tm_log3("frame", "free frame %d: %p, stack = %p", i, f, f->stack);
    tm_free(f->stack, f->stacksize * sizeof(tm_obj));
    // f->ex, f->file will handled by gc
  }
}

void
print_usage(){
  puts("usage tmvm [file]");
}


void traceback(){
    int i;
    int cur = tm->cur;
    int hasHandler = 0;
    int handlerFrame = 0;
    // 返回上一帧
    tm_frame* f = NULL;
    tm_obj exlist = list_new(10);
    for(i = cur; i >= 0; i-- ){
      f = tm->frames + i;
      if( f->jmp == NULL && TM_NON != f->fnc.type){
        tm_obj file = get_fnc_file(f->fnc);
        tm_obj fnc_name = get_fnc_name(f->fnc);
        tm_obj ex = tm_format("  File \"@\": in @ , @", file, fnc_name, f->line);
        list_append( get_list(exlist), ex );
      }else if( f->jmp != NULL ){
        hasHandler = 1;
        handlerFrame = i;
        break;
      }
    }
    if( hasHandler ){
        tm_func* fnc = get_func(f->fnc);
        tm_obj newobj = method_new(f->fnc, fnc->self);
        get_func(newobj)->pc = f->jmp;
        tm->cur = handlerFrame - 1 ; // will use the locals of last frame
        f->jmp = NULL;
        protected_run(newobj , obj_none);
    }else{
        printf("Traceback (most recent call last):\n");
        int i;for(i = list_len(exlist) - 1; i >= 0; i--){
            cprintln(list_nodes(exlist)[i]);
        }
        tm_printf("Exception: @", tm->frames[tm->cur].ex);
    }
}

int protected_run( tm_obj fnc, tm_obj params ){
    jmp_buf buf;
    int rs = setjmp( buf );
    if( rs == 0){
        tm_eval(fnc, params);
    }else if(rs == 1){
        traceback();
    }
    return 0;
}

int tm_run(int argc, char* argv[]){
    int rs = setjmp(tm->buf);
    if(  rs == 0 ){
        tm->cur = -1;
        gc_init();
        tm_obj p = list_new(argc);
        int i;for(i = 1; i < argc; i++){
        tm_obj arg = str_new(argv[i], strlen(argv[i]));
            list_append( get_list(p), arg);
        }
            
        reg_builtins();
        tm_set(tm->builtins, str_new("ARGV", -1), p);
        CHECK_MEM_USAGE("builtins");
        
        frames_init();
        CHECK_MEM_USAGE("frames");
        
        disable_log();
        load_bultin_module("_boot", _boot_pyc, -1);
        load_bultin_module("tokenize", tokenize_pyc, -1);  
        load_bultin_module("expression", expression_pyc, -1);  
        load_bultin_module("parse", parse_pyc,-1);  
        load_bultin_module("instruction", instruction_pyc, -1);  
        load_bultin_module("encode", encode_pyc, -1);  
        enable_log();
        tm_log0("mod", "modules loading done");
        
        if( argc >= 2){
            char* fname = argv[1];
            if( strcmp(argv[1], "-d") == 0){
                enable_debug = 1;
                fname = argv[2];
            }
            tm_obj mod_name = str_new(fname, strlen(fname));
            tm_call("_boot", "_execute_file", as_list(1, mod_name));
            // tm_call("_boot", "_import", as_list(1, mod_name));
            // tm_call("encode", "b_compile", as_list(2, mod_name, str_new("test.tmc", -1)));
            // tm_call("parse", "_parse", as_list(1, mod_name));
            // tm_call("tokenize", "_tokenize", as_list(1, mod_name));
            CHECK_MEM_USAGE("after eval");
        }else {
            tm_call("_boot", "_repl", obj_none);
        }
    }else if(rs == 1) {
        traceback();
    }else if( rs == 2){
        // normal exit
    }
}

int test_init(void (*func)()){
  int rs = setjmp(tm->buf);
  if(  rs == 0 ){
    tm->cur = -1;
    gc_init();
    reg_builtins();
    frames_init();
    /* run test function */
    func();
  }else if(rs == 1) {
      traceback();
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
  frames_free();
  gc_free();
  free(tm);
  return 0;
}

int main(int argc, char* argv[]){
  return tm_init(argc, argv);
}
