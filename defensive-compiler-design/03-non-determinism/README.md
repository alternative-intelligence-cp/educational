# Module 03: The Determinism Requirement
## ARIA-026 Bug #3 - When sprintf("%lld") Broke AGI Reasoning Logs

### The Bug Story

**Date:** January 1, 2026  
**Severity:** MEDIUM-HIGH (Doesn't cause immediate harm, but violates critical requirement)  
**Found By:** Gemini AI audit  
**Context:** String conversion for template literals  
**Why It Matters:** AGI systems require bit-identical reproducibility for reasoning verification

### The Vulnerability

**Requirement:** All operations must be deterministic  
**Reality:** Used locale-dependent `sprintf()` for int→string conversion  
**Result:** Same code + same input = different output on different systems

### The Vulnerable Code

```cpp
auto int64ToString = [this](llvm::Value* intVal) -> llvm::Value* {
    // Allocate buffer for string
    llvm::Value* bufferPtr = /* ... */;
    
    // Get sprintf function
    llvm::Function* sprintfFn = module->getFunction("sprintf");
    llvm::Value* formatPtr = /* ... pointer to "%lld" format string ... */;
    
    // 💀 BUG: sprintf respects locale settings
    builder.CreateCall(sprintfFn, { bufferPtr, formatPtr, intVal });
    
    // Get string length
    llvm::Function* strlenFn = module->getFunction("strlen");
    llvm::Value* length = builder.CreateCall(strlenFFn, { bufferPtr });
    
    return length;
};
```

**What Could Go Wrong?**

### The Attack Vector (Cultural)

User sets locale to German:

```bash
export LC_ALL=de_DE.UTF-8
```

**Before (US locale):**
```
Value: 1000000
String: "1000000"
```

**After (German locale):**
```
Value: 1000000
String: "1.000.000"  # Uses period as thousands separator!
```

**Impact:** Same program, same input, different output

### The AGI Reasoning Scenario

**Context:** Teacher AI system maintaining decision log for accountability

```aria
// Log decision with timestamp and reasoning
func log_decision(timestamp: int64, reason: string) {
    let log_entry = `Decision at {timestamp}: {reason}`;
    append_to_reasoning_log(log_entry);
}

// Later: Verify decision sequence
func verify_reasoning_chain() {
    let log = read_reasoning_log();
    let hash = cryptographic_hash(log);
    
    // 💀 BUG: Hash will differ if locale changed!
    verify_signature(hash, trusted_signature);
}
```

**Failure Scenario:**

```
Day 1 (en_US locale):
  timestamp = 1735689600
  log_entry = "Decision at 1735689600: Increase grip force"
  hash = 0xDEADBEEF...

Day 2 (system updated, locale changed to en_GB):
  Same code, same timestamp
  log_entry = "Decision at 1735689600: Increase grip force"  
  hash = 0xCAFEBABE...  # DIFFERENT! (implementation detail changed)

Verification: FAIL ❌
Audit trail: BROKEN
Legal defense: IMPOSSIBLE
```

### Why Determinism Matters for AGI

**Requirement from safety standard:**

> "All operations must produce bit-identical results across:  
> - Different hardware (x86, ARM, RISC-V)  
> - Different OSes (Linux, BSD, embedded)  
> - Different locales  
> - Different library versions  
> - Different times (today vs. 10 years from now)"

**Why?**

1. **Auditability:** "Why did the robot do that?" needs reproducible analysis
2. **Verification:** Re-run decision sequence and get same result
3. **Debugging:** Time-travel debugging requires determinism
4. **Legal:** "The code would NEVER do that" requires proof through reproduction
5. **Trust:** Non-deterministic AI is unpredictable AI

### The Subtle Horror

This bug is insidious because:

- ✅ Works perfectly in testing (your locale)
- ✅ Works perfectly in production (most of the time)
- ❌ Breaks silently when environment changes
- ❌ Makes debugging impossible (can't reproduce)
- ❌ Violates fundamental safety requirement

**You won't know it's broken until you need determinism, and then it's too late.**

### The Construction Analogy

You're building a house. You use a tape measure that:

- Shows inches when it's warm
- Shows centimeters when it's cold
- Owner doesn't realize measurements are inconsistent

**Result:** Door frames in summer, windows in winter. Nothing lines up.

**Fix:** Use a tape measure that always shows the same units regardless of temperature.

### The Defensive Fix

**Replace locale-dependent library call with deterministic runtime function:**

```cpp
// BEFORE: Using sprintf (locale-dependent)
builder.CreateCall(sprintfFn, { bufferPtr, formatPtr, intVal });
llvm::Value* length = builder.CreateCall(strlenFn, { bufferPtr });

// AFTER: Using aria_int64_to_str (deterministic)
llvm::Function* toStrFn = module->getFunction("aria_int64_to_str");

// Signature: int64_t aria_int64_to_str(int64_t value, char* buffer)
// Returns length directly, no strlen needed
llvm::Value* length = builder.CreateCall(toStrFn, { intVal, bufferPtr });
```

**Runtime implementation (in C, but deterministic):**

```c
// aria_int64_to_str - Deterministic integer to string conversion
// No locale dependencies. No sprintf. Just math.
int64_t aria_int64_to_str(int64_t value, char* buffer) {
    char temp[32];  // Enough for 64-bit int
    int pos = 0;
    int is_negative = 0;
    
    // Handle zero explicitly
    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return 1;
    }
    
    // Handle negative
    if (value < 0) {
        is_negative = 1;
        value = -value;  // Make positive for digit extraction
    }
    
    // Extract digits (reverse order)
    while (value > 0) {
        temp[pos++] = '0' + (value % 10);
        value /= 10;
    }
    
    // Add negative sign if needed
    if (is_negative) {
        temp[pos++] = '-';
    }
    
    // Reverse into output buffer
    for (int i = 0; i < pos; i++) {
        buffer[i] = temp[pos - 1 - i];
    }
    
    buffer[pos] = '\0';
    return pos;  // Return length
}
```

**Why this works:**

- No locale checking
- No standard library dependencies
- Pure math (division and modulo)
- Same algorithm everywhere
- Bit-identical output guaranteed

### The Numbers

**Test: Convert 1735689600 to string across locales**

**Vulnerable version (sprintf):**
```
en_US: "1735689600"        → Hash: 0xABCD1234
de_DE: "1735689600"        → Hash: 0xABCD1234 (lucky - %lld doesn't localize)
fr_FR: "1735689600"        → Hash: 0xABCD1234 (lucky)
Custom locale: ???         → Hash: ??? (undefined behavior!)
```

Wait... `%lld` doesn't actually localize! So why is this a bug?

**The Real Problem:** We're relying on undefined behavior

- POSIX doesn't guarantee `%lld` ignores locale
- Future libc could change this
- Custom libc might behave differently  
- Embedded systems might implement it differently

**We got lucky that it works NOW. That's not good enough.**

**Defensive version (aria_int64_to_str):**
```
en_US: "1735689600"        → Hash: 0xDEADBEEF
de_DE: "1735689600"        → Hash: 0xDEADBEEF
fr_FR: "1735689600"        → Hash: 0xDEADBEEF
Custom: "1735689600"       → Hash: 0xDEADBEEF
In 10 years: "1735689600"  → Hash: 0xDEADBEEF

DETERMINISTIC ✅
```

### Lab Exercise: Break The Determinism

**Files in `lab/`:**

- `vulnerable_convert.c` - Using sprintf
- `safe_convert.c` - Using aria_int64_to_str
- `test_locales.sh` - Script to test across locales
- `verify_determinism.sh` - Hash outputs and compare
- `Makefile` - Build both versions

**Your Mission:**

1. Compile both versions
2. Run `./test_locales.sh`
3. See if you can make sprintf produce different output
4. Test on different systems (x86, ARM, etc.)
5. Hash the outputs - are they identical?
6. Understand why one is guaranteed, one is not

**Extra Evil Mode:**
- Write a malicious libc that changes %lld behavior
- See how vulnerable code breaks
- See how defensive code doesn't care

### The Flip Test

**Question:** How do you verify determinism?

**Answer:** Run it twice and compare!

```bash
# Generate output 1000 times
for i in {1..1000}; do
    ./convert 1735689600
done | sort | uniq -c

# If deterministic: 1 unique line
# If not: Multiple unique lines

# Expected output (deterministic):
#   1000 1735689600

# Bad output (non-deterministic):
#    500 1735689600
#    300 1.735.689.600
#    200 1'735'689'600
```

**If you get one unique output across 1000 runs, you're deterministic.** (Probably. Test more!)

### Real-World Impact

**Before fix:**
- Undefined behavior in string conversion
- No guarantee of reproducibility
- Audit trail potentially corrupted
- Legal liability in case of incident

**After fix:**
- Mathematically deterministic
- Reproducible across all platforms
- Audit trail reliable
- Provable behavior in court

**Time to fix:** 30 minutes (write the function)  
**Value:** Priceless (literal legal defense)

### The Philosophy

**From Construction:**

Your level has a crack in the glass. It LOOKS straight, but you can't flip-test it to verify.

**Option 1:** Use it anyway, hope for the best  
**Option 2:** Get a level you can verify

**In this case:**

**sprintf:** Can't verify determinism (depends on unknowable factors)  
**aria_int64_to_str:** Can verify by inspection (just math)

**Choose tools you can verify.**

### Questions to Ponder

1. **Why not just document "use en_US locale"?**
   - Users will forget
   - Systems will change
   - Future maintainers won't know
   - Documentation is not enforcement

2. **Is this over-engineering?**
   - Not if bit-identical reproducibility is a requirement
   - Not if you need audit trails
   - Not if legal liability exists
   - Not if predictability equals safety

3. **What other operations might be non-deterministic?**
   - Float-to-string conversion (definitely!)
   - Hash functions (maybe - depends on implementation)
   - File iteration order (yes - filesystem dependent)
   - Thread scheduling (yes - OS dependent)
   - Think of more! This is your homework.

4. **How do you test for determinism?**
   - Run same code 1000 times, compare outputs
   - Run on different machines, compare results
   - Run at different times, compare behavior
   - If any differ → not deterministic

### Next Module

[Module 04: The Recursion Depth Bomb](../04-recursion-limits/) - How unlimited expression nesting enables DoS

---

**The Pattern Continues:**

Module 01: Silent defaults → corruption  
Module 02: Incomplete cases → bypass  
Module 03: Hidden dependencies → non-determinism  

**The fix pattern:**
1. Identify external dependencies
2. Replace with controlled implementations
3. Verify behavior through testing
4. Document guarantees explicitly

**Boring is armor. Determinism is testability. Testability is confidence.**

---

## Navigation

**Learning Path:**
- ← Previous: [Module 02: Pointer Loading](../02-pointer-loading/) - Safety bypass from pointer confusion
- → Next: [Module 04: Recursion Limits](../04-recursion-limits/) - DoS from missing resource guards
- ↑ Back to: [Main README](../README.md) - Curriculum overview

**Related Resources:**
- 📜 [Manifesto](../MANIFESTO.md) - The Fourth Principle: The $8 Level (simple verifiable tools)
- 🤝 [Contributing](../CONTRIBUTING.md) - Submit your own bug stories
- 🎯 [Educational Repo](../../) - More learning resources

---

**Remember:** Hidden dependencies are trust boundaries in disguise.  
**This ladder:** 5 hours of debugging, 1 locale trace, 1 pure math replacement.  
**The alternative:** AGI reasoning diverges, audit trails broken, legal defense impossible.

**Control your dependencies. Verify your determinism.**

🛡️ **AILP Educational Initiative** 🛡️
