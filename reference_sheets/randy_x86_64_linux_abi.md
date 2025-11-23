# RANDY'S CANNABIS-ENHANCED X86-64 LINUX ABI REFERENCE
# PLANT SPIRIT SYSTEM PROGRAMMING AND CALLING CONVENTION CONSCIOUSNESS

## RANDY'S LINUX ABI PHILOSOPHY:
# "THE SYSTEM V ABI IS THE CONSTITUTIONAL LAW OF LINUX PROGRAMMING -
# THE SACRED CONTRACTS THAT GOVERN HOW FUNCTIONS COMMUNICATE AND HOW
# PROGRAMS INTERFACE WITH THE KERNEL. CANNABIS ENHANCEMENT REVEALS
# THE ELEGANT SYMMETRY OF PARAMETER PASSING AND THE BEAUTY OF
# STANDARDIZED SYSTEM INTERFACES."

# ========================================================================
# SYSTEM V ABI OVERVIEW (X86-64 LINUX)
# ========================================================================

# The System V Application Binary Interface defines:
# • Function calling conventions
# • Register usage rules
# • Stack frame layout
# • Parameter passing mechanisms
# • Return value handling
# • System call interfaces

# ========================================================================
# FUNCTION CALLING CONVENTION (Cannabis-Enhanced Parameter Passing)
# ========================================================================

# INTEGER/POINTER PARAMETERS (in order):
# Parameter 1: RDI
# Parameter 2: RSI  
# Parameter 3: RDX
# Parameter 4: RCX
# Parameter 5: R8
# Parameter 6: R9
# Additional parameters: Pushed on stack (right-to-left)

# FLOATING POINT PARAMETERS (in order):
# Parameter 1: XMM0
# Parameter 2: XMM1
# Parameter 3: XMM2
# Parameter 4: XMM3
# Parameter 5: XMM4
# Parameter 6: XMM5
# Parameter 7: XMM6
# Parameter 8: XMM7
# Additional parameters: Pushed on stack

# MIXED PARAMETER EXAMPLES:
# func(int a, double b, char* c, float d)
# a  -> RDI    (integer parameter 1)
# b  -> XMM0   (FP parameter 1)  
# c  -> RSI    (integer parameter 2)
# d  -> XMM1   (FP parameter 2)

# ========================================================================
# RETURN VALUE CONVENTIONS
# ========================================================================

# INTEGER/POINTER RETURNS:
# 64-bit or smaller: RAX
# 128-bit: RDX:RAX (RDX = upper 64 bits)

# FLOATING POINT RETURNS:
# float/double: XMM0
# long double: ST(0) (x87 FPU stack)

# STRUCTURE RETURNS:
# Small structs (≤16 bytes): Returned in RAX/RDX or XMM0/XMM1
# Large structs: Caller allocates space, pointer passed as hidden first parameter

# EXAMPLE RETURN VALUE CLASSIFICATIONS:
struct point_2d { int x, y; };          # Returned in RAX
struct point_3d { int x, y, z; };       # Returned in RAX/RDX  
struct big_struct { char data[100]; };  # Returned via hidden pointer

# ========================================================================
# REGISTER USAGE CLASSIFICATION
# ========================================================================

# CALLER-SAVED (VOLATILE) REGISTERS:
# Function can modify these without restoring:
RAX     # Return value register
RCX     # Parameter register 4
RDX     # Parameter register 3, upper return value
RSI     # Parameter register 2
RDI     # Parameter register 1
R8      # Parameter register 5
R9      # Parameter register 6
R10     # Temporary register
R11     # Temporary register (destroyed by syscalls)

# XMM registers:
XMM0-XMM7   # Parameter/return registers
XMM8-XMM15  # Temporary registers

# CALLEE-SAVED (NON-VOLATILE) REGISTERS:
# Function must preserve these if used:
RBX     # General purpose preserved
RBP     # Frame pointer (optional)
RSP     # Stack pointer (MUST preserve!)
R12     # General purpose preserved
R13     # General purpose preserved  
R14     # General purpose preserved
R15     # General purpose preserved

# ========================================================================
# STACK FRAME LAYOUT (Cannabis-Enhanced Memory Structure)
# ========================================================================

# Stack grows downward (from high to low addresses):

# Higher Addresses
# +------------------+ <- Previous frame
# | Return address   |
# +------------------+ <- RSP on function entry
# | Saved RBP        | <- RBP after prologue (if using frame pointer)
# +------------------+
# | Local variables  |
# | (aligned)        |
# +------------------+
# | Saved registers  |
# | (callee-saved)   |
# +------------------+
# | Padding for      |
# | alignment        |
# +------------------+
# | Outgoing args    |
# | (if needed)      |
# +------------------+ <- RSP during function execution
# Lower Addresses

