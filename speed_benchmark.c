// 🚀 RANDY'S SPEED MODE BENCHMARK
// Testing performance with different optimization flags

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

double get_time_microseconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000.0 + tv.tv_usec;
}

void benchmark_malloc_pattern(const char* test_name, int iterations) {
    printf("🧪 Testing: %s\n", test_name);
    
    double start_time = get_time_microseconds();
    
    // Typical allocation pattern
    void** ptrs = malloc(sizeof(void*) * iterations);
    
    for (int i = 0; i < iterations; i++) {
        size_t size = 16 + (i % 1024);  // Variable sizes 16-1040 bytes
        ptrs[i] = malloc(size);
        if (ptrs[i]) {
            memset(ptrs[i], 0x42, size);  // Simulate usage
        }
    }
    
    for (int i = 0; i < iterations; i++) {
        free(ptrs[i]);
    }
    
    free(ptrs);
    
    double end_time = get_time_microseconds();
    double total_time = end_time - start_time;
    
    printf("   Time: %.0f μs (%.2f μs/alloc)\n", total_time, total_time / iterations);
    printf("   Rate: %.0f allocs/sec\n\n", (iterations * 1000000.0) / total_time);
}

int main() {
    printf("🎯 RANDY'S ALLOCATOR SPEED BENCHMARK\n");
    printf("=====================================\n\n");
    
    int test_iterations = 10000;
    
    printf("Testing %d allocations with different configurations:\n\n", test_iterations);
    
    // Test current build configuration
#ifdef RANDY_SPEED_MODE
    benchmark_malloc_pattern("Randy's Allocator - SPEED MODE (All optimizations disabled)", test_iterations);
#elif defined(RANDY_DISABLE_BITSET) && defined(RANDY_DISABLE_TYPES)
    benchmark_malloc_pattern("Randy's Allocator - BitSet + Types DISABLED", test_iterations);
#elif defined(RANDY_DISABLE_BITSET)
    benchmark_malloc_pattern("Randy's Allocator - BitSet DISABLED", test_iterations);
#elif defined(RANDY_DISABLE_TYPES)
    benchmark_malloc_pattern("Randy's Allocator - Types DISABLED", test_iterations);
#else
    benchmark_malloc_pattern("Randy's Allocator - FULL FEATURES (Default)", test_iterations);
#endif
    
    printf("🚀 To test different modes, compile with:\n");
    printf("   gcc -DRANDY_SPEED_MODE ...           # Maximum speed\n");
    printf("   gcc -DRANDY_DISABLE_BITSET ...       # Disable BitSet only\n"); 
    printf("   gcc -DRANDY_DISABLE_TYPES ...        # Disable types only\n");
    printf("   gcc -DRANDY_DISABLE_LOGGING ...      # Disable logging only\n");
    printf("   gcc -DRANDY_DISABLE_STATS ...        # Disable stats only\n");
    
    return 0;
}