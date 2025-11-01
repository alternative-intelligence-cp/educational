/*
 * 🎯 RANDY'S ENHANCED SHAGGYMEM ALLOCATOR IMPLEMENTATION 🎯
 * BitSet Optimization + Type-Aware Memory Management
 * 
 * "1 still better than 8" - The Memory Efficiency Revolution
 * Educational Implementation for Aspiring Programmers
 */

#include "allocator_inc/sm_allocator_enhanced.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 🎯 Helper function to calculate memory requirements (use system function)
// Note: Using the existing calculateMemory function from sm_allocator_sysmem.h

// 🚀 Create Randy's enhanced allocator with BitSet optimization
SM_AllocatorEnhanced* SM_AllocatorEnhanced_create(smU64 storageSize, smBOL enableTypeTracking) {
    printf("🎯 Creating Randy's Enhanced Allocator...\n");
    
    SM_AllocatorEnhanced* allocator = (SM_AllocatorEnhanced*)sysalloc(sizeof(SM_AllocatorEnhanced));
    if (!allocator) {
        printf("❌ Failed to allocate enhancedvator structure!\n");
        return NULL;
    }
    
    // 📊 Initialize core memory management
    allocator->storageLastId = 0;
    allocator->storageSize = calculateMemory(storageSize);
    allocator->storageUsed = 0;
    allocator->storage = (smU08*)sysalloc(allocator->storageSize);
    
    if (!allocator->storage) {
        printf("❌ Failed to allocate storage buffer!\n");
        sysfree(allocator, sizeof(SM_AllocatorEnhanced));
        return NULL;
    }
    
    // 🎯 Randy's BitSet Optimization: "1 still better than 8"
    smU64 maxAllocations = storageSize / 64; // Reasonable default
    allocator->allocationFlags = newBitSet(maxAllocations);
    allocator->typeTrackingEnabled = newBitSet(maxAllocations);
    
    if (!allocator->allocationFlags || !allocator->typeTrackingEnabled) {
        printf("❌ Failed to create BitSet structures!\n");
        sysfree(allocator->storage, allocator->storageSize);
        sysfree(allocator, sizeof(SM_AllocatorEnhanced));
        return NULL;
    }
    
    printf("✅ BitSet created: %lu allocations using %lu bytes (vs %lu boolean bytes - %.1f%% savings!)\n",
           maxAllocations, 
           maxAllocations / 8,  // BitSet memory usage
           maxAllocations,      // Boolean array would use this much
           100.0 * (1.0 - (double)(maxAllocations / 8) / maxAllocations));
    
    // 🗃️ Initialize allocation tracking
    allocator->allocationsSize = maxAllocations;
    allocator->allocationsUsed = 0;
    allocator->allocations = (SM_AllocationEnhanced*)sysalloc(
        sizeof(SM_AllocationEnhanced) * maxAllocations);
    
    // ♻️ Initialize reuse pool
    allocator->unusedAllocationsSize = maxAllocations / 4; // 25% for reuse
    allocator->unusedAllocationsUsed = 0;
    allocator->unusedAllocations = (SM_AllocationEnhanced*)sysalloc(
        sizeof(SM_AllocationEnhanced) * allocator->unusedAllocationsSize);
    
    // 🔍 Initialize free space tracking
    allocator->freeMax = 0;
    allocator->freeSize = maxAllocations / 8;  // Conservative estimate
    allocator->freeUsed = 0;
    allocator->free = (smU64*)sysalloc(sizeof(smU64) * allocator->freeSize);
    
    // 🧠 Initialize type tracking system
    allocator->typeTrackingGlobal = enableTypeTracking;
    allocator->totalTypedAllocations = 0;
    memset(allocator->typeStatistics, 0, sizeof(allocator->typeStatistics));
    
    // 📈 Initialize performance metrics
    allocator->efficiency_ratio = 1.0;
    allocator->total_allocations = 0;
    allocator->total_deallocations = 0;
    
    printf("✅ Enhanced Allocator Created Successfully!\n");
    printf("   Storage Size: %lu bytes\n", allocator->storageSize);
    printf("   Max Allocations: %lu\n", maxAllocations);
    printf("   Type Tracking: %s\n", enableTypeTracking ? "ENABLED" : "DISABLED");
    printf("   BitSet Memory Savings: %.1f%%\n", 
           100.0 * (1.0 - (double)(maxAllocations / 8) / maxAllocations));
    
    return allocator;
}

