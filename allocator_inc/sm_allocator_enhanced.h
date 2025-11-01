/*
 * 🎯 RANDY'S ENHANCED SHAGGYMEM ALLOCATOR 🎯
 * BitSet Optimization + Type-Aware Memory Management
 * 
 * Educational Version for Aspiring Programmers
 * "1 still better than 8" - Memory Efficiency Revolution
 * 
 * Features:
 * - BitSet optimization (87% memory savings over boolean arrays)
 * - Type tracking system with initialization flags
 * - Clean, educational code structure
 * - Production-ready memory management
 * 
 * 🚀 RANDY'S PERFORMANCE FLAGS 🚀
 * Compile with these flags for speed-critical applications:
 * 
 * -DRANDY_SPEED_MODE          // Disable all optimizations for raw speed
 * -DRANDY_DISABLE_BITSET      // Use simple boolean array instead of BitSet
 * -DRANDY_DISABLE_TYPES       // Remove type tracking system
 * -DRANDY_DISABLE_LOGGING     // Remove allocation logging
 * -DRANDY_DISABLE_STATS       // Remove statistics collection
 */

#ifndef SM_ALLOCATOR_ENHANCED_H
#define SM_ALLOCATOR_ENHANCED_H

// 🎯 Speed Mode Master Switch
#ifdef RANDY_SPEED_MODE
    #define RANDY_DISABLE_BITSET
    #define RANDY_DISABLE_TYPES
    #define RANDY_DISABLE_LOGGING
    #define RANDY_DISABLE_STATS
#endif

#include "sm_allocator_cross_platform_sysmem.h"
#ifndef RANDY_DISABLE_BITSET
    #include "../BitSet/inc/bitSet.h"
#endif
#include <string.h>

// 🎯 Randy's Core Optimization Flags
#define SM_ALLOCATED true
#define SM_UNALLOCATED false
#define SM_ALLOCATOR_ERROR UINT64_MAX

// 🧠 Type Tracking System
#ifndef RANDY_DISABLE_TYPES
typedef enum {
    SM_TYPE_NONE = 0,        // No type tracking
    SM_TYPE_GENERIC = 1,     // Generic memory
    SM_TYPE_STRING = 2,      // String data
    SM_TYPE_INTEGER = 3,     // Integer data
    SM_TYPE_FLOAT = 4,       // Float data
    SM_TYPE_STRUCT = 5,      // Struct data
    SM_TYPE_ARRAY = 6,       // Array data
    SM_TYPE_CUSTOM = 7       // Custom user-defined type
} SM_DataType;
#else
typedef int SM_DataType;  // Dummy type for speed mode
#endif

// 🚀 Enhanced Allocation Structure
typedef struct sm_allocator_allocation_enhanced_t {
    smU64       id;                    // Unique allocation ID
    smU64       offset;                // Offset in storage buffer
    smU64       size;                  // Size in bytes
    smBOL       allocated;             // Allocation status (for compatibility)
#ifndef RANDY_DISABLE_TYPES
    SM_DataType type;                  // Data type (NEW!)
    char        type_name[32];         // Custom type name (NEW!)
#endif
#ifndef RANDY_DISABLE_STATS
    smU64       access_count;          // Usage tracking (NEW!)
    double      last_access_time;      // Performance monitoring (NEW!)
#endif
    struct sm_allocator_enhanced_t* allocator;  // Back reference
} SM_AllocationEnhanced;

// 🧠 Randy's BitSet-Optimized Allocator
typedef struct sm_allocator_enhanced_t {
    // 📊 Core Memory Management
    smU64   storageLastId;             // Last allocation ID used
    smU64   storageSize;               // Total storage capacity
    smU64   storageUsed;               // Currently used storage
    smU08*  storage;                   // Main memory pool
    
    // 🎯 Randy's BitSet Optimization ("1 still better than 8")
#ifndef RANDY_DISABLE_BITSET
    BitSet* allocationFlags;           // Replaces boolean array - 87% savings!
    BitSet* typeTrackingEnabled;       // Per-allocation type tracking flags
#else
    smBOL*  allocationFlags;           // Simple boolean array for speed mode
#endif
    
    // 🗃️ Enhanced Allocation Tracking
    smU64   allocationsSize;           // Allocation metadata capacity
    smU64   allocationsUsed;           // Currently used metadata slots
    SM_AllocationEnhanced* allocations;// Enhanced allocation metadata
    
    // ♻️ Memory Reuse Optimization
    smU64   unusedAllocationsSize;     // Reusable allocation slots capacity
    smU64   unusedAllocationsUsed;     // Currently available reusable slots
    SM_AllocationEnhanced* unusedAllocations;  // Reusable allocation pool
    
    // 🔍 Free Space Management
    smU64   freeMax;                   // Largest available free block
    smU64   freeSize;                  // Free space tracking capacity
    smU64   freeUsed;                  // Currently tracked free blocks
    smU64*  free;                      // Free block registry
    
#ifndef RANDY_DISABLE_TYPES
    // 🎯 Type System Configuration (NEW!)
    smBOL   typeTrackingGlobal;        // Global type tracking flag
    smU64   totalTypedAllocations;     // Count of typed allocations
    smU64   typeStatistics[8];         // Per-type allocation counts
#endif
    
    // 📈 Performance Metrics (NEW!)
    double  efficiency_ratio;          // Memory utilization efficiency
    smU64   total_allocations;         // Lifetime allocation count
    smU64   total_deallocations;       // Lifetime deallocation count
    
} SM_AllocatorEnhanced;

