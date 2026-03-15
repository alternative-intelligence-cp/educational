#!/usr/bin/env python3
"""
DecisionGradientT — DecisionGradient with a Time Axis
=======================================================
Extends DecisionGradient by recording a snapshot of the full state after
every mutation.  The snapshot history enables:

  - Trajectory review: what decisions were output at each step?
  - Jump detection: which observation caused a large confidence shift?
  - Trigger identification: what mutation crossed the decision boundary?
  - Stability analysis: how many steps were spent in each decision zone?
  - Peak tracking: highest A or B confidence ever reached?

This is the first step toward entanglement and field constructs —
once you can observe how a variable moved through decision space over
time, you can start asking what it means for two variables to move
together (entanglement) or for a whole population to share a field.

Snapshot structure (namedtuple):
    step       int    — mutation counter (0-based)
    trigger    str    — name of the mutation method that caused this snapshot
    pbc        float  — lean axis raw value (position of prob box center)
    pbm        float  — tilt axis raw value (0=none, 0.5=unkn, 1=both)
    a          float  — A confidence [0, 1]
    b          float  — B confidence [0, 1]
    decision   str    — output token: A | B | BOTH | UNKNOWN | NONE

Author: Randy Hoggard
Date: March 2026
"""

from collections import namedtuple, deque
from typing import Optional

from decision_gradient import DecisionGradient


# ── Snapshot type ─────────────────────────────────────────────────────────────

Snapshot = namedtuple(
    "Snapshot",
    ["step", "trigger", "pbc", "pbm", "a", "b", "decision"]
)


# ── DecisionGradientT ─────────────────────────────────────────────────────────

