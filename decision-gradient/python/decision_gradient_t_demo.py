#!/usr/bin/env python3
"""
DecisionGradientT — Demo Scenarios
====================================
Four practical demonstrations of the time-axis extension.

  Demo 1: Sensor drift detection
          Gradual drift vs. sudden spike in a temperature sensor.
          Shows how find_triggers() pinpoints the anomaly step.

  Demo 2: Evidence trigger identification
          Incremental observations push the gradient from UNKNOWN to B.
          The time axis reveals *which observation* crossed the boundary.

  Demo 3: Stability analysis
          Competing evidence streams push the gradient back and forth.
          stability_report() shows how long each decision zone was held.

  Demo 4: Two-axis forensics
          Watch both pbc (lean) and pbm (tilt) axes independently.
          Demonstrates that a BOTH decision is driven by tilt, not lean.

Run:
    cd decision-gradient/python
    python decision_gradient_t_demo.py

Author: Randy Hoggard
Date: March 2026
"""

from decision_gradient_t import DecisionGradientT


# ── Formatting helpers ────────────────────────────────────────────────────────

def header(title: str) -> None:
    print()
    print("=" * 70)
    print(f"  {title}")
    print("=" * 70)


def subheader(title: str) -> None:
    print(f"\n{title}")
    print("-" * 70)


# ── Demo 1: Sensor drift detection ────────────────────────────────────────────

def demo_sensor_drift():
    header("DEMO 1: Sensor Drift Detection")
    print("""
Scenario: A temperature sensor feeds evidence into a decision gradient that
tracks whether the measured value is 'normal' (A) or 'elevated' (B).

Phase 1: Gradual evidence accumulates toward B over 8 readings.
Phase 2: A sudden +40-unit spike arrives — a plausible sensor fault or
         a genuine thermal event.

The time axis lets us pinpoint exactly when the spike occurred and how
much confidence shifted in a single step.
""")

    dg = DecisionGradientT(width=100)
    dg.snapshot_now("init")

    subheader("Phase 1: gradual evidence (8 readings, +5 lean each)")
    for i in range(1, 9):
        dg.observe(lean=5, tilt=2, trigger=f"reading_{i:02d}", direction="b")

    dg.print_history()
    print()
    dg.print_trajectory()

    subheader("Phase 2: sudden spike (single +40 lean reading)")
    dg.observe(lean=40, tilt=10, trigger="spike_reading", direction="b")

    print()
    dg.print_history(limit=5)  # show last 5 steps including the spike
    print()

    subheader("Trigger analysis (A-confidence drops ≥ 0.10)")
    dg.print_triggers(threshold=0.10, axis="a")
    print()

    subheader("Trigger analysis (B-confidence jumps ≥ 0.10)")
    dg.print_triggers(threshold=0.10, axis="b")
    print()

    subheader("Decision boundary crossings")
    dg.print_decision_changes()
    print()

    subheader("Peak B confidence")
    peak = dg.peak_confidence("b")
    print(f"  Highest B confidence: {peak['value']:.4f} at step {peak['step']} [{peak['trigger']}]  →  {peak['decision']}")

    subheader("Stability report")
    report = dg.stability_report()
    for decision, count in sorted(report.items(), key=lambda x: -x[1]):
        bar = "█" * count
        print(f"  {decision:<8} {bar}  ({count} steps)")


# ── Demo 2: Evidence trigger identification ───────────────────────────────────

def demo_trigger_identification():
    header("DEMO 2: Evidence Trigger Identification")
    print("""
Scenario: An AI model is collecting evidence to decide whether a document
is 'relevant' (B) or 'irrelevant' (A).  Evidence arrives as named signals
(keyword match, citation count, author reputation, etc.).

Question: which signal actually caused the decision to flip from UNKNOWN to B?

The time axis answers this directly via find_decision_changes().
""")

    dg = DecisionGradientT(width=100)
    dg.snapshot_now("init")

    evidence_signals = [
        ("keyword_match",       6,  1, "b"),
        ("short_abstract",      3,  5, "a"),   # weak counter-evidence
        ("citation_count_high", 8,  3, "b"),
        ("author_unknown",      4,  4, "a"),   # counter-evidence
        ("topic_relevance",     9,  2, "b"),
        ("publication_year_ok", 5,  1, "b"),
        ("full_text_keywords",  12, 4, "b"),   # strong signal — likely the trigger
        ("format_correct",      2,  1, "b"),
    ]

    subheader("Applying evidence signals")
    for name, lean, tilt, direction in evidence_signals:
        before = dg.get_decision()
        dg.observe(lean=lean, tilt=tilt, trigger=name, direction=direction)
        after = dg.get_decision()
        marker = "  ◄ BOUNDARY CROSSED" if before != after else ""
        print(f"  {name:<28}  a={dg.get_opt_a():.4f}  b={dg.get_opt_b():.4f}  → {after}{marker}")

    subheader("Full history")
    dg.print_history()

    subheader("Decision boundary crossings")
    dg.print_decision_changes()

    subheader("Trajectory")
    dg.print_trajectory()

    subheader("Top B-confidence triggers (threshold ≥ 0.08)")
    dg.print_triggers(threshold=0.08, axis="b")


# ── Demo 3: Stability analysis ────────────────────────────────────────────────

