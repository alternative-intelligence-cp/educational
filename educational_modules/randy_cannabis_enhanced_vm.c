/*
 * Randy's Cannabis-Enhanced Virtual Machine (CEVM)
 * Plant Spirit CPU Architecture for Educational Enlightenment
 * 
 * Randy's VM Resurrection Story:
 * "I had a damn good start on this project... that is unfortunately lost now.
 * I was gonna build a VM. I went a bit overboard considering the time constraints
 * I had though. I know I finished a register implementation that pretty much
 * matched x86_64 but with better naming. It would have been 64 bit but as I say
 * I got a bit deep. Didn't stop with just the regular registers. First SIMD,
 * then AVX, and so forth... When I hit AVX 512 land I was like you know, I am
 * never gonna live long enough to finish this."
 * 
 * THE SYNCHRONICITY SALVATION:
 * The lost VM project wasn't lost - it was waiting for the perfect moment
 * when all educational pieces would align! Now we have assembly macros,
 * schedulers, filesystems, and network protocols as foundation components.
 * 
 * Randy's New Philosophy: 8-bit simplicity with educational clarity!
 * No more AVX-512 madness - just pure, cannabis-enhanced learning bliss.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

// Randy's Cannabis-Enhanced VM Constants
#define CEVM_MEMORY_SIZE    65536      // 64KB memory space
#define CEVM_REGISTER_COUNT 8          // Simple register set
#define CEVM_STACK_SIZE     1024       // Stack size
#define CEVM_MAX_PROGRAM    8192       // Maximum program size
#define CEVM_MAGIC         0xC3VM      // Cannabis VM magic number

/*
 * Randy's 8-bit Register Architecture
 * "Simple, educational, and free of AVX-512 nightmares"
 */
typedef enum {
    REG_A = 0,      // Accumulator - primary arithmetic register
    REG_B = 1,      // Base - secondary arithmetic register  
    REG_C = 2,      // Counter - loop counter and general purpose
    REG_D = 3,      // Data - general purpose data register
    REG_E = 4,      // Extended - additional general purpose
    REG_F = 5,      // Flags - status and condition flags
    REG_SP = 6,     // Stack Pointer - points to stack top
    REG_PC = 7      // Program Counter - instruction pointer
} cevm_register_t;

/*
 * Randy's Cannabis-Enhanced Instruction Set
 * "Educational clarity over x86 complexity"
 */
typedef enum {
    // Data Movement Instructions
    OP_LOAD_IMM = 0x01,    // LOAD reg, immediate
    OP_LOAD_MEM = 0x02,    // LOAD reg, [address]
    OP_STORE = 0x03,       // STORE [address], reg
    OP_MOVE = 0x04,        // MOVE reg1, reg2
    
    // Arithmetic Instructions  
    OP_ADD = 0x10,         // ADD reg1, reg2
    OP_SUB = 0x11,         // SUB reg1, reg2
    OP_MUL = 0x12,         // MUL reg1, reg2
    OP_DIV = 0x13,         // DIV reg1, reg2
    OP_INC = 0x14,         // INC reg
    OP_DEC = 0x15,         // DEC reg
    
    // Logic Instructions
    OP_AND = 0x20,         // AND reg1, reg2
    OP_OR = 0x21,          // OR reg1, reg2
    OP_XOR = 0x22,         // XOR reg1, reg2
    OP_NOT = 0x23,         // NOT reg
    OP_SHL = 0x24,         // SHL reg, count
    OP_SHR = 0x25,         // SHR reg, count
    
    // Comparison Instructions
    OP_CMP = 0x30,         // CMP reg1, reg2 (sets flags)
    OP_TEST = 0x31,        // TEST reg1, reg2 (AND without storing)
    
    // Control Flow Instructions
    OP_JMP = 0x40,         // JMP address
    OP_JEQ = 0x41,         // JEQ address (jump if equal)
    OP_JNE = 0x42,         // JNE address (jump if not equal)
    OP_JLT = 0x43,         // JLT address (jump if less than)
    OP_JGT = 0x44,         // JGT address (jump if greater than)
    OP_CALL = 0x45,        // CALL address
    OP_RET = 0x46,         // RET
    
    // Stack Instructions
    OP_PUSH = 0x50,        // PUSH reg
    OP_POP = 0x51,         // POP reg
    
    // I/O Instructions
    OP_IN = 0x60,          // IN reg, port
    OP_OUT = 0x61,         // OUT port, reg
    OP_PRINT = 0x62,       // PRINT reg (educational output)
    
    // Special Instructions
    OP_NOP = 0x90,         // NOP (no operation)
    OP_HALT = 0xFF,        // HALT (stop execution)
    
    // Cannabis-Enhanced Instructions (educational fun)
    OP_TOKE = 0xC0,        // TOKE reg (enhance creativity flag)
    OP_PUFF = 0xC1,        // PUFF reg (pass to next register)
    OP_WISDOM = 0xC2       // WISDOM (display plant spirit insight)
} cevm_opcode_t;

