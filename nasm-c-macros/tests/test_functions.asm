; Test function constructor macros

%include "../lib/control.asm"
%include "../lib/functions.asm"

section .text
    global _start

; Simple function with automatic stack handling
func add_numbers
    ; Arguments in rdi, rsi (System V)
    mov rax, rdi
    add rax, rsi
endfunc

; Function that preserves rbx and r12
func multiply_and_save, rbx, r12
    ; Save some values in preserved registers
    mov rbx, rdi
    mov r12, rsi
    
    ; Do multiplication
    mov rax, rbx
    imul rax, r12
endfunc

_start:
    ; Test add_numbers(7, 3) = 10
    mov rdi, 7
    mov rsi, 3
    call add_numbers
    ; rax = 10
    
    ; Test multiply_and_save(4, 5) = 20
    mov rdi, 4
    mov rsi, 5
    call multiply_and_save
    ; rax = 20
    
    ; Exit with result (should be 20)
    mov rdi, rax
    mov rax, 60
    syscall
