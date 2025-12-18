/**
 * Bug Example 4: Memory Leaks
 * 
 * WHAT'S WRONG:
 * Allocating memory and never freeing it. Over time, the program
 * consumes more and more memory until it crashes or the system
 * runs out of memory.
 * 
 * SYMPTOMS:
 * - Increasing memory usage over time
 * - OOM (Out Of Memory) killer on Linux
 * - Program slows down as memory fills
 * - Works fine for short runs, fails on long runs
 * 
 * HOW TO DETECT:
 * - LD_PRELOAD=./build/libleak.so ./program
 * - valgrind --leak-check=full ./program
 * - ApexAlloc leak detector (our tool!)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// BUGGY VERSION: Simple leak
void buggy_simple_leak(void) {
    printf("\n🐛 BUGGY: Simple Memory Leak\n");
    printf("─────────────────────────────────────────────\n");
    
    char* data = malloc(1024);
    strcpy(data, "This memory will leak!");
    
    printf("✓ Allocated 1024 bytes\n");
    printf("❌ BUG: Exiting function without free()\n");
    
    // BUG: No free() call!
}

void fixed_simple_leak(void) {
    printf("\n✅ FIXED: Always Free\n");
    printf("─────────────────────────────────────────────\n");
    
    char* data = malloc(1024);
    strcpy(data, "This memory will be freed!");
    
    printf("✓ Allocated 1024 bytes\n");
    
    // FIX: Free before returning
    free(data);
    printf("✓ Freed memory before returning\n");
}

// REAL SCENARIO: Leak in loop
void buggy_loop_leak(void) {
    printf("\n🐛 DANGEROUS: Leak in Loop\n");
    printf("─────────────────────────────────────────────\n");
    
    printf("Allocating 1000 x 1MB chunks without freeing...\n");
    
    for (int i = 0; i < 1000; i++) {
        char* buffer = malloc(1024 * 1024);  // 1 MB
        
        // Do something with it
        if (buffer) {
            buffer[0] = 'X';
        }
        
        // BUG: Never freed! Leaks 1 GB total!
        
        if (i % 100 == 0) {
            printf("  Iteration %d (leaked %d MB so far)\n", i, i);
        }
    }
    
    printf("❌ BUG: Leaked 1 GB of memory!\n");
}

void fixed_loop_leak(void) {
    printf("\n✅ FIXED: Free in Loop\n");
    printf("─────────────────────────────────────────────\n");
    
    printf("Allocating and freeing 1000 x 1MB chunks...\n");
    
    for (int i = 0; i < 1000; i++) {
        char* buffer = malloc(1024 * 1024);
        
        if (buffer) {
            buffer[0] = 'X';
            free(buffer);  // FIX: Free it!
        }
        
        if (i % 100 == 0) {
            printf("  Iteration %d (no leak!)\n", i);
        }
    }
    
    printf("✓ No memory leaked!\n");
}

// SCENARIO: Conditional leak (hardest to find!)
int buggy_conditional_leak(int success) {
    printf("\n🐛 TRICKY: Conditional Leak\n");
    printf("─────────────────────────────────────────────\n");
    
    char* temp = malloc(512);
    char* result = malloc(1024);
    
    if (!temp || !result) {
        // BUG: Only free one of them on error!
        free(temp);
        return -1;  // Leaks 'result'!
    }
    
    // Do work
    strcpy(temp, "temporary");
    strcpy(result, "result");
    
    if (success) {
        free(temp);
        free(result);
        printf("✓ Success path: freed both\n");
        return 0;
    } else {
        free(temp);
        // BUG: Forgot to free 'result' on failure path!
        printf("❌ BUG: Error path leaked 'result'\n");
        return -1;
    }
}

int fixed_conditional_leak(int success) {
    printf("\n✅ FIXED: Goto Cleanup Pattern\n");
    printf("─────────────────────────────────────────────\n");
    
    char* temp = NULL;
    char* result = NULL;
    int ret = -1;
    
    temp = malloc(512);
    result = malloc(1024);
    
    if (!temp || !result) {
        goto cleanup;  // FIX: Single cleanup path
    }
    
    // Do work
    strcpy(temp, "temporary");
    strcpy(result, "result");
    
    if (success) {
        ret = 0;
    } else {
        ret = -1;
    }
    
cleanup:
    // FIX: Always free both (free(NULL) is safe)
    free(temp);
    free(result);
    
    printf("✓ All paths free memory\n");
    return ret;
}

// SCENARIO: Lost pointer (reassignment)
void buggy_lost_pointer(void) {
    printf("\n🐛 SNEAKY: Lost Pointer\n");
    printf("─────────────────────────────────────────────\n");
    
    char* data = malloc(1024);
    strcpy(data, "Original data");
    
    printf("✓ Allocated at %p\n", (void*)data);
    
    // BUG: Reassigning without freeing!
    data = malloc(2048);
    strcpy(data, "New data");
    
    printf("❌ BUG: Reallocated at %p (lost original pointer!)\n", (void*)data);
    
    free(data);  // Only frees the second allocation
    printf("   Freed new allocation, but original is leaked\n");
}

void fixed_lost_pointer(void) {
    printf("\n✅ FIXED: Free Before Reassigning\n");
    printf("─────────────────────────────────────────────\n");
    
    char* data = malloc(1024);
    strcpy(data, "Original data");
    
    printf("✓ Allocated at %p\n", (void*)data);
    
    // FIX: Free old pointer before reassigning
    free(data);
    
    data = malloc(2048);
    strcpy(data, "New data");
    
    printf("✓ Freed old, allocated new at %p\n", (void*)data);
    
    free(data);
    printf("✓ No leaks!\n");
}

// BETTER FIX: Use realloc
void better_realloc_pattern(void) {
    printf("\n✅ BETTER: Use realloc\n");
    printf("─────────────────────────────────────────────\n");
    
    char* data = malloc(1024);
    strcpy(data, "Original data");
    
    printf("✓ Allocated at %p\n", (void*)data);
    
    // BEST: Use realloc (handles freeing automatically)
    char* new_data = realloc(data, 2048);
    if (new_data) {
        data = new_data;
        strcpy(data, "Expanded data");
        printf("✓ Reallocated to %p (old automatically freed)\n", (void*)data);
    }
    
    free(data);
    printf("✓ Clean!\n");
}

// SCENARIO: Struct with pointers (common mistake)
typedef struct {
    char* name;
    int* values;
    int count;
} data_struct_t;

void buggy_struct_leak(void) {
    printf("\n🐛 COMMON: Struct Member Leak\n");
    printf("─────────────────────────────────────────────\n");
    
    data_struct_t* obj = malloc(sizeof(data_struct_t));
    obj->name = malloc(50);
    obj->values = malloc(100 * sizeof(int));
    obj->count = 100;
    
    strcpy(obj->name, "MyObject");
    
    printf("✓ Created object with dynamic members\n");
    
    // BUG: Only freeing the struct, not the members!
    free(obj);
    
    printf("❌ BUG: Freed struct but leaked name and values!\n");
}

void fixed_struct_leak(void) {
    printf("\n✅ FIXED: Free Members First\n");
    printf("─────────────────────────────────────────────\n");
    
    data_struct_t* obj = malloc(sizeof(data_struct_t));
    obj->name = malloc(50);
    obj->values = malloc(100 * sizeof(int));
    obj->count = 100;
    
    strcpy(obj->name, "MyObject");
    
    printf("✓ Created object with dynamic members\n");
    
    // FIX: Free members first, then the struct
    free(obj->name);
    free(obj->values);
    free(obj);
    
    printf("✓ Freed everything in correct order\n");
}

int main(void) {
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  Bug Example 4: Memory Leaks\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    printf("\n💡 Run with leak detector to see reports:\n");
    printf("   LD_PRELOAD=./build/libleak.so ./bug_example\n\n");
    
    buggy_simple_leak();
    fixed_simple_leak();
    
    // Uncomment to see massive leak:
    // buggy_loop_leak();
    fixed_loop_leak();
    
    buggy_conditional_leak(0);  // Error path
    fixed_conditional_leak(0);
    
    buggy_lost_pointer();
    fixed_lost_pointer();
    better_realloc_pattern();
    
    buggy_struct_leak();
    fixed_struct_leak();
    
    printf("\n📚 KEY LESSONS:\n");
    printf("   1. Every malloc() needs a matching free()\n");
    printf("   2. Free memory in loops, not after the loop\n");
    printf("   3. Use goto cleanup pattern for multiple exit points\n");
    printf("   4. Free before reassigning pointers\n");
    printf("   5. Free struct members before freeing the struct\n");
    printf("   6. Use leak detectors during development!\n");
    printf("   7. Consider RAII in C++ or defer in Go\n\n");
    
    return 0;
}