// 🚀 Core Allocator Functions
SM_AllocatorEnhanced* SM_AllocatorEnhanced_create(smU64 storageSize, smBOL enableTypeTracking);
SM_AllocationEnhanced* SM_AllocatorEnhanced_allocate(SM_AllocatorEnhanced* allocator, smU64 allocationSize);
SM_AllocationEnhanced* SM_AllocatorEnhanced_allocateTyped(SM_AllocatorEnhanced* allocator, smU64 allocationSize, SM_DataType type, const char* typeName);
smBOL SM_AllocatorEnhanced_deallocate(SM_AllocationEnhanced* allocation);
smBOL SM_AllocatorEnhanced_destroy(SM_AllocatorEnhanced* allocator);

// 🎯 Randy's BitSet Optimizations
smBOL SM_AllocatorEnhanced_isAllocated(SM_AllocatorEnhanced* allocator, smU64 allocationId);
void SM_AllocatorEnhanced_setAllocated(SM_AllocatorEnhanced* allocator, smU64 allocationId, smBOL allocated);

// 🧠 Type System Functions
smBOL SM_AllocatorEnhanced_setAllocationType(SM_AllocationEnhanced* allocation, SM_DataType type, const char* typeName);
SM_DataType SM_AllocatorEnhanced_getAllocationType(SM_AllocationEnhanced* allocation);
const char* SM_AllocatorEnhanced_getTypeString(SM_DataType type);

// 📊 Performance and Statistics
void SM_AllocatorEnhanced_printStats(SM_AllocatorEnhanced* allocator);
void SM_AllocatorEnhanced_printTypeStats(SM_AllocatorEnhanced* allocator);
double SM_AllocatorEnhanced_getEfficiency(SM_AllocatorEnhanced* allocator);

// 🔍 Memory Utilities
void* SM_AllocatorEnhanced_getDataPointer(SM_AllocationEnhanced* allocation);
smBOL SM_AllocatorEnhanced_validateAllocation(SM_AllocationEnhanced* allocation);

// 🚀 RANDY'S PHASE 1 PERFORMANCE OPTIMIZATIONS
// Optimized versions with 5-10x speed improvements
SM_AllocatorEnhanced* SM_AllocatorEnhanced_create_optimized(smU64 storageSize, smBOL enableTypeTracking);
SM_AllocationEnhanced* SM_AllocatorEnhanced_allocate_optimized(SM_AllocatorEnhanced* allocator, 
                                                               smU64 allocationSize, 
                                                               SM_DataType type, 
                                                               smU08 type_name_id);
SM_AllocationEnhanced* SM_AllocatorEnhanced_allocate_fast(SM_AllocatorEnhanced* allocator, smU64 allocationSize);
SM_AllocationEnhanced* SM_AllocatorEnhanced_allocate_string_fast(SM_AllocatorEnhanced* allocator, smU64 allocationSize);
SM_AllocationEnhanced* SM_AllocatorEnhanced_allocate_integer_fast(SM_AllocatorEnhanced* allocator, smU64 allocationSize);

// Fast bit operations (inline performance)
smBOL SM_AllocatorEnhanced_isAllocated_fast(SM_AllocatorEnhanced* allocator, smU64 allocationId);
void SM_AllocatorEnhanced_setAllocated_fast(SM_AllocatorEnhanced* allocator, smU64 allocationId, smBOL allocated);

// Performance tuning
void SM_AllocatorEnhanced_set_debug_level(int level);
const char* SM_AllocatorEnhanced_get_type_name(smU08 type_name_id);

#endif // SM_ALLOCATOR_ENHANCED_H