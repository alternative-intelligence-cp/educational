# Prime Sieve

**Category:** Speed — memory layout and array access patterns

Sieve of Eratosthenes: find all prime numbers up to N=1,000.

The algorithm marks all multiples of each prime as composite, starting from 2. By the time you reach a number, if it hasn't been marked, it must be prime.

```
Start:  [ 2* 3* 4  5* 6  7* 8  9  10 11* 12 13* ... ]

After 2: cross out 4, 6, 8, 10, 12... (every multiple of 2)
After 3: cross out 9, 15, 21...        (every multiple of 3 not already crossed)
After 5: cross out 25, 35...
...
Stop when i² > N
```

---

## Build and run

```
# C
gcc -O2 -o sieve sieve.c && ./sieve

# Python
python3 sieve.py

# Rust
rustc -C opt-level=2 -o sieve_rs sieve.rs && ./sieve_rs

# Aria
ariac sieve.aria -o sieve_aria && ./sieve_aria
```

All four should output the same 168 primes up to 1,000.

---

## What to look for

### Memory allocation

| Language | Array | Size | Allocation |
|----------|-------|------|------------|
| C | `char is_prime[N+1]` | 1 byte/slot | Stack (for small N), static |
| Python | `bytearray([1]) * (N+1)` | 1 byte/slot | Heap (GC managed) |
| Rust | `vec![true; N+1]` | heap `Vec<bool>` | Heap (RAII, freed on scope exit) |
| Aria | `bool[1001]:is_prime` | stack | Stack (fixed compile-time size) |

The C and Aria versions allocate on the stack. Rust and Python use the heap, but Rust cleans up automatically (RAII; no GC needed). Python's GC adds a small overhead but not visible at this scale.

### Array initialization

| Language | How |
|----------|-----|
| C | `memset(is_prime, 1, sizeof(is_prime))` — one system call, very fast |
| Python | `bytearray([1]) * (N+1)` — slice multiplication; fast |
| Rust | `vec![true; N+1]` — fills at allocation time |
| Aria | `while (idx <= N) { is_prime[idx] = true; ... }` — explicit loop; no memset equivalent in current stdlib |

### Variable array indexing

All four use `is_prime[j] = false` with a variable index `j`. In C/Aria this is direct hardware array access. In Python it's a method call on `bytearray`. In Rust the compiler proves the access is in-bounds (or inserts a panic check where it can't).

### The key limitation in Aria

Aria requires the array size to be a **compile-time constant**: `bool[1001]`. You cannot write `bool[n]` where `n` is a runtime value. This means:

- The N=1,000 limit is baked into the binary
- To support larger N, you'd recompile with a different constant
- For truly dynamic sizes, the arena allocator is available via `@extern("aria_arena_alloc", ...)` — but it requires manual sizing calculations

C, Python, and Rust can all accept `n` as a runtime argument and allocate accordingly.

### String arena limitation (visible when N is large)

When printing 168 primes in a loop, `int32_toString()` is called 168 times inside the loop. Each call allocates a new `AriaString` on the string arena. By the end of the loop, the arena is nearly exhausted. Calling `string_concat()` afterward triggers a segfault.

**Workaround:** split the final output into two separate `print()` calls instead of
`println(string_concat(...))`. This avoids a new allocation.

This is a known Aria runtime issue: the string arena pool size needs to grow with allocation demand (planned fix).

---

## Performance at larger N

For N=10,000,000 (a real-world size for prime counting), the performance spread becomes obvious:

| Language | Approximate time |
|----------|-----------------|
| C (`-O2`) | ~0.05s |
| Rust (`-O2`) | ~0.05–0.08s |
| Python | ~2–5s |
| Aria | ~0.05s (expected — same memory pattern as C) |

At N=1,000 the differences are imperceptible. The sieve algorithm is a good benchmark for large N specifically because it exercises cache behavior — the C/Rust/Aria versions stay in cache better than Python's due to lower per-element overhead.

---

## Summary

This comparison shows that **algorithmic structure is the same across languages** — the Sieve is the Sieve. What changes is how each language gives you access to memory, what guarantees it enforces on that access, and how much it costs you at runtime.
