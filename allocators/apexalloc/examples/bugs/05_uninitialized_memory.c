/**
 * Bug Example 5: Uninitialized Memory
 * 
 * WHAT'S WRONG:
 * Using memory before initializing it leads to unpredictable behavior.
 * The memory contains whatever data was there before (could be anything!).
 * 
 * SYMPTOMS:
 * - Non-deterministic bugs (works sometimes, fails other times)
 * - Different behavior in debug vs release builds
 * - Security: might leak sensitive data
 * - Weird values in variables
 * 
 * HOW TO DETECT:
 * - valgrind --track-origins=yes
 * - MemorySanitizer (MSan): -fsanitize=memory
 * - Use calloc() instead of malloc()
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// BUGGY VERSION
void buggy_uninitialized(void) {
    printf("\n🐛 BUGGY: Uninitialized Memory\n");
    printf("─────────────────────────────────────────────\n");
    
    int* values = malloc(10 * sizeof(int));
    
    // BUG: Never initialized the memory!
    printf("❌ BUG: Reading uninitialized memory:\n");
    for (int i = 0; i < 10; i++) {
        printf("   values[%d] = %d (could be anything!)\n", i, values[i]);
    }
    
    free(values);
}

void fixed_memset(void) {
    printf("\n✅ FIXED: Initialize with memset\n");
    printf("─────────────────────────────────────────────\n");
    
    int* values = malloc(10 * sizeof(int));
    
    // FIX 1: Use memset to zero
    memset(values, 0, 10 * sizeof(int));
    
    printf("✓ Initialized with memset:\n");
    for (int i = 0; i < 10; i++) {
        printf("   values[%d] = %d\n", i, values[i]);
    }
    
    free(values);
}

void fixed_calloc(void) {
    printf("\n✅ BETTER: Use calloc (zeros automatically)\n");
    printf("─────────────────────────────────────────────\n");
    
    // FIX 2: Use calloc instead of malloc
    int* values = calloc(10, sizeof(int));
    
    printf("✓ Initialized with calloc:\n");
    for (int i = 0; i < 10; i++) {
        printf("   values[%d] = %d\n", i, values[i]);
    }
    
    free(values);
}

// REAL SCENARIO: Struct with uninitialized fields
typedef struct {
    int id;
    char name[50];
    double balance;
    int is_active;
} account_t;

void buggy_struct(void) {
    printf("\n🐛 DANGEROUS: Partially Initialized Struct\n");
    printf("─────────────────────────────────────────────\n");
    
    account_t* account = malloc(sizeof(account_t));
    
    // Only initialize some fields
    account->id = 12345;
    strcpy(account->name, "Alice");
    
    // BUG: balance and is_active are uninitialized!
    printf("Account ID: %d\n", account->id);
    printf("Name: %s\n", account->name);
    printf("❌ Balance: %f (uninitialized!)\n", account->balance);
    printf("❌ Active: %d (uninitialized!)\n", account->is_active);
    
    // DANGER: Using uninitialized is_active in conditional
    if (account->is_active) {
        printf("   Processing active account...\n");
    }
    
    free(account);
}

void fixed_struct(void) {
    printf("\n✅ FIXED: Zero-Initialize Struct\n");
    printf("─────────────────────────────────────────────\n");
    
    // FIX: Use calloc or memset
    account_t* account = calloc(1, sizeof(account_t));
    
    account->id = 12345;
    strcpy(account->name, "Alice");
    account->balance = 1000.50;
    account->is_active = 1;
    
    printf("✓ Account ID: %d\n", account->id);
    printf("✓ Name: %s\n", account->name);
    printf("✓ Balance: %.2f\n", account->balance);
    printf("✓ Active: %d\n", account->is_active);
    
    free(account);
}

// SECURITY ISSUE: Information leak
void security_leak(void) {
    printf("\n⚠️  SECURITY: Information Leak via Uninitialized Memory\n");
    printf("─────────────────────────────────────────────\n");
    
    // Allocate and write sensitive data
    char* secret = malloc(100);
    strcpy(secret, "PASSWORD123 SSN:123-45-6789");
    printf("Stored secret: %s\n", secret);
    free(secret);
    
    // Later, allocate same-sized buffer
    char* buffer = malloc(100);
    
    // BUG: Memory might still contain old data!
    printf("❌ New buffer contents (uninitialized):\n");
    printf("   First 20 bytes: ");
    for (int i = 0; i < 20 && buffer[i] != '\0'; i++) {
        printf("%c", buffer[i]);
    }
    printf("\n");
    printf("   ⚠️  Might leak sensitive data!\n");
    
    free(buffer);
}

void security_fixed(void) {
    printf("\n✅ SECURITY: Always Zero Sensitive Buffers\n");
    printf("─────────────────────────────────────────────\n");
    
    char* secret = malloc(100);
    strcpy(secret, "PASSWORD123 SSN:123-45-6789");
    printf("Stored secret: %s\n", secret);
    
    // FIX: Zero before freeing
    memset(secret, 0, 100);
    free(secret);
    printf("✓ Zeroed secret before freeing\n");
    
    // Allocate new buffer with calloc
    char* buffer = calloc(100, 1);
    printf("✓ New buffer is zeroed\n");
    
    free(buffer);
}

// TRICKY: Conditional initialization
void buggy_conditional(int init) {
    printf("\n🐛 TRICKY: Conditional Initialization\n");
    printf("─────────────────────────────────────────────\n");
    
    int* data = malloc(sizeof(int));
    
    if (init) {
        *data = 42;
    }
    // BUG: If init is false, data is uninitialized!
    
    printf("Data: %d (init=%d)\n", *data, init);
    printf("❌ Uninitialized if init=0!\n");
    
    free(data);
}

void fixed_conditional(int init) {
    printf("\n✅ FIXED: Always Initialize\n");
    printf("─────────────────────────────────────────────\n");
    
    int* data = malloc(sizeof(int));
    
    // FIX: Initialize with default value
    *data = 0;
    
    if (init) {
        *data = 42;
    }
    
    printf("✓ Data: %d (init=%d)\n", *data, init);
    
    free(data);
}

// PERFORMANCE NOTE: calloc vs malloc+memset
void performance_comparison(void) {
    printf("\n💡 PERFORMANCE: calloc vs malloc+memset\n");
    printf("─────────────────────────────────────────────\n");
    
    printf("Method 1: malloc + memset\n");
    void* buf1 = malloc(1024);
    memset(buf1, 0, 1024);
    printf("  Explicitly zeros 1024 bytes\n");
    free(buf1);
    
    printf("\nMethod 2: calloc\n");
    void* buf2 = calloc(1024, 1);
    printf("  OS might give already-zeroed pages (faster!)\n");
    printf("  Or zeros internally (same speed)\n");
    free(buf2);
    
    printf("\n✓ calloc is usually the better choice\n");
}

int main(void) {
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  Bug Example 5: Uninitialized Memory\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    printf("\n💡 Run with MemorySanitizer to detect:\n");
    printf("   gcc -fsanitize=memory -fPIE -pie ...\n\n");
    
    buggy_uninitialized();
    fixed_memset();
    fixed_calloc();
    
    buggy_struct();
    fixed_struct();
    
    security_leak();
    security_fixed();
    
    buggy_conditional(0);  // Uninitialized case
    buggy_conditional(1);  // Initialized case
    fixed_conditional(0);
    
    performance_comparison();
    
    printf("\n📚 KEY LESSONS:\n");
    printf("   1. ALWAYS initialize memory after allocation\n");
    printf("   2. Use calloc() for zero-initialization\n");
    printf("   3. Initialize ALL struct fields (or use calloc)\n");
    printf("   4. Zero sensitive data before freeing (security!)\n");
    printf("   5. Don't rely on \"usually zero\" - it's undefined behavior\n");
    printf("   6. Use -fsanitize=memory to catch these bugs\n");
    printf("   7. Consider = {0} initializer for local structs\n\n");
    
    return 0;
}
