# Minimal Slab Allocator - Educational Example

## Overview

This is the **simplest possible slab allocator** you can build. All complexity has been stripped away to reveal just the core concept.

**Purpose**: Educational - Understand slab allocation before diving into advanced implementations.

**Three Implementations Provided:**
- `slab_linear.js` - Linear search version (lazy allocation, O(n) lookup)
- `slab_freelist.js` - Free list version (pre-allocation, O(1) lookup)
- `slab_dynamic.js` - **NEW!** Dynamic-size allocator (variable sizes, block splitting/merging)
- `benchmark.js` - Performance comparison between approaches

## What is a Slab Allocator?

A slab allocator pre-divides memory into **fixed-size slots** (called "slabs"). 

### Core Concept
```
Memory Buffer (4096 bytes):
┌────────┬────────┬────────┬────────┬─────┬────────┐
│ Slot 0 │ Slot 1 │ Slot 2 │ Slot 3 │ ... │ Slot 63│
│ 64 B   │ 64 B   │ 64 B   │ 64 B   │     │ 64 B   │
└────────┴────────┴────────┴────────┴─────┴────────┘
```

- **Allocation**: Find a free slot, mark it used, return pointer
- **Deallocation**: Mark the slot as free again

That's it. No searching for best-fit, no coalescing, no splitting.

## Why This Example?

Other allocator examples handle variable sizes, which adds complexity:
- Size classes
- Free lists per size
- Metadata management
- Fragmentation handling

**This example removes ALL of that.** Every allocation is exactly 64 bytes. This lets you see the fundamental slab allocation pattern without distractions.

## Key Features

### ✅ Advantages
- **O(1) deallocation** - Just mark slot as free
- **No fragmentation** - All slots same size
- **Dead simple** - ~100 lines including tests
- **Cache friendly** - Predictable memory layout

### ❌ Limitations  
- **Wastes space** - Need 10 bytes? Get 64, waste 54
- **Fixed size only** - Can't allocate > 64 bytes

## Two Implementations

### Linear Search Version (`slab_linear.js`)
- **O(n) allocation** - Searches metadata object for free slot
- **O(1) deallocation** - Direct metadata update
- **Lazy initialization** - Creates TypedArray views on-demand
- **Lower memory overhead** - Only stores metadata upfront
- **Degrades with fragmentation** - More used slots = slower allocation

### Free List Version (`slab_freelist.js`)
- **O(1) allocation** - Array shift from free list
- **O(1) deallocation** - Array push to free list
- **Pre-initialization** - Creates all TypedArray views upfront
- **Higher memory overhead** - Stores all views in memory
- **Predictable performance** - Constant time regardless of usage

### Performance Comparison

Run the benchmark to see the difference:
```bash
node benchmark.js
```

**Typical Results** (64 slots, 10K operations):
- Allocation: Free list **6.3x faster**
- Reallocation: Free list **10x faster**
- Heavy churn: Free list **3.2x faster**
- Overall: Free list **262% faster**

The free list pays a tiny initialization cost (~9μs more) but dramatically outperforms linear search in all allocation scenarios.

## Configuration

```javascript
const max = 4096;  // Total buffer: 4KB
const word = 64;   // Slot size: 64 bytes
// Result: 64 slots (4096 / 64 = 64)
```

## Usage

```javascript
// Allocate a slot (64 bytes)
const slot = slab.alloc();
if (!slot) {
    console.log("Out of memory!");
}

// Use the memory
slot[0] = 0x42;
slot[63] = 0xFF;

// Free the slot
slab.free(slot);

// Slot can now be reused
const reused = slab.alloc();  // Gets same slot back
```

## Running

```bash
# Run linear search version
node slab_linear.js

# Run free list version
node slab_freelist.js

# Compare performance
node benchmark.js
```

Both versions output **11 tests** demonstrating:
- Basic allocation/deallocation
- Memory reuse after free
- Buffer filling (all 64 slots)
- Handling allocation failures
- Invalid free operations
- **⚠️ Use-after-free vulnerability demonstration** (Test 11)

### ⚠️ Important: Use-After-Free Vulnerability

Test 11 demonstrates a critical bug in manual memory management:

**After freeing a slot, the view STILL WORKS!**

```javascript
const slot = slab.alloc();
slot[0] = 0x42;
slab.free(slot);

// BUG: slot still points to freed memory!
console.log(slot[0]);  // Still reads 0x42
slot[0] = 0xFF;        // Still writes! Dangerous!

// Worse: If someone else allocates the same slot...
const slot2 = slab.alloc();  // Gets same memory!
// Now slot and slot2 point to SAME memory
// They can corrupt each other's data!
```

**Why can't we prevent this?**
- In JavaScript, we can't "invalidate" the view from inside `free()`
- Setting `view = null` only nullifies the parameter, not the caller's variable
- The Uint8Array still points to the valid ArrayBuffer region

