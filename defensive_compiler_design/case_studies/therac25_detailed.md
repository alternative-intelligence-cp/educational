# Therac-25 Detailed Technical Analysis

**Incident Timeline:** 1985-1987  
**Casualties:** 6+ patients received massive radiation overdoses, 3 deaths confirmed  
**Root Cause:** Race condition in safety-critical software  
**Preventable:** Yes, with defensive compiler design

---

## System Overview

The Therac-25 was a computer-controlled radiation therapy machine manufactured by Atomic Energy of Canada Limited (AECL). It delivered two types of radiation:

1. **Electron beam mode** (direct, lower energy)
2. **X-ray mode** (requires metal target to convert electron beam)

The system used a **turntable** that moved the metal target in/out of position:
- X-ray mode: Target IN (converts beam to X-rays)
- Electron mode: Target OUT (beam goes directly to patient)

**Critical safety dependency:** Beam intensity must match turntable position.

---

## The Software Architecture

### Language and Environment

- **Language:** PDP-11 assembly language
- **OS:** Custom real-time executive
- **Safety model:** Software interlocks (no hardware backups)
- **Code size:** ~20,000 lines

### The Fatal Assumption

Previous models (Therac-6, Therac-20) had **hardware interlocks** - physical mechanisms that prevented dangerous beam configurations.

Therac-25 designers: "Software is more reliable than hardware. We don't need physical interlocks."

**This assumption killed people.**

---

## The Bug: Race Condition in Shared Memory

### The Vulnerable Code Pattern

```assembly
; Simplified pseudocode of actual vulnerable pattern

; TASK 1: Keyboard Handler (runs when operator presses keys)
keyboard_handler:
    READ key_input
    UPDATE prescription_data
    UPDATE turntable_position_request
    ; NO SYNCHRONIZATION
    RETURN

; TASK 2: Beam Controller (runs independently)
beam_controller:
    READ turntable_position_request
    ; NO SYNCHRONIZATION
    IF turntable_in_position THEN
        CONFIGURE beam_intensity
        FIRE beam
    ENDIF
    RETURN
```

### The Critical Timing Window

**Normal sequence (safe):**
1. Operator selects X-ray mode
2. `keyboard_handler` sets `turntable_position_request = IN`
3. Turntable moves target into position
4. `beam_controller` reads position, sees target IN
5. `beam_controller` sets beam intensity to X-ray level
6. Beam fires safely (target converts to X-rays)

**Race condition (lethal):**
1. Operator selects X-ray mode
2. `keyboard_handler` sets `turntable_position_request = IN`
3. **Operator realizes mistake, quickly changes to electron mode**
4. `keyboard_handler` sets `turntable_position_request = OUT`
5. **BUT:** `beam_controller` already read old value (IN)
6. Turntable moves OUT (electron mode)
7. `beam_controller` still thinks target is IN
8. `beam_controller` sets beam to **X-ray intensity** (100x electron dose)
9. Beam fires **at full X-ray power** with **no target to absorb it**
10. **Patient receives lethal radiation dose directly**

### The Timing Requirement

This only happened if the operator:
- Typed fast enough (experienced users)
- Corrected within ~8 seconds
- Hit the exact timing window between reads

**This is why testing missed it** - testers weren't fast enough typists.

---

## The Code-Level Failure

### Shared Memory Without Protection

```assembly
; Actual memory layout (simplified)
COMMON_MEMORY:
    turntable_pos:      .BYTE 0    ; 0=OUT, 1=IN
    beam_intensity:     .WORD 0    ; Power level
    ready_flag:         .BYTE 0    ; Treatment ready
    
; Multiple tasks access this with NO LOCKS
; NO MUTEX
; NO SEMAPHORE
; NO ATOMIC OPERATIONS
```

### The Missing Synchronization

What should have been there:

```c
// CORRECT APPROACH: Atomic update with mutex
pthread_mutex_lock(&config_mutex);
    turntable_position = new_position;
    beam_intensity = calculate_intensity(new_position);
    // Both updated together, atomically
pthread_mutex_unlock(&config_mutex);
```

What was actually there:

```c
// ACTUAL CODE: No synchronization
turntable_position = new_position;  // Task 1 writes
// ... somewhere else ...
intensity = get_intensity(turntable_position);  // Task 2 reads
// RACE: Task 2 might read while Task 1 is updating
```

---

## What a Defensive Compiler Would Catch

### 1. Race Condition Detection

