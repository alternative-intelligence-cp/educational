# 🔀 Any Type POC — Type-Erased Containers from First Principles

**Location:** `any-type-poc/`

A JavaScript proof-of-concept demonstrating **ARIA's `any` type** — a type-erased container that stores raw bytes and lets you reinterpret them through different type lenses. Write as `u32`, read as `i8`, see two's complement in action. Same memory, different interpretation.

## 🎓 What You'll Learn

- **Type punning** — How the same bytes produce different values under different type interpretations
- **Memory representation** — Integer sizes, signedness, IEEE 754 floats — all visible through one 8-byte buffer
- **Fat pointers** — How a `{ptr, size}` pair enables type-erased storage (like Rust's `dyn Any` or C's `void*`, but typed)
- **Consuming semantics** — Why `resolve()` makes the container unusable (move semantics for safety)
- **Two's complement** — Store 511, read as `i8`, get -1. See WHY, not just that it happens
- **BigInt for precision** — Why JavaScript Numbers fail above 2^53 and how 64-bit types need BigInt

## 📁 Files

| File | Description |
|------|-------------|
| `any.js` | Core `Any` class — the type-erased container (~250 lines) |
| `any_demo.js` | 8 interactive demonstrations covering all features |
| `README.md` | This file — philosophy, API, and ARIA mapping |

## 🚀 Quick Start

```bash
cd any-type-poc
node any_demo.js
```

No dependencies. Just Node.js (v16+ for BigInt/DataView support).

## 💡 The Core Concept

In ARIA, the `any` type is a fat pointer:

```
┌──────────────────────────────────────┐
│  any  =  { ptr, i64 }               │
│           ────  ───                  │
│           │      └─ size of boxed    │
│           └── pointer to boxed data  │
└──────────────────────────────────────┘
```

When you write `any:box = 42i64`, the compiler:
1. Allocates 8 bytes on the stack
2. Stores 42 into those bytes
3. Builds `{ &bytes, 8 }` — the fat pointer

When you read with `.get::<int32>()`, it:
1. Follows the pointer
2. Loads the bytes as `int32` (4 bytes, signed)

**The raw data never changes. Only the interpretation does.**

This POC uses an `ArrayBuffer` as the backing store — same idea, visible in JavaScript.

## 📖 API Reference

### Constructor

```javascript
const box = new Any(type, value);
// type:  'i8' | 'i16' | 'i32' | 'i64' | 'u8' | 'u16' | 'u32' | 'u64'
//        'f32' | 'f64' | 'obj' | 'str' | 'arr'
// value: initial value (optional)
```

### Methods

| Method | Description |
|--------|-------------|
| `setType(t)` | Change the type lens (bytes unchanged) |
| `getType()` | Current type tag |
| `get([key])` | Read value; optional key for composites |
| `set(val, [key])` | Write value; optional key for composites |
| `resolve()` | Freeze and return final value (box becomes unusable) |
| `isResolved()` | Whether `resolve()` has been called |
| `toString()` | Human-readable representation |
| `rawHex()` | Dump raw 8 bytes as hex (see actual memory) |

### Type Punning Example

```javascript
const box = new Any('u32', 511);
console.log(box.get());                // 511
console.log(box.rawHex());             // "ff 01 00 00 00 00 00 00"

box.setType('u8');
console.log(box.get());                // 255  (only the low byte: 0xFF)

box.setType('i8');
console.log(box.get());                // -1   (two's complement of 0xFF)

box.setType('f32');
console.log(box.get());                // 7.16e-43 (IEEE 754 interpretation)

box.setType('u32');
console.log(box.get());                // 511  (nothing changed in memory!)
```

### Consuming Semantics

```javascript
const box = new Any('i32', 42);
const val = box.resolve();             // 42 — box is now frozen

box.get();                             // ERROR: box is resolved
box.set(99);                           // ERROR: box is resolved
```

In ARIA, this is enforced at **compile time** — the type checker marks the box as moved after `resolve()` and rejects any further use. In this POC we throw at runtime.

## 🔗 How This Maps to ARIA

| ARIA Syntax | JavaScript Equivalent |
|---|---|
| `any:box = 42i64` | `const box = new Any('i64', 42n)` |
| `box.get::<int64>()` | `box.setType('i64'); box.get()` |
| `box.set::<int32>(100i32)` | `box.setType('i32'); box.set(100)` |
| `box.resolve::<int64>()` | `box.resolve()` |

In ARIA, the turbofish `::<T>` specifies the type inline. In this POC we split that into `setType()` + `get()` since JavaScript doesn't have generics syntax.

### Under the Hood (LLVM IR)

```llvm
; any:box = 42i64
%box.boxed = alloca i64
store i64 42, ptr %box.boxed
%0 = insertvalue { ptr, i64 } undef, ptr %box.boxed, 0
%1 = insertvalue { ptr, i64 } %0,    i64 8,           1
store { ptr, i64 } %1, ptr %box

; box.get::<int64>()
%2 = load { ptr, i64 }, ptr %box
%3 = extractvalue { ptr, i64 } %2, 0    ; data pointer
%4 = load i64, ptr %3                    ; read as int64
```

## 🤔 Why This Matters

Type-erased containers appear everywhere:
- **C**: `void*` — no type info, no safety, C programmers just "know"
- **Rust**: `Box<dyn Any>` — runtime type ID, `downcast()` can fail
- **Java/C#**: `Object` — everything boxes, GC handles lifetime
- **ARIA**: `any` — fat pointer with compile-time turbofish dispatch, consuming semantics prevent use-after-resolve

This POC lets you see the mechanics in a language you already know (JavaScript) before studying the compiler implementation.

## 🔬 Experiments to Try

1. **Negative number storage**: Store -1 as `i8`, then read as `u8`. What do you get? Why?
2. **Float bits**: Store `Math.PI` as `f64`, switch to `i64`, see the IEEE 754 bit pattern
3. **Overflow**: Store 256 as `u8` — what happens? (Hint: DataView silently truncates)
4. **NaN boxing**: Store `NaN` as `f64`, read bytes. All NaN encodings are valid!
5. **Endianness**: Change `true` (little-endian) to `false` in the DataView calls. What shifts?

## 📜 Origin

This started as a quick POC (`many.js`) to explore how ARIA's `any` type would feel in practice. The original used constructor functions and `DataView` directly — this cleaned-up version uses ES2022 private fields, proper error handling, and educational documentation while preserving the core insight: **same memory, different interpretation**.

## 📚 Related

- [Quantum Types POC](../quantum-types-poc/) — Another ARIA concept demonstrated in JavaScript
- [Defensive Compiler Design](../defensive-compiler-design/) — Real bugs from ARIA's safety audit
- [ARIA Language](https://github.com/alternative-intelligence-cp/aria) — The compiler itself
