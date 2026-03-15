#!/usr/bin/env python3
"""
GradientField — A Spatial Field Driven by a Single Q Emitter
==============================================================
One DecisionGradientT acts as the emitter.  N FieldNodes each hold a
DecisionGradient and a weight vector (lean_w, tilt_w) that describes
their individual relationship to the emitter.

Projection mechanics:
    The emitter's current state is normalised into two signals:

        lean_signal = (pbc - center) / quarter_width    # [-1 .. +1]
                      -1 = maxA, 0 = center, +1 = maxB

        tilt_signal = pbm - 0.5                          # [-0.5 .. +0.5]
                      -0.5 = tilt_none, 0 = tilt_unkn, +0.5 = tilt_both

    Each node receives:
        lean_delta = lean_w × lean_signal × base_lean
        tilt_delta = tilt_w × tilt_signal × base_tilt

    lean_delta is applied toward A (negative) or B (positive).
    tilt_delta is applied toward BOTH (positive) or NONE (negative).

    The node's pbc and pbm are *reset* to center/0.5 before each
    projection so the pattern reflects the emitter's current state
    rather than accumulating history.  (This mirrors how a Chladni plate
    responds to the current frequency, not the integral of all past ones.)

Weight vector meanings:
    lean_w  tilt_w   Node role
    ──────  ──────   ────────────────────────────────────────────────────
     1.0     1.0     Directly tracks emitter — both axes
     0.0     1.0     Tilt-only — responds to uncertainty, ignores lean
    -1.0     1.0     Direction-inverted — opposite lean, same uncertainty
     1.0    -1.0     Anti-tilt — tracks lean, anti-correlated uncertainty
     0.5     0.5     Half-strength node
     0.0     0.0     Nodal point — sand accumulates here; never moves

FieldPattern (namedtuple):
    emitter_step     int    — emitter step counter at projection time
    emitter_lean     float  — lean_signal at projection time
    emitter_tilt     float  — tilt_signal at projection time
    emitter_decision str    — emitter's own decision token
    tokens           tuple  — decision token for each node
    a_vals           tuple  — A confidence for each node
    b_vals           tuple  — B confidence for each node

Vocabulary:
    "project"   — update all nodes from the emitter's current state
    "pattern"   — the N-element array of node decision tokens
    "nodal"     — a node with near-zero weights; its token rarely changes
    "anti-node" — a node whose response is inverted on one or both axes

Author: Randy Hoggard
Date: March 2026
"""

from collections import namedtuple, deque
from typing import Optional

from decision_gradient import DecisionGradient
from decision_gradient_t import DecisionGradientT


# ── FieldNode ─────────────────────────────────────────────────────────────────

class FieldNode:
    """
    A single node in the GradientField.

    Each node is a DecisionGradient plus a weight vector (lean_w, tilt_w)
    that controls how the emitter's signal reaches it.

    Args:
        name   (str):   Label for display and analysis.
        lean_w (float): Lean sensitivity [-1.0 .. 1.0].
                        Positive  → node leans in the same direction as emitter.
                        Negative  → node leans in the opposite direction.
                        0.0       → lean is ignored (nodal on lean axis).
        tilt_w (float): Tilt sensitivity [-1.0 .. 1.0].
                        Positive  → node's uncertainty rises with emitter's.
                        Negative  → node narrows when emitter widens (anti-tilt).
                        0.0       → tilt is ignored (nodal on tilt axis).
        width  (float): Width for the underlying DecisionGradient. Default 100.
    """

    def __init__(
        self,
        name: str,
        lean_w: float = 1.0,
        tilt_w: float = 1.0,
        width: float = 100,
    ):
        if not isinstance(lean_w, (int, float)) or not (-1.0 <= lean_w <= 1.0):
            raise ValueError(f"lean_w must be in [-1.0, 1.0], got {lean_w!r}")
        if not isinstance(tilt_w, (int, float)) or not (-1.0 <= tilt_w <= 1.0):
            raise ValueError(f"tilt_w must be in [-1.0, 1.0], got {tilt_w!r}")

        self.name   = name
        self.lean_w = float(lean_w)
        self.tilt_w = float(tilt_w)
        self._dg    = DecisionGradient(width)

    def reset(self) -> None:
        """Reset to neutral state (pbc=center, pbm=0.5)."""
        self._dg._pbc = self._dg._width * 0.5
        self._dg._pbm = 0.5

    def apply(self, lean_delta: float, tilt_delta: float) -> None:
        """
        Apply a lean+tilt delta to the underlying gradient.

        lean_delta > 0 → lean toward B
        lean_delta < 0 → lean toward A
        tilt_delta > 0 → tilt toward BOTH
        tilt_delta < 0 → tilt toward NONE
        """
        if lean_delta > 0:
            self._dg.lean_opt_b(lean_delta)
        elif lean_delta < 0:
            self._dg.lean_opt_a(-lean_delta)

        if tilt_delta > 0:
            self._dg.tilt_both(tilt_delta)
        elif tilt_delta < 0:
            self._dg.tilt_none(-tilt_delta)

    @property
    def decision(self) -> str:
        return self._dg.get_decision()

    @property
    def a(self) -> float:
        return self._dg.get_opt_a()

    @property
    def b(self) -> float:
        return self._dg.get_opt_b()

    @property
    def pbc(self) -> float:
        return self._dg._pbc

    @property
    def pbm(self) -> float:
        return self._dg._pbm

    def __repr__(self) -> str:
        return (f"FieldNode({self.name!r}, lean_w={self.lean_w:+.2f}, "
                f"tilt_w={self.tilt_w:+.2f}, "
                f"dec={self.decision})")


