/*
 * macro_funcgen.h
 *
 * C Macro Function Generator — Educational Module
 *
 * This header demonstrates three escalating ideas:
 *
 *   Layer 1 — Function-pointer type helpers
 *             MAKE_FTYPE  : typedef a function-pointer type in one line
 *             MAKE_FDECL  : typedef any type alias in one line
 *
 *   Layer 2 — Function-binding macros  (MAKE_BASE* / MAKE_FUNC*)
 *             Each pair generates two real C functions from a single macro call:
 *               _name(this, args...)  — the "base" function; receives an explicit
 *                                       context struct as its first argument
 *               name(args...)         — the public wrapper; the context is already
 *                                       bound at definition time (captured by value)
 *             This is C's version of partial application: you freeze the context
 *             in place and expose a clean, context-free call surface.
 *             Variants exist for 0–10 ordinary parameters plus a variadic version.
 *
 *   Layer 3 — Per-instance method binding  (FNWRPR / BINDFN)
 *             Emulates OOP "methods" without C++ by:
 *               1. Generating a uniquely-named wrapper function per object instance
 *                  (uniqueness guaranteed by __COUNTER__).
 *               2. Storing a plain function pointer to that wrapper inside the struct.
 *             Each "object" therefore carries its own bound method that already knows
 *             which instance it belongs to.
 *
 * Notes for learners
 * ------------------
 * - These macros expand to ordinary C code.  Run your code through
 *       gcc -E macro_funcgen_demo.c
 *   to see exactly what the preprocessor produces — it is very educational.
 * - Macros with body parameters work best when the body is a single statement
 *   or a compound statement wrapped in { }.
 * - The approach is a fun brain-stretcher; in real production C you would
 *   normally pass the context struct explicitly rather than bind it with macros.
 */

#ifndef MACRO_FUNCGEN_H
#define MACRO_FUNCGEN_H

#include <stdarg.h>
#include <stdio.h>

/* ============================================================
 * Layer 1 — Type helpers
 * ============================================================ */

/*
 * MAKE_FTYPE(tname, ret, ...)
 *   Creates a typedef for a function pointer named tname.
 *   ret  — return type
 *   ...  — parameter types
 *
 *   Example:
 *       MAKE_FTYPE(IntBinaryOp, int, int, int);
 *   expands to:
 *       typedef int (*IntBinaryOp)(int, int);
 */
#define MAKE_FTYPE(tname, ret, ...) \
    typedef ret (*tname)(__VA_ARGS__);

/*
 * MAKE_FDECL(tname, tval)
 *   Simple type alias.  Equivalent to:  typedef tval tname;
 */
#define MAKE_FDECL(tname, tval) \
    typedef tval tname;


/* ============================================================
 * Layer 2 — Function-binding macros (arity 0 – 10 + variadic)
 *
 * Each arity provides a pair:
 *   MAKE_BASEn(ContextType, name, ReturnType, body, t1,n1, ...)
 *     → defines:  ReturnType _name(ContextType this, t1 n1, ...)  { body }
 *
 *   MAKE_FUNCn(ContextType, ctxValue, name, ReturnType, body, t1,n1, ...)
 *     → defines:  ReturnType name(t1 n1, ...)  { return _name(ctxValue, n1, ...); }
 *
 * After both macros are used, callers invoke  name(args...)  with no context
 * argument; the bound value (ctxValue) is silently forwarded.
 * ============================================================ */

/* --- 0 parameters --- */
#define MAKE_BASE0(ContextType, name, ReturnType, body) \
    ReturnType _##name(ContextType this) { \
        (void)(this); \
        body \
    }
#define MAKE_FUNC0(ContextType, ctxValue, name, ReturnType, body) \
    ReturnType name(void) { \
        return _##name(ctxValue); \
    }

/* --- 1 parameter --- */
#define MAKE_BASE1(ContextType, name, ReturnType, body, t1, n1) \
    ReturnType _##name(ContextType this, t1 n1) { \
        (void)(this); \
        body \
    }
#define MAKE_FUNC1(ContextType, ctxValue, name, ReturnType, body, t1, n1) \
    ReturnType name(t1 n1) { \
        return _##name(ctxValue, n1); \
    }

/* --- 2 parameters --- */
#define MAKE_BASE2(ContextType, name, ReturnType, body, t1, n1, t2, n2) \
    ReturnType _##name(ContextType this, t1 n1, t2 n2) { \
        (void)(this); \
        body \
    }
#define MAKE_FUNC2(ContextType, ctxValue, name, ReturnType, body, t1, n1, t2, n2) \
    ReturnType name(t1 n1, t2 n2) { \
        return _##name(ctxValue, n1, n2); \
    }

