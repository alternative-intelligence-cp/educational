# Contributing Your Bug Stories
## How To Add Your Own Modules

---

## The Invitation

**You've shipped compiler bugs. We all have.**

**You can make them valuable by teaching others.**

This curriculum exists because we **documented our mistakes** instead of hiding them.

**Your mistakes have value too.**

---

## What We're Looking For

**Good module candidates:**

1. **Real bugs** - Not hypothetical, not contrived, actually shipped
2. **Clear harm** - Physical, emotional, or service harm (not just "technically incorrect")
3. **Defensive fix** - A fix that prevents the entire category, not just the instance
4. **Pattern extraction** - A generalizable lesson, not just "change line 47"
5. **Teaching value** - Something others can learn from

**Examples of good bugs:**

- ✅ "We didn't bounds-check array access → student code crashed Teacher → therapy interrupted"
- ✅ "We trusted user input length → buffer overflow → potential RCE"
- ✅ "We didn't limit loop iterations → infinite loop → DoS"
- ✅ "We used floating-point for money → rounding errors → billing incorrect"

**Examples of not-quite-right bugs:**

- ❌ "We had a typo in a variable name" (no pattern to extract)
- ❌ "Performance was slow" (not harm, just inconvenience)
- ❌ "The code was ugly" (aesthetic, not safety)

**If you're unsure:** Open an issue and describe the bug. We'll help you figure out if it fits!

---

## Module Structure

**Each module should include:**

### 1. The Bug Story (Header)
```markdown
### The Bug Story

**Date:** When was this bug found?  
**Severity:** HIGH/MEDIUM/LOW (based on harm potential)  
**Found By:** Audit? User report? Crash log?  
**Context:** What were you building?  
**Timeline:** How long was it in production?
```

### 2. The Vulnerability (Code)
```markdown
### The Vulnerability

**The Code:**

```[language]
// Show the ACTUAL vulnerable code
// Mark dangerous lines with 💀 BUG comments
// Include enough context to understand it
```

**What could go wrong?**
[Explain the vulnerability clearly]
```

### 3. The Attack Vector
```markdown
### The Attack Vector

**Scenario:** [Describe how this gets triggered]

[Show example input/code that triggers it]

**The Mathematics of [Consequence]:**
[Show the numbers - why does this break?]
```

### 4. The Physical Consequence
```markdown
### The Physical Consequence

**Scenario:** [Real-world harm scenario]

[Walk through timeline of harm]

**Impact:** [What harm actually occurs?]
```

### 5. The Defensive Fix
```markdown
### The Defensive Fix

[Explain the pattern-level fix]

**The fixed code:**

```[language]
// Show the ACTUAL defensive code
// Mark safety additions with ✅ comments
// Explain WHY each change prevents harm
```

**Effect:**
[Show how this prevents the entire category]
```

### 6. A Construction/Physical Analogy
```markdown
### The [Something] Analogy

[Explain the bug using physical-world wisdom]

**Like [physical thing]:** [analogy explanation]

**Result:** [what the analogy teaches]
```

### 7. Lab Exercise
```markdown
### Lab Exercise: [Name]

**Files in `lab/`:**
- `vulnerable_[thing].cpp` - The broken version
- `safe_[thing].cpp` - The fixed version
- `test_[thing].sh` - Test script
- `Makefile` - Build instructions

**Your Mission:**
1. [Step to trigger bug]
2. [Step to observe failure]
3. [Step to test fix]
4. [Step to verify safety]
```

### 8. The Flip Test
```markdown
### The Flip Test

**Question:** How do you verify [fix] works?

**Answer:** [Concrete, verifiable test]

[Show the actual verification commands]
```

### 9. Real-World Impact
```markdown
### Real-World Impact

**Before fix:**
- [List consequences of bug]

**After fix:**
- [List improvements from fix]

**Time to fix:** [How long did fix take?]  
**[Metric] saved:** [Quantify improvement]
```

