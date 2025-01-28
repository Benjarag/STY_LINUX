#include <stdio.h>
#include <stdlib.h>

// First argument argv[0] is the program name, argv[1],... the real arguments
// argc is the total number of arguments, including the program name (i.e., it is always at least 1)
int main(int argc, char *argv[]) {
    int i, j, max;
    int *primes = NULL; // size not known at compile time, needs to be allocated dynamically
    
    if (argc < 2) {
        printf("Usage: %s <number>\n", argv[0]);
        exit(1);
    }
    
    max = atoi(argv[1]);
    printf("Finding prime numbers from 1 to %d\n", max);
    
    ////
    /////////////////////////////
    //// ADD YOUR CODE HERE
    
    primes = (int *)malloc((max + 1) * sizeof(int));
    if (primes == NULL) { printf("alloc failed\n"); exit(1); }
    
    /////////////////////////////
    ////
  
    // Populate the array with natural numbers
    for (i = 2; i <= max; i++) {
        primes[i] = i;
    }
    
    // Standard prime number sieve
    for (i = 2; i * i <= max; i++) {
        if (primes[i] != 0) {
            for (j = 2; j < max; j++) {
                if (primes[i] * j > max)
                    break;
                else
                    primes[primes[i] * j] = 0;
            }
        }
    }
    
    // Print the prime numbers
    for (i = 2; i <= max; i++) {
        if (primes[i] != 0)
            printf("%d\n", primes[i]);
    }
    
    // Free allocated memory
    free(primes);
    return 0;
}