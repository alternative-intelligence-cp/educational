#!/bin/bash

# 🚀 Fast Allocator Demo Script - Educational Version
# ==================================================
# 
# This script demonstrates the high-performance educational allocator
# and compares its performance against system malloc.

echo "🚀 HIGH-PERFORMANCE ALLOCATOR DEMONSTRATION 🚀"
echo "================================================"
echo

# Compile the educational allocator
echo "📦 Compiling allocator with optimizations..."
gcc -O3 -march=native -std=c11 fast_allocator_educational.c -o allocator_demo -DCOMPILE_WITH_DEMO

if [ $? -eq 0 ]; then
    echo "✅ Compilation successful!"
    echo
    
    echo "🏃 Running demonstration..."
    echo "=========================="
    ./allocator_demo
    
    echo
    echo "🧹 Cleaning up..."
    rm -f allocator_demo
    
    echo
    echo "🎉 Demo completed successfully!"
    echo "   Check out the README for more details and exercises."
else
    echo "❌ Compilation failed!"
    echo "   Make sure you have GCC installed and the allocator file is present."
fi