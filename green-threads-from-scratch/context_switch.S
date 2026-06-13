.global thread_switch
.type thread_switch, @function

/*
 * void thread_switch(thread_context_t *old_ctx, thread_context_t *new_ctx);
 *
 * System V AMD64 ABI:
 *   rdi = old_ctx (pointer to save current context)
 *   rsi = new_ctx (pointer to load new context)
 *
 * Our thread_context_t layout:
 *   [0x00] rsp
 *   [0x08] rbp
 *   [0x10] rbx
 *   [0x18] r12
 *   [0x20] r13
 *   [0x28] r14
 *   [0x30] r15
 */
thread_switch:
    /* 1. Save the current context into old_ctx (rdi) */
    mov %rsp, 0x00(%rdi)
    mov %rbp, 0x08(%rdi)
    mov %rbx, 0x10(%rdi)
    mov %r12, 0x18(%rdi)
    mov %r13, 0x20(%rdi)
    mov %r14, 0x28(%rdi)
    mov %r15, 0x30(%rdi)

    /* 2. Load the new context from new_ctx (rsi) */
    mov 0x00(%rsi), %rsp
    mov 0x08(%rsi), %rbp
    mov 0x10(%rsi), %rbx
    mov 0x18(%rsi), %r12
    mov 0x20(%rsi), %r13
    mov 0x28(%rsi), %r14
    mov 0x30(%rsi), %r15

    /* 
     * 3. Return to whatever is on top of the newly loaded stack.
     */
    ret

.global thread_startup
.type thread_startup, @function

/*
 * When a new thread is created, we put `thread_startup` at the top of its stack.
 * When `thread_switch` executes `ret` for the very first time on this thread,
 * it will pop `thread_startup` into the Instruction Pointer (RIP).
 *
 * At thread creation, we also stored the user's C function pointer in %r12.
 */
thread_startup:
    /* 
     * The stack must be 16-byte aligned before calling a C function.
     * When `thread_switch` executed `ret`, it popped 8 bytes off the stack.
     * We should ensure alignment if needed, but our stack allocation is 16-byte aligned 
     * and we pushed 1 address (8 bytes), so it was 8-byte aligned, then `ret` popped it,
     * making it 16-byte aligned again. Perfect!
     */
    
    /* Move the function pointer from %r12 to a caller-saved register or just call it */
    mov %r12, %rax
    call *%rax

    /* 
     * When the user function returns, the thread is finished.
     * Call thread_exit() to clean up and switch to another thread.
     */
    call thread_exit
    
    /* We should never reach here */
    ud2

/* Disable executable stack warning */
.section .note.GNU-stack,"",@progbits
