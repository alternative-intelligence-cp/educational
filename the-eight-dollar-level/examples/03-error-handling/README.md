# Module 3: Error Handling
## The Rafter That Doesn't Fit

### The Story

You cut the rafter. You measured twice, cut once. You climb the ladder with confidence. You hold it up to the beam and...

**It doesn't fit.**

Now you have two choices:

1. **The $8 Approach:** Panic. Try to force it. Maybe it'll work if you hit it with a hammer. Blame the lumber yard. Blame your tape measure. Eventually give up and cut another one, but you're rattled now so you screw that one up too.

2. **The $800 Approach:** Calmly climb down. Check your measurements. Check the beam. Realize the beam itself is warped by 1/4 inch. Make a note for next time. Cut a new rafter accounting for the warp. It fits perfectly.

**The difference?** One person treats unexpected problems as *catastrophes*. The other treats them as *information*.

### The Programming Parallel

Most code handles errors like the $8 carpenter - with panic:

```javascript
// 💥 Exception-based error handling
function processPayment(amount) {
    if (amount <= 0) {
        throw new Error("Invalid amount!");  // 🚨 PANIC MODE
    }
    // ... lots of code ...
    if (cardExpired) {
        throw new Error("Card expired!");  // 🚨 MORE PANIC
    }
    // ... even more code ...
}

try {
    processPayment(amount);
    console.log("Success!"); // Maybe? Who knows what happened?
} catch (e) {
    console.log("Something went wrong..."); // But what? Where? Why?
}
```

**Problems with exceptions:**
- They're invisible (not in the function signature)
- They can come from anywhere in the call stack
- They don't tell you *what kind* of error to expect
- They encourage catch-all error handling
- They're like a rafter that flies out of your hands and hits you in the face

### The $800 Way: Errors as Data

A good carpenter *expects* problems and has a system for handling them. Good code does the same:

```javascript
// ✅ Errors as return values
function processPayment(amount) {
    if (amount <= 0) {
        return { success: false, error: "INVALID_AMOUNT" };
    }
    
    if (cardExpired) {
        return { success: false, error: "CARD_EXPIRED" };
    }
    
    return { success: true, transactionId: "..." };
}

// Now errors are VISIBLE and EXPLICIT
const result = processPayment(amount);
if (!result.success) {
    switch (result.error) {
        case "INVALID_AMOUNT":
            showError("Please enter a valid amount");
            break;
        case "CARD_EXPIRED":
            showError("Your card has expired");
            redirectToPaymentMethods();
            break;
    }
} else {
    showSuccess(result.transactionId);
}
```

**Why this is better:**
- Errors are part of the type signature (you can't ignore them)
- You know exactly what can go wrong
- You handle each error appropriately
- No surprises hiding in the call stack
- Like checking the beam before cutting - you *expect* it might be warped

### The Construction Wisdom

When a rafter doesn't fit, a professional:

1. **Stays calm** - It's just information, not a catastrophe
2. **Investigates** - What's actually wrong? Beam warped? Wrong measurement? Wrong rafter?
3. **Plans** - What's the right fix? 
4. **Acts** - Cut a new one, shim the beam, adjust the plan
5. **Documents** - Makes a note so next time they check the beam first

When code encounters an error, professionals do the same:

1. **Return explicit error** - Not a mystery exception
2. **Provide context** - What went wrong, where, why
3. **Enable recovery** - Caller can decide what to do
4. **Log appropriately** - Leave a trail for next time
5. **Don't panic** - Errors are expected, handle them gracefully

### The Real Cost

**With exceptions ($8 level):**
- 2 hours debugging "but it worked yesterday!"
- Production crashes from unhandled exceptions
- Catch-all handlers that hide the real problem
- Try-catch blocks wrapped around everything "just in case"
- Developer stress because errors are invisible time bombs

**With explicit errors ($800 level):**
- Compiler forces you to handle errors
- Clear contracts: "this function can fail in these ways"
- Errors are just data - test them like any other output
- Production systems that fail gracefully
- Developer confidence because all failure modes are visible

### The Flip Test

Can you look at a function and immediately see what can go wrong?

```javascript
// ❌ FAILS FLIP TEST - Hidden failure modes
function updateUserProfile(userId, data) {
    const user = db.getUser(userId);
    user.update(data);
    return user;
}
// What if userId doesn't exist? What if db is down? Who knows!

// ✅ PASSES FLIP TEST - Explicit failure modes  
function updateUserProfile(userId, data) {
    const result = db.getUser(userId);
    if (!result.success) {
        return { success: false, error: result.error };
    }
    
    const updateResult = result.user.update(data);
    if (!updateResult.success) {
        return { success: false, error: updateResult.error };
    }
    
    return { success: true, user: updateResult.user };
}
// Crystal clear: can fail to find user, can fail to update
```

### What You'll Learn

In this module, we'll build the same file upload system two ways:

1. **bad.js** - Exception-based error handling (panic mode)
   - Mysterious failures
   - Catch-all error handlers
   - Production surprises

2. **good.js** - Explicit error handling (information mode)
   - Result types
   - Explicit error codes
   - Graceful degradation

3. **demo.html** - Interactive comparison
   - Watch exceptions bubble up unpredictably
   - See explicit errors handled systematically
   - Track error recovery paths

### Key Principles

1. **Errors are not exceptional** - They're expected parts of the system
2. **Make errors visible** - Put them in function signatures
3. **Make errors specific** - Provide actionable information
4. **Make errors recoverable** - Let callers decide what to do
5. **Make errors testable** - They're just data

### The Bottom Line

When a rafter doesn't fit, you don't throw it at the ground and storm off. You calmly figure out *why* it doesn't fit and *what* to do about it.

When code fails, you shouldn't throw exceptions and hope someone catches them. You should return explicit errors and handle them systematically.

**Exceptions are the $8 carpenter hitting things with a hammer.**  
**Result types are the $800 carpenter checking the beam first.**

The ladder doesn't lie. Neither do explicit error codes.

---

**Next:** Run the examples and watch exceptions create chaos while explicit errors create clarity.
