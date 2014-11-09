#include "tm.h"


tm_obj _tm_str(  tm_obj a){
    switch( a.type ){
    case TM_STR:
        return a;
    case TM_NUM:
    {
        char s[20];
        double v = get_num(a);
        if( fabs(v) - fabs((long)v) < 0.000000001 ) {
            sprintf(s, "%ld", (long)v);
        }else{
            sprintf(s, "%lf", v);
        }
        return str_new(s, strlen(s));
    }
    case TM_LST:{
        tm_obj str = str_new("[", -1);
        int i, l = get_list_len(a);
        for(i = 0; i < l ; i++){
            tm_obj obj = get_list(a)->nodes[i];
            if( tm_eq(a, obj)){
                obj = str_new("[...]", -1);
            }else{
                obj = _tm_str(obj);
            }
            str= tm_add(str, obj);
            if( i != l -1)
                str= tm_add(str, str_new(",", -1));
        }
        str = tm_add(str, str_new("]", -1));
        return str;
    }
    case TM_DCT:
        return _tm_type(a);
    case TM_FNC:
        return _tm_type(a);
    case TM_MOD:
        return str_new("<module>", -1);
    case TM_NON:
        return str_new("None", -1);
    default:
        tm_raise("tm_str(): not supported type %d", a.type);
    }
    return str_new("",0);
}

tm_obj btm_str( tm_obj p){
    tm_obj a = get_arg( p, 0, -1);
    return _tm_str(a);
}

