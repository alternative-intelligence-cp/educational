# Toyota Unintended Acceleration - Software Analysis

**Incident Period:** 2002-2010  
**Casualties:** 89 confirmed deaths, ~1,000 injuries  
**Vehicles Recalled:** 9+ million  
**Financial Cost:** $3+ billion (recalls, settlements, fines)  
**Root Causes:** Multiple software defects in engine control  
**Preventable:** Yes, with defensive compiler design and formal verification

---

## Overview: When Your Car Becomes Your Enemy

### The Nightmare Scenario

You're driving on the highway. You press the brake pedal. **The car accelerates instead.**

You:
- Slam the brake harder - **Car accelerates more**
- Shift to neutral - **Transmission doesn't respond**
- Turn off the ignition - **Engine keeps running**
- Scream for help - **Car is in control, not you**

**This happened to hundreds of people.**

**89 of them died.**

---

## What Toyota Claimed

### Official Position (2002-2009)

*"There is no software problem. These incidents are caused by:"*

1. **Driver error** - "Drivers pressed gas instead of brake"
2. **Floor mat entrapment** - "Mat slid under pedal"
3. **Sticky pedal mechanism** - "Pedal return spring failed"

**Toyota's message:** "It's the drivers' fault, or mechanical issues. The software is fine."

### The Recall Strategy

**2009-2010:** Toyota recalled millions of vehicles for:
- Floor mat replacement
- Pedal mechanism upgrade
- **No software changes**

**Message to public:** "We've fixed it."

**Reality:** Software defects remained.

---

## The Bookout v. Toyota Trial

### The Case That Broke It Open

**Plaintiffs:** Jean Bookout (severely injured) and Barbara Schwarz (killed)  
**Incident:** 2007 Camry suddenly accelerated, crashed into embankment  
**Toyota's defense:** "Driver error. Our software is flawless."  
**Plaintiff's strategy:** "Let's see that software then."

### The Expert Witnesses

**NASA Analysis (2011):**
- Examined Toyota code for "single point failures"
- Found none (by design, Toyota used redundancy)
- **Conclusion:** "No smoking gun in limited analysis"
- **Critical limitation:** NASA only had 10 months, limited access to source code

**Michael Barr Analysis (2013):**
- Embedded systems expert, 25+ years experience
- **18 months** analyzing Toyota Electronic Throttle Control System (ETCS)
- **Full source code access** (under protective order)
- Found **systematic safety failures**

### The Verdict

**October 2013:** Jury finds Toyota liable

**Reasoning:** Software defects made unintended acceleration possible

**Settlement:** Undisclosed (estimated $10+ million)

**Impact:** Opened floodgates - Toyota eventually paid **$3+ billion** across all cases

---

## The Software Architecture

### Electronic Throttle Control System (ETCS)

**Traditional throttle (pre-2002):**
- Cable from gas pedal to throttle body
- Press pedal → Cable pulls → Throttle opens → Engine accelerates
- **Simple, mechanical, predictable**

**Electronic throttle (2002+):**
- Sensor on gas pedal sends signal to ECU (Engine Control Unit)
- Software calculates desired throttle position
- Motor opens throttle to calculated position
- **Complex, software-controlled, unpredictable**

### The Safety-Critical Chain

```
Pedal Sensor → ECU Software → Throttle Motor
     ↓              ↓              ↓
  Position    Calculate duty   Engine power
   reading      cycle for        output
                PWM signal
```

**If ANY part of this chain fails, engine can accelerate uncontrollably.**

### The Redundancy Architecture

Toyota used **two CPUs** for safety:

**Main CPU:**
- Reads pedal position
- Calculates throttle command
- Sends PWM signal to throttle motor

**Monitor CPU:**
- Checks Main CPU's work
- Should shut down engine if Main CPU fails
- **Critical assumption:** Monitor CPU detects all failures

**This assumption was wrong.**

---

## The Software Defects

### Finding 1: Stack Overflow

**Michael Barr's analysis:**

> *"The ETCS code contains a stack overflow vulnerability. The stack is sized at 4KB for the entire program. Analysis shows maximum stack depth under normal conditions is ~3.8KB, leaving only ~200 bytes margin."*

#### What This Means

**Stack usage:**
- Normal operation: ~3,800 bytes
- Available: 4,096 bytes
- **Safety margin: 296 bytes**

**Problem:** Single unexpected event (ISR nesting, deep function call) could exceed stack

