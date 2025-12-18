#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    printf("🧪 ApexAlloc Basic Allocation Test\n\n");
    
    // Test 1: Simple allocation
    printf("Test 1: Simple allocation...\n");
    void* ptr1 = malloc(100);
    if (ptr1) {
        printf("  ✅ malloc(100) = %p\n", ptr1);
        memset(ptr1, 0xAA, 100);  // Write to it
        free(ptr1);
        printf("  ✅ free() succeeded\n");
    } else {
        printf("  ❌ malloc(100) returned NULL\n");
        return 1;
    }
    
    // Test 2: Various sizes
    printf("\nTest 2: Various sizes...\n");
    void* ptrs[10];
    size_t sizes[] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};
    
    for (int i = 0; i < 10; i++) {
        ptrs[i] = malloc(sizes[i]);
        if (ptrs[i]) {
            printf("  ✅ malloc(%zu) = %p\n", sizes[i], ptrs[i]);
            memset(ptrs[i], i, sizes[i]);  // Write pattern
        } else {
            printf("  ❌ malloc(%zu) returned NULL\n", sizes[i]);
            return 1;
        }
    }
    
    // Free them
    printf("\n  Freeing allocations...\n");
    for (int i = 0; i < 10; i++) {
        free(ptrs[i]);
    }
    printf("  ✅ All frees succeeded\n");
    
    // Test 3: calloc
    printf("\nTest 3: calloc()...\n");
    int* arr = (int*)calloc(100, sizeof(int));
    if (arr) {
        printf("  ✅ calloc(100, %zu) = %p\n", sizeof(int), arr);
        
        // Verify it's zeroed
        int all_zero = 1;
        for (int i = 0; i < 100; i++) {
            if (arr[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        
        if (all_zero) {
            printf("  ✅ Memory is properly zeroed\n");
        } else {
            printf("  ❌ Memory not zeroed\n");
        }
        
        free(arr);
    } else {
        printf("  ❌ calloc() returned NULL\n");
        return 1;
    }
    
    // Test 4: realloc
    printf("\nTest 4: realloc()...\n");
    void* ptr = malloc(100);
    printf("  ✅ malloc(100) = %p\n", ptr);
    
    void* ptr2 = realloc(ptr, 200);
    if (ptr2) {
        printf("  ✅ realloc(%p, 200) = %p\n", ptr, ptr2);
        free(ptr2);
    } else {
        printf("  ❌ realloc() returned NULL\n");
        free(ptr);
        return 1;
    }
    
    // Test 5: Many small allocations
    printf("\nTest 5: Many small allocations...\n");
    void* small_ptrs[1000];
    for (int i = 0; i < 1000; i++) {
        small_ptrs[i] = malloc(32);
        if (!small_ptrs[i]) {
            printf("  ❌ malloc failed at iteration %d\n", i);
            return 1;
        }
    }
    printf("  ✅ Allocated 1000 x 32-byte blocks\n");
    
    for (int i = 0; i < 1000; i++) {
        free(small_ptrs[i]);
    }
    printf("  ✅ Freed all 1000 blocks\n");
    
    printf("\n🎉 All tests passed!\n");
    return 0;
}
