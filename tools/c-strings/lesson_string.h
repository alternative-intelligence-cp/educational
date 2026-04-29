#ifndef LESSON_STRING_H
#define LESSON_STRING_H

#include <stdbool.h>
#include <stddef.h>

/*
 * lesson_string.h
 *
 * Educational string library:
 * - Safe heap-managed String type
 * - Bound-checked mutation helpers
 * - Search/compare helpers
 * - Tiny StringBuilder
 *
 * Notes for learners:
 * - Every String* returned from create/clone/concat/slice/fromInt/toString
 *   must be destroyed with String_destroy().
 * - Functions that mutate in place return bool to report success/failure.
 */

typedef struct {
    char*  content;
    size_t length; /* does not include the null terminator */
} String;

typedef struct {
    char*  content;
    size_t size;    /* capacity including room for '\0' */
    size_t current; /* current length excluding '\0' */
} StringBuilder;

/* Constructors / destruction */
String* String_create(const char* content);
String* String_createLen(size_t len);
String* String_createBar(char chr, size_t count);
String* String_fromInt(int num);
String* String_clone(const String* str);
void    String_destroy(String* str);

/* Basic access */
size_t  String_size(const String* str); /* length + 1 for '\0' */
void    String_print(const String* str);
void    String_println(const String* str);
char    String_getChar(const String* str, size_t index);
bool    String_setChar(String* str, size_t index, char chr);

/* In-place transforms */
bool String_reverse(String* str);
bool String_toUpper(String* str);
bool String_toLower(String* str);

/* New-string operations */
String* String_concat(const String* str1, const String* str2);
String* String_concatC(const String* str, const char* cstr);
String* String_slice(const String* str, size_t start, size_t len);

/* In-place patch/splice */
bool String_patch(String* str, size_t start, const String* patch);
bool String_patchC(String* str, size_t start, const char* cstr);
bool String_splice(String* str, size_t start, size_t count);

/* Search + comparison */
int  String_find(const String* str, const String* target); /* -1 if not found */
int  String_findC(const String* str, const char* target);
bool String_contains(const String* str, const String* target);
bool String_containsC(const String* str, const char* target);
int  String_compare(const String* str1, const String* str2); /* strcmp-like */
int  String_compareC(const String* str, const char* cstr);
bool String_equals(const String* str1, const String* str2);
bool String_equalsC(const String* str, const char* cstr);

/* StringBuilder */
StringBuilder* StringBuilder_create(void);
void           StringBuilder_destroy(StringBuilder* sb);
bool           StringBuilder_add(StringBuilder* sb, const String* str);
bool           StringBuilder_addC(StringBuilder* sb, const char* cstr);
bool           StringBuilder_clear(StringBuilder* sb);
String*        StringBuilder_toString(const StringBuilder* sb);

#endif
