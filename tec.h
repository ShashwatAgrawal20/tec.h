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

#ifndef TEC_MAX_TESTS
#define TEC_MAX_TESTS 1024
#endif
#define TEC_MAX_FAILURE_MESSAGE_LEN 512
#define TEC_TMP_STRBUF_LEN 32

typedef void (*tec_func_t)(void);

typedef struct {
    const char* name;
    const char* file;
    tec_func_t func;
} tec_entry_t;

typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
    int total_assertions;
    int passed_assertions;
    int failed_assertions;
} tec_stats_t;

void tec_register(const char* name, const char* file, tec_func_t func);
int tec_run_all(void);

extern tec_stats_t tec_stats;
extern char tec_failure_message[];
extern tec_entry_t tec_registry[];
extern int tec_count;
extern int tec_current_passed;
extern int tec_current_failed;
extern jmp_buf tec_jump_buffer;
extern int tec_jump_set;

/*
 * keep TEC_FORMAT_SPEC and TEC_FORMAT_VALUE split to avoid -Wformat issues
 * I tried snprintf-style macro but, it caused bogus format warnings on the LSP
 * side, splitting format and value avoids LSP noise and keeps type safety.
 * default case now uses (const void *)&x to bypass int-to-pointer-size
 * warnings.
 */
#define TEC_FORMAT_SPEC(x)     \
    _Generic((x),              \
        int: "%d",             \
        float: "%f",           \
        double: "%lf",         \
        unsigned long: "%lu",  \
        unsigned int: "%u",    \
        unsigned short: "%hu", \
        unsigned char: "%hhu", \
        signed long: "%ld",    \
        signed short: "%hd",   \
        signed char: "%hhd",   \
        const char*: "%s",     \
        char*: "%s",           \
        default: "%p")

#define TEC_FORMAT_VALUE(x)  \
    _Generic((x),            \
        int: (x),            \
        float: (x),          \
        double: (x),         \
        unsigned long: (x),  \
        unsigned int: (x),   \
        unsigned short: (x), \
        unsigned char: (x),  \
        signed long: (x),    \
        signed short: (x),   \
        signed char: (x),    \
        const char*: (x),    \
        char*: (x),          \
        default: (const void*)&(x))  // avoids int-to-pointer warning

