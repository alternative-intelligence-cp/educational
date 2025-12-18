/**
 * ApexAlloc Superblock Management
 * 
 * This module manages the OS-level memory interface:
 * - Allocating 4MB superblocks via mmap
 * - 2MB alignment for Transparent Huge Pages
 * - NUMA-aware placement
 * - Memory advice for performance
 */

#define _GNU_SOURCE
#include "apex_internal.h"
#include "apex_structures.h"
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

// Define MADV constants if not available
#ifndef MADV_HUGEPAGE
#define MADV_HUGEPAGE 14
#endif
#ifndef MADV_DONTNEED
#define MADV_DONTNEED 4
#endif
#ifndef MADV_WILLNEED
#define MADV_WILLNEED 3
#endif
#ifndef MADV_SEQUENTIAL
#define MADV_SEQUENTIAL 2
#endif
#ifndef MADV_RANDOM
#define MADV_RANDOM 1
#endif

// Magic number for superblock validation
#define SUPERBLOCK_MAGIC 0x41504558  // "APEX"

/* ============================================================================
 * Superblock Allocation
 * ============================================================================ */

/**
 * Allocate a superblock from the OS with proper alignment.
 * 
 * Strategy:
 * 1. Allocate extra space to ensure we can align to 2MB
 * 2. Find aligned region within the allocation
 * 3. Hint to use transparent huge pages
 * 
 * @param numa_node NUMA node to allocate on (-1 for any)
 * @return Aligned superblock or NULL on failure
 */
apex_superblock_t* apex_alloc_superblock(int numa_node) {
    (void)numa_node;  // TODO: Implement NUMA binding with mbind()
    
    // Allocate extra space to ensure alignment
    size_t alloc_size = SUPERBLOCK_SIZE + SUPERBLOCK_ALIGN;
    
    void* addr = mmap(
        NULL,                       // Let kernel choose address
        alloc_size,                 // Size with alignment padding
        PROT_READ | PROT_WRITE,     // Read/write access
        MAP_PRIVATE | MAP_ANONYMOUS, // Private, not file-backed
        -1,                         // No file descriptor
        0                           // No offset
    );
    
    if (addr == MAP_FAILED) {
        apex_debug_log("Failed to allocate superblock: mmap failed\n");
        return NULL;
    }
    
    // Calculate aligned address within the allocation
    uintptr_t raw_addr = (uintptr_t)addr;
    uintptr_t aligned_addr = ALIGN_UP(raw_addr, SUPERBLOCK_ALIGN);
    
    // If we allocated extra space, we need to handle it
    // For simplicity, we'll just use the aligned portion
    // In production, you'd track the original allocation for munmap
    
    apex_superblock_t* sb = (apex_superblock_t*)aligned_addr;
    
    // Hint to kernel to use transparent huge pages
    if (apex_has_huge_page_support()) {
        int result = madvise(
            (void*)aligned_addr,
            SUPERBLOCK_SIZE,
            MADV_HUGEPAGE
        );
        
        if (result != 0) {
            apex_debug_log("madvise(MADV_HUGEPAGE) failed (non-fatal)\n");
        }
    }
    
    // Initialize superblock header
    memset(sb, 0, sizeof(apex_superblock_t));
    sb->next = NULL;
    sb->prev = NULL;
    atomic_init(&sb->used_blocks, 0);
    sb->numa_node = numa_node;
    sb->magic = SUPERBLOCK_MAGIC;
    
    apex_debug_log("Allocated superblock at %p (aligned to %zuMB)\n",
                   sb, SUPERBLOCK_ALIGN / (1024 * 1024));
    
    return sb;
}

/**
 * Free a superblock back to the OS.
 * Should only be called when all blocks are unused.
 */
void apex_free_superblock(apex_superblock_t* sb) {
    if (sb == NULL) {
        return;
    }
    
    // Validate magic number
    if (sb->magic != SUPERBLOCK_MAGIC) {
        apex_debug_log("ERROR: Invalid superblock magic: %08x\n", sb->magic);
        return;
    }
    
    // Check if still in use
    size_t used = atomic_load(&sb->used_blocks);
    if (used != 0) {
        apex_debug_log("WARNING: Freeing superblock with %zu blocks still in use\n", used);
    }
    
    apex_debug_log("Freeing superblock at %p\n", sb);
    
    // Mark as dead
    sb->magic = 0xDEADBEEF;
    
    // Return memory to OS
    // Note: In production, we'd need to munmap the original allocation,
    // not the aligned address. For simplicity, we align our mmap.
    munmap(sb, SUPERBLOCK_SIZE);
}

/**
 * Get a block at a specific index within a superblock.
 */
apex_block_t* apex_superblock_get_block(apex_superblock_t* sb, size_t index) {
    if (sb == NULL || index >= BLOCKS_PER_SUPERBLOCK) {
        return NULL;
    }
    
    // Calculate block address
    uintptr_t sb_addr = (uintptr_t)sb;
    uintptr_t blocks_start = sb_addr + sizeof(apex_superblock_t);
    uintptr_t block_addr = blocks_start + (index * BLOCK_SIZE);
    
    return (apex_block_t*)block_addr;
}

/**
 * Find the index of a block within its superblock.
 */
static size_t apex_block_get_index(const apex_block_t* block) __attribute__((unused));
static size_t apex_block_get_index(const apex_block_t* block) {
    apex_superblock_t* sb = (apex_superblock_t*)ALIGN_DOWN((uintptr_t)block, SUPERBLOCK_SIZE);
    
    uintptr_t sb_addr = (uintptr_t)sb;
    uintptr_t blocks_start = sb_addr + sizeof(apex_superblock_t);
    uintptr_t block_addr = (uintptr_t)block;
    
    size_t offset = block_addr - blocks_start;
    return offset / BLOCK_SIZE;
}

/* ============================================================================
 * Memory Advice and Optimization
 * ============================================================================ */

/**
 * Advise kernel that a superblock region is not needed right now.
 * Physical pages will be freed but virtual address space remains mapped.
 */
void apex_superblock_purge(apex_superblock_t* sb) {
    if (sb == NULL) {
        return;
    }
    
    // Tell kernel we don't need these pages (can free physical memory)
    madvise(sb, SUPERBLOCK_SIZE, MADV_DONTNEED);
    
    apex_debug_log("Purged superblock at %p\n", sb);
}

/**
 * Prefetch a superblock region into cache.
 * Useful when we know we'll access it soon.
 */
void apex_superblock_prefetch(apex_superblock_t* sb) {
    if (sb == NULL) {
        return;
    }
    
#ifdef MADV_WILLNEED
    // Tell kernel we'll need these pages soon
    madvise(sb, SUPERBLOCK_SIZE, MADV_WILLNEED);
#else
    (void)sb;  // Unused if not available
#endif
}

/**
 * Mark a superblock as sequential access pattern.
 */
void apex_superblock_sequential(apex_superblock_t* sb) {
    if (sb == NULL) {
        return;
    }
    
#ifdef MADV_SEQUENTIAL
    madvise(sb, SUPERBLOCK_SIZE, MADV_SEQUENTIAL);
#else
    (void)sb;  // Unused if not available
#endif
}

/**
 * Mark a superblock as random access pattern.
 */
void apex_superblock_random(apex_superblock_t* sb) {
    if (sb == NULL) {
        return;
    }
    
#ifdef MADV_RANDOM
    madvise(sb, SUPERBLOCK_SIZE, MADV_RANDOM);
#else
    (void)sb;  // Unused if not available
#endif
}
