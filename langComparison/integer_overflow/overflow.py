"""
Integer Overflow - Python

What happens when a "32-bit" integer exceeds its maximum value?

Category: Safety — how languages handle arithmetic limits

Run:
  python3 overflow.py

Notice:
  - Python integers have NO fixed size. They grow as needed (arbitrary precision).
  - There is no INT32_MAX overflow in Python — it just becomes a bigger int.
  - This eliminates an entire class of bugs, but at a cost:
    * Memory grows with the number
    * Arithmetic on large numbers is slower (not hardware-native)
    * When interfacing with C or network protocols, you must manually
      enforce 32-bit bounds (see struct.pack and ctypes)
  - sys.maxsize is the largest value that fits in a C ssize_t (platform pointer size)
    but it's not a hard limit on Python integers — just an implementation detail.
"""

import sys

INT32_MAX = 2**31 - 1
UINT32_MAX = 2**32 - 1

x = INT32_MAX
print(f"INT32_MAX         = {x}")
print(f"INT32_MAX + 1     = {x + 1}   (no overflow — Python promotes to larger int)")
print(f"INT32_MAX + 1     = {type(x + 1)}")

u = UINT32_MAX
print(f"UINT32_MAX        = {u}")
print(f"UINT32_MAX + 1    = {u + 1}   (no overflow — same promotion)")

# Demonstrating that Python integers are truly unbounded:
huge = INT32_MAX ** 10
print(f"INT32_MAX ** 10   = {huge}")    # a 92-digit number; no overflow
print(f"(sys.maxsize      = {sys.maxsize})")
