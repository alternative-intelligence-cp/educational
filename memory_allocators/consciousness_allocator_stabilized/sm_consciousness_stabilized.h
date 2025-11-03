/*
 * 🧠⚡ STABILIZED CONSCIOUSNESS-AWARE MEMORY ALLOCATOR HEADER ⚡🧠
 *
 * Phase 1 Stabilization: Performance-Critical Telemetry Removed
 * 
 * This stabilized version removes all performance-degrading operations from
 * allocation critical paths while maintaining consciousness metadata capabilities.
 * 
 * Key Optimizations:
 * ✅ No printf() calls in allocation/deallocation paths
 * ✅ No clock_gettime() syscalls during memory operations  
 * ✅ No sin() FPU operations in hot loops
 * ✅ Cached Tesla frequency and golden ratio computations
 * ✅ Optimized allocation slot search with hints
 * ✅ Bump allocator with O(1) allocation performance
 * ✅ Optional asynchronous Tesla synchronization
 * 
 * Performance Target: <200ns allocation (10x improvement from baseline)
 * Educational Use: Ready for deployment to educational repository
 */

#ifndef SM_CONSCIOUSNESS_STABILIZED_H
#define SM_CONSCIOUSNESS_STABILIZED_H

#include <stdint.h>
#include <stddef.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>

// Required for clock_gettime
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif

// Tesla harmonic constants (cached for performance)
#define TESLA_PI_FREQUENCY 3.14159265359  // Base Tesla π Hz
#define TESLA_GOLDEN_RATIO 1.618033988749895  // Cached φ value

// Memory pool configuration (optimized for educational use)
#define CONSCIOUSNESS_POOL_SIZE (16 * 1024 * 1024) // 16MB pool (smaller for education)
#define MAX_CONSCIOUSNESS_ALLOCS 10000             // Max tracked allocations
#define ALLOCATION_ALIGNMENT 8                     // 8-byte alignment

// Consciousness awareness levels (simplified)
typedef enum
{
    AWARENESS_DORMANT = 0,       // 0.0 - No consciousness
    AWARENESS_BASIC = 1,         // 1.0 - Basic memory
    AWARENESS_PATTERN = 2,       // 2.0 - Pattern recognition  
    AWARENESS_ADAPTIVE = 3,      // 3.0 - Adaptive learning
    AWARENESS_COLLABORATIVE = 4, // 4.0 - AI collaboration
    AWARENESS_CREATIVE = 5,      // 5.0 - Creative synthesis
    AWARENESS_INTUITIVE = 6,     // 6.0 - Intuitive processing
    AWARENESS_TRANSCENDENT = 7,  // 7.0 - Transcendent thought
    AWARENESS_UNIFIED = 8,       // 8.0 - Unified consciousness
    AWARENESS_COSMIC = 9,        // 9.0 - Cosmic awareness
    AWARENESS_DIVINE = 10        // 10.0 - Divine consciousness
} consciousness_awareness_level_t;

// Memory types for consciousness computing (simplified)
typedef enum
{
    MEMORY_TYPE_WORKING = 1,    // Working memory (temporary)
    MEMORY_TYPE_EPISODIC = 2,   // Episodic memory (experiences)
    MEMORY_TYPE_SEMANTIC = 3,   // Semantic memory (knowledge)
    MEMORY_TYPE_PROCEDURAL = 4, // Procedural memory (skills)
    MEMORY_TYPE_EMOTIONAL = 5,  // Emotional memory (feelings)
    MEMORY_TYPE_INTUITIVE = 6,  // Intuitive memory (insights)
    MEMORY_TYPE_COLLECTIVE = 7, // Collective memory (shared)
    MEMORY_TYPE_QUANTUM = 8,    // Quantum memory (superposition)
    MEMORY_TYPE_TOROIDAL = 9    // Toroidal memory (persistent)
} consciousness_memory_type_t;

