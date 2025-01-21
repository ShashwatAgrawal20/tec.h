#ifndef TEC_H
#define TEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    void (*func)(void);
    const char *name;
} test_case_t;

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define COLOR_RESET "\x1b[0m"
#define MAX_MESSAGE_LENGTH 256

#define type_to_format_specifier(T) \
    _Generic((T),                   \
        int: "%d",                  \
        float: "%f",                \
        double: "%lf",              \
        unsigned long: "%zu",       \
        unsigned int: "%u",         \
        unsigned short: "%hu",      \
        unsigned char: "%hhu",      \
        signed long: "%ld",         \
        signed short: "%hd",        \
        signed char: "%hhd",        \
        default: NULL)
/*******************************************************************************
               TEST INIT AND DECLARATION: WHERE LEGENDS ARE MADE
*******************************************************************************/
#define STRINGIFY(x) #x
#define TEST_FUNCTION(fn) {fn, STRINGIFY(fn)}

#define TEST_SUITE(...)             \
    (test_case_t[]) {               \
        __VA_ARGS__, { NULL, NULL } \
    }

/*******************************************************************************
                   ASSERTIONS: BECAUSE TRUST ISSUES ARE REAL
*******************************************************************************/
#define ASSERT_TRUE(condition)                                             \
    _test_results.total_assertions++;                                      \
    do {                                                                   \
        if (!(condition)) {                                                \
            _add_failed_message("Assertion failed: " #condition, __FILE__, \
                                __LINE__);                                 \
        }                                                                  \
    } while (0)

#define ASSERT_FALSE(condition) ASSERT_TRUE(!(condition))

