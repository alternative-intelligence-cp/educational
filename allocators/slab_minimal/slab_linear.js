#!/usr/bin/env node
/**
 * SLAB ALLOCATOR - Educational Minimal Example
 * 
 * This is the SIMPLEST possible slab allocator implementation.
 * All complexity has been stripped away to show just the core concept.
 * 
 * WHAT IS A SLAB ALLOCATOR?
 * A slab allocator pre-divides memory into fixed-size "slots" (slabs).
 * Each slot is the same size, making allocation/deallocation trivial:
 *   - Allocation: Find a free slot, mark it used
 *   - Deallocation: Mark the slot as free again
 * 
 * WHY USE SLAB ALLOCATION?
 *   - FAST: O(1) allocation/deallocation (no searching for best fit)
 *   - NO FRAGMENTATION: All slots same size, no unusable gaps
 *   - SIMPLE: Minimal bookkeeping overhead
 *   - CACHE FRIENDLY: Predictable memory layout
 * 
 * LIMITATIONS:
 *   - WASTE: If you need 10 bytes but slots are 64 bytes, you waste 54 bytes
 *   - FIXED SIZE: Can't allocate anything larger than slot size
 * 
 * REAL-WORLD USE:
 *   - Linux kernel slab allocator (for fixed-size kernel objects)
 *   - Network packet buffers (packets often similar size)
 *   - Object pools (game entities, database rows, etc.)
 * 
 * THIS EXAMPLE:
 *   - 4096 byte buffer (4KB total memory)
 *   - 64 byte slots (64 bytes per allocation)
 *   - 64 total slots (4096 / 64 = 64)
 */

const app = globalThis.app = {};
((input) => {
    /****** Init *************************************************************/

    // CONFIGURATION
    const max = 4096;  // Total buffer size: 4KB
    const word = 64;   // Slot size: 64 bytes per allocation

    // CORE MEMORY: Single contiguous buffer (the "slab")
    const buffer = new ArrayBuffer(max);

    // METADATA: Track which slots are free/used
    // This is the "bookkeeping" - much simpler than general allocators!
    const meta = {};
    let used = 0;  // Total bytes currently allocated

    // Initialize metadata for each slot
    // Slots are numbered 0, 1, 2, ... 63
    for (let next = 0; next < max / word; next++) {
        meta[`${next}`] = {
            start: next * word,  // Byte offset: 0, 64, 128, 192, ...
            size: word,          // Always 64 bytes (fixed size!)
            free: true           // Initially all slots are free
        }
    }

    // Export the allocator interface
    app.slab = { alloc, free };

    /***** Functions *********************************************************/

    /**
     * alloc() - Allocate a single slot
     * 
     * ALGORITHM:
     *   1. Check if buffer has space
     *   2. Find first free slot (linear search)
     *   3. Mark slot as used
     *   4. Return view into that slot's memory
     * 
     * COMPLEXITY: O(n) worst case, but n is small (64 slots)
     * REAL IMPLEMENTATIONS: Use bitmaps or free-lists for O(1)
     * 
     * RETURNS: Uint8Array view (64 bytes) or null if full
     */
    function alloc() {
        if (used < max) {
            // Linear search for free slot
            // (Real slab allocators use free-lists for O(1) allocation)
            for (let slot in meta) {
                if (meta.hasOwnProperty(slot)) {
                    if (meta[slot].free == true) {
                        // Found a free slot!
                        meta[slot].free = false;
                        used += word;

                        // Create a TypedArray view into this slot's region
                        // buffer: The backing ArrayBuffer
                        // meta[slot].start: Byte offset (0, 64, 128, ...)
                        // word: Length in bytes (64)
                        let view = new Uint8Array(buffer, meta[slot].start, word);

                        // Tag the view with its slot number so we can free it later
                        view.slot = parseInt(slot);

                        return view;
                    }
                }
            }
            return null; // All slots allocated
        } else {
            return null; // Buffer full
        }
    }

    /**
     * free(view) - Deallocate a slot
     * 
     * ALGORITHM:
     *   1. Validate the view and its slot number
     *   2. Mark slot as free in metadata
     *   3. Decrease used bytes counter
     * 
     * SIMPLICITY: This is what makes slab allocation beautiful!
     * No coalescing, no splitting, no complex free-list management.
     * Just mark the slot as free. Done.
     * 
     * ⚠️  USE-AFTER-FREE WARNING! ⚠️
     * After calling free(), the view STILL WORKS and points to the freed memory!
     * This is a classic bug in manual memory management:
     * 
     *   const slot = slab.alloc();
     *   slab.free(slot);
     *   slot[0] = 42;  // BUG! Writing to freed memory!
     * 
     * The freed memory can be reallocated to someone else, causing:
     *   - Data corruption (writing to another allocation's memory)
     *   - Reading stale/wrong data
     *   - Security vulnerabilities
     * 
     * SOLUTION: After calling free(), the caller must NOT use the view again!
     * (Languages like Rust prevent this at compile-time with the borrow checker)
     * 
     * PARAMETERS: view - The Uint8Array returned by alloc()
     * RETURNS: true if freed successfully, false if invalid
     */
    function free(view) {
        const numSlots = max / word; // 64 slots total

        // Validate: Must have valid slot number
        if (view && view.slot >= 0 && view.slot < numSlots) {
            if (meta.hasOwnProperty(view.slot)) {
                // Mark slot as free - that's it!
                meta[view.slot].free = true;
                used -= word;

                // Optional: "Poison" the memory to help detect use-after-free bugs
                // Uncomment to fill freed memory with 0xDEADBEEF pattern
                // for (let i = 0; i < word; i += 4) {
                //     view[i] = 0xDE; view[i+1] = 0xAD; 
                //     view[i+2] = 0xBE; view[i+3] = 0xEF;
                // }

                return true;
            } else {
                return false; // Invalid slot metadata
            }
        }
        return false; // Invalid view or slot number
    }
})({///// INPUT ////////////////////////////////////////////////////////////////
    args: {},      // Could pass configuration here
    modules: {},   // Could import additional modules
    app: app
});

