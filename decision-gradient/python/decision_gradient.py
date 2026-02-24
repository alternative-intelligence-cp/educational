#!/usr/bin/env python3
"""
DecisionGradient - Python Port
================================
Two-axis gradient decision construct.

  pbc  (lean)  — where the probability box sits on the A↔B axis
  pbm  (tilt)  — how wide the probability box is

The box position and width together determine how much it overlaps the A
and B halves of the [0, 1] decision space, producing independent confidence
scores for A and B that can both be high, both be low, or anything between.

Axes:
    lean: leanOptA ← leanUnkn ← → leanOptB
    tilt: tiltNone ← tiltUnkn ← → tiltBoth

Decision tokens: A | B | BOTH | UNKNOWN | NONE

Quadratic factor (pbm → width multiplier):
    f(m) = m² + 0.5·m + 0.5
    m=0.0 → 0.50×  (none  — narrowest)
    m=0.5 → 1.00×  (unkn  — normal)
    m=1.0 → 2.00×  (both  — widest)

Author: Randy Hoggard
"""


class DecisionGradient:
    """
    Two-axis gradient decision construct (pbc × pbm).

    Args:
        width (float): Total span. Must be > 1. Defaults to 100.
    """

    def __init__(self, width: float = 100):
        if not isinstance(width, (int, float)) or width <= 1:
            raise ValueError(f"Invalid width: {width}, must be number greater than 1")

        self._width: float = float(width)
        self._pbw: float = self._width * 0.5   # fixed half-span
        self._pbc: float = self._width * 0.5   # lean: [width*0.25 .. width*0.75]
        self._pbm: float = 0.5                  # tilt: [0 .. 1]

    # ── Private helpers ───────────────────────────────────────────────────────

    def _factor(self, m: float) -> float:
        """Quadratic factor: maps pbm → box-width multiplier."""
        return m * m + 0.5 * m + 0.5

    def _box_range(self) -> tuple[float, float]:
        """Normalized [lo, hi] of the probability box. May exceed [0, 1]."""
        c = self._pbc / self._width
        hw = 0.5 * self._factor(self._pbm) / 2
        return c - hw, c + hw

    @staticmethod
    def _clamp(x: float, lo: float, hi: float) -> float:
        return lo if x < lo else (hi if x > hi else x)

    def _validate_amount(self, amount) -> None:
        if not isinstance(amount, (int, float)) or amount <= 0:
            raise ValueError(f"Invalid amount: {amount}, must be number greater than 0")

    # ── Lean (pbc axis) ───────────────────────────────────────────────────────

    def lean_opt_a(self, amount: float = 1) -> "DecisionGradient":
        """Move pbc toward A end (width * 0.25)."""
        self._validate_amount(amount)
        self._pbc = self._clamp(self._pbc - amount,
                                self._width * 0.25, self._width * 0.75)
        return self

    def lean_unkn(self, amount: float = 1) -> "DecisionGradient":
        """Move pbc toward Unknown center (width * 0.5)."""
        self._validate_amount(amount)
        center = self._width * 0.5
        if self._pbc > center:
            self._pbc = max(center, self._pbc - amount)
        elif self._pbc < center:
            self._pbc = min(center, self._pbc + amount)
        return self

    def lean_opt_b(self, amount: float = 1) -> "DecisionGradient":
        """Move pbc toward B end (width * 0.75)."""
        self._validate_amount(amount)
        self._pbc = self._clamp(self._pbc + amount,
                                self._width * 0.25, self._width * 0.75)
        return self

    # ── Tilt (pbm axis) ───────────────────────────────────────────────────────
    # step = amount / pbw so a unit amount has the same feel as lean ops.

    def tilt_both(self, amount: float = 1) -> "DecisionGradient":
        """Move pbm toward 1.0 (widest: 'both')."""
        self._validate_amount(amount)
        self._pbm = self._clamp(self._pbm + amount / self._pbw, 0.0, 1.0)
        return self

    def tilt_unkn(self, amount: float = 1) -> "DecisionGradient":
        """Move pbm toward 0.5 (normal: 'unknown')."""
        self._validate_amount(amount)
        step = amount / self._pbw
        if self._pbm > 0.5:
            self._pbm = max(0.5, self._pbm - step)
        elif self._pbm < 0.5:
            self._pbm = min(0.5, self._pbm + step)
        return self

    def tilt_none(self, amount: float = 1) -> "DecisionGradient":
        """Move pbm toward 0.0 (narrowest: 'none')."""
        self._validate_amount(amount)
        self._pbm = self._clamp(self._pbm - amount / self._pbw, 0.0, 1.0)
        return self

    # ── Readers ───────────────────────────────────────────────────────────────

    def get_opt_a(self) -> float:
        """Confidence in A: overlap of box with [0, 0.5], normalized to [0, 1]."""
        lo, hi = self._box_range()
        overlap = max(0.0, min(0.5, hi) - max(0.0, lo))
        return min(1.0, overlap / 0.5)

    def get_opt_b(self) -> float:
        """Confidence in B: overlap of box with [0.5, 1.0], normalized to [0, 1]."""
        lo, hi = self._box_range()
        overlap = max(0.0, min(1.0, hi) - max(0.5, lo))
        return min(1.0, overlap / 0.5)

    def get_nb(self) -> float:
        """Current pbm position [0..1]: 0=NONE, 0.5=UNKNOWN, 1=BOTH."""
        return self._pbm

    def get_state(self) -> dict:
        """Raw state snapshot: {'a': float, 'b': float, 'nb': float}."""
        return {"a": self.get_opt_a(), "b": self.get_opt_b(), "nb": self.get_nb()}

    def get_decision(self) -> str:
        """Highest-probability token: 'A' | 'B' | 'BOTH' | 'UNKNOWN' | 'NONE'."""
        a = self.get_opt_a()
        b = self.get_opt_b()
        if a > 0.25 and b > 0.25:
            return "BOTH"
        if a > 0.25:
            return "A"
        if b > 0.25:
            return "B"
        if a >= 0.125 or b >= 0.125:
            return "UNKNOWN"
        return "NONE"

    # ── Display ───────────────────────────────────────────────────────────────

    def print_state(self) -> None:
        s = self.get_state()
        print(f"State --> A: {s['a']:.4f} , B: {s['b']:.4f} , NB: {s['nb']:.4f}")

    def print_decision(self) -> None:
        print(f"Decision --> {self.get_decision()}")

    # ── Dunder ────────────────────────────────────────────────────────────────

    def __repr__(self) -> str:
        s = self.get_state()
        return (f"DecisionGradient(a={s['a']:.4f}, b={s['b']:.4f}, "
                f"nb={s['nb']:.4f}, decision={self.get_decision()})")