**Consequence of stack overflow:**
```
Normal stack:
[Return addresses] [Local variables] [Function params]
← Safe region (4KB) →

Overflow:
[Return addresses] [Local variables] [Corrupted data]
                                      ↑
                        Stack overflow corrupts adjacent memory
```

**What gets corrupted:**
- Global variables
- Task control blocks
- **Throttle position variables**

**Result:** Throttle commanded to random position (could be wide open)

#### The Evidence

**Barr found:**
- Stack depth calculation: **Never performed by Toyota**
- Worst-case analysis: **Not done**
- Stack overflow protection: **None**

**Toyota's response:** "We tested it extensively."

**Barr's counter:** "Testing doesn't find worst-case scenarios. Static analysis does."

---

### Finding 2: Race Conditions

**Critical variable:** `TargetThrottlePosition`

**Accessed by:**
- Main calculation task (writes)
- Watchdog monitoring task (reads)
- Fault detection task (reads)
- Diagnostic task (reads/writes)

**Synchronization:** **NONE**

#### The Vulnerable Pattern

```c
// Simplified pseudocode of actual Toyota code structure

// Global, shared across tasks
volatile uint16_t TargetThrottlePosition = 0;

// TASK 1: Main throttle control (runs every 8ms)
void ThrottleControlTask() {
    uint16_t pedal = ReadPedalSensor();
    uint16_t calculated = CalculateThrottle(pedal);
    
    // NO LOCK
    TargetThrottlePosition = calculated;  // WRITE
    
    SendPWMSignal(TargetThrottlePosition);
}

// TASK 2: Watchdog monitor (runs every 10ms)
void WatchdogTask() {
    uint16_t pedal = ReadPedalSensor();
    
    // NO LOCK
    uint16_t current_target = TargetThrottlePosition;  // READ
    
    if (current_target > MAX_SAFE_THROTTLE && pedal < 10) {
        // Throttle high but pedal low - FAULT
        ShutdownEngine();
    }
}
```

#### The Race Condition

**Normal sequence (safe):**
1. ThrottleControlTask: Calculate throttle = 80
2. ThrottleControlTask: Write `TargetThrottlePosition = 80`
3. WatchdogTask: Read `TargetThrottlePosition` → 80
4. WatchdogTask: Verify 80 matches pedal → OK

**Race condition (unsafe):**
1. ThrottleControlTask: Calculate throttle = 80
2. **Interrupt occurs** (timing event, CAN bus message, sensor update)
3. WatchdogTask: Read `TargetThrottlePosition` → **Still old value (30)**
4. ThrottleControlTask: Resume, write `TargetThrottlePosition = 80`
5. WatchdogTask: Sees 30, thinks everything is fine
6. **Throttle actually commanded to 80, but watchdog thinks 30**

**If throttle is erroneously high:** Watchdog doesn't catch it because it read stale data

---

### Finding 3: Disabled Watchdog

**Purpose of watchdog:** Detect software lock-ups, reset system if CPU hangs

**Toyota's implementation:**

```c
// Watchdog can be DISABLED by software
void DisableWatchdog() {
    WATCHDOG_CONTROL_REG = 0x00;  // Turns off watchdog
}

// Found in code: Watchdog disabled during certain operations
if (DiagnosticModeActive) {
    DisableWatchdog();
    RunDiagnostics();
    // Re-enable... eventually... maybe...
}
```

**Problem:** If software crashes **while watchdog is disabled**, system never recovers

**Barr's finding:** Watchdog can remain disabled if certain fault conditions occur during diagnostic mode

**Toyota's justification:** "We need to disable it for dealer diagnostics"

**Safety principle violated:** **Critical safety system should never be software-disabled**

---

### Finding 4: Single-Bit Flip Vulnerability

**Critical variable:** `ThrottleOverride` flag

**Size:** 1 byte (8 bits)

**Values:**
- `0x00` = Normal operation
- `0xFF` = Override (ignore pedal, close throttle)

**Problem:** No redundancy, no error detection

#### The Bit Flip Scenario

**Normal memory:**
```
ThrottleOverride = 0x00 (binary: 00000000)
```

**Cosmic ray / EMI / Memory error flips one bit:**
```
Before: 00000000 (0x00 - Normal)
After:  00000001 (0x01 - UNDEFINED)
```