#define ASSERT_EQUAL(expected, actual)                                  \
    _test_results.total_assertions++;                                   \
    do {                                                                \
        if ((expected) != (actual)) {                                   \
            char message[MAX_MESSAGE_LENGTH];                           \
            snprintf(message, sizeof(message), "Expected: %s, Got: %s", \
                     #expected, #actual);                               \
            _add_failed_message(message, __FILE__, __LINE__);           \
        }                                                               \
    } while (0)

#define ASSERT_NOT_EQUAL(expected, actual)                                  \
    _test_results.total_assertions++;                                       \
    do {                                                                    \
        if (expected == actual) {                                           \
            char message[MAX_MESSAGE_LENGTH];                               \
            snprintf(message, sizeof(message),                              \
                     "Expected '%s' to be different from '%s' ", #expected, \
                     #actual);                                              \
            _add_failed_message(message, __FILE__, __LINE__);               \
        }                                                                   \
    } while (0)

/*
This string comparision shit is crap btw as the buffer is pretty much
prone to overflow... shit ig i need to come up with some clever failure
message for string maybe not avoid those `expected` and `actual` strings in
the first place.
*/
#define ASSERT_STR_EQUAL(expected, actual)                                     \
    _test_results.total_assertions++;                                          \
    do {                                                                       \
        if (strcmp((expected), (actual)) != 0) {                               \
            char message[MAX_MESSAGE_LENGTH];                                  \
            snprintf(message, sizeof(message), "Expected string: %s, Got: %s", \
                     (expected), (actual));                                    \
            _add_failed_message(message, __FILE__, __LINE__);                  \
        }                                                                      \
    } while (0)

#define ASSERT_STR_NOT_EQUAL(expected, actual)                               \
    _test_results.total_assertions++;                                        \
    do {                                                                     \
        if (strcmp((expected), (actual)) == 0) {                             \
            char message[MAX_MESSAGE_LENGTH];                                \
            snprintf(message, sizeof(message),                               \
                     "Strings are identical but should differ: (value)%s, ", \
                     (expected));                                            \
            _add_failed_message(message, __FILE__, __LINE__);                \
        }                                                                    \
    } while (0)

/*
PS: ok so the thing is that I got it somewhat working by using some extra
buffers and stuff, but still it heavily depends on the `_Generic` from C11

NOTE: This ain't check for type differences between `expected` and
`actual`.
*/
#define ASSERT_ARRAY_EQUAL(expected, actual, length)                       \
    _test_results.total_assertions++;                                      \
    do {                                                                   \
        char message[MAX_MESSAGE_LENGTH];                                  \
        const char *format_spec = type_to_format_specifier((expected)[0]); \
        if (!format_spec) {                                                \
            _add_failed_message("Unsupported type for array comparison",   \
                                __FILE__, __LINE__);                       \
            break;                                                         \
        }                                                                  \
        for (size_t i = 0; i < (length); ++i) {                            \
            if ((expected)[i] != (actual)[i]) {                            \
                char expected_str[64], actual_str[64];                     \
                snprintf(expected_str, sizeof(expected_str), format_spec,  \
                         (expected)[i]);                                   \
                snprintf(actual_str, sizeof(actual_str), format_spec,      \
                         (actual)[i]);                                     \
                snprintf(message, sizeof(message),                         \
                         "Mismatch at index %zu: expected %s, got %s", i,  \
                         expected_str, actual_str);                        \
                _add_failed_message(message, __FILE__, __LINE__);          \
                break;                                                     \
            }                                                              \
        }                                                                  \
    } while (0)

#define ASSERT_PTR_VALUE_EQUAL(expected, actual, type)                         \
    _test_results.total_assertions++;                                          \
    do {                                                                       \
        if ((expected) == NULL || (actual) == NULL) {                          \
            _add_failed_message("Cannot compare values: NULL pointer(s)",      \
                                __FILE__, __LINE__);                           \
        } else {                                                               \
            type _expected_val = *(type *)(expected);                          \
            type _actual_val = *(type *)(actual);                              \
            char message[MAX_MESSAGE_LENGTH];                                  \
            const char *format_spec = type_to_format_specifier(_expected_val); \
            if (!format_spec) {                                                \
                _add_failed_message(                                           \
                    "Unsupported type for pointer value comparison", __FILE__, \
                    __LINE__);                                                 \
            }                                                                  \
            if (_expected_val != _actual_val) {                                \
                char expected_str[64], actual_str[64];                         \
                snprintf(expected_str, sizeof(expected_str), format_spec,      \
                         _expected_val);                                       \
                snprintf(actual_str, sizeof(actual_str), format_spec,          \
                         _actual_val);                                         \
                snprintf(message, sizeof(message),                             \
                         "Value mismatch - Expected: %s, Got: %s",             \
                         expected_str, actual_str);                            \
                _add_failed_message(message, __FILE__, __LINE__);              \
            }                                                                  \
        }                                                                      \
    } while (0)

/*******************************************************************************
                         TEST RUNNER: WE DON'T JUDGE 😁
*******************************************************************************/

typedef struct {
    size_t total_tests;
    size_t passed_tests;
    size_t total_assertions;
    size_t failed_assertions;
    char **failed_messages;
    size_t failed_capacity;
} _test_result_t;

static _test_result_t _test_results = {0, 0, 0, 0, NULL, 0};
static int _current_test_failed = 0;

static inline void _print_test_results(void);
static inline void _cleanup_tests(void);
static inline void _add_failed_message(const char *message, const char *file,
                                       int line);

static inline void tec_test_run(test_case_t test_cases[]) {
    _test_results = (_test_result_t){
        .total_tests = 0,
        .passed_tests = 0,
        .total_assertions = 0,
        .failed_assertions = 0,
        .failed_messages = (char **)malloc(sizeof(char *) * 10),
        .failed_capacity = 10};

    if (!_test_results.failed_messages) {
        fprintf(stderr, "Failed to allocate memory for test messages\n");
        exit(EXIT_FAILURE);
    }

    size_t total = 0;
    for (test_case_t *test = test_cases; test->func != NULL; ++test) {
        ++total;
    }

    printf("Running %zu tests\n", total);

    for (test_case_t *test = test_cases; test->func != NULL; ++test) {
        _test_results.total_tests++;
        _current_test_failed = 0;

        printf("test %s ... ", test->name);
        test->func();

        if (_current_test_failed) {
            printf("%sFAILED%s\n", RED, COLOR_RESET);
        } else {
            _test_results.passed_tests++;
            printf("%sok%s\n", GREEN, COLOR_RESET);
        }
    }

    _print_test_results();
    _cleanup_tests();
}

static inline void _add_failed_message(const char *message, const char *file,
                                       int line) {
    if (_test_results.failed_assertions >= _test_results.failed_capacity) {
        size_t new_capacity = _test_results.failed_capacity * 2;
        char **new_messages = (char **)realloc(_test_results.failed_messages,
                                               sizeof(char *) * new_capacity);

        if (!new_messages) {
            fprintf(stderr, "Failed to reallocate memory for test messages\n");
            _cleanup_tests();
            exit(EXIT_FAILURE);
        }

        _test_results.failed_messages = new_messages;
        _test_results.failed_capacity = new_capacity;
    }

    char *full_message = (char *)malloc(MAX_MESSAGE_LENGTH);
    if (!full_message) {
        fprintf(stderr, "Failed to allocate memory for test message\n");
        _cleanup_tests();
        exit(EXIT_FAILURE);
    }

    snprintf(full_message, MAX_MESSAGE_LENGTH, "%s (File: %s, Line: %d)",
             message, file, line);
    _test_results.failed_messages[_test_results.failed_assertions++] =
        full_message;
    _current_test_failed = 1;
}

static inline void _cleanup_tests() {
    if (_test_results.failed_messages) {
        for (size_t i = 0; i < _test_results.failed_assertions; ++i) {
            free(_test_results.failed_messages[i]);
        }
        free(_test_results.failed_messages);
        _test_results.failed_messages = NULL;
    }
}

static inline void _print_test_results() {
    if (_test_results.failed_assertions > 0) {
        printf("\nfailures:\n\n");
        for (size_t i = 0; i < _test_results.failed_assertions; ++i) {
            printf("%s%s%s\n", RED, _test_results.failed_messages[i],
                   COLOR_RESET);
            // printf("%s\n", _test_results.failed_messages[i]);
        }
    }

    printf(
        "\ntest result: %s%s%s. %zu passed; %zu failed; %zu assertions "
        "failed out of %zu\n",
        _test_results.failed_assertions > 0 ? RED : GREEN,
        _test_results.failed_assertions > 0 ? "FAILED" : "ok", COLOR_RESET,
        _test_results.passed_tests,
        _test_results.total_tests - _test_results.passed_tests,
        _test_results.failed_assertions, _test_results.total_assertions);
}

#endif
