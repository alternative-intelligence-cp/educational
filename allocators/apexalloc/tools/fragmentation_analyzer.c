/**
 * ApexAlloc Fragmentation Analyzer
 * 
 * Measures and reports memory fragmentation metrics.
 * Educational tool for understanding allocator efficiency.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ALLOCS 10000

typedef struct {
    void* ptr;
    size_t requested;
    size_t actual;
} alloc_record_t;

static alloc_record_t g_records[MAX_ALLOCS];
static int g_record_count = 0;

// Get actual allocation size (usable size)
size_t get_actual_size(void* ptr) {
    // In real implementation, would use malloc_usable_size()
    // For demo, we'll estimate based on size classes
    return malloc_usable_size(ptr);
}

void record_allocation(void* ptr, size_t requested) {
    if (g_record_count >= MAX_ALLOCS) return;
    
    g_records[g_record_count].ptr = ptr;
    g_records[g_record_count].requested = requested;
    g_records[g_record_count].actual = get_actual_size(ptr);
    g_record_count++;
}

void calculate_internal_fragmentation(void) {
    size_t total_requested = 0;
    size_t total_actual = 0;
    size_t total_wasted = 0;
    
    for (int i = 0; i < g_record_count; i++) {
        total_requested += g_records[i].requested;
        total_actual += g_records[i].actual;
        total_wasted += (g_records[i].actual - g_records[i].requested);
    }
    
    double frag_percent = 0.0;
    if (total_actual > 0) {
        frag_percent = (double)total_wasted / total_actual * 100.0;
    }
    
    printf("\n╔═══════════════════════════════════════════════════════╗\n");
    printf("║        Internal Fragmentation Analysis               ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n\n");
    
    printf("📊 Summary:\n");
    printf("   Allocations:      %d\n", g_record_count);
    printf("   Requested:        %.2f KB\n", total_requested / 1024.0);
    printf("   Actually used:    %.2f KB\n", total_actual / 1024.0);
    printf("   Wasted (padding): %.2f KB\n", total_wasted / 1024.0);
    printf("   Fragmentation:    %.2f%%\n", frag_percent);
    
    printf("\n📈 Per-allocation breakdown:\n");
    printf("   Size Range     | Count | Avg Waste | Efficiency\n");
    printf("   ---------------+-------+-----------+-----------\n");
    
    // Group by size ranges
    struct {
        size_t min, max;
        int count;
        size_t waste;
    } ranges[] = {
        {1, 32, 0, 0},
        {33, 64, 0, 0},
        {65, 128, 0, 0},
        {129, 256, 0, 0},
        {257, 512, 0, 0},
        {513, 1024, 0, 0},
        {1025, 4096, 0, 0},
        {4097, 1024*1024, 0, 0}
    };
    int num_ranges = sizeof(ranges) / sizeof(ranges[0]);
    
    for (int i = 0; i < g_record_count; i++) {
        size_t req = g_records[i].requested;
        size_t waste = g_records[i].actual - g_records[i].requested;
        
        for (int r = 0; r < num_ranges; r++) {
            if (req >= ranges[r].min && req <= ranges[r].max) {
                ranges[r].count++;
                ranges[r].waste += waste;
                break;
            }
        }
    }
    
    for (int r = 0; r < num_ranges; r++) {
        if (ranges[r].count == 0) continue;
        
        double avg_waste = (double)ranges[r].waste / ranges[r].count;
        double efficiency = 100.0 - (avg_waste / ranges[r].max * 100.0);
        
        printf("   %4zu - %-7zu | %5d | %7.1f B | %7.1f%%\n",
               ranges[r].min, ranges[r].max, ranges[r].count,
               avg_waste, efficiency);
    }
}

void test_worst_case_fragmentation(void) {
    printf("\n╔═══════════════════════════════════════════════════════╗\n");
    printf("║        Worst-Case Fragmentation Test                 ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n\n");
    
    printf("Testing pathological allocation patterns...\n\n");
    
    // Test 1: Power-of-2 minus 1 (worst for binary buddy)
    printf("📍 Test 1: Power-of-2 minus 1 sizes\n");
    size_t bad_sizes[] = {31, 63, 127, 255, 511, 1023, 2047, 4095};
    
    for (int i = 0; i < 8; i++) {
        void* ptr = malloc(bad_sizes[i]);
        if (ptr) {
            record_allocation(ptr, bad_sizes[i]);
            size_t actual = get_actual_size(ptr);
            size_t waste = actual - bad_sizes[i];
            printf("   malloc(%zu) → %zu bytes (waste: %zu, %.1f%%)\n",
                   bad_sizes[i], actual, waste,
                   (double)waste / actual * 100.0);
            free(ptr);
        }
    }
    
    // Test 2: Prime numbers (hard to round efficiently)
    printf("\n📍 Test 2: Prime number sizes\n");
    size_t primes[] = {17, 37, 67, 131, 257, 521, 1031, 2053};
    
    for (int i = 0; i < 8; i++) {
        void* ptr = malloc(primes[i]);
        if (ptr) {
            record_allocation(ptr, primes[i]);
            size_t actual = get_actual_size(ptr);
            size_t waste = actual - primes[i];
            printf("   malloc(%zu) → %zu bytes (waste: %zu, %.1f%%)\n",
                   primes[i], actual, waste,
                   (double)waste / actual * 100.0);
            free(ptr);
        }
    }
}

void test_realistic_workload(void) {
    printf("\n╔═══════════════════════════════════════════════════════╗\n");
    printf("║        Realistic Workload Test                       ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n\n");
    
    printf("Simulating typical application allocations...\n\n");
    
    // Reset records
    g_record_count = 0;
    
    // Allocate various sizes like a real application
    size_t sizes[] = {
        16, 24, 32, 48, 64,      // Small objects
        100, 150, 200, 250,      // Medium objects
        512, 768, 1024,          // Large objects
        2000, 3000, 4000         // Very large
    };
    
    void* ptrs[1000];
    int ptr_count = 0;
    
    srand(time(NULL));
    
    for (int i = 0; i < 1000; i++) {
        size_t size = sizes[rand() % 15];
        void* ptr = malloc(size);
        if (ptr) {
            record_allocation(ptr, size);
            ptrs[ptr_count++] = ptr;
        }
    }
    
    calculate_internal_fragmentation();
    
    // Cleanup
    for (int i = 0; i < ptr_count; i++) {
        free(ptrs[i]);
    }
}

void print_optimization_tips(void) {
    printf("\n╔═══════════════════════════════════════════════════════╗\n");
    printf("║        Optimization Tips                             ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n\n");
    
    printf("💡 How to reduce internal fragmentation:\n\n");
    printf("1. Size Classes:\n");
    printf("   • Use logarithmic spacing (12.5%% overhead max)\n");
    printf("   • Not linear (wastes too much space)\n");
    printf("   • Example: 16, 32, 48, 64, 80, 96...\n\n");
    
    printf("2. Application Level:\n");
    printf("   • Round allocations to next size class yourself\n");
    printf("   • Reuse allocations instead of malloc/free\n");
    printf("   • Use object pools for fixed-size objects\n\n");
    
    printf("3. Alignment:\n");
    printf("   • Minimize alignment requirements when possible\n");
    printf("   • Pack structs carefully (avoid padding)\n");
    printf("   • Use __attribute__((packed)) judiciously\n\n");
    
    printf("📚 External fragmentation is a separate issue!\n");
    printf("   That's about free memory being scattered.\n");
    printf("   Use compaction or generational GC to solve it.\n\n");
}

int main(int argc, char** argv) {
    if (argc > 1 && strcmp(argv[1], "worst") == 0) {
        test_worst_case_fragmentation();
    } else if (argc > 1 && strcmp(argv[1], "tips") == 0) {
        print_optimization_tips();
    } else {
        test_realistic_workload();
        print_optimization_tips();
    }
    
    return 0;
}
