# Module 05: The OOM String Literal Attack
## ARIA-026 Bug #5 - Death By A Thousand String Literals

### The Bug Story

**Date:** January 1, 2026  
**Severity:** MEDIUM (Memory exhaustion over time, eventually causes crash)  
**Found By:** Gemini AI audit  
**Context:** String literal creation with no deduplication  
**Timeline:** Hours to trigger, but inevitable

### The Vulnerability

**The Code:**

```cpp
llvm::Value* codegenLiteral(LiteralExpr* expr) {
    if (expr->type == STRING) {
        std::string str = expr->value;
        
        // 💀 BUG: Create NEW global for EVERY string literal
        llvm::Constant* str_data = llvm::ConstantDataArray::getString(context, str, true);
        
        llvm::GlobalVariable* str_gv = new llvm::GlobalVariable(
            *module,
            str_data->getType(),
            true,  // constant
            llvm::GlobalValue::PrivateLinkage,
            str_data,
            ".str"
        );
        
        // No deduplication! If you use "hello" 1000 times,
        // you get 1000 separate global variables.
        
        return str_gv;
    }
    // ... other literal types ...
}
```

**What could go wrong?**

### The Attack Vector (Time-Based Exhaustion)

**Scenario:** Teacher AI having 8-hour conversation with student

```aria
// Simplified Teacher response generation
func generate_encouragement() -> string {
    let options = [
        "Great job!",
        "Keep going!",
        "You're doing well!",
        "Excellent!",
        "Nice work!"
    ];
    
    return options[random(0, 4)];
}

// Called after every student answer
func respond_to_student(answer: string) {
    print(generate_encouragement());  // 💀 Each creates new string literals
    print("Your answer: " + answer);   // 💀 More new literals
    // ... conversation continues ...
}
```

**The Mathematics of Doom:**

```
Session duration: 8 hours
Student interactions: 1 per minute average
Total interactions: 480

Per interaction:
- 1 encouragement string literal
- 1 "Your answer: " string literal  
- 1 student answer (stored as literal)
= 3 new global variables

Total after 8 hours: 480 × 3 = 1,440 string literals

But wait! Many are duplicates:
- "Great job!" used 96 times → 96 globals (should be 1!)
- "Keep going!" used 96 times → 96 globals (should be 1!)
- "Your answer: " used 480 times → 480 globals (should be 1!)

Total globals created: 1,440
Actual unique strings: ~100
Wasted memory: 1,340 unnecessary global variables
```

**Over a day? Week? Month?**

```
Week: 1,440 × 7 = 10,080 string globals
Month: 10,080 × 4 = 40,320 string globals
Year: 40,320 × 12 = 483,840 string globals
```

**Eventually:** System runs out of memory, Teacher crashes.

### The Physical Consequence

**Scenario:** Hospital child dependent on Teacher AI for daily interaction

```
Hour 0: System boots, conversation begins
Hour 1: 180 string globals created
Hour 2: 360 string globals
Hour 4: 720 string globals (memory usage creeping up)
Hour 6: 1,080 string globals (still functioning)
Hour 8: 1,440 string globals (conversation slowing down)
Hour 12: 2,160 string globals (noticeable lag)
Hour 16: 2,880 string globals (memory pressure)
Hour 20: 3,600 string globals (system thrashing)
Hour 24: 4,320 string globals

OOM Killer: Terminates Teacher process
Child: Loses their companion mid-conversation
Impact: Emotional distress, loss of daily routine support
```

**Not immediate harm, but inevitable harm.**

### Why This Is Subtle

**The bug doesn't present immediately:**

- First conversation: Works fine
- Second conversation: Works fine
- Third conversation: Works fine
- ...
- 100th conversation: Still works, but slower
- 1000th conversation: System starting to struggle
- 10000th conversation: Out of memory

**By the time you notice it, you've been shipping the bug for months.**

### The Exhaustion Pattern

**Memory leaks vs. Memory bloat:**

- **Leak:** Lose reference to allocated memory → can't free it
- **Bloat:** Have reference but allocate unnecessarily → waste memory