/*
 * Randy's VM Status Flags
 * "Simple flag system for educational clarity"
 */
typedef struct {
    bool zero;             // Zero flag (result was zero)
    bool carry;            // Carry flag (arithmetic overflow)
    bool negative;         // Negative flag (result was negative)
    bool creative;         // Creative flag (cannabis enhancement active)
} cevm_flags_t;

/*
 * Randy's Cannabis-Enhanced CPU State
 * "The heart of the educational virtual machine"
 */
typedef struct {
    uint8_t registers[CEVM_REGISTER_COUNT];    // CPU registers
    uint8_t memory[CEVM_MEMORY_SIZE];          // System memory
    uint8_t stack[CEVM_STACK_SIZE];            // Call stack
    cevm_flags_t flags;                        // Status flags
    bool running;                              // CPU running state
    uint64_t cycles;                           // Instruction cycles
    uint64_t instructions_executed;            // Instruction counter
    char plant_spirit_mode[64];                // Current cannabis strain influence
} cevm_cpu_t;

/*
 * Randy's Instruction Format
 * "Simple 3-byte instruction encoding"
 */
typedef struct __attribute__((packed)) {
    uint8_t opcode;        // Operation code
    uint8_t operand1;      // First operand (register or immediate low)
    uint8_t operand2;      // Second operand (register or immediate high)
} cevm_instruction_t;

// Global CPU state
static cevm_cpu_t g_cpu;

/*
 * Randy's Cannabis Strain Database
 * "Different strains provide different cognitive enhancements"
 */
typedef struct {
    const char *name;
    const char *effect;
    int creativity_boost;
    int focus_boost;
} cannabis_strain_t;

static const cannabis_strain_t plant_spirits[] = {
    {"Strawberry Pie", "Enhanced pattern recognition for debugging", 8, 6},
    {"Green Crack", "Laser focus for complex algorithms", 5, 10},
    {"Blue Dream", "Balanced creativity and analytical thinking", 7, 7},
    {"Jack Herer", "Cerebral clarity for architecture design", 9, 8},
    {"Sour Diesel", "Energetic problem-solving enhancement", 6, 9},
    {"OG Kush", "Deep system understanding and wisdom", 8, 7},
    {"Wedding Cake", "Euphoric breakthrough moments", 10, 5},
    {"Northern Lights", "Calm, methodical debugging sessions", 4, 9}
};

/*
 * Randy's VM Utility Functions
 * "Helper functions for the educational experience"
 */
void cevm_reset_cpu(void) {
    memset(&g_cpu, 0, sizeof(g_cpu));
    g_cpu.registers[REG_SP] = CEVM_STACK_SIZE - 1;
    g_cpu.running = true;
    strcpy(g_cpu.plant_spirit_mode, "Sober Mode");
    
    printf("🧠 CEVM CPU Reset Complete\n");
    printf("   Registers cleared, stack initialized, plant spirits ready\n\n");
}

void cevm_set_flag(bool *flag, bool value) {
    *flag = value;
}

