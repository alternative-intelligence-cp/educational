#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main() {
    clock_t start, end;
    double cpu_time_used;
    
    printf("🏁 Standard Printf Performance Test\n");
    printf("===================================\n\n");
    
    start = clock();
    
    // Intensive printf operations
    for (int i = 0; i < 1000; i++) {
        printf("Test %d: %x %o %d\n", i, i, i, i);
    }
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    
    printf("\n⏱️  Standard printf time: %f seconds\n", cpu_time_used);
    
    return 0;
}
