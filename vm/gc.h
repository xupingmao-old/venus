
#ifndef gc_h
#define gc_h

#include "tm.h"

void gc_init();
tm_obj gc_track(tm_obj obj);
void gc_free();
void gc_info();

#endif
