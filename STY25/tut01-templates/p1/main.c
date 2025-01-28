#include "testlib.h"
#include "greet.h"

int main() {
	test_start("greet.c");

	greet(10);

	return test_end();

}
