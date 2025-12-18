/**
 * ApexAlloc Bootstrap Allocator
 * 
 * This module handles the "cold start" problem where the dynamic linker
 * needs to allocate memory before our allocator is fully initialized.
 * 
 * Solution: Provide a static buffer and bump-pointer allocator.
 */

#include "apex_internal.h"
#include <stdatomic.h>
#include <string.h>

/* ============================================================================
 * Bootstrap Heap Configuration
 * ============================================================================ */

// Static buffer for early allocations (256KB should be plenty)
#define BOOTSTRAP_HEAP_SIZE (256 * 1024)

static uint8_t g_bootstrap_heap[BOOTSTRAP_HEAP_SIZE] __attribute__((aligned(16)));
static _Atomic size_t g_bootstrap_offset = 0;

// Track what was allocated for debugging
#ifdef DEBUG
static _Atomic size_t g_bootstrap_alloc_count = 0;
#endif

/* ============================================================================
 * Bootstrap Allocator Implementation
 * ============================================================================ */

/**
 * Simple bump-pointer allocator for bootstrap phase.
 * 
 * This is called when:
 * 1. Dynamic linker is loading our library
 * 2. Main allocator is not yet initialized
 * 
 * Returns NULL if bootstrap heap is exhausted.
 */
void* apex_bootstrap_malloc(size_t size) {
    // Align size to 16 bytes (max_align_t)
    size = (size + 15) & ~15;
    
    // Atomic fetch-and-add for thread safety
    size_t offset = atomic_fetch_add_explicit(
        &g_bootstrap_offset, 
        size, 
        memory_order_relaxed
    );
    
    // Check if we overflowed the bootstrap heap
    if (offset + size > BOOTSTRAP_HEAP_SIZE) {
        // This is a fatal error - should never happen in practice
        // The dynamic linker doesn't allocate that much
        return NULL;
    }
    
    void* ptr = &g_bootstrap_heap[offset];
    
#ifdef DEBUG
    atomic_fetch_add(&g_bootstrap_alloc_count, 1);
    apex_debug_log("Bootstrap alloc: %zu bytes at %p (count: %zu)\n", 
                   size, ptr, atomic_load(&g_bootstrap_alloc_count));
#endif
    
    return ptr;
}

/**
 * Bootstrap "free" - does nothing!
 * 
 * Memory allocated from bootstrap heap cannot be freed.
 * This is OK because:
 * 1. Bootstrap allocations are typically dynamic linker internals
 * 2. They persist for the lifetime of the process
 * 3. Total size is small (typically < 10KB)
 */
void apex_bootstrap_free(void* ptr) {
    // Check if this pointer is from bootstrap heap
    if (ptr >= (void*)g_bootstrap_heap && 
        ptr < (void*)(g_bootstrap_heap + BOOTSTRAP_HEAP_SIZE)) {
        // Bootstrap allocations cannot be freed
        // This is intentional and expected
#ifdef DEBUG
        apex_debug_log("Bootstrap free ignored: %p\n", ptr);
#endif
        return;
    }
    
    // If not from bootstrap heap, this is an error
    // Should not happen if initialization is correct
#ifdef DEBUG
    apex_debug_log("ERROR: free() called on non-bootstrap pointer before init: %p\n", ptr);
#endif
}

/**
 * Check if a pointer belongs to the bootstrap heap
 */
bool apex_is_bootstrap_pointer(const void* ptr) {
    return (ptr >= (void*)g_bootstrap_heap && 
            ptr < (void*)(g_bootstrap_heap + BOOTSTRAP_HEAP_SIZE));
}

/**
 * Get bootstrap heap statistics (for debugging/education)
 */
void apex_bootstrap_stats(size_t* used, size_t* total) {
    *used = atomic_load_explicit(&g_bootstrap_offset, memory_order_relaxed);
    *total = BOOTSTRAP_HEAP_SIZE;
}
