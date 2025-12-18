# The Craftsman's Manifesto
## Principles for Builders Who Give a Damn

This is not about being clever. This is not about looking professional. This is about **building things that work.**

---

## The Three Laws

### 1. Dead Center or Don't Bother

**The bubble is either dead center or it's not. Everything else is accumulated slop.**

"Close enough" on measurement 1 becomes "way off" by measurement 10. "In the marks" on the level means tearing down walls later. Rounding errors on transaction 1 means lost money by transaction 10,000.

**Precision compounds positively. Slop compounds negatively.**

If you can't be exact, **know how much error you're accepting and where it accumulates.**

---

### 2. The Flip Test

**If you can't verify your tool works in 5 seconds, you don't understand it well enough to use it.**

The $8 level has a flip test: place it, note the bubble, rotate 180°, compare. If it matches, the level is true. If it doesn't, the level is lying.

**Five seconds. No external dependencies. Physics-based proof.**

The $800 digital level? You have to trust the manufacturer. Trust the calibration. Trust the documentation. Trust the battery isn't dying.

**Tools you can verify > Tools you must trust.**

This applies to code too:
- Can you explain what this function does without running it?
- Can you predict the output for given inputs?
- Can you verify correctness by inspection?

If not, you don't understand your tool.

---

### 3. The Ladder Principle

**Every bug you ship is a ladder you'll climb later. Do it right once, or do it wrong repeatedly.**

You cut a rafter wrong? Climb the ladder. Still wrong? Climb again. 14 ladder trips later, you learn: **should have done the math on the ground.**

You ship code with `any` types? Debug session 1. Still broken? Debug session 2. Production incident? 3 AM debug session.

**The debugging ladder is just as real as the physical one.**

Do the work upfront or do it repeatedly under pressure. Your choice.

---

## The Eight Principles

### 1. Understand Before Building

Don't copy-paste code you don't understand. Don't use frameworks you can't explain. Don't trust tools you can't verify.

**If you can't explain why it works, you can't fix it when it breaks.**

---

### 2. Simple Tools, Deep Understanding

The $8 level beats the $800 level because you can understand it completely. Bubble in liquid. Perpendicular to gravity. That's it.

**Master simple tools before reaching for complex ones.**

A person who fully understands vanilla JavaScript will outperform someone who barely understands React. A person who understands C pointers will outperform someone who cargo-cults Rust.

---

### 3. Precision at Boundaries, Exact in Core

Convert messy external data to clean internal representation **once** at the boundary. Do exact arithmetic in the core. Convert back **once** at output.

**Don't pollute your core logic with the messiness of the outside world.**

- Money? Integer cents internally, display as dollars at edges.
- Measurements? Fixed-point or rationals internally, display as decimals at edges.
- Strings? Validate and normalize at input, process UTF-8 internally, format at output.

---

### 4. Errors Are Data, Not Exceptions

A function either succeeds (returns value) or fails (returns error). Both are data. Both flow through your program. Both are handled explicitly.

**No surprises. No hidden control flow. No "it might throw, idk."**

```
result = doThing()
if result.err:
    handle_error(result.err)
else:
    use_value(result.val)
```

Clear. Explicit. Compiler-checked. No debugging "where did this exception come from?"

---

### 5. Types Prevent Pain

Types are not busywork. Types are **documentation that the compiler verifies.**

"This function takes a User object" is a promise. With types, the compiler enforces the promise. Without types, you find out at runtime when production breaks.

**Would you rather find the error at compile time or 3 AM?**

---

### 6. Premature Optimization is Buying Tools You Can't Use

Don't buy the $800 level if you don't know how to use a level. Don't use complex frameworks if you don't understand the problem.

**Make it work. Make it right. Make it fast. In that order.**

And "make it fast" only happens when you've measured and proven it's slow. Premature optimization is guessing.

---

### 7. The Easy Path Is a Lie

"Close enough" looks easy now. Rework is hard later.

Using `any` types looks easy now. Debugging production is hard later.

Skipping tests looks easy now. Fixing bugs in customers' hands is hard later.

**The easy path is the hard path deferred with interest.**

---

### 8. You Are Responsible

Not the framework. Not the language. Not the tools. Not the manager who gave you impossible deadlines.

**You.**

When the building collapses, "but I used the standard approach" won't save you. When the financial system loses money, "but floats are easier" won't save you.

**Own your decisions. Own your work. Own the consequences.**

This is Stirner's egoism applied to craftsmanship: You are totally responsible for what you build. No excuses. No deferring blame. You did it or you didn't.

---

## The Anti-Patterns

### The AC Truck Manager

Sits in air conditioning. Never done the actual work. Doesn't know what a square root is. Makes impossible demands. Blames workers when physics disagrees.

**Does not get to make technical decisions.**

---

### The New Hire with Expensive Tools

Buys the $500 DeWalt set. Doesn't understand torque. Strips screws. Blames the tools.

**Expensive tools don't compensate for lack of understanding.**

---

### The "Close Enough" Mentality

"In the marks" on every measurement. "Almost" on every check. "Good enough" on every decision.

By measurement 10, everything is wrong. By building 4, nothing fits.

**Close enough compounds into disaster.**

---

### The Framework Chaser

Always learning the newest framework. Never masters fundamentals. Can't work without specific tools. Breaks when the ecosystem changes.

**Knows HOW to use many tools. Understands WHY for none of them.**

---

## The Craftsman's Code

**I will understand my tools.**
Not trust blindly. Not cargo-cult. Understand.

**I will demand precision.**
Not "close enough." Not "in the marks." Dead center.

**I will verify correctness.**
Not assume. Not hope. Verify.

**I will own my work.**
Not blame tools. Not blame managers. Own it.

**I will teach through action.**
Not lecture. Not condescend. Show by building things that work.

**I will respect simplicity.**
Not complexity for its own sake. Simple tools, deep understanding.

**I will build for the long term.**
Not just to pass the demo. Not just to ship this week. For years.

**I will remember the ladder.**
Every shortcut now is a debugging ladder later. Choose wisely.

---

## The Promise

Follow these principles and you will:

- Write code that works the first time (or fails obviously)
- Debug less (errors caught early, not in production)
- Sleep better (no 3 AM "website is down" calls)
- Build respect (from people who actually do the work)
- Create things that last (years without maintenance)

**You will be a craftsman, not a helper.**

---

## The Graduation Requirement

You are ready when you can:

1. Explain why "close enough" fails at scale
2. Verify your tools work (the flip test)
3. Choose simple tools over complex ones
4. Design for exactness, not approximation
5. Handle errors explicitly, not accidentally
6. Optimize after measuring, not before
7. Take responsibility without excuses
8. Teach others through working examples

**Graduate competent or get your suffering back.**

---

## The Final Word

The bubble is either dead center or it's not.

The rafter either fits or it doesn't.

The code either works or it doesn't.

**Everything else is accumulated slop that you'll pay for later.**

Build things that work. Do it right the first time. Own the result.

That's craftsmanship.

---

*"The ladder doesn't lie. Neither does the compiler."*

**— Randy's School of Hard Knocks**
