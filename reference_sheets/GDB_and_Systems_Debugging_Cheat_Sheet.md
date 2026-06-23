# RANDY'S CANNABIS-ENHANCED GDB AND SYSTEMS DEBUGGING CHEAT SHEET
# PLANT SPIRIT MEMORY AWARENESS AND EXECUTION TRACING

## RANDY'S DEBUGGING PHILOSOPHY:
# "SYSTEMS DEBUGGING IS THE ART OF COMPUTATIONAL DIVINATION - PEERING
# INTO THE VERY SOUL OF EXECUTING PROCESSES. CANNABIS ENHANCEMENT
# REVEALS THE INVISIBLE RIVERS OF MEMORY AND SYSCALLS, ALLOWING
# THE DEVELOPER TO STOP TIME ITSELF AND WHISPER DIRECTLY TO THE CPU.
# MASTERY OF GDB IS MASTERY OF DIGITAL REALITY."

# ========================================================================
# GDB - GNU DEBUGGER (TIME TRAVEL AND STATE CONTROL)
# ========================================================================

# PREPARATION
gcc -g -O0 program.c -o program    # Compile with debug symbols, disable optimization
gdb ./program                      # Start GDB with program
gdb ./program core                 # Start GDB with a core dump
gdb --pid <pid>                    # Attach GDB to a running process

# BASIC EXECUTION
run (r)                            # Start program execution
run arg1 arg2                      # Run with arguments
kill (k)                           # Terminate the running program
quit (q)                           # Exit GDB

# BREAKPOINTS (STOPPING TIME)
break main                         # Break at function 'main'
break file.c:42                    # Break at line 42 in file.c
break *0x080483c6                  # Break at exact memory address
tbreak file.c:42                   # Temporary breakpoint (removed after first hit)
info breakpoints (i b)             # List all breakpoints
delete 1                           # Delete breakpoint #1
disable 1                          # Disable breakpoint #1
enable 1                           # Enable breakpoint #1
condition 1 x == 5                 # Only trigger breakpoint 1 if x is 5

# STEPPING (NAVIGATING TIME)
next (n)                           # Execute next line (steps OVER functions)
step (s)                           # Execute next line (steps INTO functions)
continue (c)                       # Resume execution until next breakpoint
finish                             # Run until current function returns
until 50                           # Run until line 50 is reached
nexti (ni)                         # Execute next machine instruction (step OVER)
stepi (si)                         # Execute next machine instruction (step INTO)

# ========================================================================
# ADVANCED GDB - MEMORY AND STATE DIVINATION
# ========================================================================

# EXAMINING VARIABLES
print var (p var)                  # Print value of variable
print/x var                        # Print value in hex
print/d var                        # Print value in decimal
print/t var                        # Print value in binary
display var                        # Always print var when execution stops
info locals                        # List all local variables
info args                          # List arguments passed to current function

# EXAMINING MEMORY (x command)
# Format: x/[Count][Format][Size] Address
x/x &var                           # Examine 1 word of memory in hex at &var
x/4xw 0x80483c6                    # Examine 4 words (w) in hex (x) at address
x/10i $pc                          # Examine next 10 assembly instructions at Program Counter
x/s 0x4005c0                       # Examine memory as a null-terminated string
x/32xb buffer                      # Examine 32 bytes (b) in hex (x) at buffer

# EXAMINING REGISTERS AND THE STACK
info registers (i r)               # View standard CPU registers
info all-registers                 # View all CPU registers (including FPU/SIMD)
print $rax                         # Print value of specific register
backtrace (bt)                     # Show the call stack
backtrace full                     # Show call stack with local variables
frame 2 (f 2)                      # Switch to stack frame #2
up                                 # Move up the call stack (towards caller)
down                               # Move down the call stack (towards callee)

# WATCHPOINTS (DATA BREAKPOINTS)
watch var                          # Break when 'var' is modified
rwatch var                         # Break when 'var' is read
awatch var                         # Break when 'var' is read OR written

