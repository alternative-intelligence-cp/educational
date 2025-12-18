# Memory Bug Examples

Educational examples demonstrating common memory management bugs in C.

## 🐛 Bug Categories

### 1. Use-After-Free (`01_use_after_free.c`)
**What it is:** Accessing memory after calling `free()`

**Real-world impact:**
- Crashes (sometimes delayed)
- Data corruption
- Security vulnerability (UAF exploits)

**Example:**
```c
char* data = malloc(100);
free(data);
printf("%s", data);  // ❌ BUG: Use after free!
```

**How to fix:**
- Set pointers to NULL after freeing: `data = NULL;`
- Check for NULL before dereferencing
- Use AddressSanitizer: `-fsanitize=address`

---

### 2. Double-Free (`02_double_free.c`)
**What it is:** Calling `free()` twice on the same pointer

**Real-world impact:**
- Heap corruption
- Crashes in malloc/free (not your code!)
- Critical security vulnerability

**Example:**
```c
char* data = malloc(100);
free(data);
free(data);  // ❌ BUG: Double free!
```

**How to fix:**
- Set to NULL after freeing: `free(data); data = NULL;`
- Check for NULL before freeing: `if (data) free(data);`
- ApexAlloc detects this automatically!

---

### 3. Buffer Overflow (`03_buffer_overflow.c`)
**What it is:** Writing beyond allocated memory

**Real-world impact:**
- #1 cause of security exploits
- Corrupts adjacent memory
- Unpredictable crashes
- Arbitrary code execution

**Example:**
```c
char* buf = malloc(10);
strcpy(buf, "This is way too long!");  // ❌ BUG: Overflow!
```

**How to fix:**
- Use safe functions: `strncpy()`, `snprintf()`
- Check bounds in loops: `i < size` not `i <= size`
- Check for integer overflow
- Use `-fsanitize=address`

---

### 4. Memory Leaks (`04_memory_leak.c`)
**What it is:** Allocating memory and never freeing it

**Real-world impact:**
- Program memory grows over time
- System runs out of memory
- OOM killer terminates program
- Performance degradation

**Example:**
```c
for (int i = 0; i < 1000; i++) {
    char* buf = malloc(1024);
    // ❌ BUG: Never freed! Leaks 1 MB
}
```

**How to fix:**
- Every `malloc()` needs a matching `free()`
- Use goto cleanup pattern for multiple exits
- Free struct members before the struct
- Use leak detectors: valgrind, ApexAlloc leak detector

---

### 5. Uninitialized Memory (`05_uninitialized_memory.c`)
**What it is:** Reading memory before writing to it

**Real-world impact:**
- Non-deterministic bugs
- Security: leaks sensitive data
- Weird values in variables
- Different behavior each run

**Example:**
```c
int* values = malloc(10 * sizeof(int));
printf("%d", values[0]);  // ❌ BUG: Uninitialized!
```

**How to fix:**
- Use `calloc()` instead of `malloc()`
- Or use `memset(ptr, 0, size)` after malloc
- Initialize all struct fields
- Zero sensitive data before freeing

---

## 🔨 Building Examples

```bash
cd examples/bugs

# Build all examples
for file in *.c; do
    gcc -Wall -Wextra -I ../../include "$file" -o "../../build/${file%.c}"
done

# Or build individually
gcc -Wall -Wextra -I ../../include 01_use_after_free.c -o ../../build/bug_uaf
```

## 🧪 Running with Tools

### With ApexAlloc
```bash
LD_PRELOAD=./build/libapexalloc.so ./build/bug_uaf
```

### With Leak Detector
```bash
LD_PRELOAD=./build/libleak.so ./build/bug_leak
```

### With AddressSanitizer (ASan)
```bash
gcc -fsanitize=address -g 01_use_after_free.c -o bug_uaf
./bug_uaf
```

### With Valgrind
```bash
valgrind --leak-check=full ./build/bug_leak
valgrind --track-origins=yes ./build/bug_uninit
```

### With MemorySanitizer (MSan)
```bash
clang -fsanitize=memory -fPIE -pie 05_uninitialized_memory.c -o bug_uninit
./bug_uninit
```

## 🎓 Learning Path

### For Beginners:
1. Start with `01_use_after_free.c` - easiest to understand
2. Move to `04_memory_leak.c` - very common
3. Then `05_uninitialized_memory.c` - subtle but important

