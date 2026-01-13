#!/usr/bin/env node
/**
 * SLAB ALLOCATOR BENCHMARK COMPARISON
 * 
 * Compares two slab allocator implementations:
 * 1. Free List version (pre-allocates all slots, O(1) alloc/free)
 * 2. Linear Search version (lazy allocation, O(n) alloc/free)
 * 
 * Metrics measured separately:
 * - Initialization time (one-time cost)
 * - Allocation speed (allocate all slots)
 * - Deallocation speed (free all slots)
 * - Reallocation speed (allocate after freeing)
 * - Memory overhead
 */

///// FREE LIST VERSION ///////////////////////////////////////////////////////

function createFreeListAllocator() {
    const max = 4096;
    const slotSize = 64;
    const numSlots = max / slotSize;
    const buffer = new ArrayBuffer(max);
    const nodes = {};
    const freeNodes = [];
    let used = 0;

    // Pre-initialize all slots
    for (let next = 0; next < numSlots; next++) {
        let node = new Uint8Array(buffer, next * slotSize, slotSize);
        node.slot = next;
        nodes[node.slot] = node;
        freeNodes.push(node.slot);
    }

    return {
        alloc() {
            if (used < max) {
                let slot = freeNodes.shift();
                used += slotSize;
                return nodes[slot];
            }
            return null;
        },
        free(node) {
            if (node && node.slot >= 0 && node.slot < numSlots) {
                if (nodes.hasOwnProperty(node.slot)) {
                    freeNodes.push(node.slot);
                    used -= slotSize;
                    return true;
                }
            }
            return false;
        },
        getStats() {
            return {
                used,
                free: freeNodes.length,
                total: numSlots
            };
        }
    };
}

///// LINEAR SEARCH VERSION ///////////////////////////////////////////////////

function createLinearSearchAllocator() {
    const max = 4096;
    const word = 64;
    const buffer = new ArrayBuffer(max);
    const meta = {};
    let used = 0;

    // Lazy initialization - just create metadata
    for (let next = 0; next < max / word; next++) {
        meta[`${next}`] = {
            start: next * word,
            size: word,
            free: true
        };
    }

    return {
        alloc() {
            if (used < max) {
                for (let slot in meta) {
                    if (meta.hasOwnProperty(slot)) {
                        if (meta[slot].free == true) {
                            meta[slot].free = false;
                            used += word;
                            let view = new Uint8Array(buffer, meta[slot].start, word);
                            view.slot = parseInt(slot);
                            return view;
                        }
                    }
                }
                return null;
            }
            return null;
        },
        free(view) {
            const numSlots = max / word;
            if (view && view.slot >= 0 && view.slot < numSlots) {
                if (meta.hasOwnProperty(view.slot)) {
                    meta[view.slot].free = true;
                    used -= word;
                    return true;
                }
            }
            return false;
        },
        getStats() {
            let free = 0;
            for (let slot in meta) {
                if (meta[slot].free) free++;
            }
            return {
                used,
                free,
                total: max / word
            };
        }
    };
}

///// BENCHMARK UTILITIES /////////////////////////////////////////////////////

function benchmark(name, fn) {
    const start = process.hrtime.bigint();
    fn();
    const end = process.hrtime.bigint();
    const elapsed = Number(end - start) / 1_000_000; // Convert to milliseconds
    return elapsed;
}

function formatTime(ms) {
    if (ms < 1) return `${(ms * 1000).toFixed(3)} μs`;
    if (ms < 1000) return `${ms.toFixed(3)} ms`;
    return `${(ms / 1000).toFixed(3)} s`;
}

function formatOpsPerSec(ops, timeMs) {
    const opsPerSec = (ops / timeMs) * 1000;
    if (opsPerSec > 1_000_000) return `${(opsPerSec / 1_000_000).toFixed(2)}M ops/sec`;
    if (opsPerSec > 1_000) return `${(opsPerSec / 1_000).toFixed(2)}K ops/sec`;
    return `${opsPerSec.toFixed(2)} ops/sec`;
}

///// BENCHMARK TESTS /////////////////////////////////////////////////////////

console.log("╔═══════════════════════════════════════════════════════════════╗");
console.log("║         SLAB ALLOCATOR BENCHMARK COMPARISON                   ║");
console.log("╚═══════════════════════════════════════════════════════════════╝\n");

