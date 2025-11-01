/*
 * 🚀 RANDY'S PHASE 2 ALGORITHMIC OPTIMIZATIONS
 * Slab Allocator + Free Lists + Memory Pool Batching
 * 
 * Target: 5-10x additional improvement over Phase 1
 * Goal: From 6x slower than malloc to 1-2x slower than malloc
 */

#include "allocator_inc/sm_allocator_enhanced.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// 🎯 Phase 2 Advanced Structures

// Slab allocator for common sizes
#define SLAB_SIZES_COUNT 8
static const size_t SLAB_SIZES[SLAB_SIZES_COUNT] = {
    16, 32, 64, 128, 256, 512, 1024, 2048
};

// Fast free list node
typedef struct SM_FreeNode {
    struct SM_FreeNode* next;
    smU64 allocation_id;
} SM_FreeNode;

// Memory slab for O(1) allocation
typedef struct SM_Slab {
    void* memory_block;          // Pre-allocated memory chunk
    SM_FreeNode* free_list;      // O(1) free allocation list
    size_t block_size;           // Size of each block
    size_t blocks_total;         // Total blocks in slab
    size_t blocks_free;          // Available blocks
    smU64 base_allocation_id;    // First allocation ID for this slab
} SM_Slab;

// Allocation group for cache efficiency
#define ALLOC_GROUP_SIZE 64
typedef struct SM_AllocationGroup {
    SM_AllocationEnhanced allocations[ALLOC_GROUP_SIZE];
    BitField* local_flags;       // Local bitset for this group
    int active_count;
    int next_free_slot;
} SM_AllocationGroup;

// Enhanced allocator with Phase 2 optimizations
typedef struct SM_AllocatorPhase2 {
    // Base allocator
    SM_AllocatorEnhanced* base_allocator;
    
    // Slab allocator system
    SM_Slab* slabs[SLAB_SIZES_COUNT];
    smBOL slab_initialized;
    
    // Free list management
    SM_FreeNode* free_node_pool;
    SM_FreeNode* available_free_nodes;
    smU64 free_node_pool_size;
    
    // Allocation groups for cache efficiency
    SM_AllocationGroup* allocation_groups;
    smU64 group_count;
    smU64 current_group;
    
    // ID management for reuse
    smU64* reusable_ids;
    smU64 reusable_id_count;
    smU64 reusable_id_capacity;
    
    // Performance counters
    smU64 slab_allocations;
    smU64 pool_allocations;
    smU64 cache_hits;
    smU64 cache_misses;
} SM_AllocatorPhase2;

// Global debug level
static int g_phase2_debug_level = 0;