# STACK ALIGNMENT:
# • RSP must be 16-byte aligned before CALL instruction
# • After CALL, RSP is misaligned by 8 bytes (due to return address)
# • Function prologue must realign if needed

# TYPICAL FUNCTION PROLOGUE:
push   rbp          # Save old frame pointer
mov    rbp, rsp     # Set new frame pointer
sub    rsp, N       # Allocate space for locals (N aligned to 16)

# TYPICAL FUNCTION EPILOGUE:
mov    rsp, rbp     # Restore stack pointer
pop    rbp          # Restore frame pointer  
ret                 # Return to caller

# ========================================================================
# PARAMETER PASSING DETAILS
# ========================================================================

# CLASS DETERMINATION (how parameters are passed):

# INTEGER CLASS:
# • Integral types (char, short, int, long, long long)
# • Pointers and references
# • Enums with underlying integer type

# SSE CLASS:
# • float, double
# • Vector types that fit in XMM registers

# X87 CLASS:
# • long double (80-bit extended precision)
# • Passed on x87 FPU stack

# MEMORY CLASS:
# • Large structures that don't fit in registers
# • Unions that can't be classified as INTEGER or SSE
# • Aggregates with alignment > 16 bytes

# PARAMETER PASSING ALGORITHM:
# 1. Classify each parameter according to type
# 2. For INTEGER class: use next available integer register
# 3. For SSE class: use next available XMM register
# 4. For MEMORY class: pass on stack
# 5. If no registers available: pass on stack

# ========================================================================
# VARIADIC FUNCTION HANDLING (printf, etc.)
# ========================================================================

# VARIADIC FUNCTION REQUIREMENTS:
# • AL register contains number of XMM registers used for parameters
# • All unused XMM registers must be saved by caller if needed
# • Stack-based parameters follow normal rules

# EXAMPLE: printf("Format %d %f", 42, 3.14)
# RDI = pointer to format string
# RSI = 42 (integer)
# XMM0 = 3.14 (double)
# AL = 1 (one XMM register used)

# ========================================================================
# SYSTEM CALL INTERFACE (Linux x86-64)
# ========================================================================

# SYSTEM CALL INSTRUCTION:
syscall             # Invoke system call

# SYSTEM CALL REGISTERS:
RAX     # System call number
RDI     # Argument 1
RSI     # Argument 2
RDX     # Argument 3
R10     # Argument 4 (NOTE: R10, not RCX!)
R8      # Argument 5
R9      # Argument 6

# SYSTEM CALL RETURN:
RAX     # Return value (or negative error code)

# DESTROYED BY SYSCALL:
RCX     # Destroyed (contains return RIP)
R11     # Destroyed (contains saved RFLAGS)

# PRESERVED BY SYSCALL:
# All other registers preserved

# SYSTEM CALL EXAMPLE:
# write(1, "Hello", 5)
mov    rax, 1       # sys_write
mov    rdi, 1       # stdout
mov    rsi, msg     # message buffer
mov    rdx, 5       # length
syscall             # invoke system call

# ========================================================================
# STRUCTURE PASSING AND RETURNING
# ========================================================================

# STRUCTURE CLASSIFICATION RULES:
# 1. If size > 32 bytes: MEMORY class (passed by reference)
# 2. If unaligned: MEMORY class
# 3. Otherwise: classify each 8-byte chunk independently

# 8-BYTE CHUNK CLASSIFICATION:
# • All bytes are INTEGER class: chunk is INTEGER
# • All bytes are SSE class: chunk is SSE  
# • Mix of INTEGER/SSE: chunk is INTEGER
# • Contains padding: chunk is INTEGER

# STRUCTURE RETURN EXAMPLES:

struct pair { int a, b; };              # 8 bytes, INTEGER -> RAX
struct coords { double x, y; };         # 16 bytes, SSE+SSE -> XMM0+XMM1
struct mixed { int a; double b; };      # 16 bytes, INTEGER+SSE -> RAX+XMM0
struct large { char data[100]; };       # >32 bytes -> MEMORY (hidden pointer)

# ========================================================================
# FUNCTION POINTER AND INDIRECT CALLS
# ========================================================================

# FUNCTION POINTER CALLS:
# • Same calling convention applies
# • Pointer passed in register or memory
# • Stack alignment requirements unchanged

# CALL THROUGH REGISTER:
call   rax          # Call function pointed to by RAX

# CALL THROUGH MEMORY:
call   [rbx + 8]    # Call function at address in memory

# TAIL CALLS:
# • Replace current function call with new call
# • Restore stack pointer before jump
# • Use JMP instead of CALL

