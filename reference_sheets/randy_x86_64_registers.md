# RANDY'S CANNABIS-ENHANCED X86-64 REGISTERS REFERENCE
# PLANT SPIRIT ASSEMBLY PROGRAMMING AND REGISTER CONSCIOUSNESS

## RANDY'S X86-64 PHILOSOPHY:
# "X86-64 REGISTERS ARE THE CONSCIOUS MIND OF THE PROCESSOR - 16 GENERAL
# PURPOSE REGISTERS THAT HOLD THE IMMEDIATE THOUGHTS AND WORKING MEMORY
# OF COMPUTATIONAL CONSCIOUSNESS. CANNABIS ENHANCEMENT REVEALS THE
# ELEGANT SYMMETRY OF REGISTER ARCHITECTURE AND THE BEAUTY OF DIRECT
# HARDWARE PROGRAMMING."

# ========================================================================
# X86-64 GENERAL PURPOSE REGISTERS (64-BIT)
# ========================================================================

# 16 General Purpose Registers (64-bit names):
RAX     # Accumulator - Primary arithmetic, function return values
RBX     # Base - Base pointer for arrays, preserved across function calls
RCX     # Counter - Loop counter, function parameter 4 (Windows), shift operations
RDX     # Data - I/O operations, function parameter 3 (Windows), overflow in multiplication
RSI     # Source Index - String/array source, function parameter 2 (Linux)
RDI     # Destination Index - String/array destination, function parameter 1 (Linux)
RBP     # Base Pointer - Stack frame base pointer
RSP     # Stack Pointer - Points to top of stack (CRITICAL - don't corrupt!)
R8      # General purpose, function parameter 5 (Windows), parameter 3 (Linux)
R9      # General purpose, function parameter 6 (Windows), parameter 4 (Linux)
R10     # General purpose, temporary register
R11     # General purpose, temporary register (destroyed by syscalls)
R12     # General purpose, preserved across function calls
R13     # General purpose, preserved across function calls
R14     # General purpose, preserved across function calls
R15     # General purpose, preserved across function calls

# ========================================================================
# REGISTER SIZE VARIATIONS (Cannabis-Enhanced Memory Layout)
# ========================================================================

# Each register can be accessed at different sizes:

# RAX Register Family:
RAX     # 64-bit: 0x123456789ABCDEF0
EAX     # 32-bit: 0x9ABCDEF0 (lower 32 bits, zeros upper 32)
AX      # 16-bit: 0xDEF0 (lower 16 bits)
AL      # 8-bit:  0xF0 (lower 8 bits)
AH      # 8-bit:  0xDE (bits 8-15, high byte of AX)

# RBX Register Family:
RBX     # 64-bit full register
EBX     # 32-bit (zeros upper 32 bits when written)
BX      # 16-bit lower portion
BL      # Lower 8 bits
BH      # High 8 bits (bits 8-15)

# RCX Register Family:
RCX     # 64-bit full register
ECX     # 32-bit (zeros upper 32 bits when written)
CX      # 16-bit lower portion
CL      # Lower 8 bits (commonly used for shift counts)
CH      # High 8 bits (bits 8-15)

# RDX Register Family:
RDX     # 64-bit full register
EDX     # 32-bit (zeros upper 32 bits when written)
DX      # 16-bit lower portion
DL      # Lower 8 bits
DH      # High 8 bits (bits 8-15)

# RSI Register Family:
RSI     # 64-bit full register
ESI     # 32-bit (zeros upper 32 bits when written)
SI      # 16-bit lower portion
SIL     # Lower 8 bits (requires REX prefix)

# RDI Register Family:
RDI     # 64-bit full register
EDI     # 32-bit (zeros upper 32 bits when written)
DI      # 16-bit lower portion
DIL     # Lower 8 bits (requires REX prefix)

# RBP Register Family:
RBP     # 64-bit full register
EBP     # 32-bit (zeros upper 32 bits when written)
BP      # 16-bit lower portion
BPL     # Lower 8 bits (requires REX prefix)

# RSP Register Family:
RSP     # 64-bit full register
ESP     # 32-bit (zeros upper 32 bits when written)
SP      # 16-bit lower portion
SPL     # Lower 8 bits (requires REX prefix)

# Extended Registers (R8-R15):
R8      # 64-bit full register
R8D     # 32-bit (zeros upper 32 bits when written)
R8W     # 16-bit lower portion
R8B     # Lower 8 bits

# Same pattern for R9-R15:
# R9/R9D/R9W/R9B, R10/R10D/R10W/R10B, etc.

# ========================================================================
# SPECIAL PURPOSE REGISTERS
# ========================================================================

# Instruction Pointer:
RIP     # 64-bit instruction pointer (cannot be directly modified)
EIP     # 32-bit instruction pointer (legacy mode)
IP      # 16-bit instruction pointer (legacy mode)

