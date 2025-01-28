#include "print.h"
#include <stdio.h>  // for printf

void print_line(int64_t number, char *string)
{
    (void) number;
    (void) string;
    // Add code here.

    printf("%" PRId64 " %s\n", number, string);  

}