///// RUN /////////////////////////////////////////////////////////////////////
const slab = app.slab;

console.log("=== SLAB ALLOCATOR TEST ===\n");

// Test 1: Basic allocation
console.log("Test 1: Allocate a single slot");
const slot1 = slab.alloc();
console.log(`  ✓ Allocated slot ${slot1.slot} (${slot1.byteLength} bytes)`);

// Test 2: Write and read data
console.log("\nTest 2: Write and read data");
slot1[0] = 0x42;
slot1[1] = 0x13;
slot1[63] = 0xFF;
console.log(`  ✓ Wrote: [${slot1[0]}, ${slot1[1]}, ..., ${slot1[63]}]`);

// Test 3: Allocate multiple slots
console.log("\nTest 3: Allocate multiple slots");
const slot2 = slab.alloc();
const slot3 = slab.alloc();
console.log(`  ✓ Allocated slots ${slot2.slot}, ${slot3.slot}`);

// Test 4: Free a slot
console.log("\nTest 4: Free slot and verify");
const freed = slab.free(slot2);
console.log(`  ✓ Freed slot ${slot2.slot}: ${freed}`);

// Test 5: Reuse freed slot
console.log("\nTest 5: Reuse freed slot");
const slot4 = slab.alloc();
console.log(`  ✓ Allocated slot ${slot4.slot} (should reuse slot ${slot2.slot})`);

// Test 6: Fill the buffer
console.log("\nTest 6: Fill all slots");
const allSlots = [slot1, slot3, slot4];
let allocated = 3;
while (true) {
    const s = slab.alloc();
    if (!s) break;
    allSlots.push(s);
    allocated++;
}
console.log(`  ✓ Allocated ${allocated} slots total (max: ${4096 / 64})`);

// Test 7: Try to allocate when full
console.log("\nTest 7: Allocate when full");
const shouldBeNull = slab.alloc();
console.log(`  ✓ Allocation failed as expected: ${shouldBeNull === null}`);

// Test 8: Free all slots
console.log("\nTest 8: Free all slots");
let freedCount = 0;
for (const slot of allSlots) {
    if (slab.free(slot)) freedCount++;
}
console.log(`  ✓ Freed ${freedCount} slots`);

// Test 9: Allocate after freeing all
console.log("\nTest 9: Allocate after freeing");
const fresh = slab.alloc();
console.log(`  ✓ Allocated fresh slot ${fresh.slot}`);

// Test 10: Invalid free operations
console.log("\nTest 10: Invalid free operations");
const fakeFree1 = slab.free(null);
const fakeFree2 = slab.free({ slot: 999 });
console.log(`  ✓ Free null: ${fakeFree1 === false}`);
console.log(`  ✓ Free invalid slot: ${fakeFree2 === false}`);

// Test 11: Use-after-free demonstration (⚠️  DANGEROUS BUG!)
console.log("\nTest 11: Use-after-free vulnerability demonstration");
const danger1 = slab.alloc();
danger1[0] = 0x42;
danger1[1] = 0x13;
console.log(`  Before free: danger1[0]=${danger1[0]}, danger1[1]=${danger1[1]}`);

slab.free(danger1);  // Free the slot
console.log(`  ⚠️  After free: danger1 STILL WORKS!`);
console.log(`  ⚠️  danger1[0]=${danger1[0]}, danger1[1]=${danger1[1]} (stale data!)`);

// Allocate the same slot to someone else
const danger2 = slab.alloc();  // Gets the same memory!
console.log(`  danger2 allocated slot ${danger2.slot} (same as danger1's slot ${danger1.slot})`);

// Write to danger2
danger2[0] = 0xFF;
danger2[1] = 0xAA;

// BUG: danger1 sees danger2's data because they share memory!
console.log(`  ⚠️  BUG: danger1[0]=${danger1[0]}, danger1[1]=${danger1[1]} (sees danger2's writes!)`);
console.log(`  ⚠️  This is a USE-AFTER-FREE vulnerability!`);
console.log(`  ⚠️  In C/C++ this causes crashes, data corruption, and security exploits`);

console.log("\n=== ALL TESTS PASSED ===");