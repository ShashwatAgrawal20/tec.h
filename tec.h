#ifndef TEC_H
#define TEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define COLOR_RESET "\x1b[0m"
#define TEST_FUNCTIONS(...) \
    (void (*[])(void)) { __VA_ARGS__, NULL }

typedef struct {
    size_t total_tests;
    size_t passed_tests;
    size_t failed_tests;
    char **failed_messages;
    size_t failed_capacity;
} test_result_t;

static test_result_t test_results = {0, 0, 0, NULL, 0};
static int current_test_failed = 0;
static inline void print_test_results(void);
static inline void cleanup_tests(void);

static inline void tec_test_run(void (*function[])(void), size_t size) {
    test_results.total_tests = 0;
    test_results.passed_tests = 0;
    test_results.failed_tests = 0;
    test_results.failed_capacity = 10;
    test_results.failed_messages =
        (char **)malloc(sizeof(char *) * test_results.failed_capacity);

    if (test_results.failed_messages == NULL) {
        fprintf(stderr, "Failed to allocate memory for test messages\n");
        exit(EXIT_FAILURE);
    }

    printf("running %zu tests\n", size - 1);
    for (size_t i = 0; function[i] != NULL; ++i) {
        test_results.total_tests++;
        current_test_failed = 0;
        printf("test %s ... ", "FUNCTION_NAME_TEMPLATE");
        function[i]();
        if (current_test_failed) {
            test_results.failed_tests++;
            printf("%sFAILED%s\n", RED, COLOR_RESET);
        } else {
            test_results.passed_tests++;
            printf("%sok%s\n", GREEN, COLOR_RESET);
        }
    }

    print_test_results();
    cleanup_tests();
}

static inline void cleanup_tests() {
    if (test_results.failed_messages != NULL) {
        for (size_t i = 0; i < test_results.failed_tests; ++i) {
            free(test_results.failed_messages[i]);
        }
        free(test_results.failed_messages);
        test_results.failed_messages = NULL;
    }
}

static inline void add_failed_message(const char *message, const char *file,
                                      int line) {
    if (test_results.failed_tests >= test_results.failed_capacity) {
        test_results.failed_capacity *= 2;
        char **failed_messages_realloc =
            (char **)realloc(test_results.failed_messages,
                             sizeof(char *) * test_results.failed_capacity);

        if (failed_messages_realloc == NULL) {
            fprintf(stderr, "Failed to reallocate memory for test messages\n");
            cleanup_tests();  // Clean up what we had before
            exit(EXIT_FAILURE);
        }
        test_results.failed_messages = failed_messages_realloc;
    }

    char *full_message = (char *)malloc(256);
    if (full_message == NULL) {
        fprintf(stderr, "Failed to allocate memory for test message\n");
        cleanup_tests();
        exit(EXIT_FAILURE);
    }
    snprintf(full_message, 256, "%s (File: %s, Line: %d)", message, file, line);

    test_results.failed_messages[test_results.failed_tests] = full_message;
    current_test_failed = 1;
}

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
            char message[256];                                          \
            snprintf(message, sizeof(message), "Expected: %s, Got: %s", \
                     #expected, #actual);                               \
            add_failed_message(message, __FILE__, __LINE__);            \
        }                                                               \
    } while (0)

#define ASSERT_NOT_EQUAL(expected, actual)                                \
    do {                                                                  \
        typeof(expected) _expected = (expected);                          \
        typeof(actual) _actual = (actual);                                \
        if (_expected == _actual) {                                       \
            char message[256];                                            \
            snprintf(message, sizeof(message),                            \
                     "Expected '%s' (%d) to be different from '%s' (%d)", \
                     #expected, _expected, #actual, _actual);             \
            add_failed_message(message, __FILE__, __LINE__);              \
        }                                                                 \
    } while (0)

#define ASSERT_STR_EQUAL(expected, actual)                                     \
    do {                                                                       \
        if (strcmp((expected), (actual)) != 0) {                               \
            char message[256];                                                 \
            snprintf(message, sizeof(message), "Expected string: %s, Got: %s", \
                     (expected), (actual));                                    \
            add_failed_message(message, __FILE__, __LINE__);                   \
        }                                                                      \
    } while (0)

// TODO:- I really want the status to be more like rust tests. those ok and fail
// kinda stuff.
static inline void print_test_results() {
    if (test_results.failed_tests > 0) {
        printf("\nfailures:\n\n");
        for (size_t i = 0; i < test_results.failed_tests; ++i) {
            printf("%s%s%s\n", RED, test_results.failed_messages[i],
                   COLOR_RESET);
        }
    }
    printf("\ntest result: %s%s%s. %zu passed; %zu failed\n",
           test_results.failed_tests > 0 ? RED : GREEN,
           test_results.failed_tests > 0 ? "failed" : "passed", COLOR_RESET,
           test_results.passed_tests, test_results.failed_tests);
}

#endif
