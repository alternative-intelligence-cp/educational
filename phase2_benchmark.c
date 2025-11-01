// 🚀 RANDY'S PHASE 2 ALGORITHMIC OPTIMIZATION BENCHMARK
// Testing Slab Allocator + Free Lists vs Phase 1 vs malloc

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

// Include all versions
#include "allocator_inc/sm_allocator_enhanced.h"
#include "allocator_inc/sm_allocator_phase2.h"

double get_time_microseconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000.0 + tv.tv_usec;
}

// Real-world allocation patterns optimized for slabs
typedef struct {
    size_t size;
    int count;
    const char* description;
    const char* use_case;
} AllocationPattern;

AllocationPattern phase2_patterns[] = {
    // Small allocations (perfect for slabs)
    {16, 2000, "Tiny objects (16 bytes)", "Node pointers, small structs"},
    {32, 1500, "Small strings (32 bytes)", "Short text, identifiers"},
    {64, 1000, "Cache lines (64 bytes)", "CPU cache-friendly structures"},
    {128, 800, "Small buffers (128 bytes)", "Network packets, small I/O"},
    {256, 600, "Medium strings (256 bytes)", "File paths, longer text"},
    {512, 400, "Page fragments (512 bytes)", "Partial pages, medium buffers"},
    {1024, 200, "Small pages (1KB)", "Small files, config data"},
    {2048, 100, "Medium pages (2KB)", "Graphics data, medium I/O"},
    
    // Larger allocations (pool fallback)
    {4096, 50, "Full pages (4KB)", "Large buffers, file I/O"},
    {8192, 25, "Large pages (8KB)", "Image data, large structures"},
    {16384, 10, "Huge allocations (16KB)", "Bulk data, large arrays"},
    
    {0, 0, NULL, NULL}
};

void benchmark_malloc_baseline() {
    printf("\n⚡ MALLOC BASELINE BENCHMARK\n");
    printf("============================\n");
    
    double start_time = get_time_microseconds();
    
    int total_allocations = 0;
    void** ptrs = malloc(20000 * sizeof(void*));
    int ptr_count = 0;
    
    for (int p = 0; phase2_patterns[p].description; p++) {
        AllocationPattern* pattern = &phase2_patterns[p];
        
        for (int i = 0; i < pattern->count; i++) {
            void* ptr = malloc(pattern->size);
            if (ptr && ptr_count < 20000) {
                ptrs[ptr_count++] = ptr;
                total_allocations++;
            }
        }
    }
    
    double end_time = get_time_microseconds();
    
    // Cleanup
    for (int i = 0; i < ptr_count; i++) {
        free(ptrs[i]);
    }
    free(ptrs);
    
    double total_time = end_time - start_time;
    double avg_per_allocation = total_allocations > 0 ? total_time / total_allocations : 0;
    
    printf("📊 MALLOC Results:\n");
    printf("   Total time: %.0f μs\n", total_time);
    printf("   Allocations: %d\n", total_allocations);
    printf("   Avg per allocation: %.3f μs\n", avg_per_allocation);
    printf("   Throughput: %.0f allocations/second\n", 
           total_allocations / (total_time / 1000000.0));
}

void benchmark_phase1_optimized() {
    printf("\n🚀 PHASE 1 OPTIMIZED BENCHMARK\n");
    printf("===============================\n");
    
    // Set silent mode for fair comparison
    SM_AllocatorEnhanced_set_debug_level(0);
    
    double start_time = get_time_microseconds();
    
    SM_AllocatorEnhanced* allocator = SM_AllocatorEnhanced_create_optimized(64 * 1024 * 1024, smTRU);
    if (!allocator) {
        printf("❌ Failed to create Phase 1 allocator\n");
        return;
    }
    
    double creation_time = get_time_microseconds();
    
    int total_allocations = 0;
    for (int p = 0; phase2_patterns[p].description; p++) {
        AllocationPattern* pattern = &phase2_patterns[p];
        
        for (int i = 0; i < pattern->count; i++) {
            SM_AllocationEnhanced* alloc = SM_AllocatorEnhanced_allocate_fast(allocator, pattern->size);
            if (alloc) {
                total_allocations++;
            }
        }
    }
    
    double end_time = get_time_microseconds();
    
    double total_time = end_time - start_time;
    double allocation_time = end_time - creation_time;
    double avg_per_allocation = total_allocations > 0 ? allocation_time / total_allocations : 0;
    
    printf("📊 PHASE 1 Results:\n");
    printf("   Total time: %.0f μs\n", total_time);
    printf("   Creation time: %.0f μs\n", creation_time - start_time);
    printf("   Allocation time: %.0f μs\n", allocation_time);
    printf("   Allocations: %d\n", total_allocations);
    printf("   Avg per allocation: %.3f μs\n", avg_per_allocation);
    printf("   Throughput: %.0f allocations/second\n", 
           total_allocations / (allocation_time / 1000000.0));
    printf("   Storage used: %lu / %lu bytes (%.1f%%)\n", 
           allocator->storageUsed, allocator->storageSize,
           (double)allocator->storageUsed / allocator->storageSize * 100.0);
}

