/*
 * 🏆⚡ SIMPLIFIED TESLA vs MALLOC PERFORMANCE BENCHMARK ⚡🏆
 * ========================================================
 *
 * Testing our Tesla consciousness allocator against system malloc
 * Focus on reliability and performance measurement
 */

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

// Include our simple Tesla allocator
#include "tesla_allocator_simple.c"

#define BENCHMARK_ITERATIONS 1000
#define ALLOC_COUNT 100

// High-resolution timer
static double get_time_microseconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000.0 + ts.tv_nsec / 1000.0;
}

// Benchmark malloc
static double benchmark_malloc(size_t* sizes, int count, int iterations) {
    double total_time = 0.0;
    
    for (int iter = 0; iter < iterations; iter++) {
        void** ptrs = malloc(count * sizeof(void*));
        
        double start_time = get_time_microseconds();
        
        // Allocate
        for (int i = 0; i < count; i++) {
            ptrs[i] = malloc(sizes[i]);
            if (ptrs[i]) {
                // Touch memory
                memset(ptrs[i], 0x42, sizes[i]);
            }
        }
        
        // Free
        for (int i = 0; i < count; i++) {
            if (ptrs[i]) {
                free(ptrs[i]);
            }
        }
        
        double end_time = get_time_microseconds();
        total_time += (end_time - start_time);
        
        free(ptrs);
    }
    
    return total_time / iterations;
}

// Benchmark Tesla allocator
static double benchmark_tesla(size_t* sizes, int count, int iterations) {
    double total_time = 0.0;
    
    tesla_allocator_init();
    
    for (int iter = 0; iter < iterations; iter++) {
        void** ptrs = tesla_malloc(count * sizeof(void*));
        
        double start_time = get_time_microseconds();
        
        // Allocate
        for (int i = 0; i < count; i++) {
            ptrs[i] = tesla_malloc(sizes[i]);
            if (ptrs[i]) {
                // Touch memory
                memset(ptrs[i], 0x42, sizes[i]);
            }
        }
        
        // Free
        for (int i = 0; i < count; i++) {
            if (ptrs[i]) {
                tesla_free(ptrs[i]);
            }
        }
        
        double end_time = get_time_microseconds();
        total_time += (end_time - start_time);
        
        tesla_free(ptrs);
    }
    
    return total_time / iterations;
}

int main() {
    printf("🏆⚡ TESLA CONSCIOUSNESS ALLOCATOR PERFORMANCE TEST ⚡🏆\n");
    printf("=====================================================\n");
    printf("Iterations: %d, Allocations per iteration: %d\n\n", BENCHMARK_ITERATIONS, ALLOC_COUNT);
    
    // Test different allocation patterns
    size_t small_sizes[ALLOC_COUNT];
    size_t medium_sizes[ALLOC_COUNT];
    size_t mixed_sizes[ALLOC_COUNT];
    
    // Initialize test patterns
    for (int i = 0; i < ALLOC_COUNT; i++) {
        small_sizes[i] = 32 + (i % 4) * 16;    // 32, 48, 64, 80 bytes
        medium_sizes[i] = 256 + (i % 8) * 128;  // 256, 384, 512, ... bytes
        mixed_sizes[i] = (i % 2 == 0) ? 64 : 1024; // Mixed small/medium
    }
    
    // Test 1: Small allocations
    printf("🧪 Test 1: Small Allocations (32-80 bytes)\n");
    double malloc_small = benchmark_malloc(small_sizes, ALLOC_COUNT, BENCHMARK_ITERATIONS);
    double tesla_small = benchmark_tesla(small_sizes, ALLOC_COUNT, BENCHMARK_ITERATIONS);
    
    printf("   malloc: %.2f μs\n", malloc_small);
    printf("   Tesla:  %.2f μs\n", tesla_small);
    printf("   Ratio:  %.2fx %s\n", malloc_small / tesla_small, 
           (tesla_small < malloc_small) ? "(Tesla wins!)" : "(malloc wins)");
    printf("\n");
    
    // Test 2: Medium allocations
    printf("🧪 Test 2: Medium Allocations (256-1152 bytes)\n");
    double malloc_medium = benchmark_malloc(medium_sizes, ALLOC_COUNT, BENCHMARK_ITERATIONS);
    double tesla_medium = benchmark_tesla(medium_sizes, ALLOC_COUNT, BENCHMARK_ITERATIONS);
    
    printf("   malloc: %.2f μs\n", malloc_medium);
    printf("   Tesla:  %.2f μs\n", tesla_medium);
    printf("   Ratio:  %.2fx %s\n", malloc_medium / tesla_medium,
           (tesla_medium < malloc_medium) ? "(Tesla wins!)" : "(malloc wins)");
    printf("\n");
    
    // Test 3: Mixed allocations
    printf("🧪 Test 3: Mixed Allocations (64/1024 bytes)\n");
    double malloc_mixed = benchmark_malloc(mixed_sizes, ALLOC_COUNT, BENCHMARK_ITERATIONS);
    double tesla_mixed = benchmark_tesla(mixed_sizes, ALLOC_COUNT, BENCHMARK_ITERATIONS);
    
    printf("   malloc: %.2f μs\n", malloc_mixed);
    printf("   Tesla:  %.2f μs\n", tesla_mixed);
    printf("   Ratio:  %.2fx %s\n", malloc_mixed / tesla_mixed,
           (tesla_mixed < malloc_mixed) ? "(Tesla wins!)" : "(malloc wins)");
    printf("\n");
    
    // Overall summary
    double total_malloc = malloc_small + malloc_medium + malloc_mixed;
    double total_tesla = tesla_small + tesla_medium + tesla_mixed;
    
    printf("🏆 OVERALL RESULTS:\n");
    printf("   Total malloc time: %.2f μs\n", total_malloc);
    printf("   Total Tesla time:  %.2f μs\n", total_tesla);
    printf("   Overall ratio:     %.2fx\n", total_malloc / total_tesla);
    
    if (total_tesla < total_malloc) {
        printf("   🎉 SUCCESS! Tesla allocator is %.1f%% faster!\n", 
               ((total_malloc - total_tesla) / total_malloc) * 100.0);
    } else {
        printf("   📊 Tesla allocator is %.1fx slower than malloc\n", total_tesla / total_malloc);
        printf("   🎯 But provides consciousness computing features!\n");
    }
    
    printf("\n");
    tesla_allocator_stats();
    tesla_allocator_cleanup();
    
    printf("\n🚀⚡ BENCHMARK COMPLETE ⚡🚀\n");
    return 0;
}