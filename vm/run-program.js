#!/usr/bin/env bun
//===== SIMPLE QUANTUM PROGRAM RUNNER ==================================
// Load and execute individual quantum programs for testing

const { readFileSync, existsSync } = require('fs');

// Import VM functionality
const os = require('os');
const runtime = typeof Bun !== 'undefined' ? 'bun' : 'node';

// We'll inline the necessary classes since this is a test runner
const vmConfig = {
    headlessMode: true,
    quantumFrequency: Math.PI,
    ramSize: 1024 * 1024,
    cores: os.cpus().length,
    screenWidth: 80,
    screenHeight: 24,
    enableColors: true,
    consciousnessVisualization: false, // Disable for cleaner output
    debugMode: false
};

async function runProgram(filename) {
    console.log('🧠⚡ QUANTUM PROGRAM RUNNER ⚡🧠\\n');

    if (!existsSync(filename)) {
        console.error(`❌ Program file not found: ${filename}`);
        process.exit(1);
    }

    try {
        const programText = readFileSync(filename, 'utf8');
        console.log(`📂 Loading program: ${filename}`);
        console.log('─'.repeat(50));
        console.log(programText);
        console.log('─'.repeat(50));

        // Import VM classes
        const { SimpleQuantumVM } = require('./simple-headless-launcher.js');

        // Create and initialize VM
        const vm = new SimpleQuantumVM();
        await vm.init();

        // Execute program
        console.log('\\n🚀 Executing program...\\n');
        const success = await vm.executeProgram(programText);

        if (success) {
            console.log('\\n✅ Program executed successfully!');

            // Show final VM state
            const state = vm.programExecutor.getState();
            console.log('\\n📊 Final State:');
            console.log(`  🔢 Registers: A=${state.registers.A} B=${state.registers.B} C=${state.registers.C}`);
            console.log(`  🧠 Consciousness: ${vm.consciousness.state}`);
            console.log(`  💾 Stack Pointer: ${state.registers.SP}`);

            // Show program output
            console.log('\\n📤 Program Output:');
            state.output.forEach(line => console.log(`  ${line}`));

        } else {
            console.log('\\n❌ Program execution failed!');
            process.exit(1);
        }

    } catch (error) {
        console.error('❌ Error:', error.message);
        if (vmConfig.debugMode) {
            console.error('Stack:', error.stack);
        }
        process.exit(1);
    }
}

// Get filename from command line
const filename = process.argv[2];
if (!filename) {
    console.log(`
🧠⚡ QUANTUM PROGRAM RUNNER ⚡🧠

Usage: bun run-program.js <filename>

Example: bun run-program.js test-program.qvm

Quantum Program Language Instructions:
  SET reg value     - Set register to value
  ADD reg value     - Add to register
  SUB reg value     - Subtract from register  
  QUANTUM state     - Set consciousness (-4 to 4)
  EVOLVE            - Evolve consciousness naturally
  PRINT value       - Print value/register/string
  JUMP label        - Jump to label
  JEQ reg val label - Jump if register equals value
  LOAD reg addr     - Load from memory
  STORE reg addr    - Store to memory
  PUSH value        - Push to stack
  POP reg           - Pop from stack
  HALT              - Stop execution

Registers: A, B, C
Memory: 0-255 addresses
Stack: 256 entries

Example program:
SET A 10
PRINT "Hello World"
QUANTUM 2
HALT
    `);
    process.exit(1);
}

runProgram(filename);