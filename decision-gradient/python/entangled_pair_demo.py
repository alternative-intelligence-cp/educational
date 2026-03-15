#!/usr/bin/env python3
"""
EntangledPair Demonstrations
==============================
Four scenarios illustrating the tilt-coupling mechanic.

  Demo 1: Propagated uncertainty
          Alpha gets a large tilt spike.  Beta has no direct observations —
          yet its decision crosses a boundary due to propagation alone.

  Demo 2: Lean independence + shared uncertainty
          Alpha leans toward A; beta leans toward B.  A tilt spike via alpha
          drives both into BOTH — confirming lean axes are fully decoupled
          while tilt propagates.

  Demo 3: Coupling strength sweep
          Same spike event replayed at coupling = 0.0 / 0.25 / 0.5 / 0.75 / 1.0.
          Shows how beta's final pbm scales linearly with coupling, and at
          what coupling level a boundary crossing first appears in beta.

  Demo 4: Anti-entanglement (coupling = -0.8)
          When alpha becomes uncertain (tilts toward BOTH), beta narrows
          toward certainty (tilts toward NONE).  Models two complementary
          sensors whose uncertainty is inversely correlated.

Author: Randy Hoggard
Date: March 2026
"""

import sys
import os
sys.path.insert(0, os.path.dirname(__file__))

from entangled_pair import EntangledPair


# ── Formatting helpers ────────────────────────────────────────────────────────

W = 70

def banner(title: str) -> None:
    print()
    print("=" * W)
    print(f"  {title}")
    print("=" * W)

def section(title: str) -> None:
    print()
    print(title)
    print("-" * W)


# ── Demo 1: Propagated uncertainty ────────────────────────────────────────────

def demo1():
    banner("DEMO 1: Propagated Uncertainty")
    print()
    print("Scenario: Two gas sensors (alpha, beta) monitor adjacent zones.")
    print("Only alpha receives direct evidence of a gas concentration rise.")
    print("Beta has NO direct observations — passive the whole time.")
    print("coupling = 1.0 (perfect tilt mirror), crystal_threshold = 0.80")
    print()
    print("Question: does beta's decision change due to propagation alone?")

    pair = EntangledPair(width=100, coupling=1.0, crystal_threshold=0.80)
    pair.snapshot_now("init")

    # Alpha gets 5 gradual lean+tilt readings (lean toward B, tilt toward BOTH)
    section("Phase 1: gradual evidence on alpha only")
    for i in range(1, 6):
        pair.observe_alpha(lean=5, tilt=4, trigger=f"alpha_reading_{i:02d}", direction="b")

    pair.print_history()

    section("Alpha trajectory")
    print("α:", " → ".join(pair.alpha_trajectory()))

    section("Beta trajectory (no direct observations)")
    print("β:", " → ".join(pair.beta_trajectory()))

    section("Entanglement-driven boundary crossings")
    pair.print_entanglement_events()

    section("Crystallization events (threshold = 0.80)")
    pair.print_crystallization_events()

    section("Joint sync report")
    pair.print_sync_report()


# ── Demo 2: Lean independence + shared uncertainty ────────────────────────────

