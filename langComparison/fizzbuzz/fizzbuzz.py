"""
FizzBuzz - Python

Classic: count 1-30, print Fizz (÷3), Buzz (÷5), FizzBuzz (÷15), else number.

Run:
  python3 fizzbuzz.py

Notice:
  - No type declarations; variables inferred at runtime
  - elif chain mirrors the C version almost character-for-character in intent
  - print() handles int and str without conversion
  - One of the shortest implementations possible without sacrificing clarity
"""

for i in range(1, 31):
    if   i % 15 == 0: print("FizzBuzz")
    elif i %  3 == 0: print("Fizz")
    elif i %  5 == 0: print("Buzz")
    else:             print(i)
