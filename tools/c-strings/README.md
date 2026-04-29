# C Strings Lesson (Educational)

A cleaned-up version of a hands-on C string lesson, designed for learning how string operations work under the hood.

## What this module teaches

- Heap allocation and ownership (`String_create`, `String_destroy`)
- Null-terminated strings and tracked lengths
- Safe bounds checks (`getChar`, `setChar`, `slice`, `patch`, `splice`)
- Case conversion and reversal
- Search/contains/compare semantics
- Dynamic growth with a tiny `StringBuilder`

## Files

- `lesson_string.h` — public API and comments
- `lesson_string.c` — implementation with error handling
- `lesson_demo.c` — walkthrough-style demo app
- `Makefile` — build/run helpers
- `tests/` — lightweight classroom-friendly test harness

## Build and run

```bash
cd /home/randy/Workspace/REPOS/educational/tools/c-strings
make

# run tests
make test
```

## Design notes

This version intentionally keeps function names close to the original lesson (`String_*`) so it’s easy to compare old vs improved code.

Improvements added:

- `const` correctness where appropriate
- checks for `NULL` inputs
- bounds validation on all index-based operations
- safer integer conversion using `snprintf`
- `StringBuilder` capacity growth with overflow checks
- clearer comments and predictable return contracts (`bool` for in-place ops)

## Learning exercises

1. Add `String_trim()` (remove leading/trailing spaces).
2. Add `String_replaceC()` (replace all occurrences of a substring).
3. Extend the existing tests with edge cases (empty strings, huge strings, bad indices).
4. Compare this manual implementation to standard C library functions (`strcpy`, `strncpy`, `strstr`, `strcmp`).
