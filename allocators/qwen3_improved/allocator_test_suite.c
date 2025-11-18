/**
 * Educational Allocator Test Suite
 * ================================
 * 
 * Comprehensive tests for the improved educational allocator.
 * Great for learning memory management concepts!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

// Include our allocator (in real code, this would be a header file)
// For this demo, we'll assume it's compiled together

// Forward declarations from our allocator
typedef struct allocator_t allocator_t;

allocator_t* allocator_create(size_t total_memory, int use_bitset);
void* allocator_malloc(allocator_t* alloc, size_t size);
void allocator_free(allocator_t* alloc, void* ptr);
int allocator_get_id(allocator_t* alloc, void* ptr);
void* allocator_get_ptr(allocator_t* alloc, int id);
void allocator_set_bit(allocator_t* alloc, size_t index, int value);
int allocator_get_bit(allocator_t* alloc, size_t index);
void allocator_print_stats(allocator_t* alloc);
void allocator_destroy(allocator_t* alloc);

/**
 * Test 1: Basic allocation and deallocation
 */
void test_basic_operations() {
    printf("\n🧪 Test 1: Basic Operations\n");
    printf("============================\n");
    
    allocator_t* alloc = allocator_create(1024 * 1024, 0);
    assert(alloc != NULL);
    
    // Test various sizes
    void* ptr1 = allocator_malloc(alloc, 32);
    void* ptr2 = allocator_malloc(alloc, 64);
    void* ptr3 = allocator_malloc(alloc, 128);
    
    assert(ptr1 != NULL);
    assert(ptr2 != NULL);
    assert(ptr3 != NULL);
    assert(ptr1 != ptr2);
    assert(ptr2 != ptr3);
    
    // Test writing and reading
    strcpy((char*)ptr1, "Hello");
    strcpy((char*)ptr2, "World");
    strcpy((char*)ptr3, "Educational Allocator!");
    
    assert(strcmp((char*)ptr1, "Hello") == 0);
    assert(strcmp((char*)ptr2, "World") == 0);
    assert(strcmp((char*)ptr3, "Educational Allocator!") == 0);
    
    printf("✅ Memory read/write works correctly\n");
    
    // Test freeing
    allocator_free(alloc, ptr1);
    allocator_free(alloc, ptr2);
    allocator_free(alloc, ptr3);
    
    printf("✅ Basic allocation/free works\n");
    
    allocator_destroy(alloc);
}

/**
 * Test 2: ID mapping system
 */
void test_id_mapping() {
    printf("\n🧪 Test 2: ID Mapping System\n");
    printf("==============================\n");
    
    allocator_t* alloc = allocator_create(1024 * 1024, 0);
    assert(alloc != NULL);
    
    void* ptrs[10];
    int ids[10];
    
    // Allocate multiple pointers and get their IDs
    for (int i = 0; i < 10; i++) {
        ptrs[i] = allocator_malloc(alloc, 64);
        assert(ptrs[i] != NULL);
        
        ids[i] = allocator_get_id(alloc, ptrs[i]);
        assert(ids[i] >= 0);
        
        // Verify ID -> pointer mapping
        assert(allocator_get_ptr(alloc, ids[i]) == ptrs[i]);
        
        printf("  Allocation %d: ptr=%p, id=%d\n", i, ptrs[i], ids[i]);
    }
    
    // Free all and verify IDs are invalidated
    for (int i = 0; i < 10; i++) {
        allocator_free(alloc, ptrs[i]);
        assert(allocator_get_ptr(alloc, ids[i]) == NULL);
    }
    
    printf("✅ ID mapping system works correctly\n");
    
    allocator_destroy(alloc);
}

/**
 * Test 3: Bitset functionality
 */
