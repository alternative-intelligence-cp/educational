#include "lesson_string.h"

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static char* alloc_zeroed(size_t size) {
    if (size == 0) {
        return NULL;
    }
    return (char*)calloc(size, sizeof(char));
}

String* String_create(const char* content) {
    if (content == NULL) {
        return NULL;
    }

    const size_t len = strlen(content);
    String* str = (String*)malloc(sizeof(String));
    if (str == NULL) {
        return NULL;
    }

    str->content = alloc_zeroed(len + 1);
    if (str->content == NULL) {
        free(str);
        return NULL;
    }

    memcpy(str->content, content, len);
    str->length = len;
    return str;
}

String* String_createLen(size_t len) {
    String* str = (String*)malloc(sizeof(String));
    if (str == NULL) {
        return NULL;
    }

    str->content = alloc_zeroed(len + 1);
    if (str->content == NULL) {
        free(str);
        return NULL;
    }

    str->length = len;
    return str;
}

String* String_createBar(char chr, size_t count) {
    String* str = String_createLen(count);
    if (str == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < count; ++i) {
        str->content[i] = chr;
    }
    return str;
}

String* String_fromInt(int num) {
    char buffer[64];
    const int written = snprintf(buffer, sizeof(buffer), "%d", num);
    if (written <= 0 || (size_t)written >= sizeof(buffer)) {
        return NULL;
    }
    return String_create(buffer);
}

String* String_clone(const String* str) {
    if (str == NULL || str->content == NULL) {
        return NULL;
    }
    return String_create(str->content);
}

void String_destroy(String* str) {
    if (str == NULL) {
        return;
    }
    free(str->content);
    free(str);
}

size_t String_size(const String* str) {
    if (str == NULL) {
        return 0;
    }
    return str->length + 1;
}

void String_print(const String* str) {
    if (str == NULL || str->content == NULL) {
        return;
    }
    printf("%s", str->content);
}

void String_println(const String* str) {
    if (str == NULL || str->content == NULL) {
        return;
    }
    printf("%s\n", str->content);
}

char String_getChar(const String* str, size_t index) {
    if (str == NULL || str->content == NULL || index >= str->length) {
        return '\0';
    }
    return str->content[index];
}

bool String_setChar(String* str, size_t index, char chr) {
    if (str == NULL || str->content == NULL || index >= str->length) {
        return false;
    }
    str->content[index] = chr;
    return true;
}

bool String_reverse(String* str) {
    if (str == NULL || str->content == NULL) {
        return false;
    }

    for (size_t i = 0; i < str->length / 2; ++i) {
        const size_t j = str->length - i - 1;
        const char temp = str->content[i];
        str->content[i] = str->content[j];
        str->content[j] = temp;
    }
    return true;
}

bool String_toUpper(String* str) {
    if (str == NULL || str->content == NULL) {
        return false;
    }
    for (size_t i = 0; i < str->length; ++i) {
        str->content[i] = (char)toupper((unsigned char)str->content[i]);
    }
    return true;
}

bool String_toLower(String* str) {
    if (str == NULL || str->content == NULL) {
        return false;
    }
    for (size_t i = 0; i < str->length; ++i) {
        str->content[i] = (char)tolower((unsigned char)str->content[i]);
    }
    return true;
}

String* String_concat(const String* str1, const String* str2) {
    if (str1 == NULL || str2 == NULL || str1->content == NULL || str2->content == NULL) {
        return NULL;
    }

    if (str1->length > SIZE_MAX - str2->length - 1) {
        return NULL;
    }

    const size_t total = str1->length + str2->length;
    String* out = String_createLen(total);
    if (out == NULL) {
        return NULL;
    }

    memcpy(out->content, str1->content, str1->length);
    memcpy(out->content + str1->length, str2->content, str2->length);
    out->content[total] = '\0';
    return out;
}

String* String_concatC(const String* str, const char* cstr) {
    if (str == NULL || cstr == NULL) {
        return NULL;
    }
    String* tmp = String_create(cstr);
    if (tmp == NULL) {
        return NULL;
    }
    String* out = String_concat(str, tmp);
    String_destroy(tmp);
    return out;
}

bool String_patch(String* str, size_t start, const String* patch) {
    if (str == NULL || patch == NULL || str->content == NULL || patch->content == NULL) {
        return false;
    }
    if (start > str->length || patch->length > str->length - start) {
        return false;
    }

    memcpy(str->content + start, patch->content, patch->length);
    return true;
}

bool String_patchC(String* str, size_t start, const char* cstr) {
    if (cstr == NULL) {
        return false;
    }
    String* tmp = String_create(cstr);
    if (tmp == NULL) {
        return false;
    }
    const bool ok = String_patch(str, start, tmp);
    String_destroy(tmp);
    return ok;
}

bool String_splice(String* str, size_t start, size_t count) {
    if (str == NULL || str->content == NULL) {
        return false;
    }
    if (start > str->length || count > str->length - start) {
        return false;
    }

    const size_t new_len = str->length - count;
    char* new_content = alloc_zeroed(new_len + 1);
    if (new_content == NULL) {
        return false;
    }

    memcpy(new_content, str->content, start);
    memcpy(new_content + start, str->content + start + count, str->length - (start + count));

    free(str->content);
    str->content = new_content;
    str->length = new_len;
    return true;
}

