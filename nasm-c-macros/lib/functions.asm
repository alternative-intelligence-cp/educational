; ============================================================================
; NASM Function Constructor Macros
; Automatic stack frame, register preservation, argument handling
; ============================================================================

; ----------------------------------------------------------------------------
; FUNC / ENDFUNC - Function with automatic prologue/epilogue
;
; Usage:
;   func my_function
;       ; Function body
;       ; Return value in rax
;   endfunc
;
; With saved registers:
;   func my_function, rbx, r12, r13
;       ; rbx, r12, r13 automatically saved/restored
;   endfunc
; ----------------------------------------------------------------------------

%macro func 1-*
    ; %1 = function name, %2+ = registers to preserve
    %push func_ctx
    %xdefine %$func_name %1
    %assign %$num_saved %0 - 1
    
    %1:
        push rbp
        mov rbp, rsp
        
        ; Save specified registers manually
        %if %0 >= 2
            push %2
            %xdefine %$reg1 %2
        %endif
        %if %0 >= 3
            push %3
            %xdefine %$reg2 %3
        %endif
        %if %0 >= 4
            push %4
            %xdefine %$reg3 %4
        %endif
        %if %0 >= 5
            push %5
            %xdefine %$reg4 %5
        %endif
        %if %0 >= 6
            push %6
            %xdefine %$reg5 %6
        %endif
%endmacro

%macro endfunc 0
    ; Restore saved registers in reverse (LIFO) order
    %if %$num_saved >= 5
        pop %$reg5
    %endif
    %if %$num_saved >= 4
        pop %$reg4
    %endif
    %if %$num_saved >= 3
        pop %$reg3
    %endif
    %if %$num_saved >= 2
        pop %$reg2
    %endif
    %if %$num_saved >= 1
        pop %$reg1
    %endif
    
    pop rbp
    ret
    %pop
%endmacro

; ----------------------------------------------------------------------------
; LAMBDA - Create anonymous/uniquely named function
;
; Usage:
;   lambda add_five
;       add rax, 5
;   endlambda
;
; Each invocation creates a unique function name like .lambda_add_five_1, _2, etc
; ----------------------------------------------------------------------------

%assign __lambda_counter 0

%macro lambda 1
    %assign __lambda_counter __lambda_counter + 1
    %push lambda_ctx
    
    ; Define in PARENT context so it's accessible after endlambda
    ..@%[__SECT__].lambda_%1_%[__lambda_counter]:
    %define %%lambda_name ..@%[__SECT__].lambda_%1_%[__lambda_counter]
    
        push rbp
        mov rbp, rsp
%endmacro

%macro endlambda 0
    pop rbp
    ret
    ; Lambda name is still accessible via %%lambda_name
    %pop
%endmacro

; ----------------------------------------------------------------------------
; CALL_LAMBDA - Call a lambda by name (use right after defining it)
; ----------------------------------------------------------------------------

%macro call_lambda 1
    call ..@%[__SECT__].lambda_%1
%endmacro
