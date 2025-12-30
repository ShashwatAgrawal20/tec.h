#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stddef.h>
char *string_concat(const char *a, const char *b);
size_t string_length(const char *str);
char *string_reverse(const char *str);
char *string_upper(const char *str);

#endif