# ── FieldPattern ──────────────────────────────────────────────────────────────

FieldPattern = namedtuple(
    "FieldPattern",
    ["emitter_step", "emitter_lean", "emitter_tilt",
     "emitter_decision", "tokens", "a_vals", "b_vals"]
)


# ── GradientField ─────────────────────────────────────────────────────────────

class GradientField:
    """
    A spatial field driven by a single DecisionGradientT emitter.

    The emitter's state is projected onto N FieldNodes each step.
    The resulting array of decision tokens is the "field pattern" — the
    Chladni shape produced by the current "frequency" of the emitter.

    Args:
        nodes          (list[FieldNode]):  The field nodes. Must be non-empty.
        base_lean      (float):            Maximum lean delta applied to a
                                           node with lean_w=1.0 when emitter
                                           lean_signal=1.0.  Default 20.
        base_tilt      (float):            Maximum tilt delta applied to a
                                           node with tilt_w=1.0 when emitter
                                           tilt_signal=0.5.  Default 20.
        emitter_width  (float):            Width for the emitter DGT. Default 100.
        history_depth  (int|None):         Max FieldPatterns to retain. None=all.
    """

    def __init__(
        self,
        nodes: list,
        base_lean: float = 20.0,
        base_tilt: float = 20.0,
        emitter_width: float = 100,
        history_depth: Optional[int] = None,
    ):
        if not nodes:
            raise ValueError("nodes must be a non-empty list of FieldNode instances")
        self._nodes = list(nodes)
        self._base_lean = float(base_lean)
        self._base_tilt = float(base_tilt)
        self._emitter = DecisionGradientT(emitter_width)
        self._history: deque = deque(maxlen=history_depth)

    # ── Properties ────────────────────────────────────────────────────────────

    @property
    def emitter(self) -> DecisionGradientT:
        """The Q emitter driving the field."""
        return self._emitter

    @property
    def nodes(self) -> list:
        return list(self._nodes)

    @property
    def node_names(self) -> list:
        return [n.name for n in self._nodes]

    # ── Signal extraction ─────────────────────────────────────────────────────

    def _signals(self) -> tuple:
        """
        Extract normalised [lean_signal, tilt_signal] from the emitter.

        lean_signal: how far from centre, normalised to [-1, +1]
        tilt_signal: deviation from neutral tilt, normalised to [-0.5, +0.5]
        """
        e = self._emitter
        center = e._width * 0.5
        quarter = e._width * 0.25
        lean_signal = (e._pbc - center) / quarter   # [-1 .. +1]
        tilt_signal = e._pbm - 0.5                  # [-0.5 .. +0.5]
        return lean_signal, tilt_signal

    # ── Projection ────────────────────────────────────────────────────────────

    def project(self, trigger: str = "project") -> "GradientField":
        """
        Project the emitter's current state onto all nodes and record a pattern.

        Each node is reset to neutral first, then the emitter signal scaled by
        the node's weight vector is applied.  This means each pattern reflects
        the emitter's *current* state, not accumulated history.

        Args:
            trigger: Label for the emitter snapshot taken at this projection.

        Returns:
            self  (for chaining)
        """
        lean_signal, tilt_signal = self._signals()

        for node in self._nodes:
            node.reset()
            lean_delta = node.lean_w * lean_signal * self._base_lean
            tilt_delta = node.tilt_w * tilt_signal * self._base_tilt
            node.apply(lean_delta, tilt_delta)

        self._history.append(FieldPattern(
            emitter_step     = self._emitter.step_count - 1,
            emitter_lean     = lean_signal,
            emitter_tilt     = tilt_signal,
            emitter_decision = self._emitter.get_decision(),
            tokens           = tuple(n.decision for n in self._nodes),
            a_vals           = tuple(round(n.a, 4) for n in self._nodes),
            b_vals           = tuple(round(n.b, 4) for n in self._nodes),
        ))
        return self

    # ── Emitter mutations (auto-project variants) ─────────────────────────────

    def drive(
        self,
        lean: float = 0,
        tilt: float = 0,
        direction: str = "b",
        trigger: str = "drive",
    ) -> "GradientField":
        """
        Mutate the emitter (lean and/or tilt) then project onto all nodes.

        Args:
            lean:      Amount to lean (>= 0).
            tilt:      Amount to tilt (>= 0).
            direction: 'a' or 'b' — lean direction; tilt polarity follows
                       ('b' → tilt toward BOTH, 'a' → tilt toward NONE).
            trigger:   Snapshot label (stored in both emitter history and field pattern).
        """
        self._emitter.observe(lean=lean, tilt=tilt, direction=direction, trigger=trigger)
        self.project(trigger)
        return self

    def drive_lean(self, amount: float = 1, direction: str = "b", trigger: Optional[str] = None) -> "GradientField":
        """Lean emitter then project."""
        t = trigger or f"lean_{direction}"
        self._emitter.observe(lean=amount, tilt=0, direction=direction, trigger=t)
        self.project(t)
        return self

    def drive_tilt(self, amount: float = 1, direction: str = "b", trigger: Optional[str] = None) -> "GradientField":
        """Tilt emitter then project."""
        t = trigger or f"tilt_{direction}"
        self._emitter.observe(lean=0, tilt=amount, direction=direction, trigger=t)
        self.project(t)
        return self

    # ── Analysis ──────────────────────────────────────────────────────────────

    @property
    def history(self) -> list:
        return list(self._history)

    def current_pattern(self) -> tuple:
        """Current decision tokens for all nodes."""
        return tuple(n.decision for n in self._nodes)

    def consensus(self) -> dict:
        """
        Count how many nodes hold each decision token right now.

        Returns:
            dict token → count, sorted by count descending.
        """
        counts: dict = {}
        for n in self._nodes:
            counts[n.decision] = counts.get(n.decision, 0) + 1
        return dict(sorted(counts.items(), key=lambda x: -x[1]))

    def find_pattern_shifts(self) -> list:
        """
        Return steps where the field pattern changed.

        Returns:
            List of dicts: {step, trigger, before, after, changed_nodes}
        """
        events = []
        h = list(self._history)
        for i in range(1, len(h)):
            prev, curr = h[i - 1], h[i]
            if prev.tokens != curr.tokens:
                changed = [
                    self._nodes[j].name
                    for j in range(len(self._nodes))
                    if prev.tokens[j] != curr.tokens[j]
                ]
                events.append({
                    "step":          curr.emitter_step,
                    "trigger":       f"step_{curr.emitter_step}",
                    "emitter_dec":   curr.emitter_decision,
                    "before":        prev.tokens,
                    "after":         curr.tokens,
                    "changed_nodes": changed,
                })
        return events

    def pattern_at(self, step: int) -> Optional[FieldPattern]:
        """
        Retrieve the FieldPattern recorded at a given emitter step.
        Returns None if not found (history may be capped).
        """
        for p in self._history:
            if p.emitter_step == step:
                return p
        return None

    def nodal_nodes(self, threshold: float = 0.05) -> list:
        """
        Return nodes whose response is near-zero across all recorded patterns.

        A node is "nodal" if max(|lean_w|, |tilt_w|) <= threshold.
        These are the fixed points of the pattern — where sand collects.
        """
        return [n for n in self._nodes if abs(n.lean_w) <= threshold and abs(n.tilt_w) <= threshold]

    # ── Display ───────────────────────────────────────────────────────────────

    _W = 80

    def print_pattern(self) -> None:
        """Print the current field pattern as a one-line token array."""
        pattern = self.current_pattern()
        names   = self.node_names
        print("  " + "  ".join(f"{n:<6}" for n in names))
        print("  " + "  ".join(f"{t:<6}" for t in pattern))

    def print_history_table(self, limit: Optional[int] = None) -> None:
        """
        Print a table: one row per projection.
        Columns: step | lean_sig | tilt_sig | emitter_dec | [node tokens...]
        """
        h = list(self._history)
        if limit is not None:
            h = h[-limit:]

        # Header
        node_w = 8
        header = (f"{'step':>4}  {'lean_sig':>8}  {'tilt_sig':>8}  "
                  f"{'emit_dec':<8}  " +
                  "  ".join(f"{n.name[:node_w]:<{node_w}}" for n in self._nodes))
        print(header)
        print("─" * len(header))
        for p in h:
            row = (f"{p.emitter_step:>4}  {p.emitter_lean:>+8.4f}  "
                   f"{p.emitter_tilt:>+8.4f}  "
                   f"{p.emitter_decision:<8}  " +
                   "  ".join(f"{t:<{node_w}}" for t in p.tokens))
            print(row)

    def print_pattern_shifts(self) -> None:
        """Print steps where the field pattern changed."""
        events = self.find_pattern_shifts()
        if not events:
            print("  (no pattern shifts)")
            return
        for e in events:
            before_s = "[" + ", ".join(e["before"]) + "]"
            after_s  = "[" + ", ".join(e["after"])  + "]"
            changed  = ", ".join(e["changed_nodes"])
            print(f"  step {e['step']:>3}  emitter={e['emitter_dec']:<8}  "
                  f"changed: {changed}")
            print(f"         {before_s}")
            print(f"       → {after_s}")

    def print_consensus(self) -> None:
        """Print current node consensus."""
        total = len(self._nodes)
        c = self.consensus()
        max_bar = 20
        for token, count in c.items():
            bar = "█" * int(max_bar * count / total)
            pct = 100 * count / total
            print(f"  {token:<8}  {bar:<{max_bar}}  {count}/{total} nodes  ({pct:.0f}%)")

    def print_weight_map(self) -> None:
        """Print the weight vector for each node."""
        print(f"  {'name':<18}  {'lean_w':>7}  {'tilt_w':>7}  {'role'}")
        print("  " + "─" * 60)
        for n in self._nodes:
            if abs(n.lean_w) < 0.05 and abs(n.tilt_w) < 0.05:
                role = "nodal (fixed point)"
            elif n.lean_w < -0.5 and n.tilt_w > 0.5:
                role = "lean-inverted, tilt-coupled"
            elif n.lean_w > 0.5 and n.tilt_w < -0.5:
                role = "lean-coupled, tilt-inverted"
            elif n.lean_w < -0.5:
                role = "lean-inverted"
            elif n.tilt_w < -0.5:
                role = "tilt-inverted"
            elif abs(n.lean_w) < 0.2:
                role = "tilt-dominant"
            elif abs(n.tilt_w) < 0.2:
                role = "lean-dominant"
            elif n.lean_w > 0 and n.tilt_w > 0:
                role = "direct"
            else:
                role = "mixed"
            print(f"  {n.name:<18}  {n.lean_w:>+7.2f}  {n.tilt_w:>+7.2f}  {role}")

    def __repr__(self) -> str:
        return (f"GradientField("
                f"nodes={len(self._nodes)}, "
                f"base_lean={self._base_lean}, "
                f"base_tilt={self._base_tilt}, "
                f"pattern={list(self.current_pattern())})")
