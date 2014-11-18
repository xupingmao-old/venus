
#include "profile.c"

#define TIMES 100000000
void test_stack(){
    double dstack[1000];
    double *stack = dstack;
    double rs;
    for(long i = 0; i < TIMES; i++){
        *(stack++) = 1;
        *(stack++) = 2;
        stack[3] = *(--stack) + *(--stack);
    }
}

void test_reg(){
    double stack[1000];
    double rs;
    stack[1] = 1;
    stack[2] = 2;
    for(long i = 0; i < TIMES; i++){
        stack[3] = stack[1] + stack[2];
    }
}

int main(){
    profile(test_stack, "test_stack");
    profile(test_reg, "test_reg");
    return 0;
}