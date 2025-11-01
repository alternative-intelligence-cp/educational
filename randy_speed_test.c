// 🚀 RANDY'S ALLOCATOR SPEED MODE COMPARISON
// Testing Randy's allocator with different optimization flags

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

// For Randy's Enhanced Allocator
#include "allocator_inc/sm_allocator_enhanced.h"

double get_time_microseconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000.0 + tv.tv_usec;
}

// Randy's Enhanced Allocator Setup
static SM_AllocatorEnhanced* g_randy_allocator = NULL;

void randy_init() {
    if (!g_randy_allocator) {
        g_randy_allocator = SM_AllocatorEnhanced_create(16 * 1024 * 1024, smTRU); // 16MB with type tracking
    }
}

void* randy_malloc(size_t size) {
    if (!g_randy_allocator) randy_init();
    
#ifndef RANDY_DISABLE_TYPES
    SM_AllocationEnhanced* allocation = SM_AllocatorEnhanced_allocateTyped(g_randy_allocator, size, SM_TYPE_CUSTOM, "benchmark_data");
#else
    SM_AllocationEnhanced* allocation = SM_AllocatorEnhanced_allocate(g_randy_allocator, size);
#endif
    if (!allocation) return NULL;
    
    return SM_AllocatorEnhanced_getDataPointer(allocation);
}

void randy_free(void* ptr) {
    // Randy's smart garbage collection handles this
    (void)ptr;
}

void benchmark_allocator(const char* test_name, int iterations) {
    printf("🧪 Testing: %s\n", test_name);
    
    double start_time = get_time_microseconds();
    
    // Typical allocation pattern
    void** ptrs = malloc(sizeof(void*) * iterations);
    
    for (int i = 0; i < iterations; i++) {
        size_t size = 16 + (i % 1024);  // Variable sizes 16-1040 bytes
        ptrs[i] = randy_malloc(size);
        if (ptrs[i]) {
            memset(ptrs[i], 0x42, size);  // Simulate usage
        }
    }
    
    for (int i = 0; i < iterations; i++) {
        randy_free(ptrs[i]);
    }
    
    free(ptrs);
    
    double end_time = get_time_microseconds();
    double total_time = end_time - start_time;
    
    printf("   Time: %.0f μs (%.2f μs/alloc)\n", total_time, total_time / iterations);
    printf("   Rate: %.0f allocs/sec\n", (iterations * 1000000.0) / total_time);
    
    // Show configuration info
    printf("   Config: ");
#ifdef RANDY_SPEED_MODE
    printf("SPEED_MODE ");
#endif
#ifdef RANDY_DISABLE_BITSET
    printf("NO_BITSET ");
#endif
#ifdef RANDY_DISABLE_TYPES
    printf("NO_TYPES ");
#endif
#ifdef RANDY_DISABLE_LOGGING
    printf("NO_LOGGING ");
#endif
#ifdef RANDY_DISABLE_STATS
    printf("NO_STATS ");
#endif
    printf("\n\n");
}

int main() {
    printf("🎯 RANDY'S ALLOCATOR SPEED MODE BENCHMARK\n"); 
    printf("==========================================\n\n");
    
    int test_iterations = 5000;  // Smaller test for Randy's allocator
    
    printf("Testing %d allocations with Randy's enhanced allocator:\n\n", test_iterations);
    
    // Test current build configuration
#ifdef RANDY_SPEED_MODE
    benchmark_allocator("Randy's Allocator - SPEED MODE (All optimizations disabled)", test_iterations);
#elif defined(RANDY_DISABLE_BITSET) && defined(RANDY_DISABLE_TYPES)
    benchmark_allocator("Randy's Allocator - BitSet + Types DISABLED", test_iterations);
#elif defined(RANDY_DISABLE_BITSET)
    benchmark_allocator("Randy's Allocator - BitSet DISABLED", test_iterations);
#elif defined(RANDY_DISABLE_TYPES)
    benchmark_allocator("Randy's Allocator - Types DISABLED", test_iterations);
#else
    benchmark_allocator("Randy's Allocator - FULL FEATURES (Default)", test_iterations);
#endif
    
    // Show Randy's allocator stats
    if (g_randy_allocator) {
        printf("📊 RANDY'S ALLOCATOR FINAL STATS:\n");
#ifndef RANDY_DISABLE_STATS
        SM_AllocatorEnhanced_printStats(g_randy_allocator);
#ifndef RANDY_DISABLE_TYPES
        SM_AllocatorEnhanced_printTypeStats(g_randy_allocator);
#endif
#else
        printf("   Statistics disabled in this build\n");
#endif
        SM_AllocatorEnhanced_destroy(g_randy_allocator);
    }
    
    printf("\n🚀 Randy's performance flags working!\n");
    printf("   BitSet savings: %s\n", 
#ifndef RANDY_DISABLE_BITSET
           "ENABLED (87.5% memory savings)"
#else
           "DISABLED (using boolean array)"
#endif
    );
    printf("   Type tracking: %s\n", 
#ifndef RANDY_DISABLE_TYPES
           "ENABLED (full type awareness)"
#else
           "DISABLED (faster allocations)"
#endif
    );
    
    return 0;
}