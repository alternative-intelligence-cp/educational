/*
 * Integer Overflow - C
 *
 * What happens when a signed 32-bit integer exceeds its maximum value?
 *
 * Category: Safety — undefined behavior and silent corruption
 *
 * Build:
 *   gcc -o overflow overflow.c && ./overflow
 *   gcc -fsanitize=undefined -o overflow_ub overflow.c && ./overflow_ub
 *
 * Notice:
 *   - Signed integer overflow (int, long, etc.) is UNDEFINED BEHAVIOR in C.
 *     The standard says the behavior is undefined — the compiler may:
 *       * Wrap around (common on x86/arm)
 *       * Delete the code entirely
 *       * Produce a different value
 *       * Corrupt adjacent memory
 *   - Unsigned overflow is defined: it wraps modulo 2^N (a guarantee)
 *   - Run with -fsanitize=undefined to catch the UB at runtime
 *   - At -O2 or higher, the optimizer may assume signed overflow never
 *     happens and remove loops that depend on it (a real source of bugs!)
 */

#include <stdio.h>
#include <limits.h>
#include <stdint.h>

int main(void) {
    /* Signed overflow — UNDEFINED BEHAVIOR */
    int32_t x = INT32_MAX;
    printf("INT32_MAX         = %d\n", x);
    printf("INT32_MAX + 1     = %d   (UB — likely wraps on x86, but not guaranteed)\n", x + 1);

    /* Unsigned overflow — DEFINED: wraps modulo 2^32 */
    uint32_t u = UINT32_MAX;
    printf("UINT32_MAX        = %u\n", u);
    printf("UINT32_MAX + 1    = %u   (defined: wraps to 0)\n", u + 1);

    /* Overflow in an expression used for array indexing — dangerous */
    int32_t big = INT32_MAX;
    int32_t step = 1;
    int32_t index = big + step;  /* UB — this is how buffer overflows happen */
    printf("(as array index)  = %d   (would access memory at an unintended offset)\n", index);

    return 0;
}