/* --- 3 parameters --- */
#define MAKE_BASE3(ContextType, name, ReturnType, body, t1, n1, t2, n2, t3, n3) \
    ReturnType _##name(ContextType this, t1 n1, t2 n2, t3 n3) { \
        (void)(this); \
        body \
    }
#define MAKE_FUNC3(ContextType, ctxValue, name, ReturnType, body, t1, n1, t2, n2, t3, n3) \
    ReturnType name(t1 n1, t2 n2, t3 n3) { \
        return _##name(ctxValue, n1, n2, n3); \
    }

/* --- 4 parameters --- */
#define MAKE_BASE4(ContextType, name, ReturnType, body, t1, n1, t2, n2, t3, n3, t4, n4) \
    ReturnType _##name(ContextType this, t1 n1, t2 n2, t3 n3, t4 n4) { \
        (void)(this); \
        body \
    }
#define MAKE_FUNC4(ContextType, ctxValue, name, ReturnType, body, t1, n1, t2, n2, t3, n3, t4, n4) \
    ReturnType name(t1 n1, t2 n2, t3 n3, t4 n4) { \
        return _##name(ctxValue, n1, n2, n3, n4); \
    }

/* --- 5 parameters --- */
#define MAKE_BASE5(ContextType, name, ReturnType, body, t1, n1, t2, n2, t3, n3, t4, n4, t5, n5) \
    ReturnType _##name(ContextType this, t1 n1, t2 n2, t3 n3, t4 n4, t5 n5) { \
        (void)(this); \
        body \
    }
#define MAKE_FUNC5(ContextType, ctxValue, name, ReturnType, body, t1, n1, t2, n2, t3, n3, t4, n4, t5, n5) \
    ReturnType name(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5) { \
        return _##name(ctxValue, n1, n2, n3, n4, n5); \
    }

/* --- 6 parameters --- */
#define MAKE_BASE6(ContextType, name, ReturnType, body, t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6) \
    ReturnType _##name(ContextType this, t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6) { \
        (void)(this); \
        body \
    }
#define MAKE_FUNC6(ContextType, ctxValue, name, ReturnType, body, t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6) \
    ReturnType name(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6) { \
        return _##name(ctxValue, n1, n2, n3, n4, n5, n6); \
    }

/* --- 7 parameters --- */
#define MAKE_BASE7(ContextType, name, ReturnType, body, t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7) \
    ReturnType _##name(ContextType this, t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7) { \
        (void)(this); \
        body \
    }
#define MAKE_FUNC7(ContextType, ctxValue, name, ReturnType, body, t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7) \
    ReturnType name(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7) { \
        return _##name(ctxValue, n1, n2, n3, n4, n5, n6, n7); \
    }

/* --- 8 parameters --- */
#define MAKE_BASE8(ContextType, name, ReturnType, body, t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8) \
    ReturnType _##name(ContextType this, t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8) { \
        (void)(this); \
        body \
    }
#define MAKE_FUNC8(ContextType, ctxValue, name, ReturnType, body, t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8) \
    ReturnType name(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8) { \
        return _##name(ctxValue, n1, n2, n3, n4, n5, n6, n7, n8); \
    }

/* --- 9 parameters --- */
#define MAKE_BASE9(ContextType, name, ReturnType, body, \
                   t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9) \
    ReturnType _##name(ContextType this, t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9) { \
        (void)(this); \
        body \
    }
#define MAKE_FUNC9(ContextType, ctxValue, name, ReturnType, body, \
                   t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9) \
    ReturnType name(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9) { \
        return _##name(ctxValue, n1, n2, n3, n4, n5, n6, n7, n8, n9); \
    }

/* --- 10 parameters --- */
#define MAKE_BASE10(ContextType, name, ReturnType, body, \
                    t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, \
                    t6, n6, t7, n7, t8, n8, t9, n9, t10, n10) \
    ReturnType _##name(ContextType this, t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, \
                       t6 n6, t7 n7, t8 n8, t9 n9, t10 n10) { \
        (void)(this); \
        body \
    }
#define MAKE_FUNC10(ContextType, ctxValue, name, ReturnType, body, \
                    t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, \
                    t6, n6, t7, n7, t8, n8, t9, n9, t10, n10) \
    ReturnType name(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10) { \
        return _##name(ctxValue, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10); \
    }

/* --- variadic (at least one fixed parameter required) ---
 *
 * The base receives a va_list already started by the wrapper.
 * The body should use vargs as the va_list identifier.
 * The wrapper handles va_start / va_end automatically.
 *
 * Example body:  vprintf(fmt, vargs);
 */
