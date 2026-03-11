#!/usr/bin/env node
/**
 * Binary Search - JavaScript Implementation
 *
 * Binary search works on a sorted list by repeatedly halving the search space:
 *   1. Check the middle element
 *   2. If it matches, done
 *   3. If the target is larger, discard the left half
 *   4. If the target is smaller, discard the right half
 *   5. Repeat until found or the search space is empty
 *
 * Time complexity:  O(log n)  — each step cuts the problem in half
 * Space complexity: O(1)      — iterative, no extra memory needed
 *
 * Usage:
 *   node binfind.js [target] [n1 n2 n3 ...]
 *
 * Examples:
 *   node binfind.js                      # uses built-in demo list, target 31
 *   node binfind.js 73                   # search for 73 in demo list
 *   node binfind.js 7 1 3 5 7 9 11 13   # search for 7 in a custom sorted list
 */

'use strict';

// ── Default demo data ─────────────────────────────────────────────────────────

const DEFAULT_LIST   = [1, 3, 6, 8, 9, 10, 11, 23, 25, 31, 65, 73, 105, 205, 569, 1024];
const DEFAULT_TARGET = 31;

// ── Argument parsing ──────────────────────────────────────────────────────────

const rawTarget = process.argv[2];
const rawList   = process.argv.slice(3);

// Parse target — must be a finite integer
let target;
if (rawTarget === undefined) {
    target = DEFAULT_TARGET;
} else {
    target = Number(rawTarget);
    if (!Number.isFinite(target)) {
        console.error(`Error: target "${rawTarget}" is not a valid number.`);
        process.exit(1);
    }
}

// Parse list — all items must be finite numbers
let list;
if (rawList.length === 0) {
    list = DEFAULT_LIST;
} else {
    list = rawList.map((s, i) => {
        const n = Number(s);
        if (!Number.isFinite(n)) {
            console.error(`Error: list item at position ${i + 1} ("${s}") is not a valid number.`);
            process.exit(1);
        }
        return n;
    });
}

// Verify the list is sorted (binary search requires a sorted input)
for (let i = 1; i < list.length; i++) {
    if (list[i] < list[i - 1]) {
        console.error(`Error: list is not sorted. Item at index ${i} (${list[i]}) is less than item at index ${i - 1} (${list[i - 1]}).`);
        process.exit(1);
    }
}

if (list.length === 0) {
    console.error('Error: list must have at least one element.');
    process.exit(1);
}

// ── Visualization helpers ─────────────────────────────────────────────────────

const COL_WIDTH = 6; // characters per number cell in the display

/**
 * Render the current search state as a one-line bar showing active range,
 * eliminated sections, and the probe point.
 *
 * Example:
 *   [ ░░░  1    3    6  |[  8  ]|  9   10   11   ░░░ ]
 *    eliminated ^        ^ mid  ^           ^ eliminated
 */
function renderStep(list, lo, hi, mid, stepNum, found) {
    const items = list.map((val, idx) => {
        const s = String(val).padStart(COL_WIDTH);
        if (found && idx === mid) return `\x1b[32m${s}\x1b[0m`; // green = found
        if (idx === mid)          return `\x1b[33m${s}\x1b[0m`; // yellow = probe
        if (idx < lo || idx > hi) return `\x1b[90m${s}\x1b[0m`; // grey = eliminated
        return s;
    });

    const lo_marker  = ' '.repeat((COL_WIDTH) * lo) + '[';
    const hi_marker  = ']';
    const bar        = items.join('');

    console.log(`  Step ${String(stepNum).padStart(2)}: [${bar} ]`);
    console.log(`           lo=${lo}  mid=${mid}  hi=${hi}  probe=${list[mid]}`);
}

// ── Binary search ─────────────────────────────────────────────────────────────

/**
 * Search for `target` in a sorted `list`.
 * Returns the index of the target, or -1 if not found.
 * Prints a visualization of each step.
 */
function binarySearch(list, target) {
    let lo   = 0;
    let hi   = list.length - 1;
    let step = 0;

    console.log('\n  Legend:  \x1b[33m[probe]\x1b[0m  \x1b[32m[found]\x1b[0m  \x1b[90m[eliminated]\x1b[0m\n');

    while (lo <= hi) {
        step++;
        const mid = Math.floor((lo + hi) / 2);
        const val = list[mid];

        if (val === target) {
            renderStep(list, lo, hi, mid, step, true);
            return { index: mid, steps: step };
        }

        renderStep(list, lo, hi, mid, step, false);

        if (val < target) {
            // Target is in the right half — eliminate everything ≤ mid
            lo = mid + 1;
        } else {
            // Target is in the left half — eliminate everything ≥ mid
            hi = mid - 1;
        }
    }

    return { index: -1, steps: step };
}

// ── Main ──────────────────────────────────────────────────────────────────────

console.log('─'.repeat(60));
console.log('  Binary Search');
console.log('─'.repeat(60));
console.log(`  List   : [${list.join(', ')}]`);
console.log(`  Length : ${list.length}`);
console.log(`  Target : ${target}`);
console.log('─'.repeat(60));

const result = binarySearch(list, target);

console.log('─'.repeat(60));
if (result.index !== -1) {
    console.log(`  Found!  value=${target}  index=${result.index}  position=${result.index + 1}`);
} else {
    console.log(`  Not found: ${target} is not in the list.`);
}
console.log(`  Steps taken: ${result.steps}  (list has ${list.length} items, max possible: ${Math.ceil(Math.log2(list.length + 1))})`);
console.log('─'.repeat(60));
console.log('');