def demo_stability_analysis():
    header("DEMO 3: Stability Analysis — Competing Evidence Streams")
    print("""
Scenario: A negotiation-monitoring system receives alternating signals —
one party pushes toward outcome A, the other pushes toward outcome B.
The gradient oscillates.

stability_report() shows how many steps were spent in each zone, which
maps to: "how dominant was each party's argument over time?"
""")

    dg = DecisionGradientT(width=100)
    dg.snapshot_now("init")

    # Alternating pushes — party A is slightly stronger early, B recovers
    rounds = [
        ("party_A_arg_1",   10, 3, "a"),
        ("party_B_arg_1",    7, 2, "b"),
        ("party_A_arg_2",   12, 4, "a"),
        ("party_B_arg_2",   11, 3, "b"),
        ("party_A_arg_3",    6, 1, "a"),
        ("party_B_arg_3",   14, 5, "b"),   # B starts gaining ground
        ("party_A_arg_4",    8, 2, "a"),
        ("party_B_arg_4",   15, 6, "b"),   # B dominates
        ("party_B_arg_5",   10, 4, "b"),
        ("party_A_arg_5",    9, 2, "a"),
        ("party_B_arg_6",   12, 5, "b"),
        ("neutral_mediate",  1, 8, "b"),   # mediator widens the box → BOTH?
    ]

    subheader("Round-by-round play")
    for name, lean, tilt, direction in rounds:
        dg.observe(lean=lean, tilt=tilt, trigger=name, direction=direction)
        print(f"  {name:<24}  A={dg.get_opt_a():.3f}  B={dg.get_opt_b():.3f}  → {dg.get_decision()}")

    subheader("Decision trajectory")
    dg.print_trajectory()

    subheader("Stability report (steps per zone)")
    report = dg.stability_report()
    total = sum(report.values())
    for decision, count in sorted(report.items(), key=lambda x: -x[1]):
        pct = count / total * 100
        bar = "█" * count
        print(f"  {decision:<8} {bar:<20}  {count:>2} steps  ({pct:.0f}%)")

    subheader("Decision boundary crossings")
    dg.print_decision_changes()

    subheader("Final state")
    dg.print_state()
    dg.print_decision()


# ── Demo 4: Two-axis forensics ────────────────────────────────────────────────

def demo_two_axis_forensics():
    header("DEMO 4: Two-Axis Forensics — Lean vs Tilt")
    print("""
Scenario: A sensor system has two independent evidence channels:
  - lean_channel: directional evidence pushing toward A or B
  - tilt_channel: certainty evidence — "we might be seeing BOTH" or "neither"

By watching pbc and pbm separately, we can distinguish:
  "The decision changed because we got directional evidence" (lean jump)
  vs.
  "The decision changed because certainty shifted" (tilt jump)

This is the key to understanding how BOTH and NONE states emerge —
they are driven by the tilt axis, not the lean axis.
""")

    dg = DecisionGradientT(width=100)
    dg.snapshot_now("init")

    subheader("Phase 1: Pure lean — drive toward B, no tilt change")
    for i in range(4):
        dg.lean_opt_b(8)

    dg.print_history()
    print()
    print(f"  Decision: {dg.get_decision()}")
    print(f"  (pbm={dg.get_nb():.3f} — tilt unchanged at 0.5)")

    subheader("Phase 2: Pure tilt toward BOTH — lean stays fixed")
    for i in range(6):
        dg.tilt_both(8)

    dg.print_history(limit=7)
    print()
    print(f"  Decision: {dg.get_decision()}")
    print(f"  (pbc stayed at B-side, but pbm rose to {dg.get_nb():.3f})")

    subheader("Phase 3: Pull back with tilt_none — watch BOTH collapse")
    for i in range(4):
        dg.tilt_none(8)

    dg.print_history(limit=5)
    print()
    print(f"  Decision: {dg.get_decision()}")

    subheader("Full trajectory")
    dg.print_trajectory()

    subheader("Lean-axis triggers (pbc, threshold ≥ 5.0)")
    dg.print_triggers(threshold=5.0, axis="pbc")

    subheader("Tilt-axis triggers (pbm, threshold ≥ 0.08)")
    dg.print_triggers(threshold=0.08, axis="pbm")

    subheader("Decision boundary crossings")
    dg.print_decision_changes()

    print()
    print("""Key insight:
  All lean triggers occurred in Phase 1.
  All tilt triggers occurred in Phases 2 and 3.
  The BOTH state was born entirely from tilt, not from lean —
  the gradient was already pointing at B, but certainty had to
  rise above a threshold before BOTH appeared.
  This is the axis that entanglement and field constructs will
  propagate: two variables sharing a tilt axis would both shift
  toward BOTH (or NONE) together, even if their lean axes differ.
""")


# ── Entry point ───────────────────────────────────────────────────────────────

if __name__ == "__main__":
    print("DecisionGradientT — Time Axis Demonstrations")
    print("=" * 70)
    print("Extending DecisionGradient with a snapshot history.")
    print("Demos: sensor drift | trigger ID | stability | two-axis forensics")

    demo_sensor_drift()
    demo_trigger_identification()
    demo_stability_analysis()
    demo_two_axis_forensics()

    print()
    print("=" * 70)
    print("  All demos complete.")
    print("=" * 70)
