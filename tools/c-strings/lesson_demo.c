#include "lesson_string.h"

#include <stdio.h>
#include <stdlib.h>

static int check_ptr(const void* ptr, const char* label) {
    if (ptr != NULL) {
        return 1;
    }
    fprintf(stderr, "[error] allocation failed for %s\n", label);
    return 0;
}

int main(int argc, char** argv) {
    const char* input = (argc > 1) ? argv[1] : "12345";

    String* myStr = String_create(input);
    if (!check_ptr(myStr, "myStr")) return 1;

    printf("content: %s, length: %zu, size: %zu\n",
           myStr->content, myStr->length, String_size(myStr));
    printf("getChar(0): %c\n", String_getChar(myStr, 0));

    String* rev = String_clone(myStr);
    String* upper = String_clone(myStr);
    String* lower = String_clone(myStr);
    if (!check_ptr(rev, "rev") || !check_ptr(upper, "upper") || !check_ptr(lower, "lower")) {
        String_destroy(myStr);
        String_destroy(rev);
        String_destroy(upper);
        String_destroy(lower);
        return 1;
    }

    String_reverse(rev);
    String_toUpper(upper);
    String_toLower(lower);

    printf("reversed: %s\n", rev->content);
    printf("toUpper : %s\n", upper->content);
    printf("toLower : %s\n", lower->content);

    String_destroy(myStr);
    String_destroy(rev);
    String_destroy(upper);
    String_destroy(lower);

    String* lenStr = String_createLen(10);
    if (!check_ptr(lenStr, "lenStr")) return 1;
    String_setChar(lenStr, 0, 'a');
    String_setChar(lenStr, 1, 'b');
    String_setChar(lenStr, 2, 'c');
    String_println(lenStr);
    String_destroy(lenStr);

    String* bar = String_createBar('#', 40);
    if (!check_ptr(bar, "bar")) return 1;
    String_println(bar);
    String_destroy(bar);

    String* intStr = String_fromInt(-1787654);
    if (!check_ptr(intStr, "intStr")) return 1;
    String_println(intStr);
    String_destroy(intStr);

    String* c1 = String_create("Yo yo yo! ");
    String* c2 = String_concatC(c1, "Pacman is here! ");
    String* c3 = String_create("What it is?");
    String* c4 = String_concat(c2, c3);
    if (!check_ptr(c1, "c1") || !check_ptr(c2, "c2") || !check_ptr(c3, "c3") || !check_ptr(c4, "c4")) {
        String_destroy(c1);
        String_destroy(c2);
        String_destroy(c3);
        String_destroy(c4);
        return 1;
    }
    String_println(c4);
    String_destroy(c1);
    String_destroy(c2);
    String_destroy(c3);
    String_destroy(c4);

    String* p1 = String_create("This is a door!");
    if (!check_ptr(p1, "p1")) return 1;

    if (String_patchC(p1, 10, "barn")) {
        String_println(p1);
    } else {
        printf("Patch failed (out of bounds).\n");
    }

    if (String_splice(p1, 4, 3)) {
        String_println(p1);
    } else {
        printf("Splice failed (out of bounds).\n");
    }

    String* part = String_slice(p1, 0, 4);
    if (part != NULL) {
        printf("slice: %s\n", part->content);
        String_destroy(part);
    }
    String_destroy(p1);

    String* haystack = String_create("Bob is a nice man");
    if (!check_ptr(haystack, "haystack")) return 1;

    const int idx1 = String_findC(haystack, " nice");
    const int idx2 = String_findC(haystack, "man");
    printf("find \" nice\" => %d\n", idx1);
    printf("find \"man\"   => %d\n", idx2);
    printf("contains \"Bob\"? %s\n", String_containsC(haystack, "Bob") ? "yes" : "no");
    printf("contains \"zzz\"? %s\n", String_containsC(haystack, "zzz") ? "yes" : "no");
    String_destroy(haystack);

    String* comp1 = String_create("ABC");
    String* comp2 = String_create("aBC");
    if (!check_ptr(comp1, "comp1") || !check_ptr(comp2, "comp2")) {
        String_destroy(comp1);
        String_destroy(comp2);
        return 1;
    }
    printf("compare(ABC, aBC): %d\n", String_compare(comp1, comp2));
    printf("equalsC(ABC, ABC): %s\n", String_equalsC(comp1, "ABC") ? "true" : "false");
    String_destroy(comp1);
    String_destroy(comp2);

    StringBuilder* sb = StringBuilder_create();
    if (!check_ptr(sb, "StringBuilder")) return 1;

    if (!StringBuilder_addC(sb, "12345") ||
        !StringBuilder_addC(sb, "67890") ||
        !StringBuilder_addC(sb, "abcde") ||
        !StringBuilder_addC(sb, "fghij")) {
        fprintf(stderr, "[error] StringBuilder append failed\n");
        StringBuilder_destroy(sb);
        return 1;
    }

    String* sbStr = StringBuilder_toString(sb);
    if (!check_ptr(sbStr, "sbStr")) {
        StringBuilder_destroy(sb);
        return 1;
    }

    String_println(sbStr);
    printf("SB Size: %zu, SB Current: %zu, string length: %zu\n",
           sb->size, sb->current, sbStr->length);

    StringBuilder_clear(sb);
    printf("SB after clear -> size: %zu, current: %zu, content: '%s'\n",
           sb->size, sb->current, sb->content);

    String_destroy(sbStr);
    StringBuilder_destroy(sb);

    return 0;
}
