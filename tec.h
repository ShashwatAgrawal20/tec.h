/*
 * MIT License
 *
 * Copyright (c) 2025 Shashwat Agrawal
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef TEC_H
#define TEC_H

#include <float.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
#include <sstream>
#include <stdexcept>
#include <string>
#endif

#ifdef __cplusplus
extern "C" {
#endif
#define TEC_RED "\033[31m"
#define TEC_GREEN "\033[32m"
#define TEC_YELLOW "\033[33m"
#define TEC_BLUE "\033[34m"
#define TEC_MAGENTA "\033[35m"
#define TEC_CYAN "\033[36m"
#define TEC_RESET "\033[0m"

// MIGHT MAKE THE ASCII one the defaults
#ifdef _WIN32
#define TEC_TICK_CHAR "[+]"
#define TEC_CROSS_CHAR "[x]"
#define TEC_ARROW_CHAR "->"
#define TEC_LINE_CHAR "|"
#else
#define TEC_TICK_CHAR "✓"
#define TEC_CROSS_CHAR "✗"
#define TEC_ARROW_CHAR "»"
#define TEC_LINE_CHAR "│"
#endif

#define TEC_PRE_SPACE "    "
#define TEC_PRE_SPACE_SHORT "  "

#define TEC_MAX_FAILURE_MESSAGE_LEN 1024
#define TEC_TMP_STRBUF_LEN 256
#define TEC_FMT_SLOTS 2
#define TEC_FMT_SLOT_SIZE TEC_TMP_STRBUF_LEN

#define _TEC_FABS(x) ((x) < 0.0 ? -(x) : (x))

typedef enum { TEC_INITIAL, TEC_FAIL, TEC_SKIP_e } JUMP_CODES;

typedef void (*tec_func_t)(void);

typedef struct {
    const char* suite;
    const char* name;
    const char* file;
    tec_func_t func;
    bool xfail;
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
        bool filter_by_filename;
    } options;
    size_t current_passed;
    size_t current_failed;
    bool jump_set;
} tec_context_t;

void tec_register(const char* suite, const char* name, const char* file,
                  tec_func_t func, bool xfail);

void _tec_post_wrapper(bool is_fail_case);
void TEC_POST_FAIL(void);
void _tec_skip_impl(const char* reason, int line);

extern tec_context_t tec_context;

#ifdef __cplusplus
}  // extern "C"
#endif

#ifdef __cplusplus
#define TEC_AUTO_TYPE auto
#else
#define TEC_AUTO_TYPE __auto_type
#endif

#ifdef __cplusplus
class tec_assertion_failure : public std::runtime_error {
   public:
    tec_assertion_failure(const char* msg) : std::runtime_error(msg) {}
};
class tec_skip_test : public std::runtime_error {
   public:
    tec_skip_test(const char* msg) : std::runtime_error(msg) {}
};
#endif

#ifdef __cplusplus
template <typename T>
std::string tec_to_string(const T& value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}
inline std::string tec_to_string(const char* value) {
    if (value == NULL) return "(null)";
    return "\"" + std::string(value) + "\"";
}
inline std::string tec_to_string(char* value) {
    return tec_to_string(const_cast<const char*>(value));
}
#define TEC_FMT(x, buf) \
    snprintf((buf), TEC_FMT_SLOT_SIZE, "%s", tec_to_string(x).c_str())

#else  // C-ONLY: Original _Generic implementation
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
#define TEC_FORMAT_SPEC(x)                                                \
    _Generic((x),                                                         \
        int8_t: "%hhd",                                                   \
        int16_t: "%hd",                                                   \
        int32_t: "%d",                                                    \
        int64_t: "%ld", /* fuck this. lp64 vs llp64; portable C my ass */ \
        uint8_t: "%hhu",                                                  \
        uint16_t: "%hu",                                                  \
        uint32_t: "%u",                                                   \
        size_t: "%zu", /* fuck windows, fuck mingw/msvc-crt, fuck me */   \
        float: "%f",                                                      \
        double: "%lf",                                                    \
        long double: "%Lf",                                               \
        char*: "%s",                                                      \
        const char*: "%s",                                                \
        default: "%p")

