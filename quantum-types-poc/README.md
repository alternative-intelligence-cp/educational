# Quantum Types - JavaScript POC

**A Proof-of-Concept Implementation of ARIA's Q3 and Q9 Quantum Number Types**

This is an educational demonstration showing how quantum types work before the full ARIA language is complete. Quantum types model **uncertainty** and **gradual confidence building**, making them ideal for:

- **Sensor fusion** (conflicting readings from multiple sensors)
- **AI decision-making** (multiple hypotheses, evidence accumulation)
- **Scientific measurements** (uncertainty ranges, precision tracking)
- **Cognitive modeling** (gradient thinking vs forced binary choices)
- **Graceful degradation** (handle sensor failures without crashing)

---

## Quick Start

```bash
cd /home/randy/Workspace/TMP/js
node quantum_demo.js
```

---

## What Are Quantum Types?

Quantum types **maintain TWO hypotheses simultaneously** (a superposition) along with a **confidence metric** that tracks which hypothesis is more likely correct.

### Q9 - Nine-State Quantum

- **Range**: 9 confidence states from -4 to +4
- **States**:
  - `-4` = NEITHER (reject both hypotheses)
  - `-3` to `-1` = Lean toward hypothesis A (increasing confidence)
  - `0` = UNKNOWN (no preference)
  - `+1` to `+3` = Lean toward hypothesis B (increasing confidence)
  - `+4` = BOTH (accept both hypotheses)

### Q3 - Three-State Quantum

- **Range**: 3 confidence states: -1, 0, +1
- **States**:
  - `-1` = Lean toward A
  - `0` = UNKNOWN
  - `+1` = Lean toward B

---

## Core Concepts

### 1. Superposition

A quantum number holds **two values** (hypotheses a and b) simultaneously:

```javascript
const q = new QuantumNumber(9, 10, 20, 0);  // a=10, b=20, confidence=0 (unknown)
```

Operations apply to **BOTH hypotheses**:

```javascript
q.add(5);   // Both a and b increase by 5 → a=15, b=25
q.mul(2);   // Both doubled → a=30, b=50
```

### 2. Confidence Building

Evidence gradually shifts confidence from unknown toward one hypothesis:

```javascript
const sensor = new QuantumNumber(9, 100, 150, 0);  // Two conflicting sensor readings

// Evidence 1: Reading closer to B
sensor.qif(true, 0, 0, 1);  // c: 0 → 1

// Evidence 2: More evidence for B
sensor.qif(true, 0, 0, 1);  // c: 1 → 2

// Now crystallizable!
const value = sensor.crystallize();  // Returns 150 (hypothesis B won)
```

### 3. Crystallization

**Crystallization** is the "collapse" moment: converting quantum superposition → single definite value.

Crystallization succeeds when:
- Q9: `abs(c) >= 1` (any lean counts)
- Q3: `abs(c) >= 1` (must lean one way)

Crystallization **fails** (returns `null`) when:
- `c == 0` (UNKNOWN: no clear winner)
- `c == -4` (Q9 NEITHER: both rejected)
- `c == +4` (Q9 BOTH: both accepted equally)

```javascript
const q = new QuantumNumber(9, 10, 20, 2);
const result = q.crystallize();  // Returns 20 (c=2 leans toward b)
```

---

## API Reference

### Constructor

```javascript
new QuantumNumber(width, a=0, b=0, c=0)
```

- `width`: 3 (Q3) or 9 (Q9)
- `a`: Hypothesis A value
- `b`: Hypothesis B value
- `c`: Confidence (-4 to +4 for Q9, -1 to +1 for Q3)

### Arithmetic Operations

All operations apply to **both hypotheses**:

```javascript
q.add(5)      // Add 5 to both a and b
q.sub(3)      // Subtract 3 from both
q.mul(2)      // Multiply both by 2
q.div(4)      // Divide both by 4
q.mod(7)      // Modulo both by 7
q.sqrt()      // Square root both
```

### Quantum Conditional Updates