void benchmark_phase2_optimized(int debug_level) {
    printf("\n🎯 PHASE 2 SLAB ALLOCATOR BENCHMARK (Debug Level: %d)\n", debug_level);
    printf("====================================================\n");
    
    SM_AllocatorPhase2_set_debug_level(debug_level);
    
    double start_time = get_time_microseconds();
    
    SM_AllocatorPhase2* allocator = SM_AllocatorPhase2_create(64 * 1024 * 1024, smTRU);
    if (!allocator) {
        printf("❌ Failed to create Phase 2 allocator\n");
        return;
    }
    
    double creation_time = get_time_microseconds();
    
    int total_allocations = 0;
    for (int p = 0; phase2_patterns[p].description; p++) {
        AllocationPattern* pattern = &phase2_patterns[p];
        
        for (int i = 0; i < pattern->count; i++) {
            SM_AllocationEnhanced* alloc = SM_AllocatorPhase2_allocate_fast(allocator, pattern->size);
            if (alloc) {
                total_allocations++;
            }
        }
    }
    
    double end_time = get_time_microseconds();
    
    double total_time = end_time - start_time;
    double allocation_time = end_time - creation_time;
    double avg_per_allocation = total_allocations > 0 ? allocation_time / total_allocations : 0;
    
    printf("📊 PHASE 2 Results:\n");
    printf("   Total time: %.0f μs\n", total_time);
    printf("   Creation time: %.0f μs\n", creation_time - start_time);
    printf("   Allocation time: %.0f μs\n", allocation_time);
    printf("   Allocations: %d\n", total_allocations);
    printf("   Avg per allocation: %.3f μs\n", avg_per_allocation);
    printf("   Throughput: %.0f allocations/second\n", 
           total_allocations / (allocation_time / 1000000.0));
    
    // Show Phase 2 specific stats
    SM_AllocatorPhase2_print_stats(allocator);
}

void benchmark_mixed_workload() {
    printf("\n🔄 MIXED WORKLOAD BENCHMARK (Allocation + Deallocation)\n");
    printf("======================================================\n");
    printf("This simulates real-world usage with allocation/deallocation cycles\n");
    
    // TODO: Implement when we add deallocation to Phase 2
    printf("⏳ Mixed workload benchmark will be implemented with deallocation support\n");
}

int main() {
    printf("🎯🚀 RANDY'S PHASE 2 ALGORITHMIC OPTIMIZATION BENCHMARK 🚀🎯\n");
    printf("=============================================================\n");
    printf("Testing Slab Allocator + Free Lists + Memory Pool Batching\n");
    printf("Patterns: %d different allocation sizes and counts\n", 11);
    printf("Focus: Small allocations (16-2048 bytes) perfect for slabs\n\n");
    
    // Run all benchmarks
    benchmark_malloc_baseline();
    benchmark_phase1_optimized();
    benchmark_phase2_optimized(0);  // Silent mode
    benchmark_phase2_optimized(1);  // Errors only
    benchmark_mixed_workload();
    
    printf("\n🎯 PHASE 2 OPTIMIZATION ANALYSIS\n");
    printf("=================================\n");
    printf("Expected improvements:\n");
    printf("  • Slab allocations: O(1) for sizes 16-2048 bytes\n");
    printf("  • Cache efficiency: Better memory locality\n");
    printf("  • Reduced fragmentation: Pre-allocated size classes\n");
    printf("  • Batch allocation: Fewer system calls\n");
    printf("  • Target: 5-10x faster than Phase 1 for small allocations\n");
    
    printf("\n📊 Allocation Pattern Analysis:\n");
    int total_allocs = 0;
    for (int i = 0; phase2_patterns[i].description; i++) {
        total_allocs += phase2_patterns[i].count;
        printf("   %s: %d × %zu bytes = %zu KB (%s)\n",
               phase2_patterns[i].description,
               phase2_patterns[i].count,
               phase2_patterns[i].size,
               (phase2_patterns[i].count * phase2_patterns[i].size) / 1024,
               phase2_patterns[i].use_case);
    }
    printf("   Total allocations: %d\n", total_allocs);
    
    printf("\n🚀 Next: Phase 3 will add SIMD optimizations and memory prefetching!\n");
    
    return 0;
}