**Code logic:**
```c
if (ThrottleOverride == 0xFF) {
    CloseThrottle();
} else {
    // Normal operation - use calculated throttle
}
```

**If bit flips from 0x00 to anything except 0xFF:** Code takes "normal" path

**But:** Variable is corrupted. Undefined behavior follows.

**Safe pattern (never implemented):**

```c
// Use redundant encoding
typedef enum {
    NORMAL = 0xA5A5,     // Pattern 1
    OVERRIDE = 0x5A5A    // Pattern 2
} ThrottleMode;

ThrottleMode mode_primary = NORMAL;
ThrottleMode mode_redundant = NORMAL;

if (mode_primary == OVERRIDE && mode_redundant == OVERRIDE) {
    CloseThrottle();
} else if (mode_primary == NORMAL && mode_redundant == NORMAL) {
    NormalOperation();
} else {
    // MISMATCH - corruption detected
    EnterFailSafe();
}
```

**Toyota's implementation:** No redundancy, no error detection

---

### Finding 5: Unprotected Memory Access

**Critical throttle variables stored in RAM with no protection:**

```
Memory Layout (simplified):
[Code] [Stack] [Global Variables] [Throttle Data] [Heap]
                                       ↑
                              No memory protection
```

**Vulnerabilities:**
- Stack overflow can corrupt throttle data
- Pointer bugs can write to throttle variables
- No memory protection unit (MPU) configured
- No bounds checking on array accesses

**Example of vulnerable pattern found in code:**

```c
// Array of sensor readings
uint16_t SensorHistory[16];

// Unchecked index
void UpdateSensorHistory(uint8_t index, uint16_t value) {
    // NO BOUNDS CHECK
    SensorHistory[index] = value;  // If index > 15, overwrites adjacent memory
}

// Adjacent in memory:
uint16_t ThrottlePosition;  // If SensorHistory overflows, this gets corrupted
```

---

### Finding 6: Non-Deterministic Execution

**Real-Time OS requirement:** Task execution must be predictable and verifiable

**Toyota's implementation:**
- Multiple priority levels
- Preemptive scheduling
- **NO worst-case execution time analysis**

**Problem:** Can't prove critical tasks meet deadlines

**Example scenario:**

```
Task priorities:
  1 (Highest): Throttle Control (must run every 8ms)
  2: Diagnostics
  3: CAN Bus communication
  4 (Lowest): Logging

Normal case:
  Throttle runs every 8ms - OK

Worst case (not analyzed):
  CAN bus message arrives (priority 3)
  Diagnostic triggered (priority 2)
  Both preempt throttle task
  Throttle delayed by 3ms
  Next cycle starts before previous completes
  STACK OVERFLOW or MISSED DEADLINE
```

**Toyota never analyzed worst-case scenarios.**

---

## What a Defensive Compiler Would Catch

### 1. Stack Overflow Prevention

```
ERROR: Insufficient stack space allocation

Analysis:
  Stack size: 4096 bytes
  Maximum usage (computed): 3847 bytes
  Safety margin: 249 bytes
  
REQUIREMENT: Safety-critical systems need 2x safety margin minimum
  Recommended stack size: 8192 bytes
  
Call chain contributing to max depth:
  main() → ProcessThrottle() → CalculateTorque() → 
  InterpolateTable() → ValidateRange() → LogError()
  
Stack usage:
  main: 64 bytes
  ProcessThrottle: 128 bytes  
  CalculateTorque: 892 bytes (large local arrays)
  InterpolateTable: 256 bytes
  ValidateRange: 64 bytes
  LogError: 2443 bytes (sprintf buffer - EXCESSIVE)
  
COMPILATION BLOCKED

RECOMMENDED FIX:
  1. Increase stack size to 8192 bytes, OR
  2. Reduce LogError buffer allocation (use static buffer), OR
  3. Prove maximum interrupt nesting depth < N
```

### 2. Race Condition Detection