```
ERROR: Unsynchronized shared memory access detected
  
  Variable: turntable_position (address 0x2040)
  
  WRITER TASK: keyboard_handler
    Location: line 847
    Access: WRITE (no lock held)
    
  READER TASK: beam_controller  
    Location: line 1891
    Access: READ (no lock held)
    
  ANALYSIS: These tasks execute concurrently
  RISK: Reader may see inconsistent state during write
  
  CONSEQUENCE: In safety-critical system, this can cause:
    - Beam fired at wrong intensity
    - Treatment applied with wrong parameters
    - POTENTIAL HARM TO PATIENT
    
  REQUIRED FIX:
    Option 1: Add mutex protection
    Option 2: Use atomic operations
    Option 3: Prove tasks never execute simultaneously (provide proof)
    
  COMPILATION BLOCKED until race condition resolved
```

### 2. Safety-Critical Variable Tracking

```
ERROR: Safety-critical variable modified without validation
  
  Variable: beam_intensity
  Classification: SAFETY_CRITICAL (annotated in source)
  
  Modified at: line 1923
  Validation: NONE DETECTED
  
  REQUIRED: Safety-critical variables must be validated before use
  
  Example fix:
    if (validate_beam_config(turntable_pos, beam_intensity)) {
        fire_beam();
    } else {
        abort_treatment("Configuration mismatch");
    }
    
  COMPILATION BLOCKED until validation added
```

### 3. Concurrent Access Pattern Analysis

```
WARNING: Shared memory accessed from multiple execution contexts
  
  Variable: turntable_pos
  
  Context 1: Interrupt handler (keyboard_irq)
  Context 2: Main loop (beam_control_task)
  Context 3: Timer interrupt (watchdog_task)
  
  RISK: Interrupt can occur during main loop execution
  CONSEQUENCE: Partial updates visible to other contexts
  
  RECOMMENDATION: 
    - Disable interrupts during critical section, OR
    - Use atomic operations, OR
    - Add memory barriers
    
  Severity: HIGH (safety-critical system)
```

---

## The Incident Timeline

### June 1985: First Overdose (Kennestone Hospital, Georgia)

**Patient:** Woman receiving treatment for breast cancer

**What happened:**
1. Operator selected X-ray mode
2. Realized mistake, quickly changed to electron mode
3. Machine displayed "MALFUNCTION 54" (cryptic error)
4. Operator hit "PROCEED" (had seen minor errors before)
5. Patient reported feeling "intense heat"
6. Actual dose: ~15,000-20,000 rads (normal: 200 rads)

**Outcome:** Patient died 5 months later from radiation-induced complications

**AECL Response:** "Impossible. Software has interlocks."

### July 1985: Second Overdose (Ontario Cancer Foundation)

**Patient:** Woman receiving treatment for cervical cancer

**What happened:**
1. Operator experienced fast, repeated pattern: change mode, error, change mode
2. Each time, machine showed "MALFUNCTION 54"
3. Each time, operator hit "PROCEED"
4. Patient received multiple overdoses

**Outcome:** Patient experienced severe radiation burns, died 4 months later

**AECL Response:** "Software tested extensively. Probably equipment failure."

### December 1985: Third Overdose (Yakima Valley Memorial Hospital)

**Patient:** Woman receiving routine radiation therapy

**What happened:**
1. Same pattern: fast mode changes by experienced operator
2. Machine error message
3. Operator assumed minor malfunction (common on this machine)
4. Patient received massive overdose

**Outcome:** Patient died 3 months later

**Critical moment:** Operator reported exact sequence to trigger bug. AECL finally investigated.

---

## The Investigation

### Initial Denial

AECL engineers insisted the bug was impossible:
- "Software has been tested for thousands of hours"
- "Mathematical proof shows race condition can't occur"
- "Operators must be doing something wrong"

### The Breakthrough

One physicist **recreated the bug** by:
1. Practicing the exact key sequence
2. Timing it precisely
3. Reproducing the error in controlled conditions

**Result:** Machine delivered massive overdose to test phantom.

### The Admission

AECL finally acknowledged:
- Race condition existed
- Software interlocks were insufficient
- Hardware safety systems were needed

---

## The Root Causes (Beyond the Code)

### 1. Software as Sole Safety System

**Assumption:** "Software is more reliable than hardware"

**Reality:** Software complexity makes it **less** predictable than simple mechanical interlocks.

**Lesson:** Safety-critical systems need **defense in depth** - software + hardware + procedural safeguards.

### 2. Inadequate Testing

**What was tested:**
- Normal operation sequences
- Individual component behavior
- Slow, deliberate operator actions

**What was NOT tested:**
- Fast mode changes
- Concurrent task interactions
- All possible timing windows

**Lesson:** Testing can't find bugs you don't think to test for. Static analysis catches patterns you don't imagine.

### 3. Opaque Error Messages

**Actual error:** "MALFUNCTION 54"

**What operator needed to know:** "CRITICAL: Beam configuration mismatch. DO NOT PROCEED."

**What operator thought:** "Minor glitch, probably the turntable sensor again."

**Lesson:** Error messages in safety-critical systems must communicate DANGER, not just malfunction codes.

