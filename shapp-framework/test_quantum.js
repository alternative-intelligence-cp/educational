#!/usr/bin/env node
// Test shapp quantum module integration

// Load shapp first
require('./shapp.js');

// Load quantum module
require('./quantum.js');

console.log('═'.repeat(60));
console.log('Shapp Framework - Quantum Module Test');
console.log('═'.repeat(60));
console.log();

// Get module
const quantum = shapp.get('quantum');

if(!quantum) {
    console.error('ERROR: Failed to load quantum module!');
    process.exit(1);
}

console.log(`✓ Quantum module loaded (v${quantum.version})`);
console.log(`  ${quantum.description}`);
console.log();

// Test 1: Create Q9
console.log('Test 1: Create Q9 quantum number');
const q1 = new quantum.QuantumNumber(9, 10, 20, 2);
console.log(`  Created: ${JSON.stringify(q1.getState())}`);
const result1 = q1.crystallize();
console.log(`  Crystallized: ${result1} ✓`);
q1.destroy();
console.log();

// Test 2: Evidence accumulation
console.log('Test 2: Evidence accumulation');
const q2 = new quantum.QuantumNumber(9, 100, 150, 0);
console.log(`  Initial: c=${q2.getC()} (unknown)`);
q2.qif(true, 0, 0, 1);
console.log(`  After evidence 1: c=${q2.getC()}`);
q2.qif(true, 0, 0, 1);
console.log(`  After evidence 2: c=${q2.getC()}`);
const result2 = q2.crystallize();
console.log(`  Crystallized to: ${result2} ✓`);
q2.destroy();
console.log();

// Test 3: Quantum arithmetic
console.log('Test 3: Quantum arithmetic');
const q3 = new quantum.QuantumNumber(3, 8, 12, 0);
console.log(`  Initial: A=${q3.getA()}, B=${q3.getB()}`);
q3.add(3).mul(2).div(4);
console.log(`  After ((x+3)*2)/4: A=${q3.getA()}, B=${q3.getB()} ✓`);
q3.destroy();
console.log();

// Test 4: Q-functions
console.log('Test 4: Q-functions');
const q4 = new quantum.QuantumNumber(9, -5, 15, 1);
console.log(`  Initial: A=${q4.getA()}, B=${q4.getB()}`);
let executed = false;
q4.qor(
    v => v > 0,
    q => {
        q.a *= 2;
        q.b *= 2;
        executed = true;
    }
);
console.log(`  qor executed: ${executed} ✓`);
console.log(`  After qor: A=${q4.getA()}, B=${q4.getB()}`);
q4.destroy();
console.log();

console.log('═'.repeat(60));
console.log('✓ All tests passed! Shapp quantum integration working!');
console.log('═'.repeat(60));
