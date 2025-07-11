#ifndef TEC_H
#define TEC_H

#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEC_RED "\033[31m"
#define TEC_GREEN "\033[32m"
#define TEC_YELLOW "\033[33m"
#define TEC_BLUE "\033[34m"
#define TEC_MAGENTA "\033[35m"
#define TEC_CYAN "\033[36m"
#define TEC_RESET "\033[0m"

typedef void (*tec_func_t)(void);

typedef struct {
    const char* name;
    const char* file;
    tec_func_t func;
} tec_entry_t;

#define TEC_MAX_TESTS 1024
#define TEC_MAX_FAILURE_MESSAGE_LEN 512
static char tec_failure_message[TEC_MAX_FAILURE_MESSAGE_LEN];
static tec_entry_t tec_registry[TEC_MAX_TESTS];
static int tec_count = 0;
static int tec_current_passed = 0;
static int tec_current_failed = 0;
static jmp_buf tec_jump_buffer;
static int tec_jump_set = 0;

typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
    int total_assertions;
    int passed_assertions;
    int failed_assertions;
} tec_stats_t;

static tec_stats_t tec_stats = {0};

static void tec_register(const char* name, const char* file, tec_func_t func) {
    if (tec_count < TEC_MAX_TESTS) {
        tec_registry[tec_count].name = name;
        tec_registry[tec_count].file = file;
        tec_registry[tec_count].func = func;
        tec_count++;
    }
}

