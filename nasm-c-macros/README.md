# NASM C-Style Macros

**Building C-like control flow and abstractions using NASM's context stack**

This project demonstrates how to create high-level programming constructs (if/else, loops, classes, functions) using pure NASM macro preprocessing. No external tools - just the power of NASM's context stack (`%push`/`%pop`) and label scoping (`%$label`).

## Why This Exists

Learning assembly is hard. Learning NASM macros is harder. The official documentation is sparse, and most examples are trivial. This project shows **real-world** macro programming - the kind that takes days of experimentation to figure out.

If you've ever wondered:
- "How do compilers turn `if/else` into jumps?"
- "What's this context stack thing in NASM?"
- "Can I make my own programming language with macros?"

You're in the right place.

## What's Implemented

### Control Flow (`lib/control.asm`)

**If/Else/Elif:**
```nasm
cmp rax, 5
if g                ; if (rax > 5)
    ; then block
else
    ; else block
endi
```

Supports: `e, ne, g, ge, l, le, a, ae, b, be, z, nz` (all conditional jumps)

**While Loops:**
```nasm
while
    cmp rcx, 0
    je break        ; or just: break
    ; loop body
    dec rcx
endw
```

**For Loops:**
```nasm
mov rcx, 10
for
    cmp rcx, 0
    je break
    ; loop body
    dec rcx
endf
```

**Switch/Case:**
```nasm
mov rax, value
switch
    case 1
        ; code
        break
    case 2
        ; code
        break
    default
        ; default code
ends
```

**Break/Continue:**
Work with `while`, `for`, and `switch` contexts automatically.

### Classes (`lib/classes.asm`)

Define structures and methods with automatic `this` pointer handling:

```nasm
; Define a class structure
struc Point
    .x: resq 1
    .y: resq 1
endstruc

; Create instances
section .bss
    point1: resb Point_size
    point2: resb Point_size

; Define methods
Point_set:
    ; rdi = this pointer (System V calling convention)
    mov [rdi + Point.x], rsi
    mov [rdi + Point.y], rdx
    ret

; Call methods
lea rdi, [point1]
mov rsi, 10
mov rdx, 20
call Point_set
```

### Functions (`lib/functions.asm`)

Automatic stack frame and register preservation:

```nasm
; Simple function
func add_numbers
    mov rax, rdi
    add rax, rsi
endfunc

; Function with preserved registers
func complex_calc, rbx, r12, r13
    ; rbx, r12, r13 automatically saved/restored
    ; Do complex work...
endfunc

; Call it
mov rdi, 7
mov rsi, 3
call add_numbers  ; rax = 10
```

## How It Works: The Context Stack

NASM's context stack is like a compile-time call stack. Each `%push` creates a new scope with unique labels:

```nasm
%macro if 1
    %push if_ctx          ; Create new context
    jne %$else_label      ; %$ = current context's label
%endmacro

%macro else 0
    jmp %$end_label
  %$else_label:           ; Define the label
%endmacro

%macro endi 0
  %$end_label:
    %pop                  ; Destroy context
%endmacro
```

When you nest:
```nasm
if g
    if l              ; Creates SECOND context
        ; Inner if has different %$else_label
    endi              ; Pops inner context
endi                  ; Pops outer context
```

Each context gets unique labels (like `..@4.else_label`, `..@7.else_label`), preventing conflicts.

## Technical Details

### Label Scoping

- `%%label` - Local to macro invocation (single use)
- `%$label` - Local to current context (survives until `%pop`)
- `%$$label` - Parent context's label

### Variable Assignment

- `%assign` - Numeric variables (can do math)
- `%define` - Text substitution (simple)
- `%xdefine` - Expanded text substitution (evaluates immediately)

### Challenges We Solved

1. **Elif chains** - Each elif needs unique labels but must connect to previous branches
2. **Nested contexts** - Inner loops must not break outer loops
3. **Register preservation** - Saving/restoring arbitrary registers in LIFO order
4. **Switch fallthrough** - Handling break vs fallthrough behavior

## Testing

```bash
cd tests
nasm -f elf64 test_if.asm -o test_if.o && ld test_if.o -o test_if && ./test_if
nasm -f elf64 test_while.asm -o test_while.o && ld test_while.o -o test_while && ./test_while
nasm -f elf64 test_switch.asm -o test_switch.o && ld test_switch.o -o test_switch && ./test_switch
nasm -f elf64 test_class.asm -o test_class.o && ld test_class.o -o test_class && ./test_class
nasm -f elf64 test_functions.asm -o test_functions.o && ld test_functions.o -o test_functions && ./test_functions
```

## Educational Value

This project shows:

1. **How compilers work** - C's `if/else` is just macros over jumps
2. **Macro metaprogramming** - Building a DSL in assembly
3. **Context stack mastery** - NASM's most powerful (and undocumented) feature
4. **Calling conventions** - How functions actually pass arguments
5. **Stack discipline** - Why alignment matters (16-byte on x86-64)

## Limitations

- No type checking (it's assembly!)
- Error messages can be cryptic
- Stack alignment is manual for complex cases
- Lambdas are partially implemented (complex scope issues)

## Future Ideas

- Function overloading (different arities)
- Try/catch exception handling
- Defer statements (Go-style)
- Generators/coroutines
- Full lambda/closure support
- Compile-time computation (constexpr)

## Learning Resources

- [NASM Manual - Section 4.7: Context Stack](https://www.nasm.us/doc/nasmdoc4.html#section-4.7)
- [x86-64 Calling Convention](https://wiki.osdev.org/Calling_Conventions)
- [System V ABI](https://refspecs.linuxbase.org/elf/x86_64-abi-0.99.pdf)

## Philosophy

Assembly is often taught as "just push bytes around." But with macros, you can build **abstraction layers** that make complex code readable while still having full control.

This isn't about replacing C. It's about understanding **what C becomes** when compiled, and proving you can build those abstractions yourself.

## Connection to Aria Language

This project is foundational research for the [Aria programming language](https://github.com/alternative-intelligence-cp/aria-lang) - exploring how to combine:

- **Compile-time metaprogramming** (Zig's `comptime` + NASM context stack)
- **Memory safety** (Rust-style borrow checking)
- **Explicit opt-in to unsafe** (TOS keywords - "Throw Off Safety")
- **Zero-cost abstractions** (for both compiler internals and user code)

NASM's context stack demonstrates compile-time scoping mechanisms that can be adapted for Aria's comptime features, while maintaining the performance characteristics needed for systems programming.

## License

Do whatever you want with this. If it helps you learn, that's all that matters.

---

*Built through painful trial and error. Documented so you don't have to.*