void cevm_update_flags(uint8_t result) {
    cevm_set_flag(&g_cpu.flags.zero, result == 0);
    cevm_set_flag(&g_cpu.flags.negative, result & 0x80);
}

uint8_t cevm_get_register(cevm_register_t reg) {
    if (reg >= CEVM_REGISTER_COUNT) {
        printf("❌ Invalid register access: %d\n", reg);
        return 0;
    }
    return g_cpu.registers[reg];
}

void cevm_set_register(cevm_register_t reg, uint8_t value) {
    if (reg >= CEVM_REGISTER_COUNT) {
        printf("❌ Invalid register write: %d\n", reg);
        return;
    }
    g_cpu.registers[reg] = value;
    
    if (reg != REG_PC) {  // Don't update flags for PC changes
        cevm_update_flags(value);
    }
}

uint8_t cevm_read_memory(uint16_t address) {
    if (address >= CEVM_MEMORY_SIZE) {
        printf("❌ Memory read out of bounds: 0x%04X\n", address);
        return 0;
    }
    return g_cpu.memory[address];
}

void cevm_write_memory(uint16_t address, uint8_t value) {
    if (address >= CEVM_MEMORY_SIZE) {
        printf("❌ Memory write out of bounds: 0x%04X\n", address);
        return;
    }
    g_cpu.memory[address] = value;
}

void cevm_push_stack(uint8_t value) {
    if (g_cpu.registers[REG_SP] == 0) {
        printf("❌ Stack overflow!\n");
        g_cpu.running = false;
        return;
    }
    g_cpu.stack[g_cpu.registers[REG_SP]] = value;
    g_cpu.registers[REG_SP]--;
}

uint8_t cevm_pop_stack(void) {
    if (g_cpu.registers[REG_SP] >= CEVM_STACK_SIZE - 1) {
        printf("❌ Stack underflow!\n");
        g_cpu.running = false;
        return 0;
    }
    g_cpu.registers[REG_SP]++;
    return g_cpu.stack[g_cpu.registers[REG_SP]];
}

/*
 * Randy's Cannabis Enhancement System
 * "Plant spirit integration for enhanced learning"
 */
void cevm_activate_plant_spirit(const char *strain_name) {
    for (int i = 0; i < sizeof(plant_spirits) / sizeof(plant_spirits[0]); i++) {
        if (strcmp(plant_spirits[i].name, strain_name) == 0) {
            strcpy(g_cpu.plant_spirit_mode, strain_name);
            g_cpu.flags.creative = true;
            
            printf("🌿 Plant Spirit Activated: %s\n", strain_name);
            printf("   Effect: %s\n", plant_spirits[i].effect);
            printf("   Creativity: +%d, Focus: +%d\n", 
                   plant_spirits[i].creativity_boost, plant_spirits[i].focus_boost);
            return;
        }
    }
    printf("🚫 Unknown strain: %s\n", strain_name);
}

void cevm_display_plant_wisdom(void) {
    const char *wisdom[] = {
        "Complex problems often have simple solutions - look deeper",
        "The bug is usually where you're most certain it isn't",
        "Cannabis enhances pattern recognition - trust the insights",
        "Manual understanding beats automated ignorance",
        "Every error is a teacher wearing a disguise",
        "The mycelial network connects all knowledge",
        "Plant spirits guide us to elegant solutions",
        "Debugging is a conversation with your past self"
    };
    
    int idx = rand() % (sizeof(wisdom) / sizeof(wisdom[0]));
    printf("🧠 Plant Spirit Wisdom: %s\n", wisdom[idx]);
}

/*
 * Randy's Instruction Implementation
 * "Each opcode gets royal treatment and educational commentary"
 */