### 4. Trusting Developer Claims

**AECL claim:** "This is impossible."

**Reality:** Happened six times, killed three people.

**Lesson:** "Trust but verify" fails in safety-critical systems. Verification must be automated and mandatory.

---

## What Defensive Compiler Features Prevent This

### Feature 1: Mandatory Concurrency Analysis

```
COMPILER REQUIREMENT: All shared memory must be annotated

// Required annotation
SHARED volatile uint8_t turntable_position PROTECTED_BY(config_lock);

// Compiler enforces:
access_variable(turntable_position) {
    REQUIRE: config_lock.is_held()
    ELSE: COMPILATION ERROR
}
```

### Feature 2: Safety-Critical Type System

```c
// Define safety level in type system
typedef safety_critical<uint16_t> BeamIntensity;
typedef safety_critical<uint8_t> TurntablePosition;

// Compiler enforces validation before use
void fire_beam(BeamIntensity intensity, TurntablePosition position) {
    // Won't compile unless both parameters are validated<> type
}

// Correct usage
validated<BeamIntensity> safe_intensity = 
    validate_beam_config(turntable_pos, requested_intensity);
    
if (safe_intensity.is_valid()) {
    fire_beam(safe_intensity.value(), position);
}
```

### Feature 3: Temporal Safety Checks

```
ERROR: Time-of-check to time-of-use gap detected
  
  Line 1847: if (turntable_position == IN)
  Line 1891: fire_beam(intensity)
  
  GAP: 44 lines, ~340 microseconds estimated
  RISK: turntable_position could change between check and use
  
  REQUIRED: Atomic read-and-use or locked critical section
```

---

## The Legal and Regulatory Aftermath

### Criminal Investigations

- Multiple lawsuits filed
- AECL settled out of court (amounts undisclosed)
- FDA investigation launched

### Regulatory Changes

**Before Therac-25:**
- Software in medical devices largely unregulated
- Self-certification acceptable
- No mandatory independent verification

**After Therac-25:**
- FDA now requires software documentation
- Independent safety analysis mandatory
- Hardware interlocks required for life-critical functions
- IEC 62304 standard created (medical device software lifecycle)

### Industry Impact

The Therac-25 is now **the canonical example** of software failure in safety-critical systems. Studied in:
- Computer science safety courses
- Medical device engineering programs
- Software engineering ethics courses
- Legal case studies on product liability

---

## Lessons for Compiler Designers

### 1. Default to Paranoid, Not Permissive

**Traditional compiler:** "This code is technically valid. Compiling."

**Defensive compiler:** "This code has unsynchronized shared access. What happens if tasks interleave? Prove it's safe or I won't compile it."

### 2. Make Dangerous Patterns Unwritable

Instead of hoping developers remember to lock:

```c
// Make the type system enforce it
class Locked<T> {
    T value;
    Mutex lock;
    
    // Can't access value without holding lock
    auto access() -> LockedGuard<T> {
        return LockedGuard(value, lock);
    }
};

Locked<TurntablePosition> turntable_pos;

// Won't compile: turntable_pos.value = X;  // Direct access forbidden
// Must use: auto guard = turntable_pos.access(); guard.value = X;
```

### 3. Require Safety Proofs for Critical Code

```c
// Annotation declares criticality
SAFETY_CRITICAL void fire_beam() {
    // Compiler requires proof of:
    // - All inputs validated
    // - No race conditions possible  
    // - Error handling complete
    // - Invariants maintained
}
```

### 4. Make Error Messages Explain Consequences

**Bad:** "MALFUNCTION 54"

**Good:**
```
CRITICAL ERROR: Beam/Target Mismatch
  Expected: X-ray target IN position
  Actual: Target OUT (electron mode)
  Consequence: Firing beam would deliver 100x normal dose
  ACTION REQUIRED: STOP. Do not proceed. Shut down system.
```

---

## Modern Parallel: Where This Still Happens

### Medical Devices

Insulin pumps, pacemakers, surgical robots - still have software bugs that cause patient harm.

**Why?** Firmware updates ship with inadequate verification.

### Automotive

Tesla autopilot, Toyota unintended acceleration, Jeep Cherokee remote hack - software controls life-critical functions.

**Why?** Speed to market prioritized over exhaustive safety testing.

### Aviation

Boeing 737 MAX (MCAS system), Airbus A320 flight control bugs - software can override pilot input.

**Why?** Certification processes lag behind software complexity.

### Industrial Control

Nuclear plants, chemical facilities, power grids - increasingly software-controlled.

**Why?** Legacy systems with no formal verification now connected to networks.

---

## The Core Principle

### Therac-25 Teaches Us

**You cannot test your way to safety in complex systems.**

- Testing found 0 bugs before deployment
- 6 patients suffered before bug was confirmed
- Bug was reproducible but only under specific timing

