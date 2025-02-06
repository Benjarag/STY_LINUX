#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// Divide an integer by 4 using bitwise shift
int divide_by_4(int n) {
  return n >> 2;  // Right shift by 2 is equivalent to division by 4
}

// Multiply an integer by 8 using bitwise shift
int multiply_by_8(int n) {
  return n << 3;  // Left shift by 3 is equivalent to multiplication by 8
}

// Round down to the nearest multiple of 1024 (1024 = 2^10)
int round_down_1024(int n) {
  return n & ~1023; // Clears the last 10 bits
}

// Round up to the nearest multiple of 1024
int round_up_1024(int n) {
  return round_down_1024(n+1023); // Add 1023, then clear last 10 bits
}

// Check if a number is a power of 2
bool is_power_of_2(int n) {
  return n > 0 && !(n & (n - 1)); // remember, 0 is not power of 2
}

// Check if the two least significant bits are both set
bool least_two_bits_set(int n) {
  return !((n & 3) ^ 3);   // 3 in binary is 11, checking if both LSBs are 1
}

// Round up an integer to the nearest multiple of 16
uint64_t roundUp(uint64_t n) {
  return (n+15) & ~15; // Add 15, then clear the last 4 bits
}



int main() {
    int x = 19;
    printf("divide_by_4(%d) = %d\n", x, divide_by_4(x));
    printf("multiply_by_8(%d) = %d\n", x, multiply_by_8(x));
    printf("round_down_1024(%d) = %d\n", x, round_down_1024(x));
    printf("round_up_1024(%d) = %d\n", x, round_up_1024(x));
    printf("is_power_of_2(%d) = %d\n", x, is_power_of_2(x));
    printf("least_two_bits_set(%d) = %d\n", x, least_two_bits_set(x));
    printf("roundUp(%lu) = %lu\n", (uint64_t)x, roundUp(x));
    return 0;
}
