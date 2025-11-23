#!/bin/bash

# 🚀 Custom Printf Performance Demo
# Comparing our implementation with standard printf

echo "🚀 Custom Printf Performance Comparison"
echo "======================================="
echo ""

cd /home/randy/WORK_SPACE/work/tools/custom_printf

# Create a performance test program
cat > performance_test.c << 'EOF'
#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main() {
    clock_t start, end;
    double cpu_time_used;
    
    printf("🏁 Standard Printf Performance Test\n");
    printf("===================================\n\n");
    
    start = clock();
    
    // Intensive printf operations
    for (int i = 0; i < 1000; i++) {
        printf("Test %d: %x %o %d\n", i, i, i, i);
    }
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    
    printf("\n⏱️  Standard printf time: %f seconds\n", cpu_time_used);
    
    return 0;
}
EOF

echo "🔧 Building performance test..."
gcc -o performance_test performance_test.c

echo ""
echo "⚡ PERFORMANCE SHOWDOWN"
echo "======================"
echo ""

echo "🥊 Round 1: Standard Printf"
echo "----------------------------"
time ./performance_test > /tmp/standard_output.txt 2>&1
tail -n 5 /tmp/standard_output.txt

echo ""
echo "🥊 Round 2: Our Custom Printf (Pure Syscalls)"
echo "---------------------------------------------"
time ./build/custom_printf_pure > /tmp/custom_output.txt 2>&1

echo ""
echo "📊 ANALYSIS"
echo "==========="
echo ""

echo "📏 Output size comparison:"
echo "Standard printf output: $(wc -c < /tmp/standard_output.txt) bytes"
echo "Custom printf output:   $(wc -c < /tmp/custom_output.txt) bytes"

echo ""
echo "🔍 Binary analysis:"
echo "Standard printf binary: $(ls -lh performance_test | awk '{print $5}')"
echo "Custom printf binary:   $(ls -lh build/custom_printf_pure | awk '{print $5}')"

echo ""
echo "📈 Memory usage:"
echo "Standard printf dependencies:"
ldd performance_test | wc -l
echo "Custom printf dependencies:"
ldd build/custom_printf_pure 2>&1 | grep -v "not a dynamic" | wc -l

echo ""
echo "🧠 What This Shows:"
echo "=================="
echo ""
echo "✅ Custom Implementation Benefits:"
echo "  • No library dependencies (0 vs multiple .so files)"
echo "  • Smaller binary size (11KB vs larger)"
echo "  • Direct system call efficiency"
echo "  • Educational understanding of internals"
echo ""
echo "✅ Standard Printf Benefits:"
echo "  • Full feature set (width, precision, locale)"
echo "  • Heavily optimized (years of improvements)"
echo "  • Extensive format support"
echo "  • Thread-safe and robust"
echo ""
echo "💡 Key Learning:"
echo "  • Both have their place in programming"
echo "  • Understanding basics helps optimize high-level code"
echo "  • Simple implementations teach transferable concepts"
echo "  • Real optimization comes from algorithmic improvements"

echo ""
echo "🎯 Educational Value:"
echo "  📚 Understanding printf demystifies all text output"
echo "  📚 System programming concepts apply everywhere"
echo "  📚 Knowledge scales from embedded to enterprise"
echo "  📚 Foundation for graphics, networking, file I/O"

# Cleanup
rm -f /tmp/standard_output.txt /tmp/custom_output.txt performance_test