```javascript
q.qif(condition, a_change, b_change, c_change)
```

If `condition` is true, apply changes to the quantum state:

```javascript
q.qif(sensorA > threshold, 0, 0, -1);  // Increase confidence toward A
q.qif(sensorB > threshold, 0, 0, +1);  // Increase confidence toward B
```

### Q-Functions (Conditional Quantum Operations)

Execute callback **only if hypotheses meet condition**:

#### qor - Apply if EITHER hypothesis meets condition

```javascript
q.qor(v => v > 0, q => {
    q.a *= 2;
    q.b *= 2;
});
// Executes if a>0 OR b>0
```

#### qand - Apply if BOTH hypotheses meet condition

```javascript
q.qand(v => v > 0, q => {
    q.a = Math.sqrt(q.a);
    q.b = Math.sqrt(q.b);
});
// Executes ONLY if a>0 AND b>0
```

#### qxor - Apply if EXACTLY ONE hypothesis meets condition

```javascript
q.qxor(v => v > 100, q => {
    // Adjust confidence toward the one that meets condition
    if(q.a > 100) q.c--;
    if(q.b > 100) q.c++;
});
// Executes if (a>100 XOR b>100), not if both or neither
```

#### qnor - Apply if NEITHER hypothesis meets condition

```javascript
q.qnor(v => v > 0, q => {
    console.log("Both hypotheses negative!");
});
// Executes ONLY if a<=0 AND b<=0
```

#### qnand - Apply if NOT (both meet condition)

```javascript
q.qnand(v => v > 0, q => {
    console.log("At least one hypothesis non-positive");
});
// Executes if !(a>0 AND b>0)
```

#### qconf - Apply if confidence >= threshold

```javascript
q.qconf(v => true, 2, q => {
    console.log("High confidence operation");
});
// Executes ONLY if abs(c) >= 2
```

#### qnconf - Apply if confidence < threshold

```javascript
q.qnconf(v => true, 2, q => {
    console.log("Low confidence - need more evidence");
});
// Executes ONLY if abs(c) < 2
```

### Query Methods

```javascript
q.getA()         // Get hypothesis A value
q.getB()         // Get hypothesis B value
q.getC()         // Get confidence value
q.getState()     // Get {a, b, c, width} object
q.crystallize()  // Get winning value (or null if can't crystallize)
```

### Lifecycle

```javascript
q.destroy()      // Free resources, return ID to pool
```

---

## Example Use Cases

### 1. Sensor Fusion (Conflicting Thermometers)

```javascript
// Two thermometers disagree: 20.5°C vs 21.3°C
const temp = new QuantumNumber(9, 20.5, 21.3, 0);

// Reading 1: 21.1°C (closer to B)
temp.qif(Math.abs(21.1 - temp.getB()) < Math.abs(21.1 - temp.getA()), 0, 0, 1);

// Reading 2: 21.2°C (closer to B)
temp.qif(Math.abs(21.2 - temp.getB()) < Math.abs(21.2 - temp.getA()), 0, 0, 1);

// Confidence built, crystallize
const finalTemp = temp.crystallize();  // Returns 21.3 (sensor B was correct)
```

### 2. AI Decision Making (Path Planning)

```javascript
// Two possible paths: Short (10km) but risky, Long (15km) but safe
const path = new QuantumNumber(9, 10, 15, 0);

// Evidence: Weather good (favors short path)
path.qif(weatherGood, 0, 0, -1);

// Evidence: Traffic heavy (favors long path to avoid city)
path.qif(trafficHeavy, 0, 0, 1);

// Evidence: Time critical (favors short path)
path.qif(timeCritical, 0, 0, -2);

// Decide based on accumulated evidence
const chosenPath = path.crystallize();
```

### 3. Scientific Measurement Uncertainty

```javascript
// Particle position measurement: Two detectors give 10.2 vs 10.8
const position = new QuantumNumber(9, 10.2, 10.8, 0);

// Perform calculations on the uncertain value
position.add(5).mul(2);  // Both hypotheses evolve: a=(10.2+5)*2=30.4, b=(10.8+5)*2=31.6

// Later, when uncertainty resolved by additional evidence
position.qif(detectorACalibrated, 0, 0, -3);
const finalPosition = position.crystallize();  // Choose based on confidence
```

