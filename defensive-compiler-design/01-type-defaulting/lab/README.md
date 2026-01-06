# Lab 01: Type Defaulting Stack Corruption
## Hands-On Experience With Silent Defaults

---

## Overview

In this lab, you'll:
1. Compile and run code that silently defaults unknown types to `i32`
2. Watch stack corruption happen in real-time
3. Fix the compiler to reject unknown types explicitly
4. Verify the defensive fix prevents the corruption

**Time required:** 30-45 minutes  
**Difficulty:** Intermediate  
**Tools needed:** C++ compiler, GDB (optional but recommended)

---

## Lab Files

```
lab/
├── README.md                    # This file
├── vulnerable_compiler.cpp      # Compiler with silent default bug
├── safe_compiler.cpp            # Compiler with defensive fix
├── test_program.aria            # Test program that triggers bug
├── test.sh                      # Automated test script
└── Makefile                     # Build system
```

---

## Part 1: Build The Vulnerable Compiler

```bash
make vulnerable
```

This builds a minimal compiler that demonstrates the bug:
- Parses a simple type annotation
- Maps types to LLVM types
- **SILENTLY DEFAULTS** unknown types to `i32`

---

## Part 2: Trigger The Bug

```bash
./vulnerable_compiler test_program.aria
```

**What you'll see:**
```
[CODEGEN] Compiling function: motorControl
[TYPE MAPPING] Mapping type: MotorCommand
[TYPE MAPPING] WARNING: Unknown type 'MotorCommand', defaulting to i32
[ALLOCA] Allocating 4 bytes for variable 'cmd'
[STORE] Storing 16 bytes into 4-byte allocation... 💥
Segmentation fault (core dumped)
```

**What just happened?**

1. Program declares `cmd: MotorCommand` (16-byte struct)
2. Compiler doesn't recognize `MotorCommand`
3. Compiler **silently defaults** to `i32` (4 bytes)
4. Allocates only 4 bytes on stack
5. Tries to store 16 bytes into 4-byte space
6. **Stack corruption!**

---

## Part 3: Examine The Corruption (GDB)

**Optional but highly educational:**

```bash
make vulnerable-debug
gdb ./vulnerable_compiler
(gdb) break vulnerable_compiler.cpp:67  # Line before the store
(gdb) run test_program.aria
(gdb) print sizeof(cmd)
# Shows: 4 (WRONG! Should be 16)
(gdb) print sizeof(MotorCommand)  
# Error: Unknown type (that's the problem!)
(gdb) x/20xb $rbp-20  # Examine stack before store
# Shows clean stack memory
(gdb) continue
# Crash!
(gdb) x/20xb $rbp-20  # Examine stack after attempted store
# Shows corrupted stack memory - overwrote adjacent variables!
```

**What you're seeing:**
- Stack memory before: Clean, organized
- Stack memory after: Trashed, adjacent data overwritten
- Return address might be corrupted (potential RCE!)

---

## Part 4: Understand The Vulnerable Code

**Open `vulnerable_compiler.cpp` and find this function:**

```cpp
llvm::Type* mapTypeToLLVM(const std::string& type_name) {
    if (type_name == "i32") {
        return llvm::Type::getInt32Ty(context);
    }
    else if (type_name == "i64") {
        return llvm::Type::getInt64Ty(context);
    }
    else if (type_name == "f64") {
        return llvm::Type::getDoubleTy(context);
    }
    // ... other known types ...
    
    // 💀 BUG: Silent default for unknown types!
    std::cerr << "[TYPE MAPPING] WARNING: Unknown type '" 
              << type_name << "', defaulting to i32" << std::endl;
    return llvm::Type::getInt32Ty(context);
}
```

**The problem:**
- No `MotorCommand` case
- Falls through to default
- Returns `i32` (4 bytes) instead of throwing error
- **Compiler lies about understanding the type**

---

## Part 5: Build The Safe Compiler

```bash
make safe
```

This builds the **defensive** version with the fix.

---

## Part 6: Verify The Fix

```bash
./safe_compiler test_program.aria
```

**What you'll see:**
```
[CODEGEN] Compiling function: motorControl
[TYPE MAPPING] Mapping type: MotorCommand
[ERROR] Internal Compiler Error: Unknown type 'MotorCommand'
[ERROR] This is a bug in the compiler, not your code.
[ERROR] Please report this with your source code.
Compilation failed.
```

**What just happened?**

