

#include "tm.h"

void print_tags(tm_module *mod){
  int i;
  for(i = 0; i < mod->tagsize; i++){
      printf("TAG %d : %p\n", i, mod->tags[i]);
  }
}

/**
   store tag  index and value to module
**/
void store_tag( tm_module* mod, int idx, unsigned char* pos){
  if( mod->tags == NULL ){
    tm_raise("code_check(): invalid code, miss TAGSIZE information");
  }else if( idx >= mod->tagsize ){
    tm_raise("code_check(): invalid code, error with TAG value %d , but tagsize = %d, s0 = %d, s1 = %d", 
      idx, mod->tagsize, *(pos-2), *(pos-1) );
  }
  // printf("tags:%d -> %s\n", idx, pos);
  mod->tags[idx] = pos;
}

unsigned char** init_tags(int size){
    if( size <= 0) return NULL;
  unsigned char** tags = tm_alloc(size * sizeof(unsigned char*));
  int i = 0;for(i = 0; i < size; i++){
    tags[i] = NULL;
  }
  return tags;
}

/** check code, compute stacksize, tags **/
struct tm_check_result_st code_check(tm_obj _mod,  unsigned char*s , int isFuncDef){
    struct tm_check_result_st st;
    int len = 0;
    int stacksize = 1;
    int curstack = 0;
    int temp = 0;
    int def_count = 0;
    int maxlocals = 0;
    int maxstack = 0;
    st.pc = s;
    if( isFuncDef )
      def_count = 1;
    tm_module* mod = get_mod(_mod);
    while(1){
        int ins = next_byte(s);
        int val = next_short(s);
        len+=3;
        switch(ins){
        case NEW_STRING:
        case NEW_NUMBER:
            len += val;
            s+=val;
            break;
        case ADD:case SUB:case MUL:case DIV:case MOD:
		case GT:case LT:case GTEQ:case LTEQ:case EQEQ:case NOTEQ:
		case IN:case NOTIN:case GET:case POP:
		case RETURN:case AND: case OR:
        case LIST_APPEND:
        case POP_JUMP_ON_TRUE:
        case POP_JUMP_ON_FALSE:
			stacksize--;
			break;
		case NEG : case NOT:
        case LOAD_PARAMS:
        case JUMP_ON_FALSE:
        case JUMP_ON_TRUE:
        case JUMP:
        case TM_FOR:
        case SETJUMP:
        case LIST:
        case DICT:
            break;
        case SET:
            stacksize-=3;
            break;
        case DICT_SET:
            stacksize-=2;
			break;
        case STORE_LOCAL:
        case LOAD_LOCAL:
			if( LOAD_LOCAL == ins) {
				stacksize++;
			}else{
				stacksize--;
			}
			maxstack = max( maxstack, stacksize);
			if( isFuncDef){
				maxlocals = max(maxlocals, val);
			}
			break;
        case CALL:
			stacksize-=val;
			break;
        case LOAD_CONSTANT:
        case LOAD_GLOBAL:
        case STORE_GLOBAL:
			if( STORE_GLOBAL == ins){
				stacksize--;
			}else{
				stacksize++;
			}
			maxstack = max(maxstack, stacksize);
			break;
        case TM_DEF:
			stacksize++;
			maxstack = max( maxstack, stacksize);
            if( isFuncDef )
              def_count++;
            break;
        case TAGSIZE:
            mod->tagsize = val;
            // printf("tagsize = %d\n", mod->tagsize );
            mod->tags = init_tags(mod->tagsize);
            break;
        case TAG:
			store_tag(mod, val, s);
			break;
        case TM_EOF:
			stacksize--;
			if( isFuncDef){
				def_count--;
			// printf("def_count = %d\n",def_count );
			if( def_count == 0)
				goto ret;
			}
			break;
        case TM_EOP:
            mod->checked = 1;
            goto ret;
        default:
			tm_raise("code_check: BAD INSTRUCTION @, handled code len = @", number_new(ins),number_new(len));
        }
    }
    ret:
    //printf("maxlocals = %d, maxstack = %d \n", *maxlocals, maxstack);
    st.len = len;
    st.maxlocals = maxlocals;
    st.maxstack = maxstack;
    return st;
}
