; Test while/endw macros

%include "../lib/control.asm"

section .data
    msg db "Loop iteration", 10, 0
    len equ $ - msg - 1

section .text
    global _start

_start:
    ; Simple while loop - count down from 5
    mov rcx, 5
    
    while
        ; Check condition
        cmp rcx, 0
        je %$end      ; break out if rcx == 0
        
        ; Print message
        mov rax, 1
        mov rdi, 1
        mov rsi, msg
        mov rdx, len
        syscall
        
        ; Decrement counter
        dec rcx
    endw
    
    ; Nested while test
    mov rbx, 2
    while
        cmp rbx, 0
        je %$end
        
        mov rcx, 3
        while
            cmp rcx, 0
            je %$end
            
            ; Inner loop work
            dec rcx
        endw
        
        dec rbx
    endw
    
    ; Exit
    mov rax, 60
    xor rdi, rdi
    syscall