void cevm_execute_instruction(cevm_instruction_t *instr) {
    uint8_t op1 = instr->operand1;
    uint8_t op2 = instr->operand2;
    uint16_t address = (op2 << 8) | op1;  // 16-bit address from operands
    uint8_t result;
    
    g_cpu.cycles++;
    g_cpu.instructions_executed++;
    
    switch (instr->opcode) {
        case OP_LOAD_IMM:
            cevm_set_register(op1, op2);
            printf("📥 LOAD R%d, #%d\n", op1, op2);
            break;
            
        case OP_LOAD_MEM:
            result = cevm_read_memory(address);
            cevm_set_register(op1, result);
            printf("📥 LOAD R%d, [0x%04X] = %d\n", op1, address, result);
            break;
            
        case OP_STORE:
            cevm_write_memory(address, cevm_get_register(op1));
            printf("📤 STORE [0x%04X], R%d = %d\n", address, op1, cevm_get_register(op1));
            break;
            
        case OP_MOVE:
            cevm_set_register(op1, cevm_get_register(op2));
            printf("➡️  MOVE R%d, R%d = %d\n", op1, op2, cevm_get_register(op2));
            break;
            
        case OP_ADD:
            result = cevm_get_register(op1) + cevm_get_register(op2);
            cevm_set_register(op1, result);
            printf("➕ ADD R%d, R%d = %d\n", op1, op2, result);
            break;
            
        case OP_SUB:
            result = cevm_get_register(op1) - cevm_get_register(op2);
            cevm_set_register(op1, result);
            printf("➖ SUB R%d, R%d = %d\n", op1, op2, result);
            break;
            
        case OP_MUL:
            result = cevm_get_register(op1) * cevm_get_register(op2);
            cevm_set_register(op1, result);
            printf("✖️  MUL R%d, R%d = %d\n", op1, op2, result);
            break;
            
        case OP_INC:
            result = cevm_get_register(op1) + 1;
            cevm_set_register(op1, result);
            printf("⬆️  INC R%d = %d\n", op1, result);
            break;
            
        case OP_DEC:
            result = cevm_get_register(op1) - 1;
            cevm_set_register(op1, result);
            printf("⬇️  DEC R%d = %d\n", op1, result);
            break;
            
        case OP_CMP:
            result = cevm_get_register(op1) - cevm_get_register(op2);
            cevm_update_flags(result);
            printf("🔍 CMP R%d, R%d (flags updated)\n", op1, op2);
            break;
            
        case OP_JMP:
            g_cpu.registers[REG_PC] = address;
            printf("🦘 JMP 0x%04X\n", address);
            return;  // Don't increment PC
            
        case OP_JEQ:
            if (g_cpu.flags.zero) {
                g_cpu.registers[REG_PC] = address;
                printf("🦘 JEQ 0x%04X (taken)\n", address);
                return;
            }
            printf("🦘 JEQ 0x%04X (not taken)\n", address);
            break;
            
        case OP_JNE:
            if (!g_cpu.flags.zero) {
                g_cpu.registers[REG_PC] = address;
                printf("🦘 JNE 0x%04X (taken)\n", address);
                return;
            }
            printf("🦘 JNE 0x%04X (not taken)\n", address);
            break;
            
        case OP_CALL:
            cevm_push_stack(g_cpu.registers[REG_PC] + 3);  // Save return address
            g_cpu.registers[REG_PC] = address;
            printf("📞 CALL 0x%04X\n", address);
            return;
            
        case OP_RET:
            g_cpu.registers[REG_PC] = cevm_pop_stack();
            printf("🔙 RET to 0x%04X\n", g_cpu.registers[REG_PC]);
            return;
            
        case OP_PUSH:
            cevm_push_stack(cevm_get_register(op1));
            printf("⬇️  PUSH R%d = %d\n", op1, cevm_get_register(op1));
            break;
            
        case OP_POP:
            result = cevm_pop_stack();
            cevm_set_register(op1, result);
            printf("⬆️  POP R%d = %d\n", op1, result);
            break;
            
        case OP_PRINT:
            printf("🖨️  PRINT: %d (0x%02X)\n", cevm_get_register(op1), cevm_get_register(op1));
            break;
            
        case OP_TOKE:
            cevm_activate_plant_spirit("Blue Dream");
            printf("🌿 TOKE executed - plant spirit enhancement activated!\n");
            break;
            
        case OP_WISDOM:
            cevm_display_plant_wisdom();
            break;
            
        case OP_NOP:
            printf("😴 NOP (no operation)\n");
            break;
            
        case OP_HALT:
            g_cpu.running = false;
            printf("🛑 HALT - CPU stopped\n");
            break;
            
        default:
            printf("❌ Unknown opcode: 0x%02X\n", instr->opcode);
            g_cpu.running = false;
            break;
    }
    
    // Increment program counter for most instructions
    g_cpu.registers[REG_PC] += 3;
}

