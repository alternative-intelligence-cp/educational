#ifndef SM_ALLOCATOR_MEMORY_H
    #define SM_ALLOCATOR_MEMORY_H
    #define _GNU_SOURCE     //for MAP_ANONYMOUS, MAP_PRIVATE, PROT_READ, PROT_WRITE, MAP_FAILED, PROT_EXEC,
    #include <stddef.h>     //for size_t, NULL
    #include <stdint.h>     //for uint8_t, uint16_t, uint32_t, uint64_t, 
    #include <stdbool.h>    //for bool, true, false
    #include <sys/mman.h>   //for mmap and munmap
    #include <string.h>     //for memcpy, memset, memmove
    #include <unistd.h>     //for sysconf and _SC_PAGESIZE

    //#define SM_PAGESIZE sysconf(_SC_PAGESIZE)
    #define SM_PAGESIZE 4096
    #define smU64 uint64_t
    #define smU32 uint32_t
    #define smU16 uint16_t
    #define smU08 uint8_t
    #define smI64 int64_t
    #define smI32 int32_t
    #define smI16 int16_t
    #define smI08 int8_t
    #define smCHR uint8_t
    #define smSTR uint8_t*
    #define smNUL NULL
    #define smPTR void*
    #define smBOL bool
    #define smTRU true
    #define smFLS false
    
    void *sysalloc(size_t size); //use os memory allocation
    int sysfree(void *ptr, size_t size); //use os memory deallocation
    void *sysrealloc(void *ptr,size_t oldSize, size_t newSize); //use os memory reallocation
    void* syscalloc(size_t nmemb, size_t size); //use os memory allocation and zero out the memory
    void* sysset(void *ptr, int value, size_t size); //use os memory set to set all bytes to a specific value
    void* syscpy(void *dest, const void *src, size_t size); //use os memory copy to copy memory from one location to another
    void* sysmov(void *dest, const void *src, size_t size); //use os memory move to move memory from one location to another
    void* syszero(void *ptr, size_t size); //use os memory set to zero all the bytes
    void* sysfill(void *ptr, int value, size_t size); //use os memory set to set all bytes to a specific value
    __uint64_t calculateMemory(__uint64_t sizeInBytes); //calculate the number of whole pages needed for a given size in bytes then multiply it by page size

#endif