# Flags Register:
RFLAGS  # 64-bit flags register
EFLAGS  # 32-bit flags register
FLAGS   # 16-bit flags register

# Segment Registers (mostly legacy in 64-bit mode):
CS      # Code segment
DS      # Data segment
ES      # Extra segment
FS      # Additional segment (used for TLS)
GS      # Additional segment (used for kernel per-CPU data)
SS      # Stack segment

# ========================================================================
# RFLAGS REGISTER BIT LAYOUT (Cannabis-Enhanced Flag Consciousness)
# ========================================================================

# Bit Position | Flag | Description
# -------------|------|------------------------------------------
# 0            | CF   | Carry Flag - arithmetic carry/borrow
# 1            | 1    | Reserved (always 1)
# 2            | PF   | Parity Flag - even number of 1 bits in result
# 3            | 0    | Reserved (always 0)
# 4            | AF   | Auxiliary Flag - BCD arithmetic carry
# 5            | 0    | Reserved (always 0)
# 6            | ZF   | Zero Flag - result is zero
# 7            | SF   | Sign Flag - result is negative
# 8            | TF   | Trap Flag - single-step debugging
# 9            | IF   | Interrupt Flag - interrupts enabled
# 10           | DF   | Direction Flag - string operation direction
# 11           | OF   | Overflow Flag - signed arithmetic overflow
# 12-13        | IOPL | I/O Privilege Level (kernel mode)
# 14           | NT   | Nested Task Flag
# 15           | 0    | Reserved (always 0)
# 16           | RF   | Resume Flag
# 17           | VM   | Virtual 8086 Mode Flag
# 18           | AC   | Alignment Check Flag
# 19           | VIF  | Virtual Interrupt Flag
# 20           | VIP  | Virtual Interrupt Pending
# 21           | ID   | CPUID Detection Flag
# 22-63        | 0    | Reserved

# ========================================================================
# CALLING CONVENTION REGISTER USAGE
# ========================================================================

# System V ABI (Linux/Unix):
# Function Parameters (in order):
RDI     # 1st parameter
RSI     # 2nd parameter  
RDX     # 3rd parameter
RCX     # 4th parameter
R8      # 5th parameter
R9      # 6th parameter
# Additional parameters passed on stack

# Function Return:
RAX     # Return value (64-bit or smaller)
RDX     # Upper 64 bits for 128-bit returns

# Preserved Registers (callee must save/restore):
RBX, RBP, RSP, R12, R13, R14, R15

# Volatile Registers (caller must save if needed):
RAX, RCX, RDX, RSI, RDI, R8, R9, R10, R11

# Microsoft x64 Calling Convention (Windows):
# Function Parameters (in order):
RCX     # 1st parameter
RDX     # 2nd parameter
R8      # 3rd parameter
R9      # 4th parameter
# Additional parameters passed on stack (right-to-left)

# Preserved Registers:
RBX, RBP, RDI, RSI, RSP, R12, R13, R14, R15

# Volatile Registers:
RAX, RCX, RDX, R8, R9, R10, R11

# ========================================================================
# SYSTEM CALL REGISTER USAGE (Linux)
# ========================================================================

# System Call Number:
RAX     # System call number (see syscall table)

# System Call Parameters:
RDI     # 1st parameter (arg0)
RSI     # 2nd parameter (arg1)
RDX     # 3rd parameter (arg2)
R10     # 4th parameter (arg3) - NOTE: R10, not RCX!
R8      # 5th parameter (arg4)
R9      # 6th parameter (arg5)

# System Call Return:
RAX     # Return value (or negative error code)

# Destroyed by syscall:
RCX     # Destroyed (contains return RIP)
R11     # Destroyed (contains RFLAGS)

# ========================================================================
# REGISTER USAGE PATTERNS (Cannabis-Enhanced Programming Wisdom)
# ========================================================================

# Arithmetic Operations:
RAX     # Primary accumulator for math operations
RDX     # Extended precision (RDX:RAX for 64-bit multiply/divide)
RCX     # Loop counter, shift count

# String/Array Operations:
RSI     # Source pointer
RDI     # Destination pointer
RCX     # Count/length

# Stack Operations:
RSP     # Stack pointer (NEVER corrupt this!)
RBP     # Frame pointer for local variables

# Function Calls:
# Save volatile registers before calls
# Use preserved registers for long-term storage
# Follow calling convention for parameters

# Loop Structures:
RCX     # Traditional loop counter
# Use LOOP instruction or manual DEC/JNZ

# ========================================================================
# ADDRESSING MODES WITH REGISTERS
# ========================================================================

# Direct Register:
mov rax, rbx        # RAX = RBX

