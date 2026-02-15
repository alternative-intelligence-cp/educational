#!/usr/bin/env python3
"""
Q21 Interactive Demonstrations
===============================
Practical examples showing Q21 quantum type use cases:
1. AGI consciousness substrate
2. Multi-sensor fusion
3. Multi-hypothesis tracking
4. Negotiation/compromise detection
5. Contradictory evidence handling

Author: Randy Hoggard
Date: February 14, 2026
"""

from q21 import Q21, q21_unknown, q21_opta, q21_optb, q21_none, q21_both


def print_header(title: str):
    """Print formatted section header"""
    print("\n" + "=" * 70)
    print(f"  {title}")
    print("=" * 70)


def print_subheader(title: str):
    """Print formatted subsection header"""
    print(f"\n{title}")
    print("-" * 70)


def demo_agi_consciousness():
    """Demonstrate Q21 for AGI consciousness modeling"""
    print_header("DEMO 1: AGI Consciousness Substrate")
    
    print("""
Q21 enables AGI to distinguish:
- "I'm uncertain" (UNKNOWN = 0)
- "I'm certain both options are wrong" (NONE states)
- "I'm certain both options are right" (BOTH states)
- "I'm confident in one option" (OPTA/OPTB states)

This is critical for consciousness because it models:
1. Genuine uncertainty vs. reasoned rejection
2. Superposition confidence (certain it's BOTH)
3. Negation confidence (certain it's NEITHER)
""")
    
    print_subheader("Scenario: AGI analyzing philosophical question")
    print("  Question: 'Is consciousness physical?'\n")
    
    # AGI starts uncertain
    belief = q21_unknown()
    print(f"  Initial state:           {belief}")
    
    # Evidence for physicalism
    print("\n  Reading physicalist arguments...")
    belief.observe(3)
    print(f"  After physicalist args:  {belief}")
    
    # Evidence for dualism
    print("\n  Reading dualist arguments...")
    belief.observe(-3)
    print(f"  After dualist args:      {belief}")
    
    # More evidence suggests BOTH have merit
    print("\n  Deep analysis reveals both perspectives valid in different contexts...")
    belief.observe(7)  # Strong evidence for BOTH
    print(f"  After deep analysis:     {belief}")
    
    result = belief.crystallize()
    print(f"\n  Crystallized decision:   {result}")
    print("\n  Interpretation: AGI recognizes BOTH physicalism AND dualism")
    print("  are valid frameworks depending on context (levels of description).")


def demo_sensor_fusion():
    """Demonstrate Q21 for multi-sensor fusion"""
    print_header("DEMO 2: Multi-Sensor Fusion")
    
    print("""
Q21 handles contradictory sensor data intelligently:
- BOTH state: Both sensors correct due to different contexts
- NONE state: Both sensors wrong (equipment failure)
- OPTA/OPTB: One sensor more reliable
""")
    
    print_subheader("Scenario: Autonomous vehicle - Is path clear?")
    
    # Initialize with uncertainty
    path_clear = q21_unknown()
    print(f"  Initial:                 {path_clear}")
    
    # Camera says CLEAR
    print("\n  Camera sensor:           CLEAR (+2 evidence)")
    path_clear.observe(2)
    print(f"  After camera:            {path_clear}")
    
    # LIDAR says BLOCKED
    print("\n  LIDAR sensor:            BLOCKED (-3 evidence)")
    path_clear.observe(-3)
    print(f"  After LIDAR:             {path_clear}")
    
    # Radar confirms BLOCKED
    print("\n  Radar sensor:            BLOCKED (-2 evidence)")
    path_clear.observe(-2)
    print(f"  After radar:             {path_clear}")
    
    result = path_clear.crystallize()
    print(f"\n  Decision:                {result}")
    print("\n  System correctly prioritizes LIDAR+Radar over camera.")
    print("  (Camera may see through rain, LIDAR/Radar detect physical object)")
    
    # Scenario 2: Both sensors correct
    print_subheader("\nScenario 2: Glass wall (BOTH sensors correct)")
    
    path_clear2 = q21_unknown()
    print(f"  Initial:                 {path_clear2}")
    
    print("\n  Camera:                  CLEAR (+3) - sees through glass")
    path_clear2.observe(3)
    print(f"  After camera:            {path_clear2}")
    
    print("\n  LIDAR:                   BLOCKED (-3) - detects glass surface")
    path_clear2.observe(-3)
    print(f"  After LIDAR:             {path_clear2}")
    
    print("\n  Integration layer recognizes 'glass' context...")
    print("  Both sensors are RIGHT in their own reference frame!")
    path_clear2.observe(8)  # Strong BOTH evidence
    print(f"  After integration:       {path_clear2}")
    
    result2 = path_clear2.crystallize()
    print(f"\n  Decision:                {result2}")
    print("\n  System recognizes BOTH sensors correct - physical barrier exists")
    print("  that is visually transparent. Safe action: treat as blocked.")


