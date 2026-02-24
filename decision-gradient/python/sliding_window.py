#!/usr/bin/env python3
"""
SlidingWindow - Python Port
============================
Zero-sum A/B sliding window. The a + b == MAX invariant is always maintained.
Moving toward A nudges away from B and vice versa — a simple single-axis
decision weight tracker.

Author: Randy Hoggard
"""


class SlidingWindow:
    """
    Zero-sum sliding window between two options (A and B).

    Maintains the invariant: a + b == width at all times.
    Both start at width/2 (perfectly balanced).

    Args:
        width (float): Total span. Must be > 1. Defaults to 100.
    """

    def __init__(self, width: float = 100):
        if not isinstance(width, (int, float)) or width <= 1:
            raise ValueError(f"Invalid width: {width}, must be number greater than 1")

        self._max: float = float(width)
        self._a: float = self._max * 0.5
        self._b: float = self._max * 0.5

    # ── Mutators ──────────────────────────────────────────────────────────────

    def shift_a(self, amount: float = 1) -> "SlidingWindow":
        """Increase weight on A (decreases B by the same amount)."""
        if not isinstance(amount, (int, float)) or amount <= 0:
            raise ValueError(f"Invalid amount: {amount}, must be number greater than 0")
        if self._a + amount > self._max:
            self._a = self._max
            self._b = 0.0
        else:
            self._a += amount
            self._b -= amount
        return self

    def shift_b(self, amount: float = 1) -> "SlidingWindow":
        """Increase weight on B (decreases A by the same amount)."""
        if not isinstance(amount, (int, float)) or amount <= 0:
            raise ValueError(f"Invalid amount: {amount}, must be number greater than 0")
        if self._b + amount > self._max:
            self._b = self._max
            self._a = 0.0
        else:
            self._b += amount
            self._a -= amount
        return self

    # ── Readers ──────────────────────────────────────────────────────────────

    def get_a(self) -> float:
        """Normalized A weight in [0, 1]."""
        return self._a / self._max

    def get_b(self) -> float:
        """Normalized B weight in [0, 1]."""
        return self._b / self._max

    def get_state(self) -> dict:
        """Raw state snapshot: {'a': float, 'b': float}."""
        return {"a": self.get_a(), "b": self.get_b()}

    def get_best(self) -> int:
        """Returns 0=UNKNOWN, 1=A, 2=B."""
        if self._a == self._b:
            return 0
        return 1 if self._a > self._b else 2

    def get_best_semantic(self) -> str:
        """Returns 'UNKNOWN', 'A', or 'B'."""
        return ("UNKNOWN", "A", "B")[self.get_best()]

    # ── Display ───────────────────────────────────────────────────────────────

    def print_state(self) -> None:
        s = self.get_state()
        print(f"State --> A: {s['a']} , B: {s['b']}")

    # ── Dunder ────────────────────────────────────────────────────────────────

    def __repr__(self) -> str:
        s = self.get_state()
        return f"SlidingWindow(a={s['a']:.4f}, b={s['b']:.4f}, best={self.get_best_semantic()})"