### 4. Graceful Degradation (Sensor Failure)

```javascript
const altitude = new QuantumNumber(9, gps_altitude, barometer_altitude, 2);

// GPS fails: Set confidence to unknown
if(gpsFailure) {
    altitude.qif(true, 0, 0, -2);  // Reduce confidence toward GPS
}

// Can still compute, just with awareness of increased uncertainty
const currentAltitude = altitude.crystallize();
if(currentAltitude === null) {
    console.log("WARNING: Cannot determine altitude with confidence");
    // Fall back to safe altitude assumption
}
```

---

## Philosophy: Gradient Thinking

**Binary thinking** forces premature decisions:

```javascript
// Binary: Must choose NOW
const value = (sensorA + sensorB) / 2;  // What if one is broken?
```

**Quantum thinking** allows uncertainty to exist until resolved:

```javascript
// Quantum: Track both, decide when confident
const value = new QuantumNumber(9, sensorA, sensorB, 0);
// ... accumulate evidence ...
const final = value.crystallize();  // Decide when ready
```

This models how **real systems** work:
- Scientists don't force measurement to one value immediately
- AI doesn't commit to one path without evidence
- Humans don't make binary choices instantly

Quantum types **embrace uncertainty** as a first-class concept.

---

## Implementation Notes

### ID Pooling

Destroyed quantum numbers return their IDs to a pool for reuse:

```javascript
const q = new QuantumNumber(9, 1, 2, 0);  // Gets ID 0
q.destroy();                               // Returns ID 0 to pool

const q2 = new QuantumNumber(9, 3, 4, 0); // Reuses ID 0
```

### Confidence Clamping

Confidence is automatically clamped to valid range:

```javascript
const q = new QuantumNumber(9, 10, 20, 3);
q.qif(true, 0, 0, 5);  // Tries to set c=8, but clamped to c=4 (max for Q9)
```

### Comparison Logic

`Q_COMPARE()` returns the winning hypothesis OR null:

- `c === 0` → `null` (unknown)
- `c === -4` (Q9 NEITHER) → `null` (both rejected)
- `c === +4` (Q9 BOTH) → `null` (both accepted)
- `c < 0` → Returns `a` (lean toward A)
- `c > 0` → Returns `b` (lean toward B)

---

## Files

- **quantum.js** - Core quantum type implementation
- **quantum_demo.js** - Comprehensive demonstration with 8 examples
- **README_QUANTUM.md** - This file

---

## Educational Value

This POC demonstrates quantum types in **JavaScript** (a familiar language) before the full ARIA language is complete. It shows:

1. ✓ How superposition works (two values simultaneously)
2. ✓ How confidence builds gradually from evidence
3. ✓ How crystallization collapses superposition to single value
4. ✓ How Q-functions enable conditional quantum operations
5. ✓ How quantum arithmetic preserves superposition
6. ✓ Why quantum types model real-world uncertainty better than binary types

---

## Future: ARIA Language Integration

In the full ARIA language, quantum types will have native syntax:

```aria
// Native ARIA quantum types (future)
q9 sensor_reading = {100, 150, 0};  // Q9 type

sensor_reading + 5;                 // Quantum arithmetic
sensor_reading.qif(condition, 0, 0, 1);  // Evidence accumulation

f64 result = sensor_reading.#;     // Crystallization operator
```

Until then, this JavaScript POC shows the concept in action! 🚀

---

## License

Part of the ARIA programming language educational materials.

---

**Status**: ✓ Complete POC Implementation

**Lines**: ~450 (quantum.js + quantum_demo.js + README)

**Demonstrates**: Q3/Q9 quantum numbers, crystallization, Q-functions, evidence accumulation, sensor fusion, graceful degradation

**Ready for**: Educational repository, language specification reference, cognitive modeling research