def demo_multi_hypothesis():
    """Demonstrate Q21 for multi-hypothesis tracking"""
    print_header("DEMO 3: Multi-Hypothesis Tracking")
    
    print("""
Q21 tracks confidence across competing hypotheses:
- BOTH state: Multiple hypotheses simultaneously valid
- NONE state: All hypotheses rejected
- OPTA/OPTB: Single hypothesis preferred
""")
    
    print_subheader("Scenario: Object tracking - Friend or Foe?")
    
    # Initialize
    is_friendly = q21_unknown()
    print(f"  Initial classification:  {is_friendly}")
    
    # IFF transponder signal
    print("\n  IFF signal detected:     FRIENDLY (+4 evidence)")
    is_friendly.observe(4)
    print(f"  After IFF:               {is_friendly}")
    
    # Aggressive maneuver
    print("\n  Aggressive maneuver:     HOSTILE (-5 evidence)")
    is_friendly.observe(-5)
    print(f"  After maneuver:          {is_friendly}")
    
    # Signal analysis: spoofed IFF
    print("\n  Signal analysis:         IFF potentially spoofed (-2 evidence)")
    is_friendly.observe(-2)
    print(f"  After analysis:          {is_friendly}")
    
    result = is_friendly.crystallize()
    print(f"\n  Classification:          {result}")
    print("\n  System crystallizes to HOSTILE - aggressive behavior overrides")
    print("  potentially spoofed IFF signal.")
    
    # Scenario 2: Neutral/unknown entity
    print_subheader("\nScenario 2: Civilian aircraft in restricted zone")
    
    civilian = q21_unknown()
    print(f"  Initial:                 {civilian}")
    
    print("\n  No IFF signal:           Suspicious (-2)")
    civilian.observe(-2)
    print(f"  After no IFF:            {civilian}")
    
    print("\n  Flight path:             Commercial route (+3)")
    civilian.observe(3)
    print(f"  After flight path:       {civilian}")
    
    print("\n  Radio contact:           Lost/no response (-2)")
    civilian.observe(-2)
    print(f"  After radio:             {civilian}")
    
    result2 = civilian.crystallize()
    print(f"\n  Classification:          {result2 if result2 else 'UNCERTAIN'}")
    print("\n  System remains UNCERTAIN - neither clearly hostile nor friendly.")
    print("  Appropriate action: Monitor closely, attempt contact.")


def demo_negotiation():
    """Demonstrate Q21 for negotiation/compromise detection"""
    print_header("DEMO 4: Negotiation & Compromise Detection")
    
    print("""
Q21 models negotiation states:
- OPTA/OPTB: One party's position preferred
- BOTH state: Compromise/middle ground found
- NONE state: Both positions rejected (need new approach)
""")
    
    print_subheader("Scenario: Business negotiation - Which proposal?")
    
    # Start with competing proposals
    proposal = q21_unknown()
    print(f"  Initial stance:          {proposal}")
    print("    Option A: Low cost, low features")
    print("    Option B: High cost, high features")
    
    # Financial analysis
    print("\n  Financial analysis:      Option A better (+3)")
    proposal.observe(3)
    print(f"  After financial:         {proposal}")
    
    # Technical analysis
    print("\n  Technical analysis:      Option B better (-4)")
    proposal.observe(-4)
    print(f"  After technical:         {proposal}")
    
    # Customer feedback
    print("\n  Customer feedback:       Want features but cheaper!")
    print("  Suggests BOTH proposals have merit... (+6 toward BOTH)")
    proposal.observe(6)
    print(f"  After feedback:          {proposal}")
    
    result = proposal.crystallize()
    print(f"\n  Decision:                {result}")
    print("\n  BOTH proposals have merit! Solution: Create hybrid")
    print("  - Medium cost, essential features only")
    print("  - Compromise satisfies both financial AND technical concerns")