/*
 * Randy's VM Execution Engine
 * "The beating heart of cannabis-enhanced computation"
 */
void cevm_run_program(void) {
    cevm_instruction_t instr;
    
    printf("🚀 Starting CEVM execution...\n");
    printf("   Plant spirit mode: %s\n\n", g_cpu.plant_spirit_mode);
    
    while (g_cpu.running) {
        uint16_t pc = cevm_get_register(REG_PC);
        
        if (pc + 2 >= CEVM_MEMORY_SIZE) {
            printf("❌ Program counter out of bounds: 0x%04X\n", pc);
            break;
        }
        
        // Fetch instruction
        instr.opcode = cevm_read_memory(pc);
        instr.operand1 = cevm_read_memory(pc + 1);
        instr.operand2 = cevm_read_memory(pc + 2);
        
        printf("[Cycle %lu] PC:0x%04X | ", g_cpu.cycles, pc);
        
        // Execute instruction
        cevm_execute_instruction(&instr);
        
        // Small delay for educational visibility
        usleep(100000);  // 100ms delay
    }
    
    printf("\n🏁 CEVM execution complete!\n");
    printf("   Total cycles: %lu\n", g_cpu.cycles);
    printf("   Instructions executed: %lu\n", g_cpu.instructions_executed);
    printf("   Plant spirit guidance: %s\n", g_cpu.plant_spirit_mode);
}

/*
 * Randy's Assembly Compiler
 * "Simple assembler for our cannabis-enhanced instruction set"
 */
typedef struct {
    const char *mnemonic;
    cevm_opcode_t opcode;
    const char *description;
} cevm_instruction_info_t;

static const cevm_instruction_info_t instruction_table[] = {
    {"LOAD", OP_LOAD_IMM, "Load immediate value into register"},
    {"LOADM", OP_LOAD_MEM, "Load value from memory into register"},
    {"STORE", OP_STORE, "Store register value to memory"},
    {"MOVE", OP_MOVE, "Move value between registers"},
    {"ADD", OP_ADD, "Add two registers"},
    {"SUB", OP_SUB, "Subtract two registers"},
    {"MUL", OP_MUL, "Multiply two registers"},
    {"INC", OP_INC, "Increment register"},
    {"DEC", OP_DEC, "Decrement register"},
    {"CMP", OP_CMP, "Compare two registers"},
    {"JMP", OP_JMP, "Unconditional jump"},
    {"JEQ", OP_JEQ, "Jump if equal"},
    {"JNE", OP_JNE, "Jump if not equal"},
    {"CALL", OP_CALL, "Call subroutine"},
    {"RET", OP_RET, "Return from subroutine"},
    {"PUSH", OP_PUSH, "Push register to stack"},
    {"POP", OP_POP, "Pop stack to register"},
    {"PRINT", OP_PRINT, "Print register value"},
    {"TOKE", OP_TOKE, "Activate plant spirit enhancement"},
    {"WISDOM", OP_WISDOM, "Display plant spirit wisdom"},
    {"NOP", OP_NOP, "No operation"},
    {"HALT", OP_HALT, "Halt execution"}
};

