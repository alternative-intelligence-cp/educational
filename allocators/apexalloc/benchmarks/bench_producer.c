/**
 * ApexAlloc Producer-Consumer Benchmark
 * 
 * Tests the pathological case where one thread allocates
 * and another thread frees (remote free scenario).
 * 
 * This is where message-passing allocators shine!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define QUEUE_SIZE 10000

typedef struct {
    void* items[QUEUE_SIZE];
    int head;
    int tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} work_queue_t;

static double get_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void queue_init(work_queue_t* q) {
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
}

void queue_push(work_queue_t* q, void* item) {
    pthread_mutex_lock(&q->mutex);
    
    while (q->count >= QUEUE_SIZE) {
        pthread_cond_wait(&q->not_full, &q->mutex);
    }
    
    q->items[q->tail] = item;
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->count++;
    
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
}

void* queue_pop(work_queue_t* q) {
    pthread_mutex_lock(&q->mutex);
    
    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }
    
    void* item = q->items[q->head];
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->count--;
    
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);
    
    return item;
}

typedef struct {
    work_queue_t* queue;
    size_t iterations;
    size_t size;
    int done;
} worker_arg_t;

// Producer: allocate and send to consumer
void* producer(void* arg) {
    worker_arg_t* warg = (worker_arg_t*)arg;
    
    for (size_t i = 0; i < warg->iterations; i++) {
        void* ptr = malloc(warg->size);
        if (ptr) {
            memset(ptr, 0xAA, warg->size);
            queue_push(warg->queue, ptr);
        }
    }
    
    // Signal done
    warg->done = 1;
    
    return NULL;
}

// Consumer: receive and free
void* consumer(void* arg) {
    worker_arg_t* warg = (worker_arg_t*)arg;
    size_t processed = 0;
    
    while (processed < warg->iterations || !warg->done) {
        pthread_mutex_lock(&warg->queue->mutex);
        if (warg->queue->count == 0 && warg->done && processed >= warg->iterations) {
            pthread_mutex_unlock(&warg->queue->mutex);
            break;
        }
        pthread_mutex_unlock(&warg->queue->mutex);
        
        if (warg->queue->count > 0) {
            void* ptr = queue_pop(warg->queue);
            if (ptr) {
                free(ptr);
                processed++;
            }
        } else {
            usleep(100);  // Brief sleep if queue empty
        }
    }
    
    return NULL;
}

// Benchmark: single producer, single consumer
void bench_spsc(size_t iterations, size_t size) {
    printf("\n📊 Single Producer, Single Consumer\n");
    printf("  Iterations: %zu, Size: %zu bytes\n", iterations, size);
    
    work_queue_t queue;
    queue_init(&queue);
    
    worker_arg_t arg = {
        .queue = &queue,
        .iterations = iterations,
        .size = size,
        .done = 0
    };
    
    pthread_t prod_thread, cons_thread;
    
    double start = get_time();
    
    pthread_create(&prod_thread, NULL, producer, &arg);
    pthread_create(&cons_thread, NULL, consumer, &arg);
    
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);
    
    double total_time = get_time() - start;
    
    printf("  Total time: %.3f ms\n", total_time * 1000);
    printf("  Throughput: %.0f items/sec\n", iterations / total_time);
    printf("  Per-item: %.0f ns\n", (total_time / iterations) * 1e9);
}

// Benchmark: multiple producers, multiple consumers
void bench_mpmc(int num_producers, int num_consumers, 
                size_t iterations_per_producer, size_t size) {
    printf("\n📊 Multi Producer, Multi Consumer\n");
    printf("  Producers: %d, Consumers: %d\n", num_producers, num_consumers);
    printf("  Iterations/producer: %zu, Size: %zu bytes\n", 
           iterations_per_producer, size);
    
    work_queue_t queue;
    queue_init(&queue);
    
    worker_arg_t* prod_args = malloc(num_producers * sizeof(worker_arg_t));
    worker_arg_t* cons_args = malloc(num_consumers * sizeof(worker_arg_t));
    
    pthread_t* prod_threads = malloc(num_producers * sizeof(pthread_t));
    pthread_t* cons_threads = malloc(num_consumers * sizeof(pthread_t));
    
    // Initialize args
    for (int i = 0; i < num_producers; i++) {
        prod_args[i].queue = &queue;
        prod_args[i].iterations = iterations_per_producer;
        prod_args[i].size = size;
        prod_args[i].done = 0;
    }
    
    size_t total_items = num_producers * iterations_per_producer;
    size_t items_per_consumer = total_items / num_consumers;
    
    for (int i = 0; i < num_consumers; i++) {
        cons_args[i].queue = &queue;
        cons_args[i].iterations = items_per_consumer;
        cons_args[i].size = size;
        cons_args[i].done = 0;
    }
    
    double start = get_time();
    
    // Create consumers first
    for (int i = 0; i < num_consumers; i++) {
        pthread_create(&cons_threads[i], NULL, consumer, &cons_args[i]);
    }
    
    // Create producers
    for (int i = 0; i < num_producers; i++) {
        pthread_create(&prod_threads[i], NULL, producer, &prod_args[i]);
    }
    
    // Wait for producers
    for (int i = 0; i < num_producers; i++) {
        pthread_join(prod_threads[i], NULL);
    }
    
    // Signal consumers that production is done
    for (int i = 0; i < num_consumers; i++) {
        cons_args[i].done = 1;
    }
    
    // Wait for consumers
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(cons_threads[i], NULL);
    }
    
    double total_time = get_time() - start;
    
    printf("  Total time: %.3f ms\n", total_time * 1000);
    printf("  Throughput: %.0f items/sec\n", total_items / total_time);
    printf("  Remote frees: %zu (%.1f%%)\n", 
           total_items, 100.0);  // All frees are remote in this pattern
    
    free(prod_args);
    free(cons_args);
    free(prod_threads);
    free(cons_threads);
}

int main(int argc, char** argv) {
    printf("═══════════════════════════════════════════════\n");
    printf("  ApexAlloc Producer-Consumer Benchmark\n");
    printf("═══════════════════════════════════════════════\n");
    
    printf("\nThis benchmark tests REMOTE FREE performance:\n");
    printf("  One thread allocates, another frees.\n");
    printf("  This is the hardest workload for allocators!\n");
    
    // Parse options
    int quick = (argc > 1 && strcmp(argv[1], "quick") == 0);
    size_t iterations = quick ? 10000 : 50000;
    
    printf("\nMode: %s (%zu iterations)\n", 
           quick ? "Quick" : "Full", iterations);
    
    // Single producer/consumer tests
    bench_spsc(iterations, 64);
    bench_spsc(iterations, 256);
    bench_spsc(iterations, 1024);
    
    // Multi producer/consumer tests
    bench_mpmc(2, 2, iterations / 2, 128);
    bench_mpmc(4, 4, iterations / 4, 256);
    bench_mpmc(2, 4, iterations / 2, 512);  // Asymmetric
    
    printf("\n═══════════════════════════════════════════════\n");
    printf("  Benchmark Complete!\n");
    printf("\n  💡 Tip: Compare with system malloc:\n");
    printf("     ./bench_producer\n");
    printf("     LD_PRELOAD=./build/libapexalloc.so ./bench_producer\n");
    printf("═══════════════════════════════════════════════\n");
    
    return 0;
}
