
#include "../tm.h"

void test_dict(){
	tm_obj dict = dict_new();
	tm_set(dict, S("abc"), N(123));
	tm_set(dict, S("bcd"), N(456));
	tm_set(dict, S("def"), N(34.34));
	tm_set(dict, S("abc"), S("test"));
	dict_print(get_dict(dict));
	cprintln(dict);
	// cprint(dict);
	// printf("\n\n\n");
	tm_set( dict, S("aaa"), S("tetss"));
	tm_set( dict, S("bbb"),  S("jfjjjj"));
	tm_set( dict, S("ccc"), S("value32"));
	tm_set( dict, S("ggg"), S("tetss"));
	tm_set( dict, S("abc"),  S("jfjjjj"));
	tm_set( dict, S("ddd"), S("value322"));

	dict_print(get_dict(dict));
	cprint(dict);

	tm_obj mm = dict_new();
	tm_set(mm , S("testfor"), S("testwhile"));
}
