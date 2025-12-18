/**
 * ApexAlloc Internal Header
 * Private definitions and utilities
 */

#ifndef APEX_INTERNAL_H
#define APEX_INTERNAL_H

#include "../include/apexalloc.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <pthread.h>

/* ============================================================================
 * Configuration Constants
 * ============================================================================ */

// Superblock (Segment) configuration
#define SUPERBLOCK_SIZE (4 * 1024 * 1024)  // 4MB
#define SUPERBLOCK_ALIGN (2 * 1024 * 1024) // 2MB (huge page size)

// Block (Page) configuration
#define BLOCK_SIZE (64 * 1024)  // 64KB
#define BLOCKS_PER_SUPERBLOCK (SUPERBLOCK_SIZE / BLOCK_SIZE)

// Size class configuration
#define SIZE_CLASS_COUNT 64
#define SIZE_CLASS_MIN 16
#define SIZE_CLASS_MAX (32 * 1024)  // 32KB (larger goes to large alloc)

// Security configuration
#define ENABLE_CHECKSUMS 1
#define ENABLE_SAFE_LINKING 1
#define ENABLE_DOUBLE_FREE_CHECK 1

/* ============================================================================
 * Forward Declarations
 * ============================================================================ */

typedef struct apex_superblock_t apex_superblock_t;
typedef struct apex_block_t apex_block_t;

/* ============================================================================
 * Initialization State
 * ============================================================================ */

extern _Atomic bool g_apex_initialized;

/* ============================================================================
 * Bootstrap Allocator
 * ============================================================================ */

void* apex_bootstrap_malloc(size_t size);
void apex_bootstrap_free(void* ptr);
bool apex_is_bootstrap_pointer(const void* ptr);
void apex_bootstrap_stats(size_t* used, size_t* total);

/* ============================================================================
 * Initialization
 * ============================================================================ */

void apex_init_once(void);
void apex_global_init(void);

/* ============================================================================
 * Global State Accessors
 * ============================================================================ */

uint64_t apex_get_process_cookie(void);
uint64_t apex_get_pointer_key(void);
bool apex_has_crc32_support(void);
bool apex_has_huge_page_support(void);
int apex_get_numa_node_count(void);

/* ============================================================================
 * Superblock Management
 * ============================================================================ */

apex_superblock_t* apex_alloc_superblock(int numa_node);
void apex_free_superblock(apex_superblock_t* sb);
apex_block_t* apex_superblock_get_block(apex_superblock_t* sb, size_t index);

/* ============================================================================
 * Block Management
 * ============================================================================ */

apex_block_t* apex_alloc_block(uint8_t size_class);
void apex_free_block(apex_block_t* block);
void* apex_block_alloc(apex_block_t* block);
void apex_block_free(apex_block_t* block, void* ptr);

/* ============================================================================
 * Internal Allocation Functions
 * ============================================================================ */

void* apex_malloc_impl(size_t size);
void apex_free_impl(void* ptr);

/* ============================================================================
 * Size Class System
 * ============================================================================ */

uint8_t apex_size_to_class(size_t size);
size_t apex_class_to_size(uint8_t class_idx);
uint16_t apex_class_to_count(uint8_t class_idx);
uint8_t apex_class_fragmentation(uint8_t class_idx, size_t actual_size);

/* ============================================================================
 * Debugging
 * ============================================================================ */

#ifdef DEBUG
#include <stdio.h>
#define apex_debug_log(fmt, ...) \
    fprintf(stderr, "[ApexAlloc] " fmt, ##__VA_ARGS__)
#else
#define apex_debug_log(fmt, ...) ((void)0)
#endif

/* ============================================================================
 * Compiler Hints
 * ============================================================================ */

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define ALWAYS_INLINE __attribute__((always_inline)) inline

/* ============================================================================
 * Utility Macros
 * ============================================================================ */

#define ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))
#define ALIGN_DOWN(x, align) ((x) & ~((align) - 1))
#define IS_ALIGNED(x, align) (((uintptr_t)(x) & ((align) - 1)) == 0)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#endif /* APEX_INTERNAL_H */
