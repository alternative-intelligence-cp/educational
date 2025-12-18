/**
 * ApexAlloc Block Management and Allocation
 * 
 * This module implements:
 * - Block initialization and management
 * - Fast path allocation (thread-local)
 * - Free list management
 * - Security features (checksums, safe-linking)
 */

#include "apex_internal.h"
#include "apex_structures.h"
#include <pthread.h>
#include <string.h>

/* ============================================================================
 * Thread-Local Allocation Buffer (TLAB)
 * ============================================================================ */

typedef struct {
    apex_block_t* active_blocks[SIZE_CLASS_COUNT];  // One block per size class
    uint64_t malloc_count;
    uint64_t free_count;
    uint64_t remote_free_count;
} apex_tlab_t;

// Thread-local storage
static __thread apex_tlab_t* g_tlab = NULL;
static __thread bool g_tlab_initialized = false;

/**
 * Initialize thread-local state.
 */
static void apex_tlab_init(void) {
    if (g_tlab_initialized) {
        return;
    }
    
    // Allocate TLAB (use system malloc for now, or bootstrap)
    g_tlab = (apex_tlab_t*)calloc(1, sizeof(apex_tlab_t));
    if (g_tlab == NULL) {
        apex_debug_log("FATAL: Could not allocate TLAB\n");
        return;
    }
    
    g_tlab_initialized = true;
    apex_debug_log("Initialized TLAB for thread %lu\n", pthread_self());
}

/**
 * Get current thread's TLAB, initializing if necessary.
 */
static inline apex_tlab_t* apex_get_tlab(void) {
    if (unlikely(!g_tlab_initialized)) {
        apex_tlab_init();
    }
    return g_tlab;
}

/* ============================================================================
 * Block Initialization
 * ============================================================================ */

/**
 * Initialize a block for a given size class.
 */
static void apex_block_init(apex_block_t* block, uint8_t size_class) {
    memset(block, 0, sizeof(apex_block_t));
    
    block->size_class = size_class;
    block->max_count = apex_class_to_count(size_class);
    block->free_count = block->max_count;
    block->flags = BLOCK_FLAG_ACTIVE;
    block->owner_tid = pthread_self();
    
    // Initialize bump pointer
    block->scan_ptr = (void*)block->chunks_data;
    block->scan_end = (void*)((uint8_t*)block + BLOCK_SIZE);
    
    // Generate block-specific secret for safe-linking
    block->block_secret = apex_get_pointer_key() ^ (uint64_t)block;
    
    // Get superblock reference
    block->superblock = (apex_superblock_t*)ALIGN_DOWN((uintptr_t)block, SUPERBLOCK_SIZE);
    
    // No free lists yet (will use bump pointer)
    block->local_free_list = NULL;
    atomic_init(&block->thread_free_list, NULL);
    
    apex_debug_log("Initialized block %p for size class %u (%zu bytes, %u objects)\n",
                   block, size_class, apex_class_to_size(size_class), block->max_count);
}

/**
 * Allocate a new block for a size class.
 * This is the "slow path" when TLAB runs out.
 */
apex_block_t* apex_alloc_block(uint8_t size_class) {
    // For now, allocate a new superblock each time
    // TODO: Implement superblock cache and reuse
    apex_superblock_t* sb = apex_alloc_superblock(-1);
    if (sb == NULL) {
        return NULL;
    }
    
    // Get first block from superblock
    apex_block_t* block = apex_superblock_get_block(sb, 0);
    if (block == NULL) {
        apex_free_superblock(sb);
        return NULL;
    }
    
    // Initialize the block
    apex_block_init(block, size_class);
    
    // Increment superblock usage
    atomic_fetch_add(&sb->used_blocks, 1);
    
    return block;
}

/* ============================================================================
 * Safe-Linking (Pointer Obfuscation)
 * ============================================================================ */

/**
 * Protect a pointer using safe-linking.
 * XORs pointer with a secret key to prevent tampering.
 */
static inline void* apex_protect_ptr(void* ptr, apex_block_t* block) {
#if ENABLE_SAFE_LINKING
    if (ptr == NULL) return NULL;
    uint64_t secret = block->block_secret;
    uintptr_t protected = (uintptr_t)ptr ^ secret ^ ((uintptr_t)ptr >> 12);
    return (void*)protected;
#else
    (void)block;
    return ptr;
#endif
}

/**
 * Reveal a safe-linked pointer.
 */
static inline void* apex_reveal_ptr(void* ptr, apex_block_t* block) {
#if ENABLE_SAFE_LINKING
    if (ptr == NULL) return NULL;
    uint64_t secret = block->block_secret;
    uintptr_t protected = (uintptr_t)ptr;
    uintptr_t revealed = protected ^ secret;
    revealed ^= (revealed >> 12);
    return (void*)revealed;
#else
    (void)block;
    return ptr;
#endif
}

/* ============================================================================
 * Checksums
 * ============================================================================ */

/**
 * Calculate CRC32 checksum (software fallback).
 */
static uint32_t apex_crc32_software(uint32_t crc, const void* data, size_t len) {
    const uint8_t* buf = (const uint8_t*)data;
    crc = ~crc;
    
    for (size_t i = 0; i < len; i++) {
        crc ^= buf[i];
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }
    
    return ~crc;
}

/**
 * Calculate checksum for a chunk header.
 */
static uint32_t apex_calc_checksum(const apex_chunk_header_t* header, const void* ptr) {
    uint64_t cookie = apex_get_process_cookie();
    
    // Hash: cookie ^ ptr ^ size_idx ^ state
    uint64_t hash = cookie;
    hash ^= (uint64_t)ptr;
    hash ^= (uint64_t)header->size_idx << 16;
    hash ^= (uint64_t)header->state;
    
    // Use CRC32 for avalanche effect
    return apex_crc32_software(0, &hash, sizeof(hash));
}

