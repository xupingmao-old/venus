/**

date : 2014-9-2
**/

#include "tm.h"
#include "instruction.h"

tm_frame* frame_new( tm_func* func){
    tm->cur++;
    tm_frame* f = tm->frames + tm->cur;
    f->code = func->code;
    //f->mod = func->mod;
    return f;
}

// 一个模块的常量
void push_constant(tm_obj mod, tm_obj v){
    tm_obj constants = tm_get( mod, string_new("__constants__", 0));
    int i = list_index( get_list(constants), v);
    if( i == -1 ){
        list_append(get_list(constants), v);
    }
}

tm_obj* get_constants(tm_obj mod){
    tm_obj __constants__ = string_new("__constants__", 0);
    tm_map* m = get_map(mod);
    tm_obj v;
    if( map_iget(m, mod, &v) ){
    }else{
        v = list_new(20);
        list_append( get_list(v), tm->none);
        tm_set(mod, __constants__, v);
    }
    return get_list(v)->nodes;
}

#define next_char( s ) *s++
#define next_byte( s ) *s++
#define next_short( s ) ((int) (*s++) << 8) + (int) (*s++)
#define read_number( v, s) v = *(double*)s; s+=sizeof(double);
#define TM_PUSH( x ) *(top++) = x
#define TM_POP() *(top--)

#define CASE( code, body ) case code :  body ; break;

#define PRINT_INS 1

tm_obj tm_eval( tm_obj mod ){
    tm_obj __code__ = string_new("__code__", 0);
    tm_obj g = mod;
    tm_obj code = tm_get(mod, __code__);
    char* s = get_str(code);
    // constants will be built in modules.
    // get constants from function object.
    tm_obj* constants = get_constants(mod);

    tm_frame* f = tm->frames + tm->cur;
    tm_obj* locals = f->locals;
    tm_obj* top = f->stack;

    tm_obj x, k, v;
    int i;
    switch( next_char(s) ){
        case NEW_NUMBER: {
            double d;
            read_number( d, s);
            #if PRINT_INS
                printf("NEW_NUMBER %g\n", d);
            #endif
            v = tm_number(d);
            push_constant( mod , v);
        }break;
        case NEW_STRING: {
            int len = next_short( s );
            v = string_new( s, len);
            #if PRINT_INS
                tm_printf("NEW_STRING @\n", v);
            #endif
            s+=len;
            push_constant( mod, v);
        }break;
    	case LOAD_CONSTANT: {
    	    i = next_short( s );
    	    TM_PUSH( constants[ i ] );
        }break;
        case LOAD_LOCAL: {
            i = next_char(s);
            TM_PUSH( locals[i] );
        }break;
        case LOAD_GLOBAL: {
            i = next_short(s);
            k = constants[ i ];
            TM_PUSH( tm_get(g, k) );
        }break;
        case STORE_LOCAL: {
            i = next_char( s );
            locals[i] = TM_POP();
        }break;
        case STORE_GLOBAL:{
            i = next_short( s );
            k = constants[ i ];
            x = TM_POP();
            tm_set( g, k, x );
        }break;
        case CALL: {
            i = next_byte( s );
            tm_obj p= list_new(i);
            tm_list* _p = get_list(p);
            tm_obj* j;
            for(j = top - i; j < top; j++){
                list_append(_p, *j);
            }
            top-=i;
            x = TM_POP(); // function
            v = tm_call(x, p);
            TM_PUSH(v);
        }break;
    }
}
