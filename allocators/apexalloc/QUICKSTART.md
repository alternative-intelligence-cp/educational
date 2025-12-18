# ApexAlloc Quick Reference

## 🚀 Getting Started

```bash
cd allocators/apexalloc
make              # Build release version
make debug        # Build with sanitizers
make test         # Run tests
make clean        # Clean build
```

## 📂 File Structure

```
src/apex_bootstrap.c   - Bootstrap allocator (Phase I)
src/apex_malloc.c      - Main entry points (Phase I)
src/apex_internal.h    - Internal definitions
include/apexalloc.h    - Public API
```

## 🔧 Key Compiler Flags

```makefile
-fno-builtin-malloc    # Don't replace our malloc with built-in
-fvisibility=hidden    # Hide internal symbols
-fPIC                  # Position independent code (for shared lib)
```

## 📖 Tutorial Navigation

1. **Phase I**: Bootstrap & Build System → `docs/TUTORIAL.md`
2. **Phase II**: Memory Hierarchy → (Coming next)
3. **Phase III**: Global Initialization
4. **Phase IV**: OS Interface
5. **Phase V**: Fast Path Allocation
6. **Phase VI**: Concurrency
7. **Phase VII**: Security
8. **Phase VIII**: Reclamation
9. **Phase IX**: Introspection
10. **Phase X**: Testing

## 💡 Key Concepts

### The Bootstrap Problem
Dynamic linker needs memory → calls malloc → malloc needs init → needs memory → 💥

**Solution**: Static 256KB buffer for early allocations

### Memory Hierarchy
```
Superblock (4MB)
  └─ Block (64KB)
       └─ Chunk (8-32KB)
```

### Size Classes
Segregated free lists: all objects in a block are the same size
- Eliminates external fragmentation
- Fast allocation (just pop from list)
- Cache-friendly

### Thread-Local Allocation
- No locks on fast path
- Each thread owns blocks
- Remote frees use message passing

## 🐛 Debugging

```bash
# Build with debug symbols
make clean && make debug

# Run with address sanitizer
LD_PRELOAD=./build/libapexalloc_debug.so ./your_program

# Enable debug logging
# Add -DDEBUG to CFLAGS in Makefile
```

## 📊 Testing

```c
// Run specific test
LD_PRELOAD=./build/libapexalloc.so ./build/test_bootstrap

// Test with real program
LD_PRELOAD=./build/libapexalloc.so ls -la
```

## 🎯 Common Tasks

### Add a new source file
1. Create `src/apex_newfile.c`
2. Add includes to `apex_internal.h`
3. Run `make` (automatically picks up *.c)

### Add a test
1. Create `tests/test_something.c`
2. Include `<apexalloc.h>`
3. Run `make test`

### Check for memory leaks
```bash
valgrind --leak-check=full \
  LD_PRELOAD=./build/libapexalloc.so \
  ./your_program
```

## 📝 Code Style

```c
// Function comments
/**
 * Brief description.
 * 
 * Longer explanation...
 */
void apex_function(void);

// Constants: UPPERCASE
#define BLOCK_SIZE (64 * 1024)

// Types: snake_case with _t suffix
typedef struct apex_block_t apex_block_t;

// Functions: apex_ prefix
void apex_init_once(void);

// Atomics: explicit ordering
atomic_store_explicit(&var, value, memory_order_release);
```

## 🔗 Useful Resources

- **mimalloc paper**: Fast thread-local allocation
- **snmalloc paper**: Message-passing remote free
- **jemalloc docs**: Introspection API design
- **Scudo docs**: Security hardening techniques

## ⚡ Performance Tips

1. **Hot path first**: Optimize malloc before free
2. **Measure, don't guess**: Use benchmarks
3. **Cache locality**: Keep related data together
4. **Atomic ordering**: Use `relaxed` when possible
5. **False sharing**: Separate cache lines for different threads

## 🔒 Security Checklist

- [ ] Header checksums (detect corruption)
- [ ] Safe-linking (obfuscate pointers)
- [ ] Double-free detection
- [ ] Use-after-free quarantine
- [ ] Heap randomization

## 📈 Benchmarking

```bash
# Simple benchmark
time LD_PRELOAD=./build/libapexalloc.so ./your_program

# Compare with system malloc
time ./your_program
```

## 🎓 Learning Path

**Beginner**: Understand bootstrap, build Phase I-III
**Intermediate**: Implement concurrency (Phase VI)
**Advanced**: Add security (Phase VII), optimize performance

---

**Need help?** Check `docs/TUTORIAL.md` or `PROJECT_STATUS.md`
