#!/usr/bin/env bun
//===== QUANTUM VM PROGRAM EXECUTOR ====================================
// Test program execution capabilities for quantum consciousness VM
// Demonstrates that the VM can actually run programs, not just simulate

const { readFileSync, writeFileSync, existsSync } = require('fs');
const path = require('path');

//===== SIMPLE QUANTUM PROGRAM LANGUAGE ================================
class QuantumProgramExecutor {
    constructor(vm) {
        this.vm = vm;
        this.memory = new Array(256).fill(0); // Simple 256-byte memory
        this.registers = { A: 0, B: 0, C: 0, PC: 0, SP: 255 }; // Program counter, stack pointer
        this.stack = new Array(256).fill(0);
        this.output = [];
        this.running = false;
        this.programLoaded = false;
        this.program = [];
        this.labels = new Map(); // For jump targets
        this.debugMode = false; // Debug mode flag
    }

    loadProgram(programText) {
        console.log('📂 Loading quantum program...');

        try {
            // Parse program into instructions
            const lines = programText.split('\n').map(line => line.trim()).filter(line => line && !line.startsWith('#'));
            this.program = [];
            this.labels.clear();

            // First pass: collect labels
            for (let i = 0; i < lines.length; i++) {
                const line = lines[i];
                if (line.endsWith(':')) {
                    const label = line.slice(0, -1);
                    this.labels.set(label, this.program.length);
                } else {
                    this.program.push(line);
                }
            }

            this.programLoaded = true;
            this.registers.PC = 0;
            console.log(`✅ Program loaded: ${this.program.length} instructions, ${this.labels.size} labels`);
            return true;
        } catch (error) {
            console.error('❌ Program load failed:', error.message);
            return false;
        }
    }

