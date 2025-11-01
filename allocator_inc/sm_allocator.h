#ifndef SM_ALLOCATOR_H
    #define SM_ALLOCATOR_H
    #include "sm_allocator_sysmem.h"
   
    #define SM_ALLOCATED true
    #define SM_UNALLOCATED false
    #define SM_ALLOCATOR_ERROR UINT64_MAX

    typedef struct sm_allocator_allocation_t SM_Allocation;
    typedef struct sm_allocator_allocation_meta_t SM_AllocationMeta;
    typedef struct sm_allocator_t SM_Allocator;


    typedef struct sm_allocator_allocation_t{
        smU64   id;//the id of the allocation
        smU64   offset;//the offset in bytes from the start of the storage
        smU64   size;//the size of the allocation in bytes
        smBOL   allocated;//is the allocation currently allocated
        SM_Allocator* allocator;//the allocator that created this allocation
    } SM_Allocation;
    
   
    
    typedef struct sm_allocator_t{
        smU64   storageLastId;//the last id used for an allocation
        smU64   storageSize;//the current amount of space we have available in the storage
        smU64   storageUsed;//the amount of space we have allocated in the storage
        smU08*  storage;//the actual storage space

        smU64   allocationsSize;//the current amount of space we have to store meta data
        smU64   allocationsUsed;//how much of the space we have to store meta data is used
        SM_Allocation* allocations;//stores the meta data for each allocation

        smU64   unusedAllocationsSize;//the current amount of space we have to store unused allocations
        smU64   unusedAllocationsUsed;//how much of the space we have to store unused allocations is used
        SM_Allocation* unusedAllocations;//the ids of allocations that no longer point to memory, reuse rather than make more

        smU64   freeMax;   //the largest free allocation available
        smU64   freeSize; //the current amount of space we have to store free allocationIds
        smU64   freeUsed; //how much of the space we have to store free allocationIds is used
        smU64*  free; //stores the allocationId of the free space
    }SM_Allocator;



    SM_Allocator* SM_Allocator_create(smU64 storageSize);
    SM_Allocation* SM_Allocator_allocate(SM_Allocator* allocator,smU64 allocationSize);
    smBOL SM_Allocator_deallocate(SM_Allocation* allocation);
    smBOL SM_Allocator_destroy(SM_Allocator* allocator);
    smBOL SM_Allocator_resize(SM_Allocator* allocator, SM_Allocation* allocation, smU64 newSize);
    smBOL SM_Allocator_reallocate(SM_Allocator* allocator, SM_Allocation* allocation, smU64 newSize);
    smBOL SM_Allocator_isValid(SM_Allocator* allocator, SM_Allocation* allocation);
    smBOL SM_Allocator_isAllocated(SM_Allocation* allocation);
    SM_Allocation* SM_Allocator_getAllocation(SM_Allocator* allocator, smU64 id);
    smBOL SM_Allocator_isDeallocated(SM_Allocation* allocation);
    smU64 SM_Allocator_getSize(SM_Allocation* allocation);
    smU64 SM_Allocator_getId(SM_Allocation* allocation);
    smU64 SM_Allocator_getOffset(SM_Allocation* allocation);
    smU08* SM_Allocator_getPointer(SM_Allocation* allocation);
    smU64 SM_Allocator_getStorageSize(SM_Allocator* allocator);
    smU64 SM_Allocator_getStorageUsed(SM_Allocator* allocator);
    smBOL SM_Allocator_growStorage(SM_Allocator* allocator, smU64 minNewBytes);
    smBOL SM_Allocator_shrinkStorage(SM_Allocator* allocator, smU64 maxNewBytes);
    smU64 SM_Allocator_getAllocationsSize(SM_Allocator* allocator);
    smU64 SM_Allocator_getAllocationsUsed(SM_Allocator* allocator);
    smBOL SM_Allocator_growAllocations(SM_Allocator* allocator, smU64 minNewBytes);;
    smBOL SM_Allocator_getUnusedAllocationsSize(SM_Allocator* allocator);
    smBOL SM_Allocator_getUnusedAllocationsUsed(SM_Allocator* allocator);   
    smBOL SM_Allocator_growUnusedAllocations(SM_Allocator* allocator, smU64 minNewBytes);
    smU64 SM_Allocator_getFreeSize(SM_Allocator* allocator);
    smU64 SM_Allocator_getFreeUsed(SM_Allocator* allocator);
    smBOL SM_Allocator_growFree(SM_Allocator* allocator, smU64 minNewBytes);
    smU64 SM_Allocator_getFreeMax(SM_Allocator* allocator);
    smBOL SM_Allocator_updateFreeMax(SM_Allocator* allocator);;
    smBOL SM_Allocator_isValidFree(SM_Allocator* allocator, SM_Allocation* allocation);
    SM_Allocation* SM_Allocator_allocateStorage(SM_Allocator* allocator, smU64 allocationSize);
    smBOL SM_Allocator_mergeAllocations(SM_Allocation* allocation1, SM_Allocation* allocation2);
    SM_Allocation* SM_Allocator_splitAllocation(SM_Allocator* allocator, SM_Allocation* allocation, smU64 newSize);
    smBOL SM_Allocator_compress(SM_Allocator* allocator);
    
    







#endif