// 📝 Phase 2 logging (even more optimized)
#define LOG_PHASE2_ERROR(fmt, ...) \
    do { if (g_phase2_debug_level >= 1) printf("🔥 " fmt "\n", ##__VA_ARGS__); } while(0)

#define LOG_PHASE2_INFO(fmt, ...) \
    do { if (g_phase2_debug_level >= 2) printf("⚡ " fmt "\n", ##__VA_ARGS__); } while(0)

#define LOG_PHASE2_VERBOSE(fmt, ...) \
    do { if (g_phase2_debug_level >= 3) printf("🎯 " fmt "\n", ##__VA_ARGS__); } while(0)

// 🚀 Phase 2 Implementation

// Initialize slab allocator
static smBOL init_slab_allocator(SM_AllocatorPhase2* allocator) {
    LOG_PHASE2_INFO("Initializing slab allocator with %d size classes", SLAB_SIZES_COUNT);
    
    for (int i = 0; i < SLAB_SIZES_COUNT; i++) {
        SM_Slab* slab = (SM_Slab*)sysalloc(sizeof(SM_Slab));
        if (!slab) {
            LOG_PHASE2_ERROR("Failed to allocate slab %d", i);
            return smFLS;
        }
        
        slab->block_size = SLAB_SIZES[i];
        slab->blocks_total = (4096 * 4) / slab->block_size;  // 16KB per slab
        slab->blocks_free = slab->blocks_total;
        slab->base_allocation_id = (i + 1) * 10000;  // Unique ID range per slab
        
        // Allocate memory block
        size_t total_size = slab->blocks_total * slab->block_size;
        slab->memory_block = sysalloc(total_size);
        if (!slab->memory_block) {
            LOG_PHASE2_ERROR("Failed to allocate slab memory for size %zu", slab->block_size);
            sysfree(slab, sizeof(SM_Slab));
            return smFLS;
        }
        
        // Initialize free list
        slab->free_list = NULL;
        char* block_ptr = (char*)slab->memory_block;
        for (smU64 j = 0; j < slab->blocks_total; j++) {
            SM_FreeNode* node = (SM_FreeNode*)block_ptr;
            node->next = slab->free_list;
            node->allocation_id = slab->base_allocation_id + j;
            slab->free_list = node;
            block_ptr += slab->block_size;
        }
        
        allocator->slabs[i] = slab;
        LOG_PHASE2_VERBOSE("Slab %d: %zu bytes, %lu blocks", i, slab->block_size, slab->blocks_total);
    }
    
    return smTRU;
}

// Find best slab for size
static int find_slab_for_size(size_t size) {
    for (int i = 0; i < SLAB_SIZES_COUNT; i++) {
        if (size <= SLAB_SIZES[i]) {
            return i;
        }
    }
    return -1;  // Too big for slab allocation
}

// O(1) slab allocation
static SM_AllocationEnhanced* allocate_from_slab(SM_AllocatorPhase2* allocator, size_t size, int slab_index) {
    SM_Slab* slab = allocator->slabs[slab_index];
    
    if (!slab->free_list || slab->blocks_free == 0) {
        LOG_PHASE2_VERBOSE("Slab %d exhausted", slab_index);
        return NULL;  // Slab full, fall back to pool allocation
    }
    
    // O(1) pop from free list
    SM_FreeNode* node = slab->free_list;
    slab->free_list = node->next;
    slab->blocks_free--;
    
    // Create allocation metadata
    SM_AllocationEnhanced* allocation = (SM_AllocationEnhanced*)sysalloc(sizeof(SM_AllocationEnhanced));
    if (!allocation) {
        // Put node back
        node->next = slab->free_list;
        slab->free_list = node;
        slab->blocks_free++;
        return NULL;
    }
    
    allocation->id = node->allocation_id;
    allocation->size = size;
    allocation->allocated = SM_ALLOCATED;
    allocation->type = SM_TYPE_GENERIC;
    allocation->offset = (char*)node - (char*)slab->memory_block;
#ifndef RANDY_DISABLE_STATS
    allocation->access_count = 1;
    allocation->last_access_time = 0.0;  // Skip time for speed
#endif
    allocation->allocator = (struct sm_allocator_enhanced_t*)allocator->base_allocator;
    
    // Update counters
    allocator->slab_allocations++;
    allocator->cache_hits++;
    
    LOG_PHASE2_VERBOSE("Slab alloc: %zu bytes, ID %lu, slab %d", size, allocation->id, slab_index);
    
    return allocation;
}

// Create Phase 2 optimized allocator
SM_AllocatorPhase2* SM_AllocatorPhase2_create(smU64 storageSize, smBOL enableTypeTracking) {
    LOG_PHASE2_INFO("Creating Phase 2 optimized allocator - Storage: %lu bytes", storageSize);
    
    SM_AllocatorPhase2* allocator = (SM_AllocatorPhase2*)sysalloc(sizeof(SM_AllocatorPhase2));
    if (!allocator) {
        LOG_PHASE2_ERROR("Failed to allocate Phase 2 allocator");
        return NULL;
    }
    
    // Initialize base allocator
    allocator->base_allocator = SM_AllocatorEnhanced_create_optimized(storageSize, enableTypeTracking);
    if (!allocator->base_allocator) {
        LOG_PHASE2_ERROR("Failed to create base allocator");
        sysfree(allocator, sizeof(SM_AllocatorPhase2));
        return NULL;
    }
    
    // Initialize slab system
    if (!init_slab_allocator(allocator)) {
        LOG_PHASE2_ERROR("Failed to initialize slab allocator");
        // Cleanup base allocator here
        sysfree(allocator, sizeof(SM_AllocatorPhase2));
        return NULL;
    }
    allocator->slab_initialized = smTRU;
    
    // Initialize allocation groups
    allocator->group_count = 32;  // Start with 32 groups
    allocator->allocation_groups = (SM_AllocationGroup*)sysalloc(
        allocator->group_count * sizeof(SM_AllocationGroup));
    if (!allocator->allocation_groups) {
        LOG_PHASE2_ERROR("Failed to allocate allocation groups");
        sysfree(allocator, sizeof(SM_AllocatorPhase2));
        return NULL;
    }
    
    // Initialize groups
    for (smU64 i = 0; i < allocator->group_count; i++) {
        allocator->allocation_groups[i].active_count = 0;
        allocator->allocation_groups[i].next_free_slot = 0;
        allocator->allocation_groups[i].local_flags = newBitSet(ALLOC_GROUP_SIZE);
    }
    allocator->current_group = 0;
    
    // Initialize ID reuse system
    allocator->reusable_id_capacity = 1024;
    allocator->reusable_ids = (smU64*)sysalloc(allocator->reusable_id_capacity * sizeof(smU64));
    allocator->reusable_id_count = 0;
    
    // Initialize counters
    allocator->slab_allocations = 0;
    allocator->pool_allocations = 0;
    allocator->cache_hits = 0;
    allocator->cache_misses = 0;
    
    LOG_PHASE2_INFO("Phase 2 allocator created successfully");
    return allocator;
}

// Ultra-fast Phase 2 allocation
SM_AllocationEnhanced* SM_AllocatorPhase2_allocate_fast(SM_AllocatorPhase2* allocator, smU64 size) {
    if (!allocator || size == 0) {
        LOG_PHASE2_ERROR("Invalid allocator or zero size");
        return NULL;
    }
    
    // Try slab allocation first for small sizes
    int slab_index = find_slab_for_size(size);
    if (slab_index >= 0) {
        SM_AllocationEnhanced* allocation = allocate_from_slab(allocator, size, slab_index);
        if (allocation) {
            return allocation;  // O(1) success!
        }
        // Fall through to pool allocation
        allocator->cache_misses++;
    }
    
    // Fall back to optimized pool allocation
    allocator->pool_allocations++;
    return SM_AllocatorEnhanced_allocate_fast(allocator->base_allocator, size);
}

// Specialized fast allocators for common types
SM_AllocationEnhanced* SM_AllocatorPhase2_allocate_string_fast(SM_AllocatorPhase2* allocator, smU64 size) {
    return SM_AllocatorPhase2_allocate_fast(allocator, size);
}

SM_AllocationEnhanced* SM_AllocatorPhase2_allocate_buffer_fast(SM_AllocatorPhase2* allocator, smU64 size) {
    return SM_AllocatorPhase2_allocate_fast(allocator, size);
}

// Get performance statistics
void SM_AllocatorPhase2_print_stats(SM_AllocatorPhase2* allocator) {
    if (!allocator) return;
    
    printf("🚀 PHASE 2 ALLOCATOR PERFORMANCE STATS:\n");
    printf("   Slab allocations: %lu\n", allocator->slab_allocations);
    printf("   Pool allocations: %lu\n", allocator->pool_allocations);
    printf("   Cache hits: %lu\n", allocator->cache_hits);
    printf("   Cache misses: %lu\n", allocator->cache_misses);
    
    if (allocator->cache_hits + allocator->cache_misses > 0) {
        double hit_rate = (double)allocator->cache_hits / (allocator->cache_hits + allocator->cache_misses);
        printf("   Cache hit rate: %.1f%%\n", hit_rate * 100.0);
    }
    
    // Slab utilization
    printf("   Slab utilization:\n");
    for (int i = 0; i < SLAB_SIZES_COUNT; i++) {
        SM_Slab* slab = allocator->slabs[i];
        double util = (double)(slab->blocks_total - slab->blocks_free) / slab->blocks_total;
        printf("     %zu bytes: %.1f%% (%lu/%lu)\n", 
               slab->block_size, util * 100.0, 
               slab->blocks_total - slab->blocks_free, slab->blocks_total);
    }
}

// Set debug level
void SM_AllocatorPhase2_set_debug_level(int level) {
    g_phase2_debug_level = level;
    LOG_PHASE2_INFO("Phase 2 debug level set to %d", level);
}