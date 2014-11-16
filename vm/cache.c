
int tm_cache_push_string(tm_frame* f){
    return 1;
}

int tm_cache_push_number(tm_frame*f){
    return 1;
}

int tm_cache_add( tm_frame*f){
    tm_obj x, y;
    x = *(f->top);
    y = *(f->top);
    *(++f->top) = tm_add(x,y);
    f->pc++;
    return 1;
}

