#!/bin/bash
# ApexAlloc Benchmark Comparison Script
# Compares ApexAlloc against system malloc

set -e

echo "═══════════════════════════════════════════════"
echo "  ApexAlloc vs System Malloc Comparison"
echo "═══════════════════════════════════════════════"
echo ""

# Build directory
BUILD_DIR="build"
BENCH_DIR="benchmarks"

# Check if library exists
if [ ! -f "$BUILD_DIR/libapexalloc.so" ]; then
    echo "❌ Error: ApexAlloc library not found"
    echo "   Run 'make' first to build the library"
    exit 1
fi

# Build benchmarks if needed
echo "🔨 Building benchmarks..."
for bench in bench_simple bench_threads bench_producer; do
    if [ ! -f "$BUILD_DIR/$bench" ]; then
        gcc -std=c11 -O2 -pthread -I include \
            "$BENCH_DIR/${bench}.c" -o "$BUILD_DIR/$bench"
    fi
done
echo ""

# Function to run benchmark with both allocators
run_comparison() {
    local bench=$1
    local name=$2
    
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "  $name"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
    
    echo "▶ System Malloc:"
    echo "─────────────────────────────────────────────"
    ./"$BUILD_DIR/$bench" quick 2>&1 | grep -E "(Total|Throughput|Alloc:|Free:|Per-)"
    echo ""
    
    echo "▶ ApexAlloc:"
    echo "─────────────────────────────────────────────"
    LD_PRELOAD=./"$BUILD_DIR/libapexalloc.so" ./"$BUILD_DIR/$bench" quick 2>&1 | grep -E "(Total|Throughput|Alloc:|Free:|Per-)"
    echo ""
}

# Run comparisons
run_comparison "bench_simple" "Simple Benchmark (Sequential, Interleaved, Random)"
run_comparison "bench_threads" "Multi-threaded Benchmark (Concurrency)"
run_comparison "bench_producer" "Producer-Consumer Benchmark (Remote Frees)"

echo "═══════════════════════════════════════════════"
echo "  Comparison Complete!"
echo "═══════════════════════════════════════════════"
echo ""
echo "💡 Tips:"
echo "  • ApexAlloc should excel at thread-local allocations"
echo "  • Watch for differences in concurrent scenarios"
echo "  • Producer-consumer shows remote free performance"
echo ""
echo "📊 For detailed results, run benchmarks individually:"
echo "  ./build/bench_simple"
echo "  LD_PRELOAD=./build/libapexalloc.so ./build/bench_simple"
