#include "testlib.h"
#include "malloc.h"
#include <stdio.h>

int main()
{
	test_start("malloc.c");
	initAllocator();

	void *a = my_malloc(2016);
	my_malloc(3);
	my_malloc(28372);
	void *b = my_malloc(16);
	void *c = my_malloc(5);
	my_malloc(2112);

	dumpAllocator();

	my_free(b);
	my_free(c);
	my_free(a);
	dumpAllocator();
	my_malloc(16746540);
	my_malloc(1);
	my_malloc(1);
	my_malloc(1);
	my_malloc(1);
	dumpAllocator();
	my_malloc(16775168);
	my_malloc(1);
	dumpAllocator();
	
	printf("\nNote: This test always passes. It does not do any checks.\n It just prints the memory layout after some operations\n");
	return test_end();
}
