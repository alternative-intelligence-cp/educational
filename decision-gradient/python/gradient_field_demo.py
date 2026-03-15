#!/usr/bin/env python3
"""
GradientField Demonstrations
==============================
Four scenarios showing how a single emitter Q variable produces
spatial decision patterns across a field of nodes.

  Demo 1: Chladni sweep
          A 9-node "plate" with symmetric weights [-1, -0.7, -0.3, -0.1, 0,
          0.1, 0.3, 0.7, 1].  The emitter sweeps from maxA to maxB.
          Watch the decision pattern shift across the plate — the nodal
          centre (weight=0) never moves; the outer nodes invert.

  Demo 2: Sensor network with topology
          7 nodes representing a physical sensor network: one upwind node
          (strong direct coupling), two midfield nodes (medium), two
          crossfield nodes (tilt-only, no lean coupling), and two downwind
          nodes (inverted — far side of an obstacle).
          A pollutant plume event drives the emitter; the pattern shows how
          the signal propagates across the "field geometry."

  Demo 3: Pattern crystallisation
          An emitter starts neutral and is gradually driven toward BOTH
          via tilt.  Shows how each node's pattern evolves as uncertainty
          rises — tilt-sensitive nodes crystallise first; lean-dominant
          nodes barely respond.

  Demo 4: Resonance scanning
          The emitter cycles through 5 distinct states in a loop (like
          sweeping frequency).  pattern_history shows which emitter states
          produce the most complex field patterns (most tokens in play).

Author: Randy Hoggard
Date: March 2026
"""

import sys
import os
sys.path.insert(0, os.path.dirname(__file__))

from gradient_field import GradientField, FieldNode


# ── Formatting helpers ────────────────────────────────────────────────────────

W = 72

def banner(title: str) -> None:
    print()
    print("=" * W)
    print(f"  {title}")
    print("=" * W)

def section(title: str) -> None:
    print()
    print(title)
    print("-" * W)


# ── Demo 1: Chladni sweep ─────────────────────────────────────────────────────

def demo1():
    banner("DEMO 1: Chladni Sweep — One Emitter, Symmetric Plate")
    print()
    print("A 9-node 'plate' with lean weights symmetric about zero.")
    print("The emitter sweeps from maxA lean to maxB lean in equal steps.")
    print("Tilt is held constant (neutral).")
    print()
    print("The central node (weight=0) is a nodal point — it never moves.")
    print("Outer nodes invert when the emitter crosses centre.")

    nodes = [
        FieldNode("n1", lean_w=-1.00, tilt_w=0.5),
        FieldNode("n2", lean_w=-0.70, tilt_w=0.5),
        FieldNode("n3", lean_w=-0.30, tilt_w=0.5),
        FieldNode("n4", lean_w=-0.10, tilt_w=0.5),
        FieldNode("n5", lean_w= 0.00, tilt_w=0.5),   # nodal
        FieldNode("n6", lean_w= 0.10, tilt_w=0.5),
        FieldNode("n7", lean_w= 0.30, tilt_w=0.5),
        FieldNode("n8", lean_w= 0.70, tilt_w=0.5),
        FieldNode("n9", lean_w= 1.00, tilt_w=0.5),
    ]
    field = GradientField(nodes, base_lean=24, base_tilt=20)

    section("Weight map")
    field.print_weight_map()

    section("Emitter sweep: maxA → center → maxB (pure lean)")
    # Manually set emitter states across the full lean range
    lean_states = [
        ("maxA",      -25, "a"),
        ("lean_a_70", -18, "a"),
        ("lean_a_30",  -7, "a"),
        ("center",      0, "b"),   # return to center
        ("lean_b_30",   7, "b"),
        ("lean_b_70",  18, "b"),
        ("maxB",       25, "b"),
    ]

    # Start emitter at maxA
    field.emitter.lean_opt_a(25)
    field.project("maxA")

    # Sweep by stepping from state to state
    step_amounts = [7, 11, 18, 7, 11, 18]   # cumulative lean movements
    labels = ["lean_a_70", "lean_a_30", "center", "lean_b_30", "lean_b_70", "maxB"]
    step_dirs = ["b", "b", "b", "b", "b", "b"]   # all moving toward B from maxA
    for amt, lbl, d in zip(step_amounts, labels, step_dirs):
        field.drive_lean(amt, direction=d, trigger=lbl)

    section("Pattern table (one column per node)")
    field.print_history_table()

    section("Pattern shifts")
    field.print_pattern_shifts()

    nodal = field.nodal_nodes(threshold=0.05)
    section("Nodal nodes (fixed points)")
    if nodal:
        for n in nodal:
            print(f"  {n.name}  lean_w={n.lean_w:+.2f}  tilt_w={n.tilt_w:+.2f}  "
                  f"current={n.decision}")
    else:
        print("  (none perfectly nodal — check threshold)")

    section("Final consensus")
    field.print_consensus()


# ── Demo 2: Sensor network with topology ─────────────────────────────────────

