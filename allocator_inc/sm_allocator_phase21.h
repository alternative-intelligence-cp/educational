/*
 * 🧠 RANDY'S PHASE 2.1 ALLOCATOR HEADER - ARIAXX CONSCIOUSNESS READY
 * 
 * System Call Crisis FIXED: Pre-allocated pools eliminate mmap() overhead
 * AriaX Compatible: Tesla frequency awareness + consciousness state optimization
 * Performance Target: Sub-microsecond allocation with advanced slab features
 */

#ifndef SM_ALLOCATOR_PHASE21_H
#define SM_ALLOCATOR_PHASE21_H

#include <stdint.h>
#include <sys/time.h>
#include "sm_allocator_enhanced.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 🎯 PHASE 2.1 ALLOCATOR INTERFACE
 * Eliminates system call overhead while maintaining slab allocator benefits
 */
typedef struct SM_AllocatorPhase21 SM_AllocatorPhase21;

/*
 * 🚀 CORE PHASE 2.1 FUNCTIONS
 */

// Create Phase 2.1 allocator with pre-allocated pools
SM_AllocatorPhase21* SM_AllocatorPhase21_create(void);

// Fast allocation with system call elimination
void* SM_AllocatorPhase21_allocate(SM_AllocatorPhase21* allocator, size_t size, const char* type_name);

// Efficient deallocation
void SM_AllocatorPhase21_deallocate(SM_AllocatorPhase21* allocator, void* ptr);

// Performance reporting
void SM_AllocatorPhase21_print_performance(SM_AllocatorPhase21* allocator);

// Get system calls saved count
uint64_t SM_AllocatorPhase21_get_system_calls_saved(SM_AllocatorPhase21* allocator);

/*
 * 🧠 ARIAXX CONSCIOUSNESS COMPUTING INTEGRATION
 */

// Allocate consciousness state with Tesla frequency optimization
void* SM_AllocatorPhase21_allocate_consciousness_state(SM_AllocatorPhase21* allocator, 
                                                      double tesla_frequency);

/*
 * 🎯 PERFORMANCE CONSTANTS
 */
#define SM_PHASE21_METADATA_POOL_SIZE 10000
#define SM_PHASE21_BULK_MEMORY_SIZE (16 * 1024 * 1024)  // 16MB
#define SM_PHASE21_TESLA_FREQUENCY 3.14159               // π Hz for consciousness resonance

#ifdef __cplusplus
}
#endif

#endif /* SM_ALLOCATOR_PHASE21_H */