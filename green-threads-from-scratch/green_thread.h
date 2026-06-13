#ifndef GREEN_THREAD_H
#define GREEN_THREAD_H

#include <stddef.h>
#include <stdint.h>

// Standard stack size for each green thread (e.g., 64KB)
#define STACK_SIZE (64 * 1024)

// Maximum number of threads our simple scheduler will handle
#define MAX_THREADS 32

// Thread states
typedef enum {
    THREAD_UNUSED,
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_DEAD
} thread_state_t;

// Context structure to hold the CPU registers we need to save.
// For x86_64 System V ABI, the callee-saved registers are:
// rbx, rbp, r12, r13, r14, r15.
// We also need to save the stack pointer (rsp).
// We don't save rax, rcx, rdx, rsi, rdi, r8-r11 because they are caller-saved
// and the C compiler will save them before calling our yield function.
typedef struct {
    uint64_t rsp;
    uint64_t rbp;
    uint64_t rbx;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
} thread_context_t;

// Thread Control Block (TCB)
typedef struct {
    int id;
    thread_state_t state;
    thread_context_t context;
    void *stack;
} green_thread_t;

// API functions

// Initialize the green threading system (call once at start of main)
void thread_init(void);

// Create a new green thread. Returns the thread ID or -1 on failure.
int thread_create(void (*function)(void));

// Yield the CPU to another ready thread
void thread_yield(void);

// Terminate the current running thread
void thread_exit(void);

// Wait for all threads to finish before exiting the program
void thread_wait_all(void);

#endif // GREEN_THREAD_H
