#include "inc/sm_allocator_sysmem.h"

    void *sysalloc(size_t size){
        void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if(ptr == MAP_FAILED){
            return NULL;
        }
        return ptr;
    }

    int sysfree(void *ptr, size_t size){
        return munmap(ptr, size);
    }

    void *sysrealloc(void *ptr,size_t oldSize, size_t newSize){
        void *newPtr = sysalloc(newSize);
        if(newPtr == NULL){
            return NULL;
        }
        syscpy(newPtr, ptr, oldSize);
        sysfree(ptr, oldSize);
        return newPtr;
    }

    void* syscalloc(size_t nmemb, size_t size){
        size_t totalSize = nmemb * size;
        void *ptr = sysalloc(totalSize);
        if(ptr == NULL){
            return NULL;
        }
        sysset(ptr, 0, totalSize);
        return ptr;
    }

    void* sysset(void *ptr, int value, size_t size){
        return memset(ptr, value, size);
    }

    void* syscpy(void *dest, const void *src, size_t size){
        return memcpy(dest, src, size);
    }

    void* sysmov(void *dest, const void *src, size_t size){
        return memmove(dest, src, size);
    }

    void* syszero(void *ptr, size_t size){
        return sysset(ptr, 0, size);
    }

    void* sysfill(void *ptr, int value, size_t size){
        return sysset(ptr, value, size);
    }

    __uint64_t calculateMemory(__uint64_t sizeInBytes){
        return (((sizeInBytes/SM_PAGESIZE) + (sizeInBytes % SM_PAGESIZE != 0 ? 1 : 0)) * SM_PAGESIZE);
    }