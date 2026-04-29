/*
 * tests/test_macro_funcgen.c
 *
 * Assertion-based tests for macro_funcgen.h.
 * No external framework required.
 *
 * Each test section verifies observable behaviour of one macro layer:
 *   - Layer 2 (MAKE_BASEn / MAKE_FUNCn): correct value forwarding and
 *     return values, across arity 0, 1, 2, 3, and variadic.
 *   - Layer 3 (FNWRPR / BINDFN): per-instance wrapper stores and calls
 *     the right implementation for each struct instance.
 *
 * Run via:  make test
 */

#include "../macro_funcgen.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static int g_failed = 0;

#define CHECK(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s\n", msg); \
            ++g_failed; \
        } \
    } while (0)


/* ------------------------------------------------------------------ *
 * Helpers shared across tests                                         *
 * ------------------------------------------------------------------ */

typedef struct { int id; } Ctx;

/* A small scratch buffer used by tests that verify printed output. */
static char g_buf[256];
static void reset_buf(void) { memset(g_buf, 0, sizeof(g_buf)); }


/* ================================================================== *
 * Layer 2 — arity tests                                               *
 * ================================================================== */

/* --- 0-arg: context id captured, no parameters --- */
static void test_arity0(void) {
    Ctx ctx = { .id = 10 };

    #define body0t { snprintf(g_buf, sizeof(g_buf), "id=%d", this.id); }
    MAKE_BASE0(Ctx, T0, void, body0t)
    MAKE_FUNC0(Ctx, ctx, T0, void, body0t)
    #undef body0t

    reset_buf();
    T0();
    CHECK(strcmp(g_buf, "id=10") == 0, "arity-0: captured id should appear in output");
}

/* --- 1-arg: value forwarded correctly --- */
static void test_arity1(void) {
    Ctx ctx = { .id = 20 };

    #define body1t { snprintf(g_buf, sizeof(g_buf), "id=%d a=%d", this.id, a); }
    MAKE_BASE1(Ctx, T1, void, body1t, int, a)
    MAKE_FUNC1(Ctx, ctx, T1, void, body1t, int, a)
    #undef body1t

    reset_buf();
    T1(42);
    CHECK(strcmp(g_buf, "id=20 a=42") == 0, "arity-1: id and first arg should be forwarded");
}

/* --- 2-arg: both args forwarded --- */
static void test_arity2(void) {
    Ctx ctx = { .id = 30 };

    #define body2t { snprintf(g_buf, sizeof(g_buf), "%d+%d=%d", a, b, a + b); }
    MAKE_BASE2(Ctx, T2, void, body2t, int, a, int, b)
    MAKE_FUNC2(Ctx, ctx, T2, void, body2t, int, a, int, b)
    #undef body2t

    reset_buf();
    T2(5, 7);
    CHECK(strcmp(g_buf, "5+7=12") == 0, "arity-2: both args forwarded and computed");
}

/* --- 3-arg: three args forwarded --- */
static void test_arity3(void) {
    Ctx ctx = { .id = 40 };

    #define body3t { snprintf(g_buf, sizeof(g_buf), "%d-%d-%d", x, y, z); }
    MAKE_BASE3(Ctx, T3, void, body3t, int, x, int, y, int, z)
    MAKE_FUNC3(Ctx, ctx, T3, void, body3t, int, x, int, y, int, z)
    #undef body3t

    reset_buf();
    T3(1, 2, 3);
    CHECK(strcmp(g_buf, "1-2-3") == 0, "arity-3: all three args forwarded");
}

/* --- variadic: va_list forwarded through base correctly --- */
static void test_variadic(void) {
    Ctx ctx = { .id = 50 };

    #define vbodyt { vsnprintf(g_buf, sizeof(g_buf), fmt, vargs); }
    MAKE_BASEVAR(Ctx, TVPrint, void, vbodyt, const char*, fmt)
    MAKE_FUNCVAR(Ctx, ctx, TVPrint, void, vbodyt, const char*, fmt)
    #undef vbodyt

    reset_buf();
    TVPrint("%d+%d=%d", 3, 4, 7);
    CHECK(strcmp(g_buf, "3+4=7") == 0, "variadic: formatted output should match expected");
}

/* --- context isolation: two bindings of the same body, different contexts ---
 * Each name must have a matching base+func pair; the "base" name in the pair
 * determines which _name function the wrapper will call. */
static void test_context_isolation(void) {
    Ctx ctx_a = { .id = 100 };
    Ctx ctx_b = { .id = 200 };

    #define bodyci { snprintf(g_buf, sizeof(g_buf), "ctx=%d", this.id); }
    /* Two independent base+func pairs, different contexts, same body. */
    MAKE_BASE0(Ctx, TIsoA, void, bodyci)
    MAKE_FUNC0(Ctx, ctx_a, TIsoA, void, bodyci)
    MAKE_BASE0(Ctx, TIsoB, void, bodyci)
    MAKE_FUNC0(Ctx, ctx_b, TIsoB, void, bodyci)
    #undef bodyci

    reset_buf();
    TIsoA();
    CHECK(strcmp(g_buf, "ctx=100") == 0, "isolation: TIsoA should see id=100");

    reset_buf();
    TIsoB();
    CHECK(strcmp(g_buf, "ctx=200") == 0, "isolation: TIsoB should see id=200");
}


/* ================================================================== *
 * Layer 3 — per-instance method binding                               *
 * ================================================================== */

static void test_instance_binding(void) {
    /* Define the struct with a bound method slot. */
    MAKE_FTYPE(ObjShowFn, void);

    typedef struct ObjT ObjT;
    struct ObjT {
        int       id;
        ObjShowFn show;
    };

    /* Shared implementation -- writes id into g_buf. */
    #define showbody { snprintf(g_buf, sizeof(g_buf), "obj=%d", this.id); }
    MAKE_BASEWBODY(ObjT, ObjT_showBase, void, showbody)
    #undef showbody

    ObjT o0, o1, o2;
    o0.id = 7;
    o1.id = 8;
    o2.id = 9;

    FNWRPR(o0, ObjT_showBase, void, show, __COUNTER__)
    FNWRPR(o1, ObjT_showBase, void, show, __COUNTER__)
    FNWRPR(o2, ObjT_showBase, void, show, __COUNTER__)

    reset_buf();
    o0.show();
    CHECK(strcmp(g_buf, "obj=7") == 0, "instance binding: o0.show() should output id=7");

    reset_buf();
    o1.show();
    CHECK(strcmp(g_buf, "obj=8") == 0, "instance binding: o1.show() should output id=8");

    reset_buf();
    o2.show();
    CHECK(strcmp(g_buf, "obj=9") == 0, "instance binding: o2.show() should output id=9");

    /* Verify that the show pointers are distinct functions (each instance has its own). */
    CHECK(o0.show != o1.show, "instance binding: o0 and o1 should have distinct wrappers");
    CHECK(o1.show != o2.show, "instance binding: o1 and o2 should have distinct wrappers");
    CHECK(o0.show != o2.show, "instance binding: o0 and o2 should have distinct wrappers");
}


/* ================================================================== *
 * Main                                                                *
 * ================================================================== */

int main(void) {
    printf("Running macro_funcgen tests...\n");

    test_arity0();
    test_arity1();
    test_arity2();
    test_arity3();
    test_variadic();
    test_context_isolation();
    test_instance_binding();

    if (g_failed == 0) {
        printf("[PASS] all tests passed\n");
        return 0;
    }

    fprintf(stderr, "[FAIL] %d test assertion(s) failed\n", g_failed);
    return 1;
}