**This is bloat.** The globals ARE reachable (in the module's global list), but they're duplicates we don't need.

**Like a hoarder:** "I might need this 'hello' global variable... and this OTHER 'hello' global variable... and this THIRD 'hello'..."

### The Defensive Fix: String Interning

**Add a string pool (std::map):**

```cpp
class ExprCodegen {
    // ... existing members ...
    
    // ✅ ARIA-026: String pool for deduplication
    std::map<std::string, llvm::GlobalVariable*> string_pool_;
};
```

**Check pool before creating:**

```cpp
llvm::Value* codegenLiteral(LiteralExpr* expr) {
    if (expr->type == STRING) {
        std::string str = expr->value;
        
        // ✅ Check if we've seen this string before
        llvm::GlobalVariable* str_gv = nullptr;
        auto pool_it = string_pool_.find(str);
        
        if (pool_it != string_pool_.end()) {
            // ✅ REUSE existing global
            str_gv = pool_it->second;
            std::cerr << "[STRING POOL] Reusing pooled string: \"" 
                      << str << "\"" << std::endl;
        } else {
            // ✅ CREATE new global (first time seeing this string)
            llvm::Constant* str_data = llvm::ConstantDataArray::getString(
                context, str, true
            );
            
            str_gv = new llvm::GlobalVariable(
                *module,
                str_data->getType(),
                true,
                llvm::GlobalValue::PrivateLinkage,
                str_data,
                ".str"
            );
            
            // ✅ ADD to pool for future reuse
            string_pool_[str] = str_gv;
            std::cerr << "[STRING POOL] Created new pooled string: \"" 
                      << str << "\"" << std::endl;
        }
        
        return str_gv;
    }
    // ... other literal types ...
}
```

**Effect:**

```
First "hello": Create global + add to pool
Second "hello": Reuse from pool (no new allocation!)
Third "hello": Reuse from pool
...
Millionth "hello": STILL reusing the same global ✅
```

### The Mathematics of Victory

**Before fix (8-hour session):**
```
Total string literals encountered: 1,440
Unique strings: 100
Globals created: 1,440
Memory wasted: 1,340 unnecessary globals
Memory usage: 1,440 × ~50 bytes = ~72 KB
```

**After fix (8-hour session):**
```
Total string literals encountered: 1,440
Unique strings: 100
Globals created: 100 (one per unique string!)
Memory saved: 1,340 globals not created
Memory usage: 100 × ~50 bytes = ~5 KB

Savings: 67 KB (93% reduction!)
```

**After fix (1-year session):**
```
Total literals: 4,204,800 (480 per day × 365 days)
Unique strings: ~500 (more variety over time)
Globals created: 500 (not 4.2 million!)
Memory saved: 4,204,300 unnecessary globals
Memory usage: 500 × ~50 bytes = ~25 KB

Savings: ~200 MB (99.99% reduction!)
```

**System:** Runs indefinitely without memory exhaustion ✅

### The Construction Analogy

You're building identical houses in a development. Each house needs:
- Nails
- Screws  
- Lumber
- Shingles

**Without pooling:**
- Buy nails for house 1
- Buy nails for house 2 (same nails!)
- Buy nails for house 3 (STILL same nails!)
- ...
- Warehouse full of duplicate nail boxes

**With pooling:**
- Buy nails once
- Use from shared supply for all houses
- When depleted, buy more
- Warehouse holds ONE set of each type

**Result:** Same houses built, fraction of the storage cost.

### Lab Exercise: Watch Memory Grow

**Files in `lab/`:**

- `vulnerable_literal.cpp` - No string pooling
- `safe_literal.cpp` - With string interning
- `conversation_simulator.py` - Generates repetitive strings
- `measure_memory.sh` - Tracks memory usage over time
- `visualize_growth.py` - Plots memory usage graph
- `Makefile` - Build and test

**Your Mission:**

1. Generate long conversation: `./conversation_simulator.py 10000`
2. Compile with vulnerable version
3. Run `./measure_memory.sh vulnerable`
4. Watch memory grow linearly 📈
5. Compile with safe version  
6. Run `./measure_memory.sh safe`
7. Watch memory plateau 📊
8. Compare graphs - dramatic difference!

**Extra Credit:**
- Calculate exact memory per global
- Predict OOM based on growth rate
- Find how many iterations until crash
- Understand linear growth vs. bounded growth

### The Flip Test

**Question:** How do you verify string pooling works?

**Answer:** Count the globals!

```bash
# Compile a program with 1000 "hello" literals
echo 'let x = "hello"; let y = "hello"; ...' > test.aria

# Without pooling:
./vulnerable_compiler test.aria
llvm-dis output.bc | grep "private.*str.*hello" | wc -l
# Output: 1000 (one per literal!)

# With pooling:
./safe_compiler test.aria
llvm-dis output.bc | grep "private.*str.*hello" | wc -l
# Output: 1 (reused for all!)
```

**If you see 1 global for N identical literals, pooling works.**

### Real-World Impact

**Before fix:**
- Unbounded global variable creation
- Memory grows linearly with usage time
- Eventually exhausts system resources
- Long-running sessions guaranteed to crash
- Service denial is a matter of when, not if

**After fix:**
- Bounded global variable creation
- Memory plateaus at unique string count
- No resource exhaustion over time
- Can run indefinitely
- Service remains available

**Time to fix:** 30 minutes  
**Memory saved:** Potentially gigabytes over time  
**Service reliability:** Infinite → Actually infinite

### The Performance Bonus

**Unexpected benefit:** String comparison becomes pointer comparison!

```cpp
// Before fix: Must compare string contents
if (str1 == str2) {  // Calls strcmp() - O(n)
    // ...
}

// After fix: Can compare pointers!
if (str1_ptr == str2_ptr) {  // Pointer comparison - O(1)
    // ...
}
```

**Why?** If two string literals are identical, they have the SAME global variable. Different content = different pointer.

**Bonus:** Faster equality checks for constants!

### The Semantic Question

**Is string pooling always correct?**

**Yes for literals:**
- String literals are immutable
- Same content = semantically identical
- Safe to share the same backing store

**Maybe not for mutable strings:**
- If strings can be modified, sharing would cause problems
- "hello" in function A modified → "hello" in function B changes!
- Different concern - handled separately

**In our case:** Literals are constant → pooling is safe ✅

### Questions to Ponder

1. **Why use std::map instead of std::unordered_map?**
   - Either works for correctness
   - unordered_map is faster (O(1) vs O(log n))
   - map is simpler for debugging (sorted output)
   - Choice depends on priorities

2. **What about string template literals?**
   - Template parts should be pooled too!
   - But interpolated values are unique
   - Pool the constant parts, generate the variable parts
   - This was part of ARIA-026 fix

3. **Could the pool itself exhaust memory?**
   - Yes, but only if you have MANY unique strings
   - Pool size = number of unique string literals in all code
   - Realistically bounded by code size
   - Much better than unbounded duplication

4. **What about other types of literals?**
   - Numbers: Usually don't need pooling (small, inlined)
   - Floats: Could pool, but less common
   - Arrays: Definitely pool! Same problem applies
   - Think about duplication patterns in your domain

### The Complete Pattern

**All 5 ARIA-026 bugs share common themes:**

1. **Silent defaults** → Fail explicitly
2. **Incomplete handling** → Enumerate all cases
3. **Hidden dependencies** → Control externalities  
4. **Missing guards** → Limit resources
5. **Unbounded allocation** → Pool and reuse

**The defensive mindset:**
- Assume the worst case will happen
- Protect against it architecturally  
- Don't rely on "users won't do that"
- Make the RIGHT thing automatic
- Make the WRONG thing impossible

---

## Course Complete! 🎓

You've now seen **5 real bugs from a safety-critical compiler** and learned how defensive design prevents them.

**Next Steps:**

1. [Review the Audit Methodology](../audit-methodology/) - How we found these
2. [Build Your Own Type Checker](../projects/type-checker/) - Apply these lessons
3. [Contribute Your Bugs](../CONTRIBUTING.md) - Share your experiences
4. [Join the Community](https://github.com/RandyBrown67/educational) - Learn together

---

**From the Craftsman's Manifesto:**

> "Every bug you ship is a ladder you'll climb later."

**We climbed these ladders so you don't have to. Learn from our mistakes.**

**Boring is armor. Precision compounds. Ship safely.**

---

## Navigation

**Learning Path:**
- ← Previous: [Module 04: Recursion Limits](../04-recursion-limits/) - DoS from missing resource guards
- ✓ **You've completed all 5 modules!**
- → Next Steps:
  - Read the [Manifesto](../MANIFESTO.md) for synthesis of all defensive principles
  - Review [Contributing Guide](../CONTRIBUTING.md) to submit your own bug stories
  - Apply these patterns to your own codebase
- ↑ Back to: [Main README](../README.md) - Curriculum overview

**Related Resources:**
- 📜 [Manifesto](../MANIFESTO.md) - All 10 principles of defensive programming
- 🤝 [Contributing](../CONTRIBUTING.md) - Your bugs can teach others too
- 🎯 [Educational Repo](../../) - More learning resources

---

**Remember:** Unbounded allocation is time-delayed harm.  
**This ladder:** 8 hours of runtime debugging, 1 memory profiler session, 1 pooling fix.  
**The alternative:** OOM crash during vulnerable child's therapy, emotional distress.

**Pool your resources. Intern your strings. Prevent exhaustion.**

---

## 🎓 Congratulations!

You've completed the Defensive Compiler Design curriculum. You've learned:

1. ✅ **Silent defaults kill** (Module 01) - Fail explicitly, never guess
2. ✅ **Enumerate all cases** (Module 02) - Incomplete handling = bugs waiting to happen
3. ✅ **Control dependencies** (Module 03) - Hidden externalities break determinism
4. ✅ **Limit resources** (Module 04) - Unbounded anything enables attacks
5. ✅ **Pool and reuse** (Module 05) - Deduplication prevents exhaustion

**The meta-pattern:** Defensive programming is about making the **right thing automatic** and the **wrong thing impossible**.

**What's next?**
- Apply these patterns to your own codebase
- Look for silent defaults, incomplete cases, hidden dependencies, unbounded resources
- Add guards, throw errors, verify determinism, pool allocations
- **Submit your bugs** as teaching modules - help others learn from your experience

---

🛡️ **AILP Educational Initiative** 🛡️

**"Every bug documented is a ladder someone else won't have to climb."**
