# ApexAlloc Tutorial: Build Your Own Production-Grade Memory Allocator

## 🎓 Welcome!

Welcome to the most comprehensive memory allocator tutorial available. This guide will take you from zero to a fully-functional, production-grade allocator that rivals mimalloc, snmalloc, and jemalloc.

## 📋 Prerequisites

Before starting, you should be comfortable with:

- **C Programming**: Pointers, structs, function pointers
- **Basic Concurrency**: Threads, mutexes (we'll teach lock-free!)
- **Linux System Calls**: Basic familiarity with mmap, madvise
- **Computer Architecture**: Caches, TLB, memory hierarchy basics

**Don't worry if you're rusty!** Each phase includes detailed explanations.

## 🗺️ Tutorial Structure

Each phase follows this structure:

1. **🎯 Learning Objectives** - What you'll master
2. **📖 Conceptual Overview** - The "why" behind the design
3. **🔧 Implementation Guide** - Step-by-step coding
4. **✅ Checkpoints** - Verify your progress
5. **🎓 Deep Dive** - Advanced concepts and optimizations
6. **💡 Exercises** - Solidify your understanding

## 📚 Phase Overview

| Phase | Focus | Estimated Time |
|-------|-------|----------------|
| [Phase I](#phase-i-build-system--bootstrapping) | Build System & Bootstrapping | 2-3 hours |
| [Phase II](#phase-ii-memory-hierarchy) | Data Structures | 3-4 hours |
| [Phase III](#phase-iii-global-initialization) | Global State | 2-3 hours |
| [Phase IV](#phase-iv-superblock-management) | OS Interface | 2-3 hours |
| [Phase V](#phase-v-fast-path-allocation) | Thread-Local Allocation | 4-5 hours |
| [Phase VI](#phase-vi-concurrency) | Lock-Free Remote Free | 5-6 hours |
| [Phase VII](#phase-vii-security-hardening) | Security Features | 3-4 hours |
| [Phase VIII](#phase-viii-memory-reclamation) | Memory Management | 3-4 hours |
| [Phase IX](#phase-ix-introspection) | Observability | 2-3 hours |
| [Phase X](#phase-x-testing--verification) | Testing & Benchmarks | 3-4 hours |

**Total**: ~30-40 hours (can be done over several weeks)

---

# Phase I: Build System & Bootstrapping

## 🎯 Learning Objectives

By the end of this phase, you will:

- ✅ Configure a build system for a shared library allocator
- ✅ Understand symbol visibility and interposition
- ✅ Solve the "bootstrapping paradox"
- ✅ Implement a pre-initialization allocator

## 📖 Conceptual Overview

### The Challenge

Memory allocators face a unique chicken-and-egg problem:

```
Dynamic Linker: "I need to load libapexalloc.so"
Dynamic Linker: "To load it, I need to allocate memory for its structures"
Dynamic Linker: "I'll call malloc()..."
ApexAlloc: "Oh, you called me! Let me initialize..."
ApexAlloc: "Wait, I need memory to initialize!"
💥 CRASH (Stack overflow)
```

### The Solution: Bootstrap Allocator

We implement a **static buffer** that serves memory during initialization:

```c
static uint8_t g_bootstrap_heap[256 * 1024];  // 256KB static buffer
static _Atomic size_t g_bootstrap_offset = 0;
static _Atomic bool g_fully_initialized = false;
```

**How it works:**
1. First `malloc()` call → use bootstrap heap (bump allocator)
2. Bootstrap allocations leak (but they're permanent linker structures)
3. Complete initialization
4. Set `g_fully_initialized = true`
5. All future calls → normal allocator

## 🔧 Implementation Guide

### Step 1: Create the Makefile

Create `Makefile` in the project root:

```makefile
# ApexAlloc Makefile
# Educational memory allocator

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror \
         -fPIC \
         -fvisibility=hidden \
         -fno-builtin-malloc -fno-builtin-free \
         -fno-builtin-calloc -fno-builtin-realloc \
         -O2 -march=native

DEBUG_FLAGS = -g -O0 -DDEBUG -fsanitize=address -fsanitize=undefined
LDFLAGS = -shared -pthread
DEBUG_LDFLAGS = $(LDFLAGS) -fsanitize=address -fsanitize=undefined

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
TEST_DIR = tests
BENCH_DIR = benchmarks

# Output
LIB = $(BUILD_DIR)/libapexalloc.so
LIB_DEBUG = $(BUILD_DIR)/libapexalloc_debug.so

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
OBJS_DEBUG = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_debug.o)

# Test files
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_BINS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(BUILD_DIR)/test_%)

# Benchmark files
BENCH_SRCS = $(wildcard $(BENCH_DIR)/*.c)
BENCH_BINS = $(BENCH_SRCS:$(BENCH_DIR)/%.c=$(BUILD_DIR)/bench_%)

.PHONY: all clean test bench debug help

all: $(BUILD_DIR) $(LIB)

debug: $(BUILD_DIR) $(LIB_DEBUG)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build shared library (release)
$(LIB): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
	@echo "✅ Built release library: $@"

# Build shared library (debug)
$(LIB_DEBUG): $(OBJS_DEBUG)
	$(CC) $(DEBUG_LDFLAGS) -o $@ $^
	@echo "✅ Built debug library: $@"

# Compile object files (release)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# Compile object files (debug)
$(BUILD_DIR)/%_debug.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -I$(INC_DIR) -c $< -o $@

# Build tests
test: $(LIB) $(TEST_BINS)
	@echo "🧪 Running tests..."
	@for test in $(TEST_BINS); do \
		echo "Running $$test..."; \
		LD_PRELOAD=$(LIB) $$test || exit 1; \
	done
	@echo "✅ All tests passed!"

$(BUILD_DIR)/test_%: $(TEST_DIR)/%.c $(LIB)
	$(CC) $(CFLAGS) -I$(INC_DIR) $< -o $@ -L$(BUILD_DIR) -lapexalloc

# Build benchmarks
bench: $(LIB) $(BENCH_BINS)
	@echo "📊 Running benchmarks..."
	@for bench in $(BENCH_BINS); do \
		echo "Running $$bench..."; \
		LD_PRELOAD=$(LIB) $$bench; \
	done

$(BUILD_DIR)/bench_%: $(BENCH_DIR)/%.c $(LIB)
	$(CC) $(CFLAGS) -I$(INC_DIR) $< -o $@ -L$(BUILD_DIR) -lapexalloc

# Clean
clean:
	rm -rf $(BUILD_DIR)
	@echo "🧹 Cleaned build directory"

# Help
help:
	@echo "ApexAlloc Build System"
	@echo "====================="
	@echo ""
	@echo "Targets:"
	@echo "  make          - Build release library"
	@echo "  make debug    - Build debug library with sanitizers"
	@echo "  make test     - Build and run tests"
	@echo "  make bench    - Build and run benchmarks"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make help     - Show this help"
	@echo ""
	@echo "Usage with LD_PRELOAD:"
	@echo "  LD_PRELOAD=./build/libapexalloc.so ./your_program"
```

### Step 2: Create the Public API Header

Create `include/apexalloc.h`:

```c
/**
 * ApexAlloc - Educational High-Performance Memory Allocator
 * Public API Header
 */

#ifndef APEXALLOC_H
#define APEXALLOC_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Standard Memory Allocation API
 * ============================================================================
 * These functions replace the standard libc malloc family.
 */

/**
 * Allocate memory of at least `size` bytes.
 * Returns NULL if allocation fails.
 */
void* malloc(size_t size) __attribute__((malloc, alloc_size(1)));

/**
 * Free memory allocated by malloc/calloc/realloc.
 * Calling free(NULL) is safe and does nothing.
 */
void free(void* ptr);

/**
 * Allocate zeroed memory for `nmemb` elements of `size` bytes each.
 */
void* calloc(size_t nmemb, size_t size) 
    __attribute__((malloc, alloc_size(1, 2)));

/**
 * Resize allocation. Returns NULL on failure (original allocation unchanged).
 */
void* realloc(void* ptr, size_t size) 
    __attribute__((alloc_size(2)));

/**
 * Allocate aligned memory.
 * `alignment` must be a power of 2 and a multiple of sizeof(void*).
 */
int posix_memalign(void** memptr, size_t alignment, size_t size);

/**
 * Allocate aligned memory (C11).
 */
void* aligned_alloc(size_t alignment, size_t size)
    __attribute__((malloc, alloc_size(2), alloc_align(1)));

/**
 * Get the usable size of an allocation.
 * This may be larger than the requested size due to rounding.
 */
size_t malloc_usable_size(void* ptr);

/* ============================================================================
 * ApexAlloc Extended API
 * ============================================================================
 * Educational introspection and control interface.
 */

/**
 * Introspection and control (inspired by jemalloc's mallctl)
 * 
 * Usage:
 *   size_t allocated;
 *   size_t len = sizeof(allocated);
 *   apex_ctl("stats.allocated", &allocated, &len, NULL, 0);
 * 
 * Common operations:
 *   "stats.allocated"   - Total bytes allocated
 *   "stats.active"      - Active allocations
 *   "stats.resident"    - Resident set size
 *   "arena.purge"       - Force memory return to OS
 *   "thread.reset"      - Reset thread-local cache
 */
int apex_ctl(const char* operation, 
             void* oldp, size_t* oldlenp,
             void* newp, size_t newlen);

/**
 * Per-thread statistics structure
 */
typedef struct {
    uint64_t malloc_count;        // Total mallocs
    uint64_t free_count;          // Total frees
    uint64_t bytes_allocated;     // Total bytes allocated
    uint64_t bytes_freed;         // Total bytes freed
    uint64_t remote_frees;        // Frees from other threads
    uint64_t cache_hits;          // Fast path hits
    uint64_t cache_misses;        // Slow path misses
} apex_thread_stats_t;

/**
 * Get statistics for the current thread
 */
void apex_thread_stats(apex_thread_stats_t* stats);

/**
 * Global statistics structure
 */
typedef struct {
    uint64_t superblocks_allocated;
    uint64_t superblocks_active;
    uint64_t blocks_allocated;
    uint64_t blocks_active;
    uint64_t total_allocated_bytes;
    uint64_t total_freed_bytes;
    uint64_t peak_allocated_bytes;
    uint64_t fragmentation_percent;  // Internal fragmentation (0-100)
} apex_global_stats_t;

/**
 * Get global allocator statistics
 */
void apex_global_stats(apex_global_stats_t* stats);

/**
 * Heap visualization callback
 * Called for each active block with allocation information
 */
typedef void (*apex_heap_visitor_t)(
    void* block_addr,
    size_t block_size,
    size_t size_class,
    size_t used_chunks,
    size_t total_chunks,
    void* userdata
);

/**
 * Walk the heap and call visitor for each block
 * Useful for visualization and debugging
 */
void apex_heap_walk(apex_heap_visitor_t visitor, void* userdata);

#ifdef __cplusplus
}
#endif

#endif /* APEXALLOC_H */
```

### Step 3: Create the Bootstrap Allocator

Create `src/apex_bootstrap.c`:

```c
/**
 * ApexAlloc Bootstrap Allocator
 * 
 * This module handles the "cold start" problem where the dynamic linker
 * needs to allocate memory before our allocator is fully initialized.
 * 
 * Solution: Provide a static buffer and bump-pointer allocator.
 */

#include "apex_internal.h"
#include <stdatomic.h>
#include <string.h>

/* ============================================================================
 * Bootstrap Heap Configuration
 * ============================================================================ */

// Static buffer for early allocations (256KB should be plenty)
#define BOOTSTRAP_HEAP_SIZE (256 * 1024)

static uint8_t g_bootstrap_heap[BOOTSTRAP_HEAP_SIZE] __attribute__((aligned(16)));
static _Atomic size_t g_bootstrap_offset = 0;

// Track what was allocated for debugging
#ifdef DEBUG
static _Atomic size_t g_bootstrap_alloc_count = 0;
#endif

/* ============================================================================
 * Bootstrap Allocator Implementation
 * ============================================================================ */

/**
 * Simple bump-pointer allocator for bootstrap phase.
 * 
 * This is called when:
 * 1. Dynamic linker is loading our library
 * 2. Main allocator is not yet initialized
 * 
 * Returns NULL if bootstrap heap is exhausted.
 */
void* apex_bootstrap_malloc(size_t size) {
    // Align size to 16 bytes (max_align_t)
    size = (size + 15) & ~15;
    
    // Atomic fetch-and-add for thread safety
    size_t offset = atomic_fetch_add_explicit(
        &g_bootstrap_offset, 
        size, 
        memory_order_relaxed
    );
    
    // Check if we overflowed the bootstrap heap
    if (offset + size > BOOTSTRAP_HEAP_SIZE) {
        // This is a fatal error - should never happen in practice
        // The dynamic linker doesn't allocate that much
        return NULL;
    }
    
    void* ptr = &g_bootstrap_heap[offset];
    
#ifdef DEBUG
    atomic_fetch_add(&g_bootstrap_alloc_count, 1);
    apex_debug_log("Bootstrap alloc: %zu bytes at %p (count: %zu)\n", 
                   size, ptr, atomic_load(&g_bootstrap_alloc_count));
#endif
    
    return ptr;
}

/**
 * Bootstrap "free" - does nothing!
 * 
 * Memory allocated from bootstrap heap cannot be freed.
 * This is OK because:
 * 1. Bootstrap allocations are typically dynamic linker internals
 * 2. They persist for the lifetime of the process
 * 3. Total size is small (typically < 10KB)
 */
void apex_bootstrap_free(void* ptr) {
    // Check if this pointer is from bootstrap heap
    if (ptr >= (void*)g_bootstrap_heap && 
        ptr < (void*)(g_bootstrap_heap + BOOTSTRAP_HEAP_SIZE)) {
        // Bootstrap allocations cannot be freed
        // This is intentional and expected
#ifdef DEBUG
        apex_debug_log("Bootstrap free ignored: %p\n", ptr);
#endif
        return;
    }
    
    // If not from bootstrap heap, this is an error
    // Should not happen if initialization is correct
#ifdef DEBUG
    apex_debug_log("ERROR: free() called on non-bootstrap pointer before init: %p\n", ptr);
#endif
}

/**
 * Check if a pointer belongs to the bootstrap heap
 */
bool apex_is_bootstrap_pointer(const void* ptr) {
    return (ptr >= (void*)g_bootstrap_heap && 
            ptr < (void*)(g_bootstrap_heap + BOOTSTRAP_HEAP_SIZE));
}

/**
 * Get bootstrap heap statistics (for debugging/education)
 */
void apex_bootstrap_stats(size_t* used, size_t* total) {
    *used = atomic_load_explicit(&g_bootstrap_offset, memory_order_relaxed);
    *total = BOOTSTRAP_HEAP_SIZE;
}
```

### Step 4: Create the Internal Header

Create `src/apex_internal.h`:

```c
/**
 * ApexAlloc Internal Header
 * Private definitions and utilities
 */

#ifndef APEX_INTERNAL_H
#define APEX_INTERNAL_H

#include "../include/apexalloc.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <pthread.h>

/* ============================================================================
 * Configuration Constants
 * ============================================================================ */

// Superblock (Segment) configuration
#define SUPERBLOCK_SIZE (4 * 1024 * 1024)  // 4MB
#define SUPERBLOCK_ALIGN (2 * 1024 * 1024) // 2MB (huge page size)

// Block (Page) configuration
#define BLOCK_SIZE (64 * 1024)  // 64KB
#define BLOCKS_PER_SUPERBLOCK (SUPERBLOCK_SIZE / BLOCK_SIZE)

// Size class configuration
#define SIZE_CLASS_COUNT 64
#define SIZE_CLASS_MIN 16
#define SIZE_CLASS_MAX (32 * 1024)  // 32KB (larger goes to large alloc)

// Security configuration
#define ENABLE_CHECKSUMS 1
#define ENABLE_SAFE_LINKING 1
#define ENABLE_DOUBLE_FREE_CHECK 1

/* ============================================================================
 * Initialization State
 * ============================================================================ */

extern _Atomic bool g_apex_initialized;

/* ============================================================================
 * Bootstrap Allocator
 * ============================================================================ */

void* apex_bootstrap_malloc(size_t size);
void apex_bootstrap_free(void* ptr);
bool apex_is_bootstrap_pointer(const void* ptr);
void apex_bootstrap_stats(size_t* used, size_t* total);

/* ============================================================================
 * Initialization
 * ============================================================================ */

void apex_init_once(void);

/* ============================================================================
 * Debugging
 * ============================================================================ */

#ifdef DEBUG
#include <stdio.h>
#define apex_debug_log(fmt, ...) \
    fprintf(stderr, "[ApexAlloc] " fmt, ##__VA_ARGS__)
#else
#define apex_debug_log(fmt, ...) ((void)0)
#endif

/* ============================================================================
 * Compiler Hints
 * ============================================================================ */

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define ALWAYS_INLINE __attribute__((always_inline)) inline

/* ============================================================================
 * Utility Macros
 * ============================================================================ */

#define ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))
#define ALIGN_DOWN(x, align) ((x) & ~((align) - 1))
#define IS_ALIGNED(x, align) (((uintptr_t)(x) & ((align) - 1)) == 0)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#endif /* APEX_INTERNAL_H */
```

### Step 5: Create the Main Entry Point

Create `src/apex_malloc.c`:

```c
/**
 * ApexAlloc - Main Entry Point
 * 
 * This file contains the primary malloc/free implementation
 * with bootstrap handling.
 */

#include "apex_internal.h"
#include <stdatomic.h>

/* ============================================================================
 * Global Initialization State
 * ============================================================================ */

_Atomic bool g_apex_initialized = false;
static pthread_once_t g_init_once = PTHREAD_ONCE_INIT;

/* ============================================================================
 * Initialization
 * ============================================================================ */

/**
 * One-time initialization of the allocator.
 * Called automatically on first malloc().
 */
void apex_init_once(void) {
    apex_debug_log("ApexAlloc initialization started\n");
    
    // TODO: Initialize global state (Phase III)
    // TODO: Initialize size classes
    // TODO: Initialize entropy/secrets
    // TODO: Detect CPU features
    
    // Mark as initialized
    atomic_store_explicit(&g_apex_initialized, true, memory_order_release);
    
    apex_debug_log("ApexAlloc initialization complete\n");
}

/**
 * Ensure allocator is initialized (thread-safe, happens once)
 */
static ALWAYS_INLINE void ensure_initialized(void) {
    if (unlikely(!atomic_load_explicit(&g_apex_initialized, memory_order_acquire))) {
        pthread_once(&g_init_once, apex_init_once);
    }
}

/* ============================================================================
 * Main Allocation Functions (Bootstrap Phase)
 * ============================================================================ */

/**
 * malloc() implementation
 */
void* malloc(size_t size) {
    // Check if we're initialized
    if (unlikely(!atomic_load_explicit(&g_apex_initialized, memory_order_acquire))) {
        // Not initialized yet - use bootstrap allocator
        return apex_bootstrap_malloc(size);
    }
    
    // TODO: Normal allocation path (Phase V)
    return NULL;  // Placeholder
}

/**
 * free() implementation
 */
void free(void* ptr) {
    if (ptr == NULL) {
        return;  // free(NULL) is a no-op
    }
    
    // Check if this is a bootstrap pointer
    if (apex_is_bootstrap_pointer(ptr)) {
        apex_bootstrap_free(ptr);
        return;
    }
    
    // Check if we're initialized
    if (unlikely(!atomic_load_explicit(&g_apex_initialized, memory_order_acquire))) {
        // Shouldn't happen, but be safe
        return;
    }
    
    // TODO: Normal free path (Phase V)
}

/**
 * calloc() implementation
 */
void* calloc(size_t nmemb, size_t size) {
    // Check for overflow
    if (nmemb != 0 && size > SIZE_MAX / nmemb) {
        return NULL;
    }
    
    size_t total = nmemb * size;
    void* ptr = malloc(total);
    
    if (ptr) {
        // Zero the memory
        __builtin_memset(ptr, 0, total);
    }
    
    return ptr;
}

/**
 * realloc() implementation
 */
void* realloc(void* ptr, size_t size) {
    if (ptr == NULL) {
        return malloc(size);
    }
    
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    
    // TODO: Optimized realloc (Phase V)
    // For now, simple copy
    void* new_ptr = malloc(size);
    if (new_ptr) {
        // TODO: Get actual old size, for now assume worst case
        size_t old_size = size;  // Placeholder
        __builtin_memcpy(new_ptr, ptr, MIN(size, old_size));
        free(ptr);
    }
    
    return new_ptr;
}

/**
 * posix_memalign() implementation
 */
int posix_memalign(void** memptr, size_t alignment, size_t size) {
    // Validate alignment
    if (alignment % sizeof(void*) != 0 || 
        (alignment & (alignment - 1)) != 0) {
        return 22;  // EINVAL
    }
    
    ensure_initialized();
    
    // TODO: Aligned allocation (Phase V)
    *memptr = NULL;
    return 12;  // ENOMEM (not implemented yet)
}

/**
 * aligned_alloc() implementation (C11)
 */
void* aligned_alloc(size_t alignment, size_t size) {
    void* ptr = NULL;
    posix_memalign(&ptr, alignment, size);
    return ptr;
}

/**
 * malloc_usable_size() implementation
 */
size_t malloc_usable_size(void* ptr) {
    if (ptr == NULL) {
        return 0;
    }
    
    // Check bootstrap pointer
    if (apex_is_bootstrap_pointer(ptr)) {
        return 0;  // Unknown
    }
    
    // TODO: Return actual usable size (Phase V)
    return 0;  // Placeholder
}
```

## ✅ Checkpoint 1: Build the Bootstrap Phase

Let's verify everything compiles:

```bash
cd /path/to/apexalloc
make clean
make
```

Expected output:
```
✅ Built release library: build/libapexalloc.so
```

### Test the Bootstrap

Create `tests/test_bootstrap.c`:

```c
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main() {
    printf("Testing bootstrap allocator...\n");
    
    // This will trigger bootstrap allocations
    void* handle = dlopen("./build/libapexalloc.so", RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        fprintf(stderr, "dlopen failed: %s\n", dlerror());
        return 1;
    }
    
    printf("✅ Bootstrap allocator survived dlopen!\n");
    
    // Try a simple allocation
    void* ptr = malloc(100);
    if (ptr) {
        printf("✅ malloc(100) succeeded: %p\n", ptr);
        free(ptr);
        printf("✅ free() succeeded\n");
    } else {
        printf("❌ malloc() returned NULL\n");
        return 1;
    }
    
    dlclose(handle);
    printf("✅ All bootstrap tests passed!\n");
    return 0;
}
```

Build and run:
```bash
make test
```

## 🎓 Deep Dive: Why Bootstrap is Necessary

### The Dynamic Linker Problem

When you run:
```bash
LD_PRELOAD=./libapexalloc.so ./myprogram
```

The dynamic linker (`ld.so`) must:
1. Parse the ELF headers of `libapexalloc.so`
2. Allocate memory for relocation tables
3. Allocate memory for symbol tables
4. Allocate memory for TLS (thread-local storage)

But wait... to allocate memory, it calls `malloc()`... which is now **our** `malloc()`!

### Solution Approaches

Different allocators handle this differently:

**jemalloc**: Uses `sbrk()` initially, transitions to `mmap()` later
**mimalloc**: Similar static buffer approach
**tcmalloc**: Relies on system malloc for initialization

ApexAlloc uses a **hybrid approach**: static buffer for bootstrap, full system after init.

### Educational Exercise

**Exercise 1.1**: Modify the bootstrap heap size and observe behavior

1. Change `BOOTSTRAP_HEAP_SIZE` to 1024 (1KB)
2. Rebuild and run tests
3. What happens? Why?

**Exercise 1.2**: Track bootstrap allocations

1. Enable `DEBUG` mode: `make clean && make DEBUG=1`
2. Run: `LD_DEBUG=all LD_PRELOAD=./build/libapexalloc.so /bin/true 2>&1 | grep Bootstrap`
3. How many bootstrap allocations occur?
4. What is their total size?

---

## 📖 What's Next?

In **Phase II**, we'll design the three-level memory hierarchy:
- Superblocks (4MB segments from OS)
- Blocks (64KB pages for size classes)
- Chunks (user objects with headers)

### Preview of Phase II Concepts

- **Segregated Free Lists**: Why same-size objects eliminate fragmentation
- **Size Classes**: Balancing between too many (overhead) and too few (waste)
- **Alignment**: How to guarantee 16-byte alignment for SIMD
- **Metadata Placement**: Where to store headers without breaking locality

---

**Continue to:** [Phase II - Memory Hierarchy](./TUTORIAL_PHASE_II.md)
