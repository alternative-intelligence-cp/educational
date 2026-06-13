#include "green_thread.h"
#include <stdlib.h>
#include <stdio.h>

// External assembly function
extern void thread_switch(thread_context_t *old_ctx, thread_context_t *new_ctx);

// Thread storage
static green_thread_t threads[MAX_THREADS];
static int current_thread = 0;
static int thread_count = 0;

// Internal function to find the next ready thread
static int get_next_thread() {
    int next = current_thread;
    for (int i = 0; i < MAX_THREADS; i++) {
        next = (next + 1) % MAX_THREADS;
        if (threads[next].state == THREAD_READY) {
            return next;
        }
    }
    return -1; // No other ready thread
}



void thread_init(void) {
    // Initialize the main thread (thread 0)
    threads[0].id = 0;
    threads[0].state = THREAD_RUNNING;
    threads[0].stack = NULL; // Main thread uses the OS stack
    current_thread = 0;
    thread_count = 1;
    
    for (int i = 1; i < MAX_THREADS; i++) {
        threads[i].state = THREAD_UNUSED;
    }
}

int thread_create(void (*function)(void)) {
    if (thread_count >= MAX_THREADS) return -1;

    // Find an unused slot
    int id = -1;
    for (int i = 1; i < MAX_THREADS; i++) {
        if (threads[i].state == THREAD_UNUSED || threads[i].state == THREAD_DEAD) {
            id = i;
            break;
        }
    }
    
    if (id == -1) return -1;

    // Allocate stack memory
    threads[id].stack = malloc(STACK_SIZE);
    if (!threads[id].stack) return -1;

    // Setup the stack
    // x86_64 stacks grow downwards, so we start at the top of the allocated memory.
    // The ABI requires the stack pointer to be 16-byte aligned before a call instruction,
    // which pushes an 8-byte return address.
    uint64_t *stack_top = (uint64_t *)((uint8_t *)threads[id].stack + STACK_SIZE);
    
    // We want to simulate a function call to thread_switch, so when thread_switch
    // returns, it "returns" to our wrapper function.
    
    // Push the argument for thread_start_wrapper (the function pointer)
    // Actually, in x86_64, the first argument goes in rdi.
    // We can't easily set rdi purely from the stack.
    // BUT wait: we can just push a small assembly trampoline or set it up cleverly.
    // A simpler way without a trampoline: 
    // The newly created thread has its own context. We can set up a "dummy" function
    // that just pops the function pointer into rdi and calls it.
    // Even easier: We can write a tiny assembly wrapper or handle it directly in C 
    // if we control the context fully.
    
    // For educational simplicity, since thread_start_wrapper needs the function pointer,
    // we could just store it in a global or thread-local variable, BUT that's unsafe.
    // Instead, let's look at `thread_context_t`. It only saves callee-saved registers.
    // r12 is a callee-saved register. We can store the function pointer in r12.
    // Then we need an assembly trampoline that moves r12 to rdi and calls thread_start_wrapper.
    
    // Let's implement that! We'll add the trampoline to context_switch.S later,
    // or just write a pure assembly start function.
    
    // For now, let's use a pure assembly start function `thread_startup`
    // that assumes r12 holds the user function pointer.
    extern void thread_startup(void);
    
    // Push the startup function address onto the stack.
    // When thread_switch does `ret`, it will jump to thread_startup.
    *(--stack_top) = (uint64_t)thread_startup;

    // Setup context
    threads[id].context.rsp = (uint64_t)stack_top;
    threads[id].context.rbp = 0;
    threads[id].context.rbx = 0;
    threads[id].context.r12 = (uint64_t)function; // Pass function pointer in r12
    threads[id].context.r13 = 0;
    threads[id].context.r14 = 0;
    threads[id].context.r15 = 0;

    threads[id].id = id;
    threads[id].state = THREAD_READY;
    thread_count++;

    return id;
}

void thread_yield(void) {
    int next = get_next_thread();
    if (next == -1) {
        // No other thread to run, just return
        return;
    }

    int old_thread = current_thread;
    
    if (threads[old_thread].state == THREAD_RUNNING) {
        threads[old_thread].state = THREAD_READY;
    }
    
    threads[next].state = THREAD_RUNNING;
    current_thread = next;

    // printf("Switching from thread %d to thread %d\n", old_thread, next);
    thread_switch(&threads[old_thread].context, &threads[next].context);
}

void thread_exit(void) {
    threads[current_thread].state = THREAD_DEAD;
    if (threads[current_thread].stack) {
        free(threads[current_thread].stack);
        threads[current_thread].stack = NULL;
    }
    thread_count--;

    int next = get_next_thread();
    if (next == -1) {
        // All threads finished
        exit(0);
    }

    threads[next].state = THREAD_RUNNING;
    int old_thread = current_thread;
    current_thread = next;

    // We don't care about saving the current context since it's dead,
    // but thread_switch requires two pointers. We can just pass the dead thread's context anyway.
    thread_switch(&threads[old_thread].context, &threads[next].context);
}

void thread_wait_all(void) {
    while (thread_count > 1) {
        thread_yield();
    }
}