def demo_contradictory_evidence():
    """Demonstrate Q21 handling contradictory evidence"""
    print_header("DEMO 5: Contradictory Evidence Resolution")
    
    print("""
Q21 distinguishes:
- Uncertainty (no evidence)
- Contradiction (balanced opposing evidence)
- Certainty in rejection (both options wrong)
- Certainty in acceptance (both options right)
""")
    
    print_subheader("Scenario: Scientific hypothesis testing")
    
    # Hypothesis: "Dark matter is WIMPs"
    hypothesis = q21_unknown()
    print(f"  Initial:                 {hypothesis}")
    print("    Testing: 'Dark matter is WIMPs'\n")
    
    # Experimental evidence
    print("  Experiment 1 (XENON):    No WIMPs detected (-3)")
    hypothesis.observe(-3)
    print(f"  After XENON:             {hypothesis}")
    
    print("\n  Experiment 2 (LUX):      No WIMPs detected (-2)")
    hypothesis.observe(-2)
    print(f"  After LUX:               {hypothesis}")
    
    print("\n  Theoretical review:      WIMPs still viable (+1)")
    hypothesis.observe(1)
    print(f"  After theory:            {hypothesis}")
    
    print("\n  Experiment 3 (PandaX):   No WIMPs detected (-2)")
    hypothesis.observe(-2)
    print(f"  After PandaX:            {hypothesis}")
    
    result = hypothesis.crystallize()
    print(f"\n  Conclusion:              {result}")
    print("\n  Multiple null results crystallize to rejection.")
    print("  Scientific consensus: WIMPs unlikely, explore alternatives.")
    
    # Quantum mechanics interpretation
    print_subheader("\nScenario 2: Quantum interpretation - Wave or Particle?")
    
    nature = q21_unknown()
    print(f"  Initial:                 {nature}")
    print("    Question: 'Are electrons waves or particles?'\n")
    
    print("  Double-slit experiment:  Wave behavior (+4)")
    nature.observe(4)
    print(f"  After double-slit:       {nature}")
    
    print("\n  Photoelectric effect:    Particle behavior (-4)")
    nature.observe(-4)
    print(f"  After photoelectric:     {nature}")
    
    print("\n  QM theory review:        BOTH descriptions valid!")
    print("  Wave-particle duality recognized... (+8 toward BOTH)")
    nature.observe(8)
    print(f"  After QM review:         {nature}")
    
    result2 = nature.crystallize()
    print(f"\n  Conclusion:              {result2}")
    print("\n  Physics recognizes BOTH wave AND particle nature.")
    print("  Context determines which description is appropriate.")
    print("  This is fundamental quantum complementarity!")


def demo_confidence_progression():
    """Show confidence level progression"""
    print_header("DEMO 6: Confidence Level Progression")
    
    print("""
Q21 has symmetric 5-level confidence for ALL states:
- Each region (NONE, OPTA, OPTB, BOTH) has MIN/LOW/MED/HIGH/MAX
- Saturation barriers prevent accidental drift
- Crystallization requires 60% confidence (MED level)
""")
    
    print_subheader("Progressive evidence accumulation (Option B)")
    
    q = q21_unknown()
    evidence_sequence = [1, 1, 1, 1, 1, 2, 2, 2]
    
    print(f"  Start:           {q}")
    
    for i, evidence in enumerate(evidence_sequence, 1):
        q.observe(evidence)
        crystallized = q.crystallize()
        status = f"→ {crystallized}" if crystallized else ""
        print(f"  Step {i} (+{evidence}):      {q} {status}")
    
    print("\n  Confidence increases gradually from MIN (20%) to MAX (100%)")
    print("  Crystallization occurs at MED (60%) level")
    
    print_subheader("\nCrossing saturation barrier (OPTB → BOTH)")
    
    q2 = Q21(5)  # OPTB_MAX
    print(f"  Start (OPTB_MAX):        {q2}")
    
    print("\n  Weak evidence (+1):      ")
    q2.observe(1)
    print(f"                           {q2} (stopped at barrier)")
    
    print("\n  Strong evidence (+3):    ")
    q2.observe(3)
    print(f"                           {q2} (crossed barrier!)")


def run_all_demos():
    """Run all demonstrations"""
    print("""
╔══════════════════════════════════════════════════════════════════════╗
║                                                                      ║
║                 Q21 Quantum Type - Python POC                        ║
║                  Interactive Demonstrations                          ║
║                                                                      ║
║  Demonstrating 21-state quantum confidence modeling for:            ║
║  • AGI consciousness substrate                                      ║
║  • Multi-sensor fusion                                               ║
║  • Multi-hypothesis tracking                                         ║
║  • Negotiation & compromise detection                                ║
║  • Contradictory evidence handling                                   ║
║                                                                      ║
╚══════════════════════════════════════════════════════════════════════╝
    """)
    
    demos = [
        demo_agi_consciousness,
        demo_sensor_fusion,
        demo_multi_hypothesis,
        demo_negotiation,
        demo_contradictory_evidence,
        demo_confidence_progression
    ]
    
    for demo in demos:
        demo()
        input("\n\n  Press ENTER to continue to next demo...")
    
    print_header("All Demonstrations Complete!")
    print("""
Q21 provides sophisticated confidence modeling that goes beyond
traditional binary or probabilistic approaches:

✓ Distinguishes uncertainty from rejection
✓ Models superposition certainty (BOTH states)
✓ Models negation certainty (NONE states)
✓ Prevents accidental drift with saturation barriers
✓ Requires 60% confidence for crystallization
✓ Symmetric confidence encoding (all states have 5 levels)

Perfect for:
- AGI consciousness modeling
- Multi-sensor fusion systems
- Multi-hypothesis tracking
- Negotiation & compromise detection
- Scientific hypothesis testing
- Quantum interpretation frameworks

See q21.py for implementation details.
See the Aria programming guide for full Q21 type specification.
    """)


if __name__ == "__main__":
    run_all_demos()
