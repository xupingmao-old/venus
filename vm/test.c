#include "tm.h"
#include "./test/test_map.c"
/*tm_obj tm_c_call(tm_vm* tm, char* mod, char* func, tm_obj params);

void test_func_call(tm_vm* tm){
	tm_obj arg = tm_arg1(tm, string_new(tm, "test"));
	tm_c_call(tm, 0, "print", arg);
	tm_obj n = tm_number(500);
	arg = tm_arg1(tm, n);
	tm_c_call(tm, 0, "sleep", arg);
	tm_c_call(tm, 0, "print", arg);
}

void test_print(tm_vm* tm){
	tm_set(  tm->modules, string_new(tm, "io"), string_new(tm, "test"));
	tm_obj args = tm_arg1(tm, tm->modules);
	tm_print(tm, args);



	tm_obj d = string_new(tm, "hellworld");

	tm_obj o;
	o.type = TM_MAP;
	o.value.map = tm->strings;
	tm_print(tm, tm_arg1(tm, o));
}

void test_add(tm_vm* tm){
	tm_obj a = string_new(tm, "hell");
	tm_obj b = string_new(tm, "world");
	tm_obj c = tm_add(tm, a, b);
	tm_print(tm, tm_arg1(tm, c));
}

void test_list_insert(tm_vm* tm){
	tm_obj list = list_new(tm);
	tm_obj a, b, c;
	a = string_new(tm, "test1");
	b = string_new(tm, "test2");
	c = string_new(tm, "test3");
	list_push(tm, list.value.list, a);
	list_insert(tm, list.value.list, 0, b);
	_tm_print(list);
}

void test(tm_vm* tm){
//	test_func_call(tm);
	test_list_insert();
}
*/