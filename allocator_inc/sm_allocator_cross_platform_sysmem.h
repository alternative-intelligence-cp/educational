/*
 * 🌍 RANDY'S CROSS-PLATFORM SYSTEM MEMORY INTERFACE 🌍
 * Supporting Windows, Linux, and macOS
 * 
 * Randy's Original Vision: "One allocator, all platforms!"
 * From first week learning C to cross-platform mastery!
 * 
 * Platform Support:
 * - 🐧 Linux: mmap/munmap (original)
 * - 🪟 Windows: VirtualAlloc/VirtualFree  
 * - 🍎 macOS: mmap/munmap (same as Linux)
 * - 📱 iOS/Android: mmap/munmap (same as Linux)
 */

#ifndef SM_ALLOCATOR_CROSS_PLATFORM_SYSMEM_H
#define SM_ALLOCATOR_CROSS_PLATFORM_SYSMEM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// 🎯 Randy's Cross-Platform Type Definitions
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

// 🌍 Platform Detection and Includes
#if defined(_WIN32) || defined(_WIN64)
    // 🪟 Windows Platform
    #define RANDY_PLATFORM_WINDOWS
    #include <windows.h>
    #define SM_PAGESIZE 4096  // Windows typical page size
    
#elif defined(__APPLE__)
    // 🍎 macOS/iOS Platform  
    #define RANDY_PLATFORM_APPLE
    #include <sys/mman.h>
    #include <unistd.h>
    #include <string.h>
    #define _GNU_SOURCE
    #define SM_PAGESIZE 4096  // macOS page size
    
#elif defined(__linux__)
    // 🐧 Linux Platform (original)
    #define RANDY_PLATFORM_LINUX
    #define _GNU_SOURCE
    #include <sys/mman.h>
    #include <unistd.h>
    #include <string.h>
    #define SM_PAGESIZE 4096  // Linux page size
    
#else
    // 🤖 Other Unix-like platforms (Android, FreeBSD, etc.)
    #define RANDY_PLATFORM_UNIX
    #include <sys/mman.h>
    #include <unistd.h>
    #include <string.h>
    #include <stdlib.h>
    #define SM_PAGESIZE 4096  // Default page size
    
#endif

// 🎯 Randy's Cross-Platform Function Declarations
void* sysalloc(size_t size);
int sysfree(void* ptr, size_t size);
void syscpy(void* dest, const void* src, size_t size);
smU64 calculateMemory(smU64 sizeInBytes);
const char* randy_get_platform_name(void);
void randy_show_platform_info(void);

#endif // SM_ALLOCATOR_CROSS_PLATFORM_SYSMEM_H