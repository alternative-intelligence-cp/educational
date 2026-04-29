#include "lesson_string.h"

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

static void test_create_and_destroy(void) {
    String* s = String_create("hello");
    CHECK(s != NULL, "String_create should allocate");
    if (s == NULL) return;

    CHECK(s->length == 5, "String length should be 5");
    CHECK(strcmp(s->content, "hello") == 0, "String content should match input");
    CHECK(String_size(s) == 6, "String_size should include null terminator");

    String_destroy(s);
}

static void test_char_access(void) {
    String* s = String_create("abc");
    CHECK(s != NULL, "setup for char access");
    if (s == NULL) return;

    CHECK(String_getChar(s, 0) == 'a', "getChar index 0 should be 'a'");
    CHECK(String_getChar(s, 99) == '\0', "getChar out of bounds should be NUL");
    CHECK(String_setChar(s, 1, 'Z') == true, "setChar valid index should succeed");
    CHECK(strcmp(s->content, "aZc") == 0, "setChar should mutate content");
    CHECK(String_setChar(s, 99, 'x') == false, "setChar out of bounds should fail");

    String_destroy(s);
}

static void test_transforms(void) {
    String* lower = String_create("abc123");
    String* mixed = String_create("AbC");
    CHECK(lower != NULL && mixed != NULL, "setup for transforms");
    if (lower == NULL || mixed == NULL) {
        String_destroy(lower);
        String_destroy(mixed);
        return;
    }

    CHECK(String_toUpper(lower) == true, "toUpper should succeed");
    CHECK(strcmp(lower->content, "ABC123") == 0, "toUpper should uppercase letters");

    CHECK(String_toLower(mixed) == true, "toLower should succeed");
    CHECK(strcmp(mixed->content, "abc") == 0, "toLower should lowercase letters");

    CHECK(String_reverse(mixed) == true, "reverse should succeed");
    CHECK(strcmp(mixed->content, "cba") == 0, "reverse should reverse content");

    String_destroy(lower);
    String_destroy(mixed);
}

static void test_concat_slice_patch_splice(void) {
    String* a = String_create("hello");
    String* b = String_create(" world");
    String* cat = String_concat(a, b);
    CHECK(a != NULL && b != NULL && cat != NULL, "setup for concat/slice/patch/splice");
    if (a == NULL || b == NULL || cat == NULL) {
        String_destroy(a);
        String_destroy(b);
        String_destroy(cat);
        return;
    }

    CHECK(strcmp(cat->content, "hello world") == 0, "concat should join strings");

    String* sl = String_slice(cat, 6, 5);
    CHECK(sl != NULL, "slice should allocate substring");
    if (sl != NULL) {
        CHECK(strcmp(sl->content, "world") == 0, "slice should produce expected range");
    }

    CHECK(String_patchC(cat, 6, "there") == true, "patchC valid bounds should succeed");
    CHECK(strcmp(cat->content, "hello there") == 0, "patchC should replace range");

    CHECK(String_splice(cat, 5, 1) == true, "splice should remove one char");
    CHECK(strcmp(cat->content, "hellothere") == 0, "splice should collapse content");

    CHECK(String_patchC(cat, 99, "x") == false, "patchC invalid bounds should fail");
    CHECK(String_splice(cat, 99, 1) == false, "splice invalid bounds should fail");

    String_destroy(a);
    String_destroy(b);
    String_destroy(cat);
    String_destroy(sl);
}

static void test_search_compare(void) {
    String* s = String_create("abcabc");
    String* n = String_create("cab");
    CHECK(s != NULL && n != NULL, "setup for search/compare");
    if (s == NULL || n == NULL) {
        String_destroy(s);
        String_destroy(n);
        return;
    }

    CHECK(String_find(s, n) == 2, "find should return first match index");
    CHECK(String_findC(s, "zzz") == -1, "findC should return -1 when absent");
    CHECK(String_containsC(s, "abc") == true, "containsC should detect present substring");
    CHECK(String_containsC(s, "qqq") == false, "containsC should detect absent substring");

    CHECK(String_compareC(s, "abcabc") == 0, "compareC equal strings should be 0");
    CHECK(String_compareC(s, "abcabd") < 0, "compareC lexicographic less-than");
    CHECK(String_compareC(s, "abcabb") > 0, "compareC lexicographic greater-than");
    CHECK(String_equalsC(s, "abcabc") == true, "equalsC should be true for equal strings");

    String_destroy(s);
    String_destroy(n);
}

static void test_string_builder(void) {
    StringBuilder* sb = StringBuilder_create();
    CHECK(sb != NULL, "StringBuilder_create should allocate");
    if (sb == NULL) return;

    CHECK(StringBuilder_addC(sb, "12345") == true, "StringBuilder_addC first append");
    CHECK(StringBuilder_addC(sb, "67890") == true, "StringBuilder_addC second append");

    String* extra = String_create("abc");
    CHECK(extra != NULL, "setup extra string for StringBuilder_add");
    if (extra != NULL) {
        CHECK(StringBuilder_add(sb, extra) == true, "StringBuilder_add with String* should succeed");
    }

    String* out = StringBuilder_toString(sb);
    CHECK(out != NULL, "StringBuilder_toString should allocate");
    if (out != NULL) {
        CHECK(strcmp(out->content, "1234567890abc") == 0, "StringBuilder output should match expected content");
    }

    CHECK(StringBuilder_clear(sb) == true, "StringBuilder_clear should succeed");
    CHECK(sb->current == 0, "StringBuilder current should reset to zero");

    String_destroy(extra);
    String_destroy(out);
    StringBuilder_destroy(sb);
}

int main(void) {
    printf("Running c-strings lesson tests...\n");

    test_create_and_destroy();
    test_char_access();
    test_transforms();
    test_concat_slice_patch_splice();
    test_search_compare();
    test_string_builder();

    if (g_failed == 0) {
        printf("[PASS] all tests passed\n");
        return 0;
    }

    fprintf(stderr, "[FAIL] %d test assertion(s) failed\n", g_failed);
    return 1;
}
