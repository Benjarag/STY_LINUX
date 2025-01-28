#include "countchr.h"

int countchr(char *string, char c) {
	// (void) string;
	// (void) c;
	// Add code here.
	int count = 0;
	for (int i = 0; string[i] != '\0'; i++) {
		if (string[i] == c)
			count++;
	}


	return count;
}