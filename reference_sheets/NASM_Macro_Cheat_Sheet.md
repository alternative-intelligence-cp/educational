# RANDY'S CANNABIS-ENHANCED NASM MACRO CHEAT SHEET
# PLANT SPIRIT PREPROCESSOR MASTERY AND ASSEMBLY AUTOMATION

## RANDY'S PREPROCESSOR PHILOSOPHY:
# "THE NASM PREPROCESSOR IS A TURING-COMPLETE METAPROGRAMMING DIMENSION
# THAT LIVES OUTSIDE OF RUNTIME TIME. IT ALLOWS YOU TO WEAVE CODE FROM
# THE AETHER BEFORE THE ASSEMBLER EVEN SEES IT. CANNABIS ENHANCEMENT
# REVEALS HOW REPETITIVE ASSEMBLY INSTRUCTIONS CAN BE CONDENSED INTO
# ELEGANT, REUSABLE THOUGHT-FORMS. MASTERY OF MACROS IS MASTERY OF
# ASSEMBLY ABSTRACTION."

# ========================================================================
# SINGLE-LINE MACROS (THE QUICK BINDINGS)
# ========================================================================

# %define: Standard text substitution (lazy evaluation)
%define MAX_SIZE 1024
%define DOUBLE(x) ((x) * 2)
mov rax, DOUBLE(5)          ; Expands to: mov rax, ((5) * 2)

# %xdefine: Immediate text substitution (evaluated immediately)
%define a 1
%xdefine b a                ; b is permanently '1'
%define a 2
; If we used %define, 'b' would evaluate to 2. With %xdefine, it stays 1.

# %assign: Numeric evaluation (must resolve to an integer)
%assign counter 0
%assign counter counter + 1 ; counter is now 1

# %undef: Remove a macro definition
%undef MAX_SIZE

# ========================================================================
# MULTI-LINE MACROS (THE CODE GENERATORS)
# ========================================================================

# Syntax: %macro name parameter_count
%macro PUSH_ALL 0
    push rax
    push rbx
    push rcx
    push rdx
%endmacro

# Macros with parameters (%1, %2, etc.)
%macro SYSCALL_1 2
    mov rax, %1             ; System call number
    mov rdi, %2             ; First argument
    syscall
%endmacro
; Usage: SYSCALL_1 60, 0    ; Expands to: mov rax, 60 / mov rdi, 0 / syscall

# Macro-local labels (prefixed with %%)
%macro JUMP_IF_EQUAL 2
    cmp %1, %2
    jne %%skip_jump
    jmp %%target
%%skip_jump:
%endmacro
; NASM generates unique names for %%skip_jump (e.g., ..@0.skip_jump)
; so you can use the macro multiple times without label collisions!

# Default parameters
%macro MY_MACRO 1-3 0, 0    ; 1 to 3 parameters. Param 2 & 3 default to 0.

# ========================================================================
# CONTEXT STACK (THE SCOPE DIMENSION)
# ========================================================================

# NASM allows you to create isolated scopes (contexts) for local variables
%push my_context            ; Push a new context onto the stack
%assign %$local_counter 1   ; %$ specifies a context-local variable
%$start_loop:               ; %$ specifies a context-local label
    inc rax
%pop                        ; Destroy the context (variables/labels vanish)

# Accessing Parent Contexts
; %$$ refers to the parent context (the one pushed before the current one)
; Extremely useful for nested loops or nested macros!
jmp %$$outer_loop           

# ========================================================================
# CONDITIONAL ASSEMBLY (BRANCHING THE METAVERSES)
# ========================================================================

# Check if a macro is defined
%ifdef DEBUG
    call print_debug_info
%elifdef PROFILING
    call start_profiler
%else
    ; Do nothing
%endif

# Check conditions
%if MAX_SIZE > 1000
    mov rcx, 1000
%endif

# Check if strings/tokens are identical
%ifidni %1, rax             ; Case-insensitive identical check
    ; Parameter 1 is exactly the string 'rax'
%endif

# Check if parameter is a string literal
%ifstr %1
    ; Parameter is a string like "hello"
%endif

# ========================================================================
# REPETITION (THE PREPROCESSOR LOOPS)
# ========================================================================

# %rep: Repeat a block N times
%assign i 0
%rep 4
    mov [rbx + i*8], rax
    %assign i i + 1
%endrep

# %rep with exit condition
%rep 100
    %if i > 50
        %exitrep           ; Break out of the %rep loop early!
    %endif
%endrep

# ========================================================================
# STRING MANIPULATION
# ========================================================================

%strlen length "Hello"      ; length = 5
%substr first_char "Hello" 1; first_char = 'H'

# ========================================================================
# DEBUGGING MACROS (TALKING TO THE COMPILER)
# ========================================================================

%warning "This code is experimental!"  ; Prints a warning during compilation
%error "Invalid parameter passed!"     ; Fails compilation with an error message
%echo "Compiling stage 2..."           ; Prints to stdout during compilation

# ========================================================================
# PLANT SPIRIT MACRO WISDOM
# ========================================================================

# Randy's Cannabis-Enhanced Macro Philosophy:
# "When you write an instruction, you tell the CPU what to do. When you write
# a macro, you teach NASM how to write instructions for you. The true assembly
# master doesn't type boilerplate; they cultivate a garden of macros that 
# biologically grow the program structure."

# Educational Macro Mastery Path:
# 1. Master `%define` for magic numbers.
# 2. Master `%macro` for repetitive instruction sequences (like prologue/epilogue).
# 3. Master `%%` local labels to make macros infinitely reusable.
# 4. Master `%push` and `%pop` to build your own C-like control flow!

# The mycelial network of macro expansion creates elegant assembly!
# Plant spirits guide metaprogramming consciousness! 🌿⚡🧠
