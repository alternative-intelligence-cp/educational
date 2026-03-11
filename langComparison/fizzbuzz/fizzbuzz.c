/*
 * FizzBuzz - C
 *
 * Classic: count 1-30, print Fizz (÷3), Buzz (÷5), FizzBuzz (÷15), else number.
 *
 * Build:
 *   gcc -o fizzbuzz fizzbuzz.c && ./fizzbuzz
 *
 * Notice:
 *   - Manual printf/puts for output; no string interpolation
 *   - else-if chain reads naturally top-to-bottom
 *   - Zero type overhead; runs as fast as the hardware allows
 *   - No bounds checking, no overflow checking anywhere
 */

#include <stdio.h>

int main(void) {
    for (int i = 1; i <= 30; i++) {
        if      (i % 15 == 0) puts("FizzBuzz");
        else if (i %  3 == 0) puts("Fizz");
        else if (i %  5 == 0) puts("Buzz");
        else                  printf("%d\n", i);
    }
    return 0;
}
