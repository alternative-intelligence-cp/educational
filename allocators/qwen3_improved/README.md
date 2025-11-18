# Educational Memory Allocator 🎓

An advanced educational memory allocator designed to teach memory management concepts through hands-on coding and experimentation.

## 🌟 Features

### Core Functionality
- **Chunked Allocation**: Size-based pools (8B to 262KB) for efficient memory management
- **Large Allocation Support**: Direct mmap for allocations > 262KB
- **Memory Tracking**: Complete allocation statistics and usage monitoring
- **ID Mapping System**: Track allocations by unique IDs for educational exploration
- **Free List Management**: Efficient memory reuse through linked free lists
- **Optional Bitset**: Learn bit manipulation with built-in bitset operations

### Educational Benefits
- **Verbose Logging**: See exactly what the allocator is doing
- **Statistics Tracking**: Understand memory usage patterns
- **Error Handling**: Learn proper error detection and handling
- **Memory Safety**: Built-in protections against common mistakes
- **Performance Insights**: Time allocation/free operations

## 🚀 Quick Start

```bash
# Build the allocator
make

# Run all tests
make test

# Run with debugging (AddressSanitizer)
make debug

# Performance benchmark
make bench

# Clean up
make clean
```

## 📝 Example Usage

```c
#include "improved_allocator.c"

int main() {
    // Create allocator with 1MB capacity
    allocator_t* alloc = allocator_create(1024 * 1024, 1); // Enable bitset
    
    // Allocate some memory
    void* ptr1 = allocator_malloc(alloc, 64);
    void* ptr2 = allocator_malloc(alloc, 128);
    
    // Use the memory
    strcpy((char*)ptr1, "Hello World!");
    printf("Stored: %s\\n", (char*)ptr1);
    
    // Get allocation ID (educational feature)
    int id = allocator_get_id(alloc, ptr1);
    printf("Allocation ID: %d\\n", id);
    
    // Use bitset features
    allocator_set_bit(alloc, 42, 1);
    printf("Bit 42: %d\\n", allocator_get_bit(alloc, 42));
    
    // Check statistics
    allocator_print_stats(alloc);
    
    // Clean up
    allocator_free(alloc, ptr1);
    allocator_free(alloc, ptr2);
    allocator_destroy(alloc);
    
    return 0;
}
```

## 🔧 API Reference

### Core Functions
- `allocator_create(size, use_bitset)` - Create new allocator
- `allocator_malloc(alloc, size)` - Allocate memory
- `allocator_free(alloc, ptr)` - Free memory
- `allocator_destroy(alloc)` - Destroy allocator

### Educational Functions
- `allocator_get_id(alloc, ptr)` - Get allocation ID
- `allocator_get_ptr(alloc, id)` - Get pointer from ID
- `allocator_print_stats(alloc)` - Print detailed statistics
- `allocator_set_bit(alloc, index, value)` - Set bit in bitset
- `allocator_get_bit(alloc, index)` - Get bit from bitset

## 🧪 Test Suite

The comprehensive test suite covers:

1. **Basic Operations** - Allocation, writing, reading, freeing
2. **ID Mapping** - Pointer-to-ID and ID-to-pointer conversions
3. **Bitset Operations** - Setting, getting, and clearing bits
4. **Large Allocations** - Testing mmap path for big allocations
5. **Memory Reuse** - Verifying free list functionality
6. **Stress Testing** - 1000+ allocation/free cycles
7. **Edge Cases** - Error handling and boundary conditions

## 📊 Performance Characteristics

- **Small allocations** (≤262KB): O(1) from free lists
- **Large allocations** (>262KB): O(1) via mmap
- **Memory overhead**: Chunk header + tracking info
- **Fragmentation**: Minimized through size classes

## 🐛 Fixed Issues

This improved version fixes several issues from the original Qwen3 design:

1. **Large Allocation Free Bug** - Fixed segfault when freeing mmap'd memory
2. **Memory Accounting** - Proper tracking of used/free memory
3. **Error Handling** - Graceful handling of edge cases
4. **Double Free Detection** - Prevents crashes from programming errors

## 🎯 Learning Objectives

After using this allocator, students will understand:

- How chunked allocation reduces fragmentation
- Why size classes improve performance
- How free lists enable memory reuse
- The trade-offs between different allocation strategies
- Memory alignment and its importance
- The difference between heap and mmap allocation
- How to implement proper error handling
- Performance measurement and optimization

## 🔗 Additional Resources

- [Memory Management Concepts](https://en.wikipedia.org/wiki/Memory_management)
- [malloc Implementation](https://sourceware.org/glibc/wiki/MallocInternals)
- [mmap System Call](https://man7.org/linux/man-pages/man2/mmap.2.html)

## 📄 License

Educational use only. Feel free to modify and experiment!

---

**Happy Learning! 🎓✨**

*This allocator is designed for education, not production use. It prioritizes learning and understanding over maximum performance.*