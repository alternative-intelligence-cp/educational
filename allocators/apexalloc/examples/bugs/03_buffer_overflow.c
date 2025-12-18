/**
 * Bug Example 3: Buffer Overflow
 * 
 * WHAT'S WRONG:
 * Writing beyond allocated memory corrupts adjacent allocations
 * or heap metadata. This is a CRITICAL security vulnerability!
 * 
 * SYMPTOMS:
 * - Crashes (often delayed, not at the bug site)
 * - Data corruption in unrelated variables
 * - Checksum failures in ApexAlloc
 * - Exploitable for arbitrary code execution
 * 
 * HOW APEXALLOC HELPS:
 * - CRC32 checksums detect metadata corruption
 * - Guard pages (with mprotect) can catch overflows
 * - Size class segregation limits damage
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// BUGGY VERSION
void buggy_strcpy(void) {
    printf("\n🐛 BUGGY: Buffer Overflow with strcpy\n");
    printf("─────────────────────────────────────────────\n");
    
    char* buffer = malloc(10);
    
    printf("✓ Allocated 10 bytes\n");
    
    // BUG: String is 25 bytes (including null terminator)
    strcpy(buffer, "This is way too long!");  // KABOOM!
    
    printf("❌ BUG: Wrote 25 bytes to 10-byte buffer\n");
    printf("   Buffer contains: %s\n", buffer);
    
    free(buffer);
}

void fixed_strcpy(void) {
    printf("\n✅ FIXED: Use strncpy or snprintf\n");
    printf("─────────────────────────────────────────────\n");
    
    char* buffer = malloc(10);
    
    printf("✓ Allocated 10 bytes\n");
    
    // FIX 1: Use strncpy (but remember to null-terminate!)
    strncpy(buffer, "This is way too long!", 9);
    buffer[9] = '\0';  // Ensure null termination
    
    printf("✓ Safely copied (truncated): %s\n", buffer);
    
    free(buffer);
    
    // FIX 2: Even better - use snprintf
    buffer = malloc(10);
    snprintf(buffer, 10, "This is way too long!");
    printf("✓ snprintf handles it: %s\n", buffer);
    
    free(buffer);
}

// REAL SCENARIO: Off-by-one error
void buggy_offbyone(void) {
    printf("\n🐛 CLASSIC: Off-By-One Error\n");
    printf("─────────────────────────────────────────────\n");
    
    int size = 10;
    char* buffer = malloc(size);
    
    // BUG: Should be i < size, not i <= size!
    for (int i = 0; i <= size; i++) {  // <-- BUG: writes 11 bytes!
        buffer[i] = 'A';
    }
    
    printf("❌ BUG: Wrote %d bytes to %d-byte buffer\n", size + 1, size);
    
    free(buffer);
}

void fixed_offbyone(void) {
    printf("\n✅ FIXED: Correct Loop Bounds\n");
    printf("─────────────────────────────────────────────\n");
    
    int size = 10;
    char* buffer = malloc(size);
    
    // FIX: Use < not <=
    for (int i = 0; i < size - 1; i++) {  // Leave room for null!
        buffer[i] = 'A';
    }
    buffer[size - 1] = '\0';
    
    printf("✓ Wrote safely: %s\n", buffer);
    
    free(buffer);
}

// DANGEROUS: Integer overflow leading to buffer overflow
void buggy_integer_overflow(void) {
    printf("\n🐛 DANGEROUS: Integer Overflow → Buffer Overflow\n");
    printf("─────────────────────────────────────────────\n");
    
    unsigned int size = 0xFFFFFFFF;  // Max uint
    unsigned int count = 2;
    
    // BUG: size + count wraps around to 1!
    unsigned int total = size + count;
    
    printf("Size: %u, Count: %u\n", size, count);
    printf("Total (with overflow): %u\n", total);
    
    // Allocate tiny buffer due to overflow
    char* buffer = malloc(total);
    
    // Try to write based on original size - KABOOM!
    if (buffer) {
        printf("❌ BUG: Allocated only %u bytes, but will try to write %u!\n",
               total, size);
        // Don't actually do this:
        // memset(buffer, 'A', size);
        free(buffer);
    }
}

void fixed_integer_overflow(void) {
    printf("\n✅ FIXED: Check for Overflow\n");
    printf("─────────────────────────────────────────────\n");
    
    unsigned int size = 0xFFFFFFFF;
    unsigned int count = 2;
    
    // FIX: Check for overflow before arithmetic
    if (size > UINT_MAX - count) {
        printf("✓ Detected overflow! Not allocating.\n");
        return;
    }
    
    unsigned int total = size + count;
    char* buffer = malloc(total);
    
    if (buffer) {
        memset(buffer, 'A', total);
        printf("✓ Safe allocation\n");
        free(buffer);
    }
}

// VISUALIZATION: See the corruption
void demonstrate_corruption(void) {
    printf("\n🔍 VISUALIZATION: How Overflow Corrupts Memory\n");
    printf("─────────────────────────────────────────────\n");
    
    // Allocate two adjacent buffers
    char* victim = malloc(64);
    char* overflow_buffer = malloc(16);
    
    strcpy(victim, "IMPORTANT DATA");
    
    printf("Before overflow:\n");
    printf("  victim:          %p = \"%s\"\n", (void*)victim, victim);
    printf("  overflow_buffer: %p\n", (void*)overflow_buffer);
    
    // Overflow into victim (if they happen to be adjacent)
    strcpy(overflow_buffer, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");  // 33 bytes!
    
    printf("\nAfter overflow:\n");
    printf("  victim might now be corrupted: \"%s\"\n", victim);
    printf("  (Corruption depends on allocator layout)\n");
    
    free(victim);
    free(overflow_buffer);
}

int main(void) {
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  Bug Example 3: Buffer Overflow\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    printf("\n⚠️  WARNING: Buffer overflow is a CRITICAL security bug!\n");
    printf("   It's the #1 cause of exploits in C/C++ programs.\n");
    printf("   Run with AddressSanitizer: gcc -fsanitize=address\n\n");
    
    // buggy_strcpy();  // Uncomment to see crash
    fixed_strcpy();
    
    // buggy_offbyone();
    fixed_offbyone();
    
    buggy_integer_overflow();
    fixed_integer_overflow();
    
    demonstrate_corruption();
    
    printf("\n📚 KEY LESSONS:\n");
    printf("   1. NEVER use strcpy, strcat, gets - use safe versions\n");
    printf("   2. Always check array bounds (use < not <=)\n");
    printf("   3. Check for integer overflow before allocation\n");
    printf("   4. Use -fsanitize=address during development\n");
    printf("   5. Consider bounds-checked languages (Rust, Go) for security\n");
    printf("   6. ApexAlloc's checksums can detect some overflows\n\n");
    
    return 0;
}
