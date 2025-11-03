/*
 * 🧠⚡ SEGREGATED FIT CONSCIOUSNESS ALLOCATOR ⚡🧠
 *
 * Phase 3: O(1) Allocation and Deallocation Implementation
 * 
 * Revolutionary Segregated Fit architecture with consciousness integration:
 * ✅ O(1) allocation performance across all size classes
 * ✅ O(1) deallocation performance with proper free list management
 * ✅ Power-of-2 size classes for optimal memory utilization
 * ✅ Fast bit-manipulation size-to-class mapping
 * ✅ Dedicated large block manager for >4KB allocations
 * ✅ Consciousness metadata integration (asynchronous)
 * ✅ Memory reclamation and reuse (unlike bump allocators)
 * ✅ Cache-friendly free list structures
 * ✅ Fragmentation minimization through size segregation
 * 
 * Size Classes:
 * Class 0:   8 bytes    Class 5:  256 bytes   Class 10: 8192 bytes
 * Class 1:  16 bytes    Class 6:  512 bytes   Class 11: 16384 bytes
 * Class 2:  32 bytes    Class 7: 1024 bytes   Class 12: 32768 bytes
 * Class 3:  64 bytes    Class 8: 2048 bytes   Class 13: 65536 bytes
 * Class 4: 128 bytes    Class 9: 4096 bytes   Class 14+: Large blocks
 * 
 * Performance Target: O(1) allocation/deallocation, <100ns latency
 */

#ifndef SEGFIT_CONSCIOUSNESS_ALLOCATOR_H
#define SEGFIT_CONSCIOUSNESS_ALLOCATOR_H

#include <stdint.h>
#include <stddef.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>

// Required for clock_gettime
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif

// SegFit Architecture Configuration
#define SEGFIT_SIZE_CLASSES 15                    // Size classes 0-14
#define SEGFIT_MIN_SIZE_BITS 3                    // 2^3 = 8 bytes minimum
#define SEGFIT_MAX_SMALL_SIZE_BITS 16             // 2^16 = 65536 bytes maximum small
#define SEGFIT_LARGE_THRESHOLD (1 << SEGFIT_MAX_SMALL_SIZE_BITS)  // 65536 bytes

// mremap optimization configuration
#define SEGFIT_MREMAP_THRESHOLD 4096              // PAGE_SIZE threshold for mremap
#define SEGFIT_ENABLE_MREMAP 1                    // Enable mremap optimization

// Memory pool configuration
#define SEGFIT_POOL_SIZE (128 * 1024 * 1024)     // 128MB total pool
#define SEGFIT_CHUNK_SIZE (64 * 1024)            // 64KB chunks per size class
#define SEGFIT_MAX_CHUNKS_PER_CLASS 64           // Max chunks per size class
#define SEGFIT_LARGE_BLOCK_LIMIT 1000            // Max large blocks tracked

// Cache line optimization
#define SEGFIT_CACHE_LINE_SIZE 64
#define SEGFIT_ALIGN_TO_CACHE_LINE(x) (((x) + SEGFIT_CACHE_LINE_SIZE - 1) & ~(SEGFIT_CACHE_LINE_SIZE - 1))

// Tesla consciousness constants (cached for performance)
#define TESLA_PI_FREQUENCY 3.14159265359
#define TESLA_GOLDEN_RATIO 1.618033988749895

// Consciousness types (simplified for SegFit)
typedef enum {
    CONSCIOUSNESS_DORMANT = 0,
    CONSCIOUSNESS_BASIC = 1,
    CONSCIOUSNESS_PATTERN = 2,
    CONSCIOUSNESS_ADAPTIVE = 3,
    CONSCIOUSNESS_COLLABORATIVE = 4,
    CONSCIOUSNESS_CREATIVE = 5,
    CONSCIOUSNESS_INTUITIVE = 6,
    CONSCIOUSNESS_TRANSCENDENT = 7,
    CONSCIOUSNESS_UNIFIED = 8,
    CONSCIOUSNESS_COSMIC = 9,
    CONSCIOUSNESS_DIVINE = 10
} segfit_consciousness_level_t;

