# Language Comparison

Side-by-side implementations of the same programs in **C**, **Python**, **Rust**, and **Aria**.

The goal is an *honest* comparison of trade-offs — not a marketing brochure for any language. Every language shown here has valid use cases. The question is always: for your specific problem, which trade-offs matter most?

---

## Languages

| Language | Paradigm | Memory | Error model | Speed |
|----------|----------|--------|-------------|-------|
| **C** | Imperative, procedural | Manual (`malloc`/`free`) | Return codes, `errno` | Baseline fast |
| **Python** | Multi-paradigm, high-level | Garbage collected | Exceptions | ~10–100× slower than C |
| **Rust** | Systems, ownership | Ownership + RAII | `Result<T,E>`, `Option<T>` | ≈ C |
| **Aria** | Systems (in development) | Fixed arrays + arena allocator | `Result<T>` (partial) | ≈ C (goal) |

---

## Comparisons

### [fizzbuzz](fizzbuzz/) — Syntax baseline

The simplest possible non-trivial program. Same behavior, visibly different syntax and idiom. Good first place to look when learning a new language.

**Key differences to notice:** loop syntax, conditional chains, string/int output, implicit vs explicit types.

---

### [error_handling](error_handling/) — Error propagation models

Safe integer division that can fail (divide by zero), chained across two operations.

**Key differences:** C uses return flags + output pointers. Python uses exceptions that unwind the stack. Rust uses `Result<T,E>` with the `?` operator for zero-friction propagation. Aria currently uses sentinel values (C-style) — explicit `fail()` / `Err` syntax is planned.

This comparison shows something important: **how a language's error model affects the shape of your code**, not just the error-handling lines themselves.

---

### [prime_sieve](prime_sieve/) — Speed and memory

Sieve of Eratosthenes to find all primes up to N=1,000.

**Key differences:** C and Rust give direct, fast array access. Python is readable but much slower at large N. Aria requires a fixed compile-time array size (a current limitation — dynamic allocation via the arena allocator is available but not yet as ergonomic). All four produce identical output.

---

### [integer_overflow](integer_overflow/) — Safety and undefined behavior

What happens when a 32-bit signed integer exceeds its maximum value.

**Key differences:** C signed overflow is undefined behavior — the compiler may do anything, including silently generating wrong results. Python integers have no fixed size and never overflow. Rust panics in debug builds; release builds wrap but require explicit `wrapping_add`. Aria currently behaves like C at the LLVM level — no overflow checks are added.

This is the most important safety comparison in low-level systems work. Silent overflow bugs are a real source of security vulnerabilities (CVE history is full of them).

---

## Notes on Aria

Aria is a systems language under active development. These programs are both educational comparisons *and* real-world tests of the language as it evolves. Where Aria's current implementation differs from its design goals, the README in each subdirectory says so explicitly.

Current limitations visible in these comparisons:
- Arrays require a compile-time size (`int32[N]:arr` not `int32[]:arr` with dynamic N)
- User functions can only produce success results (`pass(value)`); an explicit `fail()` keyword is planned
- No signed overflow checking (same as C; safe arithmetic builtins are planned)
- No `for` loop syntax yet; `while` + counter is the current idiom