**Real-world consequences:**
- **C/C++**: Crashes, segmentation faults, security exploits
- **Rust**: Prevented at compile-time by the borrow checker!
- **This example**: Shows why manual memory management requires discipline

**The fix**: Programmers must discipline themselves to never use a view after freeing it. This is why languages like Rust enforce memory safety at compile-time.

## Real-World Slab Allocators

This minimal example teaches the concept. Production slab allocators add:

1. **Free Lists** - O(1) allocation instead of linear search
2. **Multiple Sizes** - Several slab pools (64B, 128B, 256B, etc.)
3. **Slab Caching** - CPU-local caches for multicore
4. **Bulk Operations** - Allocate/free multiple slabs at once
5. **Statistics** - Track usage, fragmentation, performance

### Examples in This Repo:
- **SegFit Advanced** - Production allocator with 15 size classes
- **Qwen3 Improved** - Educational allocator with 16 size classes
- **ApexAlloc** - Consciousness-synchronized production allocator

### Industry Examples:
- **Linux kernel** - SLUB/SLAB allocators for kernel objects
- **jemalloc** - Uses slab allocation for small objects
- **tcmalloc** - Thread-caching with slab-based pools

## Learning Path

1. **Start here** - Understand fixed-size slab allocation
2. **qwen3_improved/** - Multiple size classes with detailed logging
3. **segfit_advanced/** - Production-ready segregated fit allocator
4. **apexalloc/** - Consciousness computing with Tesla frequency sync

## Why JavaScript?

JavaScript with `ArrayBuffer` and `TypedArray` makes memory management **visible** without pointer arithmetic getting in the way. You can see:
- The backing buffer
- Byte offsets
- Memory views
- No hidden malloc/free

Perfect for learning before implementing in C/C++.

## Key Takeaway

**Slab allocation is beautifully simple**: Pre-divide memory into fixed slots, track free/used, done.

The complexity in "real" allocators comes from:
- Supporting variable sizes
- Performance optimization (O(1) allocation)
- Fragmentation management
- Multi-threading

But the core idea? That's right here in ~100 lines.

---

## NEW: Dynamic-Size Allocator (`slab_dynamic.js`)

Unlike the fixed-size versions above, this implementation handles **variable-size allocations**. It's a hybrid between a slab allocator and a buddy allocator.

### Key Features

**Variable-Size Blocks:**
- Allocate any size from 1 byte to 4096 bytes
- No wasted space for small allocations
- Blocks split to exact requested size (+metadata)

**Block Splitting:**
```javascript
alloc(100);  // Splits 4096-byte block into:
             // - 100-byte allocated block
             // - 3996-byte free block (returned to free list)
```

**Block Merging (Coalescing):**
```javascript
free(block1);  // Automatically merges with adjacent free blocks
free(block2);  // Prevents fragmentation
// Adjacent blocks → Single large block
```

**Best-Fit Allocation:**
- Searches free list for smallest block that fits
- Minimizes wasted space
- Reduces external fragmentation

### Metadata Structure

Each block tracks:
```javascript
meta[slot] = {
    start: 0,      // Byte offset in buffer
    size: 4096     // Block size in bytes
}
```

### Performance Characteristics

- **Allocation**: O(n) - walks free list to find best fit
- **Deallocation**: O(n²) - merges with all adjacent neighbors
- **Space Efficiency**: Excellent - minimal waste
- **Fragmentation**: Low - aggressive merging prevents buildup

### When to Use Each Version

| Scenario | Use This |
|----------|----------|
| Fixed-size objects (64B) | `slab_freelist.js` - Fastest |
| Fixed-size, memory-limited | `slab_linear.js` - Low overhead |
| Variable sizes, low fragmentation | `slab_dynamic.js` - Most flexible |
| Need exact-size allocations | `slab_dynamic.js` |
| Maximum performance | `slab_freelist.js` |

### Example Usage

```javascript
const slab = app.slab;

// Allocate different sizes
const small = slab.alloc(10);    // 10 bytes
const medium = slab.alloc(256);  // 256 bytes
const large = slab.alloc(1000);  // 1000 bytes

// Write data
small[0] = 0x42;
medium[100] = 0xFF;

// Free blocks (auto-merges neighbors)
slab.free(medium);
slab.free(small);
slab.free(large);  // All merged back into one 4096-byte block!
```

### Educational Value

The dynamic version teaches:
1. **Block Splitting** - How to divide large blocks into smaller ones
2. **Coalescing** - Merging adjacent free blocks to prevent fragmentation
3. **Best-Fit Search** - Finding the optimal block for each allocation
4. **Metadata Management** - Tracking block boundaries and sizes
5. **Fragmentation** - Understanding internal vs external fragmentation

Run `node slab_dynamic.js` to see all features in action, including the use-after-free vulnerability demonstration (educational security bug showcase).

---
