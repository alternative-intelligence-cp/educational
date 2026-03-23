; Test switch/case/default/ends macros

%include "../lib/control.asm"

section .data
    msg_one db "Case 1", 10, 0
    len_one equ $ - msg_one - 1
    msg_two db "Case 2", 10, 0
    len_two equ $ - msg_two - 1
    msg_three db "Case 3", 10, 0
    len_three equ $ - msg_three - 1
    msg_default db "Default case", 10, 0
    len_default equ $ - msg_default - 1

section .text
    global _start

_start:
    ; Test 1: switch with value = 2
    mov rax, 2
    
    switch
        case 1
            mov rax, 1
            mov rdi, 1
            mov rsi, msg_one
            mov rdx, len_one
            syscall
            break
        
        case 2
            mov rax, 1
            mov rdi, 1
            mov rsi, msg_two
            mov rdx, len_two
            syscall
            break
        
        case 3
            mov rax, 1
            mov rdi, 1
            mov rsi, msg_three
            mov rdx, len_three
            syscall
            break
        
        default
            mov rax, 1
            mov rdi, 1
            mov rsi, msg_default
            mov rdx, len_default
            syscall
    ends
    
    ; Test 2: switch with value = 99 (should hit default)
    mov rax, 99
    
    switch
        case 1
            mov rax, 1
            mov rdi, 1
            mov rsi, msg_one
            mov rdx, len_one
            syscall
            break
        
        default
            mov rax, 1
            mov rdi, 1
            mov rsi, msg_default
            mov rdx, len_default
            syscall
    ends
    
    ; Exit
    mov rax, 60
    xor rdi, rdi
    syscall