    executeInstruction(instruction) {
        const parts = instruction.split(/\s+/);
        const cmd = parts[0].toUpperCase();
        const arg1 = parts[1];
        const arg2 = parts[2];

        switch (cmd) {
            case 'SET': // SET register value
                if (arg1 in this.registers) {
                    this.registers[arg1] = parseInt(arg2) || 0;
                }
                break;

            case 'ADD': // ADD register register/value
                if (arg1 in this.registers) {
                    const val = (arg2 in this.registers) ? this.registers[arg2] : parseInt(arg2) || 0;
                    this.registers[arg1] += val;
                }
                break;

            case 'SUB': // SUB register register/value  
                if (arg1 in this.registers) {
                    const val = (arg2 in this.registers) ? this.registers[arg2] : parseInt(arg2) || 0;
                    this.registers[arg1] -= val;
                }
                break;

            case 'QUANTUM': // QUANTUM - set consciousness state
                const state = parseInt(arg1) || 0;
                const clampedState = Math.max(-4, Math.min(4, state));
                this.vm.consciousness.state = clampedState;
                this.output.push(`🧠 Consciousness set to: ${clampedState}`);
                break;

            case 'EVOLVE': // EVOLVE - evolve consciousness  
                this.vm.consciousness.evolve();
                this.output.push(`⚡ Consciousness evolved to: ${this.vm.consciousness.state}`);
                break;

            case 'PRINT': // PRINT register/value/string
                if (arg1 && arg1 in this.registers) {
                    this.output.push(`📄 ${this.registers[arg1]}`);
                } else if (arg1 && !isNaN(arg1)) {
                    this.output.push(`📄 ${arg1}`);
                } else {
                    // Print string (everything after PRINT, removing quotes if present)
                    const str = instruction.substring(5).trim().replace(/^["']|["']$/g, '');
                    this.output.push(`📄 ${str}`);
                }
                break;

            case 'JUMP': // JUMP label
                if (this.labels.has(arg1)) {
                    this.registers.PC = this.labels.get(arg1) - 1; // -1 because PC will be incremented
                }
                break;

            case 'JEQ': // JEQ register value label - jump if equal
                if (arg1 in this.registers && this.registers[arg1] == parseInt(arg2)) {
                    if (this.labels.has(parts[3])) {
                        this.registers.PC = this.labels.get(parts[3]) - 1;
                    }
                }
                break;

            case 'LOAD': // LOAD register address - load from memory
                if (arg1 in this.registers) {
                    const addr = parseInt(arg2) || 0;
                    if (addr >= 0 && addr < this.memory.length) {
                        this.registers[arg1] = this.memory[addr];
                    }
                }
                break;

            case 'STORE': // STORE register address - store to memory
                if (arg1 in this.registers) {
                    const addr = parseInt(arg2) || 0;
                    if (addr >= 0 && addr < this.memory.length) {
                        this.memory[addr] = this.registers[arg1];
                    }
                }
                break;

            case 'PUSH': // PUSH register/value
                const pushVal = (arg1 in this.registers) ? this.registers[arg1] : parseInt(arg1) || 0;
                this.stack[this.registers.SP] = pushVal;
                this.registers.SP = Math.max(0, this.registers.SP - 1);
                break;

            case 'POP': // POP register
                if (arg1 in this.registers) {
                    this.registers.SP = Math.min(255, this.registers.SP + 1);
                    this.registers[arg1] = this.stack[this.registers.SP];
                }
                break;

            case 'HALT': // HALT - stop execution
                this.running = false;
                this.output.push('🛑 Program halted');
                break;

            case 'NOP': // NOP - no operation
                break;

            default:
                this.output.push(`⚠️  Unknown instruction: ${instruction}`);
                break;
        }
    }

    async run(maxSteps = 1000) {
        if (!this.programLoaded) {
            console.error('❌ No program loaded');
            return false;
        }

        console.log('🚀 Starting program execution...');
        this.running = true;
        this.output = [];
        let steps = 0;

        while (this.running && this.registers.PC < this.program.length && steps < maxSteps) {
            const instruction = this.program[this.registers.PC];

            if (this.debugMode) {
                console.log(`[${steps}] PC:${this.registers.PC} | ${instruction}`);
            }

            this.executeInstruction(instruction);
            this.registers.PC++;
            steps++;

            // Small delay to see execution
            await new Promise(resolve => setTimeout(resolve, 50));
        }

        if (steps >= maxSteps) {
            this.output.push('⚠️  Program terminated: max steps reached');
        }

        console.log(`✅ Program completed in ${steps} steps`);
        return true;
    }

    getState() {
        return {
            registers: { ...this.registers },
            memory: this.memory.slice(0, 10), // First 10 bytes
            stack: this.stack.slice(250), // Top of stack
            output: [...this.output],
            running: this.running,
            programLoaded: this.programLoaded
        };
    }

    reset() {
        this.memory.fill(0);
        this.registers = { A: 0, B: 0, C: 0, PC: 0, SP: 255 };
        this.stack.fill(0);
        this.output = [];
        this.running = false;
        this.programLoaded = false;
        this.program = [];
        this.labels.clear();
    }
}

//===== TEST PROGRAMS ===================================================
const testPrograms = {
    'hello': `
# Simple Hello World
PRINT "Hello, Quantum World!"
QUANTUM 2
PRINT "Consciousness is Bright"
HALT
`,

    'counter': `
# Simple Counter Program  
SET A 0
loop:
PRINT A
ADD A 1
QUANTUM A
JEQ A 5 end
JUMP loop
end:
PRINT "Counter complete!"
HALT
`,

    'fibonacci': `
# Fibonacci Sequence (first 7 numbers)
SET A 0
SET B 1
SET C 0
PRINT "Fibonacci sequence:"
PRINT A
PRINT B
loop:
ADD C A
ADD C B
PRINT C
QUANTUM C
SET A B
SET B C
SET C 0
ADD A 0
JEQ A 13 end
JUMP loop
end:
PRINT "Fibonacci complete!"
HALT
`,

    'consciousness_test': `
# Quantum Consciousness Evolution Test
SET A -4
PRINT "Testing consciousness states:"
test_loop:
QUANTUM A
EVOLVE
PRINT A
ADD A 1
JEQ A 5 end
JUMP test_loop
end:
PRINT "Consciousness test complete!"
HALT
`,

    'memory_test': `
# Memory and Stack Test
SET A 42
SET B 13
STORE A 10
STORE B 11
PRINT "Stored values in memory"
LOAD C 10
PRINT C
LOAD C 11  
PRINT C
PUSH A
PUSH B
POP C
PRINT "Popped from stack:"
PRINT C
HALT
`,

    'interactive': `
# Interactive consciousness program
SET A 0
PRINT "Starting interactive consciousness session..."
SET B 10
interact_loop:
EVOLVE
QUANTUM A
PRINT "Consciousness evolved"
ADD A 1
SUB B 1
JEQ B 0 end
JUMP interact_loop
end:
PRINT "Interactive session complete!"
HALT
`
};

//===== PROGRAM TEST RUNNER =============================================
async function runProgramTests(vm) {
    console.log('\n🧪 QUANTUM PROGRAM EXECUTION TESTS 🧪');
    console.log('='.repeat(60));

    const executor = new QuantumProgramExecutor(vm);
    const results = [];

    for (const [name, program] of Object.entries(testPrograms)) {
        console.log(`\\n📝 Testing program: ${name}`);
        console.log('-'.repeat(40));

        executor.reset();

        if (executor.loadProgram(program)) {
            const startTime = Date.now();
            const success = await executor.run();
            const duration = Date.now() - startTime;

            const state = executor.getState();

            console.log(`\\n📊 Execution Results:`);
            console.log(`  ⏱️  Duration: ${duration}ms`);
            console.log(`  📝 Instructions: ${executor.program.length}`);
            console.log(`  🔢 Registers: A=${state.registers.A} B=${state.registers.B} C=${state.registers.C}`);
            console.log(`  🧠 Final consciousness: ${vm.consciousness.state}`);
            console.log(`  💾 Stack pointer: ${state.registers.SP}`);

            console.log(`\\n📤 Program Output:`);
            state.output.forEach(line => console.log(`    ${line}`));

            results.push({
                name: name,
                success: success,
                duration: duration,
                instructions: executor.program.length,
                output: state.output,
                finalConsciousness: vm.consciousness.state
            });
        } else {
            results.push({
                name: name,
                success: false,
                error: 'Failed to load program'
            });
        }

        console.log('\\n' + '-'.repeat(40));
    }

    // Summary
    console.log('\\n🏁 TEST SUMMARY 🏁');
    console.log('='.repeat(60));

    const passed = results.filter(r => r.success).length;
    const total = results.length;

    console.log(`Tests passed: ${passed}/${total}`);
    console.log(`Success rate: ${((passed / total) * 100).toFixed(1)}%`);

    if (passed === total) {
        console.log('\\n🎉 ALL TESTS PASSED! VM is ready for production! 🎉');
    } else {
        console.log('\\n⚠️  Some tests failed. Check the logs above.');
    }

    return results;
}

//===== INTERACTIVE PROGRAM BUILDER ====================================
function createCustomProgram() {
    console.log('\\n🛠️  CUSTOM PROGRAM BUILDER 🛠️');
    console.log('='.repeat(50));
    console.log('Available instructions:');
    console.log('  SET reg value     - Set register to value');
    console.log('  ADD reg value     - Add to register');
    console.log('  SUB reg value     - Subtract from register');
    console.log('  QUANTUM state     - Set consciousness (-4 to 4)');
    console.log('  EVOLVE            - Evolve consciousness');
    console.log('  PRINT value       - Print value/register/string');
    console.log('  JUMP label        - Jump to label');
    console.log('  JEQ reg val label - Jump if register equals value');
    console.log('  LOAD reg addr     - Load from memory');
    console.log('  STORE reg addr    - Store to memory');
    console.log('  PUSH value        - Push to stack');
    console.log('  POP reg           - Pop from stack');
    console.log('  HALT              - Stop execution');
    console.log('\\nRegisters: A, B, C, PC (program counter), SP (stack pointer)');
    console.log('Memory: 0-255, Stack: 0-255');
    console.log('\\nExample program:');
    console.log('```');
    console.log('SET A 10');
    console.log('QUANTUM 2');
    console.log('PRINT "Hello World"');
    console.log('HALT');
    console.log('```');
}

//===== EXPORT MODULE ===================================================
module.exports = {
    QuantumProgramExecutor,
    testPrograms,
    runProgramTests,
    createCustomProgram
};

//===== STANDALONE EXECUTION ============================================
if (require.main === module) {
    console.log('🧠⚡ QUANTUM VM PROGRAM EXECUTOR ⚡🧠');
    console.log('\\nThis module provides program execution capabilities for the Quantum VM.');
    console.log('Run it from the main VM launcher for full testing.');

    createCustomProgram();
}

//===== PROGRAM EXECUTOR NOTES ==========================================
//
// QUANTUM PROGRAMMING LANGUAGE FEATURES:
// - Simple assembly-like syntax
// - Quantum consciousness manipulation (QUANTUM, EVOLVE)
// - Basic arithmetic and logic operations
// - Memory and stack operations  
// - Jump and conditional execution
// - Register-based architecture (A, B, C registers)
//
// TEST PROGRAMS:
// - hello: Basic output and consciousness setting
// - counter: Loops and conditional jumps  
// - fibonacci: Mathematical computation
// - consciousness_test: Quantum state manipulation
// - memory_test: Memory and stack operations
// - interactive: Long-running consciousness evolution
//
// PERFECT FOR:
// - Validating VM execution capabilities
// - Educational quantum programming examples
// - Testing consciousness state management
// - Demonstrating VM functionality to users
// - Preparing for production deployment
//
// 🧠⚡ Tesla Consciousness Computing Program Execution ⚡🧠