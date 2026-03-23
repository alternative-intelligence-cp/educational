# NASM C-Style Macros - Quick Reference

## Control Flow

### If/Else
```nasm
cmp rax, rbx
if g                    ; Conditions: e, ne, g, ge, l, le, a, ae, b, be, z, nz
    ; then block
else
    ; else block
endi
```

### If/Elif/Else
```nasm
cmp rax, 5
if g
    ; greater than 5
cmp rax, 3              ; New comparison for elif
elif g
    ; greater than 3 but <= 5
else
    ; <= 3
endi
```

### While Loop
```nasm
while
    cmp rcx, 0
    je %$break          ; Exit loop
    ; loop body
    dec rcx
endw
```

### For Loop
```nasm
mov rcx, 10
for
    cmp rcx, 0
    je %$break
    ; loop body
    dec rcx
endf
```

### Switch/Case
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

## Loop Control

**Break** (exit loop):
```nasm
jmp %$break             ; In while/for
break                   ; In switch
```

**Continue** (next iteration):
```nasm
jmp %$start             ; Jump to loop start
jmp %$$start            ; If inside nested if - jump to parent loop
```

## Functions

### Basic Function
```nasm
func my_function
    ; rdi, rsi, rdx, rcx, r8, r9 = arguments (System V)
    ; Return value in rax
endfunc
```

### Function with Preserved Registers
```nasm
func my_function, rbx, r12, r13
    ; rbx, r12, r13 automatically saved/restored
    ; Can clobber them safely
endfunc
```

## Classes/Structures

### Define Structure
```nasm
struc MyClass
    .field1: resq 1     ; 8 bytes
    .field2: resq 1
    .name: resb 32      ; 32 bytes
endstruc
```

### Create Instance
```nasm
section .bss
    instance1: resb MyClass_size
```

### Access Fields
```nasm
mov rax, [instance1 + MyClass.field1]
mov [instance1 + MyClass.field2], rbx
```

### Define Method
```nasm
MyClass_method_name:
    push rbp
    mov rbp, rsp
    ; rdi = 'this' pointer
    mov rax, [rdi + MyClass.field1]
    pop rbp
    ret
```

### Call Method
```nasm
lea rdi, [instance1]    ; Load 'this'
call MyClass_method_name
```

## Context Stack Cheat Sheet

| Symbol | Meaning |
|--------|---------|
| `%push name` | Create new context |
| `%pop` | Destroy current context |
| `%$label` | Current context's label |
| `%$$label` | Parent context's label |
| `%$var` | Current context's variable |
| `%assign %$var value` | Assign numeric value |
| `%define %$var text` | Define text substitution |
| `%xdefine %$var text` | Define with immediate expansion |

## Condition Codes

| Code | Meaning | Flags |
|------|---------|-------|
| `e` | Equal | ZF=1 |
| `ne` | Not Equal | ZF=0 |
| `g` | Greater (signed) | ZF=0 && SF=OF |
| `ge` | Greater or Equal (signed) | SF=OF |
| `l` | Less (signed) | SF≠OF |
| `le` | Less or Equal (signed) | ZF=1 OR SF≠OF |
| `a` | Above (unsigned) | CF=0 && ZF=0 |
| `ae` | Above or Equal (unsigned) | CF=0 |
| `b` | Below (unsigned) | CF=1 |
| `be` | Below or Equal (unsigned) | CF=1 OR ZF=1 |
| `z` | Zero | ZF=1 |
| `nz` | Not Zero | ZF=0 |

## System V x86-64 Calling Convention

**Arguments** (in order):
1. `rdi`
2. `rsi`
3. `rdx`
4. `rcx`
5. `r8`
6. `r9`
7. Stack (right to left)

**Return value**: `rax` (or `rdx:rax` for 128-bit)

**Caller-saved** (can be clobbered):
`rax, rcx, rdx, rsi, rdi, r8, r9, r10, r11`

**Callee-saved** (must be preserved):
`rbx, rsp, rbp, r12, r13, r14, r15`

**Stack alignment**: 16 bytes before `call`

## Common Patterns

### Nested If
```nasm
cmp rax, 10
if g
    cmp rbx, 5
    if l
        ; rax > 10 && rbx < 5
    endi
endi
```

### Loop with Continue
```nasm
mov rcx, 10
while
    cmp rcx, 0
    je %$break
    
    cmp rcx, 5
    if e
        dec rcx
        jmp %$start     ; Skip when rcx == 5
    endi
    
    ; Normal processing
    dec rcx
endw
```

### Class with Multiple Methods
```nasm
struc Counter
    .value: resq 1
    .max: resq 1
endstruc

Counter_increment:
    inc qword [rdi + Counter.value]
    ret

Counter_reset:
    mov qword [rdi + Counter.value], 0
    ret

; Usage:
lea rdi, [my_counter]
call Counter_increment
```

## Debugging Tips

1. **Use `nasm -E` to see macro expansion**:
   ```bash
   nasm -E -f elf64 myfile.asm > expanded.asm
   ```

2. **Check context depth**:
   Add `%warning "Context depth: %$"` to see current context

3. **Label conflicts**: Each context creates unique labels like `..@4.label`

4. **Missing %pop**: Will cause "context stack not empty" warnings

## Examples

See `examples/comprehensive.asm` for a full demonstration of all features working together.

See `tests/` directory for individual feature tests.

---

*Master NASM macros. Build your own language. Understand compilers from the inside out.*