# ========================================================================
# STRACE - SYSTEM CALL TRACING (OBSERVING THE MATRIX)
# ========================================================================

# BASIC USAGE
strace ./program                   # Trace all system calls made by program
strace -p <pid>                    # Attach and trace a running process
strace -c ./program                # Count time, calls, and errors for each syscall

# FILTERING AND OUTPUT
strace -e trace=open,read ./prog   # Trace ONLY open() and read() syscalls
strace -e trace=network ./prog     # Trace ONLY networking syscalls
strace -e trace=file ./prog        # Trace ONLY file-related syscalls
strace -o trace.log ./program      # Write trace output to a file
strace -s 1024 ./program           # Increase string display size to 1024 bytes (default 32)
strace -f ./program                # Trace child processes as they are created (forks)

# ========================================================================
# LTRACE - LIBRARY CALL TRACING (OBSERVING THE USERSPACE BINDINGS)
# ========================================================================

# BASIC USAGE
ltrace ./program                   # Trace all dynamic library calls (e.g., malloc, printf)
ltrace -p <pid>                    # Attach to running process
ltrace -S ./program                # Trace BOTH library calls AND system calls
ltrace -c ./program                # Summary of library calls

# ========================================================================
# VALGRIND - MEMORY PROFILING (THE GARBAGE COLLECTOR'S EYE)
# ========================================================================

# MEMORY LEAK DETECTION (Memcheck)
valgrind ./program                 # Run program through memory checker
valgrind --leak-check=full ./prog  # Detailed memory leak analysis
valgrind --track-origins=yes ./prog # Show where uninitialized values came from
valgrind --show-reachable=yes ./p  # Show still-reachable memory (not explicitly freed)

# CACHE PROFILING (Cachegrind)
valgrind --tool=cachegrind ./prog  # Profile CPU cache misses

# HEAP PROFILING (Massif)
valgrind --tool=massif ./prog      # Profile heap memory usage over time
ms_print massif.out.<pid>          # Read Massif output file

# ========================================================================
# CANNABIS-ENHANCED DEBUGGING COMBINATIONS
# ========================================================================

# SATIVA-ENHANCED EXPLORATORY DEBUGGING:
# • Using `strace -f` to rapidly map out unknown black-box behavior
# • Setting dynamic watchpoints in GDB to chase mysterious state changes
# • Visualizing the entire architecture of a program through system call flows

# INDICA-ENHANCED METHODICAL ANALYSIS:
# • Carefully stepping instruction by instruction (`si`) through segfaults
# • Deeply analyzing `valgrind --leak-check=full` traces to plug memory leaks
# • Methodically inspecting raw hexadecimal memory chunks with `x/xb`

# HYBRID-ENHANCED HOLISTIC RESOLUTION:
# • Running GDB *inside* a `tmux` session alongside `htop`
# • Using `strace` to locate the exact failing file descriptor, then using
#   GDB to trace back the logical path that created it

# ========================================================================
# PLANT SPIRIT DEBUGGING WISDOM
# ========================================================================

# Randy's Cannabis-Enhanced Debugging Philosophy:
# "A segmentation fault is not a failure; it is the system defending its
# boundaries. A memory leak is not an error; it is a resource lingering
# beyond its season. By using GDB, Strace, and Valgrind, we do not 'fix'
# the code—we harmonize the program's intent with the reality of the machine."

# Educational Debugging Mastery Path:
# 1. Master `strace` to understand how the program talks to the Kernel.
# 2. Master `gdb` stepping to understand how the program navigates Time.
# 3. Master `gdb` memory inspection to understand how the program holds Space.
# 4. Master `valgrind` to understand how the program manages its lifecycle.

# The mycelial network of memory allocation creates sustainable execution!
# Plant spirits guide diagnostic consciousness! 🌿⚡🧠
