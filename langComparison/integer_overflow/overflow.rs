/*
 * Integer Overflow - Rust
 *
 * What happens when a signed 32-bit integer exceeds its maximum value?
 *
 * Category: Safety — language-enforced arithmetic contracts
 *
 * Build:
 *   rustc -o overflow overflow.rs         # debug build (panics on overflow)
 *   rustc -C opt-level=2 -o overflow overflow.rs   # release (wraps silently)
 *
 * Notice:
 *   - In DEBUG builds, Rust adds overflow checks: x + 1 panics if x == i32::MAX
 *   - In RELEASE builds (-O2), overflow wraps (like C unsigned) for speed
 *   - Neither behavior is silent corruption — you either get a panic (debug)
 *     or a well-defined wrap (release). There is no undefined behavior.
 *   - For code that must be correct in release, use the explicit methods:
 *       checked_add(1) -> Option<i32>  — returns None on overflow
 *       wrapping_add(1) -> i32          — always wraps
 *       saturating_add(1) -> i32        — clamps to i32::MAX
 *       overflowing_add(1) -> (i32, bool) — returns value + overflow flag
 *   - The type system makes the intent explicit and auditable
 */

fn main() {
    let x: i32 = i32::MAX;
    println!("i32::MAX              = {}", x);

    // checked_add: returns None if overflow would occur
    match x.checked_add(1) {
        Some(v) => println!("checked_add(1)        = {}", v),
        None    => println!("checked_add(1)        = OVERFLOW (would panic in debug, wrap in release)"),
    }

    // wrapping_add: always wraps, never panics — the "C unsigned" behavior made explicit
    println!("wrapping_add(1)       = {}", x.wrapping_add(1));

    // saturating_add: clamps at the type's boundary
    println!("saturating_add(1)     = {}", x.saturating_add(1));

    // overflowing_add: returns the wrapped value AND a flag
    let (val, did_overflow) = x.overflowing_add(1);
    println!("overflowing_add(1)    = {} (overflowed: {})", val, did_overflow);

    // Unsigned: same methods available, wrapping is defined either way
    let u: u32 = u32::MAX;
    println!("u32::MAX              = {}", u);
    println!("u32 wrapping_add(1)   = {}", u.wrapping_add(1));
}
