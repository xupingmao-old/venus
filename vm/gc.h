
#ifndef gc_h
#define gc_h

#include "tm.h"

void gc_init(tm_vm* tm);
tm_obj gc_track(tm_vm* tm, tm_obj obj);

void gc_free(tm_vm* tm);

#endif