def demo2():
    banner("DEMO 2: Lean Independence + Shared Uncertainty")
    print()
    print("Scenario: Two analysts receive opposite directional evidence.")
    print("  alpha: evidence consistently points toward outcome A.")
    print("  beta:  evidence consistently points toward outcome B.")
    print("Then: a shared environmental uncertainty event (tilt spike via alpha)")
    print("drives both into BOTH, despite their lean axes pointing opposite ways.")
    print()
    print("This demonstrates the axis separation — lean is private, tilt is shared.")

    pair = EntangledPair(width=100, coupling=1.0, crystal_threshold=0.85)
    pair.snapshot_now("init")

    # Build opposite leans — pure lean, no tilt
    section("Phase 1: building opposite leans (no tilt)")
    for i in range(1, 5):
        pair.lean_alpha(amount=8, direction="a", trigger=f"alpha_lean_{i}")
        pair.lean_beta(amount=8, direction="b", trigger=f"beta_lean_{i}")

    # Show state before the tilt event
    print(f"\n  After lean phase:")
    print(f"  α: pbc={pair.alpha._pbc:.1f}  pbm={pair.alpha._pbm:.3f}  A={pair.alpha.get_opt_a():.4f}  dec={pair.alpha.get_decision()}")
    print(f"  β: pbc={pair.beta._pbc:.1f}   pbm={pair.beta._pbm:.3f}  B={pair.beta.get_opt_b():.4f}  dec={pair.beta.get_decision()}")

    # Tilt spike via alpha — both should drift toward BOTH
    section("Phase 2: tilt spike via alpha (lean axes unchanged)")
    for i in range(1, 6):
        pair.tilt_alpha(amount=8, toward="both", trigger=f"uncertainty_wave_{i}")

    section("Full history")
    pair.print_history()

    section("Joint trajectories")
    pair.print_joint_trajectory()

    section("Key insight")
    final_a = pair.alpha._pbc
    final_b = pair.beta._pbc
    print(f"  Alpha still leans A:  pbc={final_a:.1f} (< 50 = A side)")
    print(f"  Beta  still leans B:  pbc={final_b:.1f} (> 50 = B side)")
    print(f"  Both decision tokens: α={pair.alpha.get_decision()}, β={pair.beta.get_decision()}")
    print(f"  Lean axes are private. Tilt is shared. BOTH emerged from tilt, not lean.")

    section("Entanglement-driven boundary crossings")
    pair.print_entanglement_events()


# ── Demo 3: Coupling strength sweep ───────────────────────────────────────────

def demo3():
    banner("DEMO 3: Coupling Strength Sweep")
    print()
    print("Scenario: Same spike event (5 × tilt_alpha 10 toward BOTH) replayed")
    print("across five EntangledPair instances with different coupling values.")
    print()
    print("Shows how beta's final pbm scales with coupling, and at what coupling")
    print("level beta crosses a decision boundary.")

    couplings = [0.0, 0.25, 0.50, 0.75, 1.0]

    print()
    print(f"  {'coupling':>8}  {'α_pbm_final':>12}  {'β_pbm_final':>12}  "
          f"{'α_decision':<10}  {'β_decision':<10}  {'entangled_crossings':>20}")
    print("  " + "-" * 80)

    for c in couplings:
        pair = EntangledPair(width=100, coupling=c, crystal_threshold=0.85)
        pair.snapshot_now("init")
        for i in range(1, 6):
            pair.tilt_alpha(amount=10, toward="both", trigger=f"spike_{i}")

        events = pair.entanglement_events()
        print(
            f"  {c:>8.2f}  "
            f"{pair.alpha._pbm:>12.4f}  "
            f"{pair.beta._pbm:>12.4f}  "
            f"{pair.alpha.get_decision():<10}  "
            f"{pair.beta.get_decision():<10}  "
            f"{len(events):>20}"
        )

    print()
    print("  Observation:")
    print("  - Alpha's pbm is always the same — it does not depend on coupling.")
    print("  - Beta's pbm scales with coupling: beta_pbm ≈ 0.5 + coupling × total_tilt")
    print("    where total_tilt = Σ(amount/pbw) = 5×(10/50) = 1.0 in this scenario.")
    print("  - Propagation fires even when alpha is saturated at pbm=1.0 — the")
    print("    'stress' continues to transmit regardless of clamping.")
    print("  - No decision crossings occur here because both members stay at pbc=50")
    print("    (A=B symmetry) — tilt alone cannot break that.  See Demo 2 for")
    print("    entanglement-driven crossings: beta leaned toward B first, so the")
    print("    propagated tilt had a boundary to cross.")

    # Detailed view at coupling=1.0 and coupling=0.5 for comparison
    for c in [1.0, 0.5]:
        pair = EntangledPair(width=100, coupling=c, crystal_threshold=0.85)
        pair.snapshot_now("init")
        for i in range(1, 6):
            pair.tilt_alpha(amount=10, toward="both", trigger=f"spike_{i}")

        section(f"Detailed table — coupling={c:.2f}")
        pair.print_history()
        pair.print_entanglement_events()


