/**
 * ApexAlloc Statistics Dumper
 * 
 * User-friendly wrapper around apex_ctl() introspection API.
 * Shows allocator health and performance metrics.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Mock stats structures (would come from apexalloc.h)
typedef struct {
    size_t alloc_count;
    size_t free_count;
    size_t bytes_allocated;
    size_t bytes_freed;
    size_t active_bytes;
    size_t peak_bytes;
} apex_thread_stats_t;

typedef struct {
    size_t total_superblocks;
    size_t total_blocks;
    size_t active_allocations;
    size_t bytes_committed;
    size_t bytes_mapped;
    double fragmentation_ratio;
} apex_global_stats_t;

// ANSI colors
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_RED     "\033[31m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"

void print_header(void) {
    printf("\n");
    printf(COLOR_BOLD "╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           ApexAlloc Statistics Report                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    printf("\n");
}

void print_memory_usage(void) {
    // In real implementation, would call apex_ctl()
    // For demo, we'll show the structure
    
    printf(COLOR_CYAN "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("  Memory Usage\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n" COLOR_RESET);
    
    printf("\n");
    printf("  Mapped from OS:      %s%.2f MB%s\n", 
           COLOR_YELLOW, 64.5, COLOR_RESET);
    printf("  Committed pages:     %s%.2f MB%s\n", 
           COLOR_GREEN, 48.2, COLOR_RESET);
    printf("  Active allocations:  %s%.2f MB%s\n", 
           COLOR_GREEN, 32.1, COLOR_RESET);
    printf("  Internal metadata:   %s%.2f MB%s\n", 
           COLOR_BLUE, 2.4, COLOR_RESET);
    
    printf("\n");
    printf("  Efficiency:          %s%.1f%%%s (active / committed)\n",
           COLOR_GREEN, 66.6, COLOR_RESET);
    printf("  Overhead:            %s%.1f%%%s (metadata / total)\n",
           COLOR_BLUE, 3.7, COLOR_RESET);
    printf("\n");
}

void print_allocation_stats(void) {
    printf(COLOR_CYAN "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("  Allocation Statistics\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n" COLOR_RESET);
    
    printf("\n");
    printf("  Total allocations:   %s%'zu%s\n", 
           COLOR_GREEN, (size_t)1248392, COLOR_RESET);
    printf("  Total frees:         %s%'zu%s\n", 
           COLOR_GREEN, (size_t)1248201, COLOR_RESET);
    printf("  Currently active:    %s%'zu%s\n", 
           COLOR_YELLOW, (size_t)191, COLOR_RESET);
    printf("  Peak active:         %s%'zu%s\n", 
           COLOR_RED, (size_t)1024, COLOR_RESET);
    
    printf("\n");
    printf("  Fast path hits:      %s99.2%%%s (TLAB allocations)\n",
           COLOR_GREEN, COLOR_RESET);
    printf("  Slow path:           %s0.8%%%s (required new block)\n",
           COLOR_YELLOW, COLOR_RESET);
    printf("\n");
}

void print_size_class_distribution(void) {
    printf(COLOR_CYAN "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("  Size Class Distribution\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n" COLOR_RESET);
    
    printf("\n");
    printf("  Size Range    | Active | Blocks | Utilization\n");
    printf("  --------------+--------+--------+-------------\n");
    
    struct {
        const char* range;
        int active;
        int blocks;
        double util;
    } classes[] = {
        {"16 - 32 B   ", 48, 2, 98.5},
        {"33 - 64 B   ", 124, 5, 87.2},
        {"65 - 128 B  ", 8, 1, 65.3},
        {"129 - 256 B ", 6, 1, 45.1},
        {"257 - 512 B ", 3, 1, 32.8},
        {"513 - 1 KB  ", 2, 1, 28.4},
        {"1 - 4 KB    ", 0, 0, 0.0},
        {"4 - 32 KB   ", 0, 0, 0.0}
    };
    
    for (int i = 0; i < 8; i++) {
        if (classes[i].active == 0) continue;
        
        const char* color = COLOR_GREEN;
        if (classes[i].util < 50.0) color = COLOR_YELLOW;
        if (classes[i].util < 30.0) color = COLOR_RED;
        
        printf("  %s | %6d | %6d | %s%7.1f%%%s\n",
               classes[i].range, classes[i].active, classes[i].blocks,
               color, classes[i].util, COLOR_RESET);
    }
    printf("\n");
}

void print_thread_stats(void) {
    printf(COLOR_CYAN "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("  Per-Thread Statistics\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n" COLOR_RESET);
    
    printf("\n");
    printf("  Thread | Allocations | Frees  | Active | Peak KB\n");
    printf("  -------+-------------+--------+--------+---------\n");
    
    printf("  %6d | %11d | %6d | %6d | %7.1f\n", 1, 842381, 842298, 83, 256.4);
    printf("  %6d | %11d | %6d | %6d | %7.1f\n", 2, 301849, 301821, 28, 128.2);
    printf("  %6d | %11d | %6d | %6d | %7.1f\n", 3, 104162, 104082, 80, 512.8);
    
    printf("\n");
    printf("  💡 Most allocations stay on their origin thread (good!)\n");
    printf("     Remote frees: %s2.3%%%s (low is better)\n", 
           COLOR_GREEN, COLOR_RESET);
    printf("\n");
}

void print_security_features(void) {
    printf(COLOR_CYAN "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("  Security Features Status\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n" COLOR_RESET);
    
    printf("\n");
    printf("  %s✓%s CRC32 checksums:       %sENABLED%s\n", 
           COLOR_GREEN, COLOR_RESET, COLOR_GREEN, COLOR_RESET);
    printf("  %s✓%s Safe-linking:          %sENABLED%s\n", 
           COLOR_GREEN, COLOR_RESET, COLOR_GREEN, COLOR_RESET);
    printf("  %s✓%s Double-free detection: %sENABLED%s\n", 
           COLOR_GREEN, COLOR_RESET, COLOR_GREEN, COLOR_RESET);
    printf("  %s✓%s Random entropy:        %s0x%016lx%s\n", 
           COLOR_GREEN, COLOR_RESET, COLOR_CYAN, 0x1a2b3c4d5e6f7890UL, COLOR_RESET);
    
    printf("\n");
    printf("  Security events:\n");
    printf("    Checksum failures: %s0%s\n", COLOR_GREEN, COLOR_RESET);
    printf("    Double-frees:      %s0%s\n", COLOR_GREEN, COLOR_RESET);
    printf("    Invalid pointers:  %s0%s\n", COLOR_GREEN, COLOR_RESET);
    printf("\n");
}

void print_health_score(void) {
    printf(COLOR_CYAN "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("  Overall Health Score\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n" COLOR_RESET);
    
    printf("\n");
    
    int score = 92;
    const char* color = COLOR_GREEN;
    const char* grade = "A";
    const char* emoji = "🎉";
    
    if (score < 90) { color = COLOR_YELLOW; grade = "B"; emoji = "👍"; }
    if (score < 80) { color = COLOR_YELLOW; grade = "C"; emoji = "⚠️"; }
    if (score < 70) { color = COLOR_RED; grade = "D"; emoji = "❌"; }
    
    printf("  %s Score: %d/100 (Grade: %s)%s %s\n\n", 
           color, score, grade, COLOR_RESET, emoji);
    
    printf("  Breakdown:\n");
    printf("    Memory efficiency:   %s95/100%s\n", COLOR_GREEN, COLOR_RESET);
    printf("    Fragmentation:       %s88/100%s\n", COLOR_GREEN, COLOR_RESET);
    printf("    Locality:            %s92/100%s\n", COLOR_GREEN, COLOR_RESET);
    printf("    Security:            %s100/100%s\n", COLOR_GREEN, COLOR_RESET);
    
    printf("\n");
    printf("  Recommendations:\n");
    printf("    • Consider purging unused blocks\n");
    printf("    • Size class 129-256B underutilized\n");
    printf("\n");
}

void live_monitor(void) {
    printf("Starting live monitor (Ctrl+C to stop)...\n\n");
    
    while (1) {
        system("clear");
        print_header();
        print_memory_usage();
        print_allocation_stats();
        
        printf(COLOR_BLUE "  [Press Ctrl+C to exit]" COLOR_RESET "\n");
        
        sleep(2);
    }
}

int main(int argc, char** argv) {
    // Enable thousands separators
    setenv("LC_NUMERIC", "en_US.UTF-8", 1);
    
    if (argc > 1 && strcmp(argv[1], "live") == 0) {
        live_monitor();
    } else if (argc > 1 && strcmp(argv[1], "quick") == 0) {
        print_header();
        print_memory_usage();
        print_allocation_stats();
    } else {
        // Full report
        print_header();
        print_memory_usage();
        print_allocation_stats();
        print_size_class_distribution();
        print_thread_stats();
        print_security_features();
        print_health_score();
    }
    
    return 0;
}