```
ERROR: Unsynchronized shared variable access

Variable: TargetThrottlePosition (address 0x40002040)
  Type: volatile uint16_t
  Classification: SAFETY_CRITICAL
  
CONCURRENT ACCESSES:
  
  WRITER: ThrottleControlTask (priority 1)
    Location: throttle.c:847
    Access: WRITE, no lock held
    Frequency: Every 8ms
    
  READER: WatchdogTask (priority 2)
    Location: watchdog.c:234  
    Access: READ, no lock held
    Frequency: Every 10ms
    
  READER: DiagnosticTask (priority 3)
    Location: diag.c:445
    Access: READ, no lock held
    Frequency: On-demand
    
RISK ANALYSIS:
  Tasks execute concurrently (preemptive scheduler)
  Watchdog may read partial/stale value
  Safety check bypassed if stale data read
  
CONSEQUENCE: Engine control safety monitor ineffective

COMPILATION BLOCKED

REQUIRED FIX:
  Option 1: Add mutex protection
  Option 2: Use atomic read/write operations
  Option 3: Disable preemption during update (justify latency)
```

### 3. Watchdog Disable Detection

```
ERROR: Safety-critical watchdog disabled in software

Location: diag.c:892
  DisableWatchdog();

POLICY VIOLATION: Watchdog must not be software-disabled

ANALYSIS:
  Watchdog protects against: CPU hang, infinite loop, memory corruption
  Disabling watchdog removes protection
  If system hangs while disabled: NO RECOVERY POSSIBLE
  
In automotive safety (ISO 26262):
  ASIL-D systems require independent watchdog
  Watchdog must be hardware-only or provably fault-tolerant
  
COMPILATION BLOCKED

ALLOWED ALTERNATIVES:
  1. Remove DisableWatchdog() calls
  2. Use hardware watchdog that cannot be disabled
  3. Provide formal proof that all paths re-enable watchdog within 100ms
```

### 4. Single-Point-of-Failure Detection

```
ERROR: Safety-critical variable lacks redundancy

Variable: ThrottleOverride
  Type: uint8_t (single bit pattern)
  Classification: SAFETY_CRITICAL
  Redundancy: NONE
  
RISK: Single bit flip causes undefined behavior
  Cosmic ray, EMI, memory error → corruption
  No detection mechanism
  Undefined state → unpredictable throttle control
  
ISO 26262 REQUIREMENT (ASIL-D):
  Safety variables must use redundant encoding
  
COMPILATION BLOCKED

REQUIRED FIX: Use redundant storage

  typedef struct {
      uint16_t value_primary;
      uint16_t value_redundant;
      uint16_t checksum;
  } SafeThrottleOverride;
  
  SafeThrottleOverride override = {
      .value_primary = NORMAL_MODE,
      .value_redundant = ~NORMAL_MODE,
      .checksum = 0x5A5A
  };
  
  // Verify integrity before use
  if (override.value_primary != ~override.value_redundant) {
      EnterFailSafe();  // Corruption detected
  }
```

### 5. Memory Protection Configuration

```
ERROR: Safety-critical variables in unprotected memory region

Variables in unprotected RAM:
  - TargetThrottlePosition (0x40002040)
  - ThrottleOverride (0x40002042)
  - SafetyChecksum (0x40002044)
  
RISK: Stack overflow or pointer error can corrupt throttle control

REQUIREMENT: Safety-critical variables must be in MPU-protected region

COMPILATION BLOCKED

REQUIRED FIX: Configure Memory Protection Unit (MPU)

  // MPU Region for safety variables
  MPU_Region_Safety = {
      .base_address = 0x40002000,
      .size = 1024 bytes,
      .permissions = READ_ONLY (except from authorized tasks),
      .fault_on_violation = true
  };
  
  // Compiler verifies all safety variable accesses go through MPU
```

---

## The Investigation's Key Finding

### Michael Barr's Testimony

From trial transcript:

> *"Toyota's software development process was deficient in multiple ways:*
>
> *1. No stack depth analysis performed  
> 2. No worst-case execution time analysis  
> 3. Race conditions on safety-critical variables  
> 4. Watchdog can be disabled by software  
> 5. Single-point failures in throttle control path  
> 6. Violations of NASA MISRA-C coding standards: **11,000+ rule violations***
>
> *In my opinion, this software is not fit for use in a safety-critical system."*

### The 11,000 Violations

**MISRA-C:** Motor Industry Software Reliability Association C coding standard

**Purpose:** Prevent common embedded software bugs

**Toyota's compliance:** **11,000+ violations** in ETCS code

**Examples of violated rules:**

| Rule | Violation Found | Consequence |
|------|----------------|-------------|
| **Rule 12.4** | Insufficient stack space | Stack overflow |
| **Rule 16.2** | Unprotected shared access | Race conditions |
| **Rule 21.1** | Array index unchecked | Memory corruption |
| **Rule 1.2** | Dead code present | 35% unreachable code |

