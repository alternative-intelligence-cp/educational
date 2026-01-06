# Defensive Compiler Design: Type Safety From First Principles
## Learning Through Real Bugs That Could Cause Physical Harm

### What This Is

This is not a typical compiler tutorial. This is a collection of **actual bugs found in a real safety-critical compiler** - bugs that could have caused a robot to crush a child's hand - and how defensive design prevents them.

You'll learn type safety by seeing what happens when it fails catastrophically.

### The Stakes

The Aria programming language is being built to control robotic assistants for vulnerable populations: neurodivergent children, hospital patients, elderly with dementia. **One bug could cause physical harm.**

That constraint changes everything about compiler design.

### Who This Is For

- Anyone building compilers or type checkers
- Developers who want to understand *why* type safety matters
- Students tired of toy examples that don't show real consequences
- Anyone who thinks "it compiles, ship it" is good enough

### Who This Is NOT For

- People looking for quick hacks
- Anyone who thinks "it works on my machine" means it's done
- Developers who trust assumptions instead of verifying them

### The Philosophy

**Three Core Principles (From Construction Work):**

1. **Dead Center or Don't Bother** - Type checking is either correct or it's dangerous. "Close enough" compounds into catastrophe.

2. **The Flip Test** - Every type mapping must be verifiable. If you can't prove it's correct, it's wrong.

3. **No Silent Defaults** - Defaulting to `i32` for unknown types is like defaulting to "yes" when you don't understand the question. Catastrophic.

### What You'll Learn

Each module shows a **real bug from production code**, the harm it could cause, and how to prevent it:

1. **[The i32 Default Catastrophe](./01-type-defaulting/)** - ARIA-026 Bug #1
   - How silent defaulting causes memory corruption
   - Why 16-byte struct → 4-byte i32 = robot crushes child
   - Building a type checker that fails hard on unknowns

2. **[The Pointer Confusion Bug](./02-pointer-loading/)** - ARIA-026 Bug #2
   - How returning pointer address instead of value disables safety limits
   - Why MAX_FORCE = 10 becoming 93 trillion is bad
   - Distinguishing stack variables from globals from SSA values

3. **[The Determinism Requirement](./03-non-determinism/)** - ARIA-026 Bug #3
   - Why locale-dependent serialization breaks AGI reasoning logs
   - How sprintf("%lld") violates bit-identical requirements
   - Building truly deterministic operations

4. **[The Recursion Depth Bomb](./04-recursion-limits/)** - ARIA-026 Bug #4
   - Why unlimited recursion enables DoS attacks
   - How stack overflow prevents Teacher system from responding
   - Adding depth guards that actually work

5. **[The OOM String Literal Attack](./05-string-pooling/)** - ARIA-026 Bug #5
   - How unbounded literal creation causes memory exhaustion
   - Why Teacher system crashes after 8 hours of conversation
   - Implementing string interning correctly

### The Learning Model

Each module has:

1. **The Vulnerability** - Code that looks fine but fails catastrophically
2. **The Attack Vector** - How this gets exploited (malicious or accidental)
3. **The Physical Consequence** - What happens when a robot runs this code
4. **The Defensive Fix** - How to prevent it architecturally
5. **The Interactive Lab** - Trigger the bug yourself, then fix it

### Why "Defensive" Design?

Most compiler tutorials teach correctness: "make the output match the spec."

This teaches **survival**: "assume everything will be attacked, including by yourself."

Defensive design means:
- No silent failures (throw exceptions for unknown cases)
- No trust in assumptions (verify everything)
- No optimization before correctness (boring is armor)
- No features without safety analysis

### Real-World Context

These bugs were found during a comprehensive safety audit on January 1, 2026. The first audit using this methodology achieved **100% accuracy (5/5 real bugs, 0 false positives)** after weeks of refinement.

The prompt engineering that enabled that success is documented here too. You'll learn:
- How to conduct effective code audits
- How to communicate safety requirements to AI assistants
- How to maintain 80%+ accuracy in bug detection
- How iterative refinement leads to perfect methodology

### Prerequisites

- Basic understanding of compilers (lexer, parser, codegen)
- Familiarity with LLVM or similar IR (helpful but not required)
- Ability to read C++ or Rust
- Willingness to break things to understand them

### Quick Start

```bash
cd 01-type-defaulting/
# Read README.md for the bug story
# Try the vulnerable code in lab/
# Implement the fix yourself
# Compare to the reference solution
```

### The Bigger Picture

This isn't just about compilers. It's about **engineering mindset for safety-critical systems**:

- Medical devices
- Automotive control systems
- Industrial automation
- Financial transaction processing
- Anything where bugs = harm

The principles you learn here apply everywhere precision matters.

### The Learning Path

**Suggested order for working through the modules:**

```
START → Module 01 (Type Defaulting) 
          ↓
        Module 02 (Pointer Loading)
          ↓
        Module 03 (Non-Determinism)
          ↓
        Module 04 (Recursion Limits)
          ↓
        Module 05 (String Pooling)
          ↓
        Read MANIFESTO.md for synthesis
          ↓
        Contribute your own bugs! → CONTRIBUTING.md
```

**Each module is independent**, but they build on common defensive patterns. Working through them in order helps recognize those patterns.

**Time commitment:**
- Per module: 1-2 hours (reading + lab)
- Full curriculum: 1-2 days (5 modules + manifesto)
- Compare to: Shipping these bugs = weeks of debugging + potential harm

**That's the deal.**

---

### Additional Resources

**Core Documents:**
- 📜 [**MANIFESTO.md**](./MANIFESTO.md) - The complete defensive programming philosophy (10 principles)
- 🤝 [**CONTRIBUTING.md**](./CONTRIBUTING.md) - Submit your own bug stories as teaching modules
- 🎯 Main [Educational Repository](../) - More learning resources

**Module Quick Links:**
- 📚 [Module 01: Type Defaulting](./01-type-defaulting/) - Stack corruption from silent defaults
- 📚 [Module 02: Pointer Loading](./02-pointer-loading/) - Safety bypass from pointer confusion
- 📚 [Module 03: Non-Determinism](./03-non-determinism/) - Audit trail corruption from locale deps
- 📚 [Module 04: Recursion Limits](./04-recursion-limits/) - DoS from missing resource guards
- 📚 [Module 05: String Pooling](./05-string-pooling/) - OOM from unbounded allocation

**Related Topics:**
- Type systems and safety
- Compiler security considerations
- Defensive programming patterns
- Safety-critical system design
- Resource limiting and DoS prevention

---

### Contributing

Found a bug in your compiler? **Submit it as a teaching module!**

Every real vulnerability is a learning opportunity. We document:
- The bug (with full context)
- How it was found
- Why it matters
- How it was fixed
- What we learned

This is **living curriculum** - it grows with every mistake we catch.

See [CONTRIBUTING.md](./CONTRIBUTING.md) for detailed submission guidelines.

---

### Philosophy in Action

From the Craftsman's Manifesto:

> **"Every bug you ship is a ladder you'll climb later. Do it right once, or do it wrong repeatedly."**

These modules are the ladders we've already climbed. Learn from them so you don't have to climb the same ones.

---

### License

MIT License - See LICENSE file in repository root.

By contributing bug stories, you help build collective knowledge of defensive programming.

---

**Part of the AILP Educational Initiative**  
**"Boring is armor. Precision compounds. Ship safely or don't ship."**

🛡️ [Educational Repository Home](../) | 📖 [Manifesto](./MANIFESTO.md) | 🤝 [Contributing](./CONTRIBUTING.md)
