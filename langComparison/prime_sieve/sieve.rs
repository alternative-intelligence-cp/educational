/*
 * Prime Sieve - Rust (Sieve of Eratosthenes)
 *
 * Find all prime numbers up to N using the classic sieve algorithm.
 *
 * Category: Speed — memory layout, array access patterns
 *
 * Build:
 *   rustc -C opt-level=2 -o sieve sieve.rs && ./sieve
 *
 * Notice:
 *   - vec![true; N+1] allocates a heap Vec<bool> initialized to true
 *     (Rust's standard library optimizes Vec<bool> storage)
 *   - Explicit type annotations: usize for indices (pointer-sized, platform)
 *   - The borrow checker ensures is_prime[..] is not aliased during mutation
 *   - No bounds check is elided by default; Rust proves safety at each access
 *     (the optimizer removes redundant checks where it can prove them safe)
 *   - Speed in practice: within ~5-10% of the C version at -O2
 */

fn main() {
    const N: usize = 1000;

    let mut is_prime = vec![true; N + 1];
    is_prime[0] = false;
    is_prime[1] = false;

    let mut i = 2usize;
    while i * i <= N {
        if is_prime[i] {
            let mut j = i * i;
            while j <= N {
                is_prime[j] = false;
                j += i;
            }
        }
        i += 1;
    }

    let mut count = 0usize;
    let primes: Vec<String> = (2..=N)
        .filter(|&i| is_prime[i])
        .map(|i| { count += 1; i.to_string() })
        .collect();

    println!("{}", primes.join(" "));
    println!("{} primes found up to {}", count, N);
}