**Toyota's response:** "Those are just guidelines."

**ISO 26262 response:** "Those are requirements for ASIL-D systems."

---

## The Smoking Gun: Task Death and Resurrection

### Finding 7: "Zombie Tasks"

**Most damning finding:** Tasks can die and resurrect without detection

**What Barr found:**

```c
// Task control block corruption possible
typedef struct {
    uint32_t stack_pointer;
    uint8_t priority;
    uint8_t state;  // RUNNING, READY, BLOCKED
    // ...
} TaskControlBlock;

TaskControlBlock tasks[MAX_TASKS];

// Stack overflow can corrupt this array
// If throttle task's TCB corrupted:
//   - Task appears "dead" to scheduler
//   - Watchdog task thinks throttle task is running
//   - Monitor CPU doesn't detect failure
//   - Throttle commands stop updating
//   - Last commanded throttle position HELD
//   - If last command was "accelerate" → STUCK ACCELERATING
```

### The Scenario

**Normal operation:**
1. Driver presses pedal
2. Throttle task calculates 60% throttle
3. Throttle opens to 60%
4. Driver releases pedal
5. Throttle task calculates 0% throttle
6. Throttle closes

**With task death:**
1. Driver presses pedal
2. Throttle task calculates 60% throttle
3. Throttle opens to 60%
4. **Stack overflow kills throttle task**
5. **Last command (60%) held by hardware**
6. Driver releases pedal
7. **Throttle task dead, can't send new command**
8. **Throttle stays at 60%**
9. **Car continues accelerating**

**Watchdog doesn't catch it because:**
- Task control block corrupted
- Monitor CPU reads corrupted data
- Appears as if task is still running normally

---

## The Timeline of Denial

### 2002-2007: Incidents Reported

- Hundreds of complaints to NHTSA (National Highway Traffic Safety Administration)
- Toyota's response: "Driver error"
- Internal memos (discovered later): **Engineers aware of software issues**

### 2007-2009: Deaths Mount

- Media coverage increases
- Toyota blames floor mats
- **Recall 4 million vehicles for floor mat replacement**
- **No software changes made**

### 2009: The Saylor Family Tragedy

**August 28, 2009:**
- California Highway Patrol Officer Mark Saylor driving with family
- 2009 Lexus ES suddenly accelerates
- Reaches 120+ mph
- **Saylor calls 911:** "We're in a Lexus... we're going north on 125 and our accelerator is stuck... hold on... hold on and pray... pray..."
- Car crashes, all four occupants killed
- **911 recording goes viral**

**Public outcry forces action**

### 2010: Toyota Recalls 9 Million Vehicles

- Initially for "sticky pedal" mechanical issue
- **Still claims software is fine**

### 2011: NASA Investigation

- NHTSA commissions NASA to examine software
- Limited scope: "Find single-point failures"
- **Limited time:** 10 months
- **Limited access:** Partial source code
- **Conclusion:** "No smoking gun found"
- **Toyota claims vindication:** "See, NASA says software is fine"

### 2013: Bookout Trial Reveals Truth

- Michael Barr gets **18 months** and **full source code access**
- Finds **systemic safety failures**
- Jury finds Toyota liable
- **Truth finally public**

### 2014: Toyota Settles with Department of Justice

- **$1.2 billion fine** for misleading consumers
- Admits software defects existed
- Largest criminal penalty in automotive history at the time

---

## The Economic Impact

### Direct Costs

| Cost Category | Amount |
|--------------|--------|
| Vehicle recalls | $2+ billion |
| Legal settlements | $1+ billion |
| DOJ criminal fine | $1.2 billion |
| Lost sales (reputation) | Estimated $5+ billion |
| **Total** | **~$10 billion** |

### Human Cost

| Impact | Count |
|--------|-------|
| Deaths (confirmed linked) | 89 |
| Injuries | ~1,000 |
| Complaints to NHTSA | 9,000+ |
| Families destroyed | Incalculable |

---

## What Defensive Compilation Would Have Prevented

### At Compile Time

**Blocked compilation due to:**
1. ✅ Insufficient stack space
2. ✅ Race conditions on shared variables
3. ✅ Watchdog disable calls
4. ✅ Lack of memory protection
5. ✅ Missing redundancy on safety variables

**Estimated time to fix with compiler errors guiding:** 2-4 weeks

### At Runtime

