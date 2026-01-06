# Module 04: The Recursion Depth Bomb
## ARIA-026 Bug #4 - How 501 Nested Expressions Freeze The Teacher

### The Bug Story

**Date:** January 1, 2026  
**Severity:** MEDIUM (DoS attack vector, prevents system response)  
**Found By:** Gemini AI audit  
**Context:** Expression code generation had no depth guard  
**Irony:** Statement codegen HAD depth guard (ARIA-022), expressions didn't

### The Vulnerability

**The Code:**

```cpp
llvm::Value* ExprCodegen::codegenExpressionNode(ASTNode* node, ExprCodegen* codegen) {
    if (!node) throw std::runtime_error("Null expression node");
    
    // 💀 BUG: No recursion depth check!
    // Just... keep recursing forever
    
    switch (node->type) {
        case BINARY_OP:
            // Recurse on left and right
            auto left = codegenExpressionNode(node->left, codegen);
            auto right = codegenExpressionNode(node->right, codegen);
            return /* combine them */;
            
        case FUNCTION_CALL:
            // Recurse on each argument
            for (auto arg : node->args) {
                codegenExpressionNode(arg, codegen);
            }
            return /* call function */;
            
        // ... more cases, all recursing without limit
    }
}
```

**Meanwhile, in statement codegen (ARIA-022 already fixed this):**

```cpp
void StmtCodegen::codegenStatement(ASTNode* node) {
    // ✅ HAS depth guard!
    if (++stmt_depth_ > MAX_STMT_DEPTH) {
        throw std::runtime_error("Statement nesting too deep");
    }
    
    // ... do work ...
    
    --stmt_depth_;  // Restore on exit
}
```

**The problem:** Someone fixed it for statements but forgot expressions!

### The Attack Vector (Malicious OR Accidental)

**Malicious:** Student submits code to Teacher AI system:

```aria
// Generated code - 501 nested additions
let x = 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + ...
    // ... 501 more + operations
    + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1;
```

**Or as a helper to generate it:**

```python
# Attack code generator
def generate_depth_bomb(depth=501):
    expr = "1"
    for i in range(depth):
        expr = f"({expr} + 1)"
    return f"let x = {expr};"

print(generate_depth_bomb())
```

**Result:**

```
Step 1: Student submits code
Step 2: Compiler starts parsing
Step 3: AST created with 501 nested expressions
Step 4: Codegen calls codegenExpressionNode()
Step 5: Recurses 501 times
Step 6: Stack overflow
Step 7: Compiler crashes
Step 8: Teacher system offline
Step 9: Other students can't get help
Step 10: Lesson disrupted
```

**Accidental:** Student writes nested comprehension:

```aria
// Looks innocent, but generates deep nesting
let matrix = [[[[[x * y for y in 0..10]
                    for x in 0..10]
                    for z in 0..10]
                    for w in 0..10]
                    for v in 0..10];
```

**Each level of comprehension adds expression nesting depth!**

### The Physical Consequence

**Scenario:** Hospital child using robot companion for physical therapy

```
10:00 AM: Therapy session begins
10:15 AM: Child asks Teacher AI: "Can you help with my math homework?"
10:16 AM: Teacher AI processes request
10:17 AM: Malicious student submits depth bomb elsewhere in system
10:17 AM: Compiler hits depth bomb, stack overflows
10:17 AM: Teacher system crashes (shares compiler service)
10:18 AM: Child's question goes unanswered
10:19 AM: Therapy robot waiting for Teacher guidance
10:20 AM: Child frustrated, loses focus
10:25 AM: Therapy session fails
10:30 AM: Hospital staff has to intervene

Impact: Denial of service to vulnerable child
```

Not physical harm, but **denial of needed service = harm**.

### The Mathematics of Stack Overflow

**x86_64 typical stack size:** 8 MB  
**Average stack frame size:** ~200 bytes (with LLVM locals)  
**Maximum safe recursion:** ~40,000 calls