#define TEC_ASSERT(condition)                                          \
    do {                                                               \
        tec_stats.total_assertions++;                                  \
        __auto_type _tec_cond_result = (condition);                    \
        if (!(_tec_cond_result)) {                                     \
            snprintf(tec_failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     "    " TEC_RED "✗" TEC_RESET                      \
                     " Assertion failed: %s (line %d)\n",              \
                     #condition, __LINE__);                            \
            TEC_POST_FAIL();                                           \
        } else {                                                       \
            TEC_POST_PASS();                                           \
        }                                                              \
    } while (0)

#define TEC_ASSERT_EQ(a, b)                                            \
    do {                                                               \
        tec_stats.total_assertions++;                                  \
        __auto_type _a = a;                                            \
        __auto_type _b = b;                                            \
        if ((_a) != (_b)) {                                            \
            char a_str[TEC_TMP_STRBUF_LEN];                            \
            char b_str[TEC_TMP_STRBUF_LEN];                            \
            snprintf(a_str, sizeof(a_str), TEC_FORMAT_SPEC(_a),        \
                     TEC_FORMAT_VALUE(_a));                            \
            snprintf(b_str, sizeof(b_str), TEC_FORMAT_SPEC(_b),        \
                     TEC_FORMAT_VALUE(_b));                            \
            snprintf(tec_failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     "    " TEC_RED "✗" TEC_RESET                      \
                     " Expected %s == %s, got %s != %s (line %d)\n",   \
                     #a, #b, a_str, b_str, __LINE__);                  \
            TEC_POST_FAIL();                                           \
        } else {                                                       \
            TEC_POST_PASS();                                           \
        }                                                              \
    } while (0)

#define TEC_ASSERT_NE(a, b)                                             \
    do {                                                                \
        __auto_type _a = a;                                             \
        __auto_type _b = b;                                             \
        tec_stats.total_assertions++;                                   \
        if ((_a) == (_b)) {                                             \
            char a_str[TEC_TMP_STRBUF_LEN];                             \
            snprintf(a_str, sizeof(a_str), TEC_FORMAT_SPEC(_a),         \
                     TEC_FORMAT_VALUE(_a));                             \
            snprintf(tec_failure_message, TEC_MAX_FAILURE_MESSAGE_LEN,  \
                     "    " TEC_RED "✗" TEC_RESET                       \
                     " Expected %s != %s, but both are %s (line %d)\n", \
                     #a, #b, a_str, __LINE__);                          \
            TEC_POST_FAIL();                                            \
        } else {                                                        \
            TEC_POST_PASS();                                            \
        }                                                               \
    } while (0)

/*
 * NOTE: If both strings are NULL, should this count as equal or not?
 * Current behavior treats it as a failure not sure if that's a feature or a
 * bug.
 *
 * PS: DONE
 */
#define TEC_ASSERT_STR_EQ(a, b)                                                \
    do {                                                                       \
        tec_stats.total_assertions++;                                          \
        const char* _a = (a);                                                  \
        const char* _b = (b);                                                  \
        int equal =                                                            \
            ((_a == NULL && _b == NULL) || (_a && _b && strcmp(_a, _b) == 0)); \
        if (!equal) {                                                          \
            snprintf(tec_failure_message, TEC_MAX_FAILURE_MESSAGE_LEN,         \
                     "    " TEC_RED "✗" TEC_RESET                              \
                     " Expected strings equal: \"%s\" != \"%s\" (line %d)\n",  \
                     (_a), (_b), __LINE__);                                    \
            TEC_POST_FAIL();                                                   \
        } else {                                                               \
            TEC_POST_PASS();                                                   \
        }                                                                      \
    } while (0)

#define TEC_ASSERT_NULL(ptr)                                           \
    do {                                                               \
        tec_stats.total_assertions++;                                  \
        void* _ptr = ptr;                                              \
        if ((_ptr) != NULL) {                                          \
            snprintf(tec_failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     "    " TEC_RED "✗" TEC_RESET                      \
                     " Expected %s to be NULL, got %p (line %d)\n",    \
                     #ptr, (void*)(_ptr), __LINE__);                   \
            TEC_POST_FAIL();                                           \
        } else {                                                       \
            TEC_POST_PASS();                                           \
        }                                                              \
    } while (0)

#define TEC_ASSERT_NOT_NULL(ptr)                                       \
    do {                                                               \
        tec_stats.total_assertions++;                                  \
        void* _ptr = ptr;                                              \
        if ((_ptr) == NULL) {                                          \
            snprintf(tec_failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     "    " TEC_RED "✗" TEC_RESET                      \
                     " Expected %s to not be NULL (line %d)\n",        \
                     #ptr, __LINE__);                                  \
            TEC_POST_FAIL();                                           \
        } else {                                                       \
            TEC_POST_PASS();                                           \
        }                                                              \
    } while (0)

#define TEC(test_name)                                                        \
    static void tec_##test_name(void);                                        \
    static void __attribute__((constructor)) tec_register_##test_name(void) { \
        tec_register(#test_name, __FILE__, tec_##test_name);                  \
    }                                                                         \
    static void tec_##test_name(void)

#define TEC_POST_FAIL()                                \
    do {                                               \
        tec_current_failed++;                          \
        tec_stats.failed_assertions++;                 \
        if (tec_jump_set) longjmp(tec_jump_buffer, 1); \
    } while (0);

#define TEC_POST_PASS()                \
    do {                               \
        tec_current_passed++;          \
        tec_stats.passed_assertions++; \
    } while (0);

#ifdef TEC_IMPLEMENTATION
char tec_failure_message[TEC_MAX_FAILURE_MESSAGE_LEN];
tec_entry_t tec_registry[TEC_MAX_TESTS];
int tec_count = 0;
int tec_current_passed = 0;
int tec_current_failed = 0;
jmp_buf tec_jump_buffer;
int tec_jump_set = 0;
tec_stats_t tec_stats = {0};

void tec_register(const char* name, const char* file, tec_func_t func) {
    if (!name || !file || !func) {
        fprintf(stderr,
                TEC_RED "Error: NULL argument to tec_register\n" TEC_RESET);
        return;
    }
    if (tec_count < TEC_MAX_TESTS) {
        tec_registry[tec_count].name = name;
        tec_registry[tec_count].file = file;
        tec_registry[tec_count].func = func;
        tec_count++;
    }
}

void tec_print_test_result(const char* test_name, int failed) {
    if (failed == 0) {
        printf("  " TEC_GREEN "✓" TEC_RESET " %s \n", test_name);
    } else {
        fprintf(stderr,
                "  " TEC_RED "✗" TEC_RESET
                " %s "
                " - %d assertion(s) failed\n",
                test_name, failed);
        fprintf(stderr, "%s", tec_failure_message);
    }
}

int tec_run_all(void) {
    printf(TEC_BLUE "================================\n");
    printf("         C Test Runner          \n");
    printf("================================" TEC_RESET "\n\n");

    const char* current_file = NULL;

    for (int i = 0; i < tec_count; ++i) {
        if (current_file == NULL ||
            strcmp(current_file, tec_registry[i].file) != 0) {
            current_file = tec_registry[i].file;
            const char* display_name = current_file;
            const char* prefix_to_strip = "tests/";
            const size_t prefix_to_strip_len = strlen(prefix_to_strip);

            if (strncmp(display_name, prefix_to_strip, prefix_to_strip_len) ==
                0) {
                display_name += prefix_to_strip_len;
            }

            if (i > 0) printf("\n");
            printf(TEC_MAGENTA "%s" TEC_RESET "\n", display_name);
        }

        tec_current_passed = 0;
        tec_current_failed = 0;

        tec_jump_set = 1;
        if (setjmp(tec_jump_buffer) == 0) {
            tec_registry[i].func();
        }
        tec_jump_set = 0;

        tec_print_test_result(tec_registry[i].name, tec_current_failed);

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

#endif  // TEC_IMPLEMENTATION
#endif  // TEC_H
