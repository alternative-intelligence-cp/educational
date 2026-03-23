; Test class/method macros - Point class example

%include "../lib/control.asm"
%include "../lib/classes.asm"

; Define Point class structure
struc Point
    .x: resq 1
    .y: resq 1
endstruc

section .data
    newline db 10, 0

section .bss
    point1: resb Point_size
    point2: resb Point_size

section .text
    global _start

; Point::set(x, y) - Set coordinates
; rdi = this, rsi = x, rdx = y
Point_set:
    push rbp
    mov rbp, rsp
    mov [rdi + Point.x], rsi
    mov [rdi + Point.y], rdx
    pop rbp
    ret

; Point::get_x() - Get x coordinate
; rdi = this, returns in rax
Point_get_x:
    push rbp
    mov rbp, rsp
    mov rax, [rdi + Point.x]
    pop rbp
    ret

; Point::get_y() - Get y coordinate
; rdi = this, returns in rax
Point_get_y:
    push rbp
    mov rbp, rsp
    mov rax, [rdi + Point.y]
    pop rbp
    ret

; Helper: print number in rax
print_num:
    push rbp
    mov rbp, rsp
    push rax
    
    ; Convert to ASCII (simple: just add '0' for single digits)
    add al, '0'
    
    ; Print digit
    mov byte [rel newline], al
    mov rax, 1
    mov rdi, 1
    lea rsi, [rel newline]
    mov rdx, 1
    syscall
    
    ; Print newline
    mov byte [rel newline], 10
    mov rax, 1
    mov rdi, 1
    lea rsi, [rel newline]
    mov rdx, 1
    syscall
    
    pop rax
    pop rbp
    ret

_start:
    ; Test point1.set(5, 7)
    lea rdi, [rel point1]
    mov rsi, 5
    mov rdx, 7
    call Point_set
    
    ; Test point1.get_x()
    lea rdi, [rel point1]
    call Point_get_x
    call print_num
    
    ; Test point1.get_y()
    lea rdi, [rel point1]
    call Point_get_y
    call print_num
    
    ; Test point2.set(3, 9)
    lea rdi, [rel point2]
    mov rsi, 3
    mov rdx, 9
    call Point_set
    
    ; Test point2.get_x()
    lea rdi, [rel point2]
    call Point_get_x
    call print_num
    
    ; Exit
    mov rax, 60
    xor rdi, rdi
    syscall