tm_obj tm_copy(tm_vm* tm, tm_obj o){
    switch( o.type ){
    case TM_NUM:
        return o;
    case TM_STR:
        {
            int len = str_len(o);
            return str_new(get_str(o), len);
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
    return obj_none;
}

int _tm_len(tm_obj o){
    switch(o.type){
    case TM_STR:return get_str_len(o);
    case TM_LST:return list_len(o);
    case TM_DCT:return dict_len(o);
    }
    tm_raise("tm_len: @ has no attribute len", o);
    return 0;
}

tm_obj tm_len(tm_obj p){
    tm_obj o = get_arg(p, 0, -1);
    return number_new(_tm_len(o));
}


void _tm_print(tm_obj o, int depth, int show_special){
	depth--;
	if( depth < 0 )
		return;
	switch( o.type ){
	case TM_STR:
		{
			int i;int len = str_len(o);
			unsigned char *s = get_str(o);
			if(show_special){
				putchar('"');
				for(i = 0; i < len; i++){
					if(s[i] == '\0'){
						printf("\\0");
					}else if( s[i] == '\r'){
						printf("\\r");
					}else if( s[i] == '\n'){
						printf("\\n");
					}else if( s[i] == '\t'){
						printf("\\t");
					}else if( s[i] > 126 || s[i] < 32 ){
                        printf("0x%X", (int)s[i]);
                    }else{
						putchar(s[i]);
					}
				}
				putchar('"');
			}else{
                // if( len >= 20) len = 20;
                for(i = 0; i < len; i++){
                    if( '\n' == s[i] || '\t' == s[i] || '\r' == s[i]) {
                        putchar(s[i]);
                    }else if( s[i] > 126 || s[i] < 32){
                        printf("0x%X", (int)s[i]);
                    }else{
                        putchar(s[i]);
                    }
                }
                // if( get_str_len(o) >= 20) printf("...");
            }
		}
		break;
	case TM_NUM:
        o = _tm_str(o);
        printf("%s", get_str(o));
		break;
	case TM_LST:
	{
		int i = 0;
		tm_list* list = get_list(o);
		printf("[");
		for(i = 0; i < list->len; i++){
			_tm_print(list->nodes[i], depth, show_special);
			if( i+1 != list->len){
				putchar(',');
			}
		}
		printf("]");
		break;
	}
	case TM_DCT:
	{
		int i = 0;
		tm_obj k,v;
		tm_dict* dict = get_dict( o );
		int len = dict->len;
		putchar('{');
		for(i = 0; dict_inext(dict, &k, &v); i++){
			_tm_print(k, depth, show_special);
			putchar(':');
			_tm_print(v, depth, show_special);
			if( i != len -1 ) putchar(',');
		}
		putchar('}');
		break;
	}
	// case TM_DCT:dict_print(get_dict(o));break;
	case TM_FNC:
		if( get_func(o)->self.type != TM_NON){
			printf("<method %p ",get_func(o));
		}else{
			printf("<function %p ",get_func(o));
		}
		if( get_func(o)->name.type != TM_NON){
			cprint(get_func(o)->name);
		}
		printf(">");
		break;
	case TM_MOD:
		printf("<module %p ",get_mod(o));
        if( get_mod(o)->file.type != TM_NON ){
            cprint(get_mod(o)->file);
        }
        printf(">");
		break;
	case TM_NON:
		printf("None");break;
	}
}

void cprint(tm_obj o){
	_tm_print(o, 4, 0);
}

void cprintln_show_special(tm_obj o){
	_tm_print(o, 4, 1);
	puts("");
}

void cprintln(tm_obj o){
	cprint(o);
	puts("");
}


tm_obj tm_print(tm_obj params){
	int i = 0;
	tm_list* list = get_list(params);
    // cprintln(params);
	for(i = 0; i < list->len; i++ ){
		cprint(list->nodes[i]);
		if( i + 1 != list->len){
			putchar(' ');
		}
	}
	putchar('\n');
	return obj_none;
}

tm_obj _tm_format(char* fmt, va_list ap, int appendln){
	int i;
	int len = strlen(fmt);
	tm_obj nstr = str_new("", 0);
	int  templ = 0;
	char* start = fmt;
    int istrans = 1;
	for(i = 0; i < len; i++){
		if( fmt[i] == '@' ){
			if( templ > 0){
				tm_obj txt = str_new(start, templ);
				nstr = tm_add(nstr, txt);
				templ = 0;
			}
			tm_obj v = va_arg(ap, tm_obj);
			nstr = tm_add(nstr, _tm_str(v));
			start = fmt + i + 1;
		}else if( fmt[i] == '%' ){
            i++;
            int intval;double fval;
            tm_obj obj;
            char c = fmt[i];
            switch( fmt[i] ){
                case 'd':
                    intval = va_arg(ap, int); 
                    obj = number_new(intval);
                    break;
                case 'f':
                /* ... will pass float as double */
                    fval = va_arg(ap, double);
                    obj = number_new(fval);
                    break;
                    /* ... will pass char  as int */
                case 'c':
                    intval = va_arg(ap, int);
                    obj = _tm_chr(intval);
                    break;
                case 's':{
                    char* sval = va_arg(ap, char*);
                    obj = str_new(sval, strlen(sval));
                    break;
                }
                case 'P':
                case 'p':{
                    void *pval = va_arg(ap, void*);
                    char pchars[200];
                    sprintf(pchars, "%p", pval);
                    obj = str_new(pchars, strlen(pchars));
                    break;
                }
                case 't':
                    obj = va_arg(ap, tm_obj);
                    obj = _tm_type(obj);
                    break;
                case 'o':{
                    tm_obj v = va_arg(ap, tm_obj);
                    obj = _tm_str(v);
                    break;
                }
                case 'l':{
                    tm_obj v = va_arg(ap, tm_obj);
                    if( v.type != TM_LST) tm_raise("_tm_foramt(): expect list");
                    obj = str_new("",0);
                    int k;for(k = 0; k < list_len(v);k++){
                        obj = tm_add(obj, _tm_type(list_nodes(v)[k]));
                    }
                    break;
                }

                default: tm_raise("_tm_format(), unknown pattern %c", fmt[i]); break;
            }
            if( templ > 0) {
                tm_obj txt = str_new(start, templ );
                nstr = tm_add( nstr, txt );
                templ = 0;
            }
            start = fmt + i + 1;
            if( istrans ) {
                nstr = tm_add(nstr, _tm_str(obj));
            }
		}else {
            templ++;
        }
	}
	if( templ > 0){
		tm_obj txt = str_new(start, templ);
		nstr = tm_add(nstr, txt);
	}
    if( appendln ) nstr = tm_add( nstr, str_new("\n", -1));
	return nstr;
}

tm_obj tm_format(char* fmt, ...){
	va_list a; 
	va_start(a,fmt);
	tm_obj  v = _tm_format(fmt, a, 0);
	va_end(a);
	return v;
}
void _tm_printf(char* fmt, va_list ap){
	int i;
	int len = strlen(fmt);
	char*s = fmt;
	for(i = 0; i < len; i++){
		if( s[i] == '@' ){
			tm_obj v = va_arg(ap, tm_obj);
			cprint(v);
		}else{
			putchar(s[i]);
		}
	}
}

void tm_printf_only_type(char* fmt, ...){
	va_list ap; 
	va_start(ap,fmt);
	int i, len = strlen(fmt);
    char*s = fmt;
    for(i = 0; i < len;i++){
        if( s[i] == '@'){
            tm_obj v = va_arg(ap, tm_obj);
            switch( v.type ){
                case TM_NUM:printf("<number %g>", get_num(v));break;
                case TM_STR:printf("<string %p>", get_str(v));break;
                case TM_LST:printf("<list %p>", get_list(v));break;
                case TM_DCT:printf("<dict %p>", get_dict(v));break;
                case TM_MOD:printf("<mod %p>", get_mod(v));break;
                case TM_FNC:printf("<function %p>", get_func(v));break;
                case TM_NON:printf("<none>");break;
                default:printf("<unknown %d>", v.type);
            }
        }else{
            putchar(s[i]);
        }
	va_end(ap);
    }
}

void tm_printf(char* fmt, ...){
	va_list a; 
	va_start(a,fmt);
	_tm_printf(fmt, a);
	va_end(a);
}

tm_obj tm_sleep( tm_obj p){
	int i = 0;
	tm_obj time = get_arg(p, 0, TM_NUM);
	int t = get_num(time);
#ifdef _WINDOWS_H
	Sleep(t);
#else
	sleep(t);
#endif
	return obj_none;
}

tm_obj tm_input(tm_obj p){
	int i = 0;
	if( list_len(p) > 0){
		for(i = 0; i < list_len(p); i++){
			cprint(get_list(p)->nodes[i]);
		}
	}
	char buf[2048];
	fgets(buf, 2048, stdin);
	return str_new(buf, strlen(buf));
}

tm_obj tm_int(tm_obj p){
	tm_obj v = get_arg(p, 0, -1);
	if( v.type == TM_NUM ){
		return number_new( (int) get_num(v) );
	}else if( v.type == TM_STR ){
		return number_new( (int)atof(get_str(v)) );
	}
	tm_raise("tm_int: @ can not parse to int ", v);
	return obj_none;
}

tm_obj tm_float( tm_obj p){
	tm_obj v = get_arg(p, 0, -1);
	if( v.type == TM_NUM ){
		return v;
	}else if( v.type == TM_STR ){
		return number_new( atof(get_str(v)));
	}
	tm_raise("tm_float: @ can not parse to float", v);
	return obj_none;
}

tm_obj tm_range( tm_obj p){
    long start = 0;
    long end = 0;
    int inc;
    switch( list_len(p) ){
        case 1: start = 0; end = get_num( get_arg(p, 0, TM_NUM) ); inc = 1; break;
        case 2: start = get_num( get_arg(p, 0, TM_NUM )); end = get_num(get_arg(p,1,TM_NUM)); inc = 1; break;
        case 3: start = get_num( get_arg(p, 0, TM_NUM ));
                end = get_num( get_arg(p, 1, TM_NUM ));
                inc = get_num( get_arg(p, 2, TM_NUM ));
                break;
        default: tm_raise("range([n, [ n, [n]]]), but see %d arguments", list_len(p));
    }
    if( inc == 0) tm_raise("range(): increment can not be 0!");
    /* eg. (1, 10, -1),  (10, 1, 1) : range can not be the same signal */
    if( inc * ( end - start ) < 0 ) tm_raise("range(%d, %d, %d): not valid range!", start, end, inc);
    tm_obj list = list_new( (end - start) / inc );
	long i = 0;
	for(i = start; i < end; i+=inc){
		list_append( get_list(list), number_new(i));
	}
	return list;
}

tm_obj _merge(tm_obj des, tm_obj src){
	if( des.type != src.type || des.type != TM_DCT)
		tm_raise("merge: can not merge @ and @", des, src);
	tm_obj k,v;
	dict_iter_init(get_dict(src));
	while( dict_inext(get_dict(src), &k, &v)){
		tm_set( des, k, v);
	}
	return des;
}

/* import file */
/*
tm_obj tm_import( tm_obj p){
	tm_obj name = get_arg(p, 0, TM_STR);
	tm_obj arg1 = get_arg(p, 1, TM_STR);
	tm_obj mod;
    // already loaded the module, ( will check the compiled module next step)
	if(_tm_has( tm->modules, name) ) {
	 	mod = tm_get( tm->modules, name);
	}else{
        // not handle the module yet .
		mod = dict_new();
		// printf("new a module with empty value\n");
	}
	if( strcmp(get_str(arg1), "*") == 0 ){
		_merge( get_fnc_globals( tm->frames[tm->cur].fnc) ,  mod);
	}
	return obj_none;
}
*/

tm_obj load_module( tm_obj p){
    tm_obj name = get_arg(p, 0, TM_STR);
    tm_obj code = get_arg(p, 1, TM_STR);
    tm_obj mod;
    if( list_len(p) == 3){
        mod = module_new(name, get_arg(p, 2, TM_STR) , code );
    }else{
        mod = module_new(name, name , code );
    }
    tm_obj fnc = func_new(mod, obj_none, NULL);
    get_func(fnc)->pc = get_str(code);
    get_func(fnc)->name = obj__main__;
    return tm_eval( fnc , obj_none);
}

tm_obj get_last_frame_globals(tm_obj p){
    return get_fnc_globals( tm->frames[tm->cur-1].fnc ) ;
}


/* get globals */
tm_obj tm_globals(tm_obj p){
	return get_fnc_globals( tm->frames[tm->cur].fnc );
}

/* get object type */
tm_obj _tm_type( tm_obj o){
	static char info[200];
	switch( o.type ){
	case TM_NUM: sprintf(info, "<number %g>", get_num(o));break;
    case TM_STR:{
        sprintf(info, "<string \"%s\"%p>", _str_limit(o, 10), get_str(o));
        break;
    }
	case TM_DCT: sprintf(info, "<dict %d %p>",dict_len(o), get_dict(o));break;
    case TM_LST: sprintf(info, "<list %d %p>",list_len(o), get_list(o));break;
    case TM_FNC:
        sprintf(info, "<function %s %p>",_str_limit(get_func(o)->name,10), get_func(o));
        break;
	case TM_NON: sprintf(info, "<none>");break;
	default: sprintf(info, "<unknown %d %p>",o.type, o.value.ptr);break;
	}
	return str_new( info, strlen(info));
}

/* return the type and value of a object */
tm_obj _obj_info(tm_obj o){
	tm_obj str = _tm_type(o);
	tm_obj com = str_new(":", -1);
	str = tm_add(str, com);
	return tm_add(str, _tm_str(o));
}

tm_obj tm_exit( tm_obj p){
	longjmp(tm->buf, 2);
	return obj_none;
}

tm_obj tm_add_type_method(tm_obj p){
    tm_obj type = get_arg(p, 0, TM_STR);
    tm_obj fname = get_arg(p, 1, TM_STR);
    tm_obj fnc = get_arg(p, 2, TM_FNC);
    char*s = get_str(type);
    if( strcmp(s, "str") == 0) {
        tm_set(str_class, fname, fnc);
    }else if( strcmp(s, "list") == 0) {
        tm_set(list_class, fname, fnc);
    }else if( strcmp(s, "dict") == 0){
        tm_set(dict_class, fname, fnc);
    }else {
        tm_raise("add_type_method(), expect (str, str, fnc)");
    }
    return obj_none;
}

tm_obj tm_istype( tm_obj p){
	tm_obj arg0 = get_arg(p, 0, -1);
	int type = arg0.type;
	tm_obj arg1 = get_arg(p, 1, TM_STR);
    char *s = get_str(arg1);
	if( strcmp(s, "string") == 0 ){
		return number_new( type == TM_STR);
	}else if( strcmp(s, "list") == 0 ){
		return number_new( type == TM_LST);
	}else if( strcmp(s, "dict") == 0 ){
		return number_new( type == TM_DCT);
	}else if( strcmp(s, "function") == 0 ){
		return number_new( type == TM_FNC);
	}else if( strcmp(s, "none") == 0 ){
		return number_new( type == TM_NON);
	}else if ( strcmp(s, "number") == 0 ){
		return number_new( type == TM_NUM);
	}
	return obj_none;
}

tm_obj tm_makesure( tm_obj p){
	tm_obj arg0 = get_arg(p, 0, -1);
	tm_obj arg1;
	if( list_len(p) == 1){
		arg1 = obj_none;
	}else{
		arg1 = get_arg(p, 1, -1);
	}
	if( !_tm_bool(arg0)){
		tm_raise("AssertException, @", arg1);
	}
	return obj_none;
}

tm_obj _tm_dir( tm_obj o) {
    tm_obj lst = obj_none;
    switch( o.type ){
        case TM_NUM:lst = list_new(2);break;
        case TM_STR:lst = dict_keys( get_dict( str_class ) );break;
        case TM_LST:lst = dict_keys( get_dict( list_class ) );break;
        case TM_DCT:lst = dict_keys( get_dict( dict_class ));break;
        default: tm_raise("dir(), not supported type %d", o.type );
    }
    return lst;
}
tm_obj tm_dir(tm_obj p){
    return _tm_dir( get_arg(p, 0, -1));
}

tm_inline
tm_obj _tm_chr( int n ){
    if( n < 0 || n >= 256 ){
        tm_raise("chr(): index overflow");
    }
    return __chars__[n];
}

tm_obj tm_chr(tm_obj p){
    int n = get_num(get_arg(p, 0, TM_NUM));
    return _tm_chr( n );
}

tm_obj tm_ord(tm_obj p){
    tm_obj c = get_arg(p, 0, TM_STR);
    TM_ASSERT( get_str_len(c) == 1, "ord() expected a character");
    return number_new( (unsigned char) get_str(c)[0]);
}

/* create a temporary module and run it */
tm_obj btm_run(tm_obj p){
    tm_obj code = get_arg(p, 0, TM_STR);
    tm_obj mod = module_new(str_new("<shell>", -1), obj__main__ , code );
    tm_obj fnc = func_new(mod, obj_none, NULL);
    get_func(fnc)->pc = get_str(code);
    get_func(fnc)->name = obj__main__;
    return tm_eval( fnc , obj_none);
}

tm_obj tm_code8(tm_obj p) {
    int n = get_num( get_arg(p, 0, TM_NUM) );
    if( n < 0 || n > 255 ) tm_raise("code8(): expect number 0-255, but see %d", n);
    return __chars__[n];
}

tm_obj tm_code16(tm_obj p){
    int n = get_num( get_arg(p, 0, TM_NUM));
    if( n < 0 || n > 0xffff) tm_raise("tm_code16(): expect number 0-0xffff, but see %x", n);
    tm_obj nchar = str_new(NULL, 2);
    ((unsigned char*) get_str(nchar)) [0] = (unsigned char)((n >> 8) & 0xff);
    ((unsigned char*) get_str(nchar)) [1] = (unsigned char) (n & 0xff);
    return nchar;
}

tm_obj tm_codeF( tm_obj p){
    static double d = 0;
    d = get_num( get_arg(p, 0, TM_NUM) );
    char* val;
    val = (char*)&d;
    return str_new(val, sizeof(double));
}

tm_obj tm_clock( tm_obj p){
#ifdef _WIN32
    return number_new(clock());
#else
    return number_new(clock()/1000);
#endif
}

tm_obj btm_raise(tm_obj p){
    tm_raise("%o", get_arg(p,0,-1));
    return obj_none;
}

/*
tm_obj def_mod_global(tm_obj p){
    tm_obj mod = get_arg(p, 0, TM_STR);
    tm_obj name = get_arg(p, 1, TM_STR);
    tm_obj var = get_arg(p, 2, -1);
    if( !_tm_has(tm->modules, name) ){
        tm_raise("def_mod_global():can not find module %o", name);
    }
    mod = tm_get( tm->modules, mod);
    tm_set(mod, name, var);
    return obj_none;
}*/