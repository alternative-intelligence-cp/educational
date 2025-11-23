#!/bin/bash

# 🏗️ Build Script for Custom Printf Educational Tools
#
# Compiles all versions and shows the differences

echo "🏗️ Building Custom Printf Educational Tools"
echo "==========================================="
echo ""

# Create build directory
mkdir -p build
cd build

echo "📦 BUILDING C VERSION"
echo "====================="
echo ""

echo "🔧 Building with no standard library (pure syscalls)..."
gcc -nostdlib -static -o custom_printf_pure ../custom_printf.c
if [ $? -eq 0 ]; then
    echo "✅ custom_printf_pure built successfully"
else
    echo "❌ Failed to build custom_printf_pure"
    exit 1
fi

echo ""
echo "🔧 Building with standard library (for comparison)..."
gcc -o custom_printf_std ../custom_printf.c
if [ $? -eq 0 ]; then
    echo "✅ custom_printf_std built successfully"
else
    echo "❌ Failed to build custom_printf_std"
fi

echo ""
echo "📦 BUILDING ASSEMBLY VERSION"
echo "============================"
echo ""

echo "🔧 Assembling x86_64 assembly version..."
nasm -f elf64 -o assembly_printf.o ../assembly_printf.asm
if [ $? -eq 0 ]; then
    echo "✅ Assembly object file created"
    
    ld -o assembly_printf assembly_printf.o
    if [ $? -eq 0 ]; then
        echo "✅ assembly_printf built successfully"
    else
        echo "❌ Failed to link assembly_printf"
    fi
else
    echo "❌ Failed to assemble assembly_printf"
fi

echo ""
echo "📊 ANALYZING BUILD RESULTS"
echo "=========================="
echo ""

echo "📏 Binary sizes (smaller = fewer dependencies):"
ls -lh custom_printf_* assembly_printf 2>/dev/null | awk '{print $5 "\t" $9}' | sort -h
echo ""

echo "🔍 Library dependencies:"
echo ""
echo "Pure syscall version:"
ldd custom_printf_pure 2>&1 | head -n 3
echo ""

if [ -f custom_printf_std ]; then
    echo "Standard library version:"
    ldd custom_printf_std | head -n 3
    echo ""
fi

echo "Assembly version:"
ldd assembly_printf 2>&1 | head -n 3
echo ""

echo "🧪 QUICK FUNCTIONALITY TEST"
echo "==========================="
echo ""

echo "Testing pure syscall version:"
echo "-----------------------------"
./custom_printf_pure | head -n 10
echo ""

if [ -f assembly_printf ]; then
    echo "Testing assembly version:"
    echo "------------------------"
    ./assembly_printf | head -n 10
    echo ""
fi

echo "✅ Build complete! All versions ready for demonstration."
echo ""
echo "💡 To run the full educational demo:"
echo "   cd .. && ./demo_custom_printf.sh"
echo ""
echo "🎯 Educational files created:"
echo "   • custom_printf_pure  - No libc dependencies"
echo "   • custom_printf_std   - With standard library"
echo "   • assembly_printf     - Pure assembly implementation"
echo ""
echo "🏆 These show the progression from high-level to low-level!"

cd ..