#include "../include/string_utils.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

char* string_concat(const char* a, const char* b) {
    if (!a || !b) return NULL;
    char* result = malloc(strlen(a) + strlen(b) + 1);
    if (!result) return NULL;
    strcpy(result, a);
    strcat(result, b);
    return result;
}

int string_length(const char* str) {
    if (!str) return -1;
    return strlen(str);
}

char* string_reverse(const char* str) {
    if (!str) return NULL;
    int len = strlen(str);
    char* result = malloc(len + 1);
    if (!result) return NULL;

    for (int i = 0; i < len; i++) {
        result[i] = str[len - 1 - i];
    }
    result[len] = '\0';
    return result;
}

char* string_upper(const char* str) {
    if (!str) return NULL;
    int len = strlen(str);
    char* result = malloc(len + 1);
    if (!result) return NULL;

    for (int i = 0; i < len; i++) {
        result[i] = toupper(str[i]);
    }
    result[len] = '\0';
    return result;
}
