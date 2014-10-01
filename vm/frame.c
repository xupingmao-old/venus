/**

date : 2014-9-2
**/

#include "tm.h"

tm_frame* frame_new( char* code, tm_obj params){
    return NULL;
}

// 一个模块的常量
void push_constant(tm_obj mod, tm_obj v){
    tm_obj constants = tm_get( mod, tm->constants);
    int i = list_index( get_list(constants), v);
    if( i != -1 ){
        tm_push(get_list(constants), v);
    }
}

#define next_char( s ) *s++
#define next_byte( s ) *s++
#define next_short( s ) ((int) (*s++) << 8) + (int) (*s++)
#define read_number( v, s) v = *(double*)s; s+=sizeof(double);
#define PUSH( x ) *(top++) = x
#define POP() *(top--)

#define CASE( code, body ) case code :  body ; break;
tm_obj tm_eval( tm_frame* frame){
    unsigned char* code = frame->code;
    unsigned char* s = code;
    tm_obj* constants = get_list(frame->constants)->nodes;
    tm_obj* locals = get_list(frame->locals)->nodes;
    tm_obj* top = frame->stack;
    tm_map* g = get_map(frame->globals);
    tm_obj mod = frame->mod;

    tm_obj x, k, v;
    int i;
    switch( next_char(s) ){
        CASE(PUSH_NUMBER, {
            double d;
            v = tm_number(d);
            read_number( d, s);
            push_constant( mod , v);
        })
        CASE(PUSH_STRING, {
            int len = next_short( s );
            v = tm_string( s, len);
            s+=len;
            push_constant( mod, v);
        })
    	CASE(LOAD_CONST, {
    	    i = next_short( s );
    	    PUSH( con[ i ] );
        })
        CASE(LOAD_LOCAL, {
            i = next_char(s);
            PUSH( locals[i] )
        })
        CASE(LOAD_GLOBAL, {
            i = next_short(s);
            k = constants[ i ];
            PUSH( tm_get(g, k) );
        })
        CASE(STORE_LOCAL, {
            i = next_char( s );
            locals[i] = POP()
        })
        CASE(STORE_GLOBAL ,{
            i = next_short( s );
            k = constants[ i ];
            x = POP();
            tm_set( g, k, x );
        })
        CASE(CALL, {
            i = next_byte( s );
            tm_obj p= list_new(i);
            tm_list* _p = get_list(p);
            tm_obj* j;
            for(j = top - i; j < top; j++){
                list_append(_p, *j);
            }
            top-=i;
            x = POP(); // function
            tm_call(x, p);
        })
    }
}
