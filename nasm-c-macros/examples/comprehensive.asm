; =============================================================================
; NASM C-Style Macros - Comprehensive Example
; Demonstrates all control flow constructs working together
; =============================================================================

%include "../lib/control.asm"
%include "../lib/classes.asm"
%include "../lib/functions.asm"

; =============================================================================
; Data Structures
; =============================================================================

struc Counter
    .value: resq 1
    .max: resq 1
endstruc

section .data
    msg_fizz db "Fizz", 10, 0
    len_fizz equ $ - msg_fizz - 1
    msg_buzz db "Buzz", 10, 0
    len_buzz equ $ - msg_buzz - 1
    msg_fizzbuzz db "FizzBuzz", 10, 0
    len_fizzbuzz equ $ - msg_fizzbuzz - 1
    msg_number db "Number", 10, 0
    len_number equ $ - msg_number - 1
    newline db 10

section .bss
    counter1: resb Counter_size

section .text
    global _start

; =============================================================================
; Functions
; =============================================================================

; Print FizzBuzz for numbers 1-15
; Demonstrates: switch, case, if, while, break
func fizzbuzz
    mov rcx, 1          ; Start at 1
    
    while
        cmp rcx, 15
        jg %$break      ; Stop after 15
        
        ; Check divisibility
        mov rax, rcx
        xor rdx, rdx
        mov rbx, 15
        div rbx
        
        cmp rdx, 0
        if e
            ; Divisible by 15
            mov rax, 1
            mov rdi, 1
            mov rsi, msg_fizzbuzz
            mov rdx, len_fizzbuzz
            syscall
        else
            ; Check mod 3 and mod 5
            mov rax, rcx
            xor rdx, rdx
            mov rbx, 3
            div rbx
            
            cmp rdx, 0
            if e
                ; Divisible by 3
                mov rax, 1
                mov rdi, 1
                mov rsi, msg_fizz
                mov rdx, len_fizz
                syscall
            else
                ; Check mod 5
                mov rax, rcx
                xor rdx, rdx
                mov rbx, 5
                div rbx
                
                cmp rdx, 0
                if e
                    ; Divisible by 5
                    mov rax, 1
                    mov rdi, 1
                    mov rsi, msg_buzz
                    mov rdx, len_buzz
                    syscall
                else
                    ; Regular number
                    mov rax, 1
                    mov rdi, 1
                    mov rsi, msg_number
                    mov rdx, len_number
                    syscall
                endi
            endi
        endi
        
        inc rcx
    endw
endfunc

; Nested loop example
; Demonstrates: for, nested loops, break, continue
func nested_loops
    mov rbx, 3          ; Outer loop counter
    
    for
        cmp rbx, 0
        je %$break
        
        ; Inner loop
        mov rcx, 5
        for
            cmp rcx, 0
            je %$break
            
            ; Skip if rcx == 3
            cmp rcx, 3
            if e
                dec rcx
                jmp %$$start ; continue - jump to parent (for) loop start
            endi
            
            ; Do work here
            dec rcx
        endf
        
        dec rbx
    endf
endfunc

; Switch statement example
; Demonstrates: switch, case, default, break
func test_switch
    mov rax, 2
    
    switch
        case 1
            mov rax, 1
            mov rdi, 1
            mov rsi, msg_fizz
            mov rdx, len_fizz
            syscall
            break
        
        case 2
            mov rax, 1
            mov rdi, 1
            mov rsi, msg_buzz
            mov rdx, len_buzz
            syscall
            break
        
        case 3
            mov rax, 1
            mov rdi, 1
            mov rsi, msg_fizzbuzz
            mov rdx, len_fizzbuzz
            syscall
            break
        
        default
            mov rax, 1
            mov rdi, 1
            mov rsi, msg_number
            mov rdx, len_number
            syscall
    ends
endfunc

; Class method example
Counter_increment:
    push rbp
    mov rbp, rsp
    
    ; Check if at max
    mov rax, [rdi + Counter.value]
    mov rbx, [rdi + Counter.max]
    
    cmp rax, rbx
    if ge
        ; At max, reset to 0
        mov qword [rdi + Counter.value], 0
    else
        ; Increment
        inc qword [rdi + Counter.value]
    endi
    
    pop rbp
    ret

Counter_get:
    push rbp
    mov rbp, rsp
    mov rax, [rdi + Counter.value]
    pop rbp
    ret

; =============================================================================
; Main Program
; =============================================================================

_start:
    ; Test 1: FizzBuzz with nested if/else
    call fizzbuzz
    
    ; Test 2: Nested loops with continue
    call nested_loops
    
    ; Test 3: Switch statement
    call test_switch
    
    ; Test 4: Class methods
    lea rdi, [rel counter1]
    mov qword [rdi + Counter.value], 0
    mov qword [rdi + Counter.max], 10
    
    ; Increment counter 3 times
    mov rcx, 3
    for
        cmp rcx, 0
        je %$break
        
        lea rdi, [rel counter1]
        call Counter_increment
        
        dec rcx
    endf
    
    ; Get final value
    lea rdi, [rel counter1]
    call Counter_get
    ; rax now has counter value (3)
    
    ; Exit with counter value
    mov rdi, rax
    mov rax, 60
    syscall
