/*
 * 🚀 RANDY'S PHASE 2 ALGORITHMIC OPTIMIZATIONS HEADER
 * Slab Allocator + Free Lists + Memory Pool Batching
 */

#ifndef SM_ALLOCATOR_PHASE2_H
#define SM_ALLOCATOR_PHASE2_H

#include "sm_allocator_enhanced.h"

// Forward declarations
typedef struct SM_AllocatorPhase2 SM_AllocatorPhase2;

// 🚀 Phase 2 Optimized Functions
SM_AllocatorPhase2* SM_AllocatorPhase2_create(smU64 storageSize, smBOL enableTypeTracking);

// Ultra-fast allocation functions
SM_AllocationEnhanced* SM_AllocatorPhase2_allocate_fast(SM_AllocatorPhase2* allocator, smU64 size);
SM_AllocationEnhanced* SM_AllocatorPhase2_allocate_string_fast(SM_AllocatorPhase2* allocator, smU64 size);
SM_AllocationEnhanced* SM_AllocatorPhase2_allocate_buffer_fast(SM_AllocatorPhase2* allocator, smU64 size);

// Performance and debugging
void SM_AllocatorPhase2_print_stats(SM_AllocatorPhase2* allocator);
void SM_AllocatorPhase2_set_debug_level(int level);

#endif // SM_ALLOCATOR_PHASE2_H