/**
 * Verify chunk header checksum.
 */
static bool apex_verify_checksum(const apex_chunk_header_t* header, const void* ptr) {
#if ENABLE_CHECKSUMS
    uint32_t expected = apex_calc_checksum(header, ptr);
    return header->checksum == expected;
#else
    (void)header;
    (void)ptr;
    return true;
#endif
}

/* ============================================================================
 * Block Allocation (Fast Path)
 * ============================================================================ */

/**
 * Allocate from a block using bump pointer or free list.
 */
void* apex_block_alloc(apex_block_t* block) {
    if (block == NULL) {
        return NULL;
    }
    
    // Try local free list first (fastest)
    if (block->local_free_list != NULL) {
        void* ptr = block->local_free_list;
        
        // Get next pointer (safe-linked)
        apex_chunk_header_t* header = (apex_chunk_header_t*)((uint8_t*)ptr - sizeof(apex_chunk_header_t));
        void** next_ptr = (void**)ptr;
        block->local_free_list = apex_reveal_ptr(*next_ptr, block);
        
        // Update header
        header->state = CHUNK_STATE_ALLOCATED;
        header->checksum = apex_calc_checksum(header, ptr);
        
        block->free_count--;
        return ptr;
    }
    
    // Try bump pointer (for fresh blocks)
    if (block->scan_ptr < block->scan_end) {
        size_t obj_size = apex_class_to_size(block->size_class);
        size_t total_size = sizeof(apex_chunk_header_t) + obj_size;
        
        if ((uint8_t*)block->scan_ptr + total_size <= (uint8_t*)block->scan_end) {
            // Allocate from bump pointer
            apex_chunk_header_t* header = (apex_chunk_header_t*)block->scan_ptr;
            void* ptr = (void*)((uint8_t*)header + sizeof(apex_chunk_header_t));
            
            // Initialize header
            header->size_idx = block->size_class;
            header->state = CHUNK_STATE_ALLOCATED;
            header->unused = 0;
            header->checksum = apex_calc_checksum(header, ptr);
            
            // Advance bump pointer
            block->scan_ptr = (void*)((uint8_t*)block->scan_ptr + total_size);
            block->free_count--;
            
            return ptr;
        }
    }
    
    // Block is full
    block->flags |= BLOCK_FLAG_FULL;
    return NULL;
}

/**
 * Free a pointer back to its block.
 */
void apex_block_free(apex_block_t* block, void* ptr) {
    if (block == NULL || ptr == NULL) {
        return;
    }
    
    // Get chunk header
    apex_chunk_header_t* header = (apex_chunk_header_t*)((uint8_t*)ptr - sizeof(apex_chunk_header_t));
    
    // Verify checksum
    if (!apex_verify_checksum(header, ptr)) {
        apex_debug_log("ERROR: Checksum mismatch for %p - heap corruption!\n", ptr);
        // In production: abort() or handle gracefully
        return;
    }
    
    // Check for double-free
#if ENABLE_DOUBLE_FREE_CHECK
    if (header->state == CHUNK_STATE_FREE) {
        apex_debug_log("ERROR: Double free detected at %p\n", ptr);
        // In production: abort()
        return;
    }
#endif
    
    // Check if this is owned by current thread
    uint64_t my_tid = pthread_self();
    if (block->owner_tid == my_tid) {
        // Local free - add to local free list
        header->state = CHUNK_STATE_FREE;
        header->checksum = apex_calc_checksum(header, ptr);
        
        // Link into local free list (safe-linking)
        void** next_ptr = (void**)ptr;
        *next_ptr = apex_protect_ptr(block->local_free_list, block);
        block->local_free_list = ptr;
        
        block->free_count++;
        block->flags &= ~BLOCK_FLAG_FULL;  // No longer full
    } else {
        // Remote free - add to atomic mailbox
        // TODO: Implement remote free queue (Phase VI)
        apex_debug_log("Remote free not yet implemented\n");
    }
}

/* ============================================================================
 * Main Allocation API
 * ============================================================================ */

/**
 * Allocate memory of given size (internal implementation).
 */
void* apex_malloc_impl(size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    // Get size class
    uint8_t size_class = apex_size_to_class(size);
    
    if (size_class >= SIZE_CLASS_COUNT) {
        // Large allocation (>32KB) - not yet implemented
        apex_debug_log("Large allocations not yet implemented\n");
        return NULL;
    }
    
    // Get TLAB
    apex_tlab_t* tlab = apex_get_tlab();
    if (tlab == NULL) {
        return NULL;
    }
    
    // Get active block for this size class
    apex_block_t* block = tlab->active_blocks[size_class];
    
    // If no block or block is full, allocate a new one
    if (block == NULL || (block->flags & BLOCK_FLAG_FULL)) {
        block = apex_alloc_block(size_class);
        if (block == NULL) {
            return NULL;
        }
        tlab->active_blocks[size_class] = block;
    }
    
    // Allocate from block
    void* ptr = apex_block_alloc(block);
    
    if (ptr != NULL) {
        tlab->malloc_count++;
    }
    
    return ptr;
}

/**
 * Free memory (internal implementation).
 */
void apex_free_impl(void* ptr) {
    if (ptr == NULL) {
        return;
    }
    
    // Get block from pointer
    apex_block_t* block = (apex_block_t*)ALIGN_DOWN((uintptr_t)ptr, BLOCK_SIZE);
    
    // Free to block
    apex_block_free(block, ptr);
    
    // Update stats
    apex_tlab_t* tlab = apex_get_tlab();
    if (tlab != NULL) {
        tlab->free_count++;
    }
}
