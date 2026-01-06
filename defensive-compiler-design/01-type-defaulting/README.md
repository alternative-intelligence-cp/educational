# Module 01: The i32 Default Catastrophe
## ARIA-026 Bug #1 - When "Close Enough" Kills

### The Bug Story

**Date:** January 1, 2026  
**Severity:** CATASTROPHIC  
**Found By:** Gemini AI audit (first 100% accurate audit after 2 weeks of methodology refinement)  
**Context:** Type mapping in LLVM code generation

### The Vulnerable Code

```cpp
llvm::Type* getLLVMType(Type* type) {
    // ... handle some known types ...
    
    if (!type->isPrimitive()) {
        // Unknown non-primitive type? Just default to i32!
        return llvm::Type::getInt32Ty(context);  // 💀 CATASTROPHIC BUG
    }
    
    // ... more handling ...
    
    // Don't know what this is? Default to i32!
    return llvm::Type::getInt32Ty(context);  // 💀 ALSO CATASTROPHIC
}
```

**What Could Go Wrong?** _(Narrator: Everything)_

### The Attack Vector

Developer writes perfectly valid Aria code:

```aria
struct SensorReading {
    timestamp: int64,
    value: fix256      // Q128.128 fixed-point (256 bits total)
}

func read_force_sensor() -> SensorReading {
    // ... read from sensor ...
}

func apply_safe_grip(reading: SensorReading) {
    // Apply force up to reading.value (should be 10 Newtons)
    motor.apply_force(reading.value);
}
```

### What the Compiler Does (With Bug)

1. **Struct declaration:** Compiler sees `SensorReading` - it's not primitive!
2. **Type mapping:** Calls `getLLVMType()` to get LLVM type
3. **💀 DEFAULT:** Returns `i32` (4 bytes) instead of proper struct layout
4. **Memory allocation:** Allocates 4 bytes instead of required 16+ bytes
5. **Stack corruption:** Writing sensor data overflows the 4-byte allocation
6. **Memory scrambling:** Adjacent stack variables (including return addresses) corrupted

### The Physical Consequence

**Scenario:** Robot assisting child with limited mobility to hold cup of juice

```
Step 1: Sensor reads safe grip force = 10 Newtons
Step 2: Compiler maps struct → i32 (WRONG SIZE)
Step 3: Stack allocation only 4 bytes
Step 4: Sensor write overflows into motor control variables
Step 5: MAX_FORCE_LIMIT (adjacent on stack) overwritten with garbage
Step 6: Motor applies 1000N instead of 10N
Step 7: Child's hand crushed

Game over for entire project.
```

**This is not hypothetical.** With sufficient memory corruption, this COULD happen.

### Why Silent Defaults Are Evil

The compiler had choices:

1. ❌ **Default to i32** - "I don't know, so I'll guess"
2. ✅ **Throw exception** - "I don't know, and that's a compiler bug"

**Silent default** = Compiler lies about understanding your code  
**Hard failure** = Compiler admits ignorance honestly

### The Defensive Fix

```cpp
llvm::Type* getLLVMType(Type* type) {
    // ... handle known types ...
    
    if (!type->isPrimitive()) {
        if (type->isPointer()) {
            return llvm::PointerType::get(context, 0);
        }
        
        // FAIL HARD - No defaults!
        throw std::runtime_error(
            "ICE: Code generation encountered unmapped non-primitive type kind: " + 
            std::to_string(static_cast<int>(type->getKind())) + 
            ". This is a compiler bug - all types must be explicitly mapped."
        );
    }
    
    // ... handle more known types ...
    
    // NO FALLBACK DEFAULT - If we get here, it's a compiler bug
    throw std::runtime_error(
        "ICE: Unmapped type in getLLVMType - compiler bug, not user error"
    );
}
```

### The Philosophy

**From Construction Work:**

You're framing a wall. You don't know if this stud should be 16" or 24" on center.

**Bad approach:** "I'll just guess 16" - it's the most common"  
**Result:** Wall doesn't line up with roof trusses, entire frame wrong

**Good approach:** "Stop. Measure. Check plans. Verify."  
**Result:** Wall is exactly right, roof fits perfectly

**In compilers:** Guessing a type mapping = guessing stud spacing

### The Compound Error Problem

One bad default doesn't just corrupt one variable:

```
Allocation 1: Wrong size (4 bytes instead of 16)
  ↓
Write 1: Overflows into next variable
  ↓
Variable 2: Now contains garbage
  ↓
Calculation 2: Uses garbage as input
  ↓
Result 2: Complete nonsense
  ↓
Decision 3: Based on nonsense
  ↓
Motor command 3: Executes nonsense
  ↓
PHYSICAL HARM
```

**Each step multiplies the error.** By step 7, you can't even debug it because the corruption is everywhere.

### Lab Exercise: Trigger The Bug

**Setup:**

1. Compile the vulnerable version of Aria (tag: `vulnerable-ARIA-026-1`)
2. Write code that uses a custom struct
3. Run in debugger and watch the carnage

**Files in `lab/`:**

- `vulnerable_compiler.cpp` - The buggy type mapper
- `safe_compiler.cpp` - The defensive version
- `test_struct.aria` - Code that triggers the bug
- `Makefile` - Build and run both versions
- `debug_guide.md` - How to use GDB to see the corruption

**Your Task:**

1. Compile with vulnerable version
2. Single-step through stack allocation
3. Watch it allocate 4 bytes instead of 16
4. Watch the write overflow
5. Watch adjacent variables get corrupted
6. Now fix it with defensive approach
7. Verify exception is thrown instead

### Key Takeaways

1. **Silent defaults are catastrophic** - Fail hard or don't fail at all
2. **Errors compound** - One wrong size becomes total corruption
3. **Physical consequences are real** - This isn't academic CS, this is safety
4. **Boring is armor** - Exception > guess
5. **Trust nothing** - If you don't explicitly handle it, you don't understand it

### Real-World Impact

After this fix was applied:

- ✅ Compiler now fails fast on unmapped types
- ✅ Developer gets clear error message
- ✅ Bug is caught at compile time, not runtime
- ✅ No possibility of type-confusion corruption
- ✅ Children remain uncrushed

### Questions to Ponder

1. Why did the original programmer add the i32 default?
   - Convenience? Laziness? Misunderstanding?
   
2. How many other compilers have similar bugs?
   - Most, probably. They just haven't hurt anyone yet.
   
3. Could static analysis catch this?
   - Yes! We're teaching the compiler to analyze itself.
   
4. What about gradual typing or `any` types?
   - Forbidden in safety-critical code. Explicitness over convenience.

### Next Module

[Module 02: The Pointer Confusion Bug](../02-pointer-loading/) - When returning an address instead of a value makes MAX_FORCE = 93 trillion

---

**Remember:** Every bug you ship is a ladder you'll climb later.  
**This ladder:** 2 hours of debugging, 1 comprehensive audit, 1 architectural fix.  
**The alternative:** Shipped bug, runtime crashes, maybe a lawsuit, definitely regret.

**Choose wisely.**
