; ============================================================================
; NASM C-Style Class/Object Macros
; Using context stack for instance management and automatic 'this' injection
; ============================================================================

; Just use NASM's built-in struc for class definitions
; The magic is in the METHOD macros that auto-inject 'this'

; ----------------------------------------------------------------------------
; METHOD / ENDMETHOD - Define a class method with automatic 'this'
;
; Usage:
;   method MyClass, print
;       ; 'this' is automatically saved in the context
;       ; Use this_load to get it back into rdi
;   endmethod
; ----------------------------------------------------------------------------

%macro method 2-*
    ; %1 = class name, %2 = method name, %3+ = optional args
    %push method_ctx
    %xdefine %$class_name %1
    %xdefine %$method_name %2
    
    %1_%2:
        push rbp
        mov rbp, rsp
        ; 'this' pointer is in rdi (System V calling convention)
        ; We'll keep track of it in the context
%endmacro

%macro this_load 0
    ; Reload 'this' into rdi (in case it was clobbered)
    mov rdi, [rbp + 16]  ; Caller's rdi from stack
%endmacro

%macro endmethod 0
    pop rbp
    ret
    %pop
%endmacro