// 🎯 Randy's BitSet optimization functions
smBOL SM_AllocatorEnhanced_isAllocated(SM_AllocatorEnhanced* allocator, smU64 allocationId) {
    return isBitSetAtIndex(allocator->allocationFlags, allocationId);
}

void SM_AllocatorEnhanced_setAllocated(SM_AllocatorEnhanced* allocator, smU64 allocationId, smBOL allocated) {
    setBitAtIndex(allocator->allocationFlags, allocationId, allocated ? 1 : 0);
}

// 🚀 Enhanced allocation with optional type tracking
SM_AllocationEnhanced* SM_AllocatorEnhanced_allocate(SM_AllocatorEnhanced* allocator, smU64 allocationSize) {
    return SM_AllocatorEnhanced_allocateTyped(allocator, allocationSize, SM_TYPE_GENERIC, "generic");
}

SM_AllocationEnhanced* SM_AllocatorEnhanced_allocateTyped(SM_AllocatorEnhanced* allocator, smU64 allocationSize, SM_DataType type, const char* typeName) {
    if (!allocator || allocationSize == 0) {
        printf("❌ Invalid allocator or zero size!\n");
        return NULL;
    }
    
    // Check if we have space
    if (allocator->storageUsed + allocationSize > allocator->storageSize) {
        printf("❌ Out of memory! Requested: %lu, Available: %lu\n", 
               allocationSize, allocator->storageSize - allocator->storageUsed);
        return NULL;
    }
    
    // Check if we have allocation slots
    if (allocator->allocationsUsed >= allocator->allocationsSize) {
        printf("❌ Out of allocation slots!\n");
        return NULL;
    }
    
    SM_AllocationEnhanced* allocation = NULL;
    
    // 🔄 Try to reuse an unused allocation
    if (allocator->unusedAllocationsUsed > 0) {
        allocation = &allocator->unusedAllocations[allocator->unusedAllocationsUsed - 1];
        allocator->unusedAllocationsUsed--;
        printf("♻️ Reusing allocation slot ID %lu\n", allocation->id);
    } else {
        // 🆕 Create new allocation
        allocation = &allocator->allocations[allocator->allocationsUsed];
        allocation->id = allocator->storageLastId++;
        allocator->allocationsUsed++;
    }
    
    // 📝 Set up the allocation
    allocation->offset = allocator->storageUsed;
    allocation->size = allocationSize;
    allocation->allocated = SM_ALLOCATED;
    allocation->type = type;
    allocation->access_count = 1;
    allocation->last_access_time = (double)clock() / CLOCKS_PER_SEC;
    allocation->allocator = allocator;
    
    // 🏷️ Set type name
    if (typeName) {
        strncpy(allocation->type_name, typeName, sizeof(allocation->type_name) - 1);
        allocation->type_name[sizeof(allocation->type_name) - 1] = '\0';
    } else {
        strcpy(allocation->type_name, "unknown");
    }
    
    // 🎯 Randy's BitSet optimization: "1 still better than 8"
    SM_AllocatorEnhanced_setAllocated(allocator, allocation->id, SM_ALLOCATED);
    
    // 🧠 Update type tracking if enabled
    if (allocator->typeTrackingGlobal && type != SM_TYPE_NONE) {
        setBitAtIndex(allocator->typeTrackingEnabled, allocation->id, 1);
        allocator->totalTypedAllocations++;
        if (type < 8) {  // Safety check for array bounds
            allocator->typeStatistics[type]++;
        }
    }
    
    // 📊 Update allocator state
    allocator->storageUsed += allocationSize;
    allocator->total_allocations++;
    
    // 📈 Update efficiency ratio
    allocator->efficiency_ratio = (double)allocator->storageUsed / allocator->storageSize;
    
    printf("✅ Allocated %lu bytes at ID %lu (offset %lu) - Type: %s\n", 
           allocationSize, allocation->id, allocation->offset, allocation->type_name);
    
    return allocation;
}