#define TEC_ASSERT(condition)                                          \
    do {                                                               \
        tec_stats.total_assertions++;                                  \
        if (!(condition)) {                                            \
            snprintf(tec_failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     "    " TEC_RED "✗" TEC_RESET                      \
                     " Assertion failed: %s (line %d)\n",              \
                     #condition, __LINE__);                            \
            tec_current_failed++;                                      \
            tec_stats.failed_assertions++;                             \
            if (tec_jump_set) longjmp(tec_jump_buffer, 1);             \
        } else {                                                       \
            tec_current_passed++;                                      \
            tec_stats.passed_assertions++;                             \
        }                                                              \
    } while (0)

#define TEC_ASSERT_EQ(a, b)                                            \
    do {                                                               \
        tec_stats.total_assertions++;                                  \
        if ((a) != (b)) {                                              \
            snprintf(tec_failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     "    " TEC_RED "✗" TEC_RESET                      \
                     " Expected %s == %s, got %ld != %ld (line %d)\n", \
                     #a, #b, (long)(a), (long)(b), __LINE__);          \
            tec_current_failed++;                                      \
            tec_stats.failed_assertions++;                             \
            if (tec_jump_set) longjmp(tec_jump_buffer, 1);             \
        } else {                                                       \
            tec_current_passed++;                                      \
            tec_stats.passed_assertions++;                             \
        }                                                              \
    } while (0)

#define TEC_ASSERT_NE(a, b)                                              \
    do {                                                                 \
        tec_stats.total_assertions++;                                    \
        if ((a) == (b)) {                                                \
            snprintf(tec_failure_message, TEC_MAX_FAILURE_MESSAGE_LEN,   \
                     "    " TEC_RED "✗" TEC_RESET                        \
                     " Expected %s != %s, but both are %ld (line %d)\n", \
                     #a, #b, (long)(a), __LINE__);                       \
            tec_current_failed++;                                        \
            tec_stats.failed_assertions++;                               \
            if (tec_jump_set) longjmp(tec_jump_buffer, 1);               \
        } else {                                                         \
            tec_current_passed++;                                        \
            tec_stats.passed_assertions++;                               \
        }                                                                \
    } while (0)

#define TEC_ASSERT_STR_EQ(a, b)                                               \
    do {                                                                      \
        tec_stats.total_assertions++;                                         \
        if (strcmp((a), (b)) != 0) {                                          \
            snprintf(tec_failure_message, TEC_MAX_FAILURE_MESSAGE_LEN,        \
                     "    " TEC_RED "✗" TEC_RESET                             \
                     " Expected strings equal: \"%s\" != \"%s\" (line %d)\n", \
                     (a), (b), __LINE__);                                     \
            tec_current_failed++;                                             \
            tec_stats.failed_assertions++;                                    \
            if (tec_jump_set) longjmp(tec_jump_buffer, 1);                    \
        } else {                                                              \
            tec_current_passed++;                                             \
            tec_stats.passed_assertions++;                                    \
        }                                                                     \
    } while (0)

#define TEC_ASSERT_NULL(ptr)                                           \
    do {                                                               \
        tec_stats.total_assertions++;                                  \
        if ((ptr) != NULL) {                                           \
            snprintf(tec_failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     "    " TEC_RED "✗" TEC_RESET                      \
                     " Expected %s to be NULL, got %p (line %d)\n",    \
                     #ptr, (void*)(ptr), __LINE__);                    \
            tec_current_failed++;                                      \
            tec_stats.failed_assertions++;                             \
            if (tec_jump_set) longjmp(tec_jump_buffer, 1);             \
        } else {                                                       \
            tec_current_passed++;                                      \
            tec_stats.passed_assertions++;                             \
        }                                                              \
    } while (0)

#define TEC_ASSERT_NOT_NULL(ptr)                                       \
    do {                                                               \
        tec_stats.total_assertions++;                                  \
        if ((ptr) == NULL) {                                           \
            snprintf(tec_failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     "    " TEC_RED "✗" TEC_RESET                      \
                     " Expected %s to not be NULL (line %d)\n",        \
                     #ptr, __LINE__);                                  \
            tec_current_failed++;                                      \
            tec_stats.failed_assertions++;                             \
            if (tec_jump_set) longjmp(tec_jump_buffer, 1);             \
        } else {                                                       \
            tec_current_passed++;                                      \
            tec_stats.passed_assertions++;                             \
        }                                                              \
    } while (0)

#define TEC(test_name)                                                        \
    static void tec_##test_name(void);                                        \
    static void __attribute__((constructor)) tec_register_##test_name(void) { \
        tec_register(#test_name, __FILE__, tec_##test_name);                  \
    }                                                                         \
    static void tec_##test_name(void)

static void tec_print_test_result(const char* test_name, const char* file,
                                  int failed) {
    const char* filename = strrchr(file, '/');
    if (filename)
        filename++;
    else
        filename = file;

    if (failed == 0) {
        printf("  " TEC_GREEN "✓" TEC_RESET " %s " TEC_CYAN "(%s)" TEC_RESET
               "\n",
               test_name, filename);
    } else {
        fprintf(stderr,
                "  " TEC_RED "✗" TEC_RESET " %s " TEC_CYAN "(%s)" TEC_RESET
                " - %d assertion(s) failed\n",
                test_name, filename, failed);
        fprintf(stderr, "%s", tec_failure_message);
    }
}

static int tec_run_all(void) {
    printf(TEC_BLUE "================================\n");
    printf("         C Test Runner          \n");
    printf("================================" TEC_RESET "\n\n");

    const char* current_file = NULL;

    for (int i = 0; i < tec_count; i++) {
        if (current_file == NULL ||
            strcmp(current_file, tec_registry[i].file) != 0) {
            current_file = tec_registry[i].file;
            const char* filename = strrchr(current_file, '/');
            if (filename)
                filename++;
            else
                filename = current_file;

            if (i > 0) printf("\n");
            printf(TEC_MAGENTA "%s" TEC_RESET "\n", filename);
        }

        tec_current_passed = 0;
        tec_current_failed = 0;

        tec_jump_set = 1;
        if (setjmp(tec_jump_buffer) == 0) {
            tec_registry[i].func();
        }
        tec_jump_set = 0;

        tec_print_test_result(tec_registry[i].name, tec_registry[i].file,
                              tec_current_failed);

        tec_stats.total_tests++;
        if (tec_current_failed == 0) {
            tec_stats.passed_tests++;
        } else {
            tec_stats.failed_tests++;
        }
    }

    printf("\n" TEC_BLUE "================================" TEC_RESET "\n");
    printf("Tests: %d total, " TEC_GREEN "%d passed" TEC_RESET ", " TEC_RED
           "%d failed" TEC_RESET "\n",
           tec_stats.total_tests, tec_stats.passed_tests,
           tec_stats.failed_tests);
    printf("Assertions: %d total, " TEC_GREEN "%d passed" TEC_RESET ", " TEC_RED
           "%d failed" TEC_RESET "\n",
           tec_stats.total_assertions, tec_stats.passed_assertions,
           tec_stats.failed_assertions);

    if (tec_stats.failed_tests == 0) {
        printf("\n" TEC_GREEN "All tests passed!" TEC_RESET "\n");
        return 0;
    } else {
        printf("\n" TEC_RED "Some tests failed!" TEC_RESET "\n");
        return 1;
    }
}

#define TEC_MAIN() \
    int main(void) { return tec_run_all(); }

#endif  // TEC_H
