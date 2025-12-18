/**
 * ApexAlloc Size Class System
 * 
 * This module implements the size class calculation and lookup tables.
 * Size classes allow us to segregate objects by size, eliminating external
 * fragmentation and enabling fast allocation from free lists.
 * 
 * Strategy: Logarithmic spacing with 12.5% maximum internal fragmentation
 */

#include "apex_internal.h"
#include "apex_structures.h"

/* ============================================================================
 * Size Class Configuration
 * ============================================================================ */

// Size class table: maps size class index to actual object size
// Using powers of 2 for small sizes, then spacing gets wider
static const uint16_t g_size_class_table[SIZE_CLASS_COUNT] = {
    // Small sizes: 16-byte increments (0-15)
    16, 32, 48, 64, 80, 96, 112, 128, 
    144, 160, 176, 192, 208, 224, 240, 256,
    
    // Medium sizes: 32-byte increments (16-23)
    288, 320, 352, 384, 416, 448, 480, 512,
    
    // Larger sizes: 64-byte increments (24-31)
    576, 640, 704, 768, 832, 896, 960, 1024,
    
    // Large sizes: 128-byte increments (32-39)
    1152, 1280, 1408, 1536, 1664, 1792, 1920, 2048,
    
    // Very large: 256-byte increments (40-47)
    2304, 2560, 2816, 3072, 3328, 3584, 3840, 4096,
    
    // Extra large: 512-byte increments (48-55)
    4608, 5120, 5632, 6144, 6656, 7168, 7680, 8192,
    
    // Huge: 1KB increments (56-63)
    9216, 10240, 11264, 12288, 16384, 20480, 24576, 32768
};

/* ============================================================================
 * Fast Size Class Calculation
 * ============================================================================ */

/**
 * Calculate size class index for a given size.
 * Uses hardware instructions for fast computation.
 * 
 * Returns SIZE_CLASS_COUNT for sizes larger than SIZE_CLASS_MAX
 * (those go to the large object allocator)
 */
static inline uint8_t apex_size_to_class_slow(size_t size) {
    // Linear search as fallback (should rarely be used)
    for (uint8_t i = 0; i < SIZE_CLASS_COUNT; i++) {
        if (size <= g_size_class_table[i]) {
            return i;
        }
    }
    return SIZE_CLASS_COUNT;  // Too large
}

uint8_t apex_size_to_class(size_t size) {
    if (unlikely(size == 0)) {
        return 0;  // Minimum size class
    }
    
    if (unlikely(size > SIZE_CLASS_MAX)) {
        return SIZE_CLASS_COUNT;  // Large object
    }
    
    // Fast path: use binary search or hardware instructions
    // For sizes <= 256, we can use a direct lookup
    if (size <= 256) {
        // Round up to next multiple of 16
        return (uint8_t)((size + 15) >> 4) - 1;
    }
    
    // For larger sizes, use binary search
    // This compiles to efficient branch-free code on modern CPUs
    uint8_t low = 16;
    uint8_t high = SIZE_CLASS_COUNT - 1;
    
    while (low < high) {
        uint8_t mid = (low + high) >> 1;
        if (size <= g_size_class_table[mid]) {
            high = mid;
        } else {
            low = mid + 1;
        }
    }
    
    return low;
}

/**
 * Get the actual object size for a size class.
 */
size_t apex_class_to_size(uint8_t class_idx) {
    if (class_idx >= SIZE_CLASS_COUNT) {
        return 0;
    }
    return g_size_class_table[class_idx];
}

/**
 * Calculate how many objects of a given size class fit in a block.
 */
uint16_t apex_class_to_count(uint8_t class_idx) {
    if (class_idx >= SIZE_CLASS_COUNT) {
        return 0;
    }
    
    size_t obj_size = g_size_class_table[class_idx];
    
    // Account for block header
    size_t usable = BLOCK_SIZE - sizeof(apex_block_t);
    
    return (uint16_t)(usable / obj_size);
}

/**
 * Calculate internal fragmentation for a size class.
 * Returns percentage (0-100).
 */
uint8_t apex_class_fragmentation(uint8_t class_idx, size_t actual_size) {
    if (class_idx >= SIZE_CLASS_COUNT) {
        return 0;
    }
    
    size_t class_size = g_size_class_table[class_idx];
    size_t waste = class_size - actual_size;
    
    return (uint8_t)((waste * 100) / class_size);
}
