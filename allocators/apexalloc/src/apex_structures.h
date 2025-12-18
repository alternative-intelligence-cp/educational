/**
 * ApexAlloc Data Structures
 * 
 * This file defines the three-level memory hierarchy:
 * - Superblock (4MB OS-level segment)
 * - Block (64KB thread-owned page)
 * - Chunk (user object with header)
 */

#include "apex_internal.h"

/* ============================================================================
 * Level 1: Superblock (Segment)
 * ============================================================================ */

/**
 * Superblock: Large contiguous region from OS (4MB)
 * Aligned to 2MB for Transparent Huge Pages
 */
struct apex_superblock_t {
    struct apex_superblock_t* next;     // Global cache linkage
    struct apex_superblock_t* prev;     // Doubly-linked for easy removal
    _Atomic size_t used_blocks;         // Reference count of active blocks
    int numa_node;                      // NUMA node ID (-1 if unknown)
    uint32_t magic;                     // Magic number for validation
    uint32_t padding;                   // Align to 32 bytes
    uint8_t blocks_data[];              // Blocks start here
};

/* ============================================================================
 * Level 2: Block (Page)
 * ============================================================================ */

/**
 * Block: 64KB region dedicated to a single size class
 * Owned by one thread at a time
 */
struct apex_block_t {
    // Metadata (hot path - accessed frequently)
    struct apex_block_t* next;          // Link in TLAB list
    uint16_t size_class;                // Size class index
    uint16_t free_count;                // Available chunks
    uint16_t max_count;                 // Total capacity
    uint16_t flags;                     // State flags
    
    // Thread ownership
    uint64_t owner_tid;                 // Thread ID of owner
    
    // Free lists (different cache lines to avoid false sharing)
    void* local_free_list;              // Local free list (fast path)
    uint8_t padding1[56];               // Pad to cache line (64 bytes total)
    
    _Atomic(void*) thread_free_list;    // Remote free list (mailbox)
    uint8_t padding2[56];               // Pad to cache line
    
    // Bump pointer for fresh blocks
    void* scan_ptr;                     // Next unallocated chunk
    void* scan_end;                     // End of allocatable region
    
    // Security
    uint64_t block_secret;              // Random key for pointer obfuscation
    
    // Back pointer to superblock
    struct apex_superblock_t* superblock;
    
    uint8_t chunks_data[];              // Chunks start here
};

// Block flags
#define BLOCK_FLAG_ACTIVE    (1 << 0)   // Block is in use
#define BLOCK_FLAG_ORPHANED  (1 << 1)   // Owner thread exited
#define BLOCK_FLAG_FULL      (1 << 2)   // No free chunks

/* ============================================================================
 * Level 3: Chunk (User Object)
 * ============================================================================ */

/**
 * Chunk header: Prepended to every user allocation
 * Contains security and management metadata
 */
typedef struct {
    uint32_t checksum;      // CRC32 integrity check
    uint16_t size_idx;      // Size class index
    uint16_t state : 2;     // Allocation state
    uint16_t unused : 14;   // Reserved for future use
} apex_chunk_header_t;

// Chunk states
#define CHUNK_STATE_FREE       0
#define CHUNK_STATE_ALLOCATED  1
#define CHUNK_STATE_QUARANTINE 2

/* ============================================================================
 * Helper Functions
 * ============================================================================ */

/**
 * Get the block that contains a given chunk pointer
 */
static inline apex_block_t* apex_ptr_to_block(const void* ptr) {
    uintptr_t addr = (uintptr_t)ptr;
    uintptr_t block_addr = ALIGN_DOWN(addr, BLOCK_SIZE);
    return (apex_block_t*)block_addr;
}

/**
 * Get the superblock that contains a given block
 */
static inline apex_superblock_t* apex_block_to_superblock(const apex_block_t* block) {
    uintptr_t addr = (uintptr_t)block;
    uintptr_t sb_addr = ALIGN_DOWN(addr, SUPERBLOCK_SIZE);
    return (apex_superblock_t*)sb_addr;
}

/**
 * Get chunk header from user pointer
 */
static inline apex_chunk_header_t* apex_ptr_to_header(const void* ptr) {
    return (apex_chunk_header_t*)((uint8_t*)ptr - sizeof(apex_chunk_header_t));
}

/**
 * Get user pointer from chunk header
 */
static inline void* apex_header_to_ptr(const apex_chunk_header_t* header) {
    return (void*)((uint8_t*)header + sizeof(apex_chunk_header_t));
}

/**
 * Check if pointer is likely valid (basic sanity check)
 */
static inline bool apex_ptr_is_valid(const void* ptr) {
    if (ptr == NULL) return false;
    
    uintptr_t addr = (uintptr_t)ptr;
    
    // Check alignment (should be at least 16-byte aligned)
    if ((addr & 15) != 0) return false;
    
    // Check it's not in low memory (likely NULL-ish)
    if (addr < 4096) return false;
    
    // Check it's not in kernel space (on x86_64)
    if (addr > 0x00007fffffffffffUL) return false;
    
    return true;
}

/* ============================================================================
 * Size Class System (forward declarations)
 * ============================================================================ */

uint8_t apex_size_to_class(size_t size);
size_t apex_class_to_size(uint8_t class_idx);
uint16_t apex_class_to_count(uint8_t class_idx);
uint8_t apex_class_fragmentation(uint8_t class_idx, size_t actual_size);
