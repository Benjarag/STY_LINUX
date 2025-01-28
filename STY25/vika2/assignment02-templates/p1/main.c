#include <stdio.h>  
#include <stdlib.h>
#include "func.h"

int main()
{
    int *parg, result;

    parg = (int*)malloc(sizeof(int));
    if (parg == NULL) exit(1);
    *parg = 10;
    
    result = func(parg);
    free(parg);

    return result;
}