// BENCHMARK 1: Initialization Time
console.log("─────────────────────────────────────────────────────────────────");
console.log("BENCHMARK 1: Initialization Time (one-time cost)");
console.log("─────────────────────────────────────────────────────────────────");

let freeListAllocator, linearSearchAllocator;

const initTimeFreeList = benchmark("Free List Init", () => {
    freeListAllocator = createFreeListAllocator();
});

const initTimeLinear = benchmark("Linear Search Init", () => {
    linearSearchAllocator = createLinearSearchAllocator();
});

console.log(`  Free List:      ${formatTime(initTimeFreeList)}`);
console.log(`  Linear Search:  ${formatTime(initTimeLinear)}`);
console.log(`  Difference:     ${formatTime(Math.abs(initTimeFreeList - initTimeLinear))}`);
console.log(`  Winner:         ${initTimeLinear < initTimeFreeList ? 'Linear Search' : 'Free List'} ` +
            `(${((Math.max(initTimeFreeList, initTimeLinear) / Math.min(initTimeFreeList, initTimeLinear) - 1) * 100).toFixed(1)}% faster)\n`);

// BENCHMARK 2: First Allocation (allocate all 64 slots)
console.log("─────────────────────────────────────────────────────────────────");
console.log("BENCHMARK 2: Allocate All Slots (64 allocations)");
console.log("─────────────────────────────────────────────────────────────────");

const slots1 = [];
const allocTimeFreeList = benchmark("Free List Alloc", () => {
    for (let i = 0; i < 64; i++) {
        slots1.push(freeListAllocator.alloc());
    }
});

const slots2 = [];
const allocTimeLinear = benchmark("Linear Search Alloc", () => {
    for (let i = 0; i < 64; i++) {
        slots2.push(linearSearchAllocator.alloc());
    }
});

console.log(`  Free List:      ${formatTime(allocTimeFreeList)} (${formatOpsPerSec(64, allocTimeFreeList)})`);
console.log(`  Linear Search:  ${formatTime(allocTimeLinear)} (${formatOpsPerSec(64, allocTimeLinear)})`);
console.log(`  Speedup:        ${(Math.max(allocTimeFreeList, allocTimeLinear) / Math.min(allocTimeFreeList, allocTimeLinear)).toFixed(2)}x`);
console.log(`  Winner:         ${allocTimeLinear < allocTimeFreeList ? 'Linear Search' : 'Free List'}\n`);

// BENCHMARK 3: Deallocation (free all 64 slots)
console.log("─────────────────────────────────────────────────────────────────");
console.log("BENCHMARK 3: Deallocate All Slots (64 frees)");
console.log("─────────────────────────────────────────────────────────────────");

const freeTimeFreeList = benchmark("Free List Free", () => {
    for (const slot of slots1) {
        freeListAllocator.free(slot);
    }
});

const freeTimeLinear = benchmark("Linear Search Free", () => {
    for (const slot of slots2) {
        linearSearchAllocator.free(slot);
    }
});

console.log(`  Free List:      ${formatTime(freeTimeFreeList)} (${formatOpsPerSec(64, freeTimeFreeList)})`);
console.log(`  Linear Search:  ${formatTime(freeTimeLinear)} (${formatOpsPerSec(64, freeTimeLinear)})`);
console.log(`  Speedup:        ${(Math.max(freeTimeFreeList, freeTimeLinear) / Math.min(freeTimeFreeList, freeTimeLinear)).toFixed(2)}x`);
console.log(`  Winner:         ${freeTimeLinear < freeTimeFreeList ? 'Linear Search' : 'Free List'}\n`);

// BENCHMARK 4: Reallocation (allocate after freeing)
console.log("─────────────────────────────────────────────────────────────────");
console.log("BENCHMARK 4: Reallocation (allocate → free → allocate, 1000 cycles)");
console.log("─────────────────────────────────────────────────────────────────");

const reallocTimeFreeList = benchmark("Free List Realloc", () => {
    for (let i = 0; i < 1000; i++) {
        const s = freeListAllocator.alloc();
        freeListAllocator.free(s);
    }
});

const reallocTimeLinear = benchmark("Linear Search Realloc", () => {
    for (let i = 0; i < 1000; i++) {
        const s = linearSearchAllocator.alloc();
        linearSearchAllocator.free(s);
    }
});