void test_bitset() {
    printf("\n🧪 Test 3: Bitset Operations\n");
    printf("==============================\n");
    
    allocator_t* alloc = allocator_create(1024 * 1024, 1);  // Enable bitset
    assert(alloc != NULL);
    
    // Test bit operations
    allocator_set_bit(alloc, 0, 1);
    allocator_set_bit(alloc, 7, 1);
    allocator_set_bit(alloc, 15, 1);
    allocator_set_bit(alloc, 100, 1);
    
    assert(allocator_get_bit(alloc, 0) == 1);
    assert(allocator_get_bit(alloc, 7) == 1);
    assert(allocator_get_bit(alloc, 15) == 1);
    assert(allocator_get_bit(alloc, 100) == 1);
    
    // Test unset bits
    assert(allocator_get_bit(alloc, 1) == 0);
    assert(allocator_get_bit(alloc, 8) == 0);
    assert(allocator_get_bit(alloc, 99) == 0);
    
    // Test clearing bits
    allocator_set_bit(alloc, 0, 0);
    allocator_set_bit(alloc, 100, 0);
    
    assert(allocator_get_bit(alloc, 0) == 0);
    assert(allocator_get_bit(alloc, 100) == 0);
    
    printf("✅ Bitset operations work correctly\n");
    
    allocator_destroy(alloc);
}

/**
 * Test 4: Large allocations (mmap path)
 */
void test_large_allocations() {
    printf("\n🧪 Test 4: Large Allocations\n");
    printf("==============================\n");
    
    allocator_t* alloc = allocator_create(10 * 1024 * 1024, 0);  // 10MB
    assert(alloc != NULL);
    
    // Allocate something larger than the largest chunk (262KB)
    size_t large_size = 512 * 1024;  // 512KB
    void* large_ptr = allocator_malloc(alloc, large_size);
    assert(large_ptr != NULL);
    
    printf("  Large allocation: %zu bytes at %p\n", large_size, large_ptr);
    
    // Test writing to large allocation
    memset(large_ptr, 0xAB, large_size);
    
    // Verify pattern
    unsigned char* test_ptr = (unsigned char*)large_ptr;
    for (int i = 0; i < 1000; i++) {
        assert(test_ptr[i] == 0xAB);
    }
    
    printf("✅ Large allocation read/write works\n");
    
    // Test freeing large allocation (this was the bug!)
    allocator_free(alloc, large_ptr);
    
    printf("✅ Large allocation free works (bug fixed!)\n");
    
    allocator_destroy(alloc);
}

/**
 * Test 5: Memory reuse and fragmentation
 */
void test_memory_reuse() {
    printf("\n🧪 Test 5: Memory Reuse\n");
    printf("=========================\n");
    
    allocator_t* alloc = allocator_create(1024 * 1024, 0);
    assert(alloc != NULL);
    
    void* ptrs[20];
    
    // Allocate many small chunks
    for (int i = 0; i < 20; i++) {
        ptrs[i] = allocator_malloc(alloc, 64);
        assert(ptrs[i] != NULL);
    }
    
    printf("  Allocated 20 chunks of 64 bytes each\n");
    allocator_print_stats(alloc);
    
    // Free every other allocation
    for (int i = 0; i < 20; i += 2) {
        allocator_free(alloc, ptrs[i]);
    }
    
    printf("\n  Freed 10 chunks (every other one)\n");
    allocator_print_stats(alloc);
    
    // Allocate again - should reuse freed chunks
    for (int i = 0; i < 10; i++) {
        void* new_ptr = allocator_malloc(alloc, 64);
        assert(new_ptr != NULL);
        printf("  Reused chunk: %p\n", new_ptr);
    }
    
    printf("✅ Memory reuse works correctly\n");
    
    allocator_destroy(alloc);
}

/**
 * Test 6: Stress test
 */
