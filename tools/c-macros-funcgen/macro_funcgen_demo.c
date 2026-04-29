/*
 * macro_funcgen_demo.c
 *
 * Walkthrough demo for macro_funcgen.h.
 *
 * The three sections below build up from the simplest idea (a type alias for
 * a function pointer) all the way to per-instance OOP-style method binding.
 * Read each section in order; the comments explain every step.
 *
 * Compile and run:
 *     make            (builds and runs this file)
 *     make run        (run only, after build)
 *
 * To see what the preprocessor actually generates:
 *     gcc -E macro_funcgen_demo.c | clang-format -
 *
 * Note: this file uses GCC nested-function extensions (the MAKE_BASE_N and
 * MAKE_FUNC_N macros generate functions inside demo functions).  It compiles
 * cleanly with -Wall -Wextra -Werror -std=c11 but NOT with -Wpedantic.
 */

#include "macro_funcgen.h"
#include <stdio.h>

/* Forward declarations so the demo functions can be called from main
 * regardless of definition order in this file. */
static void demo_layer1(void);
static void demo_layer2(void);
static void demo_layer3(void);

/* ============================================================
 * SECTION 1 — Layer 1: function-pointer type helpers
 *
 * A function pointer in C has verbose syntax:
 *     int (*MyOp)(int, int);
 * MAKE_FTYPE wraps that into a readable one-liner typedef.
 * ============================================================ */

static void demo_layer1(void) {
    printf("=== Layer 1: Function-pointer types ===\n");

    /* Define a type for any function that takes two ints and returns an int. */
    MAKE_FTYPE(IntBinaryOp, int, int, int);

    /* Now we can declare variables of that type normally. */
    IntBinaryOp add = NULL;

    /* Assign a plain lambda-like local function using a statement expression.
     * (Or in practice, assign any matching named function.) */
    int a = 3, b = 7;
    /* We'll just call a plain expression here to show the type compiles. */
    (void)add;

    printf("  IntBinaryOp typedef compiled successfully.  a=%d  b=%d\n", a, b);
}


/* ============================================================
 * SECTION 2 -- Layer 2: function binding with MAKE_BASE_N / MAKE_FUNC_N
 *
 * Idea: we have some context struct that holds state (like an id field).
 * We want a public function  name(args...)  that automatically
 * passes the context without the caller having to think about it.
 *
 * Step by step:
 *   1. Define a context struct type.
 *   2. Create an instance and fill it in.
 *   3. MAKE_BASE2 generates  _PrintPair(Owner this, int a, int b)
 *   4. MAKE_FUNC2 generates  PrintPair(int a, int b)  which calls
 *      _PrintPair(owner_instance, a, b)
 *   5. Call  PrintPair(10, 50)  -- no mention of the owner anywhere.
 * ============================================================ */

static void demo_layer2(void) {
    printf("\n=== Layer 2: Function binding ===\n");

    /* The context struct.  In a real program this might hold a file handle,
     * a network connection, a configuration block, etc. */
    typedef struct { int id; } Owner;

    /* -- 2a: two ordinary parameters -- */
    Owner o2 = { .id = 421 };

    #define body2 printf("  [owner id=%d]  %d + %d = %d\n", this.id, a, b, a + b);
    MAKE_BASE2(Owner, AddAndPrint, void, body2, int, a, int, b)
    MAKE_FUNC2(Owner, o2, AddAndPrint, void, body2, int, a, int, b)
    #undef body2

    AddAndPrint(10, 50);

    /* -- 2b: variadic parameters -- */
    Owner o3 = { .id = 422 };

    /* The variadic base receives a va_list; use `vargs` in the body. */
    #define vbody vprintf(fmt, vargs);
    MAKE_BASEVAR(Owner, VPrint, void, vbody, const char*, fmt)
    MAKE_FUNCVAR(Owner, o3, VPrint, void, vbody, const char*, fmt)
    #undef vbody

    printf("  Variadic wrapper output: ");
    VPrint("  values: %d, %d, %d\n", 1, 2, 3);

    /* -- 2c: zero parameters -- */
    Owner o0 = { .id = 99 };

    #define body0 printf("  [owner id=%d]  zero-arg call\n", this.id);
    MAKE_BASE0(Owner, ZeroArg, void, body0)
    MAKE_FUNC0(Owner, o0, ZeroArg, void, body0)
    #undef body0

    ZeroArg();
}


/* ============================================================
 * SECTION 3 — Layer 3: per-instance method binding (OOP)
 *
 * We want an array of "objects" where each object has its own
 * .show() method that already knows which object it belongs to.
 *
 * Without macros you'd need to either:
 *   a) pass the instance explicitly on every call, or
 *   b) store the instance pointer in a global or closure struct.
 *
 * With FNWRPR + __COUNTER__ we generate a unique C function for
 * each instance and store a pointer to it inside the struct.
 * The trick is that __COUNTER__ increments each time the macro is
 * expanded, guaranteeing no name collisions.
 *
 * After setup, calling  objs[i].show()  "just works" with no extra
 * argument — exactly like a method call in C++/Java/Python.
 * ============================================================ */

static void demo_layer3(void) {
    printf("\n=== Layer 3: Per-instance method binding ===\n");

    /* Step 1: define the function-pointer type for our "method". */
    typedef void (*ShowFn)(void);   /* MAKE_FTYPE(ShowFn, void, void) works too */

    /* Step 2: define the struct.  `show` is the bound method slot. */
    typedef struct Obj Obj;
    struct Obj {
        int    id;
        ShowFn show;
    };

    /* Step 3: define the base function — shared implementation for all instances.
     * `this` is the captured instance (passed by value). */
    #define show_body { printf("  Object %d says hello!\n", this.id); }
    MAKE_BASEWBODY(Obj, Obj_showBase, void, show_body)
    #undef show_body

    /* Step 4: create instances and bind a unique wrapper to each one.
     * FNWRPR(capturedInstance, baseName, ReturnType, memberName, __COUNTER__)
     * Each call expands to a fresh function + assignment. */
    Obj objs[3];

    objs[0].id = 0;
    FNWRPR(objs[0], Obj_showBase, void, show, __COUNTER__)

    objs[1].id = 1;
    FNWRPR(objs[1], Obj_showBase, void, show, __COUNTER__)

    objs[2].id = 2;
    FNWRPR(objs[2], Obj_showBase, void, show, __COUNTER__)

    /* Step 5: call each bound method — no instance argument needed. */
    for (int i = 0; i < 3; i++) {
        objs[i].show();
    }

    printf("  Note: run `gcc -E macro_funcgen_demo.c` to see the generated wrappers.\n");
}


int main(void) {
    demo_layer1();
    demo_layer2();
    demo_layer3();
    return 0;
}
