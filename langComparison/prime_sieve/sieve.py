"""
Prime Sieve - Python (Sieve of Eratosthenes)

Find all prime numbers up to N using the classic sieve algorithm.

Category: Speed — memory layout, array access patterns

Run:
  python3 sieve.py

Notice:
  - bytearray gives a fast, compact boolean array (1 byte per slot)
  - Slice assignment is_prime[i*i::i] = b'\\x00' * len(is_prime[i*i::i])
    crosses out all multiples of i in one line — very Pythonic
  - The loop bound uses int(N**0.5) + 1 for the square-root optimization
  - Python integers have no fixed size; range(N+1) produces exact values
  - GC and runtime overhead make Python noticeably slower than C/Rust at
    large N — for N=10_000_000 expect ~10-30x slower wall time
"""

import math

N = 1000

is_prime = bytearray([1]) * (N + 1)
is_prime[0] = 0
is_prime[1] = 0

for i in range(2, int(math.isqrt(N)) + 1):
    if is_prime[i]:
        is_prime[i * i :: i] = bytes(len(is_prime[i * i :: i]))

primes = [i for i in range(2, N + 1) if is_prime[i]]
print(' '.join(map(str, primes)))
print(f'{len(primes)} primes found up to {N}')
