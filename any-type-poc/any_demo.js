#!/usr/bin/env node
/**
 * any_demo.js — Interactive demonstration of the Any type
 *
 * Run:   node any_demo.js
 *
 * Each section shows a different aspect of how ARIA's `any` type works under
 * the hood. Follow along with the output to understand type punning, memory
 * reinterpretation, and consuming semantics.
 */

'use strict';

const { Any } = require('./any.js');

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
function section(title) {
    console.log(`\n${'='.repeat(70)}`);
    console.log(`  ${title}`);
    console.log('='.repeat(70));
}

function show(label, value) {
    console.log(`  ${label.padEnd(30)} → ${value}`);
}

// ===========================================================================
// 1. Basic usage — store and retrieve
// ===========================================================================
section('1. Basic Usage');

const box = new Any('i32', 42);
show('Type', box.getType());
show('Value', box.get());
show('Raw bytes', box.rawHex());
show('toString()', box.toString());

// ===========================================================================
// 2. Type punning — same bytes, different interpretation
// ===========================================================================
section('2. Type Punning (the core concept)');

// Store 511 as an unsigned 32-bit integer
const pun = new Any('u32', 511);

console.log('\n  Store 511 as u32:');
show('u32 value', pun.get());
show('Raw bytes', pun.rawHex());

// Now look at those same bytes through different lenses
console.log('\n  Read the SAME bytes through different lenses:');

pun.setType('u8');
show('as u8 (low byte only)', pun.get());     // 255 (0xFF)

pun.setType('i8');
show('as i8 (signed byte)', pun.get());        // -1  (two's complement of 0xFF)

pun.setType('u16');
show('as u16 (low 2 bytes)', pun.get());       // 511

pun.setType('i32');
show('as i32 (signed 32)', pun.get());         // 511 (fits in positive range)

pun.setType('f32');
show('as f32 (IEEE 754)', pun.get());          // tiny float ≈ 7.16e-43

pun.setType('u32');
show('Back to u32', pun.get());                // 511 again — nothing changed!

console.log('\n  WHY? The bytes never changed. Only the INTERPRETATION changed.');
console.log('  This is exactly what ARIA\'s any:box does at the LLVM level.');

// ===========================================================================
// 3. Composite types — objects, strings, arrays
// ===========================================================================
section('3. Composite Types');

const multi = new Any('obj');
multi.set({ name: 'Alice', age: 30 });
show('obj (full)', JSON.stringify(multi.get()));
show('obj key "name"', multi.get('name'));

multi.set(25, 'age');
show('obj after set(25, "age")', JSON.stringify(multi.get()));

console.log();

multi.setType('str');
multi.set('hello world');
show('str (full)', multi.get());
show('str[4]', multi.get(4));

multi.set('O', 4);
show('str after set("O", 4)', multi.get());

console.log();

multi.setType('arr');
multi.set([10, 20, 30, 40]);
show('arr (full)', JSON.stringify(multi.get()));
show('arr[2]', multi.get(2));

multi.set(99, 2);
show('arr after set(99, 2)', JSON.stringify(multi.get()));

// ===========================================================================
// 4. Resolve — consuming semantics
// ===========================================================================
section('4. Resolve (Consuming Semantics)');

const temp = new Any('i32', 1234);
show('Before resolve', temp.get());
show('isResolved()', temp.isResolved());

const final = temp.resolve();
show('Resolved value', final);
show('isResolved()', temp.isResolved());
show('toString()', temp.toString());

console.log('\n  Attempting to use the box after resolve...');
try {
    temp.get();
} catch (e) {
    show('Error (expected!)', e.message);
}

console.log('\n  In ARIA, this is a COMPILE-TIME error. The type checker');
console.log('  tracks move/consume state so you can\'t use a resolved box.');

// ===========================================================================
// 5. BigInt for 64-bit types
// ===========================================================================
section('5. 64-bit Types (BigInt)');

const big = new Any('i64', -9007199254740993n);
show('i64 value', big.get().toString());
show('Raw bytes', big.rawHex());

big.setType('u64');
show('Same bytes as u64', big.get().toString());

console.log('\n  JavaScript Numbers lose precision above 2^53.');
console.log('  BigInt preserves exact 64-bit values — same as ARIA\'s int64/uint64.');

// ===========================================================================
// 6. Float ↔ Integer reinterpretation
// ===========================================================================
section('6. Float ↔ Integer Reinterpretation');

const fi = new Any('f64', 3.14);
show('f64 value', fi.get());
show('Raw bytes', fi.rawHex());

fi.setType('i64');
show('Same bytes as i64', fi.get().toString());

fi.setType('u64');
show('Same bytes as u64', fi.get().toString());

fi.setType('f64');
show('Back to f64', fi.get());

console.log('\n  This is the foundation of float-to-int bit tricks');
console.log('  (e.g. fast inverse square root). Same bytes, different meaning.');

// ===========================================================================
// 7. Error handling
// ===========================================================================
section('7. Error Handling');

console.log('\n  Invalid type:');
try {
    const bad = new Any('int128');
} catch (e) {
    show('Error', e.message);
}

console.log('\n  Wrong value type for i64 (needs BigInt):');
try {
    const bad2 = new Any('i64');
    bad2.set(42);  // should be 42n
} catch (e) {
    show('Error', e.message);
}

console.log('\n  Out of bounds:');
try {
    const bad3 = new Any('arr', [1, 2, 3]);
    bad3.get(10);
} catch (e) {
    show('Error', e.message);
}

// ===========================================================================
// 8. How this maps to ARIA's compiler
// ===========================================================================
section('8. How This Maps to ARIA');

console.log(`
  ARIA syntax                    JavaScript equivalent
  ─────────────────────────────  ────────────────────────────────
  any:box = 42i64                const box = new Any('i64', 42n)
  box.get::<int64>()             box.setType('i64'); box.get()
  box.set::<int32>(100i32)       box.setType('i32'); box.set(100)
  box.resolve::<int64>()         box.resolve()

  In ARIA, the turbofish ::<T> specifies the type inline:
    int64:val = box.get::<int64>();

  In this POC, we split that into setType() + get() since
  JavaScript doesn't have generics syntax.

  Under the hood in LLVM IR:
    any  →  { ptr, i64 }     (data pointer + size)
    get  →  load through ptr, bitcast to target type
    set  →  store through ptr
    resolve → return ptr (box becomes unusable)
`);

// ===========================================================================
section('Done!');
console.log('  All examples completed successfully.\n');