console.log(`  Free List:      ${formatTime(reallocTimeFreeList)} (${formatOpsPerSec(1000, reallocTimeFreeList)})`);
console.log(`  Linear Search:  ${formatTime(reallocTimeLinear)} (${formatOpsPerSec(1000, reallocTimeLinear)})`);
console.log(`  Speedup:        ${(Math.max(reallocTimeFreeList, reallocTimeLinear) / Math.min(reallocTimeFreeList, reallocTimeLinear)).toFixed(2)}x`);
console.log(`  Winner:         ${reallocTimeLinear < reallocTimeFreeList ? 'Linear Search' : 'Free List'}\n`);

// BENCHMARK 5: Heavy Churn (many alloc/free operations)
console.log("─────────────────────────────────────────────────────────────────");
console.log("BENCHMARK 5: Heavy Churn (10,000 mixed alloc/free operations)");
console.log("─────────────────────────────────────────────────────────────────");

// Reset allocators
freeListAllocator = createFreeListAllocator();
linearSearchAllocator = createLinearSearchAllocator();

const churnTimeFreeList = benchmark("Free List Churn", () => {
    const active = [];
    for (let i = 0; i < 10000; i++) {
        if (Math.random() > 0.5 && active.length < 64) {
            // Allocate
            active.push(freeListAllocator.alloc());
        } else if (active.length > 0) {
            // Free random slot
            const idx = Math.floor(Math.random() * active.length);
            freeListAllocator.free(active[idx]);
            active.splice(idx, 1);
        }
    }
    // Cleanup
    for (const s of active) freeListAllocator.free(s);
});

const churnTimeLinear = benchmark("Linear Search Churn", () => {
    const active = [];
    for (let i = 0; i < 10000; i++) {
        if (Math.random() > 0.5 && active.length < 64) {
            // Allocate
            active.push(linearSearchAllocator.alloc());
        } else if (active.length > 0) {
            // Free random slot
            const idx = Math.floor(Math.random() * active.length);
            linearSearchAllocator.free(active[idx]);
            active.splice(idx, 1);
        }
    }
    // Cleanup
    for (const s of active) linearSearchAllocator.free(s);
});

console.log(`  Free List:      ${formatTime(churnTimeFreeList)} (${formatOpsPerSec(10000, churnTimeFreeList)})`);
console.log(`  Linear Search:  ${formatTime(churnTimeLinear)} (${formatOpsPerSec(10000, churnTimeLinear)})`);
console.log(`  Speedup:        ${(Math.max(churnTimeFreeList, churnTimeLinear) / Math.min(churnTimeFreeList, churnTimeLinear)).toFixed(2)}x`);
console.log(`  Winner:         ${churnTimeLinear < churnTimeFreeList ? 'Linear Search' : 'Free List'}\n`);

// SUMMARY
console.log("╔═══════════════════════════════════════════════════════════════╗");
console.log("║                         SUMMARY                               ║");
console.log("╚═══════════════════════════════════════════════════════════════╝\n");

const totalTimeFreeList = initTimeFreeList + allocTimeFreeList + freeTimeFreeList + reallocTimeFreeList + churnTimeFreeList;
const totalTimeLinear = initTimeLinear + allocTimeLinear + freeTimeLinear + reallocTimeLinear + churnTimeLinear;

console.log(`Total Time (all benchmarks):`);
console.log(`  Free List:      ${formatTime(totalTimeFreeList)}`);
console.log(`  Linear Search:  ${formatTime(totalTimeLinear)}`);
console.log(`  Overall Winner: ${totalTimeLinear < totalTimeFreeList ? 'Linear Search' : 'Free List'} ` +
            `(${((Math.max(totalTimeFreeList, totalTimeLinear) / Math.min(totalTimeFreeList, totalTimeLinear) - 1) * 100).toFixed(1)}% faster)\n`);

console.log(`Trade-offs:`);
console.log(`  Free List:`);
console.log(`    + O(1) allocation (array shift)`);
console.log(`    + O(1) deallocation (array push)`);
console.log(`    + Predictable performance`);
console.log(`    - Higher initialization cost (pre-allocates all slots)`);
console.log(`    - Higher memory overhead (stores all TypedArray views upfront)\n`);
console.log(`  Linear Search:`);
console.log(`    + Lower initialization cost (lazy allocation)`);
console.log(`    + Lower memory overhead (creates views on-demand)`);
console.log(`    - O(n) allocation (searches for free slot)`);
console.log(`    - O(1) deallocation but slower than array push`);
console.log(`    - Degrades with fragmentation (more used slots = slower alloc)\n`);
