
#include "tm.h"

void test_map(tm_vm*tm){
	tm_obj map = map_new(tm);
	tm_set(map, S("abc"), N(123));
	tm_set(map, S("bcd"), N(456));
	tm_set(map, S("def"), N(34.34));
	tm_set(map, S("abc"), S("test"));
	tm_obj k,v;
	while( map_inext(tm, map.value.map, &k, &v)){
		cprint(k);
		printf(":");
		_tm_print(v);
	}
	map_print(map.value.map);
	printf("\n\n\n");
	tm_set( map, S("aaa"), S("tetss"));
	tm_set( map, S("bbb"),  S("jfjjjj"));
	tm_set( map, S("ccc"), S("value32"));
	tm_set( map, S("ggg"), S("tetss"));
	tm_set( map, S("abc"),  S("jfjjjj"));
	tm_set( map, S("ddd"), S("value322"));

	map_print(map.value.map);
}
