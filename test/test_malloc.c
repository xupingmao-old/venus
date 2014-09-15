/*************************************************************************
    > File Name: test_malloc.c
    > Author: xupingmao
    > Mail: 578749341@qq.com 
    > Created Time: 2014年09月14日 星期日 01时42分06秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>

int test1(int i, int j){
    return i * j + 21;
}

void* test2(){
    return malloc( 20 );
}

int main(){

long times = 1000000;

long i;

time_t t1 = clock();
for(i = 0;i < times; i++){
test1(10,30);
}
time_t t2 = clock();
printf("test1 -> %ld\n", t2-t1);


t1 = clock();
for(i = 0; i < times; i++){
void* i = test2();
free(i);
}

t2 = clock();
printf("test2 -> %ld\n", t2-t1);
return 0;
}