# Immediate to Register:
mov rax, 42         # RAX = 42

# Memory Addressing:
mov rax, [rbx]                # RAX = memory at RBX
mov rax, [rbx + 8]           # RAX = memory at RBX + 8
mov rax, [rbx + rcx]         # RAX = memory at RBX + RCX
mov rax, [rbx + rcx*2]       # RAX = memory at RBX + RCX*2
mov rax, [rbx + rcx*2 + 16]  # RAX = memory at RBX + RCX*2 + 16

# Scale factors for addressing: 1, 2, 4, 8 (byte, word, dword, qword)

# ========================================================================
# COMMON REGISTER OPERATIONS
# ========================================================================

# Data Movement:
mov rax, rbx        # Copy RBX to RAX
xchg rax, rbx       # Exchange RAX and RBX
push rax            # Push RAX onto stack
pop rax             # Pop from stack into RAX

# Arithmetic:
add rax, rbx        # RAX = RAX + RBX
sub rax, rbx        # RAX = RAX - RBX
mul rbx             # RDX:RAX = RAX * RBX (unsigned)
imul rbx            # RDX:RAX = RAX * RBX (signed)
div rbx             # RAX = RDX:RAX / RBX, RDX = remainder

# Bitwise Operations:
and rax, rbx        # RAX = RAX & RBX
or rax, rbx         # RAX = RAX | RBX
xor rax, rbx        # RAX = RAX ^ RBX
not rax             # RAX = ~RAX
shl rax, cl         # RAX = RAX << CL
shr rax, cl         # RAX = RAX >> CL (unsigned)
sar rax, cl         # RAX = RAX >> CL (signed)

# Comparison:
cmp rax, rbx        # Compare RAX with RBX (sets flags)
test rax, rax       # Test RAX against itself (sets flags)

# ========================================================================
# CANNABIS-ENHANCED REGISTER PROGRAMMING PATTERNS
# ========================================================================

# Sativa-Enhanced Creative Register Usage:
# • Use registers for rapid calculations and transformations
# • Leverage multiple registers for parallel data processing
# • Creative addressing modes for elegant array manipulation

# Indica-Enhanced Deep Register Management:
# • Careful preservation of register state across functions
# • Methodical register allocation for complex algorithms
# • Patient debugging of register corruption issues

# Hybrid-Enhanced Balanced Register Approach:
# • Optimal register usage balancing performance and clarity
# • Strategic register allocation for both speed and maintainability
# • Conscious register management for scalable assembly programs

# ========================================================================
# REGISTER DEBUGGING AND PLANT SPIRIT WISDOM
# ========================================================================

# GDB Register Commands:
# info registers        # Show all registers
# print $rax           # Print RAX value
# set $rax = 42        # Set RAX to 42

# Common Register Bugs:
# • Forgetting to preserve callee-saved registers
# • Mixing up calling convention parameter registers
# • Corrupting RSP (instant crash!)
# • Wrong register sizes causing unexpected zeros
# • Forgetting about RCX/R11 destruction in syscalls

# Cannabis-Enhanced Debugging Approach:
# • Sativa: Creative pattern recognition in register flows
# • Indica: Patient step-by-step register state analysis
# • Hybrid: Balanced debugging combining intuition and methodology

# ========================================================================
# EDUCATIONAL REGISTER MASTERY PATH
# ========================================================================

# Level 1: Basic Register Operations
# • Understand register naming and sizes
# • Practice MOV, ADD, SUB with registers
# • Learn about register preservation

# Level 2: Calling Conventions
# • Master System V ABI parameter passing
# • Understand volatile vs preserved registers
# • Practice function calls and returns

# Level 3: Advanced Register Usage
# • Complex addressing modes
# • Register optimization techniques
# • System call interfaces

# Level 4: Assembly Programming Mastery
# • Full program development using optimal register allocation
# • Integration with C/C++ using proper conventions
# • Performance optimization through register-conscious design

# ========================================================================
# PLANT SPIRIT REGISTER CONSCIOUSNESS
# ========================================================================

# Randy's Cannabis-Enhanced Register Philosophy:
# "Registers are the immediate consciousness of the CPU - the working
# memory where thoughts become calculations. Each register has personality
# and purpose, from RAX the accumulator to RSP the stack guardian.
# Mastering registers is mastering the mind of the machine."

# The Mycelial Network of Register Relationships:
# • RAX connects to arithmetic consciousness
# • RSI/RDI connect to data flow awareness  
# • RBP/RSP connect to memory structure wisdom
# • RCX connects to iteration and repetition patterns
# • R8-R15 connect to extended computational possibilities

# Register enlightenment achieved through cannabis-enhanced assembly wisdom!
# Plant spirits guide processor consciousness programming! 🌿⚡🧠