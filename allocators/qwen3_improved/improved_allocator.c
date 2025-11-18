/**
 * Educational Custom Memory Allocator
 * ==================================
 * 
 * A chunked memory allocator with ID mapping and optional bitset functionality.
 * Designed for educational purposes to demonstrate memory management concepts.
 * 
 * Features:
 * - Size-based chunking (8B to 262KB)
 * - Pointer ID mapping system
 * - Optional bitset operations
 * - Memory usage tracking
 * - Free list management
 * - Large allocation support via mmap
 * 
 * Author: Based on Qwen3 design, improved for education
 * License: Educational use
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

// Configuration constants
#define CHUNK_SIZES 16
#define MAX_ID 10000  // Reduced for educational clarity
#define ALIGNMENT 8   // 8-byte alignment

// Chunk size definitions (powers of 2 for efficiency)
static const size_t CHUNK_SIZE_TABLE[CHUNK_SIZES] = {
    8, 16, 32, 64, 128, 256, 512, 1024,
    2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144
};

/**
 * Chunk header structure
 * Used for managing allocated chunks in free lists
 */
typedef struct chunk_header {
    size_t size;                    // Size of this chunk
    int is_free;                   // Free flag
    struct chunk_header* next;     // Next chunk in free list
    struct chunk_header* prev;     // Previous chunk in free list
} chunk_header_t;

/**
 * Allocation tracking structure
 * Stores metadata about each allocation
 */
typedef struct alloc_info {
    void* ptr;                     // Pointer to allocated memory
    size_t size;                   // Size of allocation
    int is_large;                  // 1 if large (mmap'd), 0 if chunked
    int in_use;                    // 1 if allocated, 0 if free
} alloc_info_t;

/**
 * Main allocator structure
 */
typedef struct {
    // Chunk management
    chunk_header_t* free_lists[CHUNK_SIZES];  // Free lists for each size
    size_t chunk_sizes[CHUNK_SIZES];          // Size table
    
    // Memory tracking
    size_t total_memory;                      // Total memory limit
    size_t used_memory;                       // Currently used memory
    size_t peak_memory;                       // Peak memory usage
    int allocation_count;                     // Number of active allocations
    
    // ID mapping system
    alloc_info_t* alloc_map;                  // Allocation info array
    int* free_ids;                            // Stack of free IDs
    int free_id_count;                        // Count of free IDs
    int max_id;                               // Maximum ID value
    
    // Optional bitset
    int use_bitset;                           // Enable bitset functionality
    unsigned char* bitset;                    // Bitset array
    size_t bitset_size;                       // Bitset size in bytes
    
    // Statistics
    int total_allocations;                    // Total allocs since creation
    int total_frees;                          // Total frees since creation
} allocator_t;

/**
 * Create a new allocator instance
 * 
 * @param total_memory: Maximum memory this allocator can use
 * @param use_bitset: Enable bitset functionality (1) or not (0)
 * @return: Pointer to allocator or NULL on failure
 */
