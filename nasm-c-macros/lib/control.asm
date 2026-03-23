; ============================================================================
; NASM C-Style Control Flow Macros
; Using context stack for nested scope management
; ============================================================================

; ----------------------------------------------------------------------------
; IF / ELSE / ELIF / ENDI
; 
; Usage:
;   cmp rax, 5
;   if g           ; if greater (any jump condition: e, ne, g, ge, l, le, etc)
;       ; then block
;   else
;       ; else block  
;   endi
;
; Or with elif:
;   cmp rax, 5
;   if g
;       ; code
;   cmp rax, 3     ; New comparison for elif
;   elif e
;       ; code
;   else
;       ; code
;   endi
; ----------------------------------------------------------------------------

%macro if 1
    %push if_ctx
    %assign %$branch_num 0
    %assign %$has_final_label 0
    ; Jump if condition is FALSE
    %ifidni %1,e
        jne %$branch_0
    %elifidni %1,ne  
        je %$branch_0
    %elifidni %1,g
        jle %$branch_0
    %elifidni %1,ge
        jl %$branch_0
    %elifidni %1,l
        jge %$branch_0
    %elifidni %1,le
        jg %$branch_0
    %elifidni %1,a
        jbe %$branch_0
    %elifidni %1,ae
        jb %$branch_0
    %elifidni %1,b
        jae %$branch_0
    %elifidni %1,be
        ja %$branch_0
    %elifidni %1,z
        jnz %$branch_0
    %elifidni %1,nz
        jz %$branch_0
    %else
        %error "Unknown condition for if: %1"
    %endif
%endmacro

%macro else 0
    jmp %$end_label
  %$branch_%[%$branch_num]:
    %assign %$has_final_label 1
%endmacro

%macro elif 1
    jmp %$end_label
  %$branch_%[%$branch_num]:
    %assign %$branch_num %$branch_num + 1
    ; Check new condition
    %ifidni %1,e
        jne %$branch_%[%$branch_num]
    %elifidni %1,ne
        je %$branch_%[%$branch_num]
    %elifidni %1,g
        jle %$branch_%[%$branch_num]
    %elifidni %1,ge
        jl %$branch_%[%$branch_num]
    %elifidni %1,l
        jge %$branch_%[%$branch_num]
    %elifidni %1,le
        jg %$branch_%[%$branch_num]
    %elifidni %1,a
        jbe %$branch_%[%$branch_num]
    %elifidni %1,ae
        jb %$branch_%[%$branch_num]
    %elifidni %1,b
        jae %$branch_%[%$branch_num]
    %elifidni %1,be
        ja %$branch_%[%$branch_num]
    %elifidni %1,z
        jnz %$branch_%[%$branch_num]
    %elifidni %1,nz
        jz %$branch_%[%$branch_num]
    %else
        %error "Unknown condition for elif: %1"
    %endif
%endmacro

%macro endi 0
    %if %$has_final_label == 0
      %$branch_%[%$branch_num]:
    %endif
  %$end_label:
    %pop
%endmacro

; ----------------------------------------------------------------------------
; WHILE / ENDW
;
; Usage:
;   while
;       ; loop body
;       cmp rcx, 0
;       je break    ; or just: break
;       dec rcx
;   endw
; ----------------------------------------------------------------------------

%macro while 0
    %push while_ctx
  %$start:
%endmacro

%macro endw 0
    jmp %$start
  %$break:
  %$end:
    %pop
%endmacro

; ----------------------------------------------------------------------------
; BREAK / CONTINUE
; These work with while, for, and switch contexts
; ----------------------------------------------------------------------------

%macro break 0
    jmp %$break
%endmacro

%macro continue 0
    jmp %$start
%endmacro

; ----------------------------------------------------------------------------
; FOR / ENDF
;
; Usage:
;   mov rcx, 10      ; init
;   for
;       cmp rcx, 0   ; condition
;       je break     ; exit if done
;       ; loop body
;       dec rcx      ; increment/decrement
;   endf
; ----------------------------------------------------------------------------

%macro for 0
    %push for_ctx
  %$start:
%endmacro

%macro endf 0
    jmp %$start
  %$break:
  %$end:
    %pop
%endmacro

; ----------------------------------------------------------------------------
; SWITCH / CASE / DEFAULT / ENDS
;
; Usage:
;   mov rax, value
;   switch
;       case 1
;           ; code for case 1
;           break
;       case 2
;           ; code for case 2
;           break
;       default
;           ; default code
;   ends
; ----------------------------------------------------------------------------

%macro switch 0
    %push switch_ctx
    %assign %$case_num 0
    %assign %$has_default 0
%endmacro

%macro case 1
    %if %$case_num > 0
        jmp %$break    ; Previous case falls through unless break was called
    %endif
    %assign %$next_case %$case_num + 1
  %$case_%[%$case_num]_check:
    cmp rax, %1
    jne %$case_%[%$next_case]_check
    %assign %$case_num %$next_case
%endmacro

%macro default 0
    %assign %$has_default 1
    jmp %$default_body
  %$case_%[%$case_num]_check:
  %$default_body:
%endmacro

%macro ends 0
    %if %$has_default == 0
      %$case_%[%$case_num]_check:
    %endif
  %$break:
  %$end:
    %pop
%endmacro