1. Program declares `cmd: MotorCommand`
2. Compiler doesn't recognize `MotorCommand`
3. Compiler **REFUSES TO GUESS**
4. Throws explicit error with helpful message
5. **No silent corruption!**

---

## Part 7: Examine The Defensive Code

**Open `safe_compiler.cpp` and find the fixed function:**

```cpp
llvm::Type* mapTypeToLLVM(const std::string& type_name) {
    if (type_name == "i32") {
        return llvm::Type::getInt32Ty(context);
    }
    else if (type_name == "i64") {
        return llvm::Type::getInt64Ty(context);
    }
    else if (type_name == "f64") {
        return llvm::Type::getDoubleTy(context);
    }
    // ... other known types ...
    
    // ✅ FIX: Throw error for unknown types!
    std::cerr << "[ERROR] Internal Compiler Error: Unknown type '" 
              << type_name << "'" << std::endl;
    std::cerr << "[ERROR] This is a bug in the compiler, not your code." 
              << std::endl;
    std::cerr << "[ERROR] Please report this with your source code." 
              << std::endl;
    throw std::runtime_error("Unknown type: " + type_name);
}
```

**The fix:**
- Explicitly check for known types
- If type is unknown, **throw exception**
- Clear error message for user
- No silent default
- **Fail loud, fail fast, fail safe**

---

## Part 8: Automated Testing

**Run the full test suite:**

```bash
./test.sh
```

**What it does:**
1. Tests vulnerable version (expects crash)
2. Tests safe version (expects clean error)
3. Compares outputs
4. Verifies defensive behavior

**Expected output:**
```
========================================
Testing Vulnerable Compiler
========================================
[Compilation crashes with segfault] ✓

========================================
Testing Safe Compiler
========================================
[Compilation fails with clear error] ✓

========================================
Test Summary
========================================
Vulnerable: Crashes (stack corruption) ✗
Safe: Fails cleanly (explicit error) ✓

The defensive fix works! ✓
```

---

## Part 9: The Flip Test

**Question:** How do you verify the fix works?

**Answer:** Try to compile code with an unknown type.

```bash
# Create test file with unknown type
echo 'let x: UnknownType = 42;' > unknown_type_test.aria

# Vulnerable version: Silently defaults (dangerous)
./vulnerable_compiler unknown_type_test.aria
# Compiles but wrong! (treats UnknownType as i32)

# Safe version: Explicit error (safe)
./safe_compiler unknown_type_test.aria
# Refuses to compile with clear error message
```

**If the safe compiler compiles unknown types:** Fix is broken!  
**If the safe compiler rejects unknown types:** Fix works! ✓

---

## Part 10: Questions To Explore

1. **What if we default to a larger type?**
   - Try changing default to `i64` instead of `i32`
   - Does it prevent the crash?
   - Does it prevent the bug? (No! Still wrong semantics)

2. **What about pointer types?**
   - Try `UnknownType*` (pointer to unknown type)
   - Does vulnerable version handle it?
   - What's the right defensive behavior?

3. **Could we emit a warning instead of error?**
   - Change `throw` to just `std::cerr << "Warning"`
   - Does code compile now?
   - Is this safe? (No! User might miss the warning)

4. **What about generic types?**
   - Try `Array<UnknownType>`
   - How should compiler handle generic containing unknown?
   - Fail early or fail at instantiation?

---

## Part 11: Real-World Connection

**This exact bug pattern appears in:**

- **Type inference engines** - Default to `int` when can't infer
- **Dynamic languages** - Coerce to number when type unclear
- **Configuration parsers** - Default to empty string on missing field
- **Data deserializers** - Use zero value when field unknown

**The defensive principle applies everywhere:**

> **Don't guess. Don't default. Fail explicitly.**

---

## Cleanup

```bash
make clean
```

Removes all compiled binaries and intermediate files.

---

## What You Learned

1. ✅ Silent defaults cause **stack corruption**
2. ✅ Compiler bugs can create **security vulnerabilities**
3. ✅ Defensive fix: **Throw error instead of guessing**
4. ✅ Flip test: **Verify unknown types are rejected**
5. ✅ Pattern recognition: **Silent failures are lies**

---

**Next Steps:**

- Try [Module 02: Pointer Loading](../../02-pointer-loading/) for pointer confusion bugs
- Examine your own codebase for silent defaults
- Add explicit error handling where you find guessing

---

**Congratulations! You've climbed this ladder so you don't have to ship it.**

🛡️ **AILP Educational Initiative** 🛡️
