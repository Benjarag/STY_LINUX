#include <stdlib.h>
#include <stdio.h>

int main() {
	int *intPtr;

	intPtr = (int *)malloc(sizeof(int));
	if(intPtr == NULL) { printf("malloc failed\n"); exit(1); }

	*intPtr = 42;

	printf("The value is %d\n", *intPtr);
	free(intPtr);
}