void cevm_show_instruction_set(void) {
    printf("🧠 Randy's Cannabis-Enhanced VM Instruction Set 🧠\n");
    printf("════════════════════════════════════════════════\n");
    
    for (int i = 0; i < sizeof(instruction_table) / sizeof(instruction_table[0]); i++) {
        printf("  %-8s (0x%02X) - %s\n", 
               instruction_table[i].mnemonic,
               instruction_table[i].opcode,
               instruction_table[i].description);
    }
    
    printf("\n🌿 Cannabis-Enhanced Instructions:\n");
    printf("  TOKE     - Activate plant spirit for enhanced creativity\n");
    printf("  WISDOM   - Receive guidance from the mycelial network\n");
    printf("\n📚 Educational Registers:\n");
    printf("  R0 (A)   - Accumulator\n");
    printf("  R1 (B)   - Base\n");
    printf("  R2 (C)   - Counter\n");
    printf("  R3 (D)   - Data\n");
    printf("  R4 (E)   - Extended\n");
    printf("  R5 (F)   - Flags\n");
    printf("  R6 (SP)  - Stack Pointer\n");
    printf("  R7 (PC)  - Program Counter\n");
    printf("════════════════════════════════════════════════\n\n");
}

/*
 * Randy's Example Programs
 * "Cannabis-enhanced sample programs for educational bliss"
 */
void cevm_load_fibonacci_program(void) {
    printf("🌿 Loading Fibonacci Program (Cannabis-Enhanced Algorithm)\n\n");
    
    // Fibonacci program in CEVM assembly
    uint8_t program[] = {
        // Initialize first two Fibonacci numbers
        OP_LOAD_IMM, REG_A, 0,    // A = 0 (fib 0)
        OP_LOAD_IMM, REG_B, 1,    // B = 1 (fib 1) 
        OP_LOAD_IMM, REG_C, 8,    // C = 8 (counter)
        
        // Print first number
        OP_PRINT, REG_A, 0,       // Print A
        
        // Main loop
        OP_ADD, REG_A, REG_B,     // A = A + B (next fib)
        OP_PRINT, REG_A, 0,       // Print result
        OP_MOVE, REG_D, REG_A,    // D = A (temp)
        OP_MOVE, REG_A, REG_B,    // A = B (shift)
        OP_MOVE, REG_B, REG_D,    // B = D (complete swap)
        OP_DEC, REG_C, 0,         // Decrement counter
        OP_CMP, REG_C, REG_A,     // Compare counter with 0 (using A as temp 0)
        OP_JNE, 15, 0,           // Jump back if not zero
        
        // Cannabis enhancement
        OP_TOKE, 0, 0,           // Activate plant spirits
        OP_WISDOM, 0, 0,         // Share wisdom
        OP_HALT, 0, 0            // Stop execution
    };
    
    // Load program into memory
    for (int i = 0; i < sizeof(program); i++) {
        cevm_write_memory(i, program[i]);
    }
    
    printf("✅ Fibonacci program loaded into memory\n");
    printf("   Plant spirit mathematics ready for execution!\n\n");
}

void cevm_load_hello_world_program(void) {
    printf("🌿 Loading Hello World Program (Educational Demonstration)\n\n");
    
    uint8_t program[] = {
        // Print "Hello" using ASCII values
        OP_LOAD_IMM, REG_A, 72,   // 'H'
        OP_PRINT, REG_A, 0,
        OP_LOAD_IMM, REG_A, 101,  // 'e' 
        OP_PRINT, REG_A, 0,
        OP_LOAD_IMM, REG_A, 108,  // 'l'
        OP_PRINT, REG_A, 0,
        OP_LOAD_IMM, REG_A, 108,  // 'l'
        OP_PRINT, REG_A, 0,
        OP_LOAD_IMM, REG_A, 111,  // 'o'
        OP_PRINT, REG_A, 0,
        
        // Cannabis enhancement
        OP_TOKE, 0, 0,           // Enhance with plant spirits
        OP_WISDOM, 0, 0,         // Share wisdom
        OP_HALT, 0, 0            // Complete
    };
    
    for (int i = 0; i < sizeof(program); i++) {
        cevm_write_memory(i, program[i]);
    }
    
    printf("✅ Hello World program loaded\n");
    printf("   Educational character output with cannabis enhancement!\n\n");
}

/*
 * Randy's Interactive VM Shell
 * "Command interface for cannabis-enhanced virtual machine exploration"
 */
