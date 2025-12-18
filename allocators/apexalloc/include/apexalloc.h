/**
 * ApexAlloc - Educational High-Performance Memory Allocator
 * Public API Header
 */

#ifndef APEXALLOC_H
#define APEXALLOC_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Standard Memory Allocation API
 * ============================================================================
 * These functions replace the standard libc malloc family.
 */

/**
 * Allocate memory of at least `size` bytes.
 * Returns NULL if allocation fails.
 */
void* malloc(size_t size) __attribute__((malloc, alloc_size(1)));

/**
 * Free memory allocated by malloc/calloc/realloc.
 * Calling free(NULL) is safe and does nothing.
 */
void free(void* ptr);

/**
 * Allocate zeroed memory for `nmemb` elements of `size` bytes each.
 */
void* calloc(size_t nmemb, size_t size) 
    __attribute__((malloc, alloc_size(1, 2)));

/**
 * Resize allocation. Returns NULL on failure (original allocation unchanged).
 */
void* realloc(void* ptr, size_t size) 
    __attribute__((alloc_size(2)));

/**
 * Allocate aligned memory.
 * `alignment` must be a power of 2 and a multiple of sizeof(void*).
 */
int posix_memalign(void** memptr, size_t alignment, size_t size);

/**
 * Allocate aligned memory (C11).
 */
void* aligned_alloc(size_t alignment, size_t size)
    __attribute__((malloc, alloc_size(2), alloc_align(1)));

/**
 * Get the usable size of an allocation.
 * This may be larger than the requested size due to rounding.
 */
size_t malloc_usable_size(void* ptr);

/* ============================================================================
 * ApexAlloc Extended API
 * ============================================================================
 * Educational introspection and control interface.
 */

/**
 * Introspection and control (inspired by jemalloc's mallctl)
 * 
 * Usage:
 *   size_t allocated;
 *   size_t len = sizeof(allocated);
 *   apex_ctl("stats.allocated", &allocated, &len, NULL, 0);
 * 
 * Common operations:
 *   "stats.allocated"   - Total bytes allocated
 *   "stats.active"      - Active allocations
 *   "stats.resident"    - Resident set size
 *   "arena.purge"       - Force memory return to OS
 *   "thread.reset"      - Reset thread-local cache
 */
int apex_ctl(const char* operation, 
             void* oldp, size_t* oldlenp,
             void* newp, size_t newlen);

/**
 * Per-thread statistics structure
 */
typedef struct {
    uint64_t malloc_count;        // Total mallocs
    uint64_t free_count;          // Total frees
    uint64_t bytes_allocated;     // Total bytes allocated
    uint64_t bytes_freed;         // Total bytes freed
    uint64_t remote_frees;        // Frees from other threads
    uint64_t cache_hits;          // Fast path hits
    uint64_t cache_misses;        // Slow path misses
} apex_thread_stats_t;

/**
 * Get statistics for the current thread
 */
void apex_thread_stats(apex_thread_stats_t* stats);

/**
 * Global statistics structure
 */
typedef struct {
    uint64_t superblocks_allocated;
    uint64_t superblocks_active;
    uint64_t blocks_allocated;
    uint64_t blocks_active;
    uint64_t total_allocated_bytes;
    uint64_t total_freed_bytes;
    uint64_t peak_allocated_bytes;
    uint64_t fragmentation_percent;  // Internal fragmentation (0-100)
} apex_global_stats_t;

/**
 * Get global allocator statistics
 */
void apex_global_stats(apex_global_stats_t* stats);

/**
 * Heap visualization callback
 * Called for each active block with allocation information
 */
typedef void (*apex_heap_visitor_t)(
    void* block_addr,
    size_t block_size,
    size_t size_class,
    size_t used_chunks,
    size_t total_chunks,
    void* userdata
);

/**
 * Walk the heap and call visitor for each block
 * Useful for visualization and debugging
 */
void apex_heap_walk(apex_heap_visitor_t visitor, void* userdata);

#ifdef __cplusplus
}
#endif

#endif /* APEXALLOC_H */
