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
    tm_obj constants = tm_get( mod, string_new("constants", 0));
    int i = list_index( get_list(constants), v);
    if( i != -1 ){
        list_append(get_list(constants), v);
    }
}

#define next_char( s ) *s++
#define next_byte( s ) *s++
#define next_short( s ) ((int) (*s++) << 8) + (int) (*s++)
#define read_number( v, s) v = *(double*)s; s+=sizeof(double);
#define TM_PUSH( x ) *(top++) = x
#define TM_POP() *(top--)

#define CASE( code, body ) case code :  body ; break;
tm_obj tm_eval( tm_frame* frame){
    unsigned char* code = get_str(frame->code);
    unsigned char* s = code;
    // constants will be built in modules.
    // get constants from function object.
    tm_obj* constants = get_list(frame->constants)->nodes;
    tm_obj* locals = frame->locals;
    tm_obj* top = frame->stack;
    tm_obj g = frame->globals;
    // module name will be compiled into bytecode;
    tm_obj mod = tm->none;

    tm_obj x, k, v;
    int i;
    switch( next_char(s) ){
        case NEW_NUMBER: {
            double d;
            v = tm_number(d);
            read_number( d, s);
            push_constant( mod , v);
        }break;
        case NEW_STRING: {
            int len = next_short( s );
            v = string_new( s, len);
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
            tm_call(x, p);
        }break;
    }
}
