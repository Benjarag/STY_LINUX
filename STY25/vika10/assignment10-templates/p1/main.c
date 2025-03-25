#include "testlib.h"
#include "ls.h"
#include <stdio.h>
#include <unistd.h>


void printLine(size_t size, const char *mode, const char* name, const char *typestr)
{
	printf("%s %13lu  %s%s\n", mode, size, name, typestr);
}


int main()
{
	test_start("ls.c");

	printf("All files in current directory:\n");
	printf("permissions         size  name and type\n");
	
	test_equals_int(list(".", 0), 0);

	printf("\n");
	printf("All files in /home/sty25/A10/sampledir, recursively:\n");
	printf("permissions         size  name and type\n");
	test_equals_int(list("/home/sty25/A10/sampledir", 1), 0);

	return test_end();
}
