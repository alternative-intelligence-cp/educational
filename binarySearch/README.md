# Binary Search

A Rosetta Stone–style collection of binary search implementations in four languages: **JavaScript**, **Python**, **C**, and **Aria**. Each version is standalone, produces the same output format, and includes step-by-step terminal visualization.

---

## The Algorithm

Binary search finds a target value in a **sorted** list by repeatedly halving the search space:

```
list: [ 1  3  6  8  9  10  11 | 23  25  31  65  73  105  205  569  1024 ]
                                          ^target=31

Step 1: probe the middle → 23.  31 > 23 → discard the left half.
Step 2: probe the new middle → 73.  31 < 73 → discard the right half.
Step 3: probe the new middle → 31.  Match!  Found at index 9.
```

This is the same strategy used in the classic game of Battleship when an experienced player doesn't search square by square but jumps straight to the middle of unexplored territory — each miss eliminates half the remaining possibilities.

| Metric           | Value                          |
|------------------|--------------------------------|
| Time complexity  | **O(log n)** — halves each step |
| Space complexity | **O(1)** — iterative, in-place  |
| Requirement      | List must be **sorted**         |

For a list of **n = 16**: at most `⌈log₂(17)⌉ = 5` steps are ever needed.

---

## Implementations

### JavaScript

```
node binfind.js [target] [n1 n2 ...]
```

```
node binfind.js                       # default list, target 31
node binfind.js 73                    # search for 73
node binfind.js 999                   # not-found case
node binfind.js 7 1 3 5 7 9 11 13    # custom sorted list
```

Uses ANSI color codes: **yellow** = current probe, **green** = found, **grey** = eliminated.  
Requires Node.js.

---

### Python

```
python3 binfind.py [target] [n1 n2 ...]
```

```
python3 binfind.py                    # default list, target 31
python3 binfind.py 73                 # search for 73
python3 binfind.py 999                # not-found case
python3 binfind.py 7 1 3 5 7 9 11 13 # custom sorted list
```

Same ANSI visualization as the JS version.  
Requires Python 3.

---

### C

Build once, run anywhere:

```
gcc -o binfind binfind.c -lm
```

```
./binfind                             # default list, target 31
./binfind 73                          # search for 73
./binfind 999                         # not-found case
./binfind 7 1 3 5 7 9 11 13           # custom sorted list
```

Portable C99 with no dependencies beyond libc. Max list length: 256 items (`MAX_LIST`).

---

### Aria

```
ariac binfind.aria -o binfind_aria
./binfind_aria
```

A hardcoded demo using the default list and `target = 31`. Aria is a systems language under active development — this file serves as a real-world test of its string utilities (`string_convert`, `print_utils`) and demonstrates clean iterative code including:

- Fixed-size arrays (`int32[16]`)
- `while` loops with variable array indexing (`list[mid]`)
- String building with `string_concat` and `int32_toString`
- Result-type propagation (`drop()`, `pass()`)

---

## Sample Output

Running `node binfind.js` (colors rendered in a terminal):

```
────────────────────────────────────────────────────────────
  Binary Search
────────────────────────────────────────────────────────────
  List   : [1, 3, 6, 8, 9, 10, 11, 23, 25, 31, 65, 73, 105, 205, 569, 1024]
  Length : 16
  Target : 31
────────────────────────────────────────────────────────────

  Legend:  [probe]  [found]  [eliminated]

  Step  1: [  1   3   6   8   9  10  11  23  25  31  65  73 105 205 569 1024 ]
           lo=0  mid=7  hi=15  probe=23  -> go right
  Step  2: ...
  Step  3: ...

────────────────────────────────────────────────────────────
  Found!  value=31  index=9  position=10
  Steps taken: 3  (list has 16 items, max possible: 5)
────────────────────────────────────────────────────────────
```

---

## Why this matters for learners

1. **The halving insight** — after just one step you've eliminated half the list. After 5 steps you've searched 32 items. After 10 steps: 1,024. After 20 steps: over a million. This is the power of O(log n).

2. **The sorted requirement** — binary search only works because we can make a decision at each step: "too big, go left; too small, go right." On an unsorted list you can't make that decision — you have to look at everything (O(n)).

3. **Off-by-one errors** — the single most common bug. The midpoint is `lo + (hi - lo) / 2`, not `(lo + hi) / 2` (overflow), and updating `lo = mid + 1` / `hi = mid - 1` (not `mid`) ensures the search space strictly shrinks each step.

---

## Part of the educational series

This module is part of a Rosetta Stone–style collection comparing algorithms and data structures across languages. The intent is to show that the **core idea is the same** — only the syntax changes.

See the [educational repo](https://github.com/alternative-intelligence-cp/educational) for more.
