"""
Error Handling - Python

Safe integer division that may fail (divide by zero), chained across two ops.
Shows the Python idiom: raise an exception; let it propagate naturally up the
call stack until something catches it.

Category: Safety — error propagation models

Run:
  python3 divide.py

Notice:
  - divide_chain needs no error-handling code at all — exceptions propagate
    automatically. The function is as short as the happy path only.
  - The try/except at the call site catches anything the chain might raise.
  - This is powerful, but also means callers can easily forget the try/except
    and get an unhandled exception at runtime.
  - You cannot tell from divide()'s signature that it can raise a ZeroDivisionError
    without reading its documentation or source.
"""


def divide(a: int, b: int) -> int:
    if b == 0:
        raise ZeroDivisionError(f"cannot divide {a} by zero")
    return a // b


def divide_chain(a: int, b: int, c: int) -> int:
    """(a / b) / c — exceptions propagate automatically; no boilerplate needed."""
    mid = divide(a, b)   # may raise — no try needed here
    return divide(mid, c)  # may raise


def show(a: int, b: int, c: int) -> None:
    try:
        result = divide_chain(a, b, c)
        print(f"({a} / {b}) / {c}  =  {result}")
    except ZeroDivisionError as e:
        print(f"({a} / {b}) / {c}  =  ERROR: {e}")


show(100, 5, 4)   # ok:  (100/5)/4 = 5
show(100, 0, 4)   # err: 100/0 fails first
show(100, 5, 0)   # err: 20/0 fails second
