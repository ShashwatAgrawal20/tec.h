#ifndef TEC_H
#define TEC_H

#include <setjmp.h>
#include <stdbool.h>
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

#define TEC_MAX_FAILURE_MESSAGE_LEN 1024
#define TEC_TMP_STRBUF_LEN 256
#define TEC_FMT_SLOTS 2
#define TEC_FMT_SLOT_SIZE TEC_TMP_STRBUF_LEN

typedef enum { TEC_INITIAL, TEC_FAIL, TEC_SKIP } JUMP_CODES;

typedef void (*tec_func_t)(void);

typedef struct {
    const char* suite;
    const char* name;
    const char* file;
    tec_func_t func;
} tec_entry_t;

typedef struct {
    jmp_buf jump_buffer;
    char failure_message[TEC_MAX_FAILURE_MESSAGE_LEN];
    char format_bufs[TEC_FMT_SLOTS][TEC_FMT_SLOT_SIZE];
    struct {
        size_t ran_tests;
        size_t passed_tests;
        size_t failed_tests;
        size_t skipped_tests;
        size_t filtered_tests;
        size_t total_assertions;
        size_t passed_assertions;
        size_t failed_assertions;
    } stats;
    struct {
        tec_entry_t* entries;
        size_t tec_count;
        size_t tec_capacity;
    } registry;
    struct {
        char** filters;
        size_t filter_count;
    } options;
    size_t current_passed;
    size_t current_failed;
    bool jump_set;
} tec_context_t;

void tec_register(const char* suite, const char* name, const char* file,
                  tec_func_t func);

extern tec_context_t tec_context;

#define TEC_FORMAT_VALUE_PAIR(x) TEC_FORMAT_SPEC(x), TEC_FORMAT_VALUE(x)

#define TEC_FMT(x, buf) \
    snprintf((buf), TEC_TMP_STRBUF_LEN, TEC_FORMAT_VALUE_PAIR(x))

#define TEC_TRY_BLOCK                                            \
    for (int _tec_loop_once = (tec_context.jump_set = true, 1);  \
         _tec_loop_once && setjmp(tec_context.jump_buffer) == 0; \
         _tec_loop_once = 0, tec_context.jump_set = false)