#define MAKE_BASEVAR(ContextType, name, ReturnType, body, FixedType, fixedName) \
    ReturnType _##name(ContextType this, FixedType fixedName, va_list vargs) { \
        (void)(this); \
        body \
    }
#define MAKE_FUNCVAR(ContextType, ctxValue, name, ReturnType, body, FixedType, fixedName) \
    ReturnType name(FixedType fixedName, ...) { \
        va_list vargs; \
        va_start(vargs, fixedName); \
        _##name(ctxValue, fixedName, vargs); \
        va_end(vargs); \
    }


/* ============================================================
 * Layer 3 — Per-instance method binding (OOP emulation)
 *
 * These macros let you give each struct instance its own bound
 * function pointer that already knows which instance to act on.
 *
 * How it works
 * ------------
 * 1. MAKE_BASEWBODY defines a shared "base" function:
 *        ReturnType BaseName(ContextType this) { body }
 *    This is the real implementation; it receives the instance explicitly.
 *
 * 2. MAKE_FUNCWBODY defines a named wrapper that calls the base with a
 *    specific captured instance:
 *        ReturnType WrapperName(void) { BaseName(capturedThis); }
 *
 * 3. BINDFN assigns a wrapper function to a function-pointer member of a
 *    struct instance:
 *        instance.memberName = WrapperName;
 *
 * 4. FNWRPR combines steps 2 and 3 into one call, and uses __COUNTER__
 *    to generate a unique wrapper name so that multiple instances can each
 *    have their own wrapper without name collisions.
 *
 * Usage example (see macro_funcgen_demo.c for a complete walkthrough):
 *
 *   #define fbody printf("id: %d\n", this.id);
 *   MAKE_BASEWBODY(Obj, ShowId, void, fbody)
 *   // ... create obj0, set obj0.id = 0 ...
 *   FNWRPR(obj0, ShowId, void, show, __COUNTER__)
 *   obj0.show();    // prints "id: 0"
 *
 * Key insight: every time FNWRPR is expanded, __COUNTER__ increments, so
 * each expansion produces a distinct C function name.  No manual numbering
 * is required.
 * ============================================================ */

/*
 * Token-pasting helpers used to build unique names from __COUNTER__.
 * The indirection through con1 → con2 → paste is necessary to force full
 * macro expansion before pasting; without it, __COUNTER__ would be pasted
 * as the literal text "__COUNTER__" rather than its numeric value.
 */
#define _fg_paste(a, b)  a ## b
#define _fg_con2(a, b)   _fg_paste(a, b)
#define _fg_con1(a, b)   _fg_con2(a, b)

/*
 * MAKE_BASEWBODY(ContextType, baseName, ReturnType, body)
 *   Defines the shared implementation function that every instance shares.
 *   ContextType — the struct type (e.g. Obj)
 *   baseName    — name of the base function
 *   ReturnType  — return type (typically void for simple methods)
 *   body        — function body; use `this` to access the instance
 */
#define MAKE_BASEWBODY(ContextType, baseName, ReturnType, body) \
    ReturnType baseName(ContextType this) { \
        body \
    }

/*
 * MAKE_FUNCWBODY(capturedThis, baseName, wrapperName, ReturnType)
 *   Defines a zero-argument wrapper that calls baseName with a specific instance.
 *   capturedThis — the struct variable to capture (by value at definition time)
 *   wrapperName  — unique name for the generated function
 */
#define MAKE_FUNCWBODY(capturedThis, baseName, wrapperName, ReturnType) \
    ReturnType wrapperName(void) { \
        baseName(capturedThis); \
    }

/*
 * BINDFN(instance, wrapperFn, memberName)
 *   Assigns wrapperFn to the function-pointer member named memberName.
 */
#define BINDFN(instance, wrapperFn, memberName) \
    instance.memberName = wrapperFn;

/*
 * FNWRPR(capturedThis, baseName, ReturnType, memberName, counter)
 *   Convenience macro: generates a unique wrapper and binds it in one step.
 *   Pass __COUNTER__ as the last argument to guarantee a unique name.
 *   After this macro, capturedThis.memberName() calls baseName(capturedThis).
 */
#define FNWRPR(capturedThis, baseName, ReturnType, memberName, counter) \
    MAKE_FUNCWBODY(capturedThis, baseName, _fg_con1(baseName, counter), ReturnType) \
    BINDFN(capturedThis, _fg_con1(baseName, counter), memberName)

#endif /* MACRO_FUNCGEN_H */