def demo2():
    banner("DEMO 2: Sensor Network with Field Topology")
    print()
    print("A pollutant plume event is modelled as emitter state changes.")
    print("7 sensor nodes have different positional relationships to the source:")
    print()
    print("  upwind_1       — directly in the plume path, strong direct coupling")
    print("  upwind_2       — adjacent to path, 70% lean coupling")
    print("  mid_direct     — midfield on path, 50% coupling")
    print("  cross_left     — perpendicular to plume, tilt-only (no lean signal)")
    print("  cross_right    — same, opposite side")
    print("  downwind_1     — far side of obstacle, lean-inverted (eddy current)")
    print("  downwind_2     — furthest, weak inverse lean + weak tilt")

    nodes = [
        FieldNode("upwind_1",   lean_w= 1.00, tilt_w= 1.00),
        FieldNode("upwind_2",   lean_w= 0.70, tilt_w= 0.80),
        FieldNode("mid_direct", lean_w= 0.50, tilt_w= 0.60),
        FieldNode("cross_left", lean_w= 0.00, tilt_w= 0.70),
        FieldNode("cross_right",lean_w= 0.00, tilt_w= 0.70),
        FieldNode("downwind_1", lean_w=-0.60, tilt_w= 0.40),
        FieldNode("downwind_2", lean_w=-0.30, tilt_w= 0.20),
    ]
    field = GradientField(nodes, base_lean=20, base_tilt=20)

    section("Weight map (field topology)")
    field.print_weight_map()

    # Background baseline
    field.project("baseline")

    section("Plume event: lean rises toward B (plume detected) + uncertainty grows")
    events = [
        ("plume_faint",  3,  2, "b"),
        ("plume_rising", 6,  4, "b"),
        ("plume_strong", 9,  6, "b"),
        ("plume_peak",  12,  8, "b"),
        ("plume_fading", 6,  4, "b"),
        ("plume_gone",   0,  2, "a"),
    ]
    for label, lean, tilt, direction in events:
        field.drive(lean=lean, tilt=tilt, direction=direction, trigger=label)

    section("Pattern table")
    field.print_history_table()

    section("Pattern shifts (when node decisions changed)")
    field.print_pattern_shifts()

    section("Final state consensus")
    field.print_consensus()

    section("Key insight")
    print("  cross_left and cross_right respond only to tilt (uncertainty), not lean.")
    print("  They read the same as each other throughout — they have no directional info.")
    print("  downwind nodes lag and invert — modelling an obstacle diffraction effect.")
    print("  The pattern encodes location implicitly via the weight matrix.")


# ── Demo 3: Pattern crystallisation ──────────────────────────────────────────

def demo3():
    banner("DEMO 3: Pattern Crystallisation — Tilt Sensitivity Gradient")
    print()
    print("6 nodes arranged by decreasing tilt sensitivity (1.0 → 0.0).")
    print("Lean sensitivity is held constant at 0.5 for all nodes.")
    print()
    print("Emitter is driven purely by tilt toward BOTH.")
    print("Shows which nodes 'crystallise' (leave BOTH) first as uncertainty rises.")

    nodes = [
        FieldNode("hi_tilt",    lean_w=0.5, tilt_w= 1.00),
        FieldNode("mid_hi",     lean_w=0.5, tilt_w= 0.75),
        FieldNode("mid",        lean_w=0.5, tilt_w= 0.50),
        FieldNode("mid_lo",     lean_w=0.5, tilt_w= 0.25),
        FieldNode("lo_tilt",    lean_w=0.5, tilt_w= 0.10),
        FieldNode("tilt_nodal", lean_w=0.5, tilt_w= 0.00),
    ]
    field = GradientField(nodes, base_lean=20, base_tilt=20)

    field.project("init_neutral")

    section("Tilt sweep: gradually push emitter toward BOTH")
    for i in range(1, 9):
        field.drive_tilt(amount=6, direction="b", trigger=f"tilt_step_{i:02d}")

    section("Pattern table")
    field.print_history_table()

    section("Pattern shifts (node first leaves UNKNOWN)")
    field.print_pattern_shifts()

    section("Final state per node")
    print(f"  {'name':<14}  {'tilt_w':>6}  {'pbm':>6}  {'A':>6}  {'B':>6}  {'decision'}")
    print("  " + "─" * 56)
    for n in field.nodes:
        print(f"  {n.name:<14}  {n.tilt_w:>6.2f}  {n.pbm:>6.3f}  "
              f"{n.a:>6.4f}  {n.b:>6.4f}  {n.decision}")

    section("Key insight")
    print("  Nodes with higher tilt_w accumulate more pbm per emitter tilt step.")
    print("  With lean=0 and pbc at center, A=B symmetry keeps all nodes in BOTH.")
    print("  To produce decision splits, lean must break the A/B symmetry first.")
    print("  The tilt_nodal node (tilt_w=0.0) stays at pbm=0.5 regardless of emitter.")
    print("  This is the tilt axis fixed point — no pattern can budge it.")