// 🧠 Type system functions
smBOL SM_AllocatorEnhanced_setAllocationType(SM_AllocationEnhanced* allocation, SM_DataType type, const char* typeName) {
    if (!allocation) return smFLS;
    
    allocation->type = type;
    if (typeName) {
        strncpy(allocation->type_name, typeName, sizeof(allocation->type_name) - 1);
        allocation->type_name[sizeof(allocation->type_name) - 1] = '\0';
    }
    
    printf("🏷️ Updated allocation %lu type to: %s\n", allocation->id, allocation->type_name);
    return smTRU;
}

SM_DataType SM_AllocatorEnhanced_getAllocationType(SM_AllocationEnhanced* allocation) {
    return allocation ? allocation->type : SM_TYPE_NONE;
}

const char* SM_AllocatorEnhanced_getTypeString(SM_DataType type) {
    switch (type) {
        case SM_TYPE_NONE:    return "none";
        case SM_TYPE_GENERIC: return "generic";
        case SM_TYPE_STRING:  return "string";
        case SM_TYPE_INTEGER: return "integer";
        case SM_TYPE_FLOAT:   return "float";
        case SM_TYPE_STRUCT:  return "struct";
        case SM_TYPE_ARRAY:   return "array";
        case SM_TYPE_CUSTOM:  return "custom";
        default:              return "unknown";
    }
}

// 🔓 Enhanced deallocation
smBOL SM_AllocatorEnhanced_deallocate(SM_AllocationEnhanced* allocation) {
    if (!allocation || !allocation->allocator) {
        printf("❌ Invalid allocation!\n");
        return smFLS;
    }
    
    SM_AllocatorEnhanced* allocator = allocation->allocator;
    
    if (!SM_AllocatorEnhanced_isAllocated(allocator, allocation->id)) {
        printf("❌ Allocation %lu is already deallocated!\n", allocation->id);
        return smFLS;
    }
    
    // 🎯 Randy's BitSet optimization
    SM_AllocatorEnhanced_setAllocated(allocator, allocation->id, SM_UNALLOCATED);
    
    // 🧠 Update type tracking
    if (allocator->typeTrackingGlobal && allocation->type != SM_TYPE_NONE) {
        setBitAtIndex(allocator->typeTrackingEnabled, allocation->id, 0);
        allocator->totalTypedAllocations--;
        if (allocation->type < 8) {
            allocator->typeStatistics[allocation->type]--;
        }
    }
    
    // 📊 Update allocator state
    allocator->storageUsed -= allocation->size;
    allocator->total_deallocations++;
    allocator->efficiency_ratio = (double)allocator->storageUsed / allocator->storageSize;
    
    // ♻️ Add to reuse pool if there's space
    if (allocator->unusedAllocationsUsed < allocator->unusedAllocationsSize) {
        allocator->unusedAllocations[allocator->unusedAllocationsUsed] = *allocation;
        allocator->unusedAllocationsUsed++;
    }
    
    // 🏷️ Clear allocation data
    allocation->allocated = SM_UNALLOCATED;
    allocation->type = SM_TYPE_NONE;
    strcpy(allocation->type_name, "deallocated");
    
    printf("✅ Deallocated %lu bytes from ID %lu\n", allocation->size, allocation->id);
    
    return smTRU;
}

// 🔍 Memory utilities
void* SM_AllocatorEnhanced_getDataPointer(SM_AllocationEnhanced* allocation) {
    if (!allocation || !allocation->allocator) return NULL;
    
    // Update access tracking
    allocation->access_count++;
    allocation->last_access_time = (double)clock() / CLOCKS_PER_SEC;
    
    return allocation->allocator->storage + allocation->offset;
}

smBOL SM_AllocatorEnhanced_validateAllocation(SM_AllocationEnhanced* allocation) {
    if (!allocation || !allocation->allocator) return smFLS;
    
    SM_AllocatorEnhanced* allocator = allocation->allocator;
    
    // Check if allocation ID is valid and marked as allocated
    if (!SM_AllocatorEnhanced_isAllocated(allocator, allocation->id)) {
        printf("⚠️ Allocation %lu is not marked as allocated in BitSet!\n", allocation->id);
        return smFLS;
    }
    
    // Check bounds
    if (allocation->offset + allocation->size > allocator->storageSize) {
        printf("⚠️ Allocation %lu exceeds storage bounds!\n", allocation->id);
        return smFLS;
    }
    
    return smTRU;
}

