# Low-Level Memory Simulator (Interactive Spreadsheet)

An interactive LibreOffice Calc / Excel tool designed to demystify how computers manage data at the hardware level. This tool bridges the gap between high-level code, assembly logic, and raw memory. 

It provides visual, hands-on demonstrations of three fundamental low-level concepts:
1. **Array Indexing & Pointer Arithmetic** (Why arrays are 0-indexed).
2. **Struct Alignment & Memory Padding** (How compilers layout data structures).
3. **Endianness** (How different architectures store multi-byte data).

---

## 🛠️ Concepts & How to Use the Sheet

### Tab 1: Array Offsets & Pointer Arithmetic
High-level languages hide pointer math, but the CPU calculates every array access using a basic hardware formula:
$$\text{Target Address} = \text{Base Address} + (\text{Index} \times \text{Element Width})$$

*   **The Core Lesson:** An "index" is not a position; it is a **byte offset** from the starting address. Index `0` means zero bytes away from the base pointer.
*   **How to Interact:**
    1. Look at the variable control cells (Base Address, Element Width, and Target Index).
    2. Change the `Target Index` (e.g., from `0` to `2`).
    3. Watch the calculator compute the physical address and watch the corresponding memory row **automatically light up**.
    4. Change the `Element Width` from `4` (32-bit int) to `1` (8-bit char) to see how the CPU's "jump size" alters instantly.

### Tab 2: Struct Alignment & Padding
CPUs read memory in word-sized chunks (typically 4 or 8 bytes) to maximize bus efficiency. Because of this, variables cannot just be packed tightly into memory; they must align to addresses that are multiples of their own size.

*   **The Core Lesson:** Compilers automatically insert invisible **padding bytes** to keep variables aligned. This is why `sizeof(struct)` is often much larger than the sum of its individual data types.
*   **How to Interact:**
    1. Observe the simulated 12-byte structure in memory representing:
       ```c
       struct Student {
           char  id;       // 1 byte (+ 3 padding bytes)
           int   grade;    // 4 bytes (aligned to 4-byte boundary)
           short age;      // 2 bytes (+ 2 padding bytes)
       }; 
       ```
    2. Review the color-coded bytes. The red/gray pattern explicitly flags wasted hardware space.
    3. **Challenge:** Reorder the struct members in your mind putting the largest types first (`int`, then `short`, then `char`). Observe how the padding shrinks, illustrating how efficient code design saves actual hardware memory.

### Tab 3: Endianness (Byte Ordering)
When storing a data type that spans multiple bytes (like a 32-bit integer `0x12345678`), different CPU architectures store those individual bytes in different sequences.

*   **The Core Lesson:** 
    *   **Big Endian:** Stores the *Most Significant Byte* (`12`) at the lowest address. It reads left-to-right naturally for humans.
    *   **Little Endian (x86/ARM):** Stores the *Least Significant Byte* (`78`) at the lowest address. It looks "backwards" in memory but allows optimized hardware math execution.
*   **How to Interact:**
    1. Locate the `EndianMode` dropdown menu.
    2. Toggle between **Big Endian** and **Little Endian**.
    3. Watch the individual byte cells physically swap positions in the memory layout column in real-time.

---

## 🚀 Spreadsheet Implementation Details

For those looking under the hood of this spreadsheet tool, it utilizes native formulas rather than heavy macros to remain safe and cross-platform:
*   **Named Ranges:** Cell coordinates like `B21` are mapped to human-readable labels like `BaseAddress` and `TargetIndex` so formulas read exactly like assembly instructions.
*   **Dynamic Data Fetching:** Uses the `=OFFSET()` function to retrieve values based on calculated byte distances, matching how data registers handle pointers.
*   **Conditional Formatting:** Employs rule-based formulas (`$ByteOffset = (ElementWidth * TargetIndex)`) to visually highlight active memory rows upon user input.
*   **Memory Guardrails:** Implements **Data Validity Constraints** on the index inputs to throw a simulated `Segmentation Fault (Core Dumped)` error if a user attempts an out-of-bounds array access.

## 📄 License
This educational tool is open-source. Feel free to modify, expand, or integrate it into your own computer science curriculum!
