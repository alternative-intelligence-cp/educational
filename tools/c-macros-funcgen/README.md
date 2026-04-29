# C Macros: Function Generation & Binding (Educational)

A hands-on lesson exploring how C preprocessor macros can generate functions,
bind context to them, and even emulate object-oriented method dispatch — all
without a single line of C++.

## What this module teaches

- **Function-pointer typedefs** — the clean way to name a function-pointer type
- **Macro-generated function pairs** — using `MAKE_BASE*` / `MAKE_FUNC*` to
  produce an implementation function _and_ a bound public wrapper from one
  macro call
- **Partial application in C** — freezing a context struct into a wrapper so
  callers never need to pass it
- **Per-instance method binding** — giving each struct instance its own
  function-pointer "method" that already knows which instance it belongs to
- **`__COUNTER__`** — the GCC/Clang built-in that increments every time it is
  expanded, used here to guarantee unique generated function names

## Files

| File | Purpose |
|------|---------|
| `macro_funcgen.h` | The macro library — all three layers with detailed comments |
| `macro_funcgen_demo.c` | Step-by-step walkthrough demo; each section builds on the last |
| `Makefile` | Build, run, test, and clean targets |
| `tests/test_macro_funcgen.c` | Assertion-based test suite (no external framework) |

## Build and run

```bash
cd /home/randy/Workspace/REPOS/educational/tools/c-macros-funcgen
make          # build + run the demo
make test     # build + run the test suite
make clean    # remove build artefacts
```

## See what the macros actually expand to

One of the best learning exercises with C macros is inspecting the
preprocessor output directly:

```bash
gcc -E macro_funcgen_demo.c | grep -v "^#" | clang-format -
```

This strips the line directives and re-formats the result so you can read
exactly what C code the compiler sees.

## Macro layers at a glance

### Layer 1 — Type helpers

```c
MAKE_FTYPE(IntBinaryOp, int, int, int);
/* expands to: typedef int (*IntBinaryOp)(int, int); */
```

### Layer 2 — Function binding

```c
typedef struct { int id; } Owner;
Owner owner = { .id = 7 };

#define body printf("[owner %d] %d + %d\n", this.id, a, b);
MAKE_BASE2(Owner, Add, void, body, int, a, int, b)
MAKE_FUNC2(Owner, owner, Add, void, body, int, a, int, b)

Add(10, 20);   /* no Owner argument — it is already bound */
```

`MAKE_BASE2` generates `_Add(Owner this, int a, int b)`.
`MAKE_FUNC2` generates `Add(int a, int b)` which calls `_Add(owner, a, b)`.

### Layer 3 — Per-instance method binding

```c
typedef struct Obj Obj;
struct Obj { int id; void (*show)(void); };

#define body printf("id: %d\n", this.id);
MAKE_BASEWBODY(Obj, Obj_show, void, body)

Obj o;
o.id = 42;
FNWRPR(o, Obj_show, void, show, __COUNTER__)

o.show();   /* prints "id: 42" — no argument needed */
```

Each call to `FNWRPR` with `__COUNTER__` generates a **uniquely named**
wrapper function and assigns it to the struct's function-pointer member.

## Design notes

- This is intentionally a brain-stretcher.  The patterns are valid C11 and
  compile cleanly under `-Wall -Wextra -Werror`.
- `-Wpedantic` is deliberately omitted.  The Layer 2/3 macros generate
  function definitions inside other functions, which GCC supports as an
  extension.  Running `gcc -E` on the demo will show you exactly what that
  extension produces.
- **Executable stack note**: GCC nested functions use trampolines — small
  stub instructions allocated on the stack — to implement closures.  The
  linker will warn `requires executable stack` because of this.  It is
  harmless for this exercise, but it is one reason production code avoids
  nested functions.
- In real production C you would normally pass the context explicitly rather
  than bind it with macros.  The value of this exercise is understanding
  _how_ the language works, not advocating for this style everywhere.
- The macro library caps at 10 ordinary parameters.  Beyond that, pass a
  struct containing the arguments — which is usually the right design anyway.

## Learning exercises

1. Add `MAKE_BASE11` through `MAKE_BASE16` to extend the arity limit.
2. Modify `FNWRPR` to support one extra argument (arity-1 method binding).
3. Write a "destructor" method using `BINDFN` that frees a heap-allocated
   struct and nulls the pointer.
4. Run `gcc -E macro_funcgen_demo.c` and annotate each generated function in
   the output — trace it back to the macro that produced it.
5. Compare this approach to C++ virtual dispatch.  What does the vtable do
   that this pattern cannot?