**But wait!** Compiler also uses stack for other things:

- LLVM value objects
- Temporary allocations
- Exception handling frames
- Debug information

**Realistic safe limit:** ~500-1000 deep

**Our arbitrary choice:** 500 (conservative)

### Why 500?

**Reasoning:**

- Normal code rarely nests > 10 deep
- Crazy-but-legitimate code might hit 50
- Generated code might hit 100
- 500 gives 5× safety margin over "reasonable crazy"
- Low enough to prevent stack overflow
- High enough to allow legitimate deep nesting

**This is engineering judgment, not science.**

### The Defensive Fix

**Pattern from ARIA-022 (statement depth guard):**

```cpp
// In header file
class ExprCodegen {
    size_t expr_depth_ = 0;
    static constexpr size_t MAX_EXPR_DEPTH = 500;
    // ... rest of class ...
};

// In implementation
llvm::Value* ExprCodegen::codegenExpressionNode(ASTNode* node, ExprCodegen* codegen) {
    if (!node) throw std::runtime_error("Null expression node");
    
    // ✅ ARIA-026: Add recursion depth guard
    if (++codegen->expr_depth_ > MAX_EXPR_DEPTH) {
        --codegen->expr_depth_;  // Restore before throwing
        throw std::runtime_error(
            "Expression nesting limit exceeded (" + 
            std::to_string(MAX_EXPR_DEPTH) + " levels). " +
            "This may indicate malicious input or excessive code generation. " +
            "Consider breaking the expression into smaller parts."
        );
    }
    
    // ✅ RAII depth guard - ensures decrement on ALL exits
    struct DepthGuard {
        size_t& depth;
        DepthGuard(size_t& d) : depth(d) {}
        ~DepthGuard() { --depth; }
    } guard(codegen->expr_depth_);
    
    // ... rest of codegen logic ...
    // Depth automatically restored by DepthGuard destructor
}
```

**Key features:**

1. **Increment before check** - Catches current level
2. **Immediate decrement if limit exceeded** - Clean failure
3. **RAII guard** - Handles ALL exit paths (return, throw, break)
4. **Helpful error message** - Explains the limit and why

### The Construction Analogy

You're building a tower of scaffolding. How high can you safely go?

**No limit:** Build until it collapses (bad!)  
**Infinite limit:** "It'll be fine!" (also bad!)  
**Conservative limit:** "Stop at 50 feet, plenty of margin"

**Engineering is choosing appropriate limits based on:**
- Physical constraints (stack size)
- Safety margins (5× buffer)
- Practical limits (500 is ridiculous already)
- Error handling (clean failure, not catastrophic)

### Lab Exercise: Crash The Compiler

**Files in `lab/`:**

- `vulnerable_expr.cpp` - No depth guard
- `safe_expr.cpp` - With ARIA-026 fix
- `generate_bomb.py` - Creates deeply nested expressions
- `test_limits.sh` - Tests various depths
- `Makefile` - Build and crash!

**Your Mission:**

1. Generate expression bomb: `./generate_bomb.py 100`
2. Compile with vulnerable version
3. Watch it succeed (100 is still safe)
4. Generate bigger bomb: `./generate_bomb.py 1000`
5. Watch it crash with segfault 💥
6. Now compile with safe version
7. Watch it fail cleanly with clear error ✅
8. Understand: crash vs controlled failure

**Extra Credit:**
- Find the exact depth where vulnerable version crashes on your system
- Measure stack usage per recursion level
- Calculate theoretical max depth
- Understand why we chose 500

### The Flip Test

**Question:** How do you verify the guard works?

**Answer:** Try to exceed it!