# ── Demo 4: Resonance scanning ────────────────────────────────────────────────

def demo4():
    banner("DEMO 4: Resonance Scanning — Pattern Complexity vs Emitter State")
    print()
    print("8 nodes with a mix of positive/negative/zero lean and tilt weights.")
    print("The emitter is driven through 8 distinct states (like sweeping frequency).")
    print("We count how many unique tokens appear in the field pattern at each state.")
    print("More unique tokens = more complex pattern = higher 'resonance diversity.'")

    nodes = [
        FieldNode("pos_direct",  lean_w= 1.0, tilt_w= 1.0),
        FieldNode("pos_lean",    lean_w= 0.8, tilt_w= 0.0),
        FieldNode("pos_tilt",    lean_w= 0.0, tilt_w= 0.8),
        FieldNode("nodal",       lean_w= 0.0, tilt_w= 0.0),
        FieldNode("neg_tilt",    lean_w= 0.0, tilt_w=-0.8),
        FieldNode("neg_lean",    lean_w=-0.8, tilt_w= 0.0),
        FieldNode("mixed_anti",  lean_w=-0.7, tilt_w= 0.7),
        FieldNode("neg_direct",  lean_w=-1.0, tilt_w=-1.0),
    ]
    field = GradientField(nodes, base_lean=22, base_tilt=22)

    # Define 8 emitter states: (lean, tilt, direction, label)
    states = [
        (0,  0,  "b", "neutral"),
        (15, 0,  "b", "lean_B"),
        (15, 10, "b", "lean_B+tilt_both"),
        (0,  10, "b", "tilt_both_only"),
        (0,  0,  "a", "back_neutral"),       # return to neutral via lean_unkn
        (15, 0,  "a", "lean_A"),
        (15, 10, "a", "lean_A+tilt_none"),
        (0,  10, "a", "tilt_none_only"),
    ]

    # Reset emitter to center manually then drive through states
    # (Each drive() call uses observe() which accumulates — we want absolute positions.
    #  So we use a fresh field for the scan and re-drive from a known baseline.)
    print()
    print(f"  {'state':<22}  {'lean_sig':>8}  {'tilt_sig':>8}  "
          f"{'unique_tokens':>13}  pattern")
    print("  " + "─" * 90)

    # Drive sequentially from neutral; not resetting emitter between states
    # (accumulates naturally — each step is cumulative from neutral start)
    scan_steps = [
        (0,  0,  "b", "neutral"),
        (15, 0,  "b", "lean_B"),
        (0,  10, "b", "tilt_both"),
        (0,  0,  "a", "neutral_2"),
        (15, 0,  "a", "lean_A"),
        (0,  20, "a", "tilt_none"),
        (0,  10, "b", "re_tilt_both"),     # push tilt back up
        (15, 0,  "b", "lean_B_again"),
    ]

    for lean, tilt, d, label in scan_steps:
        field.drive(lean=lean, tilt=tilt, direction=d, trigger=label)

    section("Full pattern table")
    field.print_history_table()

    section("Resonance diversity per state")
    print(f"  {'step':<4}  {'label':<22}  {'unique_tokens':>13}  {'tokens in play'}")
    print("  " + "─" * 70)
    for p in field.history:
        unique = set(p.tokens)
        print(f"  {p.emitter_step:<4}  {p.emitter_step:<22}  {len(unique):>13}  "
              f"{', '.join(sorted(unique))}")

    # Better: reuse labels
    print()
    print(f"  {'step':<4}  {'label':<22}  {'unique':>6}  {'tokens in play'}")
    print("  " + "─" * 60)
    h = field.history
    for i, p in enumerate(h):
        lbl = scan_steps[i][3] if i < len(scan_steps) else "?"
        unique = set(p.tokens)
        print(f"  {p.emitter_step:<4}  {lbl:<22}  {len(unique):>6}  "
              f"{', '.join(sorted(unique))}")

    section("Weight map")
    field.print_weight_map()

    section("Pattern shifts")
    field.print_pattern_shifts()

    section("Key insight")
    print("  The 'nodal' node (lean_w=0, tilt_w=0) holds BOTH throughout.")
    print("  'neg_direct' inverts the emitter on both axes — it mirrors state.")
    print("  States with both lean AND tilt active produce the most diverse patterns.")
    print("  Purely tilt-driven states collapse diversity: tilt-nodal nodes don't move.")


# ── Entry point ───────────────────────────────────────────────────────────────

if __name__ == "__main__":
    print("GradientField — Single Emitter Spatial Pattern Demonstrations")
    print("=" * W)
    print("One DecisionGradientT drives N FieldNodes via a weight matrix.")
    print("The Q emitter's lean+tilt state projects onto each node,")
    print("scaled by that node's (lean_w, tilt_w) relationship vector.")
    print("Demos: Chladni sweep | sensor topology | crystallisation | resonance scan")

    demo1()
    demo2()
    demo3()
    demo4()

    print()
    print("=" * W)
    print("  All demos complete.")
    print("=" * W)