#define TEC_FORMAT_VALUE(x) \
    _Generic((x),           \
        int8_t: (x),        \
        int16_t: (x),       \
        int32_t: (x),       \
        int64_t: (x),       \
        uint8_t: (x),       \
        uint16_t: (x),      \
        uint32_t: (x),      \
        size_t: (x),        \
        float: (x),         \
        double: (x),        \
        long double: (x),   \
        char*: (x),         \
        const char*: (x),   \
        default: (const void*)&(x))  // avoids int-to-pointer warning
#endif

#define TEC_POST_PASS()                        \
    do {                                       \
        tec_context.current_passed++;          \
        tec_context.stats.passed_assertions++; \
    } while (0);

#define TEC_SKIP(reason) _tec_skip_impl(reason, __LINE__)

#define TEC_ASSERT(condition)                                                  \
    do {                                                                       \
        tec_context.stats.total_assertions++;                                  \
        TEC_AUTO_TYPE _tec_cond_result = (condition);                          \
        if (!(_tec_cond_result)) {                                             \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     TEC_PRE_SPACE TEC_RED TEC_CROSS_CHAR TEC_RESET            \
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
        TEC_AUTO_TYPE _a = a;                                                  \
        TEC_AUTO_TYPE _b = b;                                                  \
        if ((_a) != (_b)) {                                                    \
            TEC_FMT(_a, tec_context.format_bufs[0]);                           \
            TEC_FMT(_b, tec_context.format_bufs[1]);                           \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     TEC_PRE_SPACE TEC_RED TEC_CROSS_CHAR TEC_RESET            \
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
        TEC_AUTO_TYPE _a = a;                                                  \
        TEC_AUTO_TYPE _b = b;                                                  \
        tec_context.stats.total_assertions++;                                  \
        if ((_a) == (_b)) {                                                    \
            TEC_FMT(_a, tec_context.format_bufs[0]);                           \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     TEC_PRE_SPACE TEC_RED TEC_CROSS_CHAR TEC_RESET            \
                     " Expected %s != %s, but both are %s (line %d)\n",        \
                     #a, #b, tec_context.format_bufs[0], __LINE__);            \
            TEC_POST_FAIL();                                                   \
        } else {                                                               \
            TEC_POST_PASS();                                                   \
        }                                                                      \
    } while (0)

#define TEC_ASSERT_NEAR(a, b, tolerance)                                       \
    do {                                                                       \
        tec_context.stats.total_assertions++;                                  \
        TEC_AUTO_TYPE _a = (a);                                                \
        TEC_AUTO_TYPE _b = (b);                                                \
        TEC_AUTO_TYPE _tol = (tolerance);                                      \
        TEC_AUTO_TYPE _diff = _TEC_FABS((double)_a - (double)_b);              \
        if (_diff > (double)_tol) {                                            \
            snprintf(                                                          \
                tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN,      \
                TEC_PRE_SPACE TEC_RED TEC_CROSS_CHAR TEC_RESET                 \
                " Nearness assertion failed "                                  \
                "(line %d)\n" TEC_PRE_SPACE TEC_YELLOW TEC_LINE_CHAR TEC_RESET \
                " Expected: %s and %s "                                        \
                "to be within %g\n" TEC_PRE_SPACE TEC_YELLOW                   \
                    TEC_LINE_CHAR TEC_RESET                                    \
                " Actual:   they differ "                                      \
                "by %g\n",                                                     \
                __LINE__, #a, #b, (double)_tol, _diff);                        \
            TEC_POST_FAIL();                                                   \
        } else {                                                               \
            TEC_POST_PASS();                                                   \
        }                                                                      \
    } while (0)

#define TEC_ASSERT_FLOAT_EQ(a, b)                                              \
    do {                                                                       \
        tec_context.stats.total_assertions++;                                  \
        TEC_AUTO_TYPE _a = (a);                                                \
        TEC_AUTO_TYPE _b = (b);                                                \
        double _default_tol = DBL_EPSILON * 4.0;                               \
        double _diff = _TEC_FABS((double)_a - (double)_b);                     \
        if (_diff > _default_tol) {                                            \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     TEC_PRE_SPACE TEC_RED TEC_CROSS_CHAR TEC_RESET            \
                     " Floating point equality "                               \
                     "failed (line %d)\n" TEC_PRE_SPACE TEC_YELLOW             \
                         TEC_LINE_CHAR TEC_RESET                               \
                     " Expected: %s == %s\n" TEC_PRE_SPACE TEC_YELLOW          \
                         TEC_LINE_CHAR TEC_RESET                               \
                     " Actual:   %s (%g)\n" TEC_PRE_SPACE TEC_YELLOW           \
                         TEC_LINE_CHAR TEC_RESET                               \
                     "      and: %s (%g)\n" TEC_PRE_SPACE TEC_YELLOW           \
                         TEC_LINE_CHAR TEC_RESET                               \
                     " Difference: %g ( > "                                    \
                     "tolerance %g)\n",                                        \
                     __LINE__, #a, #b, #a, (double)_a, #b, (double)_b, _diff,  \
                     _default_tol);                                            \
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
                     TEC_PRE_SPACE TEC_RED TEC_CROSS_CHAR TEC_RESET            \
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
        const void* _ptr = ptr;                                                \
        if ((_ptr) != NULL) {                                                  \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     TEC_PRE_SPACE TEC_RED TEC_CROSS_CHAR TEC_RESET            \
                     " Expected %s to be NULL, got %p (line %d)\n",            \
                     #ptr, (const void*)(_ptr), __LINE__);                     \
            TEC_POST_FAIL();                                                   \
        } else {                                                               \
            TEC_POST_PASS();                                                   \
        }                                                                      \
    } while (0)

#define TEC_ASSERT_NOT_NULL(ptr)                                               \
    do {                                                                       \
        tec_context.stats.total_assertions++;                                  \
        const void* _ptr = ptr;                                                \
        if ((_ptr) == NULL) {                                                  \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     TEC_PRE_SPACE TEC_RED TEC_CROSS_CHAR TEC_RESET            \
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
        TEC_AUTO_TYPE _a = a;                                                  \
        TEC_AUTO_TYPE _b = b;                                                  \
        if (!(_a op _b)) {                                                     \
            TEC_FMT(_a, tec_context.format_bufs[0]);                           \
            TEC_FMT(_b, tec_context.format_bufs[1]);                           \
            const char* _op_str = #op;                                         \
            const char* _inv_op_str =                                          \
                ((strcmp(_op_str, ">") == 0)    ? "<="                         \
                 : (strcmp(_op_str, ">=") == 0) ? "<"                          \
                 : (strcmp(_op_str, "<") == 0)  ? ">="                         \
                 : (strcmp(_op_str, "<=") == 0) ? ">"                          \
                                                : "???");                      \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     TEC_PRE_SPACE TEC_RED TEC_CROSS_CHAR TEC_RESET            \
                     " Expected %s %s %s, got %s %s %s (line %d)\n",           \
                     #a, _op_str, #b, tec_context.format_bufs[0], _inv_op_str, \
                     tec_context.format_bufs[1], __LINE__);                    \
            TEC_POST_FAIL();                                                   \
        } else {                                                               \
            TEC_POST_PASS();                                                   \
        }                                                                      \
    } while (0)

#define TEC(suite_name, test_name)                         \
    static void tec_##suite_name_##test_name(void);        \
    static void __attribute__((constructor))               \
    tec_register_##suite_name_##test_name(void) {          \
        tec_register(#suite_name, #test_name, __FILE__,    \
                     tec_##suite_name_##test_name, false); \
    }                                                      \
    static void tec_##suite_name_##test_name(void)

#define TEC_XFAIL(suite_name, test_name)                  \
    static void tec_##suite_name_##test_name(void);       \
    static void __attribute__((constructor))              \
    tec_register_##suite_name_##test_name(void) {         \
        tec_register(#suite_name, #test_name, __FILE__,   \
                     tec_##suite_name_##test_name, true); \
    }                                                     \
    static void tec_##suite_name_##test_name(void)

#ifdef TEC_IMPLEMENTATION
#ifdef __cplusplus
extern "C" {
#endif

tec_context_t tec_context = {};

inline void TEC_POST_FAIL(void) {
    tec_context.current_failed++;
    tec_context.stats.failed_assertions++;
#ifdef __cplusplus
    throw tec_assertion_failure(tec_context.failure_message);
#else
    if (tec_context.jump_set) longjmp(tec_context.jump_buffer, TEC_FAIL);
#endif
}

inline void _tec_skip_impl(const char* reason, int line) {
    const char* _reason = (reason);
    snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN,
             TEC_PRE_SPACE TEC_YELLOW TEC_ARROW_CHAR TEC_RESET
             " Skipped: %s (line %d)\n",
             _reason, line);
#ifdef __cplusplus
    throw tec_skip_test(tec_context.failure_message);
#else
    if (tec_context.jump_set) longjmp(tec_context.jump_buffer, TEC_SKIP_e);
#endif
}

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
                  tec_func_t func, bool xfail) {
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
    tec_context.registry.entries[tec_context.registry.tec_count].xfail = xfail;
    tec_context.registry.tec_count++;
}

void tec_process_test_result(JUMP_CODES jump_val, const tec_entry_t* test) {
    bool has_failed = (jump_val == TEC_FAIL || tec_context.current_failed > 0);
    if (jump_val == TEC_SKIP_e) {
        tec_context.stats.skipped_tests++;
        printf(TEC_PRE_SPACE_SHORT TEC_YELLOW TEC_ARROW_CHAR TEC_RESET " %s\n",
               test->name);
        printf("%s", tec_context.failure_message);
        return;
    }
    if (test->xfail) {
        if (has_failed) {
            tec_context.stats.passed_tests++;
            printf(TEC_PRE_SPACE_SHORT TEC_GREEN TEC_TICK_CHAR TEC_RESET
                   " %s (expected failure)\n",
                   test->name);
        } else {
            tec_context.stats.failed_tests++;
            printf(TEC_PRE_SPACE_SHORT TEC_RED TEC_CROSS_CHAR TEC_RESET
                   " %s (unexpected success)\n",
                   test->name);
        }
    } else {
        if (has_failed) {
            tec_context.stats.failed_tests++;
            printf(TEC_PRE_SPACE_SHORT TEC_RED TEC_CROSS_CHAR TEC_RESET
                   " %s - %zu assertion(s) failed\n",
                   test->name, tec_context.current_failed);
            printf("%s", tec_context.failure_message);
        } else {
            tec_context.stats.passed_tests++;
            printf(TEC_PRE_SPACE_SHORT TEC_GREEN TEC_TICK_CHAR TEC_RESET
                   " %s\n",
                   test->name);
        }
    }
}

void tec_print_usage(const char* prog_name) {
    printf("Usage: %s [options]\n", prog_name);
    printf("Options:\n");
    printf(
        "  -f, --filter <pattern>  Run tests where the name contains the given "
        "pattern.\n"
        "                          By default, matches against "
        "'suite.name'.\n");
    printf(
        "  --file                  When present, changes the behavior of '-f' "
        "to match against\n"
        "                          the test's filename instead.\n");
    printf("  -h, --help              Display this help message.\n\n");
    printf("Examples:\n");
    printf(
        "  %s -f 'math'             # Run all tests with 'Math' in their "
        "name\n",
        prog_name);
    printf(
        "  %s --file -f 'math_utils.c'  # Run all tests in files with "
        "'_tests.c' in the name\n",
        prog_name);
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
        } else if (strcmp(argv[i], "--file") == 0) {
            tec_context.options.filter_by_filename = true;
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
    char full_name_buffer[TEC_TMP_STRBUF_LEN];
    const char* target_string;

    if (tec_context.options.filter_by_filename) {
        target_string = test->file;
    } else {
        snprintf(full_name_buffer, sizeof(full_name_buffer), "%s.%s",
                 test->suite, test->name);
        target_string = full_name_buffer;
    }

    for (size_t i = 0; i < tec_context.options.filter_count; ++i) {
        if (strstr(target_string, tec_context.options.filters[i]) != NULL) {
            return true;
        }
    }

    return false;
}

int tec_run_all(int argc, char** argv) {
    int result = 0;
    const char* current_suite = NULL;
    result = tec_parse_args(argc, argv);
    if (result) goto cleanup;
    printf(TEC_BLUE "================================\n");
    printf("         C Test Runner          \n");
    printf("================================" TEC_RESET "\n");

    qsort(tec_context.registry.entries, tec_context.registry.tec_count,
          sizeof(tec_entry_t), tec_compare_entries);

    for (size_t i = 0; i < tec_context.registry.tec_count; ++i) {
        tec_entry_t* test = &tec_context.registry.entries[i];

        if (tec_context.options.filter_count != 0 && !tec_should_run(test)) {
            tec_context.stats.filtered_tests++;
            continue;
        }

        if (current_suite == NULL || strcmp(current_suite, test->suite) != 0) {
            current_suite = test->suite;
            const char* display_name = strstr(test->file, "tests/");
            if (display_name == NULL) {
                display_name = strstr(test->file, "tests\\");
            }

            if (display_name) {
                display_name = display_name + 6;
            } else {
                const char* f_slash = strrchr(test->file, '/');
                const char* b_slash = strrchr(test->file, '\\');
                const char* last_slash =
                    (f_slash > b_slash) ? f_slash : b_slash;

                display_name = last_slash ? last_slash + 1 : test->file;
            }

            printf(TEC_MAGENTA "\nSUITE: %s" TEC_RESET " (%s)\n", current_suite,
                   display_name);
        }

        tec_context.current_passed = 0;
        tec_context.current_failed = 0;
        tec_context.failure_message[0] = '\0';
        tec_context.stats.ran_tests++;

#ifdef __cplusplus
        try {
            test->func();
            tec_process_test_result(TEC_INITIAL, test);
        } catch (const tec_assertion_failure&) {
            tec_process_test_result(TEC_FAIL, test);
        } catch (const tec_skip_test&) {
            tec_process_test_result(TEC_SKIP_e, test);
        } catch (const std::exception& e) {
            tec_context.current_failed++;
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN,
                     TEC_PRE_SPACE_SHORT TEC_RED TEC_CROSS_CHAR TEC_RESET
                     " Test threw an unhandled std::exception: %s\n",
                     e.what());
            tec_process_test_result(TEC_FAIL, test);
        } catch (...) {
            tec_context.current_failed++;
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN,
                     TEC_PRE_SPACE_SHORT TEC_RED TEC_CROSS_CHAR TEC_RESET
                     " Test threw an unknown C++ exception.\n");
            tec_process_test_result(TEC_FAIL, test);
        }
#else
        tec_context.jump_set = true;
        int jump_val = setjmp(tec_context.jump_buffer);
        if (jump_val == TEC_INITIAL) {
            test->func();
        }
        tec_context.jump_set = false;
        tec_process_test_result((JUMP_CODES)jump_val, test);
#endif
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

    if (tec_context.stats.failed_tests > 0) {
        printf("\n" TEC_RED "Some tests failed!" TEC_RESET "\n");
        result = 1;
    } else if (tec_context.stats.ran_tests == 0) {
        printf("\n" TEC_YELLOW "Warning: No tests were run." TEC_RESET "\n");
        if (tec_context.stats.filtered_tests > 0) {
            printf(TEC_YELLOW TEC_ARROW_CHAR TEC_RESET
                   " All " TEC_CYAN "%zu" TEC_RESET
                   " tests were filtered out by the following criteria:\n",
                   tec_context.stats.filtered_tests);

            const char* prefix = tec_context.options.filter_by_filename
                                     ? TEC_PRE_SPACE_SHORT "--file -f"
                                     : TEC_PRE_SPACE_SHORT "-f";
            for (size_t i = 0; i < tec_context.options.filter_count; ++i) {
                printf(TEC_PRE_SPACE_SHORT "%s " TEC_MAGENTA "%s" TEC_RESET
                                           "\n",
                       prefix, tec_context.options.filters[i]);
            }
        }
        result = 1;
    } else if (tec_context.stats.skipped_tests > 0) {
        printf("\n" TEC_YELLOW "Tests passed, but some were skipped." TEC_RESET
               "\n");
        result = 0;
    } else {
        printf("\n" TEC_GREEN "All tests passed!" TEC_RESET "\n");
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

#ifdef __cplusplus
}
#endif
#endif  // TEC_IMPLEMENTATION
#endif  // TEC_H