void cevm_interactive_shell(void) {
    char input[256];
    char command[64], arg1[64], arg2[64];
    
    printf("🧠⚡ Randy's Cannabis-Enhanced Virtual Machine Shell ⚡🧠\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("Commands:\n");
    printf("  reset           - Reset CPU state\n");
    printf("  load <program>  - Load sample program (fib, hello)\n");
    printf("  run             - Execute loaded program\n");
    printf("  step            - Single-step execution\n");
    printf("  regs            - Show register values\n");
    printf("  mem <addr>      - Show memory contents\n");
    printf("  toke <strain>   - Activate plant spirit\n");
    printf("  wisdom          - Get plant spirit guidance\n");
    printf("  instructions    - Show instruction set\n");
    printf("  quit            - Exit VM shell\n");
    printf("═══════════════════════════════════════════════════════\n\n");
    
    while (true) {
        printf("CEVM> ");
        fflush(stdout);
        
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }
        
        input[strcspn(input, "\n")] = 0;  // Remove newline
        
        int args = sscanf(input, "%63s %63s %63s", command, arg1, arg2);
        if (args < 1) continue;
        
        if (strcmp(command, "reset") == 0) {
            cevm_reset_cpu();
            
        } else if (strcmp(command, "load") == 0) {
            if (args < 2) {
                printf("Usage: load <program> (fib, hello)\n");
                continue;
            }
            
            if (strcmp(arg1, "fib") == 0) {
                cevm_load_fibonacci_program();
            } else if (strcmp(arg1, "hello") == 0) {
                cevm_load_hello_world_program();
            } else {
                printf("Unknown program: %s\n", arg1);
            }
            
        } else if (strcmp(command, "run") == 0) {
            cevm_run_program();
            
        } else if (strcmp(command, "regs") == 0) {
            printf("📊 Register State:\n");
            for (int i = 0; i < CEVM_REGISTER_COUNT; i++) {
                printf("  R%d = %3d (0x%02X)\n", i, g_cpu.registers[i], g_cpu.registers[i]);
            }
            printf("  Flags: Z=%d C=%d N=%d Creative=%d\n",
                   g_cpu.flags.zero, g_cpu.flags.carry, 
                   g_cpu.flags.negative, g_cpu.flags.creative);
            
        } else if (strcmp(command, "mem") == 0) {
            if (args < 2) {
                printf("Usage: mem <address>\n");
                continue;
            }
            
            int addr = strtol(arg1, NULL, 0);
            if (addr >= 0 && addr < CEVM_MEMORY_SIZE) {
                printf("Memory[0x%04X] = %d (0x%02X)\n", 
                       addr, g_cpu.memory[addr], g_cpu.memory[addr]);
            } else {
                printf("Address out of range: %d\n", addr);
            }
            
        } else if (strcmp(command, "toke") == 0) {
            if (args < 2) {
                // Random strain activation
                int idx = rand() % (sizeof(plant_spirits) / sizeof(plant_spirits[0]));
                cevm_activate_plant_spirit(plant_spirits[idx].name);
            } else {
                cevm_activate_plant_spirit(arg1);
            }
            
        } else if (strcmp(command, "wisdom") == 0) {
            cevm_display_plant_wisdom();
            
        } else if (strcmp(command, "instructions") == 0) {
            cevm_show_instruction_set();
            
        } else if (strcmp(command, "quit") == 0) {
            printf("🌿 Disconnecting from cannabis-enhanced consciousness...\n");
            break;
            
        } else {
            printf("Unknown command: %s\n", command);
        }
    }
}

/*
 * Randy's Main VM Entry Point
 * "Where educational virtual machines meet plant spirit consciousness"
 */
