/*
 * Error Handling - C
 *
 * Safe integer division that may fail (divide by zero), chained across two ops.
 * Shows the C idiom: return a bool success flag, write result through a pointer.
 *
 * Category: Safety — error propagation models
 *
 * Build:
 *   gcc -o divide divide.c && ./divide
 *
 * Notice:
 *   - The caller must check the return value — but nothing forces them to
 *   - Output parameters (*out) are a common C convention; harder to misread
 *     than errno, but still easy to ignore
 *   - divide_chain must explicitly thread the error upward at each step
 *   - No stack unwinding, no overhead; the cost is verbosity
 */

#include <stdio.h>
#include <stdbool.h>

/* Returns true on success; writes quotient through *out. */
static bool divide(int a, int b, int *out) {
    if (b == 0) return false;
    *out = a / b;
    return true;
}

/* Chain: (a / b) / c — propagates the first failure it hits. */
static bool divide_chain(int a, int b, int c, int *out) {
    int mid;
    if (!divide(a, b, &mid)) return false;
    if (!divide(mid, c, out)) return false;
    return true;
}

static void show(int a, int b, int c) {
    int result;
    if (divide_chain(a, b, c, &result))
        printf("(%d / %d) / %d  =  %d\n", a, b, c, result);
    else
        printf("(%d / %d) / %d  =  ERROR: division by zero\n", a, b, c);
}

int main(void) {
    show(100, 5, 4);   /* ok:  (100/5)/4 = 5  */
    show(100, 0, 4);   /* err: 100/0 fails first  */
    show(100, 5, 0);   /* err: 20/0 fails second  */
    return 0;
}
