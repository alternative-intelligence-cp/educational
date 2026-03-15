#!/usr/bin/env python3
"""
EntangledPair — Two Coupled DecisionGradients
===============================================
Two DecisionGradient instances (alpha and beta) where the tilt axis (pbm)
is coupled.  Lean axes are fully independent — only uncertainty (tilt)
propagates between members.

Coupling mechanics:
    When a tilt mutation is applied to one member, a fraction of that mutation
    (scaled by |coupling|) is applied to the other member automatically.

    coupling =  1.0 → mirror: alpha tilts toward BOTH → beta tilts toward BOTH
    coupling =  0.0 → independent: no tilt propagation at all
    coupling = -1.0 → anti-entangled: alpha tilts toward BOTH → beta tilts toward NONE

    Lean mutations never propagate — each member's directional evidence is
    always fully independent.

Crystallization:
    When both members' pbm values reach or exceed crystal_threshold, the pair
    is considered "crystallized" — both are in a state of high joint uncertainty.
    This is tracked as a flag on each PairSnapshot (rising-edge events are
    accessible via crystallization_events()).

Key insight (from Demo 4 of DecisionGradientT):
    The tilt axis is the *coupling axis*.  Two sensors that share physical
    uncertainty (e.g. both exposed to the same noisy environment) can have
    completely different directional evidence (lean) but still rise and fall
    in joint uncertainty together.  EntangledPair makes this explicit.

PairSnapshot structure (namedtuple):
    step              int    — mutation counter
    trigger           str    — label for this event
    source            str    — 'alpha', 'beta', or 'pair'
    alpha_pbc         float  — alpha lean axis position
    alpha_pbm         float  — alpha tilt axis value
    alpha_a           float  — alpha A confidence [0,1]
    alpha_b           float  — alpha B confidence [0,1]
    alpha_decision    str    — alpha output token
    beta_pbc          float  — beta lean axis position
    beta_pbm          float  — beta tilt axis value
    beta_a            float  — beta A confidence [0,1]
    beta_b            float  — beta B confidence [0,1]
    beta_decision     str    — beta output token
    propagated        bool   — did tilt propagate in this step?
    crystallized      bool   — are both pbm >= crystal_threshold?

Author: Randy Hoggard
Date: March 2026
"""

from collections import namedtuple, deque
from typing import Optional

from decision_gradient import DecisionGradient


# ── PairSnapshot ──────────────────────────────────────────────────────────────

PairSnapshot = namedtuple(
    "PairSnapshot",
    [
        "step", "trigger", "source",
        "alpha_pbc", "alpha_pbm", "alpha_a", "alpha_b", "alpha_decision",
        "beta_pbc",  "beta_pbm",  "beta_a",  "beta_b",  "beta_decision",
        "propagated", "crystallized",
    ]
)


# ── EntangledPair ─────────────────────────────────────────────────────────────

