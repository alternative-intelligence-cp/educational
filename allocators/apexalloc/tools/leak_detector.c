/**
 * ApexAlloc Simple Leak Detector
 * 
 * Wraps malloc/free to track allocations and detect leaks.
 * Educational tool - not production quality!
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <pthread.h>
#include <execinfo.h>

#define MAX_ALLOCS 100000
#define MAX_STACK_DEPTH 10

typedef struct {
    void* ptr;
    size_t size;
    void* stack[MAX_STACK_DEPTH];
    int stack_depth;
    int active;
} alloc_record_t;

static alloc_record_t g_allocs[MAX_ALLOCS];
static int g_alloc_count = 0;
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
static int g_initialized = 0;

// Real malloc/free pointers
static void* (*real_malloc)(size_t) = NULL;
static void (*real_free)(void*) = NULL;
static void* (*real_calloc)(size_t, size_t) = NULL;
static void* (*real_realloc)(void*, size_t) = NULL;

// Statistics
static size_t g_total_allocated = 0;
static size_t g_total_freed = 0;
static size_t g_current_allocated = 0;
static size_t g_peak_allocated = 0;
static int g_malloc_count = 0;
static int g_free_count = 0;

// ANSI colors
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_RED     "\033[31m"
#define COLOR_CYAN    "\033[36m"

static void init(void) {
    if (g_initialized) return;
    
    real_malloc = dlsym(RTLD_NEXT, "malloc");
    real_free = dlsym(RTLD_NEXT, "free");
    real_calloc = dlsym(RTLD_NEXT, "calloc");
    real_realloc = dlsym(RTLD_NEXT, "realloc");
    
    g_initialized = 1;
}

static void record_allocation(void* ptr, size_t size) {
    pthread_mutex_lock(&g_mutex);
    
    if (g_alloc_count >= MAX_ALLOCS) {
        fprintf(stderr, "WARNING: Allocation tracking table full!\n");
        pthread_mutex_unlock(&g_mutex);
        return;
    }
    
    int idx = g_alloc_count++;
    g_allocs[idx].ptr = ptr;
    g_allocs[idx].size = size;
    g_allocs[idx].active = 1;
    
    // Capture stack trace
    g_allocs[idx].stack_depth = backtrace(g_allocs[idx].stack, MAX_STACK_DEPTH);
    
    // Update statistics
    g_malloc_count++;
    g_total_allocated += size;
    g_current_allocated += size;
    if (g_current_allocated > g_peak_allocated) {
        g_peak_allocated = g_current_allocated;
    }
    
    pthread_mutex_unlock(&g_mutex);
}

static void record_free(void* ptr) {
    pthread_mutex_lock(&g_mutex);
    
    for (int i = 0; i < g_alloc_count; i++) {
        if (g_allocs[i].ptr == ptr && g_allocs[i].active) {
            g_allocs[i].active = 0;
            g_free_count++;
            g_total_freed += g_allocs[i].size;
            g_current_allocated -= g_allocs[i].size;
            pthread_mutex_unlock(&g_mutex);
            return;
        }
    }
    
    pthread_mutex_unlock(&g_mutex);
}

// Wrapped functions
void* malloc(size_t size) {
    if (!g_initialized) init();
    
    void* ptr = real_malloc(size);
    if (ptr) {
        record_allocation(ptr, size);
    }
    return ptr;
}

void free(void* ptr) {
    if (!ptr) return;
    if (!g_initialized) init();
    
    record_free(ptr);
    real_free(ptr);
}

void* calloc(size_t nmemb, size_t size) {
    if (!g_initialized) init();
    
    void* ptr = real_calloc(nmemb, size);
    if (ptr) {
        record_allocation(ptr, nmemb * size);
    }
    return ptr;
}

void* realloc(void* ptr, size_t size) {
    if (!g_initialized) init();
    
    if (ptr) {
        record_free(ptr);
    }
    
    void* new_ptr = real_realloc(ptr, size);
    if (new_ptr) {
        record_allocation(new_ptr, size);
    }
    return new_ptr;
}

// Report generation
static void print_leak_report(void) {
    pthread_mutex_lock(&g_mutex);
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           Memory Leak Detection Report                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    // Summary statistics
    printf(COLOR_CYAN "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("  Summary Statistics\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n" COLOR_RESET);
    printf("\n");
    printf("  Total allocations:   %d\n", g_malloc_count);
    printf("  Total frees:         %d\n", g_free_count);
    printf("  Total allocated:     %.2f KB\n", g_total_allocated / 1024.0);
    printf("  Total freed:         %.2f KB\n", g_total_freed / 1024.0);
    printf("  Peak memory:         %.2f KB\n", g_peak_allocated / 1024.0);
    printf("\n");
    
    // Find leaks
    int leak_count = 0;
    size_t leaked_bytes = 0;
    
    for (int i = 0; i < g_alloc_count; i++) {
        if (g_allocs[i].active) {
            leak_count++;
            leaked_bytes += g_allocs[i].size;
        }
    }
    
    if (leak_count == 0) {
        printf(COLOR_GREEN);
        printf("  ✅ No memory leaks detected!\n");
        printf(COLOR_RESET);
        printf("\n");
        pthread_mutex_unlock(&g_mutex);
        return;
    }
    
    // Report leaks
    printf(COLOR_RED);
    printf("  ⚠️  LEAKS DETECTED: %d allocations, %.2f KB\n", 
           leak_count, leaked_bytes / 1024.0);
    printf(COLOR_RESET);
    printf("\n");
    
    printf(COLOR_CYAN "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("  Leak Details\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n" COLOR_RESET);
    printf("\n");
    
    int shown = 0;
    for (int i = 0; i < g_alloc_count && shown < 10; i++) {
        if (!g_allocs[i].active) continue;
        
        printf(COLOR_YELLOW "  Leak #%d:" COLOR_RESET "\n", shown + 1);
        printf("    Address: %p\n", g_allocs[i].ptr);
        printf("    Size:    %zu bytes\n", g_allocs[i].size);
        printf("    Stack trace:\n");
        
        // Print stack trace
        char** symbols = backtrace_symbols(g_allocs[i].stack, g_allocs[i].stack_depth);
        if (symbols) {
            for (int j = 0; j < g_allocs[i].stack_depth; j++) {
                printf("      %s\n", symbols[j]);
            }
            free(symbols);
        }
        printf("\n");
        
        shown++;
    }
    
    if (leak_count > 10) {
        printf("  ... and %d more leaks\n\n", leak_count - 10);
    }
    
    pthread_mutex_unlock(&g_mutex);
}

// Constructor/destructor
__attribute__((constructor))
static void leak_detector_init(void) {
    init();
    printf(COLOR_CYAN);
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  ApexAlloc Leak Detector Initialized\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf(COLOR_RESET "\n");
}

__attribute__((destructor))
static void leak_detector_finish(void) {
    print_leak_report();
}