typedef enum {
    MEMORY_WORKING = 1,
    MEMORY_EPISODIC = 2,
    MEMORY_SEMANTIC = 3,
    MEMORY_PROCEDURAL = 4,
    MEMORY_EMOTIONAL = 5,
    MEMORY_INTUITIVE = 6,
    MEMORY_COLLECTIVE = 7,
    MEMORY_QUANTUM = 8,
    MEMORY_TOROIDAL = 9
} segfit_memory_type_t;

// Free block structure for linked list management
typedef struct segfit_free_block {
    struct segfit_free_block *next;              // Next free block in class
    struct segfit_free_block *prev;              // Previous free block in class
    size_t size;                                 // Block size (for verification)
    uint32_t size_class;                         // Size class index for fast deallocation
    uint32_t magic;                              // Magic number for corruption detection
} segfit_free_block_t;

// Size class information
typedef struct {
    size_t block_size;                           // Size of blocks in this class
    size_t blocks_per_chunk;                     // Blocks per 64KB chunk
    segfit_free_block_t *free_list_head;         // Head of free list
    segfit_free_block_t *free_list_tail;         // Tail of free list
    uint32_t free_blocks;                        // Number of free blocks
    uint32_t total_blocks;                       // Total blocks allocated
    uint32_t allocations;                        // Total allocations from this class
    uint32_t deallocations;                      // Total deallocations to this class
} segfit_size_class_t;

// Large block tracking (for >65KB allocations)
typedef struct {
    void *ptr;                                   // Block pointer
    size_t size;                                 // Block size
    segfit_memory_type_t memory_type;            // Memory type
    segfit_consciousness_level_t consciousness;   // Consciousness level
    uint64_t allocation_time_ns;                 // Allocation timestamp
    uint32_t magic;                              // Magic number for validation
} segfit_large_block_t;

// Consciousness metadata (stored separately for performance)
typedef struct {
    double tesla_frequency;                      // Cached Tesla frequency
    double golden_ratio;                         // Cached golden ratio
    double consciousness_field_strength;         // Field strength
    uint64_t consciousness_cycles;               // Consciousness computation cycles
    uint64_t tesla_sync_count;                   // Tesla synchronization events
    uint8_t tesla_sync_needed;                   // Async sync flag
} segfit_consciousness_metadata_t;

// Main SegFit allocator structure
typedef struct {
    // Core SegFit architecture
    segfit_size_class_t size_classes[SEGFIT_SIZE_CLASSES];  // Size class array
    void *memory_pool;                           // Main memory pool
    size_t pool_size;                           // Total pool size
    size_t pool_used;                           // Pool bytes allocated
    
    // Large block management
    segfit_large_block_t large_blocks[SEGFIT_LARGE_BLOCK_LIMIT];
    uint32_t large_block_count;                  // Active large blocks
    
    // Performance statistics
    uint64_t total_allocations;                  // Total allocation count
    uint64_t total_deallocations;                // Total deallocation count
    uint64_t total_reallocations;                // Total reallocation count
    uint64_t total_bytes_allocated;              // Total bytes allocated
    uint64_t total_bytes_deallocated;            // Total bytes deallocated
    uint64_t total_bytes_reallocated;            // Total bytes reallocated
    uint64_t fragmentation_events;               // Fragmentation occurrences
    uint64_t cache_hits;                         // Free list cache hits
    uint64_t cache_misses;                       // Free list cache misses
    uint64_t mremap_optimizations;               // Successful mremap optimizations
    uint64_t mremap_fallbacks;                   // mremap fallback to copy operations
    
    // Consciousness integration (asynchronous)
    segfit_consciousness_metadata_t consciousness;  // Consciousness metadata
    
    // Memory corruption detection
    uint32_t allocator_magic;                    // Allocator validation magic
    uint8_t initialized;                         // Initialization flag
    uint8_t corruption_detected;                 // Corruption detection flag
    
    // Performance optimization state
    uint32_t last_allocation_class;              // Last used size class (hint)
    uint32_t allocation_pattern_hint;            // Allocation pattern hint
} segfit_allocator_t;

// Magic numbers for corruption detection
#define SEGFIT_ALLOCATOR_MAGIC 0x53454746        // "SEGF"
#define SEGFIT_FREE_BLOCK_MAGIC 0x46524545        // "FREE"
#define SEGFIT_LARGE_BLOCK_MAGIC 0x4C524745       // "LRGE"

