/*
 * FizzBuzz - Rust
 *
 * Classic: count 1-30, print Fizz (÷3), Buzz (÷5), FizzBuzz (÷15), else number.
 *
 * Build:
 *   rustc -o fizzbuzz fizzbuzz.rs && ./fizzbuzz
 *
 * Notice:
 *   - match on a tuple of remainders — idiomatic Rust pattern matching
 *   - 1..=30 is an inclusive range; 1..30 would exclude 30
 *   - Compiler enforces that all match arms are exhausted (the _ wildcard)
 *   - println! is a macro, not a function; the ! is deliberate
 *   - {i} inside the format string is shorthand for {}", i} (Rust 1.58+)
 */

fn main() {
    for i in 1..=30 {
        match (i % 3, i % 5) {
            (0, 0) => println!("FizzBuzz"),
            (0, _) => println!("Fizz"),
            (_, 0) => println!("Buzz"),
            _      => println!("{i}"),
        }
    }
}
