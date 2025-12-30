#include "../include/string_utils.h"

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// clang-format off
char *string_concat(const char *a, const char *b) {
    if (!a || !b) return NULL;
    char *result = (char *)malloc(strlen(a) + strlen(b) + 1);
    if (!result) return NULL;
    strcpy(result, a);
    strcat(result, b);
    return result;
}

size_t string_length(const char *str) {
    if (!str) return SIZE_MAX;
    return strlen(str);
}

char *string_reverse(const char *str) {
    if (!str) return NULL;
    size_t len = strlen(str);
    char *result = (char *)malloc(len + 1);
    if (!result) return NULL;

    for (size_t i = 0; i < len; i++) {
        result[i] = str[len - 1 - i];
    }
    result[len] = '\0';
    return result;
}

char *string_upper(const char *str) {
    if (!str) return NULL;
    size_t len = strlen(str);
    char *result = (char *)malloc(len + 1);
    if (!result) return NULL;

    for (size_t i = 0; i < len; i++) {
        result[i] = (char)toupper((unsigned char)str[i]);
    }
    result[len] = '\0';
    return result;
}
// clang-format on