class DecisionGradientT(DecisionGradient):
    """
    DecisionGradient with a Time Axis.

    Every mutation method (lean_*, tilt_*, observe) records a Snapshot after
    it completes.  The snapshot buffer is the 'time axis': an ordered record
    of every state the gradient passed through.

    Args:
        width        (float): Total span. Must be > 1. Defaults to 100.
        history_depth (int|None): Maximum snapshots to retain.
                      None (default) = unlimited — keep everything.
                      N > 0 = keep only the N most recent snapshots.
                      Useful for long-running sensors where you only care
                      about recent history.

    History is NOT populated at construction — the first snapshot is taken
    when the first mutation occurs.  Use snapshot_now('init') if you want
    to record the starting state explicitly.
    """

    def __init__(self, width: float = 100, history_depth: Optional[int] = None):
        super().__init__(width)
        if history_depth is not None and (not isinstance(history_depth, int) or history_depth < 1):
            raise ValueError(f"history_depth must be a positive int or None, got {history_depth!r}")
        self._history_depth = history_depth
        self._history: deque = deque(maxlen=history_depth)  # maxlen=None = unlimited
        self._step: int = 0

    # ── Snapshot machinery ────────────────────────────────────────────────────

    def snapshot_now(self, trigger: str = "manual") -> "DecisionGradientT":
        """
        Record the current state as a snapshot without mutating anything.
        Useful for capturing the initial state before any mutations.

        Args:
            trigger: Label for this snapshot (e.g. 'init', 'checkpoint').

        Returns:
            self  (for chaining)
        """
        self._history.append(Snapshot(
            step     = self._step,
            trigger  = trigger,
            pbc      = self._pbc,
            pbm      = self._pbm,
            a        = self.get_opt_a(),
            b        = self.get_opt_b(),
            decision = self.get_decision(),
        ))
        self._step += 1
        return self

    def _snap(self, trigger: str) -> None:
        """Internal: take a snapshot after a mutation (increments step)."""
        self.snapshot_now(trigger)

    # ── Mutation overrides (each calls super then snaps) ──────────────────────

    def lean_opt_a(self, amount: float = 1) -> "DecisionGradientT":
        """Move pbc toward A; records snapshot."""
        super().lean_opt_a(amount)
        self._snap("lean_opt_a")
        return self

    def lean_unkn(self, amount: float = 1) -> "DecisionGradientT":
        """Move pbc toward Unknown center; records snapshot."""
        super().lean_unkn(amount)
        self._snap("lean_unkn")
        return self

    def lean_opt_b(self, amount: float = 1) -> "DecisionGradientT":
        """Move pbc toward B; records snapshot."""
        super().lean_opt_b(amount)
        self._snap("lean_opt_b")
        return self

    def tilt_both(self, amount: float = 1) -> "DecisionGradientT":
        """Move pbm toward 1.0 (both); records snapshot."""
        super().tilt_both(amount)
        self._snap("tilt_both")
        return self

    def tilt_unkn(self, amount: float = 1) -> "DecisionGradientT":
        """Move pbm toward 0.5 (unknown); records snapshot."""
        super().tilt_unkn(amount)
        self._snap("tilt_unkn")
        return self

    def tilt_none(self, amount: float = 1) -> "DecisionGradientT":
        """Move pbm toward 0.0 (none); records snapshot."""
        super().tilt_none(amount)
        self._snap("tilt_none")
        return self

    def observe(
        self,
        lean: float = 0,
        tilt: float = 0,
        trigger: str = "observe",
        direction: str = "b",
    ) -> "DecisionGradientT":
        """
        Convenience: apply lean and/or tilt evidence in one call.

        Args:
            lean      (float): Lean amount (> 0). Applied in `direction`.
            tilt      (float): Tilt amount (> 0). Applied toward 'both' when
                               direction='b', toward 'none' when direction='a'.
            trigger   (str):   Label stored in the snapshot for this observation.
            direction (str):   'a' or 'b' — which way lean (and paired tilt) go.

        Returns:
            self  (for chaining)

        Note: Takes a single snapshot after BOTH axes are applied, tagged
        with `trigger`.  This keeps the history at one entry per observation
        event rather than two, which is more useful for trigger analysis.
        """
        if lean < 0:
            raise ValueError("lean must be >= 0; pass direction='a' or 'b' to set direction")
        if tilt < 0:
            raise ValueError("tilt must be >= 0; pass direction='a' or 'b' to set tilt direction")

        # Apply lean
        if lean > 0:
            if direction == "a":
                super().lean_opt_a(lean)
            else:
                super().lean_opt_b(lean)

        # Apply tilt: evidence toward B widens the box (both); toward A narrows (none)
        if tilt > 0:
            if direction == "b":
                super().tilt_both(tilt)
            else:
                super().tilt_none(tilt)

        # Single snapshot for the whole observation
        self._history.append(Snapshot(
            step     = self._step,
            trigger  = trigger,
            pbc      = self._pbc,
            pbm      = self._pbm,
            a        = self.get_opt_a(),
            b        = self.get_opt_b(),
            decision = self.get_decision(),
        ))
        self._step += 1
        return self

    # ── Time-axis queries ─────────────────────────────────────────────────────

    @property
    def history(self) -> list[Snapshot]:
        """All recorded snapshots in chronological order."""
        return list(self._history)

    @property
    def step_count(self) -> int:
        """Total number of mutations recorded (including manual snapshots)."""
        return self._step

    def get_snapshot(self, index: int) -> Snapshot:
        """
        Retrieve snapshot by index.

        Args:
            index: 0-based from oldest retained snapshot.
                   Negative indices work like Python lists (-1 = most recent).

        Returns:
            Snapshot namedtuple.
        """
        h = list(self._history)
        return h[index]

    def trajectory(self) -> list[str]:
        """
        Return the sequence of decision tokens over time.

        Example:
            ['UNKNOWN', 'UNKNOWN', 'B', 'B', 'BOTH', 'BOTH', 'B']

        Useful for at-a-glance reading of the decision arc.
        """
        return [s.decision for s in self._history]

    def delta(self, index: int, axis: str = "a") -> float:
        """
        Compute the change in a metric between snapshot[index] and snapshot[index-1].

        Args:
            index: Target snapshot index (must be >= 1 within retained history).
            axis:  Metric to compare: 'a', 'b', 'pbc', 'pbm'.

        Returns:
            Signed delta (positive = increased, negative = decreased).
            Returns 0.0 when index == 0 or only one snapshot exists.
        """
        h = list(self._history)
        if index <= 0 or index >= len(h):
            return 0.0
        return getattr(h[index], axis) - getattr(h[index - 1], axis)

    def find_triggers(
        self,
        threshold: float = 0.15,
        axis: str = "a",
    ) -> list[dict]:
        """
        Find snapshots where the metric changed by more than `threshold`.

        These are the moments where something significant happened — the
        observations that drove the gradient over a boundary.

        Args:
            threshold (float): Minimum absolute delta to flag.
                               Default 0.15 = 15% confidence shift.
            axis      (str):   Metric to watch: 'a', 'b', 'pbc', 'pbm'.
                               'a'/'b'  — confidence output  (range 0-1).
                               'pbc'    — lean axis position  (range 0..width).
                               'pbm'    — tilt axis position  (range 0..1).

        Returns:
            List of dicts, each with:
                step     — mutation step number
                trigger  — label of the mutation that caused this
                delta    — signed change in the metric
                before   — metric value at previous snapshot
                after    — metric value at this snapshot
                decision — decision token at this snapshot
        """
        h = list(self._history)
        results = []
        for i in range(1, len(h)):
            d = getattr(h[i], axis) - getattr(h[i - 1], axis)
            if abs(d) >= threshold:
                results.append({
                    "step":     h[i].step,
                    "trigger":  h[i].trigger,
                    "delta":    round(d, 6),
                    "before":   round(getattr(h[i - 1], axis), 6),
                    "after":    round(getattr(h[i], axis), 6),
                    "decision": h[i].decision,
                })
        return results

    def find_decision_changes(self) -> list[dict]:
        """
        Return every snapshot where the decision token changed from the previous.

        These are the moments the gradient crossed a zone boundary — the
        most semantically meaningful events in the timeline.

        Returns:
            List of dicts with: step, trigger, from_decision, to_decision, a, b.
        """
        h = list(self._history)
        results = []
        for i in range(1, len(h)):
            if h[i].decision != h[i - 1].decision:
                results.append({
                    "step":          h[i].step,
                    "trigger":       h[i].trigger,
                    "from_decision": h[i - 1].decision,
                    "to_decision":   h[i].decision,
                    "a":             round(h[i].a, 6),
                    "b":             round(h[i].b, 6),
                })
        return results

    def peak_confidence(self, axis: str = "a") -> dict:
        """
        Find the snapshot with the highest value on the given axis.

        Args:
            axis: 'a' or 'b'.

        Returns:
            Dict with: step, trigger, value, decision.
            Returns empty dict if no history.
        """
        h = list(self._history)
        if not h:
            return {}
        peak = max(h, key=lambda s: getattr(s, axis))
        return {
            "step":     peak.step,
            "trigger":  peak.trigger,
            "value":    round(getattr(peak, axis), 6),
            "decision": peak.decision,
        }

    def stability_report(self) -> dict:
        """
        Count how many steps were spent in each decision zone.

        Returns:
            Dict mapping decision token → step count.
            Example: {'UNKNOWN': 3, 'B': 5, 'BOTH': 2}
        """
        counts: dict[str, int] = {}
        for s in self._history:
            counts[s.decision] = counts.get(s.decision, 0) + 1
        return counts

    def reset(self) -> "DecisionGradientT":
        """
        Reset the gradient to its initial state AND clear the history.

        Returns:
            self  (for chaining)
        """
        # Re-init the parent state
        super().__init__(self._width)
        self._history.clear()
        self._step = 0
        return self

    # ── Display ───────────────────────────────────────────────────────────────

    def print_history(self, limit: Optional[int] = None) -> None:
        """
        Print the snapshot history in a human-readable table.

        Args:
            limit: If set, print only the last N snapshots.
        """
        h = list(self._history)
        if limit is not None:
            h = h[-limit:]
        if not h:
            print("(no history)")
            return
        print(f"{'step':>4}  {'trigger':<18}  {'pbc':>7}  {'pbm':>5}  {'A':>6}  {'B':>6}  decision")
        print("─" * 72)
        for s in h:
            print(
                f"{s.step:>4}  {s.trigger:<18}  {s.pbc:7.2f}  {s.pbm:5.3f}"
                f"  {s.a:6.4f}  {s.b:6.4f}  {s.decision}"
            )

    def print_trajectory(self) -> None:
        """Print the decision token sequence on one line."""
        t = self.trajectory()
        if not t:
            print("(no history)")
            return
        print(" → ".join(t))

    def print_triggers(self, threshold: float = 0.15, axis: str = "a") -> None:
        """Print the trigger report for the given axis/threshold."""
        triggers = self.find_triggers(threshold, axis)
        if not triggers:
            print(f"No triggers found (threshold={threshold}, axis='{axis}')")
            return
        print(f"Triggers (axis='{axis}', threshold≥{threshold}):")
        for t in triggers:
            sign = "+" if t["delta"] >= 0 else ""
            print(
                f"  step {t['step']:>3}  [{t['trigger']:<18}]  "
                f"{t['before']:.4f} → {t['after']:.4f}  "
                f"({sign}{t['delta']:.4f})  → {t['decision']}"
            )

    def print_decision_changes(self) -> None:
        """Print every decision boundary crossing."""
        changes = self.find_decision_changes()
        if not changes:
            print("No decision changes in history.")
            return
        print("Decision boundary crossings:")
        for c in changes:
            print(
                f"  step {c['step']:>3}  [{c['trigger']:<18}]  "
                f"{c['from_decision']:>7} → {c['to_decision']:<7}  "
                f"(A={c['a']:.4f}, B={c['b']:.4f})"
            )

    # ── Dunder ────────────────────────────────────────────────────────────────

    def __repr__(self) -> str:
        s = self.get_state()
        return (
            f"DecisionGradientT(a={s['a']:.4f}, b={s['b']:.4f}, "
            f"nb={s['nb']:.4f}, decision={self.get_decision()}, "
            f"steps={self._step}, history={len(self._history)})"
        )
