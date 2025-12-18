/**
 * Bug Example 2: Double-Free
 * 
 * WHAT'S WRONG:
 * Calling free() twice on the same pointer corrupts the allocator's
 * internal data structures. This is a serious security vulnerability!
 * 
 * SYMPTOMS:
 * - Crashes in malloc/free (not your code!)
 * - Heap corruption
 * - Exploitable vulnerability
 * 
 * HOW APEXALLOC HELPS:
 * - Double-free detection with chunk state tracking
 * - CRC32 checksums detect corruption
 * - Aborts immediately on double-free
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// BUGGY VERSION
void buggy_simple(void) {
    printf("\n🐛 BUGGY: Simple Double-Free\n");
    printf("─────────────────────────────────────────────\n");
    
    char* data = malloc(100);
    strcpy(data, "Hello");
    
    printf("✓ Allocated: %s\n", data);
    
    free(data);
    printf("✓ Freed once\n");
    
    // BUG: Freeing again!
    free(data);  // KABOOM!
    printf("❌ BUG: Freed twice (if you see this, detection failed!)\n");
}

// REAL-WORLD SCENARIO: Multiple code paths freeing the same pointer
void cleanup_buggy(char* buffer, int error_code) {
    if (error_code != 0) {
        printf("  Error path: freeing buffer\n");
        free(buffer);
        return;
    }
    
    printf("  Success path: freeing buffer\n");
    free(buffer);  // BUG: Both paths free!
}

void buggy_multiple_paths(void) {
    printf("\n🐛 REAL SCENARIO: Multiple Free Paths\n");
    printf("─────────────────────────────────────────────\n");
    
    char* buffer = malloc(256);
    strcpy(buffer, "Test data");
    
    // Simulate error
    printf("Simulating error condition:\n");
    cleanup_buggy(buffer, 1);
    
    // Later, someone calls cleanup again (forgot it was freed)
    printf("Calling cleanup again:\n");
    cleanup_buggy(buffer, 0);  // KABOOM!
}

// FIXED VERSION
void cleanup_fixed(char** buffer_ptr, int error_code) {
    if (*buffer_ptr == NULL) {
        printf("  Buffer already freed, skipping\n");
        return;
    }
    
    if (error_code != 0) {
        printf("  Error path: freeing buffer\n");
    } else {
        printf("  Success path: freeing buffer\n");
    }
    
    free(*buffer_ptr);
    *buffer_ptr = NULL;  // <-- FIX: Null it after freeing!
}

void fixed_multiple_paths(void) {
    printf("\n✅ FIXED: Null After Free\n");
    printf("─────────────────────────────────────────────\n");
    
    char* buffer = malloc(256);
    strcpy(buffer, "Test data");
    
    // Pass pointer to pointer so we can null it
    printf("Simulating error condition:\n");
    cleanup_fixed(&buffer, 1);
    
    // Safe to call again
    printf("Calling cleanup again:\n");
    cleanup_fixed(&buffer, 0);  // Safe!
    
    printf("✓ No double-free!\n");
}

// ANOTHER SCENARIO: Shared pointers
typedef struct {
    char* data;
    int ref_count;
} shared_data_t;

void buggy_shared(void) {
    printf("\n🐛 REAL SCENARIO: Shared Ownership\n");
    printf("─────────────────────────────────────────────\n");
    
    char* shared = malloc(100);
    strcpy(shared, "Shared resource");
    
    char* user1 = shared;
    char* user2 = shared;
    
    printf("✓ Two users sharing the same pointer\n");
    
    // User 1 done
    printf("User 1 done, freeing...\n");
    free(user1);
    
    // User 2 done
    printf("User 2 done, freeing...\n");
    free(user2);  // BUG: Double-free!
}

void fixed_shared(void) {
    printf("\n✅ FIXED: Reference Counting\n");
    printf("─────────────────────────────────────────────\n");
    
    shared_data_t* shared = malloc(sizeof(shared_data_t));
    shared->data = malloc(100);
    strcpy(shared->data, "Shared resource");
    shared->ref_count = 2;  // Two users
    
    printf("✓ Created shared data with ref_count=%d\n", shared->ref_count);
    
    // User 1 done
    shared->ref_count--;
    printf("User 1 done, ref_count=%d\n", shared->ref_count);
    if (shared->ref_count == 0) {
        free(shared->data);
        free(shared);
    }
    
    // User 2 done
    if (shared->ref_count > 0) {  // Check if still valid
        shared->ref_count--;
        printf("User 2 done, ref_count=%d\n", shared->ref_count);
        if (shared->ref_count == 0) {
            free(shared->data);
            free(shared);
            printf("✓ Freed when ref_count reached 0\n");
        }
    }
}

int main(void) {
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  Bug Example 2: Double-Free\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    printf("\n⚠️  WARNING: This demonstrates a SECURITY VULNERABILITY!\n");
    printf("   Double-free can be exploited for code execution.\n");
    printf("   ApexAlloc should detect and abort on double-free.\n\n");
    
    // Uncomment to test (will likely crash):
    // buggy_simple();
    
    // buggy_multiple_paths();
    fixed_multiple_paths();
    
    // buggy_shared();
    fixed_shared();
    
    printf("\n📚 KEY LESSONS:\n");
    printf("   1. Set pointers to NULL immediately after free()\n");
    printf("   2. Use double-pointers for cleanup functions\n");
    printf("   3. Check for NULL before freeing\n");
    printf("   4. Use reference counting for shared ownership\n");
    printf("   5. Modern allocators detect double-free (ApexAlloc does!)\n\n");
    
    return 0;
}
