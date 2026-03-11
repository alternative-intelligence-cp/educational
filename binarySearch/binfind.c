/*
 * Binary Search - C Implementation
 *
 * Binary search works on a sorted list by repeatedly halving the search space:
 *   1. Check the middle element
 *   2. If it matches, done
 *   3. If the target is larger, discard the left half
 *   4. If the target is smaller, discard the right half
 *   5. Repeat until found or the search space is empty
 *
 * Time complexity:  O(log n)  — each step cuts the problem in half
 * Space complexity: O(1)      — iterative, no extra memory needed
 *
 * Build:
 *   gcc -o binfind binfind.c -lm
 *
 * Usage:
 *   ./binfind [target] [n1 n2 n3 ...]
 *
 * Examples:
 *   ./binfind                      # uses built-in demo list, target 31
 *   ./binfind 73                   # search for 73 in demo list
 *   ./binfind 7 1 3 5 7 9 11 13   # search for 7 in a custom sorted list
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

/* ── Limits ────────────────────────────────────────────────────────────────── */

#define MAX_LIST  256
#define LINE_SEP  "────────────────────────────────────────────────────────────"

/* ── ANSI colors ───────────────────────────────────────────────────────────── */

#define COL_RESET   "\033[0m"
#define COL_YELLOW  "\033[33m"   /* current probe  */
#define COL_GREEN   "\033[32m"   /* found          */
#define COL_GREY    "\033[90m"   /* eliminated     */

/* ── Default demo data ─────────────────────────────────────────────────────── */

static const double DEFAULT_LIST[] = {
    1, 3, 6, 8, 9, 10, 11, 23, 25, 31, 65, 73, 105, 205, 569, 1024
};
static const int DEFAULT_LIST_LEN = (int)(sizeof(DEFAULT_LIST) / sizeof(DEFAULT_LIST[0]));
static const double DEFAULT_TARGET = 31.0;

/* ── Step record ───────────────────────────────────────────────────────────── */

typedef struct {
    int lo, mid, hi;
} Step;

/* ── Utilities ─────────────────────────────────────────────────────────────── */

/* Parse a double from str; returns 0 on success, -1 on failure. */
static int parse_double(const char *str, double *out) {
    char *end;
    errno = 0;
    *out = strtod(str, &end);
    if (errno != 0 || end == str || *end != '\0') return -1;
    if (!isfinite(*out)) return -1;
    return 0;
}

/* Width of the %g representation of a double. */
static int num_width(double v) {
    char buf[64];
    snprintf(buf, sizeof(buf), "%g", v);
    return (int)strlen(buf);
}

/* Print a number right-justified in 'width' chars, colored. */
static void print_col(double v, int width, const char *color) {
    char buf[64];
    snprintf(buf, sizeof(buf), "%g", v);
    int len = (int)strlen(buf);
    if (color) printf("%s", color);
    /* right-justify */
    for (int i = len; i < width; i++) printf(" ");
    printf("%s", buf);
    if (color) printf("%s", COL_RESET);
}

/* ── Render one step ───────────────────────────────────────────────────────── */

static void render_step(
    const double *lst, int n, int col_w,
    int lo, int mid, int hi,
    const int *eliminated,    /* boolean array, 1 = eliminated */
    int step_num, int is_found)
{
    printf("  Step %2d: [ ", step_num);
    for (int i = 0; i < n; i++) {
        if (i > 0) printf("  ");
        if (eliminated[i]) {
            print_col(lst[i], col_w, COL_GREY);
        } else if (is_found && i == mid) {
            print_col(lst[i], col_w, COL_GREEN);
        } else if (i == mid) {
            print_col(lst[i], col_w, COL_YELLOW);
        } else {
            print_col(lst[i], col_w, NULL);
        }
    }
    printf(" ]\n");
    printf("           lo=%d  mid=%d  hi=%d  probe=%g\n", lo, mid, hi, lst[mid]);
}

/* ── Binary search ─────────────────────────────────────────────────────────── */

/*
 * Runs binary search, recording each step into 'steps' (caller provides space).
 * Returns the index of the target, or -1 if not found.
 * 'n_steps' is set to the number of steps taken.
 */
