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
static inline void print_test_results(void);
static inline void cleanup_tests(void);

static inline void tec_test_run(void (*function[])(void)) {
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

    for (size_t i = 0; function[i] != NULL; ++i) {
        function[i]();
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
    test_results.failed_tests++;
}

#define ASSERT_TRUE(condition)                                            \
    do {                                                                  \
        test_results.total_tests++;                                       \
        if (condition) {                                                  \
            test_results.passed_tests++;                                  \
        } else {                                                          \
            test_results.failed_tests;                                    \
            add_failed_message("Assertion failed: " #condition, __FILE__, \
                               __LINE__);                                 \
        }                                                                 \
    } while (0)

#define ASSERT_FALSE(condition) ASSERT_TRUE(!(condition))

#define ASSERT_EQUAL(expected, actual)                                  \
    do {                                                                \
        test_results.total_tests++;                                     \
        if ((expected) == (actual)) {                                   \
            test_results.passed_tests++;                                \
        } else {                                                        \
            test_results.failed_tests;                                  \
            char message[256];                                          \
            snprintf(message, sizeof(message), "Expected: %d, Got: %d", \
                     (expected), (actual));                             \
            add_failed_message(message, __FILE__, __LINE__);            \
        }                                                               \
    } while (0)

#define ASSERT_STR_EQUAL(expected, actual)                                     \
    do {                                                                       \
        test_results.total_tests++;                                            \
        if (strcmp((expected), (actual)) == 0) {                               \
            test_results.passed_tests++;                                       \
        } else {                                                               \
            test_results.failed_tests;                                         \
            char message[256];                                                 \
            snprintf(message, sizeof(message), "Expected string: %s, Got: %s", \
                     (expected), (actual));                                    \
            add_failed_message(message, __FILE__, __LINE__);                   \
        }                                                                      \
    } while (0)

// TODO:- I really want the status to be more like rust tests. those ok and fail
// kinda stuff.
static inline void print_test_results() {
    printf("\nTest Results:\n");
    printf("Total tests: %zu\n", test_results.total_tests);
    printf(GREEN "Passed tests: %zu\n" COLOR_RESET, test_results.passed_tests);

    if (test_results.failed_tests > 0) {
        printf(RED "Failed tests: %zu\n" COLOR_RESET,
               test_results.failed_tests);
        printf("\nFailed test details:\n");
        for (size_t i = 0; i < test_results.failed_tests; ++i) {
            printf(RED "%zu. %s\n" COLOR_RESET, i + 1,
                   test_results.failed_messages[i]);
        }
    }
}

#endif
