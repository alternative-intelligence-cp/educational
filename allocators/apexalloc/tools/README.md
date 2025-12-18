# ApexAlloc Development Tools

Educational tools for understanding and debugging memory allocation.

## 📊 Tools Overview

### 1. Heap Visualizer
**File:** `heap_visualizer.c`

Displays real-time memory layout with visual representations.

```bash
# Build
gcc -I ../include tools/heap_visualizer.c -o build/heap_visualizer \
    -L build -lapexalloc -Wl,-rpath,build

# Run demos
LD_PRELOAD=./build/libapexalloc.so ./build/heap_visualizer
./build/heap_visualizer sequential
./build/heap_visualizer fragmentation
./build/heap_visualizer mixed
```

**What it shows:**
- Memory addresses and sizes
- Visual allocation bars
- Active allocation tracking
- Fragmentation patterns

### 2. Fragmentation Analyzer
**File:** `fragmentation_analyzer.c`

Measures internal and external fragmentation.

```bash
# Build
gcc -I ../include tools/fragmentation_analyzer.c -o build/frag_analyzer \
    -L build -lapexalloc -Wl,-rpath,build

# Run analysis
LD_PRELOAD=./build/libapexalloc.so ./build/frag_analyzer

# Test worst-case patterns
LD_PRELOAD=./build/libapexalloc.so ./build/frag_analyzer worst

# Show tips
./build/frag_analyzer tips
```

**What it measures:**
- Internal fragmentation (padding waste)
- Per-size-class efficiency
- Worst-case allocation patterns
- Realistic workload simulation

### 3. Statistics Dumper
**File:** `stats_dumper.c`

User-friendly wrapper for `apex_ctl()` introspection API.

```bash
# Build
gcc -I ../include tools/stats_dumper.c -o build/stats_dumper

# Full report
./build/stats_dumper

# Quick view
./build/stats_dumper quick

# Live monitoring
./build/stats_dumper live
```

**What it reports:**
- Memory usage (mapped, committed, active)
- Allocation/free statistics
- Size class distribution
- Per-thread breakdown
- Security feature status
- Overall health score

### 4. Leak Detector
**File:** `leak_detector.c`

Wraps malloc/free to track and detect memory leaks.

```bash
# Build as shared library
gcc -shared -fPIC -I ../include tools/leak_detector.c -o build/libleak.so -ldl

# Use with any program
LD_PRELOAD=./build/libleak.so ./your_program

# Use with tests
LD_PRELOAD=./build/libleak.so ./build/test_basic
```

**What it detects:**
- Memory leaks (unfreed allocations)
- Stack traces for each leak
- Total/peak memory usage
- Allocation patterns
- Report printed at program exit

## 🎓 Educational Use

### For Students Learning Allocators

1. **Start with Visualization:**
   ```bash
   ./build/heap_visualizer sequential
   ```
   See how memory is organized!

2. **Understand Fragmentation:**
   ```bash
   ./build/frag_analyzer
   ```
   Learn why size classes matter.

3. **Check Your Implementation:**
   ```bash
   ./build/stats_dumper
   ```
   Is your allocator efficient?

4. **Debug Memory Leaks:**
   ```bash
   LD_PRELOAD=./build/libleak.so ./my_test
   ```
   Find those missing `free()` calls!

### For Comparing Allocators

Run tools with different allocators:

```bash
# System malloc
./build/stats_dumper

# ApexAlloc
LD_PRELOAD=./build/libapexalloc.so ./build/stats_dumper

# Other allocators
LD_PRELOAD=/usr/lib/libjemalloc.so ./build/stats_dumper
```

## 🔧 Building All Tools

```bash
cd /path/to/apexalloc

# Make sure library is built
make

# Build all tools
cd tools
gcc -I ../include heap_visualizer.c -o ../build/heap_visualizer -L ../build -lapexalloc -Wl,-rpath,../build
gcc -I ../include fragmentation_analyzer.c -o ../build/frag_analyzer -L ../build -lapexalloc -Wl,-rpath,../build
gcc -I ../include stats_dumper.c -o ../build/stats_dumper
gcc -shared -fPIC leak_detector.c -o ../build/libleak.so -ldl
cd ..
```

## 💡 Tips

- **Heap Visualizer:** Great for understanding allocation patterns visually
- **Frag Analyzer:** Shows the cost of poor size class choices
- **Stats Dumper:** Monitor allocator health during development
- **Leak Detector:** Always run before submitting assignments!

## 🐛 Debugging Workflow

1. Program crashes? Use leak detector:
   ```bash
   LD_PRELOAD=./build/libleak.so ./program
   ```

2. Performance issues? Check stats:
   ```bash
   LD_PRELOAD=./build/libapexalloc.so ./build/stats_dumper
   ```

3. High memory usage? Analyze fragmentation:
   ```bash
   LD_PRELOAD=./build/libapexalloc.so ./build/frag_analyzer
   ```

4. Want to see what's happening? Visualize:
   ```bash
   LD_PRELOAD=./build/libapexalloc.so ./build/heap_visualizer
   ```

## 📚 Further Reading

- **Internal Fragmentation:** Wasted space due to rounding up allocation sizes
- **External Fragmentation:** Free memory scattered in small unusable pieces
- **Size Classes:** Predefined allocation sizes to reduce fragmentation
- **TLAB:** Thread-Local Allocation Buffer for fast lock-free allocation
- **Remote Free:** Freeing memory allocated by another thread (expensive!)

---

**Note:** These are educational tools, not production-quality implementations!
They're designed to help you learn and understand allocator behavior.
