#include "testlib.h"
#include "execute.h"
#include <stdlib.h>
#include <unistd.h>

int main()
{
    test_start("execute.c");

    char *argv1[] = {
        "/",
        NULL
    };
    int result1 = execute("/bin/ls", argv1);
    test_equals_int(result1, 0);

    char * argv2[] = {
        NULL
    };
    int result2 = execute("/bin/false", argv2);
    test_equals_int(result2, 1);

    return test_end();
}
