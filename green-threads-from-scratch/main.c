#include "green_thread.h"
#include <stdio.h>
#include <unistd.h>

void worker_a(void) {
    for (int i = 0; i < 5; i++) {
        printf("[Thread A] Running iteration %d\n", i);
        // Simulate some work, then cooperatively yield
        usleep(100000); 
        thread_yield();
    }
    printf("[Thread A] Finished!\n");
}

void worker_b(void) {
    for (int i = 0; i < 3; i++) {
        printf("    [Thread B] Running iteration %d\n", i);
        usleep(150000);
        thread_yield();
    }
    printf("    [Thread B] Finished!\n");
}

void worker_c(void) {
    for (int i = 0; i < 7; i++) {
        printf("        [Thread C] Running iteration %d\n", i);
        usleep(50000);
        thread_yield();
    }
    printf("        [Thread C] Finished!\n");
}

int main(void) {
    printf("=== Green Threads Educational Demo ===\n\n");
    printf("Initializing thread system...\n");
    thread_init();

    printf("Creating threads...\n");
    int id_a = thread_create(worker_a);
    int id_b = thread_create(worker_b);
    int id_c = thread_create(worker_c);

    printf("Created Thread A (id: %d)\n", id_a);
    printf("Created Thread B (id: %d)\n", id_b);
    printf("Created Thread C (id: %d)\n", id_c);
    
    printf("\nStarting main execution loop (yielding to workers)...\n\n");

    // The main thread also participates in the scheduling!
    // We can yield in a loop until all other threads are dead.
    thread_wait_all();

    printf("\nAll worker threads finished. Exiting main.\n");
    return 0;
}