allocator_t* allocator_create(size_t total_memory, int use_bitset) {
    allocator_t* alloc = malloc(sizeof(allocator_t));
    if (!alloc) {
        fprintf(stderr, "Failed to allocate memory for allocator structure\n");
        return NULL;
    }
    
    // Initialize basic fields
    alloc->total_memory = total_memory;
    alloc->used_memory = 0;
    alloc->peak_memory = 0;
    alloc->allocation_count = 0;
    alloc->use_bitset = use_bitset;
    alloc->max_id = MAX_ID;
    alloc->total_allocations = 0;
    alloc->total_frees = 0;
    
    // Copy chunk sizes
    for (int i = 0; i < CHUNK_SIZES; i++) {
        alloc->chunk_sizes[i] = CHUNK_SIZE_TABLE[i];
        alloc->free_lists[i] = NULL;
    }
    
    // Initialize allocation tracking
    alloc->alloc_map = calloc(MAX_ID, sizeof(alloc_info_t));
    alloc->free_ids = malloc(sizeof(int) * MAX_ID);
    
    if (!alloc->alloc_map || !alloc->free_ids) {
        fprintf(stderr, "Failed to allocate tracking arrays\n");
        free(alloc->alloc_map);
        free(alloc->free_ids);
        free(alloc);
        return NULL;
    }
    
    // Initialize ID stack (reverse order for sequential allocation)
    alloc->free_id_count = MAX_ID;
    for (int i = 0; i < MAX_ID; i++) {
        alloc->free_ids[i] = MAX_ID - i - 1;
    }
    
    // Initialize bitset if requested
    if (use_bitset) {
        alloc->bitset_size = total_memory / 8 + 1;
        alloc->bitset = calloc(alloc->bitset_size, 1);
        if (!alloc->bitset) {
            fprintf(stderr, "Failed to allocate bitset\n");
            free(alloc->alloc_map);
            free(alloc->free_ids);
            free(alloc);
            return NULL;
        }
    } else {
        alloc->bitset = NULL;
        alloc->bitset_size = 0;
    }
    
    printf("📝 Allocator created: %zu bytes capacity, %s bitset\n", 
           total_memory, use_bitset ? "with" : "without");
    
    return alloc;
}

/**
 * Find the best chunk size for a given allocation size
 */
static int find_chunk_index(allocator_t* alloc, size_t size) {
    for (int i = 0; i < CHUNK_SIZES; i++) {
        if (size <= alloc->chunk_sizes[i]) {
            return i;
        }
    }
    return -1; // Too large for chunking
}

/**
 * Allocate a new ID for tracking an allocation
 */
static int allocate_id(allocator_t* alloc) {
    if (alloc->free_id_count <= 0) {
        fprintf(stderr, "No free IDs available (max %d allocations)\n", MAX_ID);
        return -1;
    }
    
    int id = alloc->free_ids[--alloc->free_id_count];
    return id;
}

/**
 * Free an ID back to the pool
 */
static void free_id(allocator_t* alloc, int id) {
    if (id < 0 || id >= MAX_ID) {
        fprintf(stderr, "Invalid ID: %d\n", id);
        return;
    }
    
    if (alloc->free_id_count >= MAX_ID) {
        fprintf(stderr, "ID stack overflow\n");
        return;
    }
    
    // Clear allocation info
    alloc->alloc_map[id].ptr = NULL;
    alloc->alloc_map[id].size = 0;
    alloc->alloc_map[id].is_large = 0;
    alloc->alloc_map[id].in_use = 0;
    
    // Return ID to free pool
    alloc->free_ids[alloc->free_id_count++] = id;
}

/**
 * Main allocation function
 * 
 * @param alloc: Allocator instance
 * @param size: Number of bytes to allocate
 * @return: Pointer to allocated memory or NULL on failure
 */