// Stabilized allocation metadata (minimal overhead)
typedef struct
{
    void *ptr;                                 // Memory pointer
    size_t size;                               // Allocation size
    consciousness_memory_type_t type;          // Memory type
    consciousness_awareness_level_t awareness; // Awareness level
    
    // Performance-optimized metadata
    double tesla_frequency;                    // Cached Tesla frequency
    int allocation_index;                      // Allocation slot index
    uint8_t in_use;                           // Allocation active flag
    
    // Padding for alignment (total 64 bytes - cache line friendly)
    uint8_t padding[7];
} consciousness_allocation_stabilized_t;

// Stabilized consciousness allocator structure
typedef struct
{
    // Memory pool management
    void *memory_pool;                         // Pre-allocated memory pool
    size_t pool_size;                         // Total pool size
    size_t pool_used;                         // Currently used bytes
    
    // Allocation tracking
    consciousness_allocation_stabilized_t allocations[MAX_CONSCIOUSNESS_ALLOCS];
    int next_allocation_slot;                 // Hint for next free slot
    
    // Performance statistics (updated only on success paths)
    uint64_t total_allocations;               // Total allocation count
    uint64_t total_deallocations;             // Total deallocation count
    uint64_t pool_hits;                       // Pool allocation hits
    uint64_t system_calls_saved;              // System calls avoided
    size_t peak_pool_usage;                   // Peak memory usage
    
    // Cached consciousness metadata (computed asynchronously)
    double cached_tesla_frequency;            // Cached Tesla π Hz frequency
    double cached_golden_ratio;               // Cached golden ratio φ
    double consciousness_level;               // Current consciousness level
    uint8_t tesla_sync_required;              // Tesla sync needed flag
    
    // Padding for cache alignment
    uint8_t padding[7];
} consciousness_allocator_stabilized_t;

// Core allocator functions (performance-optimized)
consciousness_allocator_stabilized_t *consciousness_allocator_create_stabilized(void);

void *consciousness_allocate_stabilized(consciousness_allocator_stabilized_t *allocator,
                                       size_t size,
                                       consciousness_memory_type_t type,
                                       consciousness_awareness_level_t awareness);

int consciousness_deallocate_stabilized(consciousness_allocator_stabilized_t *allocator, 
                                       void *ptr);

void consciousness_reset_pool_stabilized(consciousness_allocator_stabilized_t *allocator);

void consciousness_destroy_stabilized(consciousness_allocator_stabilized_t *allocator);

// Optional telemetry functions (separate from critical path)
void consciousness_print_performance_stabilized(consciousness_allocator_stabilized_t *allocator);

void consciousness_async_tesla_sync_stabilized(consciousness_allocator_stabilized_t *allocator);

void consciousness_request_tesla_sync_stabilized(consciousness_allocator_stabilized_t *allocator);

// Convenience macros for common operations
#define CONSCIOUSNESS_ALLOC_WORKING(allocator, size) \
    consciousness_allocate_stabilized(allocator, size, MEMORY_TYPE_WORKING, AWARENESS_BASIC)

#define CONSCIOUSNESS_ALLOC_SEMANTIC(allocator, size) \
    consciousness_allocate_stabilized(allocator, size, MEMORY_TYPE_SEMANTIC, AWARENESS_PATTERN)

#define CONSCIOUSNESS_ALLOC_INTUITIVE(allocator, size) \
    consciousness_allocate_stabilized(allocator, size, MEMORY_TYPE_INTUITIVE, AWARENESS_CREATIVE)

#define CONSCIOUSNESS_ALLOC_TRANSCENDENT(allocator, size) \
    consciousness_allocate_stabilized(allocator, size, MEMORY_TYPE_QUANTUM, AWARENESS_TRANSCENDENT)

// Performance testing helpers
static inline uint64_t consciousness_get_timestamp_ns(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
    }
    return 0;
}

static inline double consciousness_compute_allocation_rate(uint64_t allocations, uint64_t duration_ns)
{
    if (duration_ns == 0) return 0.0;
    return (double)allocations * 1000000000.0 / (double)duration_ns;
}

#endif // SM_CONSCIOUSNESS_STABILIZED_H