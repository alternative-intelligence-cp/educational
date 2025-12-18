# ApexAlloc: The Ultimate Educational Memory Allocator

## 🎯 Project Vision

ApexAlloc is a comprehensive, production-grade memory allocator designed as the definitive educational resource for understanding modern allocator design. This project synthesizes the best techniques from:

- **mimalloc**: Throughput optimization through thread-local caching
- **snmalloc**: Message-passing concurrency model for remote frees
- **jemalloc**: Observability and introspection APIs
- **Scudo**: Security hardening and exploit mitigation

## 🏗️ Architecture Overview

ApexAlloc uses a three-level memory hierarchy designed for optimal performance:

### Level 1: Superblock (4MB Segment)
- Large regions mapped from OS via `mmap`
- Aligned to 2MB boundaries for Transparent Huge Pages (THP)
- NUMA-aware allocation

### Level 2: Block (64KB Page)
- Fundamental unit of thread ownership
- Segregated by size class (eliminates external fragmentation)
- Contains both local and remote free lists

### Level 3: Chunk (User Object)
- Actual memory returned to user
- Protected with integrity checksums
- Safe-linking pointer obfuscation

## 🚀 Key Features

### Performance
- ✅ Lock-free fast path (thread-local allocation)
- ✅ Message-passing remote free (no global locks)
- ✅ Transparent Huge Page support
- ✅ Hardware-accelerated operations (CRC32)
- ✅ NUMA-aware memory placement

### Security
- ✅ Cryptographic header checksums
- ✅ Safe-linking pointer masking
- ✅ Double-free detection
- ✅ Heap randomization
- ✅ Optional quarantine for use-after-free protection

### Observability
- ✅ Comprehensive introspection API (`apex_ctl`)
- ✅ Per-thread statistics
- ✅ Heap visualization tools
- ✅ Fragmentation analysis

## 📚 Educational Structure

This repository is designed as a **learning journey** with multiple pathways:

### 1. **Quick Start** (Use the allocator)
- Pre-built library
- Simple API examples
- Performance benchmarks

### 2. **Guided Tutorial** (Build it yourself)
- Step-by-step walkthrough
- Each phase builds on the previous
- Exercises and checkpoints
- Complete explanations of every technique

### 3. **Deep Dive** (Master the concepts)
- Detailed architectural documentation
- Research paper references
- Hardware optimization guides
- Security exploit analysis

### 4. **Experimentation** (Extend and modify)
- Fuzzing infrastructure
- Benchmark suite
- Visualization tools
- Extension challenges

## 🗺️ Implementation Phases

The tutorial is organized into 10 phases matching the implementation roadmap:

| Phase | Topic | Complexity |
|-------|-------|------------|
| **I** | Build System & Bootstrapping | ⭐⭐ |
| **II** | Memory Hierarchy (Data Structures) | ⭐⭐⭐ |
| **III** | Global State & Initialization | ⭐⭐⭐ |
| **IV** | Superblock Management (OS Interface) | ⭐⭐⭐ |
| **V** | Fast Path (Thread-Local Allocation) | ⭐⭐⭐⭐ |
| **VI** | Concurrency (Remote Free) | ⭐⭐⭐⭐⭐ |
| **VII** | Security Hardening | ⭐⭐⭐⭐ |
| **VIII** | Memory Reclamation | ⭐⭐⭐ |
| **IX** | Introspection & Visualization | ⭐⭐⭐ |
| **X** | Testing & Verification | ⭐⭐⭐⭐ |

## 📖 Documentation

- [**Tutorial Guide**](./docs/TUTORIAL.md) - Start here for the step-by-step walkthrough
- [**Architecture Specification**](./docs/ARCHITECTURE.md) - Deep dive into design decisions
- [**Implementation Roadmap**](./docs/ROADMAP.md) - Detailed task breakdown
- [**API Reference**](./docs/API.md) - Public interface documentation
- [**Security Guide**](./docs/SECURITY.md) - Hardening techniques explained
- [**Performance Guide**](./docs/PERFORMANCE.md) - Optimization strategies

## 🛠️ Building

```bash
# Clone the repository
cd allocators/apexalloc

# Build the library
make

# Run tests
make test

# Run benchmarks
make bench

# Build with debug symbols and checks
make debug
```

## 🎓 Learning Pathways

### Beginner Path
1. Read the Architecture Overview
2. Follow Phase I-III tutorials
3. Understand basic allocation/deallocation
4. Run visualization tools

### Intermediate Path
1. Complete Phases IV-VI
2. Implement concurrency mechanisms
3. Study lock-free algorithms
4. Benchmark against other allocators

### Advanced Path
1. Complete all phases
2. Implement security features
3. Optimize for specific workloads
4. Contribute extensions

## 🔬 Research References

This allocator is built on decades of research:

- **mimalloc** (2019): Free list sharding, thread-local pages
- **snmalloc** (2020): Message-passing remote free
- **jemalloc** (2005): Arena-based design, introspection
- **Scudo** (2017): Hardened allocator for Android
- **glibc Safe-Linking** (2020): Pointer obfuscation
- **TCMalloc** (2005): Thread-caching malloc
- **Hoard** (2000): False sharing elimination

## 🎯 Educational Goals

By completing this tutorial, you will:

- ✅ Master multi-threaded programming in C
- ✅ Understand lock-free algorithms and atomics
- ✅ Learn OS-level memory management (mmap, huge pages)
- ✅ Implement hardware-accelerated crypto operations
- ✅ Design for NUMA architectures
- ✅ Build secure, exploit-resistant systems
- ✅ Profile and optimize systems-level code
- ✅ Read and understand research papers

## 🤝 Contributing

This is an educational project. Contributions welcome:

- 📝 Improve documentation and tutorials
- 🐛 Fix bugs and add tests
- 🎨 Create visualizations
- 📊 Add benchmark scenarios
- 🔒 Enhance security features
- 🌍 Translate tutorials

## 📜 License

This educational project is released under the MIT License.

## 🙏 Acknowledgments

This project synthesizes ideas from the allocator research community. See [REFERENCES.md](./docs/REFERENCES.md) for complete citations.

## 🚀 Status

**Current Version**: 0.1.0-alpha (Educational Implementation)

- [x] Project structure
- [ ] Phase I: Build system
- [ ] Phase II: Data structures
- [ ] Phase III: Initialization
- [ ] Phase IV: Superblock management
- [ ] Phase V: Fast path
- [ ] Phase VI: Concurrency
- [ ] Phase VII: Security
- [ ] Phase VIII: Reclamation
- [ ] Phase IX: Introspection
- [ ] Phase X: Testing

---

**Ready to begin?** Start with the [Tutorial Guide](./docs/TUTORIAL.md)!

**Need help?** Check the [FAQ](./docs/FAQ.md) or open an issue.

**Want to dive deep?** Read the [Architecture Specification](./docs/ARCHITECTURE.md).
