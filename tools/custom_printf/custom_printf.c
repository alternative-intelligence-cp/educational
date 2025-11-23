/*
 * 🖨️ Randy's Custom Printf - Pure Syscall Implementation
 * 
 * Educational printf that bypasses libc completely!
 * Shows how number-to-string conversion actually works
 * Demonstrates direct system call interface
 */

// Direct system call definitions
#define SYS_WRITE 1
#define SYS_EXIT 60
#define STDOUT_FILENO 1

// Custom write using raw syscall
long my_write(int fd, const void* buf, unsigned long count) {
    long result;
    __asm__ volatile (
        "syscall"
        : "=a" (result)
        : "a" (SYS_WRITE), "D" (fd), "S" (buf), "d" (count)
        : "rcx", "r11", "memory"
    );
    return result;
}

// Exit syscall
void my_exit(int status) {
    __asm__ volatile (
        "syscall"
        :
        : "a" (SYS_EXIT), "D" (status)
        : "rcx", "r11", "memory"
    );
    __builtin_unreachable();
}

// Calculate string length
int my_strlen(const char* str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

// Print a string using our custom write
void print_str(const char* str) {
    if (str) {
        my_write(STDOUT_FILENO, str, my_strlen(str));
    }
}

// Print a single character
void print_char(char c) {
    my_write(STDOUT_FILENO, &c, 1);
}

// 🔢 INTEGER TO STRING CONVERSION - The Heart of Printf!
void print_int(long num) {
    // Handle negative numbers
    if (num < 0) {
        print_char('-');
        num = -num;
    }
    
    // Handle zero case
    if (num == 0) {
        print_char('0');
        return;
    }
    
    // Convert digits to string (in reverse)
    char buffer[32];  // Enough for 64-bit numbers
    int pos = 0;
    
    while (num > 0) {
        buffer[pos++] = '0' + (num % 10);  // Convert digit to ASCII
        num /= 10;
    }
    
    // Print digits in correct order (reverse the buffer)
    for (int i = pos - 1; i >= 0; i--) {
        print_char(buffer[i]);
    }
}

// Print unsigned integer
void print_uint(unsigned long num) {
    if (num == 0) {
        print_char('0');
        return;
    }
    
    char buffer[32];
    int pos = 0;
    
    while (num > 0) {
        buffer[pos++] = '0' + (num % 10);
        num /= 10;
    }
    
    for (int i = pos - 1; i >= 0; i--) {
        print_char(buffer[i]);
    }
}

// 🔢 HEXADECIMAL CONVERSION
void print_hex(unsigned long num, int uppercase) {
    if (num == 0) {
        print_str("0x0");
        return;
    }
    
    print_str("0x");
    
    char hex_chars_lower[] = "0123456789abcdef";
    char hex_chars_upper[] = "0123456789ABCDEF";
    char* hex_chars = uppercase ? hex_chars_upper : hex_chars_lower;
    
    char buffer[32];
    int pos = 0;
    
    while (num > 0) {
        buffer[pos++] = hex_chars[num % 16];
        num /= 16;
    }
    
    for (int i = pos - 1; i >= 0; i--) {
        print_char(buffer[i]);
    }
}

// 🔢 BINARY CONVERSION  
void print_binary(unsigned long num) {
    if (num == 0) {
        print_str("0b0");
        return;
    }
    
    print_str("0b");
    
    char buffer[64];  // 64 bits max
    int pos = 0;
    
    while (num > 0) {
        buffer[pos++] = '0' + (num % 2);
        num /= 2;
    }
    
    for (int i = pos - 1; i >= 0; i--) {
        print_char(buffer[i]);
    }
}

// 🎯 SIMPLE FLOATING POINT CONVERSION (Basic version)
void print_float(double num, int precision) {
    // Handle negative
    if (num < 0) {
        print_char('-');
        num = -num;
    }
    
    // Print integer part
    long int_part = (long)num;
    print_int(int_part);
    print_char('.');
    
    // Print fractional part
    double frac_part = num - int_part;
    
    for (int i = 0; i < precision; i++) {
        frac_part *= 10;
        int digit = (int)frac_part;
        print_char('0' + digit);
        frac_part -= digit;
    }
}

// Simplified variable arguments for our demo
void my_printf_simple() {
    print_str("🖨️ Randy's Custom Printf - Pure Syscall Implementation\n");
    print_str("====================================================\n\n");
    
    print_str("🔢 INTEGER CONVERSIONS:\n");
    print_str("Decimal: ");
    print_int(42);
    print_str(", ");
    print_int(-17);
    print_str(", ");
    print_int(0);
    print_char('\n');
    
    print_str("Unsigned: ");
    print_uint(4294967295U);
    print_char('\n');
    
    print_str("Hexadecimal: ");
    print_hex(255, 0);
    print_str(", ");
    print_hex(255, 1);
    print_char('\n');
    
    print_str("Binary: ");
    print_binary(42);
    print_char('\n');
    
    print_str("\n📝 STRING AND CHARACTER:\n");
    print_str("String: Hello, World!\n");
    print_str("Character: ");
    print_char('A');
    print_char('\n');
    
    print_str("\n🌊 FLOATING POINT:\n");
    print_str("Float: ");
    print_float(3.14159, 6);
    print_char('\n');
    
    print_str("Negative: ");
    print_float(-2.71828, 6);
    print_char('\n');
    
    print_str("Zero: ");
    print_float(0.0, 6);
    print_char('\n');
    
    print_str("\n🎯 SPECIAL CASES:\n");
    print_str("Percent sign: %\n");
    print_str("Mixed: ");
    print_int(5);
    print_str(" + ");
    print_int(7);
    print_str(" = ");
    print_int(12);
    print_char('\n');
    
    print_str("\n📊 NUMBER SYSTEM COMPARISON:\n");
    int test_num = 42;
    print_str("Number ");
    print_int(test_num);
    print_str(" in different bases:\n");
    
    print_str("  Decimal: ");
    print_int(test_num);
    print_char('\n');
    
    print_str("  Hex: ");
    print_hex(test_num, 0);
    print_char('\n');
    
    print_str("  Binary: ");
    print_binary(test_num);
    print_char('\n');
    
    print_str("\n🔍 ASCII CONVERSION DEMO:\n");
    print_str("How digits become ASCII:\n");
    
    for (int i = 0; i <= 9; i++) {
        print_str("  Digit ");
        print_int(i);
        print_str(" -> ASCII ");
        print_int('0' + i);
        print_str(" -> Character '");
        print_char('0' + i);
        print_str("'\n");
    }
    
    print_str("\n💡 EDUCATIONAL INSIGHTS:\n");
    print_str("✅ No libc dependencies - pure syscalls!\n");
    print_str("✅ Manual ASCII conversion: digit + '0'\n");
    print_str("✅ Number base conversion algorithms\n");
    print_str("✅ Memory management without malloc\n");
    print_str("✅ Direct system call interface\n");
    
    print_str("\n🏆 This printf implementation teaches:\n");
    print_str("  • How number-to-string conversion works\n");
    print_str("  • Direct system call interface\n");
    print_str("  • ASCII character encoding\n");
    print_str("  • Memory buffer management\n");
    print_str("  • What's behind printf magic!\n");
}

// Entry point for nostdlib version
void _start() {
    my_printf_simple();
    my_exit(0);
}

// Main for standard compilation
int main() {
    my_printf_simple();
    return 0;
}