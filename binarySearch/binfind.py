#!/usr/bin/env python3
"""
Binary Search - Python Implementation

Binary search works on a sorted list by repeatedly halving the search space:
  1. Check the middle element
  2. If it matches, done
  3. If the target is larger, discard the left half
  4. If the target is smaller, discard the right half
  5. Repeat until found or the search space is empty

Time complexity:  O(log n)  — each step cuts the problem in half
Space complexity: O(1)      — iterative, no extra memory needed

Usage:
  python3 binfind.py [target] [n1 n2 n3 ...]

Examples:
  python3 binfind.py                      # uses built-in demo list, target 31
  python3 binfind.py 73                   # search for 73 in demo list
  python3 binfind.py 7 1 3 5 7 9 11 13   # search for 7 in a custom sorted list
"""

import sys
import math

# ── Default demo data ─────────────────────────────────────────────────────────

DEFAULT_LIST   = [1, 3, 6, 8, 9, 10, 11, 23, 25, 31, 65, 73, 105, 205, 569, 1024]
DEFAULT_TARGET = 31

# ── ANSI color helpers ────────────────────────────────────────────────────────

RESET  = '\033[0m'
YELLOW = '\033[33m'   # current probe
GREEN  = '\033[32m'   # found
GREY   = '\033[90m'   # eliminated

def fmt_num(n, width):
    return f'{n:>{width}}'

def colorize(text, color):
    return f'{color}{text}{RESET}'

# ── Argument parsing ──────────────────────────────────────────────────────────

args = sys.argv[1:]

# Parse target
if not args:
    target = DEFAULT_TARGET
else:
    try:
        target = float(args[0])
        if not math.isfinite(target):
            raise ValueError()
        if target == int(target):
            target = int(target)
    except ValueError:
        print(f'Error: target "{args[0]}" is not a valid number.', file=sys.stderr)
        sys.exit(1)

# Parse list
if len(args) <= 1:
    lst = DEFAULT_LIST[:]
else:
    lst = []
    for raw in args[1:]:
        try:
            v = float(raw)
            if not math.isfinite(v):
                raise ValueError()
            lst.append(int(v) if v == int(v) else v)
        except ValueError:
            print(f'Error: list item "{raw}" is not a valid number.', file=sys.stderr)
            sys.exit(1)

# Validate: non-empty
if not lst:
    print('Error: list must not be empty.', file=sys.stderr)
    sys.exit(1)

# Validate: sorted
for i in range(len(lst) - 1):
    if lst[i] > lst[i + 1]:
        print(f'Error: list is not sorted (found {lst[i]} before {lst[i+1]}).', file=sys.stderr)
        sys.exit(1)

# ── Visualization ─────────────────────────────────────────────────────────────

# Column width: pad to the widest number
col_w = max(len(str(v)) for v in lst) + 1

def render_step(lo, mid, hi, eliminated, step_num, found=False):
    parts = []
    for i, v in enumerate(lst):
        s = fmt_num(v, col_w)
        if i in eliminated:
            parts.append(colorize(s, GREY))
        elif found and i == mid:
            parts.append(colorize(s, GREEN))
        elif i == mid:
            parts.append(colorize(s, YELLOW))
        else:
            parts.append(s)
    row = '  '.join(parts)
    probe_val = lst[mid]
    label = f'lo={lo}  mid={mid}  hi={hi}  probe={probe_val}'
    print(f'  Step {step_num:2d}: [ {row} ]')
    print(f'           {label}')

# ── Binary search ─────────────────────────────────────────────────────────────

def binary_search(lst, target):
    """
    Iterative binary search.
    Returns (index, steps) where index is -1 if not found.
    Records each step for visualization.
    """
    lo = 0
    hi = len(lst) - 1
    steps = []
    eliminated = set()

    while lo <= hi:
        mid = (lo + hi) // 2
        steps.append((lo, mid, hi, frozenset(eliminated)))

        if lst[mid] == target:
            return mid, steps, eliminated
        elif lst[mid] < target:
            # eliminate everything from lo..mid
            for i in range(lo, mid + 1):
                eliminated.add(i)
            lo = mid + 1
        else:
            # eliminate everything from mid..hi
            for i in range(mid, hi + 1):
                eliminated.add(i)
            hi = mid - 1

    return -1, steps, eliminated

# ── Main output ───────────────────────────────────────────────────────────────

DIV = '─' * 60

print(DIV)
print('  Binary Search')
print(DIV)
print(f'  List   : {lst}')
print(f'  Length : {len(lst)}')
print(f'  Target : {target}')
print(DIV)
print()
print(f'  Legend:  {colorize("  [probe]  ", YELLOW)}'
      f'{colorize("  [found]  ", GREEN)}'
      f'{colorize("  [eliminated]  ", GREY)}')
print()

found_idx, steps, eliminated = binary_search(lst, target)

for i, (lo, mid, hi, elim_snap) in enumerate(steps):
    is_last = (i == len(steps) - 1)
    render_step(lo, mid, hi, elim_snap, i + 1, found=(is_last and found_idx != -1))

print()
print(DIV)

max_steps = math.ceil(math.log2(len(lst) + 1))
n_steps   = len(steps)

if found_idx != -1:
    print(f'  Found!  value={target}  index={found_idx}  position={found_idx + 1}')
else:
    print(f'  Not found: {target} is not in the list.')

print(f'  Steps taken: {n_steps}  (list has {len(lst)} items, max possible: {max_steps})')
print(DIV)
