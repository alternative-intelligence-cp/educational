;
; 🖨️ Randy's Custom Printf - Pure Assembly Implementation  
; 
; Educational printf in x86_64 assembly - shows what the compiler generates!
; Demonstrates low-level system programming and register usage
;

section .data
    ; 📊 Output strings and formatting data
    newline         db 10, 0
    minus_sign      db '-', 0
    hex_prefix      db '0x', 0
    bin_prefix      db '0b', 0
    null_str        db '(null)', 0
    
    ; Educational messages
    title_msg       db '🖨️ Assembly Printf - Pure x86_64 Implementation', 10, 0
    separator       db '================================================', 10, 0
    demo_int_msg    db 10, '🔢 INTEGER CONVERSIONS:', 10, 0
    demo_str_msg    db 10, '📝 STRING OPERATIONS:', 10, 0
    demo_hex_msg    db 10, '🔍 HEXADECIMAL:', 10, 0
    demo_end_msg    db 10, '✅ Assembly printf demonstrates:', 10
                   db '  • Direct register manipulation', 10
                   db '  • Raw syscall interface', 10
                   db '  • Manual memory management', 10
                   db '  • CPU instruction optimization', 10
                   db '🏆 This is what your C compiler generates!', 10, 0

    ; Lookup table for hexadecimal digits
    hex_chars       db '0123456789abcdef'

section .bss
    ; 💾 Buffers for number conversion
    num_buffer      resb 32     ; Buffer for number-to-string conversion
    
section .text
    global _start

; 🔧 System call wrapper - direct kernel interface
sys_write:
    ; Input: rdi = fd, rsi = buffer, rdx = count
    ; Output: rax = bytes written
    mov rax, 1          ; sys_write
    syscall
    ret

; 📏 Calculate string length
strlen:
    ; Input: rdi = string pointer
    ; Output: rax = length
    xor rax, rax        ; length = 0
.loop:
    cmp byte [rdi + rax], 0
    je .done
    inc rax
    jmp .loop
.done:
    ret

; 🖨️ Print string
print_string:
    ; Input: rdi = string pointer
    push rdi
    call strlen         ; Get string length
    mov rdx, rax        ; length in rdx
    pop rsi             ; string in rsi
    mov rdi, 1          ; stdout
    call sys_write
    ret

; 🖨️ Print single character  
print_char:
    ; Input: dil = character
    push rdi
    mov rsi, rsp        ; Character is on stack
    mov rdi, 1          ; stdout
    mov rdx, 1          ; 1 byte
    call sys_write
    pop rdi
    ret

; 🔢 Convert integer to string (the core algorithm!)
int_to_string:
    ; Input: rdi = number, rsi = buffer
    ; Output: rax = string length
    
    ; Handle negative numbers
    test rdi, rdi
    jns .positive
    
    ; Negative: store minus sign and negate
    mov byte [rsi], '-'
    inc rsi
    neg rdi
    
.positive:
    ; Handle zero special case
    test rdi, rdi
    jnz .convert
    mov byte [rsi], '0'
    inc rsi
    mov byte [rsi], 0
    mov rax, 1
    ret
    
.convert:
    mov rax, rdi        ; Number to convert
    mov rcx, rsi        ; Start of buffer
    
    ; Convert digits (stored in reverse order)
.digit_loop:
    xor rdx, rdx        ; Clear remainder
    mov rbx, 10
    div rbx             ; rax = quotient, rdx = remainder
    
    add dl, '0'         ; Convert digit to ASCII
    mov [rsi], dl       ; Store digit
    inc rsi
    
    test rax, rax       ; More digits?
    jnz .digit_loop
    
    ; Reverse the digits
    dec rsi             ; Point to last digit
    mov rdi, rcx        ; Point to first digit
    
.reverse_loop:
    cmp rdi, rsi
    jge .reverse_done
    
    ; Swap characters
    mov al, [rdi]
    mov bl, [rsi]
    mov [rdi], bl
    mov [rsi], al
    
    inc rdi
    dec rsi
    jmp .reverse_loop
    
.reverse_done:
    mov byte [rcx + 32], 0  ; Null terminate (approximate)
    ret

; 🔢 Print integer
print_int:
    ; Input: rdi = integer
    push rdi
    mov rsi, num_buffer
    call int_to_string
    mov rdi, num_buffer
    call print_string
    pop rdi
    ret

; 🔍 Convert number to hexadecimal
int_to_hex:
    ; Input: rdi = number, rsi = buffer
    ; Output: hex string in buffer
    
    ; Print "0x" prefix
    mov byte [rsi], '0'
    mov byte [rsi + 1], 'x'
    add rsi, 2
    
    ; Handle zero case
    test rdi, rdi
    jnz .convert_hex
    mov byte [rsi], '0'
    inc rsi
    mov byte [rsi], 0
    ret
    
.convert_hex:
    mov rax, rdi
    mov rcx, rsi        ; Buffer start
    
.hex_loop:
    mov rdx, rax
    and rdx, 0xF        ; Get lowest 4 bits
    
    ; Convert to hex character
    mov bl, [hex_chars + rdx]
    mov [rsi], bl
    inc rsi
    
    shr rax, 4          ; Shift right 4 bits
    test rax, rax
    jnz .hex_loop
    
    ; Reverse the hex digits
    dec rsi
    mov rdi, rcx
    
.hex_reverse:
    cmp rdi, rsi
    jge .hex_done
    
    mov al, [rdi]
    mov bl, [rsi]
    mov [rdi], bl
    mov [rsi], al
    
    inc rdi
    dec rsi
    jmp .hex_reverse
    
.hex_done:
    mov byte [rcx + 16], 0  ; Null terminate
    ret

; 🔍 Print hexadecimal
print_hex:
    ; Input: rdi = number
    push rdi
    mov rsi, num_buffer
    call int_to_hex
    mov rdi, num_buffer
    call print_string
    pop rdi
    ret

; 🎯 Main program - demonstration
_start:
    ; Print title
    mov rdi, title_msg
    call print_string
    
    mov rdi, separator
    call print_string
    
    ; 🔢 INTEGER DEMO
    mov rdi, demo_int_msg
    call print_string
    
    ; Print some integers
    mov rdi, 42
    call print_int
    mov dil, 10         ; newline
    call print_char
    
    mov rdi, -17
    call print_int
    mov dil, 10
    call print_char
    
    mov rdi, 0
    call print_int
    mov dil, 10
    call print_char
    
    ; 🔍 HEXADECIMAL DEMO
    mov rdi, demo_hex_msg
    call print_string
    
    mov rdi, 255
    call print_hex
    mov dil, 10
    call print_char
    
    mov rdi, 4095
    call print_hex
    mov dil, 10
    call print_char
    
    ; 📝 STRING DEMO
    mov rdi, demo_str_msg
    call print_string
    
    mov rdi, null_str
    call print_string
    mov dil, 10
    call print_char
    
    ; Educational summary
    mov rdi, demo_end_msg
    call print_string
    
    ; Exit program
    mov rax, 60         ; sys_exit
    mov rdi, 0          ; exit status
    syscall