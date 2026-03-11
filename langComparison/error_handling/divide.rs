/*
 * Error Handling - Rust
 *
 * Safe integer division that may fail (divide by zero), chained across two ops.
 * Shows the Rust idiom: return Result<T, E>; use ? to propagate errors.
 *
 * Category: Safety — error propagation models
 *
 * Build:
 *   rustc -o divide divide.rs && ./divide
 *
 * Notice:
 *   - divide() returns Result<i32, String>: either Ok(value) or Err(message)
 *   - The ? operator in divide_chain() is syntactic sugar:
 *       let mid = divide(a, b)?;
 *     means "if Err, return that Err from *this* function immediately"
 *   - divide_chain() is as short as the Python version, but unlike Python
 *     the failure mode is visible in the return type: -> Result<i32, String>
 *   - match at the call site forces you to handle both Ok and Err
 *   - The compiler rejects unhandled Results (unused_must_use warning/error)
 */

fn divide(a: i32, b: i32) -> Result<i32, String> {
    if b == 0 {
        return Err(format!("cannot divide {} by zero", a));
    }
    Ok(a / b)
}

fn divide_chain(a: i32, b: i32, c: i32) -> Result<i32, String> {
    let mid = divide(a, b)?;  // ? propagates Err automatically
    divide(mid, c)
}

fn show(a: i32, b: i32, c: i32) {
    match divide_chain(a, b, c) {
        Ok(v)    => println!("({} / {}) / {}  =  {}", a, b, c, v),
        Err(msg) => println!("({} / {}) / {}  =  ERROR: {}", a, b, c, msg),
    }
}

fn main() {
    show(100, 5, 4);   // ok:  (100/5)/4 = 5
    show(100, 0, 4);   // err: 100/0 fails first
    show(100, 5, 0);   // err: 20/0 fails second
}
