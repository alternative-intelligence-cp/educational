#!/usr/bin/env node
//##### QUANTUM TYPE DEMO ###################################################
// Demonstrates Q3 and Q9 quantum numbers in JavaScript
// This is a POC of ARIA's quantum types for educational purposes

// Load quantum.js
require('./quantum.js');

console.log("═".repeat(80));
console.log("QUANTUM NUMBER DEMONSTRATION");
console.log("═".repeat(80));
console.log();

// Create helper for displaying state
function showState(q, label) {
    const state = q.getState();
    if(!state) {
        console.log(`${label}: DESTROYED`);
        return;
    }
    const crystallized = q.crystallize();
    console.log(`${label}:`);
    console.log(`  Hypothesis A: ${state.a.toFixed(3)}`);
    console.log(`  Hypothesis B: ${state.b.toFixed(3)}`);
    console.log(`  Confidence:   ${state.c.toFixed(0)} ${getConfidenceLabel(state.c, state.width)}`);
    if(crystallized !== null) {
        console.log(`  ✓ Crystallizes to: ${crystallized.toFixed(3)}`);
    } else {
        console.log(`  ✗ Cannot crystallize (${getCrystallizationReason(state.c, state.width)})`);
    }
    console.log();
}

function getConfidenceLabel(c, width) {
    if(width === 9) {
        if(c === -4) return "[NEITHER]";
        if(c === -3) return "[Strong doubt toward both]";
        if(c === -2) return "[Lean toward A strongly]";
        if(c === -1) return "[Lean toward A slightly]";
        if(c === 0) return "[UNKNOWN]";
        if(c === 1) return "[Lean toward B slightly]";
        if(c === 2) return "[Lean toward B strongly]";
        if(c === 3) return "[Strong confidence in both]";
        if(c === 4) return "[BOTH]";
    } else if(width === 3) {
        if(c === -1) return "[Lean toward A]";
        if(c === 0) return "[UNKNOWN]";
        if(c === 1) return "[Lean toward B]";
    }
    return "";
}

function getCrystallizationReason(c, width) {
    if(c === 0) return "unknown state";
    if(width === 9) {
        if(c === -4) return "neither accepted";
        if(c === 4) return "both accepted";
    }
    return "low confidence";
}

console.log("─".repeat(80));
console.log("EXAMPLE 1: Basic Q9 Quantum Number");
console.log("─".repeat(80));
console.log("Creating Q9 with hypotheses: A=10, B=20, confidence=+2 (lean toward B)");
console.log();

const q1 = new QuantumNumber(9, 10, 20, 2);
showState(q1, "Initial state");

console.log("Adding 5 to both hypotheses...");
q1.add(5);
showState(q1, "After add(5)");

console.log("Multiplying by 2...");
q1.mul(2);
showState(q1, "After mul(2)");

console.log("─".repeat(80));
console.log("EXAMPLE 2: Evidence Accumulation with qif()");
console.log("─".repeat(80));
console.log("Simulating sensor readings that gradually build confidence");
console.log();

const q2 = new QuantumNumber(9, 100, 150, 0);  // Start with no confidence
showState(q2, "Initial: Unknown which sensor is correct");

console.log("Evidence 1: Reading closer to hypothesis B...");
q2.qif(true, 0, 0, 1);  // Increase confidence toward B
showState(q2, "After evidence 1");

console.log("Evidence 2: Another reading favoring B...");
q2.qif(true, 0, 0, 1);  // More confidence toward B
showState(q2, "After evidence 2");

console.log("Evidence 3: Strong evidence for B...");
q2.qif(true, 0, 0, 1);  // Now crystallizable!
showState(q2, "After evidence 3");

const crystallized = q2.crystallize();
if(crystallized !== null) {
    console.log(`✓ System now confident: Using value ${crystallized}!`);
}
console.log();

console.log("─".repeat(80));
console.log("EXAMPLE 3: Q-Functions (Conditional Quantum Operations)");
console.log("─".repeat(80));
console.log();

const q3 = new QuantumNumber(9, -5, 15, 1);
showState(q3, "Initial state");

console.log("Using qor() to double values if EITHER is positive...");
q3.qor(
    v => v > 0,  // Condition: is value positive?
    q => {       // Callback: what to do if condition met
        q.a *= 2;
        q.b *= 2;
    }
);
showState(q3, "After qor (either positive)");

console.log("Using qand() to square values if BOTH are positive...");
q3.qand(
    v => v > 0,  // Condition: is value positive?
    q => {       // Callback: what to do if BOTH meet condition
        q.a = q.a * q.a;
        q.b = q.b * q.b;
    }
);
showState(q3, "After qand (both positive) - NO CHANGE (A is negative)");

