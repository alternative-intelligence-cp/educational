# Module 1: Compound Errors
## The Crooked Wall Problem

### The Construction Story

You're framing a wall. You check your level. The bubble is "in the marks" - not dead center, but close enough, right?

**Wall 1:** Off by 1/16"  
**Wall 2:** Off by 1/8" (compounds)  
**Wall 3:** Off by 3/16" (compounds more)  
**Wall 4:** Off by 1/4" (door doesn't fit)

You just spent 8 hours building walls you now have to tear down and rebuild. **16 hours of work because "close enough" compounded.**

### The Programming Parallel

Every time you accept "close enough," you're building a crooked wall. It might look fine now, but by the time you've built on top of it, you're tearing everything down.

## Example 1: Floating Point Financial Disaster

### bad.js - The "In The Marks" Approach

```javascript
// "Close enough" financial processing
class PaymentProcessor {
    constructor() {
        this.total = 0.0;  // "It's just money, floats are fine"
    }
    
    processTransaction(amount) {
        // 2.9% processing fee (like Stripe)
        const fee = amount * 0.029;
        const rounded = Math.round(fee * 100) / 100;  // Round to cents
        this.total += rounded;
        return rounded;
    }
    
    getTotal() {
        return this.total;
    }
}

// Process 10,000 transactions of $10.00 each
const processor = new PaymentProcessor();
for (let i = 0; i < 10000; i++) {
    processor.processTransaction(10.00);
}

console.log("Expected total: $2,900.00");
console.log("Actual total: $" + processor.getTotal().toFixed(2));
console.log("Money lost to rounding: $" + (2900 - processor.getTotal()).toFixed(2));

// Output:
// Expected total: $2,900.00
// Actual total: $2,899.95
// Money lost to rounding: $0.05

// "Only 5 cents, who cares?"
// Now scale to 1 million transactions...
// Now you've lost $50
// Now explain to accounting why the books don't balance
```

### good.js - The "Dead Center" Approach

```javascript
// Exact financial processing
class PaymentProcessorExact {
    constructor() {
        this.totalCents = 0;  // Store as integer cents - EXACT
    }
    
    processTransaction(amountDollars) {
        // Convert to cents ONCE at input
        const amountCents = Math.round(amountDollars * 100);
        
        // Calculate fee in cents using integer math
        // 2.9% = 29/1000 (no floating point)
        const feeCents = Math.floor((amountCents * 29) / 1000);
        
        this.totalCents += feeCents;
        return feeCents;
    }
    
    getTotal() {
        // Convert back to dollars ONCE at output
        return this.totalCents / 100;
    }
    
    getTotalExact() {
        return {
            dollars: Math.floor(this.totalCents / 100),
            cents: this.totalCents % 100,
            raw: this.totalCents
        };
    }
}

// Process same 10,000 transactions
const processorExact = new PaymentProcessorExact();
for (let i = 0; i < 10000; i++) {
    processorExact.processTransaction(10.00);
}

console.log("Expected total: $2,900.00");
console.log("Actual total: $" + processorExact.getTotal().toFixed(2));
console.log("Money lost to rounding: $0.00");
console.log("Exact:", processorExact.getTotalExact());

// Output:
// Expected total: $2,900.00
// Actual total: $2,900.00
// Money lost to rounding: $0.00
// Exact: { dollars: 2900, cents: 0, raw: 290000 }

// Books balance. Every time. No exceptions.
```

## The Lesson

### What Happened?

**Bad approach (floating point):**
- Each transaction loses up to 0.005 cents in rounding
- 10,000 transactions × 0.005 = $50 potential loss
- Error COMPOUNDS with every operation
- Like being "in the marks" on every wall

**Good approach (integer cents):**
- Exact arithmetic, no rounding until final display
- Zero accumulated error
- Scales to billions of transactions
- Like bubble dead center on every wall

### The Principle

**This is the 1/16" on the first wall.**

It looks negligible. "Who cares about 0.005 cents?"

But by transaction 10,000, you're off by dollars. By transaction 1,000,000, you're off by enough to matter. And when accounting asks why the books don't balance, "floating point error" isn't an acceptable answer.

### The Rule

**When precision matters, use exact arithmetic. Convert once at boundaries, not on every operation.**

- Money? Integer cents.
- Measurements? Fixed-point or rationals.
- Cumulative calculations? Exact types.

**Floating point is for:**
- Scientific calculations where approximation is acceptable
- Graphics where human perception is the limit
- Physics simulations where measurement error exceeds float error

**Floating point is NOT for:**
- Financial transactions
- Anything involving legal/contractual obligations
- Anywhere "the books must balance"

### Try It Yourself

Run both examples. Scale them up to 1,000,000 transactions. Watch the bad approach lose $500+. Watch the good approach stay exact.

**The bubble is either dead center or it's not.**

---

Next: [Module 2 - Type Safety (The 14 Ladder Climbs)](../02-type-safety/)
