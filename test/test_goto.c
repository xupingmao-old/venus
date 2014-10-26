
#include <stdio.h>

int main(){

    int i = 0;
    l1:
    i++;

    l2:
    i++;

    void* label = &&skip_label;
    goto * label;


    test_label:
    printf("test_label\n");

    
    printf("i = %d\n", i);

    skip_label:
    printf("skip everything\n");
    return 0;
}