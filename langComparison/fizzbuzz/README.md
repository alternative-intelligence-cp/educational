# FizzBuzz

**Category:** Syntax baseline

Count from 1 to 30. Print `Fizz` for multiples of 3, `Buzz` for multiples of 5,
`FizzBuzz` for multiples of both, and the number itself otherwise.

This is the simplest possible program that makes you touch loops, conditionals, and output — so it's a good first look at how languages differ in feel.

---

## Build and run

```
# C
gcc -o fizzbuzz fizzbuzz.c && ./fizzbuzz

# Python
python3 fizzbuzz.py

# Rust
rustc -o fizzbuzz_rs fizzbuzz.rs && ./fizzbuzz_rs

# Aria
ariac fizzbuzz.aria -o fizzbuzz_aria && ./fizzbuzz_aria
```

All four produce identical output.

---

## What to look for

### Loop syntax

| Language | Loop |
|----------|------|
| C | `for (int i = 1; i <= 30; i++)` — C-style, init/condition/increment |
| Python | `for i in range(1, 31)` — range-based, end is exclusive |
| Rust | `for i in 1..=30` — range-based, `..=` is inclusive |
| Aria | `while (i <= 30i32) { i = i + 1i32; }` — no `for` loop yet; `while` + counter |

### Conditional chain

| Language | Pattern |
|----------|---------|
| C | `if / else if / else` — classic top-to-bottom chain |
| Python | `if / elif / else` — same logic, different spelling |
| Rust | `match (i % 3, i % 5) { (0,0) => ..., ... }` — pattern match on a tuple; exhaustiveness is checked at compile time |
| Aria | Four separate `if` statements with boolean flags `fizz`/`buzz` — no `else if`; nested `else` requires braces; the flag pattern is explicit and readable |

### Output

| Language | How |
|----------|-----|
| C | `puts("string")` for a string + newline, `printf("%d\n", i)` for numbers |
| Python | `print(...)` handles both — no conversion needed |
| Rust | `println!("FizzBuzz")`, `println!("{i}")` — macro with format string |
| Aria | `drop(println("string"))` — `println()` returns `Result<NIL>` so we `drop()` it; numbers need `int32_toString(i)` |

### Type literals

Aria requires explicit type suffixes on integer literals: `1i32`, `30i32`, `3i32`.
C and Rust infer from context. Python has no fixed-width integer types.

---

## Verdict

This example doesn't favor any language — all four are about equally readable. The differences are mostly cosmetic. The real divergence shows up in the later comparisons.
