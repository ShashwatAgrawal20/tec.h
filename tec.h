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
#define ASSERT_TRUE(condition)                                            \
    do {                                                                  \
        if (!(condition)) {                                               \
            add_failed_message("Assertion failed: " #condition, __FILE__, \
                               __LINE__);                                 \
        }                                                                 \
    } while (0)

#define ASSERT_FALSE(condition) ASSERT_TRUE(!(condition))

#define ASSERT_EQUAL(expected, actual)                                  \
    do {                                                                \
        if ((expected) != (actual)) {                                   \
            char message[MAX_MESSAGE_LENGTH];                           \
            snprintf(message, sizeof(message), "Expected: %s, Got: %s", \
                     #expected, #actual);                               \
            add_failed_message(message, __FILE__, __LINE__);            \
        }                                                               \
    } while (0)

#define ASSERT_NOT_EQUAL(expected, actual)                                  \
    do {                                                                    \
        if (expected == actual) {                                           \
            char message[MAX_MESSAGE_LENGTH];                               \
            snprintf(message, sizeof(message),                              \
                     "Expected '%s' to be different from '%s' ", #expected, \
                     #actual);                                              \
            add_failed_message(message, __FILE__, __LINE__);                \
        }                                                                   \
    } while (0)

/*
This string comparision shit is crap btw as the buffer is pretty much
prone to overflow... shit ig i need to come up with some clever failure message
for string maybe not avoid those `expected` and `actual` strings in the first
place.
*/
#define ASSERT_STR_EQUAL(expected, actual)                                     \
    do {                                                                       \
        if (strcmp((expected), (actual)) != 0) {                               \
            char message[MAX_MESSAGE_LENGTH];                                  \
            snprintf(message, sizeof(message), "Expected string: %s, Got: %s", \
                     (expected), (actual));                                    \
            add_failed_message(message, __FILE__, __LINE__);                   \
        }                                                                      \
    } while (0)

#define ASSERT_STR_NOT_EQUAL(expected, actual)                               \
    do {                                                                     \
        if (strcmp((expected), (actual)) == 0) {                             \
            char message[MAX_MESSAGE_LENGTH];                                \
            snprintf(message, sizeof(message),                               \
                     "Strings are identical but should differ: (value)%s, ", \
                     (expected));                                            \
            add_failed_message(message, __FILE__, __LINE__);                 \
        }                                                                    \
    } while (0)

/*
PS: ok so the thing is that I got it somewhat working by using some extra
buffers and stuff, but still it heavily depends on the `_Generic` from C11

NOTE: This ain't check for type differences between `expected` and
`actual`.
*/
#define ASSERT_ARRAY_EQUAL(expected, actual, length)                       \
    do {                                                                   \
        char message[MAX_MESSAGE_LENGTH];                                  \
        const char *format_spec = type_to_format_specifier((expected)[0]); \
        if (!format_spec) {                                                \
            snprintf(message, sizeof(message),                             \
                     "Unsupported type for array comparison");             \
            add_failed_message(message, __FILE__, __LINE__);               \
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
                add_failed_message(message, __FILE__, __LINE__);           \
                break;                                                     \
            }                                                              \
        }                                                                  \
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
} test_result_t;

static test_result_t test_results = {0, 0, 0, 0, NULL, 0};
static int current_test_failed = 0;

static inline void print_test_results(void);
static inline void cleanup_tests(void);
static inline void add_failed_message(const char *message, const char *file,
                                      int line);

static inline void tec_test_run(test_case_t test_cases[]) {
    test_results =
        (test_result_t){.total_tests = 0,
                        .passed_tests = 0,
                        .total_assertions = 0,
                        .failed_assertions = 0,
                        .failed_messages = (char **)malloc(sizeof(char *) * 10),
                        .failed_capacity = 10};

    if (!test_results.failed_messages) {
        fprintf(stderr, "Failed to allocate memory for test messages\n");
        exit(EXIT_FAILURE);
    }

    size_t total = 0;
    for (test_case_t *test = test_cases; test->func != NULL; ++test) {
        ++total;
    }

    printf("Running %zu tests\n", total);

    for (test_case_t *test = test_cases; test->func != NULL; ++test) {
        test_results.total_tests++;
        current_test_failed = 0;

        printf("test %s ... ", test->name);
        test->func();

        if (current_test_failed) {
            printf("%sFAILED%s\n", RED, COLOR_RESET);
        } else {
            test_results.passed_tests++;
            printf("%sok%s\n", GREEN, COLOR_RESET);
        }
    }

    print_test_results();
    cleanup_tests();
}

static inline void add_failed_message(const char *message, const char *file,
                                      int line) {
    if (test_results.failed_assertions >= test_results.failed_capacity) {
        size_t new_capacity = test_results.failed_capacity * 2;
        char **new_messages = (char **)realloc(test_results.failed_messages,
                                               sizeof(char *) * new_capacity);

        if (!new_messages) {
            fprintf(stderr, "Failed to reallocate memory for test messages\n");
            cleanup_tests();
            exit(EXIT_FAILURE);
        }

        test_results.failed_messages = new_messages;
        test_results.failed_capacity = new_capacity;
    }

    char *full_message = (char *)malloc(256);
    if (!full_message) {
        fprintf(stderr, "Failed to allocate memory for test message\n");
        cleanup_tests();
        exit(EXIT_FAILURE);
    }

    snprintf(full_message, 256, "%s (File: %s, Line: %d)", message, file, line);
    test_results.failed_messages[test_results.failed_assertions++] =
        full_message;
    current_test_failed = 1;
}

static inline void cleanup_tests() {
    if (test_results.failed_messages) {
        for (size_t i = 0; i < test_results.failed_assertions; ++i) {
            free(test_results.failed_messages[i]);
        }
        free(test_results.failed_messages);
        test_results.failed_messages = NULL;
    }
}

static inline void print_test_results() {
    if (test_results.failed_assertions > 0) {
        printf("\nfailures:\n\n");
        for (size_t i = 0; i < test_results.failed_assertions; ++i) {
            // printf("%s%s%s\n", RED, test_results.failed_messages[i],
            //        COLOR_RESET);
            printf("%s\n", test_results.failed_messages[i]);
        }
    }

    printf("\ntest result: %s%s%s. %zu passed; %zu failed\n",
           test_results.failed_assertions > 0 ? RED : GREEN,
           test_results.failed_assertions > 0 ? "FAILED" : "ok", COLOR_RESET,
           test_results.passed_tests,
           test_results.total_tests - test_results.passed_tests);
}

#endif