// 📊 Statistics and performance monitoring
void SM_AllocatorEnhanced_printStats(SM_AllocatorEnhanced* allocator) {
    if (!allocator) return;
    
    printf("\n🎯 RANDY'S ENHANCED ALLOCATOR STATISTICS 🎯\n");
    printf("===============================================\n");
    
    // 📊 Memory usage
    printf("📊 Memory Usage:\n");
    printf("   Total Storage: %lu bytes\n", allocator->storageSize);
    printf("   Used Storage: %lu bytes (%.1f%%)\n", 
           allocator->storageUsed, 100.0 * allocator->efficiency_ratio);
    printf("   Free Storage: %lu bytes\n", allocator->storageSize - allocator->storageUsed);
    
    // 🎯 BitSet efficiency
    smU64 booleanMemory = allocator->allocationsSize;
    smU64 bitsetMemory = allocator->allocationsSize / 8;
    printf("\n🎯 Randy's BitSet Optimization:\n");
    printf("   Boolean Array Would Use: %lu bytes\n", booleanMemory);
    printf("   BitSet Actually Uses: %lu bytes\n", bitsetMemory);
    printf("   Memory Savings: %.1f%% (\"1 still better than 8\" validated!)\n",
           100.0 * (1.0 - (double)bitsetMemory / booleanMemory));
    
    // 📈 Performance metrics
    printf("\n📈 Performance Metrics:\n");
    printf("   Total Allocations: %lu\n", allocator->total_allocations);
    printf("   Total Deallocations: %lu\n", allocator->total_deallocations);
    printf("   Active Allocations: %lu\n", allocator->total_allocations - allocator->total_deallocations);
    printf("   Efficiency Ratio: %.3f\n", allocator->efficiency_ratio);
    printf("   Reusable Slots: %lu\n", allocator->unusedAllocationsUsed);
}

void SM_AllocatorEnhanced_printTypeStats(SM_AllocatorEnhanced* allocator) {
    if (!allocator) return;
    
    printf("\n🧠 TYPE TRACKING STATISTICS 🧠\n");
    printf("==============================\n");
    
    if (!allocator->typeTrackingGlobal) {
        printf("Type tracking is disabled.\n");
        return;
    }
    
    printf("Total Typed Allocations: %lu\n", allocator->totalTypedAllocations);
    
    for (int i = 0; i < 8; i++) {
        if (allocator->typeStatistics[i] > 0) {
            printf("   %s: %lu allocations\n", 
                   SM_AllocatorEnhanced_getTypeString((SM_DataType)i),
                   allocator->typeStatistics[i]);
        }
    }
}

double SM_AllocatorEnhanced_getEfficiency(SM_AllocatorEnhanced* allocator) {
    return allocator ? allocator->efficiency_ratio : 0.0;
}

// 🔧 Cleanup
smBOL SM_AllocatorEnhanced_destroy(SM_AllocatorEnhanced* allocator) {
    if (!allocator) return smFLS;
    
    printf("🧹 Destroying enhanced allocator...\n");
    
    // Clean up BitSets
    if (allocator->allocationFlags) {
        destroyBitSet(allocator->allocationFlags);
    }
    if (allocator->typeTrackingEnabled) {
        destroyBitSet(allocator->typeTrackingEnabled);
    }
    
    // Clean up memory
    sysfree(allocator->storage, allocator->storageSize);
    sysfree(allocator->allocations, sizeof(SM_AllocationEnhanced) * allocator->allocationsSize);
    sysfree(allocator->unusedAllocations, sizeof(SM_AllocationEnhanced) * allocator->unusedAllocationsSize);
    sysfree(allocator->free, sizeof(smU64) * allocator->freeSize);
    sysfree(allocator, sizeof(SM_AllocatorEnhanced));
    
    printf("✅ Enhanced allocator destroyed successfully!\n");
    return smTRU;
}