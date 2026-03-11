# Integer Overflow

**Category:** Safety — arithmetic behavior at type limits

What happens when a 32-bit signed integer exceeds its maximum value (`2147483647`)?

This is not a theoretical question. Integer overflow has caused real security vulnerabilities, wrong calculations, and crashed systems. The famous Ariane 5 rocket explosion in 1996 was caused by an unchecked integer conversion.

---

## Build and run

```
# C (notice the different build flags — they change behavior)
gcc -O0 -o overflow overflow.c && ./overflow
gcc -O2 -o overflow_o2 overflow.c && ./overflow_o2

# Python
python3 overflow.py

# Rust (debug build — panics on overflow)
rustc -o overflow_rs overflow.rs && ./overflow_rs
# Rust (release build — wraps)
rustc -C opt-level=2 -o overflow_rs_rel overflow.rs && ./overflow_rs_rel

# Aria
ariac overflow.aria -o overflow_aria && ./overflow_aria
```

---

## What each language does

### When `INT32_MAX + 1` is evaluated:

| Language / Build | `INT32_MAX + 1` | Notes |
|---|---|---|
| C (`-O0`) | `-2147483648` | Hardware wraps on x86-64 |
| C (`-O2`) | `-2147483648` | GCC still wraps here (but is not required to) |
| Python | `2147483648` | No overflow — Python promotes to arbitrary-precision int |
| Rust (debug) | Panics | `overflow-checks = true` in debug mode |
| Rust (release) | `-2147483648` | Wraps — same as C, but explicit (`wrapping_add`) |
| Aria | `2147483647` | LLVM `add nsw` elides the +1 entirely — **optimizer changed the result** |

### Aria's row is the most interesting one

Aria compiles to LLVM IR. Integer arithmetic emits `add nsw i32` — the `nsw` ("no signed wrap") flag tells LLVM the programmer *guarantees* this addition will not overflow. When it does anyway, LLVM treats it as undefined behavior and **removes the operation entirely**, keeping the original value.

GCC did the "less surprising" thing here (wrapping), but it is equally within its rights to do what LLVM did. **Both behaviors are legal.** Neither is guaranteed.

This is the core danger: you cannot predict what a compiler will do with signed overflow. It depends on the optimizer, the instruction sequence, and the surrounding code. The behavior can change when you add unrelated code nearby.

---

## Why unsigned is different

| Language | `UINT32_MAX + 1` |
|---|---|
| C | `0` (defined — unsigned wraps mod 2³²) |
| Python | `4294967296` (no overflow) |
| Rust | `0` via `wrapping_add(1)`; checked: None |
| Aria | `0` (defined — LLVM unsigned arithmetic wraps) |

Unsigned overflow is **defined behavior** in both C and at the LLVM level. The modular arithmetic is guaranteed. This is why security-conscious C uses `uint32_t` for values that must wrap predictably (cryptographic operations, checksums).

---

## Rust's approach: explicit contracts

Rust gives you four methods, each with a clear semantic contract:

```rust
x.checked_add(1)    // -> Option<i32>:  None if overflow
x.wrapping_add(1)   // -> i32:          always wraps (never panics)
x.saturating_add(1) // -> i32:          clamps to i32::MAX on overflow
x.overflowing_add(1)// -> (i32, bool):  value + "did it overflow?" flag
```

In debug builds, plain `x + 1` also panics on overflow. In release builds it wraps.

The point is: **the intent is in the code**, not just in comments. A code reviewer can see which overflow contract is expected.

---

## Python's approach: just don't fix the size

Python integers grow as needed. `INT32_MAX ** 10` (a 92-digit number) is perfectly valid. This eliminates overflow bugs entirely, at the cost of:
- Variable memory usage (large numbers take more space)
- Slower arithmetic for large numbers (not native hardware words)
- No automatic enforcement of protocol-level size limits (you must add those manually for, e.g., network packets)

---

## What this means in practice

The most dangerous overflow bugs are **silent**: no crash, no exception, just a wrong value. In C (and currently Aria), a signed overflow in a path the optimizer decides was UB can produce wrong results, wrong branch decisions, or corrupted data — with no warning.

Real CVEs caused by integer overflow: [CWE-190](https://cwe.mitre.org/data/definitions/190.html) is in the OWASP Top 25 most dangerous weaknesses.

Aria's planned fix: safe arithmetic methods (`checked_add`, `wrapping_add`) matching Rust's approach.