void* allocator_malloc(allocator_t* alloc, size_t size) {
    if (!alloc || size == 0) {
        return NULL;
    }
    
    // Check memory limit
    if (alloc->used_memory + size > alloc->total_memory) {
        fprintf(stderr, "Memory limit exceeded: requested %zu, available %zu\n", 
                size, alloc->total_memory - alloc->used_memory);
        return NULL;
    }
    
    // Allocate tracking ID
    int id = allocate_id(alloc);
    if (id == -1) {
        return NULL;
    }
    
    // Align size
    size_t aligned_size = (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
    
    // Check if we should use chunked allocation
    int chunk_index = find_chunk_index(alloc, aligned_size);
    void* user_ptr = NULL;
    
    if (chunk_index == -1) {
        // Large allocation - use mmap directly
        size_t mmap_size = aligned_size;
        void* mmap_ptr = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE,
                             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        
        if (mmap_ptr == MAP_FAILED) {
            fprintf(stderr, "mmap failed for %zu bytes: %s\n", mmap_size, strerror(errno));
            free_id(alloc, id);
            return NULL;
        }
        
        user_ptr = mmap_ptr;
        
        // Record allocation info
        alloc->alloc_map[id].ptr = user_ptr;
        alloc->alloc_map[id].size = mmap_size;
        alloc->alloc_map[id].is_large = 1;
        alloc->alloc_map[id].in_use = 1;
        
        alloc->used_memory += mmap_size;
        
        printf("🔵 Large alloc: %zu bytes (mmap)\n", size);
        
    } else {
        // Chunked allocation
        size_t chunk_size = alloc->chunk_sizes[chunk_index];
        chunk_header_t* chunk = alloc->free_lists[chunk_index];
        
        if (chunk) {
            // Reuse existing chunk
            alloc->free_lists[chunk_index] = chunk->next;
            if (chunk->next) {
                chunk->next->prev = NULL;
            }
            
            printf("🟢 Reused chunk: %zu bytes (size class %d)\n", size, chunk_index);
        } else {
            // Create new chunk
            size_t total_size = sizeof(chunk_header_t) + chunk_size;
            chunk = mmap(NULL, total_size, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            
            if (chunk == MAP_FAILED) {
                fprintf(stderr, "mmap failed for chunk: %s\n", strerror(errno));
                free_id(alloc, id);
                return NULL;
            }
            
            chunk->size = total_size;
            chunk->next = NULL;
            chunk->prev = NULL;
            
            printf("🟡 New chunk: %zu bytes (size class %d)\n", size, chunk_index);
        }
        
        chunk->is_free = 0;
        user_ptr = (char*)chunk + sizeof(chunk_header_t);
        
        // Record allocation info
        alloc->alloc_map[id].ptr = user_ptr;
        alloc->alloc_map[id].size = chunk->size;
        alloc->alloc_map[id].is_large = 0;
        alloc->alloc_map[id].in_use = 1;
        
        alloc->used_memory += chunk->size;
    }
    
    // Update statistics
    alloc->allocation_count++;
    alloc->total_allocations++;
    if (alloc->used_memory > alloc->peak_memory) {
        alloc->peak_memory = alloc->used_memory;
    }
    
    return user_ptr;
}

/**
 * Free allocated memory
 * 
 * @param alloc: Allocator instance
 * @param ptr: Pointer to free
 */
void allocator_free(allocator_t* alloc, void* ptr) {
    if (!alloc || !ptr) {
        return;
    }
    
    // Find the allocation by pointer
    int id = -1;
    for (int i = 0; i < MAX_ID; i++) {
        if (alloc->alloc_map[i].in_use && alloc->alloc_map[i].ptr == ptr) {
            id = i;
            break;
        }
    }
    
    if (id == -1) {
        fprintf(stderr, "Invalid pointer passed to free: %p\n", ptr);
        return;
    }
    
    alloc_info_t* info = &alloc->alloc_map[id];
    
    if (info->is_large) {
        // Large allocation - unmap directly
        if (munmap(ptr, info->size) == -1) {
            fprintf(stderr, "munmap failed: %s\n", strerror(errno));
        }
        printf("🔴 Freed large alloc: %zu bytes\n", info->size);
    } else {
        // Chunked allocation - return to free list
        chunk_header_t* chunk = (chunk_header_t*)((char*)ptr - sizeof(chunk_header_t));
        
        // Find chunk index
        int chunk_index = find_chunk_index(alloc, chunk->size - sizeof(chunk_header_t));
        if (chunk_index >= 0) {
            chunk->is_free = 1;
            chunk->next = alloc->free_lists[chunk_index];
            chunk->prev = NULL;
            if (chunk->next) {
                chunk->next->prev = chunk;
            }
            alloc->free_lists[chunk_index] = chunk;
            
            printf("🟠 Freed to chunk pool: %zu bytes\n", info->size);
        }
    }
    
    // Update statistics
    alloc->used_memory -= info->size;
    alloc->allocation_count--;
    alloc->total_frees++;
    
    // Free the ID
    free_id(alloc, id);
}

/**
 * Get allocation ID for a pointer (educational function)
 */
int allocator_get_id(allocator_t* alloc, void* ptr) {
    if (!alloc || !ptr) return -1;
    
    for (int i = 0; i < MAX_ID; i++) {
        if (alloc->alloc_map[i].in_use && alloc->alloc_map[i].ptr == ptr) {
            return i;
        }
    }
    return -1;
}

/**
 * Get pointer for an allocation ID (educational function)
 */
void* allocator_get_ptr(allocator_t* alloc, int id) {
    if (!alloc || id < 0 || id >= MAX_ID) return NULL;
    
    if (alloc->alloc_map[id].in_use) {
        return alloc->alloc_map[id].ptr;
    }
    return NULL;
}

/**
 * Bitset operations (educational features)
 */
void allocator_set_bit(allocator_t* alloc, size_t index, int value) {
    if (!alloc || !alloc->use_bitset || !alloc->bitset) return;
    if (index >= alloc->bitset_size * 8) return;
    
    if (value) {
        alloc->bitset[index / 8] |= (1 << (index % 8));
    } else {
        alloc->bitset[index / 8] &= ~(1 << (index % 8));
    }
}

int allocator_get_bit(allocator_t* alloc, size_t index) {
    if (!alloc || !alloc->use_bitset || !alloc->bitset) return 0;
    if (index >= alloc->bitset_size * 8) return 0;
    
    return (alloc->bitset[index / 8] >> (index % 8)) & 1;
}

/**
 * Print allocator statistics (great for debugging and learning)
 */
void allocator_print_stats(allocator_t* alloc) {
    if (!alloc) return;
    
    printf("\n📊 Allocator Statistics\n");
    printf("========================\n");
    printf("Memory usage: %zu / %zu bytes (%.1f%%)\n", 
           alloc->used_memory, alloc->total_memory,
           (double)alloc->used_memory / alloc->total_memory * 100);
    printf("Peak usage: %zu bytes\n", alloc->peak_memory);
    printf("Active allocations: %d\n", alloc->allocation_count);
    printf("Total allocations: %d\n", alloc->total_allocations);
    printf("Total frees: %d\n", alloc->total_frees);
    printf("Free IDs available: %d\n", alloc->free_id_count);
    
    printf("\nChunk pools:\n");
    for (int i = 0; i < CHUNK_SIZES; i++) {
        int count = 0;
        chunk_header_t* chunk = alloc->free_lists[i];
        while (chunk) {
            count++;
            chunk = chunk->next;
        }
        if (count > 0) {
            printf("  Size %zu: %d free chunks\n", alloc->chunk_sizes[i], count);
        }
    }
    
    if (alloc->use_bitset) {
        printf("\nBitset: %zu bytes allocated\n", alloc->bitset_size);
    }
}

/**
 * Clean up all allocations and free lists
 */
void allocator_free_all(allocator_t* alloc) {
    if (!alloc) return;
    
    printf("🧹 Cleaning up all allocations...\n");
    
    // Free all active allocations
    for (int i = 0; i < MAX_ID; i++) {
        if (alloc->alloc_map[i].in_use) {
            allocator_free(alloc, alloc->alloc_map[i].ptr);
        }
    }
    
    // Clean up free lists
    for (int i = 0; i < CHUNK_SIZES; i++) {
        chunk_header_t* current = alloc->free_lists[i];
        while (current) {
            chunk_header_t* next = current->next;
            munmap(current, current->size);
            current = next;
        }
        alloc->free_lists[i] = NULL;
    }
    
    // Clear bitset
    if (alloc->use_bitset && alloc->bitset) {
        memset(alloc->bitset, 0, alloc->bitset_size);
    }
    
    printf("✅ Cleanup complete\n");
}

/**
 * Destroy allocator and free all resources
 */
void allocator_destroy(allocator_t* alloc) {
    if (!alloc) return;
    
    allocator_print_stats(alloc);
    allocator_free_all(alloc);
    
    free(alloc->alloc_map);
    free(alloc->free_ids);
    
    if (alloc->bitset) {
        free(alloc->bitset);
    }
    
    free(alloc);
    printf("🗑️  Allocator destroyed\n");
}