class EntangledPair:
    """
    Two DecisionGradient instances with a coupled tilt axis.

    Args:
        width              (float):       Total span for both members. Default 100.
        coupling           (float):       Tilt coupling coefficient [-1.0 .. 1.0].
                                          Default 1.0 (perfect mirror).
        crystal_threshold  (float):       pbm level at which a member is considered
                                          crystallized.  Default 0.8.
                                          Crystallization flag fires when BOTH cross it.
        history_depth      (int|None):    Max snapshots to retain.  None = unlimited.
    """

    def __init__(
        self,
        width: float = 100,
        coupling: float = 1.0,
        crystal_threshold: float = 0.8,
        history_depth: Optional[int] = None,
    ):
        if not isinstance(width, (int, float)) or width <= 1:
            raise ValueError(f"width must be > 1, got {width!r}")
        if not isinstance(coupling, (int, float)) or not (-1.0 <= coupling <= 1.0):
            raise ValueError(f"coupling must be in [-1.0, 1.0], got {coupling!r}")
        if not isinstance(crystal_threshold, (int, float)) or not (0.0 < crystal_threshold <= 1.0):
            raise ValueError(f"crystal_threshold must be in (0, 1], got {crystal_threshold!r}")

        self._alpha = DecisionGradient(width)
        self._beta  = DecisionGradient(width)
        self._coupling = float(coupling)
        self._crystal_threshold = float(crystal_threshold)
        self._history: deque = deque(maxlen=history_depth)
        self._step: int = 0

    # ── Properties ────────────────────────────────────────────────────────────

    @property
    def alpha(self) -> DecisionGradient:
        """The first member of the entangled pair."""
        return self._alpha

    @property
    def beta(self) -> DecisionGradient:
        """The second member of the entangled pair."""
        return self._beta

    @property
    def coupling(self) -> float:
        return self._coupling

    @property
    def crystal_threshold(self) -> float:
        return self._crystal_threshold

    # ── Snapshot machinery ────────────────────────────────────────────────────

    def _both_crystallized(self) -> bool:
        return (
            self._alpha._pbm >= self._crystal_threshold and
            self._beta._pbm  >= self._crystal_threshold
        )

    def _snap(self, trigger: str, source: str, propagated: bool) -> None:
        self._history.append(PairSnapshot(
            step              = self._step,
            trigger           = trigger,
            source            = source,
            alpha_pbc         = self._alpha._pbc,
            alpha_pbm         = self._alpha._pbm,
            alpha_a           = self._alpha.get_opt_a(),
            alpha_b           = self._alpha.get_opt_b(),
            alpha_decision    = self._alpha.get_decision(),
            beta_pbc          = self._beta._pbc,
            beta_pbm          = self._beta._pbm,
            beta_a            = self._beta.get_opt_a(),
            beta_b            = self._beta.get_opt_b(),
            beta_decision     = self._beta.get_decision(),
            propagated        = propagated,
            crystallized      = self._both_crystallized(),
        ))
        self._step += 1

    # ── Private helpers ───────────────────────────────────────────────────────

    @staticmethod
    def _apply_tilt(target: DecisionGradient, amount: float, toward: str) -> None:
        """Apply a tilt amount to a DecisionGradient in the given direction."""
        if toward == "both":
            target.tilt_both(amount)
        elif toward == "none":
            target.tilt_none(amount)
        else:
            target.tilt_unkn(amount)

    def _coupled_toward(self, toward: str) -> str:
        """Return the coupled direction — inverted for negative coupling."""
        if self._coupling < 0:
            return {"both": "none", "none": "both", "unkn": "unkn"}.get(toward, toward)
        return toward

    # ── Lean mutations (no coupling) ──────────────────────────────────────────

    def lean_alpha(
        self,
        amount: float = 1,
        direction: str = "b",
        trigger: Optional[str] = None,
    ) -> "EntangledPair":
        """
        Move alpha's lean axis.  Beta is unaffected.

        Args:
            amount:    Lean amount (> 0).
            direction: 'a' | 'b' | 'unkn'
            trigger:   Snapshot label.
        """
        t = trigger or f"lean_alpha_{direction}"
        if direction == "a":
            self._alpha.lean_opt_a(amount)
        elif direction == "unkn":
            self._alpha.lean_unkn(amount)
        else:
            self._alpha.lean_opt_b(amount)
        self._snap(t, "alpha", False)
        return self

    def lean_beta(
        self,
        amount: float = 1,
        direction: str = "b",
        trigger: Optional[str] = None,
    ) -> "EntangledPair":
        """
        Move beta's lean axis.  Alpha is unaffected.

        Args:
            amount:    Lean amount (> 0).
            direction: 'a' | 'b' | 'unkn'
            trigger:   Snapshot label.
        """
        t = trigger or f"lean_beta_{direction}"
        if direction == "a":
            self._beta.lean_opt_a(amount)
        elif direction == "unkn":
            self._beta.lean_unkn(amount)
        else:
            self._beta.lean_opt_b(amount)
        self._snap(t, "beta", False)
        return self

    # ── Tilt mutations (coupling propagates) ──────────────────────────────────

    def tilt_alpha(
        self,
        amount: float = 1,
        toward: str = "both",
        trigger: Optional[str] = None,
    ) -> "EntangledPair":
        """
        Tilt alpha; propagate |coupling| × amount to beta (in coupled direction).

        Args:
            amount:  Tilt amount (> 0).
            toward:  'both' | 'none' | 'unkn'
            trigger: Snapshot label.
        """
        t = trigger or f"tilt_alpha_{toward}"
        self._apply_tilt(self._alpha, amount, toward)
        prop = abs(self._coupling) * amount
        if prop > 0:
            self._apply_tilt(self._beta, prop, self._coupled_toward(toward))
        self._snap(t, "alpha", True)
        return self

    def tilt_beta(
        self,
        amount: float = 1,
        toward: str = "both",
        trigger: Optional[str] = None,
    ) -> "EntangledPair":
        """
        Tilt beta; propagate |coupling| × amount to alpha (in coupled direction).

        Args:
            amount:  Tilt amount (> 0).
            toward:  'both' | 'none' | 'unkn'
            trigger: Snapshot label.
        """
        t = trigger or f"tilt_beta_{toward}"
        self._apply_tilt(self._beta, amount, toward)
        prop = abs(self._coupling) * amount
        if prop > 0:
            self._apply_tilt(self._alpha, prop, self._coupled_toward(toward))
        self._snap(t, "beta", True)
        return self

    def tilt_pair(
        self,
        amount: float = 1,
        toward: str = "both",
        trigger: Optional[str] = None,
    ) -> "EntangledPair":
        """
        Tilt both members with the full amount simultaneously (shared field event).
        No asymmetric propagation — both receive exactly `amount`.

        Args:
            amount:  Tilt amount (> 0).
            toward:  'both' | 'none' | 'unkn'
            trigger: Snapshot label.
        """
        t = trigger or f"tilt_pair_{toward}"
        self._apply_tilt(self._alpha, amount, toward)
        self._apply_tilt(self._beta, amount, toward)
        self._snap(t, "pair", False)
        return self

    # ── Compound observe ──────────────────────────────────────────────────────

    def observe_alpha(
        self,
        lean: float = 0,
        tilt: float = 0,
        trigger: str = "observe_alpha",
        direction: str = "b",
    ) -> "EntangledPair":
        """
        Apply lean+tilt evidence via alpha (lean is independent; tilt propagates).
        Single snapshot per call regardless of how many axes are touched.

        Args:
            lean:      Lean amount (>= 0).
            tilt:      Tilt amount (>= 0).
            trigger:   Snapshot label.
            direction: 'a' or 'b' — controls lean direction and tilt polarity.
                       'b' → lean toward B, tilt toward BOTH
                       'a' → lean toward A, tilt toward NONE
        """
        if lean < 0 or tilt < 0:
            raise ValueError("lean and tilt must be >= 0")
        toward = "both" if direction == "b" else "none"
        if lean > 0:
            if direction == "a":
                self._alpha.lean_opt_a(lean)
            else:
                self._alpha.lean_opt_b(lean)
        if tilt > 0:
            self._apply_tilt(self._alpha, tilt, toward)
            prop = abs(self._coupling) * tilt
            if prop > 0:
                self._apply_tilt(self._beta, prop, self._coupled_toward(toward))
        self._snap(trigger, "alpha", tilt > 0)
        return self

    def observe_beta(
        self,
        lean: float = 0,
        tilt: float = 0,
        trigger: str = "observe_beta",
        direction: str = "b",
    ) -> "EntangledPair":
        """
        Apply lean+tilt evidence via beta (lean is independent; tilt propagates).
        Single snapshot per call regardless of how many axes are touched.

        Args:
            lean:      Lean amount (>= 0).
            tilt:      Tilt amount (>= 0).
            trigger:   Snapshot label.
            direction: 'a' or 'b' — controls lean direction and tilt polarity.
        """
        if lean < 0 or tilt < 0:
            raise ValueError("lean and tilt must be >= 0")
        toward = "both" if direction == "b" else "none"
        if lean > 0:
            if direction == "a":
                self._beta.lean_opt_a(lean)
            else:
                self._beta.lean_opt_b(lean)
        if tilt > 0:
            self._apply_tilt(self._beta, tilt, toward)
            prop = abs(self._coupling) * tilt
            if prop > 0:
                self._apply_tilt(self._alpha, prop, self._coupled_toward(toward))
        self._snap(trigger, "beta", tilt > 0)
        return self

    def snapshot_now(self, trigger: str = "manual") -> "EntangledPair":
        """Record the current joint state without any mutation."""
        self._snap(trigger, "manual", False)
        return self

    # ── Analysis ──────────────────────────────────────────────────────────────

    @property
    def history(self) -> list:
        return list(self._history)

    def is_crystallized(self) -> bool:
        """True when both members' pbm >= crystal_threshold."""
        return self._both_crystallized()

    def tilt_delta(self) -> float:
        """Absolute difference between alpha and beta pbm values right now."""
        return abs(self._alpha._pbm - self._beta._pbm)

    def tilt_correlation(self) -> float:
        """
        How aligned are the tilt axes? 1.0 = identical pbm, 0.0 = maximally diverged.
        """
        return max(0.0, 1.0 - self.tilt_delta())

    def alpha_trajectory(self) -> list:
        """List of alpha decision tokens over time."""
        return [s.alpha_decision for s in self._history]

    def beta_trajectory(self) -> list:
        """List of beta decision tokens over time."""
        return [s.beta_decision for s in self._history]

    def joint_trajectory(self) -> list:
        """List of (alpha_decision, beta_decision) tuples over time."""
        return [(s.alpha_decision, s.beta_decision) for s in self._history]

    def sync_report(self) -> dict:
        """
        Count steps spent in each joint decision combination.

        Returns:
            dict mapping (alpha_decision, beta_decision) → step count,
            sorted by count descending.
        """
        result: dict = {}
        for s in self._history:
            key = (s.alpha_decision, s.beta_decision)
            result[key] = result.get(key, 0) + 1
        return dict(sorted(result.items(), key=lambda x: -x[1]))

    def crystallization_events(self) -> list:
        """
        Return snapshots where crystallization became True (rising edge only).
        """
        events = []
        prev_crystal = False
        for s in self._history:
            if s.crystallized and not prev_crystal:
                events.append(s)
            prev_crystal = s.crystallized
        return events

    def entanglement_events(self) -> list:
        """
        Return steps where tilt propagation caused a decision boundary crossing
        in the receiving member.

        If source='alpha', we check whether beta crossed a decision boundary.
        If source='beta', we check whether alpha crossed a decision boundary.

        Returns:
            List of dicts: {step, trigger, source, affected, before, after}
        """
        events = []
        h = list(self._history)
        for i in range(1, len(h)):
            prev, curr = h[i - 1], h[i]
            if not curr.propagated:
                continue
            if curr.source == "alpha":
                if prev.beta_decision != curr.beta_decision:
                    events.append({
                        "step":     curr.step,
                        "trigger":  curr.trigger,
                        "source":   "alpha",
                        "affected": "beta",
                        "before":   prev.beta_decision,
                        "after":    curr.beta_decision,
                    })
            elif curr.source == "beta":
                if prev.alpha_decision != curr.alpha_decision:
                    events.append({
                        "step":     curr.step,
                        "trigger":  curr.trigger,
                        "source":   "beta",
                        "affected": "alpha",
                        "before":   prev.alpha_decision,
                        "after":    curr.alpha_decision,
                    })
        return events

    # ── Display ───────────────────────────────────────────────────────────────

    _HLINE   = "─" * 116
    _DIVIDER = "═" * 116

    def print_history(self, limit: Optional[int] = None) -> None:
        """
        Print a joint state table: one row per snapshot, both members side by side.

        Columns: step | trigger | α_pbc | α_pbm | α_A | α_B | α_dec | β_pbc | β_pbm | β_A | β_B | β_dec | flags
        Flags: P = propagated tilt, X = crystallized
        """
        h = list(self._history)
        if limit is not None:
            h = h[-limit:]

        print(f"{'step':>4}  {'trigger':<22}  "
              f"{'α_pbc':>6} {'α_pbm':>5}  {'α_A':>6} {'α_B':>6} {'α_dec':<7}  "
              f"{'β_pbc':>6} {'β_pbm':>5}  {'β_A':>6} {'β_B':>6} {'β_dec':<7}  "
              f"{'flags':<5}")
        print(self._HLINE)
        for s in h:
            flags = ("P" if s.propagated else "·") + ("X" if s.crystallized else "·")
            print(
                f"{s.step:>4}  {s.trigger:<22}  "
                f"{s.alpha_pbc:>6.2f} {s.alpha_pbm:>5.3f}  "
                f"{s.alpha_a:>6.4f} {s.alpha_b:>6.4f} {s.alpha_decision:<7}  "
                f"{s.beta_pbc:>6.2f} {s.beta_pbm:>5.3f}  "
                f"{s.beta_a:>6.4f} {s.beta_b:>6.4f} {s.beta_decision:<7}  "
                f"{flags}"
            )

    def print_joint_trajectory(self) -> None:
        """Print alpha and beta decision trajectories side by side."""
        at = self.alpha_trajectory()
        bt = self.beta_trajectory()
        print(f"α: {' → '.join(at)}")
        print(f"β: {' → '.join(bt)}")

    def print_sync_report(self) -> None:
        """Print step counts for each joint decision combination."""
        total = len(self._history)
        if total == 0:
            print("(no history)")
            return
        report = self.sync_report()
        max_bar = 20
        for (ad, bd), count in report.items():
            bar = "█" * int(max_bar * count / total)
            pct = 100 * count / total
            print(f"  ({ad:<7}, {bd:<7})  {bar:<{max_bar}}  {count:>3} steps  ({pct:.0f}%)")

    def print_entanglement_events(self) -> None:
        """Print steps where propagated tilt caused a boundary crossing in the receiver."""
        events = self.entanglement_events()
        if not events:
            print("  (no entanglement-driven boundary crossings)")
            return
        for e in events:
            print(
                f"  step {e['step']:>3}  [{e['trigger']:<22}]  "
                f"{e['source']} → {e['affected']}:  "
                f"{e['before']} → {e['after']}"
            )

    def print_crystallization_events(self) -> None:
        """Print steps where both members first crossed crystal_threshold."""
        events = self.crystallization_events()
        if not events:
            print(f"  (no crystallization events — threshold {self._crystal_threshold:.2f} not reached by both)")
            return
        for s in events:
            print(
                f"  step {s.step:>3}  [{s.trigger:<22}]  "
                f"α_pbm={s.alpha_pbm:.3f}  β_pbm={s.beta_pbm:.3f}  "
                f"α={s.alpha_decision}  β={s.beta_decision}"
            )

    def __repr__(self) -> str:
        return (
            f"EntangledPair("
            f"coupling={self._coupling:.2f}, "
            f"crystal={self._crystal_threshold:.2f}, "
            f"α={self._alpha.get_decision()}, "
            f"β={self._beta.get_decision()}, "
            f"steps={self._step})"
        )