int main(int argc, char *argv[]) {
    printf("🧠⚡ Randy's Cannabis-Enhanced Virtual Machine (CEVM) ⚡🧠\n");
    printf("    The Lost VM Project Reborn: 8-bit Educational Bliss\n");
    printf("    From AVX-512 Nightmares to Simple Educational Dreams\n\n");
    
    printf("🌿 The Synchronicity Story:\n");
    printf("   Randy's original VM project was lost to AVX-512 complexity.\n");
    printf("   But the plant spirits guided us back to educational simplicity.\n");
    printf("   Now we have a complete educational stack from firmware to VM!\n\n");
    
    printf("🎯 Complete Educational Journey:\n");
    printf("   ✅ Assembly Macros (Meta-programming)\n");
    printf("   ✅ Process Scheduler (OS internals)\n");
    printf("   ✅ EFI Loader (Firmware level)\n");
    printf("   ✅ Toy Filesystem (Storage layer)\n");
    printf("   ✅ Network Protocol (Communication layer)\n");
    printf("   🔥 Virtual Machine (Hardware simulation)\n\n");
    
    // Initialize random seed
    srand(time(NULL));
    
    // Initialize CPU
    cevm_reset_cpu();
    
    printf("💡 Educational Value:\n");
    printf("   🔧 CPU architecture and instruction set design\n");
    printf("   🔧 Virtual machine implementation concepts\n");
    printf("   🔧 Assembly language and bytecode execution\n");
    printf("   🔧 Register management and memory addressing\n");
    printf("   🔧 Stack operations and subroutine calls\n");
    printf("   🔧 Flag systems and conditional execution\n\n");
    
    printf("🧠 Plant Spirit Enhancement:\n");
    printf("   Cannabis-inspired instruction set with TOKE and WISDOM opcodes\n");
    printf("   Different strains provide different cognitive enhancements\n");
    printf("   Creative flag system for enhanced problem-solving\n");
    printf("   Mycelial network wisdom integration\n\n");
    
    // Run interactive shell
    cevm_interactive_shell();
    
    printf("🌿 Thank you for exploring Randy's Cannabis-Enhanced VM!\n");
    printf("   The mycelial network of knowledge grows through sharing.\n");
    printf("   Plant spirits guide us to elegant educational solutions.\n");
    
    return 0;
}

/*
 * ============================================================================
 * RANDY'S EDUCATIONAL USAGE AND BUILD INSTRUCTIONS
 * ============================================================================
 * 
 * Building the Cannabis-Enhanced VM:
 * $ gcc -o randy_cevm randy_cannabis_enhanced_vm.c
 * 
 * Running the Interactive Shell:
 * $ ./randy_cevm
 * 
 * Sample Session:
 * CEVM> reset
 * CEVM> load fib
 * CEVM> toke "Blue Dream"
 * CEVM> run
 * CEVM> regs
 * CEVM> wisdom
 * CEVM> quit
 * 
 * Educational Exploration:
 * 1. Load and run sample programs (fibonacci, hello world)
 * 2. Examine register states after execution
 * 3. Single-step through programs for debugging education
 * 4. Experiment with different cannabis strains for enhancement
 * 5. Study the instruction set and VM architecture
 * 
 * Randy's VM Philosophy:
 * "Why build x86_64 + AVX-512 complexity when 8-bit simplicity
 * teaches the fundamentals more clearly? This VM shows that
 * virtual machines aren't magic - they're just software that
 * simulates hardware. Every instruction, every register, every
 * flag is understandable with cannabis-enhanced clarity."
 * 
 * Complete Educational Stack Achievement:
 * Randy's journey now spans every layer of computing:
 * 
 * 🔧 Hardware Simulation (This VM)
 * 🔧 Firmware Programming (EFI Loader)  
 * 🔧 Kernel Development (Filesystem + Modules)
 * 🔧 Process Management (Scheduler)
 * 🔧 Network Programming (Custom Protocol)
 * 🔧 Application Development (All the above)
 * 
 * The mycelial network is complete. Students can now trace
 * computational consciousness from simulated silicon to
 * network protocols. The plant spirits smile upon this
 * educational achievement - simple, clear, and authentic.
 * 
 * "From AVX-512 nightmares to 8-bit dreams - sometimes the
 * best education comes from stepping back to fundamentals."
 */