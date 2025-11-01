/*
 * 🧠 RANDY'S CHUNKED ALLOCATOR HEADER - ORIGINAL VISION FINALLY IMPLEMENTED
 * 
 * Your 2-year-old question: "Would chunking help or hurt performance?"
 * Let's find out together! 🚀
 */

#ifndef ALLOCATOR_RANDY_CHUNKED_H
#define ALLOCATOR_RANDY_CHUNKED_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Randy's original chunking configuration flags
#define RANDY_CHUNKING_ENABLED 1        // Your main flag
#define RANDY_CHUNK_STATS_ENABLED 1     // Track fragmentation prevention
#define RANDY_DYNAMIC_RESIZE_ENABLED 1  // Auto-create new chunk sizes

typedef struct SM_RandyChunkedAllocator SM_RandyChunkedAllocator;

/*
 * 🚀 RANDY'S CHUNKED ALLOCATOR INTERFACE
 */

// Create Randy's chunked allocator with your original vision
SM_RandyChunkedAllocator* SM_RandyChunked_create(void);

// Smart allocation with size-aware chunk selection
void* SM_RandyChunked_allocate(SM_RandyChunkedAllocator* allocator, size_t size, const char* type_name);

// AriaX consciousness-aware allocation
void* SM_RandyChunked_allocate_consciousness_state(SM_RandyChunkedAllocator* allocator, double tesla_frequency);

// Performance analysis - answer your 2-year question!
void SM_RandyChunked_print_performance(SM_RandyChunkedAllocator* allocator);

// Get fragmentation elimination count
uint64_t SM_RandyChunked_get_fragmentation_eliminated(SM_RandyChunkedAllocator* allocator);

/*
 * 🎯 RANDY'S ORIGINAL SIZE CLASSES
 */
#define SM_CHUNK_TINY_SIZE    64    // Your tiny allocation idea
#define SM_CHUNK_SMALL_SIZE   256   // Your small allocation idea  
#define SM_CHUNK_MEDIUM_SIZE  1024  // Your medium allocation idea
#define SM_CHUNK_LARGE_SIZE   4096  // Your large allocation idea

#ifdef __cplusplus
}
#endif

#endif /* ALLOCATOR_RANDY_CHUNKED_H */