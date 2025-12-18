/**
 * ApexAlloc Multi-threaded Benchmark
 * 
 * Tests allocator performance under concurrent load.
 * This is where thread-local caching really matters!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define MAX_THREADS 32

typedef struct {
    int thread_id;
    size_t iterations;
    size_t size;
    double elapsed_time;
} thread_arg_t;

static double get_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// Worker: allocate and free in a loop
void* worker_alloc_free(void* arg) {
    thread_arg_t* targ = (thread_arg_t*)arg;
    
    double start = get_time();
    
    for (size_t i = 0; i < targ->iterations; i++) {
        void* ptr = malloc(targ->size);
        if (ptr) {
            memset(ptr, targ->thread_id, targ->size);
            free(ptr);
        }
    }
    
    targ->elapsed_time = get_time() - start;
    
    return NULL;
}

// Worker: allocate, hold, then free all
void* worker_batch(void* arg) {
    thread_arg_t* targ = (thread_arg_t*)arg;
    
    void** ptrs = malloc(targ->iterations * sizeof(void*));
    
    double start = get_time();
    
    // Allocate all
    for (size_t i = 0; i < targ->iterations; i++) {
        ptrs[i] = malloc(targ->size);
        if (ptrs[i]) {
            memset(ptrs[i], targ->thread_id, targ->size);
        }
    }
    
    // Free all
    for (size_t i = 0; i < targ->iterations; i++) {
        free(ptrs[i]);
    }
    
    targ->elapsed_time = get_time() - start;
    
    free(ptrs);
    return NULL;
}

// Benchmark: concurrent alloc/free
void bench_concurrent(int num_threads, size_t iterations, size_t size) {
    printf("\n📊 Concurrent Alloc/Free\n");
    printf("  Threads: %d, Iterations/thread: %zu, Size: %zu bytes\n", 
           num_threads, iterations, size);
    
    pthread_t threads[MAX_THREADS];
    thread_arg_t args[MAX_THREADS];
    
    double start = get_time();
    
    // Create threads
    for (int i = 0; i < num_threads; i++) {
        args[i].thread_id = i;
        args[i].iterations = iterations;
        args[i].size = size;
        pthread_create(&threads[i], NULL, worker_alloc_free, &args[i]);
    }
    
    // Join threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    double total_time = get_time() - start;
    double total_ops = num_threads * iterations;
    
    printf("  Total time: %.3f ms\n", total_time * 1000);
    printf("  Throughput: %.0f ops/sec\n", total_ops / total_time);
    printf("  Per-thread: %.0f ns/op\n", (total_time / total_ops) * 1e9);
    
    // Print per-thread times
    printf("  Thread times (ms): ");
    for (int i = 0; i < num_threads; i++) {
        printf("%.1f ", args[i].elapsed_time * 1000);
    }
    printf("\n");
}

// Benchmark: batch operations (more contention)
void bench_batch(int num_threads, size_t iterations, size_t size) {
    printf("\n📊 Batch Operations (Hold then Free)\n");
    printf("  Threads: %d, Batch size: %zu, Size: %zu bytes\n", 
           num_threads, iterations, size);
    
    pthread_t threads[MAX_THREADS];
    thread_arg_t args[MAX_THREADS];
    
    double start = get_time();
    
    // Create threads
    for (int i = 0; i < num_threads; i++) {
        args[i].thread_id = i;
        args[i].iterations = iterations;
        args[i].size = size;
        pthread_create(&threads[i], NULL, worker_batch, &args[i]);
    }
    
    // Join threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    double total_time = get_time() - start;
    double total_ops = num_threads * iterations;
    
    printf("  Total time: %.3f ms\n", total_time * 1000);
    printf("  Throughput: %.0f ops/sec\n", total_ops / total_time);
    printf("  Peak memory: ~%.1f MB\n", 
           (total_ops * size) / (1024.0 * 1024.0));
}

// Scalability test: measure speedup with increasing thread count
void bench_scalability(size_t iterations, size_t size) {
    printf("\n📊 Scalability Test (Speedup Analysis)\n");
    printf("  Iterations/thread: %zu, Size: %zu bytes\n", iterations, size);
    printf("\n  Threads | Time (ms) | Speedup | Efficiency\n");
    printf("  --------|-----------|---------|------------\n");
    
    double baseline_time = 0;
    
    for (int num_threads = 1; num_threads <= 16; num_threads *= 2) {
        pthread_t threads[MAX_THREADS];
        thread_arg_t args[MAX_THREADS];
        
        double start = get_time();
        
        for (int i = 0; i < num_threads; i++) {
            args[i].thread_id = i;
            args[i].iterations = iterations;
            args[i].size = size;
            pthread_create(&threads[i], NULL, worker_alloc_free, &args[i]);
        }
        
        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }
        
        double total_time = get_time() - start;
        
        if (num_threads == 1) {
            baseline_time = total_time;
        }
        
        double speedup = baseline_time / total_time;
        double efficiency = (speedup / num_threads) * 100.0;
        
        printf("  %7d | %9.3f | %7.2fx | %9.1f%%\n", 
               num_threads, total_time * 1000, speedup, efficiency);
    }
}

int main(int argc, char** argv) {
    printf("═══════════════════════════════════════════════\n");
    printf("  ApexAlloc Multi-threaded Benchmark\n");
    printf("═══════════════════════════════════════════════\n");
    
    // Get number of CPU cores
    int num_cpus = sysconf(_SC_NPROCESSORS_ONLN);
    printf("\nSystem: %d CPU cores detected\n", num_cpus);
    
    // Parse options
    int quick = (argc > 1 && strcmp(argv[1], "quick") == 0);
    size_t iterations = quick ? 10000 : 50000;
    
    printf("Mode: %s (%zu iterations/thread)\n", 
           quick ? "Quick" : "Full", iterations);
    
    // Run benchmarks with different thread counts
    int thread_counts[] = {1, 2, 4, 8};
    int num_tests = 4;
    
    for (int i = 0; i < num_tests; i++) {
        int threads = thread_counts[i];
        if (threads > num_cpus * 2) continue;  // Skip if too many threads
        
        bench_concurrent(threads, iterations, 64);
        bench_concurrent(threads, iterations, 256);
    }
    
    // Batch operations
    bench_batch(4, iterations / 10, 1024);
    
    // Scalability
    if (!quick) {
        bench_scalability(iterations / 2, 128);
    }
    
    printf("\n═══════════════════════════════════════════════\n");
    printf("  Benchmark Complete!\n");
    printf("═══════════════════════════════════════════════\n");
    
    return 0;
}
