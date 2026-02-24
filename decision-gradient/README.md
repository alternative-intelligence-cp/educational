# Decision Gradient

A multi-language side-by-side implementation of two decision weight constructs.
Useful for comparing idioms, ergonomics, and trade-offs across JavaScript, Python, and C++.

---

## Constructs

### SlidingWindow (simple)

Single-axis, zero-sum A/B weight tracker. The invariant `a + b == width` is always
maintained. Shifting toward A pulls the same amount away from B, and vice versa.

Returns: `UNKNOWN | A | B`

### DecisionGradient (sophisticated)

Two independent axes:

| Axis | Parameter | Range | Endpoints |
|------|-----------|-------|-----------|
| **lean** | `pbc` | `[width×0.25 .. width×0.75]` | maxA ↔ center ↔ maxB |
| **tilt** | `pbm` | `[0 .. 1]` | none ↔ unkn ↔ both |

The tilt axis drives a quadratic factor that controls the probability box width:

$$f(m) = m^2 + 0.5 \cdot m + 0.5$$

| pbm | factor | meaning |
|-----|--------|---------|
| 0.0 | 0.50× | narrow — "none of the above" |
| 0.5 | 1.00× | normal — "genuinely unknown" |
| 1.0 | 2.00× | wide   — "could be both" |

The box overlap with each half of `[0, 1]` produces independent A and B
confidence scores, enabling five output tokens:

`A | B | BOTH | UNKNOWN | NONE`

Inspired by Q-type quantum confidence modeling from the Aria project.

---

## Languages

```
js/
  slidingWindow.js       — zero-sum window (JS IIFE module)
  decisionGradient.js    — gradient construct (JS constructor)

python/
  sliding_window.py      — zero-sum window (Python class)
  decision_gradient.py   — gradient construct (Python class)

cpp/
  sliding_window.hpp     — zero-sum window (C++ header-only)
  decision_gradient.hpp  — gradient construct (C++ header-only)
```

---

## Quick Examples

### JavaScript
```js
const SlidingWindow   = require('./js/slidingWindow');
const DecisionGradient = require('./js/decisionGradient');

const sw = new SlidingWindow(100);
sw.shiftA(20);
sw.printState();           // State --> A: 0.7 , B: 0.3
console.log(sw.getBestSemantic()); // A

const dg = new DecisionGradient(100);
dg.leanOptA(20).tiltBoth(25);
dg.printState();
dg.printDecision();
```

### Python
```python
from python.sliding_window import SlidingWindow
from python.decision_gradient import DecisionGradient

sw = SlidingWindow(100)
sw.shift_a(20)
sw.print_state()           # State --> A: 0.7 , B: 0.3
print(sw.get_best_semantic()) # A

dg = DecisionGradient(100)
dg.lean_opt_a(20).tilt_both(25)
dg.print_state()
dg.print_decision()
```

### C++
```cpp
#include "cpp/sliding_window.hpp"
#include "cpp/decision_gradient.hpp"

SlidingWindow sw(100.0f);
sw.shift_a(20.0f);
sw.print_state();                   // State --> A: 0.7000 , B: 0.3000
printf("%s\n", sw.get_best_semantic()); // A

DecisionGradient dg(100.0f);
dg.lean_opt_a(20.0f).tilt_both(25.0f);
dg.print_state();
dg.print_decision();
```

---

## What the comparison shows

| Concern | JS | Python | C++ |
|---|---|---|---|
| Encapsulation | closure (IIFE) | `_` prefix convention | `private:` enforced |
| Validation | `console.error` + null | `raise ValueError` | `throw std::invalid_argument` |
| Chaining | explicit `return this` not shown | `return self` | `return *this` |
| Output token type | `string` | `string` | `const char*` + enum |
| Import model | `require` / `module.exports` | class import | `#include` |
| Float precision | IEEE 754 double | IEEE 754 double | 32-bit `float` |

The float precision difference (JS/Python use `double`, C++ uses `float`) is visible
at the 7th decimal place but produces no difference in decisions within normal usage.
