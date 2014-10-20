

#include "tm.h"



/**
   store tag  index and value to module
**/
void store_tag( tm_module* mod, int idx, char* pos){
  if( mod->tags == NULL || idx > mod->tagsize ){
    tm_raise("code_check: invalid code, error with TAG value");
  }else if( idx >= mod->tagsize ){
    tm_raise("code_check: invalid code, miss TAGSIZE information");
  }
  mod->tags[idx] = pos;
}


/** check code, compute stacksize, tags **/
int code_check(tm_obj _mod,  char*s , int isFuncDef){
    int len = 0;
    int idx;
    int stacksize = 100;
    int curstack = 0;
    int temp;
    tm_module* mod = get_mod(_mod);
    while(1){
        int ins = next_byte(s);
        switch(ins){
        case NEW_STRING:
            temp = next_short(s);
            len+=3+temp;
            s+=temp;
            break;
        case NEW_NUMBER:
            len+=1 + sizeof(double);
            s+=sizeof( double);
            break;
        case ADD:case SUB:case MUL:case DIV:case MOD:
        case LOAD_PARAMS:
        case GT:case LT:case GTEQ:case LTEQ:case EQEQ:case NOTEQ:
        case GET:case SET:case IN:
        case POP:
        case RETURN:
        case TM_DEF:
          len++;
          break;
        case STORE_LOCAL:
        case LOAD_LOCAL:
        case CALL:
          next_byte(s);
          len+=2;
          break;
        case LIST:
        case DICT:
            next_byte(s);
            len+=2;
            break;
        case LOAD_CONSTANT:
        case LOAD_GLOBAL:
        case STORE_GLOBAL:
            next_short(s);
            len+=3;
            break;
        case POP_JUMP_ON_TRUE:
        case POP_JUMP_ON_FALSE:
        case JUMP_ON_FALSE:
        case JUMP_ON_TRUE:
        case JUMP:
        case TM_FOR:
            next_short(s);
            len+=3;
            break;
        case TAGSIZE:
            mod->tagsize = next_short(s);
            mod->tags = tm_alloc( mod->tagsize * sizeof( char* ));
            break;
        case TAG:
          idx = next_short(s);
          len+=3;
          store_tag(mod, idx, s);
          break;
        case TM_EOF:
          len++;
          if( isFuncDef )
            goto ret;
        case TM_EOP:
            len++;
            mod->checked = 1;
            goto ret;
        default:
          tm_raise("code_check: BAD INSTRUCTION @", number_new(ins));
        }
    }
    ret:
    return len;
}