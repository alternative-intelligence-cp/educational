/**
 * Bug Example 1: Use-After-Free
 * 
 * WHAT'S WRONG:
 * Accessing memory after it has been freed is undefined behavior.
 * The memory might be reused for another allocation, causing corruption.
 * 
 * SYMPTOMS:
 * - Crashes (segfaults)
 * - Data corruption
 * - Works sometimes, fails randomly
 * - Heisenbug: disappears when debugging
 * 
 * HOW APEXALLOC HELPS:
 * - CRC32 checksums detect corruption
 * - Safe-linking makes exploitation harder
 * - Caught by leak detector and sanitizers
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// BUGGY VERSION
void buggy_example(void) {
    printf("\n🐛 BUGGY: Use-After-Free\n");
    printf("─────────────────────────────────────────────\n");
    
    char* data = malloc(100);
    strcpy(data, "Important data");
    
    printf("✓ Allocated and wrote: \"%s\"\n", data);
    
    // Free the memory
    free(data);
    printf("✓ Freed the memory\n");
    
    // BUG: Accessing freed memory!
    printf("❌ BUG: Reading freed memory: \"%s\"\n", data);
    
    // Even worse: writing to freed memory
    strcpy(data, "New data");  // KABOOM!
    printf("❌ BUG: Wrote to freed memory\n");
}

// FIXED VERSION
void fixed_example(void) {
    printf("\n✅ FIXED: Null After Free\n");
    printf("─────────────────────────────────────────────\n");
    
    char* data = malloc(100);
    strcpy(data, "Important data");
    
    printf("✓ Allocated and wrote: \"%s\"\n", data);
    
    // Free and NULL the pointer
    free(data);
    data = NULL;  // <-- FIX: Set to NULL after freeing!
    printf("✓ Freed and nulled the pointer\n");
    
    // Now if we accidentally access it, we get a clean crash
    if (data != NULL) {
        printf("✓ Using data: %s\n", data);
    } else {
        printf("✓ Caught: data is NULL, not using it\n");
    }
}

// REAL-WORLD SCENARIO: Dangling pointers in structs
typedef struct {
    char* name;
    int age;
} person_t;

void buggy_struct_example(void) {
    printf("\n🐛 REAL SCENARIO: Dangling Struct Pointers\n");
    printf("─────────────────────────────────────────────\n");
    
    person_t person;
    person.name = malloc(50);
    strcpy(person.name, "Alice");
    person.age = 30;
    
    printf("✓ Person: %s, age %d\n", person.name, person.age);
    
    // Free the name
    free(person.name);
    // BUG: Didn't null the pointer!
    
    // Later in code... someone uses it
    printf("❌ BUG: Name is now: %s\n", person.name);  // Undefined!
}

void fixed_struct_example(void) {
    printf("\n✅ FIXED: Null Struct Pointers\n");
    printf("─────────────────────────────────────────────\n");
    
    person_t person;
    person.name = malloc(50);
    strcpy(person.name, "Alice");
    person.age = 30;
    
    printf("✓ Person: %s, age %d\n", person.name, person.age);
    
    // Free and null
    free(person.name);
    person.name = NULL;  // <-- FIX!
    
    // Safe to check
    if (person.name != NULL) {
        printf("✓ Name: %s\n", person.name);
    } else {
        printf("✓ Name not set\n");
    }
}

int main(void) {
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  Bug Example 1: Use-After-Free\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    printf("\n⚠️  WARNING: This program demonstrates BUGS!\n");
    printf("   It may crash or behave unpredictably.\n");
    printf("   Run with: LD_PRELOAD=./build/libleak.so ./bug_example\n\n");
    
    buggy_example();
    fixed_example();
    buggy_struct_example();
    fixed_struct_example();
    
    printf("\n📚 KEY LESSONS:\n");
    printf("   1. Always set pointers to NULL after freeing\n");
    printf("   2. Check pointers before dereferencing\n");
    printf("   3. Use tools: valgrind, AddressSanitizer, ApexAlloc's leak detector\n");
    printf("   4. Consider smart pointers in C++ or unique_ptr patterns\n\n");
    
    return 0;
}
