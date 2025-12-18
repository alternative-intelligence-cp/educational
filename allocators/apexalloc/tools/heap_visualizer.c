/**
 * ApexAlloc Heap Visualizer
 * 
 * Displays memory layout and usage patterns.
 * Useful for understanding fragmentation and allocation patterns.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BLOCK_SIZE 50
#define MAX_ALLOCS 100

typedef struct {
    void* ptr;
    size_t size;
    int active;
} alloc_info_t;

static alloc_info_t g_allocs[MAX_ALLOCS];
static int g_alloc_count = 0;

// ANSI colors
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_RED     "\033[31m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"

void print_header(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           ApexAlloc Heap Visualizer                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void print_memory_map(void) {
    printf("📍 Memory Map:\n");
    printf("   ┌─────────────────────────────────────────────────┐\n");
    
    for (int i = 0; i < g_alloc_count; i++) {
        if (!g_allocs[i].active) continue;
        
        printf("   │ ");
        
        // Print address
        printf(COLOR_CYAN "%p" COLOR_RESET " ", g_allocs[i].ptr);
        
        // Print size with color coding
        if (g_allocs[i].size < 100) {
            printf(COLOR_GREEN "%4zu B" COLOR_RESET, g_allocs[i].size);
        } else if (g_allocs[i].size < 1024) {
            printf(COLOR_YELLOW "%4zu B" COLOR_RESET, g_allocs[i].size);
        } else {
            printf(COLOR_RED "%4zu B" COLOR_RESET, g_allocs[i].size);
        }
        
        // Visual bar
        int blocks = (g_allocs[i].size / 64) + 1;
        if (blocks > BLOCK_SIZE) blocks = BLOCK_SIZE;
        
        printf(" │");
        for (int j = 0; j < blocks; j++) {
            printf("█");
        }
        printf("\n");
    }
    
    printf("   └─────────────────────────────────────────────────┘\n");
}

void print_statistics(void) {
    size_t total_allocated = 0;
    size_t min_size = (size_t)-1;
    size_t max_size = 0;
    int active_count = 0;
    
    for (int i = 0; i < g_alloc_count; i++) {
        if (!g_allocs[i].active) continue;
        
        active_count++;
        total_allocated += g_allocs[i].size;
        
        if (g_allocs[i].size < min_size) min_size = g_allocs[i].size;
        if (g_allocs[i].size > max_size) max_size = g_allocs[i].size;
    }
    
    printf("\n📊 Statistics:\n");
    printf("   Active allocations: %d\n", active_count);
    printf("   Total allocated:    %.2f KB\n", total_allocated / 1024.0);
    if (active_count > 0) {
        printf("   Average size:       %.2f B\n", (double)total_allocated / active_count);
        printf("   Size range:         %zu - %zu bytes\n", min_size, max_size);
    }
    printf("\n");
}

void demo_sequential(void) {
    print_header();
    printf("Demo 1: Sequential Allocations\n");
    printf("─────────────────────────────────────────────────────\n\n");
    
    printf("Allocating 5 blocks of increasing size...\n");
    sleep(1);
    
    size_t sizes[] = {64, 128, 256, 512, 1024};
    for (int i = 0; i < 5; i++) {
        void* ptr = malloc(sizes[i]);
        if (ptr) {
            g_allocs[g_alloc_count].ptr = ptr;
            g_allocs[g_alloc_count].size = sizes[i];
            g_allocs[g_alloc_count].active = 1;
            g_alloc_count++;
            
            printf("\n✅ Allocated %zu bytes at %p\n", sizes[i], ptr);
            print_memory_map();
            sleep(1);
        }
    }
    
    print_statistics();
}

void demo_fragmentation(void) {
    print_header();
    printf("Demo 2: Fragmentation Visualization\n");
    printf("─────────────────────────────────────────────────────\n\n");
    
    printf("Allocating 8 blocks...\n");
    sleep(1);
    
    // Allocate 8 blocks
    for (int i = 0; i < 8; i++) {
        void* ptr = malloc(128);
        if (ptr) {
            g_allocs[g_alloc_count].ptr = ptr;
            g_allocs[g_alloc_count].size = 128;
            g_allocs[g_alloc_count].active = 1;
            g_alloc_count++;
        }
    }
    
    printf("Initial state:\n");
    print_memory_map();
    sleep(1);
    
    printf("\nFreeing every other block (creates fragmentation)...\n");
    sleep(1);
    
    // Free every other one
    for (int i = 0; i < g_alloc_count; i += 2) {
        if (g_allocs[i].active) {
            free(g_allocs[i].ptr);
            g_allocs[i].active = 0;
            printf("❌ Freed block at %p\n", g_allocs[i].ptr);
        }
    }
    
    printf("\nFragmented state:\n");
    print_memory_map();
    print_statistics();
    sleep(1);
    
    printf("\nAllocating large block (may need defragmentation)...\n");
    sleep(1);
    
    void* large = malloc(512);
    if (large) {
        g_allocs[g_alloc_count].ptr = large;
        g_allocs[g_alloc_count].size = 512;
        g_allocs[g_alloc_count].active = 1;
        g_alloc_count++;
        
        printf("✅ Large allocation succeeded at %p\n", large);
        print_memory_map();
        print_statistics();
    }
}

void demo_mixed_sizes(void) {
    print_header();
    printf("Demo 3: Mixed Size Allocations\n");
    printf("─────────────────────────────────────────────────────\n\n");
    
    size_t sizes[] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};
    
    printf("Allocating various sizes from 16B to 8KB...\n");
    sleep(1);
    
    for (int i = 0; i < 10; i++) {
        void* ptr = malloc(sizes[i]);
        if (ptr) {
            g_allocs[g_alloc_count].ptr = ptr;
            g_allocs[g_alloc_count].size = sizes[i];
            g_allocs[g_alloc_count].active = 1;
            g_alloc_count++;
        }
    }
    
    print_memory_map();
    print_statistics();
    
    printf("💡 Notice how allocations are grouped by size class!\n");
}

void cleanup(void) {
    printf("\n🧹 Cleaning up...\n");
    for (int i = 0; i < g_alloc_count; i++) {
        if (g_allocs[i].active) {
            free(g_allocs[i].ptr);
        }
    }
    printf("✅ All memory freed\n\n");
}

int main(int argc, char** argv) {
    if (argc > 1) {
        if (strcmp(argv[1], "sequential") == 0) {
            demo_sequential();
            cleanup();
        } else if (strcmp(argv[1], "fragmentation") == 0) {
            demo_fragmentation();
            cleanup();
        } else if (strcmp(argv[1], "mixed") == 0) {
            demo_mixed_sizes();
            cleanup();
        } else {
            printf("Usage: %s [sequential|fragmentation|mixed]\n", argv[0]);
            return 1;
        }
    } else {
        // Run all demos
        demo_sequential();
        cleanup();
        
        g_alloc_count = 0;
        demo_fragmentation();
        cleanup();
        
        g_alloc_count = 0;
        demo_mixed_sizes();
        cleanup();
    }
    
    return 0;
}
