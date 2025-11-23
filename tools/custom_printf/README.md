# 🖨️ Custom Printf - System Programming Education

Educational printf implementation using **pure syscalls** - no libc dependencies!

## 🎯 What This Teaches

### Core Concepts
- **Number to String Conversion**: How computers turn numbers into printable characters
- **System Call Interface**: Direct communication with the Linux kernel
- **ASCII Encoding**: How characters are represented as numbers
- **Memory Management**: Buffer handling without malloc/free
- **Variable Arguments**: How printf handles different parameter types

### Why This Matters
- Understanding what's "under the hood" of printf
- Learning low-level system programming
- Seeing how high-level abstractions work
- Building systems programming confidence

## 🔧 How It Works

### 1. Raw System Calls
```c
// Direct syscall - no libc!
__asm__ volatile (
    "syscall"
    : "=a" (result)
    : "a" (SYS_WRITE), "D" (fd), "S" (buf), "d" (count)
    : "rcx", "r11", "memory"
);
```

### 2. Number Conversion Algorithm
```c
// Convert 123 to "123"
while (num > 0) {
    buffer[pos++] = '0' + (num % 10);  // Magic happens here!
    num /= 10;
}
```

### 3. Base Conversion
- **Decimal**: `digit = num % 10; ascii = '0' + digit`
- **Hexadecimal**: `digit = num % 16; ascii = hex_chars[digit]`
- **Binary**: `digit = num % 2; ascii = '0' + digit`

## 🚀 Usage

### Compile and Run
```bash
# Compile (no libc linking needed!)
gcc -nostdlib -static custom_printf.c -o custom_printf

# Run the demonstration
./custom_printf
```

### Supported Format Specifiers
- `%d, %i` - Signed integers
- `%u` - Unsigned integers  
- `%x, %X` - Hexadecimal (lower/upper case)
- `%b` - Binary (custom extension!)
- `%c` - Characters
- `%s` - Strings
- `%f` - Floating point (basic implementation)
- `%%` - Literal percent sign

## 📊 Educational Examples

### Number System Conversion
```c
int num = 42;
my_printf("Decimal: %d\n", num);    // 42
my_printf("Hex: %x\n", num);        // 0x2a  
my_printf("Binary: %b\n", num);     // 0b101010
```

### ASCII Conversion Demo
```c
// Shows how '0' + digit creates ASCII characters
for (int i = 0; i <= 9; i++) {
    my_printf("Digit %d -> ASCII %d -> '%c'\n", 
              i, '0' + i, '0' + i);
}
```

## 🧠 Key Learning Points

### 1. **ASCII Magic**
- Character '0' has ASCII value 48
- To convert digit 5 to character: `'0' + 5 = 48 + 5 = 53 = '5'`
- This is how ALL number formatting works!

### 2. **System Calls vs Library Calls**
- `printf()` uses `write()` system call under the hood
- We bypass libc and call kernel directly
- Shows the "layers" of software abstraction

### 3. **Memory Without malloc**
- Uses stack-allocated buffers
- Manual buffer management
- Understanding memory layout

### 4. **Algorithm Implementation**
- Modulo arithmetic for base conversion
- Buffer reversal for correct digit order
- Floating point decomposition

## 🔍 What Happens When You Print 123?

1. **Input**: Integer `123`
2. **Extract Digits**: 
   - `123 % 10 = 3`, store `'0' + 3 = '3'`
   - `12 % 10 = 2`, store `'0' + 2 = '2'`  
   - `1 % 10 = 1`, store `'0' + 1 = '1'`
3. **Buffer**: Contains `"321"` (reversed!)
4. **Output**: Print buffer backwards: `"123"`
5. **Syscall**: `write(1, "123", 3)` to stdout

## 🏗️ Architecture

```
User Code
    ↓
my_printf()
    ↓
Format String Parser
    ↓
Number Conversion Functions
    ↓ 
Raw Syscall (my_write)
    ↓
Linux Kernel
    ↓
Terminal Output
```

## 🎓 Educational Value

This implementation demonstrates:

- ✅ **Low-level programming** - Direct kernel interaction
- ✅ **Number theory** - Base conversion algorithms  
- ✅ **Character encoding** - ASCII representation
- ✅ **Memory management** - Stack vs heap allocation
- ✅ **Systems programming** - Understanding abstractions
- ✅ **Algorithm design** - Efficient conversion methods

## 🚀 Next Steps

Want to go deeper?

1. **Assembly Version**: See how the compiler translates this
2. **Performance Analysis**: Compare with standard printf
3. **Extended Features**: Width specifiers, precision control
4. **Cross-Platform**: Windows syscalls, macOS system calls
5. **Optimization**: SIMD instructions, lookup tables

## 💡 The Big Picture

Understanding printf teaches you:
- How all text output works in computers
- The layers between your code and hardware  
- Why C is considered "close to the metal"
- How abstractions hide complexity
- Foundation for graphics, networking, file I/O

**Every programmer should understand how printf works!**