### For Intermediate:
4. Study `02_double_free.c` - understand heap corruption
5. Learn `03_buffer_overflow.c` - critical security issue

### For Advanced:
- Combine bugs (use-after-free + overflow = exploit)
- Study real CVEs (Common Vulnerabilities and Exposures)
- Read "The Art of Software Security Assessment"

## 🔍 Debugging Workflow

1. **Program crashes?**
   ```bash
   gcc -g -fsanitize=address program.c
   ./a.out
   ```

2. **Memory leak suspected?**
   ```bash
   LD_PRELOAD=./build/libleak.so ./program
   # or
   valgrind --leak-check=full ./program
   ```

3. **Weird values?**
   ```bash
   clang -fsanitize=memory -fPIE -pie program.c
   ./a.out
   ```

4. **Want to see everything?**
   ```bash
   valgrind --leak-check=full --track-origins=yes ./program
   ```

## 📊 Comparison of Detection Tools

| Bug Type | ASan | MSan | Valgrind | ApexAlloc | Leak Detector |
|----------|------|------|----------|-----------|---------------|
| Use-after-free | ✅ | ✅ | ✅ | ⚠️ | ❌ |
| Double-free | ✅ | ❌ | ✅ | ✅ | ❌ |
| Buffer overflow | ✅ | ❌ | ✅ | ⚠️ | ❌ |
| Memory leak | ✅ | ❌ | ✅ | ❌ | ✅ |
| Uninitialized | ❌ | ✅ | ✅ | ❌ | ❌ |
| Speed | Fast | Slow | Very Slow | Fast | Fast |

**Legend:**
- ✅ Detects reliably
- ⚠️ Partial detection
- ❌ Does not detect

**Recommendation:** Use ASan during development (fast), Valgrind before release (thorough).

## 💡 Best Practices

### General Rules
1. ✅ Every `malloc()` needs a matching `free()`
2. ✅ Set pointers to NULL after freeing
3. ✅ Check pointers before dereferencing
4. ✅ Use safe string functions (`strncpy`, `snprintf`)
5. ✅ Initialize all memory (use `calloc()`)
6. ✅ Free struct members before the struct
7. ✅ Use bounds checking in loops

### Tools to Use
1. 🔧 Development: `-fsanitize=address`
2. 🔧 Testing: `valgrind --leak-check=full`
3. 🔧 Production: ApexAlloc with checksums
4. 🔧 Code review: Static analyzers (clang-tidy, cppcheck)

### Code Patterns
```c
// ✅ Good: Null after free
free(ptr);
ptr = NULL;

// ✅ Good: Goto cleanup pattern
char* a = malloc(100);
char* b = malloc(200);
if (!a || !b) goto cleanup;
// ... work ...
cleanup:
    free(a);
    free(b);

// ✅ Good: Use calloc
int* arr = calloc(10, sizeof(int));  // Zeroed!

// ✅ Good: Bounds checking
for (size_t i = 0; i < n - 1; i++) {  // Leave room for null
    buf[i] = data[i];
}
buf[n-1] = '\0';
```

## 🚨 What NOT to Do

```c
// ❌ Never use these functions
gets()      // Use fgets() instead
strcpy()    // Use strncpy() or strlcpy()
strcat()    // Use strncat() or strlcat()
sprintf()   // Use snprintf() instead
scanf("%s") // Use scanf("%99s") with width

// ❌ Don't trust user input
char* buf = malloc(user_size);  // Validate user_size first!

// ❌ Don't ignore return values
malloc(size);  // Check if NULL!

// ❌ Don't cast malloc
int* p = (int*)malloc(size);  // Unnecessary in C
```

## 📚 Further Reading

**Books:**
- "The C Programming Language" - Kernighan & Ritchie
- "Expert C Programming" - Peter van der Linden
- "The Art of Software Security Assessment"

**Online Resources:**
- OWASP Top 10
- CWE Top 25 Most Dangerous Software Weaknesses
- CERT C Coding Standard

**Papers:**
- "Exploiting the Heap" - Phrack Magazine
- "The Malloc Maleficarum" - Phantasmal Phantasmagoria

---

**Remember:** These bugs are not just academic exercises. They cause real security vulnerabilities, crashes, and data loss in production systems. Learn to recognize and avoid them!

**Practice:** Try introducing bugs into your own code, then use the tools to find them. This is the best way to learn!