static int binary_search(
    const double *lst, int n, double target,
    Step *steps, int *n_steps,
    int *eliminated)      /* output: which indices were eliminated */
{
    int lo = 0, hi = n - 1;
    *n_steps = 0;

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;   /* avoids overflow vs (lo+hi)/2 */
        steps[*n_steps].lo  = lo;
        steps[*n_steps].mid = mid;
        steps[*n_steps].hi  = hi;
        (*n_steps)++;

        if (lst[mid] == target) {
            return mid;
        } else if (lst[mid] < target) {
            for (int i = lo; i <= mid; i++) eliminated[i] = 1;
            lo = mid + 1;
        } else {
            for (int i = mid; i <= hi; i++) eliminated[i] = 1;
            hi = mid - 1;
        }
    }
    return -1;
}

/* ── main ──────────────────────────────────────────────────────────────────── */

int main(int argc, char *argv[]) {
    double lst[MAX_LIST];
    int    n = 0;
    double target;

    /* Parse target */
    if (argc < 2) {
        target = DEFAULT_TARGET;
    } else {
        if (parse_double(argv[1], &target) != 0) {
            fprintf(stderr, "Error: target \"%s\" is not a valid number.\n", argv[1]);
            return 1;
        }
    }

    /* Parse list */
    if (argc < 3) {
        n = DEFAULT_LIST_LEN;
        memcpy(lst, DEFAULT_LIST, n * sizeof(double));
    } else {
        for (int i = 2; i < argc; i++) {
            if (n >= MAX_LIST) {
                fprintf(stderr, "Error: list too long (max %d items).\n", MAX_LIST);
                return 1;
            }
            if (parse_double(argv[i], &lst[n]) != 0) {
                fprintf(stderr, "Error: list item \"%s\" is not a valid number.\n", argv[i]);
                return 1;
            }
            n++;
        }
    }

    /* Validate: non-empty */
    if (n == 0) {
        fprintf(stderr, "Error: list must not be empty.\n");
        return 1;
    }

    /* Validate: sorted */
    for (int i = 0; i < n - 1; i++) {
        if (lst[i] > lst[i + 1]) {
            fprintf(stderr, "Error: list is not sorted (found %g before %g).\n",
                    lst[i], lst[i + 1]);
            return 1;
        }
    }

    /* Column width: widest number */
    int col_w = 0;
    for (int i = 0; i < n; i++) {
        int w = num_width(lst[i]);
        if (w > col_w) col_w = w;
    }

    /* Print header */
    puts(LINE_SEP);
    puts("  Binary Search");
    puts(LINE_SEP);
    printf("  List   : [");
    for (int i = 0; i < n; i++) {
        if (i > 0) printf(", ");
        printf("%g", lst[i]);
    }
    printf("]\n");
    printf("  Length : %d\n", n);
    printf("  Target : %g\n", target);
    puts(LINE_SEP);
    printf("\n");
    printf("  Legend:  %s[probe]%s  %s[found]%s  %s[eliminated]%s\n\n",
           COL_YELLOW, COL_RESET, COL_GREEN, COL_RESET, COL_GREY, COL_RESET);

    /* Run search — capture steps */
    Step steps[MAX_LIST];
    int  eliminated[MAX_LIST];
    int  n_steps;
    memset(eliminated, 0, sizeof(int) * n);

    int found_idx = binary_search(lst, n, target, steps, &n_steps, eliminated);

    /* Replay steps with per-step eliminated state */
    int elim_snap[MAX_LIST];
    int elim_running[MAX_LIST];
    memset(elim_running, 0, sizeof(int) * n);

    for (int s = 0; s < n_steps; s++) {
        int lo  = steps[s].lo;
        int mid = steps[s].mid;
        int hi  = steps[s].hi;

        /* snapshot of eliminated BEFORE this step processes */
        memcpy(elim_snap, elim_running, sizeof(int) * n);

        int is_last  = (s == n_steps - 1);
        int is_found = (is_last && found_idx != -1);
        render_step(lst, n, col_w, lo, mid, hi, elim_snap, s + 1, is_found);

        /* update eliminated after rendering */
        if (!is_found) {
            if (lst[mid] < target) {
                for (int i = lo; i <= mid; i++) elim_running[i] = 1;
            } else {
                for (int i = mid; i <= hi; i++) elim_running[i] = 1;
            }
        }
    }

    /* Final summary */
    printf("\n");
    puts(LINE_SEP);

    int max_steps = (int)ceil(log2((double)(n + 1)));
    if (found_idx != -1) {
        printf("  Found!  value=%g  index=%d  position=%d\n",
               target, found_idx, found_idx + 1);
    } else {
        printf("  Not found: %g is not in the list.\n", target);
    }
    printf("  Steps taken: %d  (list has %d items, max possible: %d)\n",
           n_steps, n, max_steps);
    puts(LINE_SEP);

    return 0;
}
