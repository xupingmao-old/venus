#include <setjmp.h>

#include <stdio.h>

void test_error(jmp_buf buf) {
	longjmp(buf, 3);
}

int main() {
	jmp_buf buf;
	int res = setjmp(buf);
	if (res == 0) {

	} else {
		printf("error occurred code is %d \n", res);
		return 1;
	}

	test_error(buf);

	return 0;
}