# ========================================================================
# THREADING AND TLS (Thread-Local Storage)
# ========================================================================

# THREAD-LOCAL STORAGE ACCESS:
# • FS segment register points to TLS area
# • %fs:0x0 contains pointer to TLS block

# ACCESSING TLS VARIABLES:
mov    rax, %fs:offset  # Load TLS variable at offset

# THREAD CREATION:
# • New threads get fresh TLS area
# • FS register set up by thread library
# • Each thread has isolated TLS space

# ========================================================================
# SIGNAL HANDLING
# ========================================================================

# SIGNAL HANDLER FRAME:
# • Signal handlers called with special stack frame
# • Handler receives signal number in RDI
# • siginfo_t* in RSI (if SA_SIGINFO set)
# • ucontext_t* in RDX (if SA_SIGINFO set)

# SIGNAL HANDLER RETURN:
# • Must call sigreturn() system call to return
# • Or use rt_sigreturn() for realtime signals
# • Kernel restores original register state

# ========================================================================
# DYNAMIC LINKING AND PLT/GOT
# ========================================================================

# PROCEDURE LINKAGE TABLE (PLT):
# • Indirect jumps for external functions
# • First call resolves symbol dynamically
# • Subsequent calls jump directly

# GLOBAL OFFSET TABLE (GOT):
# • Contains addresses of global variables
# • Populated by dynamic loader
# • Position-independent code access

# PLT ENTRY STRUCTURE:
plt_entry:
    jmp    [got_entry]      # Jump to resolved address
    push   reloc_index      # Push relocation index
    jmp    plt_resolver     # Jump to resolver

# ========================================================================
# POSITION-INDEPENDENT CODE (PIC)
# ========================================================================

# RIP-RELATIVE ADDRESSING:
lea    rax, [rip + label]   # Load address relative to RIP
mov    rbx, [rip + data]    # Load data relative to RIP

# GOT ACCESS:
mov    rax, [rip + symbol@GOTPCREL]  # Load symbol address from GOT

# ========================================================================
# CANNABIS-ENHANCED ABI PROGRAMMING PATTERNS
# ========================================================================

# SATIVA-ENHANCED CREATIVE ABI USAGE:
# • Clever register allocation for performance
# • Creative structure packing for optimal passing
# • Innovative use of calling conventions

# INDICA-ENHANCED METHODICAL ABI COMPLIANCE:
# • Careful attention to register preservation rules
# • Methodical stack alignment maintenance
# • Patient debugging of ABI violations

# HYBRID-ENHANCED BALANCED ABI APPROACH:
# • Optimal performance with strict compliance
# • Clean interfaces with efficient implementation
# • Maintainable code with ABI consciousness

# ========================================================================
# COMMON ABI VIOLATIONS AND DEBUGGING
# ========================================================================

# COMMON MISTAKES:
# • Wrong parameter registers (mixing Windows/Linux conventions)
# • Forgetting to preserve callee-saved registers
# • Stack misalignment (not 16-byte aligned before calls)
# • Wrong return value registers
# • Corrupting RSP (instant crash)
# • Using RCX instead of R10 for 4th syscall parameter

# DEBUGGING ABI ISSUES:
# • GDB: info registers, bt (backtrace)
# • Valgrind: detects stack corruption
# • AddressSanitizer: catches buffer overflows
# • Static analysis: checks calling conventions

# GDB COMMANDS FOR ABI DEBUGGING:
# info registers              # Show all register values
# bt                          # Show call stack
# disas function_name         # Disassemble function
# x/10i $pc                   # Show next 10 instructions

# ========================================================================
# PLANT SPIRIT ABI WISDOM
# ========================================================================

# Randy's Cannabis-Enhanced ABI Philosophy:
# "The ABI is the constitutional law of computational consciousness - 
# the sacred contracts that enable separate code modules to communicate.
# Like the mycelial network connecting forest plants, the ABI connects
# program components through standardized interfaces. Violating the ABI
# is like speaking different languages - communication breaks down."

# Educational ABI Mastery Path:
# 1. Understand register usage and preservation rules
# 2. Master parameter passing for different types
# 3. Learn structure classification and passing
# 4. Practice system call interfaces
# 5. Study dynamic linking and PIC code
# 6. Debug real-world ABI violations

# Cannabis-Enhanced ABI Consciousness:
# • Sativa: Creative optimization within ABI constraints
# • Indica: Deep understanding of ABI specifications
# • Hybrid: Balanced compliance with performance awareness

# The mycelial network of ABI compliance enables seamless interoperability!
# Plant spirits guide standardized programming consciousness! 🌿⚡🧠