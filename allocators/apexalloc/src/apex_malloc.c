/**
 * ApexAlloc - Main Entry Point
 * 
 * This file contains the primary malloc/free implementation
 * with bootstrap handling.
 */

#include "apex_internal.h"
#include <stdatomic.h>

/* ============================================================================
 * Global Initialization State
 * ============================================================================ */

_Atomic bool g_apex_initialized = false;
static pthread_once_t g_init_once = PTHREAD_ONCE_INIT;

/* ============================================================================
 * Initialization
 * ============================================================================ */

/**
 * One-time initialization of the allocator.
 * Called automatically on first malloc().
 */
void apex_init_once(void) {
    apex_debug_log("ApexAlloc initialization started\n");
    
    // Initialize global state (entropy, CPU features, etc.)
    apex_global_init();
    
    // TODO: Initialize thread-local state (Phase V)
    // TODO: Initialize global caches (Phase V)
    
    // Mark as initialized
    atomic_store_explicit(&g_apex_initialized, true, memory_order_release);
    
    apex_debug_log("ApexAlloc initialization complete\n");
}

/**
 * Ensure allocator is initialized (thread-safe, happens once)
 */
static ALWAYS_INLINE void ensure_initialized(void) {
    if (unlikely(!atomic_load_explicit(&g_apex_initialized, memory_order_acquire))) {
        pthread_once(&g_init_once, apex_init_once);
    }
}

/* ============================================================================
 * Main Allocation Functions (Bootstrap Phase)
 * ============================================================================ */

/**
 * malloc() implementation
 */
void* malloc(size_t size) {
    // Check if we're initialized
    if (unlikely(!atomic_load_explicit(&g_apex_initialized, memory_order_acquire))) {
        // Not initialized yet - use bootstrap allocator
        return apex_bootstrap_malloc(size);
    }
    
    // Normal allocation path
    return apex_malloc_impl(size);
}

/**
 * free() implementation
 */
void free(void* ptr) {
    if (ptr == NULL) {
        return;  // free(NULL) is a no-op
    }
    
    // Check if this is a bootstrap pointer
    if (apex_is_bootstrap_pointer(ptr)) {
        apex_bootstrap_free(ptr);
        return;
    }
    
    // Check if we're initialized
    if (unlikely(!atomic_load_explicit(&g_apex_initialized, memory_order_acquire))) {
        // Shouldn't happen, but be safe
        return;
    }
    
    // Normal free path
    apex_free_impl(ptr);
}

/**
 * calloc() implementation
 */
void* calloc(size_t nmemb, size_t size) {
    // Check for overflow
    if (nmemb != 0 && size > SIZE_MAX / nmemb) {
        return NULL;
    }
    
    size_t total = nmemb * size;
    void* ptr = malloc(total);
    
    if (ptr) {
        // Zero the memory
        __builtin_memset(ptr, 0, total);
    }
    
    return ptr;
}

/**
 * realloc() implementation
 */
void* realloc(void* ptr, size_t size) {
    if (ptr == NULL) {
        return malloc(size);
    }
    
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    
    // TODO: Optimized realloc (Phase V)
    // For now, simple copy
    void* new_ptr = malloc(size);
    if (new_ptr) {
        // TODO: Get actual old size, for now assume worst case
        size_t old_size = size;  // Placeholder
        __builtin_memcpy(new_ptr, ptr, MIN(size, old_size));
        free(ptr);
    }
    
    return new_ptr;
}

/**
 * posix_memalign() implementation
 */
int posix_memalign(void** memptr, size_t alignment, size_t size) {
    (void)size;  // Unused in bootstrap phase
    
    // Validate alignment
    if (alignment % sizeof(void*) != 0 || 
        (alignment & (alignment - 1)) != 0) {
        return 22;  // EINVAL
    }
    
    ensure_initialized();
    
    // TODO: Aligned allocation (Phase V)
    *memptr = NULL;
    return 12;  // ENOMEM (not implemented yet)
}

/**
 * aligned_alloc() implementation (C11)
 */
void* aligned_alloc(size_t alignment, size_t size) {
    void* ptr = NULL;
    posix_memalign(&ptr, alignment, size);
    return ptr;
}

/**
 * malloc_usable_size() implementation
 */
size_t malloc_usable_size(void* ptr) {
    if (ptr == NULL) {
        return 0;
    }
    
    // Check bootstrap pointer
    if (apex_is_bootstrap_pointer(ptr)) {
        return 0;  // Unknown
    }
    
    // TODO: Return actual usable size (Phase V)
    return 0;  // Placeholder
}