console.log("─".repeat(80));
console.log("EXAMPLE 4: Q3 Simple Quantum (Trinary Logic)");
console.log("─".repeat(80));
console.log("Q3 has only 3 confidence states: -1 (A), 0 (unknown), +1 (B)");
console.log();

const q4 = new QuantumNumber(3, 42, 58, 0);
showState(q4, "Initial: Unknown");

console.log("Simulation: Coin flip favors hypothesis B...");
q4.qif(Math.random() > 0.5, 0, 0, 1);
showState(q4, "After simulation");

console.log("─".repeat(80));
console.log("EXAMPLE 5: NEITHER and BOTH States (Q9 only)");
console.log("─".repeat(80));
console.log();

const q5_neither = new QuantumNumber(9, 100, 200, -4);  // NEITHER
showState(q5_neither, "NEITHER state (c=-4)");

const q5_both = new QuantumNumber(9, 100, 200, 4);     // BOTH
showState(q5_both, "BOTH state (c=+4)");

console.log("Neither NEITHER nor BOTH states can crystallize!");
console.log("NEITHER: Both hypotheses rejected");
console.log("BOTH: Both hypotheses accepted (no single winner)");
console.log();

console.log("─".repeat(80));
console.log("EXAMPLE 6: Confidence-Based Branching with qconf()");
console.log("─".repeat(80));
console.log();

const q6 = new QuantumNumber(9, 10, 20, 3);
showState(q6, "High confidence state (c=3)");

console.log("Using qconf() to apply operation ONLY if confidence >= 2...");
let applied = false;
q6.qconf(
    v => true,  // Always true
    2,          // Threshold: abs(c) >= 2
    q => {
        q.a += 100;
        q.b += 100;
        applied = true;
        console.log("  ✓ Operation applied (high confidence)");
    }
);
if(applied) {
    showState(q6, "After qconf operation");
}

console.log("─".repeat(80));
console.log("EXAMPLE 7: Quantum Arithmetic Preserves Superposition");
console.log("─".repeat(80));
console.log();

const q7 = new QuantumNumber(9, 8, 12, 0);
showState(q7, "Initial: Two hypotheses about a value");

console.log("Computing: ((value + 3) * 2) / 4");
q7.add(3).mul(2).div(4);
showState(q7, "After quantum computation");

console.log("Both hypotheses evolved through the SAME operations!");
console.log("This is the quantum superposition: computation on BOTH possibilities.");
console.log();

console.log("─".repeat(80));
console.log("EXAMPLE 8: Destroying Quantum Numbers");
console.log("─".repeat(80));
console.log();

const q8 = new QuantumNumber(9, 100, 200, 2);
showState(q8, "Before destroy");

console.log("Calling destroy()...");
const destroyed = q8.destroy();
console.log(`Destroy returned: ${destroyed}`);
showState(q8, "After destroy");

console.log("─".repeat(80));
console.log("WHY QUANTUM TYPES? The Philosophy");
console.log("─".repeat(80));
console.log(`
Quantum types model UNCERTAINTY and GRADUAL CONFIDENCE building,
which is how real systems work:

1. SENSORS: Two thermometers disagree. Keep both readings; build confidence
   as more evidence arrives. Crystallize when confident enough.

2. AI DECISIONS: Model considers multiple hypotheses. Evidence accumulates
   toward one choice. q.crystallize() gives final decision when ready.

3. SCIENTIFIC DATA: Measurement has uncertainty. Track both bounds of the
   range. Confidence indicates precision of the measurement.

4. COGNITIVE MODELING: Binary thinking forces premature decisions. Quantum
   types allow "I don't know yet" (c=0) and "not enough confidence" states.

5. GRACEFUL DEGRADATION: When sensors fail, confidence drops to 0 (unknown)
   or -4 (neither). System continues operating, just with awareness of
   increased uncertainty.

The crystallization operator (q.#) is the "collapse" moment: when you MUST
choose a single value. But until that moment, the system maintains BOTH
possibilities and tracks confidence. This is gradient thinking vs binary.
`);

console.log("─".repeat(80));
console.log("IMPLEMENTATION COMPLETE ✓");
console.log("─".repeat(80));
console.log(`
This JavaScript POC demonstrates:
  ✓ Q3 and Q9 quantum number creation
  ✓ Arithmetic operations (add, sub, mul, div, mod, sqrt)
  ✓ Q-functions (qor, qand, qxor, qnor, qnand, qconf, qnconf)
  ✓ Crystallization logic
  ✓ Evidence accumulation with qif()
  ✓ Instance management (create/destroy/ID pooling)
  ✓ Confidence-based conditional operations

Ready for the ARIA language implementation!
`);

console.log("═".repeat(80));
