#include "../include/string_utils.h"

#include <stdlib.h>

#include "../tec.h"

TEC(test_string_concat_basic) {
    char* result = string_concat("Hello", " World");
    TEC_ASSERT_NOT_NULL(result);
    TEC_ASSERT_STR_EQ(result, "Hello World");
    free(result);
}

TEC(test_string_concat_empty) {
    char* result = string_concat("", "test");
    TEC_ASSERT_STR_EQ(result, "test");
    free(result);

    result = string_concat("test", "");
    TEC_ASSERT_STR_EQ(result, "test");
    free(result);
}

TEC(test_string_length_basic) {
    TEC_ASSERT_EQ(string_length("hello"), 5);
    TEC_ASSERT_EQ(string_length(""), 0);
    TEC_ASSERT_EQ(string_length("C"), 1);
}

TEC(test_string_length_null) { TEC_ASSERT_EQ(string_length(NULL), -1); }

TEC(test_string_reverse_basic) {
    char* result = string_reverse("hello");
    TEC_ASSERT_NOT_NULL(result);
    TEC_ASSERT_STR_EQ(result, "olleh");
    free(result);

    result = string_reverse("abc");
    TEC_ASSERT_STR_EQ(result, "cba");
    free(result);
}

TEC(test_string_upper_basic) {
    char* result = string_upper("hello world");
    TEC_ASSERT_NOT_NULL(result);
    TEC_ASSERT_STR_EQ(result, "HELLO WORLD");
    free(result);

    result = string_upper("MiXeD cAsE");
    TEC_ASSERT_STR_EQ(result, "MIXED CASE");
    free(result);
}

TEC(test_null_inputs) {
    TEC_ASSERT_NULL(string_concat(NULL, "test"));
    TEC_ASSERT_NULL(string_concat("test", NULL));
    TEC_ASSERT_NULL(string_reverse(NULL));
    TEC_ASSERT_NULL(string_upper(NULL));
}