```bash
# Test depths from 1 to 600
for depth in {1..600}; do
    echo "Testing depth: $depth"
    ./generate_bomb.py $depth | ./compiler
    
    if [ $? -ne 0 ]; then
        if [ $depth -le 500 ]; then
            echo "ERROR: Failed at legal depth $depth"
        else
            echo "SUCCESS: Correctly rejected depth $depth"
        fi
    fi
done

# Expected results:
# Depths 1-500: All succeed
# Depths 501+: All fail with clean error
```

**If the boundary is sharp at 500, the guard works.**

### Real-World Impact

**Before fix:**
- Unlimited expression recursion
- Stack overflow on deep nesting
- Compiler crashes = service denial
- Malicious code can DoS system
- No protection for vulnerable users

**After fix:**
- Limited to 500 levels (plenty for legitimate code)
- Clean error message on excess
- No crashes, no stack overflow
- DoS attack prevented
- Service remains available

**Time to fix:** 10 minutes (copy from statement codegen)  
**Time we should have done it:** During initial implementation  
**Lesson:** Apply fixes consistently across similar code

### The Pattern Recognition

**ARIA-022:** Statement recursion guard  
**ARIA-026 Bug #4:** Expression recursion guard

**Why was one done and not the other?**

1. Statements implemented first
2. Bug found in statements
3. Fixed in statements
4. Expressions implemented later
5. Developer forgot to add same guard
6. Audit caught the inconsistency

**Lesson:** When you fix a category of bug, search the codebase for similar patterns!

```bash
# After fixing statement depth:
git grep -n "codegenStatement"
git grep -n "codegenExpression"

# Do both have depth guards?
# If not, add them!
```

### Questions to Ponder

1. **Why 500 and not 1000? Or 100?**
   - Engineering judgment based on tradeoffs
   - Legitimate code shouldn't need > 500
   - Low enough to prevent overflow
   - High enough to allow flexibility
   - Can be tuned based on real-world needs

2. **What about tail recursion?**
   - Compiler could optimize tail calls
   - But expression codegen isn't tail-recursive
   - This is AST depth, not call depth
   - Different problem, different solution

3. **Could you make the limit configurable?**
   - Yes! Command-line flag or config file
   - But sane default matters
   - Users shouldn't need to tune this
   - If they hit limit, code is probably wrong anyway

4. **What other resources need limits?**
   - Memory allocation (total bytes)
   - File handles (number open)
   - Compilation time (timeout)
   - Output size (generated code)
   - Think defensively!

### Next Module

[Module 05: The OOM String Literal Attack](../05-string-pooling/) - How unbounded literal creation crashes the Teacher

---

**The Pattern Deepens:**

Module 01: Silent defaults → corruption  
Module 02: Incomplete cases → bypass  
Module 03: Hidden dependencies → non-determinism  
Module 04: Missing guards → DoS  

**The fix pattern:**
1. Identify unbounded resources (stack depth!)
2. Set conservative limits
3. Enforce limits defensively
4. Fail cleanly with helpful errors
5. Apply consistently across codebase

**Resource limits are not optional. They're insurance against the unknown.**

---

## Navigation

**Learning Path:**
- ← Previous: [Module 03: Non-Determinism](../03-non-determinism/) - Audit trail corruption from locale dependencies
- → Next: [Module 05: String Pooling](../05-string-pooling/) - OOM from unbounded allocation
- ↑ Back to: [Main README](../README.md) - Curriculum overview

**Related Resources:**
- 📜 [Manifesto](../MANIFESTO.md) - The Ninth Principle: Time Reveals All Bugs
- 🤝 [Contributing](../CONTRIBUTING.md) - Submit your own bug stories
- 🎯 [Educational Repo](../../) - More learning resources

---

**Remember:** Resource limits are insurance against the unknown.  
**This ladder:** 4 hours of debugging, 1 stack trace, 1 RAII guard pattern.  
**The alternative:** DoS attack shuts down Teacher system, therapy disrupted.

**Limit your resources. Always.**

🛡️ **AILP Educational Initiative** 🛡️