String* String_slice(const String* str, size_t start, size_t len) {
    if (str == NULL || str->content == NULL) {
        return NULL;
    }
    if (start > str->length || len > str->length - start) {
        return NULL;
    }

    String* out = String_createLen(len);
    if (out == NULL) {
        return NULL;
    }

    memcpy(out->content, str->content + start, len);
    out->content[len] = '\0';
    return out;
}

int String_find(const String* str, const String* target) {
    if (str == NULL || target == NULL || str->content == NULL || target->content == NULL) {
        return -1;
    }
    if (target->length == 0 || target->length > str->length) {
        return -1;
    }

    const size_t max_start = str->length - target->length;
    for (size_t i = 0; i <= max_start; ++i) {
        if (memcmp(str->content + i, target->content, target->length) == 0) {
            if (i > (size_t)INT_MAX) {
                return -1;
            }
            return (int)i;
        }
    }
    return -1;
}

int String_findC(const String* str, const char* target) {
    if (target == NULL) {
        return -1;
    }
    String* tmp = String_create(target);
    if (tmp == NULL) {
        return -1;
    }
    const int idx = String_find(str, tmp);
    String_destroy(tmp);
    return idx;
}

bool String_contains(const String* str, const String* target) {
    return String_find(str, target) >= 0;
}

bool String_containsC(const String* str, const char* target) {
    return String_findC(str, target) >= 0;
}

int String_compare(const String* str1, const String* str2) {
    if (str1 == NULL || str2 == NULL || str1->content == NULL || str2->content == NULL) {
        return 0;
    }

    const size_t min_len = (str1->length < str2->length) ? str1->length : str2->length;
    for (size_t i = 0; i < min_len; ++i) {
        const unsigned char a = (unsigned char)str1->content[i];
        const unsigned char b = (unsigned char)str2->content[i];
        if (a < b) return -1;
        if (a > b) return 1;
    }

    if (str1->length < str2->length) return -1;
    if (str1->length > str2->length) return 1;
    return 0;
}

int String_compareC(const String* str, const char* cstr) {
    if (cstr == NULL) {
        return 0;
    }
    String* tmp = String_create(cstr);
    if (tmp == NULL) {
        return 0;
    }
    const int cmp = String_compare(str, tmp);
    String_destroy(tmp);
    return cmp;
}

bool String_equals(const String* str1, const String* str2) {
    return String_compare(str1, str2) == 0;
}

bool String_equalsC(const String* str, const char* cstr) {
    return String_compareC(str, cstr) == 0;
}

StringBuilder* StringBuilder_create(void) {
    const size_t default_size = 16;

    StringBuilder* sb = (StringBuilder*)malloc(sizeof(StringBuilder));
    if (sb == NULL) {
        return NULL;
    }

    sb->content = alloc_zeroed(default_size);
    if (sb->content == NULL) {
        free(sb);
        return NULL;
    }

    sb->size = default_size;
    sb->current = 0;
    return sb;
}

void StringBuilder_destroy(StringBuilder* sb) {
    if (sb == NULL) {
        return;
    }
    free(sb->content);
    free(sb);
}

static bool StringBuilder_ensure_capacity(StringBuilder* sb, size_t required) {
    if (sb == NULL || sb->content == NULL) {
        return false;
    }
    if (required + 1 <= sb->size) {
        return true;
    }

    size_t next = sb->size;
    while (required + 1 > next) {
        if (next > SIZE_MAX / 2) {
            return false;
        }
        next *= 2;
    }

    char* resized = (char*)realloc(sb->content, next);
    if (resized == NULL) {
        return false;
    }

    memset(resized + sb->size, 0, next - sb->size);
    sb->content = resized;
    sb->size = next;
    return true;
}

bool StringBuilder_add(StringBuilder* sb, const String* str) {
    if (sb == NULL || str == NULL || str->content == NULL) {
        return false;
    }
    if (str->length > SIZE_MAX - sb->current) {
        return false;
    }

    const size_t needed = sb->current + str->length;
    if (!StringBuilder_ensure_capacity(sb, needed)) {
        return false;
    }

    memcpy(sb->content + sb->current, str->content, str->length);
    sb->current = needed;
    sb->content[sb->current] = '\0';
    return true;
}

bool StringBuilder_addC(StringBuilder* sb, const char* cstr) {
    if (cstr == NULL) {
        return false;
    }
    String* tmp = String_create(cstr);
    if (tmp == NULL) {
        return false;
    }
    const bool ok = StringBuilder_add(sb, tmp);
    String_destroy(tmp);
    return ok;
}

bool StringBuilder_clear(StringBuilder* sb) {
    if (sb == NULL || sb->content == NULL) {
        return false;
    }

    memset(sb->content, 0, sb->size);
    sb->current = 0;
    return true;
}

String* StringBuilder_toString(const StringBuilder* sb) {
    if (sb == NULL || sb->content == NULL) {
        return NULL;
    }

    String* out = String_createLen(sb->current);
    if (out == NULL) {
        return NULL;
    }

    memcpy(out->content, sb->content, sb->current);
    out->content[sb->current] = '\0';
    return out;
}
