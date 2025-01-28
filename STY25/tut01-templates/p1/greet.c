#include <stdio.h>
#include "greet.h"

void greet(int32_t times) {
    for (int i = 1; i <= times; i++) {
        printf("%d: Hello World!\n", i);
    }
}
