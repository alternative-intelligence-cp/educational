; Test if/else/elif/endi macros

%include "../lib/control.asm"

section .data
    msg_greater db "Greater than 5", 10, 0
    msg_equal db "Equal to 5", 10, 0
    msg_less db "Less than 5", 10, 0
    len_greater equ $ - msg_greater - 1
    len_equal equ msg_equal - msg_greater - 1  
    len_less equ msg_less - msg_equal - 1

section .text
    global _start

_start:
    ; Test 1: if/else with value > 5
    mov rax, 10
    cmp rax, 5
    if g
        ; Print "Greater than 5"
        mov rax, 1          ; sys_write
        mov rdi, 1          ; stdout
        mov rsi, msg_greater
        mov rdx, len_greater
        syscall
    else
        ; This shouldn't execute
        mov rax, 1
        mov rdi, 1
        mov rsi, msg_less
        mov rdx, len_less
        syscall
    endi

    ; Test 2: if/elif/else with value == 5
    mov rax, 5
    cmp rax, 5
    if g
        mov rax, 1
        mov rdi, 1
        mov rsi, msg_greater
        mov rdx, len_greater
        syscall
    elif e
        cmp rax, 5  ; Need comparison before elif
        ; Print "Equal to 5"
        mov rax, 1
        mov rdi, 1
        mov rsi, msg_equal
        mov rdx, len_equal
        syscall
    else
        mov rax, 1
        mov rdi, 1
        mov rsi, msg_less
        mov rdx, len_less
        syscall
    endi

    ; Test 3: nested if
    mov rax, 3
    cmp rax, 5
    if l
        ; Inner if
        cmp rax, 2
        if g
            ; Print "Less than 5"
            mov rax, 1
            mov rdi, 1
            mov rsi, msg_less
            mov rdx, len_less
            syscall
        endi
    endi

    ; Exit
    mov rax, 60         ; sys_exit
    xor rdi, rdi        ; exit code 0
    syscall