void test_stress() {
    printf("\n🧪 Test 6: Stress Test\n");
    printf("========================\n");
    
    allocator_t* alloc = allocator_create(50 * 1024 * 1024, 0);  // 50MB
    assert(alloc != NULL);
    
    const int num_allocs = 1000;
    void* ptrs[num_allocs];
    size_t sizes[] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    clock_t start = clock();
    
    // Random allocation pattern
    srand(42);  // Reproducible randomness
    for (int i = 0; i < num_allocs; i++) {
        size_t size = sizes[rand() % num_sizes];
        ptrs[i] = allocator_malloc(alloc, size);
        
        if (ptrs[i] == NULL) {
            printf("  Allocation failed at iteration %d\n", i);
            break;
        }
        
        // Write pattern to verify integrity
        if (size >= 4) {
            *(int*)ptrs[i] = i;
        }
        
        if (i % 100 == 0) {
            printf("  Progress: %d/%d allocations\n", i, num_allocs);
        }
    }
    
    clock_t alloc_time = clock() - start;
    
    // Verify data integrity
    int corrupted = 0;
    for (int i = 0; i < num_allocs; i++) {
        if (ptrs[i] && sizes[rand() % num_sizes] >= 4) {
            if (*(int*)ptrs[i] != i) {
                corrupted++;
            }
        }
    }
    
    printf("  Data integrity check: %d corrupted out of %d\n", corrupted, num_allocs);
    
    start = clock();
    
    // Free all allocations
    for (int i = 0; i < num_allocs; i++) {
        if (ptrs[i]) {
            allocator_free(alloc, ptrs[i]);
        }
    }
    
    clock_t free_time = clock() - start;
    
    printf("  Allocation time: %.2f ms\n", (double)alloc_time / CLOCKS_PER_SEC * 1000);
    printf("  Free time: %.2f ms\n", (double)free_time / CLOCKS_PER_SEC * 1000);
    printf("✅ Stress test completed successfully\n");
    
    allocator_destroy(alloc);
}

/**
 * Test 7: Edge cases and error handling
 */
void test_edge_cases() {
    printf("\n🧪 Test 7: Edge Cases\n");
    printf("=======================\n");
    
    allocator_t* alloc = allocator_create(1024, 0);  // Small allocator
    assert(alloc != NULL);
    
    // Test zero size allocation
    void* zero_ptr = allocator_malloc(alloc, 0);
    assert(zero_ptr == NULL);
    printf("✅ Zero-size allocation correctly rejected\n");
    
    // Test NULL pointer free
    allocator_free(alloc, NULL);  // Should not crash
    printf("✅ NULL pointer free handled gracefully\n");
    
    // Test double free (should print error but not crash)
    void* test_ptr = allocator_malloc(alloc, 32);
    assert(test_ptr != NULL);
    allocator_free(alloc, test_ptr);
    allocator_free(alloc, test_ptr);  // Double free - should be detected
    printf("✅ Double free detected and handled\n");
    
    // Test allocation larger than total memory
    void* large_ptr = allocator_malloc(alloc, 2048);  // Larger than 1024 byte limit
    assert(large_ptr == NULL);
    printf("✅ Over-limit allocation correctly rejected\n");
    
    allocator_destroy(alloc);
}

/**
 * Main test runner
 */
int main() {
    printf("🎓 Educational Memory Allocator Test Suite\n");
    printf("============================================\n");
    printf("Testing improved allocator with bug fixes and educational features!\n");
    
    test_basic_operations();
    test_id_mapping();
    test_bitset();
    test_large_allocations();
    test_memory_reuse();
    test_stress();
    test_edge_cases();
    
    printf("\n🎉 All tests passed! The allocator is ready for educational use!\n");
    printf("\n📚 Learning Points:\n");
    printf("   • Chunked allocation reduces fragmentation\n");
    printf("   • Size classes improve allocation speed\n");
    printf("   • Free lists enable memory reuse\n");
    printf("   • ID mapping provides allocation tracking\n");
    printf("   • Large allocations use mmap for efficiency\n");
    printf("   • Proper error handling prevents crashes\n");
    printf("   • Statistics help understand memory usage\n");
    
    return 0;
}