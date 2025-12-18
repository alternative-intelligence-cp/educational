/**
 * ApexAlloc Simple Benchmark
 * 
 * Tests basic allocation/deallocation performance with various patterns.
 * Useful for comparing against system malloc and other allocators.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

// Timing helper
static double get_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// Benchmark 1: Sequential allocation
void bench_sequential(size_t count, size_t size) {
    printf("\n📊 Sequential Allocation (%zu allocs of %zu bytes)\n", count, size);
    
    void** ptrs = malloc(count * sizeof(void*));
    
    // Allocate
    double start = get_time();
    for (size_t i = 0; i < count; i++) {
        ptrs[i] = malloc(size);
    }
    double alloc_time = get_time() - start;
    
    // Touch memory to ensure it's committed
    for (size_t i = 0; i < count; i++) {
        memset(ptrs[i], 0xAA, size);
    }
    
    // Free
    start = get_time();
    for (size_t i = 0; i < count; i++) {
        free(ptrs[i]);
    }
    double free_time = get_time() - start;
    
    printf("  Alloc: %.3f ms (%.0f ns/op)\n", 
           alloc_time * 1000, (alloc_time / count) * 1e9);
    printf("  Free:  %.3f ms (%.0f ns/op)\n", 
           free_time * 1000, (free_time / count) * 1e9);
    
    free(ptrs);
}

// Benchmark 2: Interleaved alloc/free
void bench_interleaved(size_t count, size_t size) {
    printf("\n📊 Interleaved Alloc/Free (%zu ops of %zu bytes)\n", count, size);
    
    double start = get_time();
    for (size_t i = 0; i < count; i++) {
        void* ptr = malloc(size);
        memset(ptr, 0xBB, size);
        free(ptr);
    }
    double total_time = get_time() - start;
    
    printf("  Total: %.3f ms (%.0f ns/op)\n", 
           total_time * 1000, (total_time / count) * 1e9);
}

// Benchmark 3: Random sizes
void bench_random_sizes(size_t count) {
    printf("\n📊 Random Sizes (%zu allocations)\n", count);
    
    void** ptrs = malloc(count * sizeof(void*));
    size_t sizes[] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};
    size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    // Allocate with random sizes
    double start = get_time();
    for (size_t i = 0; i < count; i++) {
        size_t size = sizes[rand() % num_sizes];
        ptrs[i] = malloc(size);
        if (ptrs[i]) {
            memset(ptrs[i], 0xCC, size);
        }
    }
    double alloc_time = get_time() - start;
    
    // Free in random order
    start = get_time();
    for (size_t i = 0; i < count; i++) {
        size_t j = rand() % count;
        if (ptrs[j]) {
            free(ptrs[j]);
            ptrs[j] = NULL;
        }
    }
    // Free any remaining
    for (size_t i = 0; i < count; i++) {
        if (ptrs[i]) {
            free(ptrs[i]);
        }
    }
    double free_time = get_time() - start;
    
    printf("  Alloc: %.3f ms (%.0f ns/op)\n", 
           alloc_time * 1000, (alloc_time / count) * 1e9);
    printf("  Free:  %.3f ms (%.0f ns/op)\n", 
           free_time * 1000, (free_time / count) * 1e9);
    
    free(ptrs);
}

// Benchmark 4: Cache churn (allocate, hold, free)
void bench_cache_churn(size_t count, size_t hold_count, size_t size) {
    printf("\n📊 Cache Churn (%zu total, hold %zu, %zu bytes)\n", 
           count, hold_count, size);
    
    void** held = malloc(hold_count * sizeof(void*));
    
    // Fill cache
    for (size_t i = 0; i < hold_count; i++) {
        held[i] = malloc(size);
    }
    
    // Churn: allocate and free while holding others
    double start = get_time();
    for (size_t i = 0; i < count; i++) {
        void* ptr = malloc(size);
        memset(ptr, 0xDD, size);
        free(ptr);
    }
    double churn_time = get_time() - start;
    
    // Free held
    for (size_t i = 0; i < hold_count; i++) {
        free(held[i]);
    }
    
    printf("  Churn: %.3f ms (%.0f ns/op)\n", 
           churn_time * 1000, (churn_time / count) * 1e9);
    
    free(held);
}

// Benchmark 5: Bulk operations
void bench_bulk(size_t count, size_t size) {
    printf("\n📊 Bulk Operations (%zu allocs of %zu bytes)\n", count, size);
    
    void** ptrs = malloc(count * sizeof(void*));
    
    // Allocate all
    double start = get_time();
    for (size_t i = 0; i < count; i++) {
        ptrs[i] = malloc(size);
    }
    double alloc_time = get_time() - start;
    
    // Write all
    start = get_time();
    for (size_t i = 0; i < count; i++) {
        memset(ptrs[i], (int)i, size);
    }
    double write_time = get_time() - start;
    
    // Read all
    start = get_time();
    volatile int sum = 0;
    for (size_t i = 0; i < count; i++) {
        sum += ((char*)ptrs[i])[0];
    }
    double read_time = get_time() - start;
    
    // Free all
    start = get_time();
    for (size_t i = 0; i < count; i++) {
        free(ptrs[i]);
    }
    double free_time = get_time() - start;
    
    printf("  Alloc: %.3f ms (%.0f ns/op)\n", 
           alloc_time * 1000, (alloc_time / count) * 1e9);
    printf("  Write: %.3f ms (%.2f GB/s)\n", 
           write_time * 1000, (count * size) / (write_time * 1e9));
    printf("  Read:  %.3f ms (%.2f GB/s)\n", 
           read_time * 1000, (count * size) / (read_time * 1e9));
    printf("  Free:  %.3f ms (%.0f ns/op)\n", 
           free_time * 1000, (free_time / count) * 1e9);
    
    free(ptrs);
}

int main(int argc, char** argv) {
    printf("═══════════════════════════════════════════════\n");
    printf("  ApexAlloc Performance Benchmark Suite\n");
    printf("═══════════════════════════════════════════════\n");
    
    // Seed random number generator
    srand(time(NULL));
    
    // Parse options
    int quick = (argc > 1 && strcmp(argv[1], "quick") == 0);
    size_t iterations = quick ? 10000 : 100000;
    
    printf("\nMode: %s (%zu iterations)\n", quick ? "Quick" : "Full", iterations);
    
    // Run benchmarks
    bench_sequential(iterations, 64);
    bench_sequential(iterations, 256);
    bench_sequential(iterations, 1024);
    
    bench_interleaved(iterations, 64);
    bench_interleaved(iterations, 256);
    
    bench_random_sizes(iterations / 2);
    
    bench_cache_churn(iterations, 100, 128);
    
    bench_bulk(iterations / 10, 4096);
    
    printf("\n═══════════════════════════════════════════════\n");
    printf("  Benchmark Complete!\n");
    printf("═══════════════════════════════════════════════\n");
    
    return 0;
}
