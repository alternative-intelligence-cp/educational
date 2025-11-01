/*
 * 🌍 RANDY'S CROSS-PLATFORM SYSTEM MEMORY IMPLEMENTATION 🌍
 * Supporting Windows, Linux, and macOS
 * 
 * "From segfault struggles to cross-platform success!"
 * - Randy's C Learning Journey, Week 1 to Production Ready
 */

#include "allocator_inc/sm_allocator_cross_platform_sysmem.h"

// 🎯 Randy's Cross-Platform Memory Allocation
void* sysalloc(size_t size) {
    void* ptr = NULL;
    
    printf("🧠 Randy's sysalloc: Requesting %zu bytes on %s\n", size, randy_get_platform_name());
    
#ifdef RANDY_PLATFORM_WINDOWS
    // 🪟 Windows Implementation
    ptr = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (ptr == NULL) {
        DWORD error = GetLastError();
        printf("❌ Windows VirtualAlloc failed with error %lu\n", error);
        return NULL;
    }
    printf("✅ Windows VirtualAlloc success: %p\n", ptr);
    
#elif defined(RANDY_PLATFORM_APPLE) || defined(RANDY_PLATFORM_LINUX) || defined(RANDY_PLATFORM_UNIX)
    // 🐧🍎🤖 Unix-like Implementation (Linux, macOS, Android, etc.)
    ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        perror("❌ Randy's mmap failed");
        return NULL;
    }
    printf("✅ Unix mmap success: %p\n", ptr);
    
#else
    // 🆘 Fallback to standard malloc (shouldn't happen with our platform detection)
    printf("⚠️ Fallback to standard malloc (unknown platform)\n");
    ptr = malloc(size);
    
#endif

    return ptr;
}

// 🎯 Randy's Cross-Platform Memory Deallocation
int sysfree(void* ptr, size_t size) {
    if (ptr == NULL) return 0;
    
    printf("🗑️ Randy's sysfree: Releasing %zu bytes from %p on %s\n", 
           size, ptr, randy_get_platform_name());
    
#ifdef RANDY_PLATFORM_WINDOWS
    // 🪟 Windows Implementation
    if (!VirtualFree(ptr, 0, MEM_RELEASE)) {
        DWORD error = GetLastError();
        printf("❌ Windows VirtualFree failed with error %lu\n", error);
        return -1;
    }
    printf("✅ Windows VirtualFree success\n");
    
#elif defined(RANDY_PLATFORM_APPLE) || defined(RANDY_PLATFORM_LINUX) || defined(RANDY_PLATFORM_UNIX)
    // 🐧🍎🤖 Unix-like Implementation
    if (munmap(ptr, size) != 0) {
        perror("❌ Randy's munmap failed");
        return -1;
    }
    printf("✅ Unix munmap success\n");
    
#else
    // 🆘 Fallback to standard free
    printf("⚠️ Fallback to standard free (unknown platform)\n");
    free(ptr);
    
#endif

    return 0;
}

// 🎯 Randy's Cross-Platform Memory Copy
void syscpy(void* dest, const void* src, size_t size) {
    // All platforms support memcpy
    memcpy(dest, src, size);
}

// 🎯 Randy's Page-Aligned Memory Calculation
smU64 calculateMemory(smU64 sizeInBytes) {
    // Calculate number of pages needed, then multiply by page size
    smU64 pages = (sizeInBytes + SM_PAGESIZE - 1) / SM_PAGESIZE;
    smU64 aligned_size = pages * SM_PAGESIZE;
    
    printf("📐 Randy's calculateMemory: %lu bytes -> %lu pages -> %lu aligned bytes\n",
           sizeInBytes, pages, aligned_size);
    
    return aligned_size;
}

// 🌍 Randy's Platform Information Functions
const char* randy_get_platform_name(void) {
#ifdef RANDY_PLATFORM_WINDOWS
    return "Windows";
#elif defined(RANDY_PLATFORM_APPLE)
    return "macOS/iOS";
#elif defined(RANDY_PLATFORM_LINUX)
    return "Linux";
#elif defined(RANDY_PLATFORM_UNIX)
    return "Unix-like";
#else
    return "Unknown";
#endif
}

void randy_show_platform_info(void) {
    printf("\n🌍 RANDY'S CROSS-PLATFORM SYSTEM INFO 🌍\n");
    printf("==========================================\n");
    printf("Platform: %s\n", randy_get_platform_name());
    printf("Page Size: %d bytes\n", SM_PAGESIZE);
    
#ifdef RANDY_PLATFORM_WINDOWS
    printf("Memory API: VirtualAlloc/VirtualFree\n");
    printf("Architecture: Windows NT Kernel\n");
    
    // Windows-specific info
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    printf("Processor Count: %lu\n", sysInfo.dwNumberOfProcessors);
    printf("Page Size (actual): %lu bytes\n", sysInfo.dwPageSize);
    printf("Allocation Granularity: %lu bytes\n", sysInfo.dwAllocationGranularity);
    
#elif defined(RANDY_PLATFORM_APPLE)
    printf("Memory API: mmap/munmap (BSD-style)\n");
    printf("Architecture: Darwin/XNU Kernel\n");
    
    // Try to get system info
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size > 0) {
        printf("Page Size (actual): %ld bytes\n", page_size);
    }
    
#elif defined(RANDY_PLATFORM_LINUX)
    printf("Memory API: mmap/munmap (Linux-style)\n");
    printf("Architecture: Linux Kernel\n");
    
    // Linux-specific info
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size > 0) {
        printf("Page Size (actual): %ld bytes\n", page_size);
    }
    
    long processors = sysconf(_SC_NPROCESSORS_ONLN);
    if (processors > 0) {
        printf("Processor Count: %ld\n", processors);
    }
    
#else
    printf("Memory API: mmap/munmap (Generic Unix)\n");
    printf("Architecture: Generic Unix-like\n");
    
#endif

    printf("==========================================\n\n");
}

// 🎯 Randy's Memory System Initialization
void randy_init_memory_system(void) {
    printf("🚀 Initializing Randy's Cross-Platform Memory System...\n");
    randy_show_platform_info();
    
    // Test a small allocation to verify everything works
    void* test_ptr = sysalloc(SM_PAGESIZE);
    if (test_ptr) {
        printf("✅ Memory system test allocation successful!\n");
        sysfree(test_ptr, SM_PAGESIZE);
        printf("✅ Memory system test deallocation successful!\n");
    } else {
        printf("❌ Memory system test failed!\n");
    }
    
    printf("🎯 Randy's Cross-Platform Memory System ready!\n\n");
}