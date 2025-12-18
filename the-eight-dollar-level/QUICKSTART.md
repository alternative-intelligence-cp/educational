# Quick Start Guide

## Running the Examples

### Option 1: Command Line

```bash
# Run the "bad" approach (floating point)
npm run demo:compound-bad

# Run the "good" approach (integer cents)
npm run demo:compound-good

# Run both for comparison
npm run demo:compound
```

### Option 2: Interactive Browser Demo

```bash
# Start a local web server
npm run serve

# Then open in browser:
# http://localhost:8080/examples/01-compound-errors/demo.html
```

Or just double-click `examples/01-compound-errors/demo.html` to open directly.

## What You'll See

### The Bad Way (Floating Point)
```
Processing 10,000 transactions of $10.00 each

Expected total: $2,900.00
Actual total:   $2,899.95
Money lost:     $0.05
```

**"Only 5 cents!"** — Scale to 1 million transactions and you've lost $50.

### The Good Way (Integer Cents)
```
Processing 10,000 transactions of $10.00 each

Expected total: $2,900.00
Actual total:   $2,900.00
Money lost:     $0.00
```

**Perfect accuracy at any scale.**

## The Lesson

This is the compound error problem. Small mistakes accumulate:

- **Construction**: 1/16" off per wall → 1/4" off by wall 4 → door doesn't fit
- **Programming**: 0.005¢ lost per transaction → $50 lost over 1M transactions → books don't balance

**"Close enough" compounds into disaster.**

## Next Steps

1. Run the examples yourself
2. Modify transaction counts (try 100, 1000, 10000, 100000, 1000000)
3. Watch the error grow with scale
4. Read the [full README](./README.md)
5. Check out the [Craftsman's Manifesto](./manifesto.md)
6. Explore other modules (coming soon)

## The Philosophy

> "The bubble is either dead center or it's not. Everything else is accumulated slop."

This tutorial teaches programming through construction analogies because:

- Physical consequences make abstract concepts concrete
- Compound errors are easier to see in the real world
- "The ladder doesn't lie" — neither does the compiler
- Precision matters in both domains

**Graduate competent or get your suffering back.**

---

Questions? Found a bug? Want to contribute? Open an issue or PR!
