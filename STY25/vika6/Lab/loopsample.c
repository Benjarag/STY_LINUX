#include <stdio.h>

int main() 
{
    volatile int x = 0;
    for(int i=0; i<5000; i++) {
    x += i;
    }
    printf("x = %d\n", x);
}