### 10. Questions to Ponder
```markdown
### Questions to Ponder

1. **[Question about edge cases]**
   - [Answer/discussion]

2. **[Question about applicability]**
   - [Answer/discussion]

3. **[Question about tradeoffs]**
   - [Answer/discussion]
```

---

## Submission Process

### Step 1: Fork & Clone
```bash
git clone https://github.com/RandyBrown67/educational.git
cd educational/defensive-compiler-design
```

### Step 2: Create Module Directory
```bash
mkdir 06-your-bug-name
cd 06-your-bug-name
```

**Naming convention:** `##-bug-category`
- Use next available number (01-05 are taken)
- Use lowercase-with-hyphens
- Be descriptive but concise

### Step 3: Write README.md
```bash
# Use the structure above
# Be honest about the bug
# Include real code (anonymized if needed)
# Show actual harm or near-harm
```

### Step 4: Create Lab Materials
```bash
mkdir lab
cd lab

# Create these files:
touch vulnerable_example.cpp   # The broken code
touch safe_example.cpp         # The fixed code  
touch test.sh                  # Test script
touch Makefile                 # Build instructions
touch README.md                # Lab instructions
```

**Lab code should:**
- Be minimal (not full production code)
- Demonstrate the bug clearly
- Be actually runnable
- Include comments explaining what's happening

### Step 5: Test Everything
```bash
# Build the vulnerable version
make vulnerable

# Trigger the bug
./test.sh vulnerable
# Should show the failure!

# Build the safe version
make safe

# Verify the fix
./test.sh safe
# Should show the protection!
```

### Step 6: Submit Pull Request

```bash
git checkout -b module-06-your-bug-name
git add 06-your-bug-name/
git commit -m "Add Module 06: [Bug Name]"
git push origin module-06-your-bug-name
```

**In your PR description:**
- Summarize the bug
- Explain why it's valuable to teach
- Note any special requirements (tools needed, etc.)
- Link to any public discussion/blog posts about the bug

---

## Writing Guidelines

### Voice & Tone

**Do:**
- ✅ Write directly and honestly
- ✅ Use construction/physical analogies
- ✅ Show the actual harm or near-harm
- ✅ Include specific numbers and timelines
- ✅ Admit what you didn't know initially
- ✅ Use casual language ("Let's see what breaks")

**Don't:**
- ❌ Use corporate-speak or marketing language
- ❌ Inflate the bug's importance
- ❌ Blame others for the bug
- ❌ Use overly academic language
- ❌ Hide the messy details

**Example good voice:**
> "We shipped this for six months before an audit caught it. The bug was obvious in retrospect, but we just... didn't check that case. Classic defensive failure."

**Example bad voice:**
> "This sophisticated vulnerability demonstrates advanced exploitation techniques in type system design."

**Keep it real. Keep it honest.**

### Code Examples

**Do:**
- ✅ Show actual code (anonymized if sensitive)
- ✅ Include enough context to understand
- ✅ Mark dangerous lines clearly (💀 BUG)
- ✅ Mark safety fixes clearly (✅ FIX)
- ✅ Add inline comments explaining the bug

**Don't:**
- ❌ Show pseudocode (we want real examples)
- ❌ Strip so much context it's unclear
- ❌ Omit the embarrassing parts
- ❌ Make it seem more complex than it is

### Harm Scenarios