# ── Demo 4: Anti-entanglement ─────────────────────────────────────────────────

def demo4():
    banner("DEMO 4: Anti-Entanglement (coupling = -0.8)")
    print()
    print("Scenario: Two complementary diagnostic systems.")
    print("  alpha: a 'noise detector' — tilts toward BOTH when noise is high.")
    print("  beta:  a 'signal classifier' — should narrow toward certainty when")
    print("         noise increases (more noise = we know what we have: a noisy signal).")
    print()
    print("coupling = -0.8: alpha tilts toward BOTH → beta tilts toward NONE.")
    print()
    print("This models anti-correlated uncertainty — two sensors whose uncertainty")
    print("states are inversely related.")

    pair = EntangledPair(width=100, coupling=-0.8, crystal_threshold=0.80)
    pair.snapshot_now("init")

    # Give beta a slight lean toward B to start
    pair.lean_beta(amount=10, direction="b", trigger="beta_init_lean")

    section("Phase 1: alpha noise events (tilt toward BOTH)")
    for i in range(1, 7):
        pair.tilt_alpha(amount=8, toward="both", trigger=f"noise_event_{i}")

    section("Full history")
    pair.print_history()

    section("Trajectories")
    pair.print_joint_trajectory()

    section("Tilt values over time")
    print(f"  {'step':>4}  {'trigger':<22}  {'α_pbm':>7}  {'β_pbm':>7}  {'tilt_corr':>9}")
    print("  " + "-" * 56)
    prev_alpha_pbm = 0.5
    prev_beta_pbm  = 0.5
    for s in pair.history:
        corr = 1.0 - abs(s.alpha_pbm - s.beta_pbm)
        direction = ""
        if s.alpha_pbm > prev_alpha_pbm and s.beta_pbm < prev_beta_pbm:
            direction = " ← anti"
        prev_alpha_pbm = s.alpha_pbm
        prev_beta_pbm  = s.beta_pbm
        print(f"  {s.step:>4}  {s.trigger:<22}  {s.alpha_pbm:>7.4f}  {s.beta_pbm:>7.4f}  {corr:>9.4f}{direction}")

    section("Joint sync report")
    pair.print_sync_report()

    section("Key insight")
    print(f"  Alpha final pbm: {pair.alpha._pbm:.4f}  → {pair.alpha.get_decision()}")
    print(f"  Beta  final pbm: {pair.beta._pbm:.4f}  → {pair.beta.get_decision()}")
    print()
    print("  As alpha's uncertainty rose (pbm → 1.0, BOTH/UNKNOWN),")
    print("  beta's uncertainty fell (pbm → 0.0, NONE).")
    print("  The coupling axis transmitted the inverse of every alpha tilt event.")
    print("  Their lean axes diverged as well (alpha center, beta toward B) —")
    print("  but that was from the explicit lean_beta init, not from coupling.")
    print()
    print("  Use case: a noise classifier and a signal extractor operating on")
    print("  the same channel.  When the noise classifier becomes maximally")
    print("  uncertain, the signal extractor converges — because maximal noise")
    print("  is itself a definitive classification.")


# ── Entry point ───────────────────────────────────────────────────────────────

if __name__ == "__main__":
    print("EntangledPair — Tilt Coupling Demonstrations")
    print("=" * W)
    print("Two DecisionGradients whose tilt (uncertainty) axes are coupled.")
    print("Lean axes are always independent — only pbm propagates.")
    print("Demos: propagation | lean independence | coupling sweep | anti-entanglement")

    demo1()
    demo2()
    demo3()
    demo4()

    print()
    print("=" * W)
    print("  All demos complete.")
    print("=" * W)