**Prevented incidents via:**
1. ✅ Stack overflow protection (MPU catches overflow)
2. ✅ Atomic operations prevent races
3. ✅ Hardware watchdog (cannot disable)
4. ✅ Redundant safety checks

### The Math

**Cost of defensive compiler implementation:** ~$1-2 million (compiler development, training)

**Cost of software defects:** ~$10 billion + 89 lives

**ROI:** 5000:1 (ignoring human cost)

---

## Lessons for Compiler Designers

### 1. "Testing Is Sufficient" Is a Lie

**Toyota tested extensively:**
- Thousands of hours
- Multiple test scenarios
- Real-world driving conditions

**Toyota didn't find:**
- Stack overflow (requires worst-case analysis)
- Race conditions (requires timing analysis)
- Bit flip vulnerabilities (requires fault injection)

**Conclusion:** Testing finds bugs you think to test for. Static analysis finds bugs you don't imagine.

### 2. Complexity Is the Enemy

**ETCS code statistics:**
- ~80,000 lines of code
- ~11,000 MISRA-C violations
- 35% unreachable code (dead code)
- Cyclomatic complexity: 146 (should be < 10 for safety-critical)

**Defensive compilation principle:** **Reject complex code**

```
ERROR: Function complexity exceeds safety threshold

Function: CalculateThrottlePosition
  Lines: 847
  Cyclomatic complexity: 67
  Maximum for ASIL-D: 10
  
REQUIREMENT: Safety-critical functions must be simple and verifiable

RECOMMENDATION: Decompose into smaller functions
```

### 3. Safety Cannot Be Bolted On

**Toyota's approach:**
- Write code for performance/features
- Add safety checks afterwards
- "It'll be fine"

**Result:** **89 people dead**

**Defensive approach:**
- Safety requirements FIRST
- Code proves safety properties
- Compiler verifies before allowing compilation

### 4. Redundancy Without Verification Is Useless

**Toyota had redundancy:**
- Two CPUs (main + monitor)
- Two pedal sensors
- Watchdog timer

**Toyota's redundancy failed because:**
- Monitor CPU could be fed corrupted data
- No verification that redundant systems were truly independent
- Watchdog could be disabled

**Defensive compilation requirement:** **Prove redundancy is effective**

```
ERROR: Redundant monitoring ineffective

Monitor CPU reads variable: TargetThrottlePosition
Main CPU writes variable: TargetThrottlePosition

BOTH ACCESS SAME MEMORY LOCATION
  Corruption affects both CPUs
  Redundancy violated
  
REQUIREMENT: True redundancy requires independent state

REQUIRED FIX:
  Main CPU: Calculate and send command
  Monitor CPU: INDEPENDENTLY calculate expected command
  Compare results: If mismatch → Fail-safe
  
Current implementation: Monitor reads Main's result (NOT INDEPENDENT)
```

---

## The Regulatory Response

### Before Toyota

**Automotive software regulation:** Minimal
- No mandatory coding standards
- No required static analysis
- "Trust the manufacturer"

### After Toyota

**ISO 26262 (Functional Safety):**
- Mandatory for all automotive safety systems
- Defines ASIL levels (A-D, D = highest criticality)
- ASIL-D requirements:
  - ✅ MISRA-C compliance mandatory
  - ✅ Static analysis required
  - ✅ Formal verification for critical functions
  - ✅ Independent safety assessment
  - ✅ Freedom from interference (no task can corrupt another)

**Enforcement:**
- Manufacturers must demonstrate compliance
- Third-party audits required
- Non-compliance blocks vehicle certification

---

## Modern Parallels: Where This Still Happens

### Tesla Autopilot

- Software controls critical vehicle functions
- Over-the-air updates (can introduce bugs remotely)
- Multiple crashes attributed to software behavior
- **Same pattern:** Complex software, safety-critical control

### Medical Infusion Pumps

- Software controls drug delivery rates
- Bugs can cause over/under-dosing
- FDA recalls due to software defects
- **Same pattern:** Safety not proven, bugs found in production

### Industrial Robots

- Software controls heavy machinery
- Workers injured/killed by robot malfunctions
- Root cause: Software bugs in motion planning
- **Same pattern:** Insufficient safety verification

---

## Technical Exercise: Fix Toyota's Stack Overflow

### The Challenge

Toyota's stack overflow scenario:

```c
// Stack: 4096 bytes total
// Current usage: ~3847 bytes (worst case)
// Safety margin: 249 bytes (INSUFFICIENT)

// Deep call chain
void main() {  // 64 bytes
    ProcessThrottle();
}

void ProcessThrottle() {  // 128 bytes
    CalculateTorque();
}

void CalculateTorque() {  // 892 bytes - large local arrays
    float LookupTable[200];  // 800 bytes!
    // ...
    InterpolateTable(LookupTable);
}

void InterpolateTable(float* table) {  // 256 bytes
    ValidateRange();
}

void ValidateRange() {  // 64 bytes
    if (error) LogError();
}

void LogError() {  // 2443 bytes - PROBLEM!
    char buffer[2048];  // sprintf buffer
    char timestamp[256];
    char details[128];
    sprintf(buffer, "Error at %s: %s", timestamp, details);
    WriteToLog(buffer);
}
```

### Your Task

1. Identify the stack usage problems
2. Refactor to reduce stack depth
3. Show how defensive compiler would catch this

### Solution

```c
// SOLUTION 1: Move large buffers to static storage

// BEFORE: 2048 bytes on stack (EACH CALL)
void LogError() {
    char buffer[2048];
    sprintf(buffer, "Error...");
    WriteToLog(buffer);
}

// AFTER: 2048 bytes in .bss section (SHARED)
void LogError() {
    static char buffer[2048];  // Shared across all calls
    sprintf(buffer, "Error...");
    WriteToLog(buffer);
}
// Stack savings: 2048 bytes → ~32 bytes


// SOLUTION 2: Move lookup table to ROM

// BEFORE: 800 bytes on stack (per call)
void CalculateTorque() {
    float LookupTable[200];
    InitializeLookupTable(LookupTable);
    InterpolateTable(LookupTable);
}

// AFTER: 800 bytes in .rodata section (const)
static const float LookupTable[200] = {
    // Pre-computed values
    0.0, 0.1, 0.2, /* ... */
};

void CalculateTorque() {
    InterpolateTable(LookupTable);  // Pass pointer, not copy
}
// Stack savings: 800 bytes → 4 bytes (pointer)


// SOLUTION 3: What defensive compiler enforces

#pragma STACK_USAGE_ANALYSIS

// Compiler computes:
//   main: 64 bytes
//   ProcessThrottle: 128 bytes  
//   CalculateTorque: 892 bytes
//   InterpolateTable: 256 bytes
//   ValidateRange: 64 bytes
//   LogError: 2443 bytes
//   TOTAL: 3847 bytes
//
// Stack size: 4096 bytes
// Margin: 249 bytes (6%)
//
// ERROR: Safety margin < 50% required for ASIL-D
//
// RECOMMENDATION:
//   Option 1: Increase stack to 8192 bytes
//   Option 2: Reduce LogError() allocation (see above)
//   Option 3: Reduce CalculateTorque() allocation (see above)
//
// After fixes:
//   TOTAL: 576 bytes
//   Stack: 4096 bytes
//   Margin: 3520 bytes (86%)  ✓ SAFE
```

---

## Conclusion: When Software Kills

The Toyota unintended acceleration defects killed **89 people** because:

1. **Stack overflow** was possible but unanalyzed
2. **Race conditions** on safety-critical variables went undetected
3. **Watchdog** could be disabled, removing safety net
4. **Single-point failures** lacked redundancy
5. **Memory protection** was not configured
6. **Testing** couldn't find timing-dependent bugs

**Every single one of these issues would be caught by defensive compilation.**

### The Core Lesson

**You cannot debug your way to safety.**

**You cannot test your way to safety.**

**You must PROVE safety, at compile time.**

89 people are dead because Toyota didn't prove their software was safe.

**Your job as a compiler designer: Make sure code that can't be proven safe doesn't compile.**

---

**Return to:** [Module 0 - Why This Matters](../modules/00_why_this_matters.md)

**Previous:** [Ariane 5 Flight 501 Technical Analysis](ariane5_technical.md)

**Next:** [Module 1 - Type System Fundamentals](../modules/01_type_system_fundamentals.md)

**Practice:** [Concurrency Safety Lab](../exercises/concurrency_safety.md)

**Further Reading:**
- Michael Barr's full expert report (Toyota Bookout trial)
- ISO 26262 Functional Safety Standard
- MISRA-C:2012 Coding Guidelines
- NASA Software Safety Guidebook
