# Error Handling

**Category:** Safety — error propagation models

Safe integer division that can fail (divide by zero), chained across two operations:

```
(a / b) / c
```

The first division may fail. If it does, the second should never execute.
If it succeeds, the second division might still fail.

---

## Build and run

```
# C
gcc -o divide divide.c && ./divide

# Python
python3 divide.py

# Rust
rustc -o divide_rs divide.rs && ./divide_rs

# Aria
ariac divide.aria -o divide_aria && ./divide_aria
```

All four produce:

```
(100 / 5) / 4  =  5
(100 / 0) / 4  =  ERROR: division by zero
(100 / 5) / 0  =  ERROR: division by zero
```

---

## What to look for

### How errors propagate through a chain

This is the core question. Notice how much code each language needs in the *middle* (the `divide_chain` function), not just at the call site.

| Language | `divide_chain` body |
|----------|---------------------|
| C | 4 lines: two calls, two explicit `if (!...)` return-false checks |
| Python | 2 lines: two calls, zero boilerplate — exceptions propagate automatically |
| Rust | 2 lines: `let mid = divide(a,b)?;` then `divide(mid,c)` — `?` propagates |
| Aria | 4 lines: `? sentinel` to unwrap, then manual `if == -1` re-propagate |

Python and Rust give you "free" propagation. C and Aria (currently) require you to thread errors up manually. The **difference in design** is:

- **Exceptions** (Python): silent unless caught. You can forget `try/except` and your program crashes at runtime. The function signature doesn't tell you it can fail.
- **Return codes** (C, Aria): verbose, but always visible. Easy to *ignore*, hard to *miss entirely*.
- **Typed results** (Rust `Result<T,E>`): propagation is easy (the `?` operator), visibility is enforced by the type system. The compiler warns if you drop a `Result` without checking it.

### The cost of Aria's current limitation

Rust's `divide()` signature is `fn divide(a: i32, b: i32) -> Result<i32, String>`.
The failure mode is part of the type.

Aria's `divide()` returns `int32` and uses `-1` as a sentinel. This means:
1. The sentinel value `-1` is indistinguishable from a legitimate result of `-1`
2. Nothing in the type system reminds callers to check for it
3. Propagation requires manual `if (result == -1)` checks — same as C

The `fail()` / `Err` syntax planned for Aria will close this gap. When implemented, `divide()` would explicitly return a typed error, and `?` would propagate it with no boilerplate — matching Rust's ergonomics.

### The hidden cost of exceptions

Python's version is the shortest and looks most elegant. But:

```python
def caller():
    result = divide_chain(100, 0, 4)  # forgot try/except
    print(result)                     # crashes here with an unhandled ZeroDivisionError
```

Nothing in Python stops you from forgetting the `try`. In C/Rust/Aria the failure
mode is surfaced (in the return value) whether you want it or not.

---

## Summary

| | Propagation cost | Forgettable? | Failure in signature? |
|--|--|--|--|
| C | Manual — verbose | Yes (ignore return value) | No |
| Python | Free — automatic | Yes (forget try/except) | No |
| Rust | Nearly free (`?`) | No (compiler warning) | Yes |
| Aria (current) | Manual — like C | Yes (ignore sentinel) | No |
| Aria (planned) | Nearly free (`?`) | No | Yes |
