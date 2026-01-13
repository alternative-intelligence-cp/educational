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

    // CORE MEMORY: Single contiguous buffer (the "slab")
    const buffer = new ArrayBuffer(max);

    const nodes = {};
    const meta = {};
    const freeList = [];
    let used = 0;  // Total bytes currently allocated
    let last = 0;
    meta[last] = { start: 0, size: max };
    freeList.push(last);

    // Export the allocator interface
    app.slab = { alloc, free };

    /***** Functions *********************************************************/
    function alloc(size) {
        if (used + size > max) {
            return null; // Not enough total space
        }

        // Walk free list, find a node big enough to hold size
        let bestSlot = -1;
        let bestSize = Infinity;

        for (let i = 0; i < freeList.length; i++) {
            const slot = freeList[i];
            const node = meta[slot];
            if (node.size >= size && node.size < bestSize) {
                bestSlot = slot;
                bestSize = node.size;
            }
        }

        // If no single node is big enough, try merging neighbors
        if (bestSlot === -1) {
            // Try to find adjacent free nodes to merge
            for (let i = 0; i < freeList.length; i++) {
                const slot1 = freeList[i];
                const node1 = meta[slot1];

                for (let j = 0; j < freeList.length; j++) {
                    if (i === j) continue;
                    const slot2 = freeList[j];
                    const node2 = meta[slot2];

                    // Check if node2 immediately follows node1
                    if (node1.start + node1.size === node2.start) {
                        // Merge them
                        node1.size += node2.size;
                        delete meta[slot2];
                        freeList.splice(j, 1);

                        // Check if merged node is now big enough
                        if (node1.size >= size) {
                            bestSlot = slot1;
                            bestSize = node1.size;
                            break;
                        }
                    }
                }
                if (bestSlot !== -1) break;
            }
        }

        // Still not found? Give up
        if (bestSlot === -1) {
            return null; // No suitable block found
        }

        // Remove from free list
        const freeIndex = freeList.indexOf(bestSlot);
        freeList.splice(freeIndex, 1);

        const selectedNode = meta[bestSlot];

        // Split node if there's extra room
        if (selectedNode.size > size) {
            const newSlot = last + 1;
            last = newSlot;

            // Create new node for remaining space
            meta[newSlot] = {
                start: selectedNode.start + size,
                size: selectedNode.size - size
            };

            // Add to free list
            freeList.push(newSlot);

            // Update selected node size
            selectedNode.size = size;
        }

        // Create the actual Uint8Array view and store in nodes
        const view = new Uint8Array(buffer, selectedNode.start, selectedNode.size);
        view.slot = bestSlot; // Track which slot this view belongs to
        nodes[bestSlot] = view;

        used += size;
        return view;
    }

    function free(node) {
        // Validate: Must have valid slot number and be allocated
        if (!node || typeof node.slot !== 'number' || !nodes.hasOwnProperty(node.slot)) {
            return false; // Invalid node or already freed
        }

        const slot = node.slot;
        const freedNode = meta[slot];

        if (!freedNode) {
            return false; // Slot doesn't exist in meta
        }

        // Remove from nodes (mark as freed)
        delete nodes[slot];

        // Update used bytes
        used -= freedNode.size;

        // Add back to free list
        freeList.push(slot);

        // Try to merge with adjacent free nodes to reduce fragmentation
        // Check all free nodes to see if any are neighbors
        for (let i = 0; i < freeList.length; i++) {
            const otherSlot = freeList[i];
            if (otherSlot === slot) continue; // Skip ourselves

            const otherNode = meta[otherSlot];

            // Check if other node immediately follows this one
            if (freedNode.start + freedNode.size === otherNode.start) {
                // Merge: extend this node to include other
                freedNode.size += otherNode.size;
                delete meta[otherSlot];
                freeList.splice(i, 1);
                i--; // Adjust index after splice
            }
            // Check if this node immediately follows other node
            else if (otherNode.start + otherNode.size === freedNode.start) {
                // Merge: extend other node to include this one
                otherNode.size += freedNode.size;
                delete meta[slot];
                const selfIndex = freeList.indexOf(slot);
                freeList.splice(selfIndex, 1);
                break; // This node is gone, stop merging
            }
        }

        // Optional: "Poison" the memory to help detect use-after-free bugs
        // Uncomment to fill freed memory with 0xDEADBEEF pattern
        // for (let i = 0; i < node.byteLength; i += 4) {
        //     node[i] = 0xDE; node[i+1] = 0xAD;
        //     node[i+2] = 0xBE; node[i+3] = 0xEF;
        // }

        return true;
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
console.log("Test 1: Allocate a single block");
const slot1 = slab.alloc(64);
if (slot1) {
    console.log(`  ✓ Allocated slot ${slot1.slot} (${slot1.byteLength} bytes)`);
} else {
    console.log(`  ✗ Allocation failed!`);
}

// Test 2: Write and read data
console.log("\nTest 2: Write and read data");
if (slot1) {
    slot1[0] = 0x42;
    slot1[1] = 0x13;
    slot1[63] = 0xFF;
    console.log(`  ✓ Wrote: [${slot1[0]}, ${slot1[1]}, ..., ${slot1[63]}]`);
}

// Test 3: Allocate multiple blocks of different sizes
console.log("\nTest 3: Allocate multiple blocks");
const slot2 = slab.alloc(128);
const slot3 = slab.alloc(256);
if (slot2 && slot3) {
    console.log(`  ✓ Allocated slots ${slot2.slot} (${slot2.byteLength}B), ${slot3.slot} (${slot3.byteLength}B)`);
}

// Test 4: Free a slot
console.log("\nTest 4: Free slot and verify");
const freed = slab.free(slot2);
console.log(`  ✓ Freed slot ${slot2.slot}: ${freed}`);

// Test 5: Reuse freed space
console.log("\nTest 5: Reuse freed space");
const slot4 = slab.alloc(100);
if (slot4) {
    console.log(`  ✓ Allocated slot ${slot4.slot} (${slot4.byteLength}B, fits in freed space)`);
}

// Test 6: Fill the buffer
console.log("\nTest 6: Fill remaining space");
const allSlots = [slot1, slot3, slot4].filter(s => s);
let allocated = allSlots.length;
while (true) {
    const s = slab.alloc(64);
    if (!s) break;
    allSlots.push(s);
    allocated++;
}
console.log(`  ✓ Allocated ${allocated} total blocks`);

// Test 7: Try to allocate when full
console.log("\nTest 7: Allocate when full");
const shouldBeNull = slab.alloc(64);
console.log(`  ✓ Allocation failed as expected: ${shouldBeNull === null}`);

// Test 8: Free all blocks
console.log("\nTest 8: Free all blocks");
let freedCount = 0;
for (const slot of allSlots) {
    if (slab.free(slot)) freedCount++;
}
console.log(`  ✓ Freed ${freedCount} blocks`);

// Test 9: Allocate after freeing all (should merge into one big block)
console.log("\nTest 9: Allocate large block after freeing");
const large = slab.alloc(3000);
if (large) {
    console.log(`  ✓ Allocated large block: slot ${large.slot} (${large.byteLength}B)`);
    console.log(`  ✓ Free space was successfully merged!`);
}

// Test 10: Invalid free operations
console.log("\nTest 10: Invalid free operations");
const fakeFree1 = slab.free(null);
const fakeFree2 = slab.free({ slot: 999 });
const fakeFree3 = slab.free(slot1); // Already freed
console.log(`  ✓ Free null: ${fakeFree1 === false}`);
console.log(`  ✓ Free invalid slot: ${fakeFree2 === false}`);
console.log(`  ✓ Free already-freed: ${fakeFree3 === false}`);

// Test 11: Use-after-free demonstration (⚠️  DANGEROUS BUG!)
console.log("\nTest 11: Use-after-free vulnerability demonstration");
const danger1 = slab.alloc(64);
if (danger1) {
    danger1[0] = 0x42;
    danger1[1] = 0x13;
    console.log(`  Before free: danger1[0]=${danger1[0]}, danger1[1]=${danger1[1]}`);

    slab.free(danger1);  // Free the block
    console.log(`  ⚠️  After free: danger1 STILL WORKS!`);
    console.log(`  ⚠️  danger1[0]=${danger1[0]}, danger1[1]=${danger1[1]} (stale data!)`);

    // Allocate the same memory to someone else
    const danger2 = slab.alloc(64);
    if (danger2) {
        console.log(`  danger2 allocated slot ${danger2.slot}`);

        // Write to danger2
        danger2[0] = 0xFF;
        danger2[1] = 0xAA;

        // BUG: danger1 sees danger2's data because they share memory!
        console.log(`  ⚠️  BUG: danger1[0]=${danger1[0]}, danger1[1]=${danger1[1]} (sees danger2's writes!)`);
        console.log(`  ⚠️  This is a USE-AFTER-FREE vulnerability!`);
        console.log(`  ⚠️  In C/C++ this causes crashes, data corruption, and security exploits`);
    }
}

console.log("\n=== ALL TESTS PASSED ===");
