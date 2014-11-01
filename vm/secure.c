

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
    tm_raise("code_check: invalid code, miss TAGSIZE information");
  }else if( idx >= mod->tagsize ){
    tm_raise("code_check: invalid code, error with TAG value @ , but tagsize = @, s0 = @, s1 = @", 
      number_new(idx), number_new(mod->tagsize), number_new(*(pos-2)), number_new(*(pos-1)) );
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
int code_check(tm_obj _mod,  unsigned char*s , int isFuncDef, int* maxlocals, int *maxstack){
    int len = 0;
    int idx;
    int stacksize = 1;
    int curstack = 0;
    int temp = 0;
    int def_count = 0;
    if( isFuncDef )
      def_count = 1;
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
		case GT:case LT:case GTEQ:case LTEQ:case EQEQ:case NOTEQ:
		case IN:case NOTIN:case GET:case POP:
		case RETURN:case AND: case OR:
        case LIST_APPEND:
			len++;
			stacksize--;
			break;
        case DICT_SET:
            len++;
            stacksize--;
            break;
		case NEG : case NOT:
        case LOAD_PARAMS:
            len++;
            break;
        case SET:
			len++;
            stacksize-=2;
			break;
        case STORE_LOCAL:
        case LOAD_LOCAL:
			idx = next_byte(s);
			len+=2;
			if( LOAD_LOCAL == ins) {
				stacksize++;
			}else{
				stacksize--;
			}
			*maxstack = max( *maxstack, stacksize);
			if( isFuncDef){
				*maxlocals = max(*maxlocals, idx);
			}
			break;
        case CALL:
			idx = next_byte(s);
			stacksize-=idx;
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
			idx = next_short(s);
			if( STORE_GLOBAL == ins){
				stacksize--;
			}else{
				stacksize++;
			}
			*maxstack = max(*maxstack, stacksize);
			len+=3;
			break;
        case TM_DEF:
			stacksize++;
			*maxstack = max( *maxstack, stacksize);
            next_short(s);
            len+=3;
            if( isFuncDef )
              def_count++;
            break;
        case POP_JUMP_ON_TRUE:
        case POP_JUMP_ON_FALSE:
			idx = next_short(s);
			len+=3;
			stacksize--;
			break;
        case JUMP_ON_FALSE:
        case JUMP_ON_TRUE:
        case JUMP:
        case TM_FOR:
            idx = next_short(s);
            len+=3;
            break;
        case TAGSIZE:
            mod->tagsize = next_short(s);
            // printf("tagsize = %d\n", mod->tagsize );
            mod->tags = init_tags(mod->tagsize);
            len+=3;
            break;
        case TAG:
			idx = next_short(s);
			len+=3;
			store_tag(mod, idx, s);
			break;
        case TM_EOF:
			len++;
			stacksize--;
			if( isFuncDef){
				def_count--;
			// printf("def_count = %d\n",def_count );
			if( def_count == 0)
				goto ret;
			}
			break;
        case TM_EOP:
            len++;
            mod->checked = 1;
            goto ret;
        default:
			tm_raise("code_check: BAD INSTRUCTION @, handled code len = @", number_new(ins),number_new(len));
        }
    }
    ret:
    //printf("maxlocals = %d, maxstack = %d \n", *maxlocals, *maxstack);
    return len;
}
