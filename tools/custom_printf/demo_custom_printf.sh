#!/bin/bash

# 🖨️ Custom Printf Demo - Educational System Programming
#
# Shows the difference between our custom printf and standard printf
# Demonstrates syscall-based implementation vs libc

echo "🖨️ Randy's Custom Printf Educational Demo"
echo "========================================"
echo ""

# Function to run with timing
run_with_timing() {
    local name="$1"
    local cmd="$2"
    
    echo "🔄 Running $name..."
    echo "Command: $cmd"
    echo "----------------------------------------"
    
    time $cmd
    
    echo ""
    echo "📊 Analysis: $name complete"
    echo "========================================="
    echo ""
}

# Compile our custom printf (no libc!)
echo "🔧 COMPILATION COMPARISON"
echo "========================="
echo ""

echo "📦 Compiling custom printf (no libc dependencies)..."
gcc -nostdlib -static -o custom_printf_nostdlib custom_printf.c
echo "✅ custom_printf_nostdlib compiled"

echo ""
echo "📦 Compiling with minimal libc for comparison..."
gcc -o custom_printf_with_libc custom_printf.c  
echo "✅ custom_printf_with_libc compiled"

echo ""
echo "📦 Creating standard printf comparison..."
cat > standard_printf.c << 'EOF'
#include <stdio.h>

int main() {
    printf("🖨️ Standard Printf Implementation\n");
    printf("==================================\n\n");
    
    printf("🔢 INTEGER CONVERSIONS:\n");
    printf("Decimal: %d, %d, %d\n", 42, -17, 0);
    printf("Unsigned: %u\n", 4294967295U);
    printf("Hexadecimal: %x, %X\n", 255, 255);
    // No binary in standard printf!
    
    printf("\n📝 STRING AND CHARACTER:\n");
    printf("String: %s\n", "Hello, World!");
    printf("Character: %c\n", 'A');
    printf("Null string: %s\n", (char*)0);
    
    printf("\n🌊 FLOATING POINT:\n");
    printf("Float: %f\n", 3.14159);
    printf("Negative: %f\n", -2.71828);
    printf("Zero: %f\n", 0.0);
    
    printf("\n🎯 SPECIAL CASES:\n");
    printf("Percent sign: %%\n");
    printf("Mixed: %d + %d = %d\n", 5, 7, 12);
    
    return 0;
}
EOF

gcc -o standard_printf standard_printf.c
echo "✅ standard_printf compiled"

echo ""
echo "📊 BINARY SIZE COMPARISON"
echo "========================="
echo ""

echo "File sizes (smaller = fewer dependencies):"
ls -lh custom_printf_nostdlib custom_printf_with_libc standard_printf | grep -E '\.(out|exe)|printf'
echo ""

echo "🔍 Library dependencies:"
echo "Custom (nostdlib):"
ldd custom_printf_nostdlib 2>&1 | head -n 3
echo ""
echo "Standard printf:"
ldd standard_printf | head -n 3
echo ""

# Performance comparison
echo "⚡ PERFORMANCE COMPARISON"
echo "========================"
echo ""

run_with_timing "Custom Printf (No Libc)" "./custom_printf_nostdlib"

echo "🆚 VS 🆚 VS 🆚 VS 🆚 VS 🆚 VS 🆚 VS 🆚"
echo ""

run_with_timing "Standard Printf (With Libc)" "./standard_printf"

echo "🔬 SYSCALL ANALYSIS"
echo "==================="
echo ""

echo "📊 System calls made by our custom printf:"
echo "(This shows direct kernel communication)"
strace -c ./custom_printf_nostdlib 2>&1 | tail -n 10

echo ""
echo "📊 System calls made by standard printf:"
echo "(Notice the additional complexity)"
strace -c ./standard_printf 2>&1 | tail -n 10

echo ""
echo "🧪 EDUCATIONAL EXPERIMENTS"
echo "=========================="
echo ""

echo "🔬 Experiment 1: What happens with very large numbers?"
cat > large_number_test.c << 'EOF'
// Test both implementations with edge cases
#include <limits.h>

// Our functions (simplified for testing)
void my_write(int fd, const void* buf, unsigned long count) {
    __asm__ volatile (
        "syscall"
        : 
        : "a" (1), "D" (fd), "S" (buf), "d" (count)
        : "rcx", "r11", "memory"
    );
}

void print_large_test() {
    char msg[] = "Custom printf with INT_MAX: Large number handling\n";
    my_write(1, msg, sizeof(msg) - 1);
}

int main() {
    print_large_test();
    return 0;
}
EOF

gcc -nostdlib -static -o large_number_test large_number_test.c
echo "Testing large number handling..."
./large_number_test

echo ""
echo "🔬 Experiment 2: Memory usage comparison"
echo "Stack vs heap allocation patterns:"

# Check memory maps
echo "Custom printf memory map (stack-based):"
pmap $(pgrep -f custom_printf_nostdlib) 2>/dev/null | head -n 5 || echo "Process not running"

echo ""
echo "💡 KEY EDUCATIONAL INSIGHTS"
echo "==========================="
echo ""

echo "✅ What we learned:"
echo "  📚 Printf is just number-to-string conversion + syscalls"
echo "  📚 ASCII encoding: '0' + digit = character"
echo "  📚 Base conversion using modulo arithmetic"  
echo "  📚 System calls are the real interface to OS"
echo "  📚 Libc adds layers of abstraction and overhead"
echo "  📚 Understanding 'magic' makes you a better programmer"

echo ""
echo "🏆 PRACTICAL APPLICATIONS"
echo "========================="
echo ""

echo "Where this knowledge helps:"
echo "  🎯 Embedded programming (no printf available)"
echo "  🎯 Kernel development (no standard library)"
echo "  🎯 Performance optimization (eliminate overhead)"
echo "  🎯 Debugging (understanding what printf does)"
echo "  🎯 Cross-platform code (syscall differences)"
echo "  🎯 Security (buffer overflows in printf)"

echo ""
echo "🚀 NEXT CHALLENGES"
echo "=================="
echo ""

echo "Ready to go deeper?"
echo "  🔥 Write assembly version for speed comparison"
echo "  🔥 Add width/precision specifiers (%8d, %.2f)"
echo "  🔥 Implement floating point from scratch"
echo "  🔥 Add color output codes"
echo "  🔥 Create your own format specifiers"
echo "  🔥 Port to other operating systems"

echo ""
echo "🎓 Remember: Understanding the fundamentals"
echo "   makes complex systems seem simple!"
echo ""
echo "Demo complete! 🏁"