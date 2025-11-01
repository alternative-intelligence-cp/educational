/*
 * 🚀 RANDY'S SIMPLIFIED PHASE 2.1 ALLOCATOR HEADER
 * System Call Crisis FIXED with simple pre-allocated pool approach
 */

#ifndef ALLOCATOR_PHASE21_SIMPLE_H
#define ALLOCATOR_PHASE21_SIMPLE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SM_Phase21Simple SM_Phase21Simple;

// Create simplified Phase 2.1 allocator
SM_Phase21Simple* SM_Phase21Simple_create(void);

// Fast allocation from pre-allocated pool
void* SM_Phase21Simple_allocate(SM_Phase21Simple* allocator, size_t size, const char* type_name);

// Simple deallocation
void SM_Phase21Simple_deallocate(SM_Phase21Simple* allocator, void* ptr);

// AriaX consciousness state allocation
void* SM_Phase21Simple_allocate_consciousness_state(SM_Phase21Simple* allocator, double tesla_frequency);

// Performance reporting
void SM_Phase21Simple_print_performance(SM_Phase21Simple* allocator);
uint64_t SM_Phase21Simple_get_system_calls_saved(SM_Phase21Simple* allocator);

#ifdef __cplusplus
}
#endif

#endif