**You need:**
1. **Static analysis** - Find races before runtime
2. **Type systems** - Make dangerous patterns unwritable
3. **Formal verification** - Prove safety properties hold
4. **Defense in depth** - Software + hardware + procedures

### The Compiler's Role

A defensive compiler is **the first line of defense**:
- Catches races at compile time
- Enforces synchronization requirements
- Blocks safety violations before code runs
- Makes implicit assumptions explicit

**The Therac-25 would not have compiled** under defensive compiler design.

**The patients would have lived.**

---

## Technical Exercise: Fix the Bug

### Challenge

Here's simplified Therac-25 vulnerable code:

```c
// Shared state
volatile int turntable_position = OUT;
volatile int beam_intensity = 0;

// Task 1: Keyboard handler
void keyboard_interrupt() {
    int mode = read_keyboard();
    turntable_position = mode;  // RACE HERE
}

// Task 2: Beam controller  
void beam_control_loop() {
    while (1) {
        if (turntable_position == IN) {
            beam_intensity = XRAY_POWER;  // RACE HERE
        } else {
            beam_intensity = ELECTRON_POWER;
        }
        fire_beam(beam_intensity);
        delay_ms(100);
    }
}
```

### Your Task

1. Identify all race conditions
2. Add synchronization to make it safe
3. Explain what defensive compiler features would catch the original bug

### Solution

```c
// Add mutex for atomic updates
pthread_mutex_t config_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int position;
    int intensity;
} BeamConfig;

BeamConfig config = {OUT, ELECTRON_POWER};

// Task 1: Keyboard handler (FIXED)
void keyboard_interrupt() {
    int mode = read_keyboard();
    
    pthread_mutex_lock(&config_lock);
    config.position = mode;
    config.intensity = (mode == IN) ? XRAY_POWER : ELECTRON_POWER;
    pthread_mutex_unlock(&config_lock);
    // Now position and intensity updated atomically
}

// Task 2: Beam controller (FIXED)
void beam_control_loop() {
    while (1) {
        BeamConfig local_config;
        
        pthread_mutex_lock(&config_lock);
        local_config = config;  // Atomic read
        pthread_mutex_unlock(&config_lock);
        
        // Use local copy (consistent state)
        fire_beam(local_config.intensity);
        delay_ms(100);
    }
}
```

### What Defensive Compiler Catches

```
ERROR: Shared variable access without lock

Variable: turntable_position
  Writer: keyboard_interrupt() [line 8] - NO LOCK
  Reader: beam_control_loop() [line 17] - NO LOCK
  
RISK: Race condition - reader may see partial update

Variable: beam_intensity  
  Writer: beam_control_loop() [line 18, 20] - NO LOCK
  Writer: keyboard_interrupt() (via side effect) - NO LOCK
  Reader: fire_beam() [line 22] - NO LOCK
  
RISK: Inconsistent state between position and intensity

REQUIRED: Protect shared state with mutex or atomic operations
```

---

## Further Reading

### Primary Sources

1. **Leveson & Turner (1993)** - "An Investigation of the Therac-25 Accidents"  
   *The definitive technical paper. Required reading.*

2. **FDA Recall Reports** - Official incident documentation

3. **AECL Internal Memos** - Released during litigation (reveals denial process)

### Modern Analysis

1. **Nancy Leveson** - *Safeware: System Safety and Computers*  
   Chapter on Therac-25 with broader safety engineering context

2. **IEEE Software** - "Lessons from Therac-25" (multiple retrospectives)

3. **ACM SIGSOFT** - Software engineering ethics case studies

### Related Incidents

- **Patriot Missile Failure** (Dhahran, 1991) - Floating point error killed 28 soldiers
- **Mars Climate Orbiter** (1999) - Unit conversion error, $327 million lost  
- **Cluster Rocket Explosion** (1996) - Integer overflow, $370 million lost

---

## Conclusion: The Human Cost of Compiler Permissiveness

**The Therac-25 killed people because:**
- Compilers accepted unsafe concurrent code
- Testing couldn't find timing-dependent bugs
- Errors were cryptic, not explanatory
- Software replaced hardware safety with no verification

**Defensive compilers prevent this by:**
- **Blocking** unsafe concurrent patterns
- **Requiring** explicit synchronization
- **Enforcing** safety annotations
- **Generating** meaningful error messages

Three people died because a compiler said "this code is fine."

**Your job as a compiler designer: Make sure that never happens again.**

---

**Return to:** [Module 0 - Why This Matters](../modules/00_why_this_matters.md)

**Next Case Study:** [Ariane 5 Flight 501 - Integer Overflow Explosion](ariane5_technical.md)

**Practice Exercises:** [Race Condition Detection Lab](../exercises/race_condition_detection.md)
