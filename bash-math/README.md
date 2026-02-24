# bash-math

A sourced bash math library that wraps `bc` so you never have to remember its syntax.
Source it once, then `math.<TAB>` autocompletes everything.

---

## The Problem

Bash only does integer arithmetic natively:

```bash
echo $((3 / 2))    # → 1  (truncated — wrong)
echo $((1.5 + 2))  # → syntax error
```

The real answer lives in `bc`, but its syntax is different enough to be annoying to
context-switch to mid-script:

```bash
echo "scale=4; 3/2" | bc -l    # → 1.5000  (works, but you had to remember all that)
echo "s(1.5707)" | bc -l       # → sin — but who remembers s() vs sin()?
```

## The Solution — One Pattern

Everything in this library is a wrapper around one idiom:

```bash
echo "EXPRESSION" | bc -l
```

The `-l` flag loads bc's math library (sin, cos, log, exp) and sets
`scale=20` (20 decimal places by default).

```bash
source math.sh
math.add 1.5 2.3      # → 3.8
math.sqrt 2           # → 1.41421356237309504880
math.sin 1.5707963    # → 1.00000000000000000000
```

---

## Function Reference

### Standard Arithmetic
| Function | Description | Example |
|---|---|---|
| `math.add A B` | A + B | `math.add 1.5 2.5` → `4.0` |
| `math.sub A B` | A - B | `math.sub 10 3.5` → `6.5` |
| `math.mul A B` | A × B | `math.mul 2.5 4` → `10.0` |
| `math.div A B` | A ÷ B | `math.div 10 3` → `3.333...` |
| `math.mod A B` | A mod B | `math.mod 10 3` → `1` |
| `math.pow A B` | Aᴮ | `math.pow 2 10` → `1024` |
| `math.sqrt A` | √A | `math.sqrt 2` → `1.414...` |

### Derived Arithmetic
| Function | Description |
|---|---|
| `math.sqr A` | A² (calls `math.pow`) |
| `math.cub A` | A³ (calls `math.pow`) |
| `math.abs A` | \|A\| |
| `math.sum A B C ...` | sum of any number of arguments |

### Rounding
| Function | Description |
|---|---|
| `math.floor A` | ⌊A⌋ — toward -∞ |
| `math.ceil A` | ⌈A⌉ — toward +∞ |
| `math.round A` | round half-up |
| `math.scale A N` | trim to N decimal places |

### Range
| Function | Description |
|---|---|
| `math.min A B` | smaller of A, B |
| `math.max A B` | larger of A, B |
| `math.clamp V LO HI` | clamp V to [LO, HI] |

### Trigonometry (radians)
| Function | Description |
|---|---|
| `math.sin A` | sin(A) |
| `math.cos A` | cos(A) |
| `math.tan A` | tan(A) — derived as sin/cos |
| `math.atan A` | atan(A) |
| `math.atan2 Y X` | atan(Y/X) |

bc only provides `s()`, `c()`, and `a()` natively. `tan`, `atan2`, `asin`, `acos` are
all derivable from those three — see the source for the pattern.

### Logarithms / Exponents
| Function | Description |
|---|---|
| `math.log A` | ln(A) — natural log |
| `math.log2 A` | log₂(A) — derived: `l(A)/l(2)` |
| `math.log10 A` | log₁₀(A) — derived: `l(A)/l(10)` |
| `math.exp A` | eᴬ |

bc only provides natural log (`l()`) and `e()`. Any log base is derivable via the
change-of-base formula: **log_b(x) = ln(x) / ln(b)**.

### Constants
| Function | Returns |
|---|---|
| `math.pi` | 3.14159265358979323844 |
| `math.e` | 2.71828182845904523536 |

π is computed as `4*a(1)` — four times arctan(1). This is a classic bc idiom.

### Comparisons
All return `"1"` (true) or `"0"` (false) as strings.

```bash
if [[ $(math.gt $x $threshold) == 1 ]]; then
    echo "above threshold"
fi

# Or as arithmetic:
if (( $(math.gt $x $threshold) )); then echo "true"; fi
```

| Function | Meaning |
|---|---|
| `math.equ A B` | A == B |
| `math.gt A B` | A > B |
| `math.lt A B` | A < B |
| `math.gte A B` | A >= B |
| `math.lte A B` | A <= B |
| `math.cmp A B` | -1 / 0 / 1 (three-way) |

### Base Conversions
| Function | Description |
|---|---|
| `math.toBinary N` | decimal → binary |
| `math.fromBinary N` | binary → decimal |
| `math.toHex N` | decimal → hex |
| `math.fromHex N` | hex → decimal |
| `math.toBase N BASE` | decimal → any base |
| *(toTernary, toOctal, toNonary — same pattern)* | |

**Important bc gotcha**: when doing `ibase` + `obase` conversions, always set
`obase` before `ibase`. bc interprets subsequent literals using the current `ibase`,
so setting `ibase=16` first would cause `obase=10` to be read as hex.

### Meta / Raw
| Function | Description |
|---|---|
| `math.digits A` | digit count before the decimal point |
| `math.eval EXPR` | raw bc expression — `math.eval "3*a(1)*2"` → π×2 |
| `math.pyth2 X Y` | 2D hypotenuse √(x²+y²) |
| `math.pyth3 X Y Z` | 3D distance √(x²+y²+z²) |

---

## What This Teaches

1. **The pipe-to-bc pattern** — one idiom covers all float math in bash
2. **bc's math library** (`-l` flag) — what it provides vs what you derive
3. **Change-of-base formula** — `log2`, `log10` are not bc builtins; the source shows the derivation
4. **bc's `define`** — floor/ceil/round require defining a multi-line function inline via the heredoc-style string
5. **Comparison pattern** — using `(( $() ))` to bridge bc output back to bash conditionals
6. **ibase/obase ordering** — a subtle bc trap that bites everyone once

---

## Bugs Fixed From Original

The original `math.sh` had three bugs not visible until you actually called the functions:

```bash
# BUG 1: math.sum called bare `add` (undefined) instead of math.add
total=$(add $total $1)          # broken
total=$(math.add "$total" "$1") # fixed

# BUG 2 & 3: math.sqr / math.cub called math.exp (expression eval) not math.pow
math.sqr(){ math.exp $1 2; }   # broken — bc sees "1 2" as a syntax error
math.sqr(){ math.pow "$1" 2; } # fixed
```
