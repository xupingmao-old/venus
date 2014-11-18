
#include <stdarg.h>
#include <time.h>

long profile( void (*fnc) (), char* name, ... ){
    static int printinfo = 0;
    long t1,t2,used;
    t1 = clock();
    fnc();
    t2 = clock();
    used = t2-t1;
    if( !printinfo ){
        printf("%-20s,%-20s\n", "function", "time");
        printinfo = 1;
    }
    printf("%-20s, %-20ld\n", name, used);
    return used;
}