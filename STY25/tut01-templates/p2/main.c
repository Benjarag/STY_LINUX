#include "testlib.h"
#include "countchr.h"

int main() {
	static char *lorem = "Lorem ipsum dolor sit amet";

	test_start("countchr.c");

	test_equals_int(countchr(lorem, 'e'), 2);
	test_equals_int(countchr(lorem, 'o'), 3);
	test_equals_int(countchr(lorem, 'z'), 0);
	test_equals_int(countchr(lorem, '\0'), 0);
	test_equals_int(countchr("", 'e'), 0);

	return test_end();
}