/*
 * don't fuck with this.
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
        unsigned long: "%zu",  \
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

#define TEC_ASSERT(condition)                                                  \
    do {                                                                       \
        tec_context.stats.total_assertions++;                                  \
        __auto_type _tec_cond_result = (condition);                            \
        if (!(_tec_cond_result)) {                                             \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     "    " TEC_RED "✗" TEC_RESET                              \
                     " Assertion failed: %s (line %d)\n",                      \
                     #condition, __LINE__);                                    \
            TEC_POST_FAIL();                                                   \
        } else {                                                               \
            TEC_POST_PASS();                                                   \
        }                                                                      \
    } while (0)

#define TEC_ASSERT_EQ(a, b)                                                    \
    do {                                                                       \
        tec_context.stats.total_assertions++;                                  \
        __auto_type _a = a;                                                    \
        __auto_type _b = b;                                                    \
        if ((_a) != (_b)) {                                                    \
            TEC_FMT(_a, tec_context.format_bufs[0]);                           \
            TEC_FMT(_b, tec_context.format_bufs[1]);                           \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     "    " TEC_RED "✗" TEC_RESET                              \
                     " Expected %s == %s, got %s != %s (line %d)\n",           \
                     #a, #b, tec_context.format_bufs[0],                       \
                     tec_context.format_bufs[1], __LINE__);                    \
            TEC_POST_FAIL();                                                   \
        } else {                                                               \
            TEC_POST_PASS();                                                   \
        }                                                                      \
    } while (0)

#define TEC_ASSERT_NE(a, b)                                                    \
    do {                                                                       \
        __auto_type _a = a;                                                    \
        __auto_type _b = b;                                                    \
        tec_context.stats.total_assertions++;                                  \
        if ((_a) == (_b)) {                                                    \
            TEC_FMT(_a, tec_context.format_bufs[0]);                           \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     "    " TEC_RED "✗" TEC_RESET                              \
                     " Expected %s != %s, but both are %s (line %d)\n",        \
                     #a, #b, tec_context.format_bufs[0], __LINE__);            \
            TEC_POST_FAIL();                                                   \
        } else {                                                               \
            TEC_POST_PASS();                                                   \
        }                                                                      \
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
        tec_context.stats.total_assertions++;                                  \
        const char* _a = (a);                                                  \
        const char* _b = (b);                                                  \
        int equal =                                                            \
            ((_a == NULL && _b == NULL) || (_a && _b && strcmp(_a, _b) == 0)); \
        if (!equal) {                                                          \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     "    " TEC_RED "✗" TEC_RESET                              \
                     " Expected strings equal: \"%s\" != \"%s\" (line %d)\n",  \
                     (_a ? _a : "(null)"), (_b ? _b : "(null)"), __LINE__);    \
            TEC_POST_FAIL();                                                   \
        } else {                                                               \
            TEC_POST_PASS();                                                   \
        }                                                                      \
    } while (0)

#define TEC_ASSERT_NULL(ptr)                                                   \
    do {                                                                       \
        tec_context.stats.total_assertions++;                                  \
        void* _ptr = ptr;                                                      \
        if ((_ptr) != NULL) {                                                  \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     "    " TEC_RED "✗" TEC_RESET                              \
                     " Expected %s to be NULL, got %p (line %d)\n",            \
                     #ptr, (void*)(_ptr), __LINE__);                           \
            TEC_POST_FAIL();                                                   \
        } else {                                                               \
            TEC_POST_PASS();                                                   \
        }                                                                      \
    } while (0)

#define TEC_ASSERT_NOT_NULL(ptr)                                               \
    do {                                                                       \
        tec_context.stats.total_assertions++;                                  \
        void* _ptr = ptr;                                                      \
        if ((_ptr) == NULL) {                                                  \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     "    " TEC_RED "✗" TEC_RESET                              \
                     " Expected %s to not be NULL (line %d)\n",                \
                     #ptr, __LINE__);                                          \
            TEC_POST_FAIL();                                                   \
        } else {                                                               \
            TEC_POST_PASS();                                                   \
        }                                                                      \
    } while (0)

#define TEC_ASSERT_GT(a, b) _TEC_ASSERT_OP(a, b, >)
#define TEC_ASSERT_GE(a, b) _TEC_ASSERT_OP(a, b, >=)
#define TEC_ASSERT_LT(a, b) _TEC_ASSERT_OP(a, b, <)
#define TEC_ASSERT_LE(a, b) _TEC_ASSERT_OP(a, b, <=)

#define _TEC_ASSERT_OP(a, b, op)                                               \
    do {                                                                       \
        tec_context.stats.total_assertions++;                                  \
        __auto_type _a = a;                                                    \
        __auto_type _b = b;                                                    \
        if (!(_a op _b)) {                                                     \
            TEC_FMT(_a, tec_context.format_bufs[0]);                           \
            TEC_FMT(_b, tec_context.format_bufs[1]);                           \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     "    " TEC_RED "✗" TEC_RESET " Expected %s " #op          \
                     " %s, got %s %s %s (line %d)\n",                          \
                     #a, #b, tec_context.format_bufs[0],                       \
                     #op[0] == '>' ? "<=" : ">=", tec_context.format_bufs[1],  \
                     __LINE__);                                                \
            TEC_POST_FAIL();                                                   \
        } else {                                                               \
            TEC_POST_PASS();                                                   \
        }                                                                      \
    } while (0)

#define TEC(suite_name, test_name)                      \
    static void tec_##suite_name_##test_name(void);     \
    static void __attribute__((constructor))            \
    tec_register_##suite_name_##test_name(void) {       \
        tec_register(#suite_name, #test_name, __FILE__, \
                     tec_##suite_name_##test_name);     \
    }                                                   \
    static void tec_##suite_name_##test_name(void)

#define TEC_POST_FAIL()                                                       \
    do {                                                                      \
        tec_context.current_failed++;                                         \
        tec_context.stats.failed_assertions++;                                \
        if (tec_context.jump_set) longjmp(tec_context.jump_buffer, TEC_FAIL); \
    } while (0);

#define TEC_POST_PASS()                        \
    do {                                       \
        tec_context.current_passed++;          \
        tec_context.stats.passed_assertions++; \
    } while (0);

#define TEC_SKIP(reason)                                                      \
    do {                                                                      \
        const char* _reason = (reason);                                       \
        snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN,    \
                 "    " TEC_YELLOW "»" TEC_RESET " Skipped: %s (line %d)\n",  \
                 _reason, __LINE__);                                          \
        if (tec_context.jump_set) longjmp(tec_context.jump_buffer, TEC_SKIP); \
    } while (0)

#ifdef TEC_IMPLEMENTATION
tec_context_t tec_context = {0};

int tec_compare_entries(const void* a, const void* b) {
    tec_entry_t* entry_a = (tec_entry_t*)a;
    tec_entry_t* entry_b = (tec_entry_t*)b;
    int suite_cmp = strcmp(entry_a->suite, entry_b->suite);
    if (suite_cmp != 0) {
        return suite_cmp;
    }
    return strcmp(entry_a->name, entry_b->name);
}

void tec_register(const char* suite, const char* name, const char* file,
                  tec_func_t func) {
    if (!suite || !name || !file || !func) {
        fprintf(stderr,
                TEC_RED "Error: NULL argument to tec_register\n" TEC_RESET);
        return;
    }

    if (tec_context.registry.tec_count >= tec_context.registry.tec_capacity) {
        tec_context.registry.tec_capacity =
            tec_context.registry.tec_capacity == 0
                ? 8
                : tec_context.registry.tec_capacity * 2;
        tec_entry_t* new_registry = (tec_entry_t*)realloc(
            tec_context.registry.entries,
            tec_context.registry.tec_capacity * sizeof(tec_entry_t));

        if (new_registry == NULL) {
            fprintf(stderr, TEC_RED
                    "Error: Failed to allocate memory for test "
                    "registry\n" TEC_RESET);
            free(tec_context.registry.entries);
            exit(1);
        }

        tec_context.registry.entries = new_registry;
    }

    tec_context.registry.entries[tec_context.registry.tec_count].suite = suite;
    tec_context.registry.entries[tec_context.registry.tec_count].name = name;
    tec_context.registry.entries[tec_context.registry.tec_count].file = file;
    tec_context.registry.entries[tec_context.registry.tec_count].func = func;
    tec_context.registry.tec_count++;
}

void tec_process_test_result(JUMP_CODES jump_val, const tec_entry_t* test) {
    if (jump_val == TEC_SKIP) {
        tec_context.stats.skipped_tests++;
        printf("  " TEC_YELLOW "»" TEC_RESET " %s\n", test->name);
        printf("%s", tec_context.failure_message);
    } else if (jump_val == TEC_FAIL || tec_context.current_failed > 0) {
        tec_context.stats.failed_tests++;
        printf("  " TEC_RED "✗" TEC_RESET " %s - %zu assertion(s) failed\n",
               test->name, tec_context.current_failed);
        printf("%s", tec_context.failure_message);
    } else {
        tec_context.stats.passed_tests++;
        printf("  " TEC_GREEN "✓" TEC_RESET " %s\n", test->name);
    }
}

void tec_print_usage(const char* prog_name) {
    printf("Usage: %s [options]\n", prog_name);
    printf("Options:\n");
    printf(
        "  -f, --filter <pattern>  Run tests matching pattern. "
        "Can be used multiple times.\n"
        "                          Pattern is matched against "
        "'suite_name.test_name'.\n");
    printf("  -h, --help              Display this help message.\n");
}

int tec_parse_args(int argc, char** argv) {
    if (argc < 2) {
        return 0;
    }
    tec_context.options.filters = (char**)calloc(argc, sizeof(char*));
    if (tec_context.options.filters == NULL) {
        fprintf(stderr,
                TEC_RED "Failed to allocate memory for filters\n" TEC_RESET);
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        if ((strcmp(argv[i], "-f") == 0) ||
            (strcmp(argv[i], "--filter") == 0)) {
            if (argc > ++i) {
                tec_context.options
                    .filters[tec_context.options.filter_count++] = argv[i];
            } else {
                fprintf(
                    stderr, TEC_RED
                    "Error: Filter option requires an argument.\n" TEC_RESET);
                return 1;
            }
        } else if (strcmp(argv[i], "-h") == 0 ||
                   strcmp(argv[i], "--help") == 0) {
            tec_print_usage(argv[0]);
            return 1;
        } else {
            fprintf(stderr, TEC_RED "Error: Unknown option '%s'\n" TEC_RESET,
                    argv[i]);
            tec_print_usage(argv[0]);
            return 1;
        }
    }
    return 0;
}

bool tec_should_run(const tec_entry_t* test) {
    char full_name[TEC_TMP_STRBUF_LEN];
    snprintf(full_name, sizeof(full_name), "%s.%s", test->suite, test->name);

    for (size_t i = 0; i < tec_context.options.filter_count; ++i) {
        if (strstr(full_name, tec_context.options.filters[i]) != NULL) {
            return true;
        }
    }

    return false;
}

int tec_run_all(int argc, char** argv) {
    int result = 0;
    result = tec_parse_args(argc, argv);
    if (result) goto cleanup;
    printf(TEC_BLUE "================================\n");
    printf("         C Test Runner          \n");
    printf("================================" TEC_RESET "\n");

    qsort(tec_context.registry.entries, tec_context.registry.tec_count,
          sizeof(tec_entry_t), tec_compare_entries);

    const char* current_suite = NULL;

    for (size_t i = 0; i < tec_context.registry.tec_count; ++i) {
        tec_entry_t* test = &tec_context.registry.entries[i];

        if (tec_context.options.filter_count != 0 && !tec_should_run(test)) {
            tec_context.stats.filtered_tests++;
            continue;
        }

        if (current_suite == NULL || strcmp(current_suite, test->suite) != 0) {
            current_suite = test->suite;
            const char* display_name = strstr(test->file, "tests/");

            if (display_name) {
                display_name = display_name + 6;
            } else {
                display_name = strrchr(test->file, '/');
                display_name = display_name ? display_name + 1 : test->file;
            }

            printf(TEC_MAGENTA "\nSUITE: %s" TEC_RESET " (%s)\n", current_suite,
                   display_name);
        }

        tec_context.current_passed = 0;
        tec_context.current_failed = 0;
        tec_context.failure_message[0] = '\0';
        tec_context.stats.ran_tests++;

        tec_context.jump_set = true;
        int jump_val = setjmp(tec_context.jump_buffer);
        if (jump_val == TEC_INITIAL) {
            test->func();
        }
        tec_context.jump_set = false;

        tec_process_test_result(jump_val, test);
    }

    printf("\n" TEC_BLUE "================================" TEC_RESET "\n");
    printf("Tests:      " TEC_GREEN "%zu passed" TEC_RESET ", " TEC_RED
           "%zu failed" TEC_RESET ", " TEC_YELLOW "%zu skipped" TEC_RESET
           ", " TEC_CYAN "%zu filtered" TEC_RESET " (%zu total)\n",
           tec_context.stats.passed_tests, tec_context.stats.failed_tests,
           tec_context.stats.skipped_tests, tec_context.stats.filtered_tests,
           tec_context.registry.tec_count);

    printf("Assertions: " TEC_GREEN "%zu passed" TEC_RESET ", " TEC_RED
           "%zu failed" TEC_RESET " (%zu total)\n",
           tec_context.stats.passed_assertions,
           tec_context.stats.failed_assertions,
           tec_context.stats.total_assertions);

    if (tec_context.stats.failed_tests == 0 &&
        tec_context.stats.skipped_tests == 0) {
        printf("\n" TEC_GREEN "All tests passed!" TEC_RESET "\n");
        result = 0;
    } else if (tec_context.stats.failed_tests > 0) {
        printf("\n" TEC_RED "Some tests failed!" TEC_RESET "\n");
        result = 1;
    } else {
        printf("\n" TEC_YELLOW "Tests passed, but some were skipped." TEC_RESET
               "\n");
        result = 0;
    }

cleanup:
    free(tec_context.registry.entries);
    free(tec_context.options.filters);
    memset(&tec_context, 0, sizeof(tec_context_t));
    return result;
}

#define TEC_MAIN() \
    int main(int argc, char** argv) { return tec_run_all(argc, argv); }

#endif  // TEC_IMPLEMENTATION
#endif  // TEC_H
