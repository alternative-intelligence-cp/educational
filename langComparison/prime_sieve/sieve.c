/*
 * Prime Sieve - C (Sieve of Eratosthenes)
 *
 * Find all prime numbers up to N using the classic sieve algorithm.
 *
 * Category: Speed — memory layout, array access patterns
 *
 * Build:
 *   gcc -O2 -o sieve sieve.c && ./sieve
 *
 * Notice:
 *   - A char array is used as a boolean flag array (1 byte per element)
 *   - memset initializes the whole array to 1 in a single call
 *   - The inner loop starts at i*i, not 2*i — an important optimization
 *   - sizeof(sieve) works at compile time because sieve has a known size here
 *   - Zero overhead: the array sits directly in BSS; no GC, no bounds checks
 *   - Access to sieve[j] with a variable j is direct hardware array indexing
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#define N 1000

int main(void) {
    char is_prime[N + 1];
    memset(is_prime, 1, sizeof(is_prime));
    is_prime[0] = 0;
    is_prime[1] = 0;

    for (int i = 2; (long long)i * i <= N; i++) {
        if (is_prime[i]) {
            for (int j = i * i; j <= N; j += i)
                is_prime[j] = 0;
        }
    }

    int count = 0;
    for (int i = 2; i <= N; i++) {
        if (is_prime[i]) {
            printf("%d ", i);
            count++;
        }
    }
    printf("\n%d primes found up to %d\n", count, N);
    return 0;
}
