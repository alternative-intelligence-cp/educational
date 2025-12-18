/**
 * ApexAlloc Global Initialization
 * 
 * This module handles one-time initialization:
 * - Entropy generation for security
 * - CPU feature detection
 * - Global secrets initialization
 * - NUMA node detection
 */

#include "apex_internal.h"
#include "apex_structures.h"
#include <sys/random.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <fcntl.h>
#include <cpuid.h>
#include <string.h>
#include <time.h>

/* ============================================================================
 * Global State
 * ============================================================================ */

// Process-wide secrets (initialized once)
uint64_t g_process_cookie = 0;          // For checksums
uint64_t g_pointer_key = 0;             // For safe-linking
uint64_t g_heap_base_mask = 0;          // For ASLR

// CPU capabilities
bool g_has_crc32 = false;               // Hardware CRC32 support
bool g_has_huge_pages = false;         // Huge page support
int g_numa_node_count = 1;              // Number of NUMA nodes

// Statistics
_Atomic uint64_t g_total_allocated = 0;
_Atomic uint64_t g_total_freed = 0;
_Atomic uint64_t g_peak_allocated = 0;

/* ============================================================================
 * Entropy Generation
 * ============================================================================ */

/**
 * Get cryptographically secure random bytes.
 * Uses /dev/urandom for portability.
 */
static bool apex_get_random_bytes(void* buffer, size_t size) {
    // Read from /dev/urandom
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        return false;
    }
    
    ssize_t bytes_read = read(fd, buffer, size);
    close(fd);
    
    return bytes_read == (ssize_t)size;
}

/**
 * Initialize global secrets for security features.
 */
static void apex_init_secrets(void) {
    // Generate process cookie for checksums
    if (!apex_get_random_bytes(&g_process_cookie, sizeof(g_process_cookie))) {
        // Fallback: use process ID and timestamp (less secure but functional)
        g_process_cookie = (uint64_t)getpid() ^ (uint64_t)time(NULL);
        apex_debug_log("Warning: Using weak entropy for process cookie\n");
    }
    
    // Generate pointer obfuscation key
    if (!apex_get_random_bytes(&g_pointer_key, sizeof(g_pointer_key))) {
        g_pointer_key = g_process_cookie ^ 0xDEADBEEFCAFEBABEUL;
        apex_debug_log("Warning: Using weak entropy for pointer key\n");
    }
    
    // Generate heap base mask for ASLR
    if (!apex_get_random_bytes(&g_heap_base_mask, sizeof(g_heap_base_mask))) {
        g_heap_base_mask = g_process_cookie ^ 0x123456789ABCDEF0UL;
    }
    
    // Keep only the lower bits for reasonable offsets (0-16MB range)
    g_heap_base_mask &= 0xFFFFFF;
    
    apex_debug_log("Secrets initialized: cookie=%016lx key=%016lx\n",
                   g_process_cookie, g_pointer_key);
}

/* ============================================================================
 * CPU Feature Detection
 * ============================================================================ */

/**
 * Check if CPU supports SSE 4.2 (includes CRC32 instruction).
 */
static bool apex_detect_crc32_support(void) {
#if defined(__x86_64__) || defined(__i386__)
    unsigned int eax, ebx, ecx, edx;
    
    // Check if CPUID is available
    if (!__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
        return false;
    }
    
    // SSE 4.2 support is indicated by ECX bit 20
    return (ecx & (1 << 20)) != 0;
#elif defined(__aarch64__)
    // On ARM, CRC32 is an optional extension
    // Check via getauxval(AT_HWCAP) in a real implementation
    // For educational purposes, assume available on modern ARM
    return true;
#else
    return false;
#endif
}

/**
 * Detect huge page support.
 * Checks if the system supports transparent huge pages.
 */
static bool apex_detect_huge_pages(void) {
    // Check for THP support on Linux
    int fd = open("/sys/kernel/mm/transparent_hugepage/enabled", O_RDONLY);
    if (fd < 0) {
        return false;
    }
    
    char buf[64];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    close(fd);
    
    if (n <= 0) {
        return false;
    }
    
    buf[n] = '\0';
    
    // THP is enabled if the string contains "[always]" or "[madvise]"
    return (strstr(buf, "[always]") != NULL || strstr(buf, "[madvise]") != NULL);
}

/**
 * Detect number of NUMA nodes.
 */
static int apex_detect_numa_nodes(void) {
    // Try to read from /sys/devices/system/node/online
    int fd = open("/sys/devices/system/node/online", O_RDONLY);
    if (fd < 0) {
        return 1;  // Assume single node
    }
    
    char buf[64];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    close(fd);
    
    if (n <= 0) {
        return 1;
    }
    
    buf[n] = '\0';
    
    // Parse format like "0-3" or "0,2-3"
    // For simplicity, just check if we have a dash (indicating range)
    if (strchr(buf, '-') != NULL) {
        // Has range, likely multiple nodes
        return 2;  // Conservative estimate
    }
    
    return 1;
}

/**
 * Initialize CPU feature detection.
 */
static void apex_init_cpu_features(void) {
    g_has_crc32 = apex_detect_crc32_support();
    g_has_huge_pages = apex_detect_huge_pages();
    g_numa_node_count = apex_detect_numa_nodes();
    
    apex_debug_log("CPU features: CRC32=%d HugePages=%d NUMA=%d\n",
                   g_has_crc32, g_has_huge_pages, g_numa_node_count);
}

/* ============================================================================
 * Main Initialization
 * ============================================================================ */

/**
 * Perform one-time global initialization.
 * Called by apex_init_once() from apex_malloc.c.
 */
void apex_global_init(void) {
    apex_debug_log("Global initialization started\n");
    
    // Initialize entropy and secrets
    apex_init_secrets();
    
    // Detect CPU features
    apex_init_cpu_features();
    
    // Initialize global statistics
    atomic_store(&g_total_allocated, 0);
    atomic_store(&g_total_freed, 0);
    atomic_store(&g_peak_allocated, 0);
    
    apex_debug_log("Global initialization complete\n");
}

/* ============================================================================
 * Accessor Functions
 * ============================================================================ */

/**
 * Get the process cookie for checksum calculations.
 */
uint64_t apex_get_process_cookie(void) {
    return g_process_cookie;
}

/**
 * Get the pointer obfuscation key.
 */
uint64_t apex_get_pointer_key(void) {
    return g_pointer_key;
}

/**
 * Check if hardware CRC32 is available.
 */
bool apex_has_crc32_support(void) {
    return g_has_crc32;
}

/**
 * Check if huge pages are available.
 */
bool apex_has_huge_page_support(void) {
    return g_has_huge_pages;
}

/**
 * Get number of NUMA nodes.
 */
int apex_get_numa_node_count(void) {
    return g_numa_node_count;
}