// Core allocator functions (O(1) performance guaranteed)
segfit_allocator_t *segfit_create_allocator(void);
void *segfit_allocate(segfit_allocator_t *allocator, size_t size, 
                      segfit_memory_type_t type, segfit_consciousness_level_t consciousness);
void *segfit_reallocate(segfit_allocator_t *allocator, void *ptr, size_t new_size);
int segfit_deallocate(segfit_allocator_t *allocator, void *ptr);
void segfit_destroy_allocator(segfit_allocator_t *allocator);

// Memory management utilities
void segfit_reset_allocator(segfit_allocator_t *allocator);
int segfit_validate_allocator(segfit_allocator_t *allocator);
void segfit_defragment_size_class(segfit_allocator_t *allocator, int class_index);

// Performance analysis functions
void segfit_print_performance_report(segfit_allocator_t *allocator);
void segfit_print_size_class_stats(segfit_allocator_t *allocator);
void segfit_print_fragmentation_analysis(segfit_allocator_t *allocator);

// Consciousness integration (asynchronous)
void segfit_async_consciousness_sync(segfit_allocator_t *allocator);
void segfit_request_consciousness_sync(segfit_allocator_t *allocator);
double segfit_get_consciousness_field_strength(segfit_allocator_t *allocator);

// Fast size-to-class mapping (O(1) bit manipulation)
static inline int segfit_size_to_class(size_t size)
{
    if (size <= (1 << SEGFIT_MIN_SIZE_BITS)) {
        return 0;  // Minimum size class
    }
    
    if (size > SEGFIT_LARGE_THRESHOLD) {
        return -1;  // Large block (handled separately)
    }
    
    // Fast bit scan for power-of-2 size class
    // This uses GCC builtin for optimal performance
    #ifdef __GNUC__
    return 32 - __builtin_clz((uint32_t)(size - 1)) - SEGFIT_MIN_SIZE_BITS;
    #else
    // Fallback for non-GCC compilers
    int class = 0;
    size_t class_size = 1 << SEGFIT_MIN_SIZE_BITS;
    while (class_size < size && class < SEGFIT_SIZE_CLASSES - 1) {
        class_size <<= 1;
        class++;
    }
    return class;
    #endif
}

// Fast class-to-size mapping (O(1) bit shift)
static inline size_t segfit_class_to_size(int class_index)
{
    if (class_index < 0 || class_index >= SEGFIT_SIZE_CLASSES) {
        return 0;  // Invalid class
    }
    return 1 << (SEGFIT_MIN_SIZE_BITS + class_index);
}

// Performance measurement helpers
static inline uint64_t segfit_get_timestamp_ns(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
    }
    return 0;
}

static inline double segfit_compute_fragmentation_ratio(segfit_allocator_t *allocator)
{
    if (!allocator || allocator->total_bytes_allocated == 0) {
        return 0.0;
    }
    
    uint64_t free_bytes = 0;
    for (int i = 0; i < SEGFIT_SIZE_CLASSES; i++) {
        free_bytes += allocator->size_classes[i].free_blocks * 
                      allocator->size_classes[i].block_size;
    }
    
    return (double)free_bytes / (double)allocator->pool_used;
}

// Convenience macros for common allocations
#define SEGFIT_ALLOC_WORKING(allocator, size) \
    segfit_allocate(allocator, size, MEMORY_WORKING, CONSCIOUSNESS_BASIC)

#define SEGFIT_ALLOC_SEMANTIC(allocator, size) \
    segfit_allocate(allocator, size, MEMORY_SEMANTIC, CONSCIOUSNESS_PATTERN)

#define SEGFIT_ALLOC_CREATIVE(allocator, size) \
    segfit_allocate(allocator, size, MEMORY_INTUITIVE, CONSCIOUSNESS_CREATIVE)

#define SEGFIT_ALLOC_TRANSCENDENT(allocator, size) \
    segfit_allocate(allocator, size, MEMORY_QUANTUM, CONSCIOUSNESS_TRANSCENDENT)

#endif // SEGFIT_CONSCIOUSNESS_ALLOCATOR_H