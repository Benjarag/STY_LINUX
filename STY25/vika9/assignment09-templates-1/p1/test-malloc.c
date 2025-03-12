#include "testlib.h"
#include "malloc.h"
#include <stdlib.h>
#include <omp.h>


/* Return true (1): all ok, false (0): something is wrong */
int allocator_test()
{
	initAllocator();
	
	void *my_block[100];

	#pragma omp parallel for // tells the compiler
	for (int i = 0; i < 100; i++) {
		// my_malloc
		my_block[i] = my_malloc(22 + (100-i)*3);
	}

	#pragma omp parallel for // tells the compiler
	for (int i = 0; i < 100; i++) {
		// my_free
		my_free(my_block[i]);
	}
	// void *a = my_malloc(2016);
	// my_malloc(3);
	// my_malloc(28372);
	// void *b = my_malloc(16);
	// void *c = my_malloc(5);
	// my_malloc(2112);
	
	// dumpAllocator();
	
	// my_free(b);
	// my_free(c);
	// my_free(a);
	// dumpAllocator();
	// my_malloc(16746540);
	// my_malloc(1);
	// my_malloc(1);
	// my_malloc(1);
	// my_malloc(1);
	// dumpAllocator();
	// my_malloc(16775168);
	// my_malloc(1);
	// dumpAllocator();
	
	return 1;
}