**Do:**
- ✅ Describe realistic harm (even if it didn't occur)
- ✅ Be specific about victims (child, patient, user)
- ✅ Explain the timeline of harm
- ✅ Quantify impact where possible

**Don't:**
- ❌ Exaggerate to "system explodes"
- ❌ Use vague "bad things happen"
- ❌ Skip the harm explanation
- ❌ Focus only on technical consequences

**The harm is why we care about the bug.**

---

## Lab Code Requirements

### Vulnerable Version

**Should:**
- Compile and run (don't add syntax errors)
- Clearly demonstrate the bug when triggered
- Include comments explaining what's about to break
- Be as simple as possible while still showing the bug

**Example:**
```cpp
// vulnerable_recursion.cpp
#include <stdio.h>

int factorial(int n) {
    // 💀 BUG: No depth limit!
    // This will overflow the stack on large n
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

int main() {
    // This will crash!
    printf("%d\n", factorial(100000));
    return 0;
}
```

### Safe Version

**Should:**
- Implement the defensive fix
- Include comments explaining the safety mechanism
- Handle the error case gracefully
- Still be simple and clear

**Example:**
```cpp
// safe_recursion.cpp
#include <stdio.h>
#include <stdlib.h>

#define MAX_DEPTH 1000

int factorial_impl(int n, int depth) {
    // ✅ FIX: Check depth limit
    if (depth > MAX_DEPTH) {
        fprintf(stderr, "Error: Recursion depth exceeds %d\n", MAX_DEPTH);
        exit(1);
    }
    
    if (n <= 1) return 1;
    return n * factorial_impl(n - 1, depth + 1);
}

int factorial(int n) {
    return factorial_impl(n, 0);
}

int main() {
    // This will fail safely with clear error message!
    printf("%d\n", factorial(100000));
    return 0;
}
```

### Test Script

**Should:**
- Test both vulnerable and safe versions
- Show the difference clearly
- Be automated (no manual intervention)
- Return appropriate exit codes

**Example:**
```bash
#!/bin/bash

VERSION=$1

if [ "$VERSION" == "vulnerable" ]; then
    echo "Testing vulnerable version..."
    ./vulnerable_recursion
    # Will crash - that's the point!
elif [ "$VERSION" == "safe" ]; then
    echo "Testing safe version..."
    ./safe_recursion
    # Will fail cleanly with error message
else
    echo "Usage: ./test.sh [vulnerable|safe]"
    exit 1
fi
```

---

## Review Process

**When you submit a PR, we'll review for:**

1. **Completeness** - All required sections present?
2. **Clarity** - Is the bug and fix understandable?
3. **Value** - Does this teach something useful?
4. **Honesty** - Is this a real bug with real context?
5. **Runability** - Do the labs actually work?

**We'll provide feedback and work with you to refine it.**

**Once approved:**
- Your module gets merged
- You get added to CONTRIBUTORS.md
- Your bug story helps others avoid the same mistake

---

## Examples of Potential Modules

**If you've shipped any of these, consider contributing:**

- **Integer overflow in allocation size** → Undersized buffer → heap corruption
- **Missing null checks** → Null pointer dereference → crash
- **Race conditions in codegen** → Non-deterministic output → broken builds
- **Unbounded loops** → Infinite loop → DoS
- **Buffer overruns in string handling** → RCE vulnerability
- **Type confusion** → Wrong operation → data corruption
- **Missing cleanup on error paths** → Resource leak → OOM over time
- **Floating-point in financial code** → Rounding errors → incorrect transactions
- **Timezone handling bugs** → Wrong time calculations → scheduling failures
- **Locale-dependent parsing** → Different behavior across systems → broken deployments

**If you've climbed these ladders, document them!**

---

## Questions?

**Not sure if your bug qualifies?** Open an issue:
- Describe the bug
- Explain what went wrong
- Note what you learned

**We'll help you figure out if it's a good fit.**

**Want to contribute but don't have a bug story?** 
- Help improve existing modules
- Create additional lab exercises
- Write tests for the defensive patterns
- Improve documentation clarity

**All contributions welcome!**

---

## License

By contributing, you agree that your contributions will be licensed under the same license as this repository (MIT License).

Your bug stories help others. **Thank you for sharing them.**

---

**Every bug you document is a ladder someone else won't have to climb.**

🛡️ **AILP Educational Initiative** 🛡️  
*Building a library of real production mistakes for collective learning.*
