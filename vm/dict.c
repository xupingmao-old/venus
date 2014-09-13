#include "tm.h"

tm_dict* dict_new_(tm_vm* tm) {
#if DEBUG_GC
	int old = tm->allocated_mem;
#endif
	tm_dict* dict = tm_alloc(tm, sizeof(tm_dict));
	dict->keys = list_new_(tm);
	dict->values = list_new_(tm);
#if DEBUG_GC
	printf("dict_new: from %d to %d, allocated: %d\n", old, tm->allocated_mem,
			tm->allocated_mem - old);
#endif
	return dict;
}

tm_obj dict_new(tm_vm* tm) {
	tm_obj o;
	o.type = TM_DCT;
	o.value.dict = dict_new_(tm);
	return gc_track(tm, o);
}

void dict_free(tm_vm* tm, tm_dict* dict) {
#if DEBUG_GC
	int old = tm->allocated_mem;
#endif
	list_free(tm, dict->keys);
	list_free(tm, dict->values);
	free(dict);
	tm->allocated_mem -= sizeof(tm_dict);
#if DEBUG_GC
	printf("free dict %x, from %d to %d, freed: %d\n", dict, old, tm->allocated_mem,
			old - tm->allocated_mem);
#endif
}

int dict_find(tm_vm* tm, tm_dict* dict, tm_obj key) {
	int i;
	int len = dict->keys->len;
	tm_obj* keys = dict->keys->nodes;
	tm_obj* vals = dict->values->nodes;
	for (i = 0; i < len; i++) {
		if (tm_eq(keys[i], key)) {
			return i;
		}
	}
	return -1;
}

void dict_set(tm_vm* tm, tm_dict* dict, tm_obj k, tm_obj v) {
	int p = dict_find(tm, dict, k);
	if (p != -1) {
		dict->values->nodes[p] = v;
	} else {
		list_push(tm, dict->keys, k);
		list_push(tm, dict->values, v);
	}
}

tm_obj dict_get(tm_vm* tm, tm_dict* dict, tm_obj k) {
	int p = dict_find(tm, dict, k);
	if (p != -1) {
		return dict->values->nodes[p];
	} else {
		tm->error = string_new2(tm, "dict_get: keyError ", k);
		tm_raise(tm);
	}
	return tm->none;
}
