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
#include <inttypes.h>
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

#ifdef _WIN32
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/isatty
#include <io.h>
#include <windows.h>
#define isatty _isatty
#ifndef STDOUT_FILENO
#define STDOUT_FILENO _fileno(stdout)
#endif
#else
#include <time.h>
#include <unistd.h>
#endif

#ifdef __cplusplus
#define TEC_FUCK_MSVC_EH noexcept(false)
#else
#define TEC_FUCK_MSVC_EH
#endif

#ifdef __cplusplus
extern "C" {
#endif
#define TEC_RED (tec_context.options.no_color ? "" : "\033[31m")
#define TEC_GREEN (tec_context.options.no_color ? "" : "\033[32m")
#define TEC_YELLOW (tec_context.options.no_color ? "" : "\033[33m")
#define TEC_BLUE (tec_context.options.no_color ? "" : "\033[34m")
#define TEC_MAGENTA (tec_context.options.no_color ? "" : "\033[35m")
#define TEC_CYAN (tec_context.options.no_color ? "" : "\033[36m")
#define TEC_GRAY (tec_context.options.no_color ? "" : "\033[90m")
#define TEC_RESET (tec_context.options.no_color ? "" : "\033[0m")

#define TEC_PRE_SPACE "    "
#define TEC_PRE_SPACE_SHORT "  "

#define TEC_MAX_FAILURE_MESSAGE_LEN 1024
#define TEC_TMP_STRBUF_LEN 256
#define TEC_FMT_SLOTS 2
#define TEC_FMT_SLOT_SIZE TEC_TMP_STRBUF_LEN
#define TEC_PREFIX_SIZE 64

#define _TEC_FABS(x) ((x) < 0.0 ? -(x) : (x))

#if defined(UINTPTR_MAX) && UINTPTR_MAX == UINT64_MAX
#define TEC_64BIT_SYSTEM_SIZE_T_CONFLICT_UINT64
#endif

typedef enum { TEC_INITIAL, TEC_FAIL, TEC_SKIP_e } JUMP_CODES;

typedef enum {
    TEC_SUITE_SETUP,
    TEC_SUITE_TEARDOWN,
    TEC_TEST_SETUP,
    TEC_TEST_TEARDOWN
} tec_fixture_type;

typedef void (*tec_func_t)(void);
typedef void (*tec_fixture_func_t)(void);

typedef struct {
    const char *suite;
    const char *name;
    const char *file;
    tec_func_t func;
    bool xfail;
} tec_entry_t;

typedef struct {
    const char *name;
    tec_fixture_func_t setup;
    tec_fixture_func_t teardown;
    tec_fixture_func_t test_setup;
    tec_fixture_func_t test_teardown;
} tec_suite_t;

typedef struct {
    jmp_buf jump_buffer;
    char failure_message[TEC_MAX_FAILURE_MESSAGE_LEN];
    char format_bufs[TEC_FMT_SLOTS][TEC_FMT_SLOT_SIZE];
    struct {
        size_t ran_tests;
        size_t passed_tests;
        size_t failed_tests;
        size_t xfailed_tests;
        size_t xpassed_tests;
        size_t skipped_tests;
        size_t filtered_tests;
        size_t total_assertions;
        size_t passed_assertions;
        size_t failed_assertions;
    } stats;
    struct {
        tec_entry_t *entries;
        tec_suite_t *suites;
        size_t tec_count;
        size_t tec_capacity;
        size_t suite_count;
        size_t suite_capacity;
    } registry;
    struct {
        char **filters;
        size_t filter_count;
        bool filter_by_filename;
        bool fail_fast;
        bool no_color;
        bool use_ascii;
    } options;
    size_t current_passed;
    size_t current_failed;
    bool jump_set;
} tec_context_t;

void tec_register(const char *suite, const char *name, const char *file,
                  tec_func_t func, bool xfail);
void tec_register_fixture(const char *suite_name, tec_fixture_func_t func,
                          tec_fixture_type fixture_type);

void _tec_post_wrapper(bool is_fail_case);
void TEC_POST_FAIL(void) TEC_FUCK_MSVC_EH;
void _tec_skip_impl(const char *reason, int line) TEC_FUCK_MSVC_EH;

extern tec_context_t tec_context;
extern char tec_fail_prefix[TEC_PREFIX_SIZE];
extern char tec_pass_prefix[TEC_PREFIX_SIZE];
extern char tec_skip_prefix[TEC_PREFIX_SIZE];
extern char tec_line_prefix[TEC_PREFIX_SIZE];

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
#define TEC_AUTO_TYPE auto
#else
#define TEC_AUTO_TYPE __auto_type
#endif

#ifdef __cplusplus
class tec_assertion_failure : public std::runtime_error {
  public:
    tec_assertion_failure(const char *msg) : std::runtime_error(msg) {}
};
class tec_skip_test : public std::runtime_error {
  public:
    tec_skip_test(const char *msg) : std::runtime_error(msg) {}
};
#endif

#ifdef __cplusplus
/* FUCK STRINGSTREAM.
 * We know the "right" way to do this is with std::format from C++20.
 * BUT, that would force every poor bastard using this library to add
 * `-std=c++20`, since most compilers still default to older standards.
 * So, for maximum drop-in compatibility, we're stuck with this slow-ass thing.
 * It's a shit trade-off, but it makes the library easier to use.
 */
template <typename T> std::string tec_to_string(const T &value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}
inline std::string tec_to_string(const char *value) {
    if (value == NULL)
        return "(null)";
    return "\"" + std::string(value) + "\"";
}
inline std::string tec_to_string(char *value) {
    return tec_to_string(const_cast<const char *>(value));
}
#define TEC_FMT(x, buf)                                                        \
    snprintf((buf), TEC_FMT_SLOT_SIZE, "%s", tec_to_string(x).c_str())

#define TEC_TRY_BLOCK(code)                                                    \
    try {                                                                      \
        code                                                                   \
    } catch (const tec_assertion_failure &) {                                  \
    } catch (const tec_skip_test &) {                                          \
    }
#else
#define TEC_FORMAT_VALUE_PAIR(x) TEC_FORMAT_SPEC(x), TEC_FORMAT_VALUE(x)

#define TEC_FMT(x, buf)                                                        \
    snprintf((buf), TEC_TMP_STRBUF_LEN, TEC_FORMAT_VALUE_PAIR(x))

/*
 * #define TEC_TRY_BLOCK                                                       \
 *     for (int _tec_loop_once = (tec_context.jump_set = true, 1);             \
 *          _tec_loop_once && setjmp(tec_context.jump_buffer) == 0;            \
 *          _tec_loop_once = 0, tec_context.jump_set = false)
 */
#define TEC_TRY_BLOCK(code)                                                    \
    do {                                                                       \
        tec_context.jump_set = true;                                           \
        if (setjmp(tec_context.jump_buffer) == 0) {                            \
            code                                                               \
        }                                                                      \
        tec_context.jump_set = false;                                          \
    } while (0)

/*
 * don't fuck with this.
 * keep TEC_FORMAT_SPEC and TEC_FORMAT_VALUE split to avoid -Wformat issues
 * I tried snprintf-style macro but, it caused bogus format warnings on the LSP
 * side, splitting format and value avoids LSP noise and keeps type safety.
 * default case now uses (const void *)&x to bypass int-to-pointer-size
 * warnings.
 */
#ifdef TEC_64BIT_SYSTEM_SIZE_T_CONFLICT_UINT64
#define TEC_FORMAT_SPEC(x)                                                     \
    _Generic((x),                                                              \
        int8_t: "%" PRId8,                                                     \
        int16_t: "%" PRId16,                                                   \
        int32_t: "%" PRId32,                                                   \
        int64_t: "%" PRId64, /* fuck this. lp64 vs llp64; portable C my ass */ \
        uint8_t: "%" PRIu8,                                                    \
        uint16_t: "%" PRIu16,                                                  \
        uint32_t: "%" PRIu32,                                                  \
        size_t: "%zu", /* fuck windows, fuck mingw/msvc-crt, fuck me */        \
        float: "%f",                                                           \
        double: "%lf",                                                         \
        long double: "%Lf",                                                    \
        char *: "%s",                                                          \
        const char *: "%s",                                                    \
        default: "%p")

#define TEC_FORMAT_VALUE(x)                                                    \
    _Generic((x),                                                              \
        int8_t: (x),                                                           \
        int16_t: (x),                                                          \
        int32_t: (x),                                                          \
        int64_t: (x),                                                          \
        uint8_t: (x),                                                          \
        uint16_t: (x),                                                         \
        uint32_t: (x),                                                         \
        size_t: (x),                                                           \
        float: (x),                                                            \
        double: (x),                                                           \
        long double: (x),                                                      \
        char *: (x),                                                           \
        const char *: (x),                                                     \
        default: (const void *)&(x)) // avoids int-to-pointer warning
#else
#define TEC_FORMAT_SPEC(x)                                                     \
    _Generic((x),                                                              \
        int8_t: "%" PRId8,                                                     \
        int16_t: "%" PRId16,                                                   \
        int32_t: "%" PRId32,                                                   \
        int64_t: "%" PRId64, /* fuck this. lp64 vs llp64; portable C my ass */ \
        uint8_t: "%" PRIu8,                                                    \
        uint16_t: "%" PRIu16,                                                  \
        uint32_t: "%" PRIu32,                                                  \
        uint64_t: "%" PRIu64, /* 32 bit systems */                             \
        size_t: "%zu",        /* fuck windows, fuck mingw/msvc-crt, fuck me */ \
        float: "%f",                                                           \
        double: "%lf",                                                         \
        long double: "%Lf",                                                    \
        char *: "%s",                                                          \
        const char *: "%s",                                                    \
        default: "%p")

#define TEC_FORMAT_VALUE(x)                                                    \
    _Generic((x),                                                              \
        int8_t: (x),                                                           \
        int16_t: (x),                                                          \
        int32_t: (x),                                                          \
        int64_t: (x),                                                          \
        uint8_t: (x),                                                          \
        uint16_t: (x),                                                         \
        uint32_t: (x),                                                         \
        uint64_t: (x), /* 32 bit systems */                                    \
        size_t: (x),                                                           \
        float: (x),                                                            \
        double: (x),                                                           \
        long double: (x),                                                      \
        char *: (x),                                                           \
        const char *: (x),                                                     \
        default: (const void *)&(x)) // avoids int-to-pointer warning
#endif
#endif

#define TEC_POST_PASS()                                                        \
    do {                                                                       \
        tec_context.current_passed++;                                          \
        tec_context.stats.passed_assertions++;                                 \
    } while (0);

#define TEC_SKIP(reason) _tec_skip_impl(reason, __LINE__)

#define TEC_ASSERT(condition)                                                  \
    do {                                                                       \
        tec_context.stats.total_assertions++;                                  \
        TEC_AUTO_TYPE _tec_cond_result = (condition);                          \
        if (!(_tec_cond_result)) {                                             \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     TEC_PRE_SPACE "%sAssertion failed: %s (line %d)\n",       \
                     tec_fail_prefix, #condition, __LINE__);                   \
            TEC_POST_FAIL();                                                   \
        } else {                                                               \
            TEC_POST_PASS();                                                   \
        }                                                                      \
    } while (0)

#define TEC_ASSERT_TRUE(condition)                                             \
    do {                                                                       \
        tec_context.stats.total_assertions++;                                  \
        TEC_AUTO_TYPE _cond = (condition);                                     \
        if (!(_cond)) {                                                        \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     TEC_PRE_SPACE "%sExpected %s to be true (line %d)\n",     \
                     tec_fail_prefix, #condition, __LINE__);                   \
            TEC_POST_FAIL();                                                   \
        } else {                                                               \
            TEC_POST_PASS();                                                   \
        }                                                                      \
    } while (0)

#define TEC_ASSERT_FALSE(condition)                                            \
    do {                                                                       \
        tec_context.stats.total_assertions++;                                  \
        TEC_AUTO_TYPE _cond = (condition);                                     \
        if ((_cond)) {                                                         \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     TEC_PRE_SPACE "%sExpected %s to be false (line %d)\n",    \
                     tec_fail_prefix, #condition, __LINE__);                   \
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
                     TEC_PRE_SPACE                                             \
                     "%sExpected %s == %s, got %s != %s (line %d)\n",          \
                     tec_fail_prefix, #a, #b, tec_context.format_bufs[0],      \
                     tec_context.format_bufs[1], __LINE__);                    \
            TEC_POST_FAIL();                                                   \
        } else {                                                               \
            TEC_POST_PASS();                                                   \
        }                                                                      \
    } while (0)

#define TEC_ASSERT_NE(a, b)                                                    \
    do {                                                                       \
        tec_context.stats.total_assertions++;                                  \
        TEC_AUTO_TYPE _a = a;                                                  \
        TEC_AUTO_TYPE _b = b;                                                  \
        if ((_a) == (_b)) {                                                    \
            TEC_FMT(_a, tec_context.format_bufs[0]);                           \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     TEC_PRE_SPACE                                             \
                     "%sExpected %s != %s, but both are %s (line %d)\n",       \
                     tec_fail_prefix, #a, #b, tec_context.format_bufs[0],      \
                     __LINE__);                                                \
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
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     TEC_PRE_SPACE                                             \
                     "%sNearness assertion failed (line %d)\n" TEC_PRE_SPACE   \
                     "%sExpected: %s and %s to be within %g\n" TEC_PRE_SPACE   \
                     "%sActual:   they differ by %g\n",                        \
                     tec_fail_prefix, __LINE__, tec_line_prefix, #a, #b,       \
                     (double)_tol, tec_line_prefix, _diff);                    \
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
            snprintf(                                                          \
                tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN,      \
                TEC_PRE_SPACE                                                  \
                "%sFloating point equality failed (line %d)\n" TEC_PRE_SPACE   \
                "%sExpected: %s == %s\n" TEC_PRE_SPACE                         \
                "%sActual:   %s (%g)\n" TEC_PRE_SPACE                          \
                "%s      and: %s (%g)\n" TEC_PRE_SPACE                         \
                "%sDifference: %g ( > tolerance %g)\n",                        \
                tec_fail_prefix, __LINE__, tec_line_prefix, #a, #b,            \
                tec_line_prefix, #a, (double)_a, tec_line_prefix, #b,          \
                (double)_b, tec_line_prefix, _diff, _default_tol);             \
            TEC_POST_FAIL();                                                   \
        } else {                                                               \
            TEC_POST_PASS();                                                   \
        }                                                                      \
    } while (0)

#define TEC_ASSERT_STR_EQ(a, b)                                                \
    do {                                                                       \
        tec_context.stats.total_assertions++;                                  \
        const char *_a = (a);                                                  \
        const char *_b = (b);                                                  \
        int equal =                                                            \
            ((_a == NULL && _b == NULL) || (_a && _b && strcmp(_a, _b) == 0)); \
        if (!equal) {                                                          \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     TEC_PRE_SPACE                                             \
                     "%sExpected strings equal: \"%s\" != \"%s\" (line %d)\n", \
                     tec_fail_prefix, (_a ? _a : "(null)"),                    \
                     (_b ? _b : "(null)"), __LINE__);                          \
            TEC_POST_FAIL();                                                   \
        } else {                                                               \
            TEC_POST_PASS();                                                   \
        }                                                                      \
    } while (0)

#define TEC_ASSERT_NULL(ptr)                                                   \
    do {                                                                       \
        tec_context.stats.total_assertions++;                                  \
        const void *_ptr = ptr;                                                \
        if ((_ptr) != NULL) {                                                  \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     TEC_PRE_SPACE                                             \
                     "%sExpected %s to be NULL, got %p (line %d)\n",           \
                     tec_fail_prefix, #ptr, (const void *)(_ptr), __LINE__);   \
            TEC_POST_FAIL();                                                   \
        } else {                                                               \
            TEC_POST_PASS();                                                   \
        }                                                                      \
    } while (0)

#define TEC_ASSERT_NOT_NULL(ptr)                                               \
    do {                                                                       \
        tec_context.stats.total_assertions++;                                  \
        const void *_ptr = ptr;                                                \
        if ((_ptr) == NULL) {                                                  \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     TEC_PRE_SPACE "%sExpected %s to not be NULL (line %d)\n", \
                     tec_fail_prefix, #ptr, __LINE__);                         \
            TEC_POST_FAIL();                                                   \
        } else {                                                               \
            TEC_POST_PASS();                                                   \
        }                                                                      \
    } while (0)

#define TEC_ASSERT_FUNC_NOT_NULL(fn)                                           \
    do {                                                                       \
        tec_context.stats.total_assertions++;                                  \
        if ((fn) == NULL) {                                                    \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     TEC_PRE_SPACE                                             \
                     "%sExpected function %s to not be NULL (line %d)\n",      \
                     tec_fail_prefix, #fn, __LINE__);                          \
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
            const char *_op_str = #op;                                         \
            const char *_inv_op_str =                                          \
                (_op_str[0] == '>')   ? ((_op_str[1] == '=') ? "<" : "<=")     \
                : (_op_str[0] == '<') ? ((_op_str[1] == '=') ? ">" : ">=")     \
                                      : "???";                                 \
            snprintf(                                                          \
                tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN,      \
                TEC_PRE_SPACE "%sExpected %s %s %s, got %s %s %s (line %d)\n", \
                tec_fail_prefix, #a, _op_str, #b, tec_context.format_bufs[0],  \
                _inv_op_str, tec_context.format_bufs[1], __LINE__);            \
            TEC_POST_FAIL();                                                   \
        } else {                                                               \
            TEC_POST_PASS();                                                   \
        }                                                                      \
    } while (0)

#ifdef __cplusplus
#define TEC_ASSERT_THROWS(statement, exception_type)                           \
    do {                                                                       \
        tec_context.stats.total_assertions++;                                  \
        try {                                                                  \
            statement;                                                         \
            snprintf(                                                          \
                tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN,      \
                TEC_PRE_SPACE                                                  \
                "%sExpected statement `%s` to throw `%s`, but it did not "     \
                "(line %d).\n",                                                \
                tec_fail_prefix, #statement, #exception_type, __LINE__);       \
            TEC_POST_FAIL();                                                   \
        } catch (const tec_assertion_failure &) {                              \
            throw;                                                             \
        } catch (const exception_type &) {                                     \
            TEC_POST_PASS();                                                   \
        } catch (const std::exception &e) {                                    \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     TEC_PRE_SPACE                                             \
                     "%sExpected `%s` to throw `%s`, but it threw a "          \
                     "std::exception with what(): %s (line %d)\n",             \
                     tec_fail_prefix, #statement, #exception_type, e.what(),   \
                     __LINE__);                                                \
            TEC_POST_FAIL();                                                   \
        } catch (...) {                                                        \
            snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN, \
                     TEC_PRE_SPACE                                             \
                     "%sExpected `%s` to throw `%s`, but it threw an unknown " \
                     "exception (line %d).\n",                                 \
                     tec_fail_prefix, #statement, #exception_type, __LINE__);  \
            TEC_POST_FAIL();                                                   \
        }                                                                      \
    } while (0)
#endif

#ifdef __cplusplus
struct tec_auto_register {
    tec_auto_register(const char *suite, const char *name, const char *file,
                      tec_func_t func, bool xfail) {
        tec_register(suite, name, file, func, xfail);
    }
};
struct tec_auto_register_fixture {
    tec_auto_register_fixture(const char *suite_name, tec_fixture_func_t func,
                              tec_fixture_type fixture_type) {
        tec_register_fixture(suite_name, func, fixture_type);
    }
};

#define TEC(suite_name, test_name)                                             \
    static void tec_##suite_name##_##test_name(void);                          \
    static tec_auto_register tec_register_##suite_name##_##test_name(          \
        #suite_name, #test_name, __FILE__, tec_##suite_name##_##test_name,     \
        false);                                                                \
    static void tec_##suite_name##_##test_name(void)

#define TEC_XFAIL(suite_name, test_name)                                       \
    static void tec_##suite_name##_##test_name(void);                          \
    static tec_auto_register tec_register_##suite_name##_##test_name(          \
        #suite_name, #test_name, __FILE__, tec_##suite_name##_##test_name,     \
        true);                                                                 \
    static void tec_##suite_name##_##test_name(void)

#define _TEC_FIXTURE_FACTORY(suite_name, fixture_type_token,                   \
                             fixture_type_enum)                                \
    static void tec_##fixture_type_token##_##suite_name(void);                 \
    static tec_auto_register_fixture                                           \
        tec_register_##fixture_type_token##_##suite_name(                      \
            #suite_name, tec_##fixture_type_token##_##suite_name,              \
            fixture_type_enum);                                                \
    static void tec_##fixture_type_token##_##suite_name(void)
#else
#define TEC(suite_name, test_name)                                             \
    static void tec_##suite_name##_##test_name(void);                          \
    static void __attribute__((constructor))                                   \
    tec_register_##suite_name##_##test_name(void) {                            \
        tec_register(#suite_name, #test_name, __FILE__,                        \
                     tec_##suite_name##_##test_name, false);                   \
    }                                                                          \
    static void tec_##suite_name##_##test_name(void)

#define TEC_XFAIL(suite_name, test_name)                                       \
    static void tec_##suite_name##_##test_name(void);                          \
    static void __attribute__((constructor))                                   \
    tec_register_##suite_name##_##test_name(void) {                            \
        tec_register(#suite_name, #test_name, __FILE__,                        \
                     tec_##suite_name##_##test_name, true);                    \
    }                                                                          \
    static void tec_##suite_name##_##test_name(void)

#define _TEC_FIXTURE_FACTORY(suite_name, fixture_type_token,                   \
                             fixture_type_enum)                                \
    static void tec_##fixture_type_token##_##suite_name(void);                 \
    static void __attribute__((constructor))                                   \
    tec_register_##fixture_type_token##_##suite_name(void) {                   \
        tec_register_fixture(#suite_name,                                      \
                             tec_##fixture_type_token##_##suite_name,          \
                             fixture_type_enum);                               \
    }                                                                          \
    static void tec_##fixture_type_token##_##suite_name(void)
#endif

#define TEC_SETUP(suite_name)                                                  \
    _TEC_FIXTURE_FACTORY(suite_name, setup, TEC_SUITE_SETUP)
#define TEC_TEARDOWN(suite_name)                                               \
    _TEC_FIXTURE_FACTORY(suite_name, teardown, TEC_SUITE_TEARDOWN)
#define TEC_TEST_SETUP(suite_name)                                             \
    _TEC_FIXTURE_FACTORY(suite_name, test_setup, TEC_TEST_SETUP)
#define TEC_TEST_TEARDOWN(suite_name)                                          \
    _TEC_FIXTURE_FACTORY(suite_name, test_teardown, TEC_TEST_TEARDOWN)

#ifdef TEC_IMPLEMENTATION
#ifdef __cplusplus
extern "C" {
#endif

tec_context_t tec_context;

char tec_fail_prefix[TEC_PREFIX_SIZE];
char tec_pass_prefix[TEC_PREFIX_SIZE];
char tec_skip_prefix[TEC_PREFIX_SIZE];
char tec_line_prefix[TEC_PREFIX_SIZE];

double tec_get_time(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

void tec_format_time(double seconds, char *buf, size_t buf_size) {
    if (seconds >= 1.0) {
        snprintf(buf, buf_size, "%.3f s", seconds);
    } else if (seconds >= 1e-3) {
        snprintf(buf, buf_size, "%.3f ms", seconds * 1e3);
    } else if (seconds >= 1e-6) {
        snprintf(buf, buf_size, "%.3f us", seconds * 1e6);
    } else {
        snprintf(buf, buf_size, "%.3f ns", seconds * 1e9);
    }
}

void tec_init_prefixes(void) {
    const char *pass;
    const char *fail;
    const char *skip;
    const char *line;
#ifdef _WIN32
    // stick to good old ascii cause windows support for utf-8 is pain in the
    // ass.
    bool default_ascii = true;
#else
    bool default_ascii = false;
#endif
    bool use_ascii = tec_context.options.use_ascii || default_ascii;
    if (use_ascii) {
        pass = "[ OK ]";
        fail = "[FAIL]";
        skip = "[SKIP]";
        line = "   |  ";
    } else {
        pass = "✓";
        fail = "✗";
        skip = "»";
        line = "│";
    }

    if (tec_context.options.no_color) {
        snprintf(tec_fail_prefix, TEC_PREFIX_SIZE, "%s ", fail);
        snprintf(tec_pass_prefix, TEC_PREFIX_SIZE, "%s ", pass);
        snprintf(tec_skip_prefix, TEC_PREFIX_SIZE, "%s ", skip);
        snprintf(tec_line_prefix, TEC_PREFIX_SIZE, "%s ", line);
    } else {
        snprintf(tec_fail_prefix, TEC_PREFIX_SIZE, "\033[31m%s\033[0m ", fail);
        snprintf(tec_pass_prefix, TEC_PREFIX_SIZE, "\033[32m%s\033[0m ", pass);
        snprintf(tec_skip_prefix, TEC_PREFIX_SIZE, "\033[33m%s\033[0m ", skip);
        snprintf(tec_line_prefix, TEC_PREFIX_SIZE, "\033[33m%s\033[0m ", line);
    }
}

void _tec_detect_color_support(void) {
    bool want_color;
    if (getenv("NO_COLOR") != NULL) {
        want_color = false;
    } else if (getenv("FORCE_COLOR") != NULL) {
        want_color = true;
    } else {
        want_color = isatty(STDOUT_FILENO);
        if (want_color) {
#ifdef _WIN32
            // https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#example-of-sgr-terminal-sequences
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hOut == INVALID_HANDLE_VALUE) {
                want_color = false;
            } else {
                DWORD dwMode = 0;
                if (!GetConsoleMode(hOut, &dwMode)) {
                    want_color = false;
                } else {
                    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                    if (!SetConsoleMode(hOut, dwMode)) {
                        want_color = false;
                    }
                }
            }
#else
            const char *term = getenv("TERM");
            if (!term || !*term || strcmp(term, "dumb") == 0) {
                want_color = false;
            }
#endif
        }
    }
    tec_context.options.no_color = !want_color;
    tec_context.options.use_ascii = !want_color;
}

void TEC_POST_FAIL(void) TEC_FUCK_MSVC_EH {
    tec_context.current_failed++;
    tec_context.stats.failed_assertions++;
#ifdef __cplusplus
    throw tec_assertion_failure(tec_context.failure_message);
#else
    if (tec_context.jump_set)
        longjmp(tec_context.jump_buffer, TEC_FAIL);
#endif
}

void _tec_skip_impl(const char *reason, int line) TEC_FUCK_MSVC_EH {
    const char *_reason = (reason);
    snprintf(tec_context.failure_message, TEC_MAX_FAILURE_MESSAGE_LEN,
             TEC_PRE_SPACE "%sSkipped: %s (line %d)\n", tec_skip_prefix,
             _reason, line);
#ifdef __cplusplus
    throw tec_skip_test(tec_context.failure_message);
#else
    if (tec_context.jump_set)
        longjmp(tec_context.jump_buffer, TEC_SKIP_e);
#endif
}

int tec_compare_entries(const void *a, const void *b) {
    tec_entry_t *entry_a = (tec_entry_t *)a;
    tec_entry_t *entry_b = (tec_entry_t *)b;
    int suite_cmp = strcmp(entry_a->suite, entry_b->suite);
    if (suite_cmp != 0) {
        return suite_cmp;
    }
    return strcmp(entry_a->name, entry_b->name);
}

void tec_register(const char *suite, const char *name, const char *file,
                  tec_func_t func, bool xfail) {
    if (!suite || !name || !file || !func) {
        fprintf(stderr, "%sError: NULL argument to tec_register%s\n", TEC_RED,
                TEC_RESET);
        return;
    }

    if (tec_context.registry.tec_count >= tec_context.registry.tec_capacity) {
        tec_context.registry.tec_capacity =
            tec_context.registry.tec_capacity == 0
                ? 8
                : tec_context.registry.tec_capacity * 2;
        tec_entry_t *new_registry = (tec_entry_t *)realloc(
            tec_context.registry.entries,
            tec_context.registry.tec_capacity * sizeof(tec_entry_t));

        if (new_registry == NULL) {
            fprintf(stderr,
                    "%sError: Failed to allocate memory for test "
                    "registry%s\n",
                    TEC_RED, TEC_RESET);
            free(tec_context.registry.entries);
            free(tec_context.registry.suites);
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

void tec_register_fixture(const char *suite_name, tec_fixture_func_t func,
                          tec_fixture_type fixture_type) {
    tec_suite_t *suite = NULL;
    for (size_t i = 0; i < tec_context.registry.suite_count; ++i) {
        if (strcmp(tec_context.registry.suites[i].name, suite_name) == 0) {
            suite = &tec_context.registry.suites[i];
            break;
        }
    }
    if (suite == NULL) {
        if (tec_context.registry.suite_count >=
            tec_context.registry.suite_capacity) {
            tec_context.registry.suite_capacity =
                tec_context.registry.suite_capacity == 0
                    ? 4
                    : tec_context.registry.suite_capacity * 2;
            tec_suite_t *new_suites = (tec_suite_t *)realloc(
                tec_context.registry.suites,
                tec_context.registry.suite_capacity * sizeof(tec_suite_t));
            if (!new_suites) {
                fprintf(stderr,
                        "%sError: Failed to allocate memory for suite "
                        "registry%s\n",
                        TEC_RED, TEC_RESET);
                free(tec_context.registry.entries);
                free(tec_context.registry.suites);
                exit(1);
            }
            tec_context.registry.suites = new_suites;
        }
        suite =
            &tec_context.registry.suites[tec_context.registry.suite_count++];
        memset(suite, 0, sizeof(tec_suite_t));
        suite->name = suite_name;
    }

    switch (fixture_type) {
    case TEC_SUITE_SETUP:
        suite->setup = func;
        break;
    case TEC_SUITE_TEARDOWN:
        suite->teardown = func;
        break;
    case TEC_TEST_SETUP:
        suite->test_setup = func;
        break;
    case TEC_TEST_TEARDOWN:
        suite->test_teardown = func;
        break;
    }
}

tec_suite_t *tec_find_suite(const char *name) {
    if (!name)
        return NULL;
    for (size_t i = 0; i < tec_context.registry.suite_count; ++i) {
        if (strcmp(tec_context.registry.suites[i].name, name) == 0) {
            return &tec_context.registry.suites[i];
        }
    }
    return NULL;
}

void tec_process_test_result(JUMP_CODES jump_val, const tec_entry_t *test,
                             double elapsed) {
    char time_buf[32];
    tec_format_time(elapsed, time_buf, sizeof(time_buf));

    bool has_failed = (jump_val == TEC_FAIL || tec_context.current_failed > 0);
    if (jump_val == TEC_SKIP_e) {
        tec_context.stats.skipped_tests++;
        printf(TEC_PRE_SPACE_SHORT "%s%s %s(%s)%s\n", tec_skip_prefix,
               test->name, TEC_GRAY, time_buf, TEC_RESET);
        printf("%s", tec_context.failure_message);
        return;
    }
    if (test->xfail) {
        if (has_failed) {
            tec_context.stats.xfailed_tests++;
            printf(TEC_PRE_SPACE_SHORT "%s%s (expected failure) %s(%s)%s\n",
                   tec_pass_prefix, test->name, TEC_GRAY, time_buf, TEC_RESET);
        } else {
            tec_context.stats.xpassed_tests++;
            printf(TEC_PRE_SPACE_SHORT "%s%s (unexpected success) %s(%s)%s\n",
                   tec_fail_prefix, test->name, TEC_GRAY, time_buf, TEC_RESET);
        }
    } else {
        if (has_failed) {
            tec_context.stats.failed_tests++;
            printf(TEC_PRE_SPACE_SHORT
                   "%s%s - %zu assertion(s) failed %s(%s)%s\n",
                   tec_fail_prefix, test->name, tec_context.current_failed,
                   TEC_GRAY, time_buf, TEC_RESET);
            printf("%s", tec_context.failure_message);
        } else {
            tec_context.stats.passed_tests++;
            printf(TEC_PRE_SPACE_SHORT "%s%s %s(%s)%s\n", tec_pass_prefix,
                   test->name, TEC_GRAY, time_buf, TEC_RESET);
        }
    }
}

void tec_print_usage(const char *prog_name) {
    printf("Usage: %s [options]\n", prog_name);
    printf("\nOptions:\n");

    printf(
        "  -f, --filter <pattern>  Run tests whose name contains <pattern>.\n"
        "                          Matches against 'suite.test' by default.\n"
        "                          Prefix with '!' to exclude matches.\n");

    printf(
        "  --file                  Apply filters to the test filename instead\n"
        "                          of the test name.\n");

    printf(
        "  --fail-fast             Stop execution after the first failure or\n"
        "                          unexpected success (XPASS).\n");

    printf("  --no-color              Disable colored output.\n");
    printf("  --ascii                 Use ASCII symbols instead of Unicode.\n");

    printf("  -h, --help              Display this help message.\n");

    printf("\nExamples:\n");

    printf("  %s -f math\n"
           "      Run all tests whose suite or name contains 'math'.\n",
           prog_name);

    printf("  %s -f math -f '!division'\n"
           "      Run all math tests except those containing 'division'.\n",
           prog_name);

    printf("  %s --file -f math_utils.c\n"
           "      Run all tests defined in files matching 'math_utils.c'.\n",
           prog_name);

    printf("  %s --fail-fast\n"
           "      Stop as soon as a test fails or unexpectedly passes.\n",
           prog_name);
}

int tec_parse_args(int argc, char **argv) {
    if (argc < 2) {
        return 0;
    }
    tec_context.options.filters = (char **)calloc(argc, sizeof(char *));
    if (tec_context.options.filters == NULL) {
        fprintf(stderr, "%sFailed to allocate memory for filters%s\n", TEC_RED,
                TEC_RESET);
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        if ((strcmp(argv[i], "-f") == 0) ||
            (strcmp(argv[i], "--filter") == 0)) {
            if (argc > ++i) {
                tec_context.options
                    .filters[tec_context.options.filter_count++] = argv[i];
            } else {
                fprintf(stderr,
                        "%sError: Filter option requires an "
                        "argument.%s\n",
                        TEC_RED, TEC_RESET);
                return 1;
            }
        } else if (strcmp(argv[i], "--file") == 0) {
            tec_context.options.filter_by_filename = true;
        } else if (strcmp(argv[i], "--fail-fast") == 0) {
            tec_context.options.fail_fast = true;
        } else if (strcmp(argv[i], "--no-color") == 0) {
            tec_context.options.no_color = true;
        } else if (strcmp(argv[i], "--ascii") == 0) {
            tec_context.options.use_ascii = true;
        } else if (strcmp(argv[i], "-h") == 0 ||
                   strcmp(argv[i], "--help") == 0) {
            tec_print_usage(argv[0]);
            return 1;
        } else {
            fprintf(stderr, "%sError: Unknown option '%s'%s\n", TEC_RED,
                    TEC_RESET, argv[i]);
            tec_print_usage(argv[0]);
            return 1;
        }
    }
    return 0;
}

bool tec_should_run(const tec_entry_t *test) {
    char full_name_buffer[TEC_TMP_STRBUF_LEN];
    const char *target_string;

    if (tec_context.options.filter_by_filename) {
        target_string = test->file;
    } else {
        snprintf(full_name_buffer, sizeof(full_name_buffer), "%s.%s",
                 test->suite, test->name);
        target_string = full_name_buffer;
    }

    /* IMPORTANT:
     * Exclusion filters are vetoes and MUST be evaluated before any inclusion
     * decision is made. Never early-return on an inclusion match, doing so
     * allows a later exclusion to be silently ignored depending on filter
     * order. Accepting a test is a conclusion; excluding a test is a hard stop.
     *
     * Historical bug (now fixed):
     *   ./tests/test_runner -f math -f '!division'
     *
     * Target:
     *   mathutils.division
     *
     * A previous implementation returned true as soon as the "math" inclusion
     * matched, skipping evaluation of the "!division" exclusion and incorrectly
     * running the test.
     *
     * All exclusion filters must be checked before deciding to run a test.
     */
    bool has_inclusion_filters = false;
    bool inclusion_matched = false;
    for (size_t i = 0; i < tec_context.options.filter_count; ++i) {
        const char *f = tec_context.options.filters[i];
        if (f[0] == '!' && f[1] != '\0') {
            if (strstr(target_string, f + 1) != NULL) {
                return false;
            }
            continue;
        }
        has_inclusion_filters = true;
        if (strstr(target_string, f) != NULL) {
            inclusion_matched = true;
        }
    }

    // If inclusion filters exist, require a match.
    // Otherwise (only exclusions), allow.
    return has_inclusion_filters ? inclusion_matched : true;
}

bool _fixture_exec_helper(tec_fixture_func_t func, const char *token) {
    bool has_failed = false;
    bool should_print = token == NULL ? false : true;
#ifdef __cplusplus
    try {
        func();
    } catch (...) {
        if (should_print) {
            printf(TEC_PRE_SPACE_SHORT "%s%s Failed!\n", tec_fail_prefix,
                   token);
            printf("%s", tec_context.failure_message);
        }
        has_failed = true;
    }
#else
    tec_context.jump_set = true;
    if (setjmp(tec_context.jump_buffer) == TEC_INITIAL) {
        func();
    } else {
        if (should_print) {
            printf(TEC_PRE_SPACE_SHORT "%s%s Failed!\n", tec_fail_prefix,
                   token);
            printf("%s", tec_context.failure_message);
        }
        has_failed = true;
    }
    tec_context.jump_set = false;
#endif
    return has_failed;
}

int tec_run_all(int argc, char **argv) {
    int result = 0;
    double suite_start = 0.0;
    const char *current_suite = NULL;
    const tec_suite_t *current_suite_ptr = NULL;
    bool suite_setup_failed = false;
    bool test_setup_failed = false;
    bool has_printed_test_setup_failure = false;

    _tec_detect_color_support(); /* This should stay above `tec_parse_args` */
    result = tec_parse_args(argc, argv);
    if (result)
        goto cleanup;
    tec_init_prefixes();

    double total_start = tec_get_time();

    printf("%s================================\n", TEC_BLUE);
    printf("         C Test Runner          \n");
    printf("================================%s\n", TEC_RESET);

    qsort(tec_context.registry.entries, tec_context.registry.tec_count,
          sizeof(tec_entry_t), tec_compare_entries);

    for (size_t i = 0; i < tec_context.registry.tec_count; ++i) {
        tec_entry_t *test = &tec_context.registry.entries[i];

        if (tec_context.options.filter_count != 0 && !tec_should_run(test)) {
            tec_context.stats.filtered_tests++;
            continue;
        }

        if (current_suite == NULL || strcmp(current_suite, test->suite) != 0) {
            if (current_suite != NULL) {
                if (current_suite_ptr && current_suite_ptr->teardown &&
                    !suite_setup_failed) {
                    _fixture_exec_helper(current_suite_ptr->teardown,
                                         "Suite Teardown");
                }
                double suite_elapsed = tec_get_time() - suite_start;
                char suite_time_buf[32];
                tec_format_time(suite_elapsed, suite_time_buf,
                                sizeof(suite_time_buf));
                printf("%s  Suite total: %s%s\n", TEC_GRAY, suite_time_buf,
                       TEC_RESET);
            }
            current_suite = test->suite;
            suite_start = tec_get_time();
            const char *display_name = strstr(test->file, "tests/");
            if (display_name == NULL) {
                display_name = strstr(test->file, "tests\\");
            }
            if (display_name) {
                display_name = display_name + 6;
            } else {
                const char *f_slash = strrchr(test->file, '/');
                const char *b_slash = strrchr(test->file, '\\');
                const char *last_slash =
                    (f_slash > b_slash) ? f_slash : b_slash;

                display_name = last_slash ? last_slash + 1 : test->file;
            }

            printf("%s\nSUITE: %s%s (%s)\n", TEC_MAGENTA, current_suite,
                   TEC_RESET, display_name);

            current_suite_ptr = tec_find_suite(current_suite);
            suite_setup_failed = false;
            test_setup_failed = false;
            has_printed_test_setup_failure = false;
            if (current_suite_ptr && current_suite_ptr->setup) {
                suite_setup_failed = _fixture_exec_helper(
                    current_suite_ptr->setup, "Suite Setup");
            }
        }

        if (suite_setup_failed) {
            tec_context.stats.skipped_tests++;
            printf(TEC_PRE_SPACE_SHORT "%s%s (skipped due to setup failure)\n",
                   tec_skip_prefix, test->name);
            continue;
        }

        tec_context.current_passed = 0;
        tec_context.current_failed = 0;
        tec_context.failure_message[0] = '\0';
        test_setup_failed = false;

        if (current_suite_ptr && current_suite_ptr->test_setup) {
            test_setup_failed =
                _fixture_exec_helper(current_suite_ptr->test_setup, NULL);
        }
        if (test_setup_failed) {
            tec_context.stats.skipped_tests++;
            if (!has_printed_test_setup_failure) {
                printf(TEC_PRE_SPACE_SHORT "%sTest Setup Failed!\n",
                       tec_fail_prefix);
                printf("%s", tec_context.failure_message);
                has_printed_test_setup_failure = true;
            }
            printf(TEC_PRE_SPACE_SHORT
                   "%s%s (skipped due to test setup failure)\n",
                   tec_skip_prefix, test->name);
        } else {
            tec_context.stats.ran_tests++;
            double test_start = tec_get_time();
#ifdef __cplusplus
            try {
                test->func();
                double test_elapsed = tec_get_time() - test_start;
                tec_process_test_result(TEC_INITIAL, test, test_elapsed);
            } catch (const tec_assertion_failure &) {
                double test_elapsed = tec_get_time() - test_start;
                tec_process_test_result(TEC_FAIL, test, test_elapsed);
            } catch (const tec_skip_test &) {
                double test_elapsed = tec_get_time() - test_start;
                tec_process_test_result(TEC_SKIP_e, test, test_elapsed);
            } catch (const std::exception &e) {
                tec_context.current_failed++;
                snprintf(tec_context.failure_message,
                         TEC_MAX_FAILURE_MESSAGE_LEN,
                         TEC_PRE_SPACE_SHORT
                         "%sTest threw an unhandled std::exception: %s\n",
                         tec_fail_prefix, e.what());
                double test_elapsed = tec_get_time() - test_start;
                tec_process_test_result(TEC_FAIL, test, test_elapsed);
            } catch (...) {
                tec_context.current_failed++;
                snprintf(tec_context.failure_message,
                         TEC_MAX_FAILURE_MESSAGE_LEN,
                         TEC_PRE_SPACE_SHORT
                         "%sTest threw an unknown C++ exception.\n",
                         tec_fail_prefix);
                double test_elapsed = tec_get_time() - test_start;
                tec_process_test_result(TEC_FAIL, test, test_elapsed);
            }
#else
            tec_context.jump_set = true;
            int jump_val = setjmp(tec_context.jump_buffer);
            if (jump_val == TEC_INITIAL) {
                test->func();
            }
            tec_context.jump_set = false;
            double test_elapsed = tec_get_time() - test_start;
            tec_process_test_result((JUMP_CODES)jump_val, test, test_elapsed);
#endif
            if (current_suite_ptr && current_suite_ptr->test_teardown) {
                _fixture_exec_helper(current_suite_ptr->test_teardown,
                                     "Test Teardown");
            }
        }
        if (tec_context.options.fail_fast &&
            ((!test->xfail && tec_context.current_failed > 0) ||
             tec_context.stats.xpassed_tests > 0)) {
            break;
        }
    }

    if (current_suite_ptr && current_suite_ptr->teardown &&
        !suite_setup_failed) {
        _fixture_exec_helper(current_suite_ptr->teardown, "Suite Teardown");
    }
    double suite_elapsed = tec_get_time() - suite_start;
    char suite_time_buf[32];
    tec_format_time(suite_elapsed, suite_time_buf, sizeof(suite_time_buf));
    printf("%s  Suite total: %s%s\n", TEC_GRAY, suite_time_buf, TEC_RESET);

    double total_elapsed = tec_get_time() - total_start;
    char total_time_buf[32];
    tec_format_time(total_elapsed, total_time_buf, sizeof(total_time_buf));

    printf("\n%s================================%s\n", TEC_BLUE, TEC_RESET);
    printf("Tests:      "
           "%s%zu passed%s",
           TEC_GREEN,
           tec_context.stats.passed_tests + tec_context.stats.xfailed_tests,
           TEC_RESET);
    if (tec_context.stats.xfailed_tests > 0) {
        printf(" (%s%zuP%s, %s%zuXF%s)", TEC_GREEN,
               tec_context.stats.passed_tests, TEC_RESET, TEC_MAGENTA,
               tec_context.stats.xfailed_tests, TEC_RESET);
    }
    printf(", %s%zu failed%s", TEC_RED,
           tec_context.stats.failed_tests + tec_context.stats.xpassed_tests,
           TEC_RESET);
    if (tec_context.stats.xpassed_tests > 0) {
        printf(" (%s%zuF%s, %s%zuXP%s)", TEC_RED,
               tec_context.stats.failed_tests, TEC_RESET, TEC_MAGENTA,
               tec_context.stats.xpassed_tests, TEC_RESET);
    }
    if (tec_context.stats.skipped_tests > 0) {
        printf(", %s%zu skipped%s", TEC_YELLOW, tec_context.stats.skipped_tests,
               TEC_RESET);
    }
    if (tec_context.stats.filtered_tests > 0) {
        printf(", %s%zu filtered%s", TEC_CYAN, tec_context.stats.filtered_tests,
               TEC_RESET);
    }
    printf(" (%zu total)\n", tec_context.registry.tec_count);

    printf("Assertions: %s%zu passed%s, %s%zu failed%s (%zu total)\n",
           TEC_GREEN, tec_context.stats.passed_assertions, TEC_RESET, TEC_RED,
           tec_context.stats.failed_assertions, TEC_RESET,
           tec_context.stats.total_assertions);
    printf("Time:       %s%s%s\n", TEC_CYAN, total_time_buf, TEC_RESET);

    if (tec_context.stats.failed_tests > 0 ||
        tec_context.stats.xpassed_tests > 0) {
        printf("\n%sSome tests failed!%s\n", TEC_RED, TEC_RESET);
        result = 1;
    } else if (tec_context.stats.ran_tests == 0) {
        printf("\n%sWarning: No tests were run.%s\n", TEC_YELLOW, TEC_RESET);
        if (tec_context.stats.filtered_tests > 0) {
            printf("%sAll %s%zu%s tests were filtered out by the following "
                   "criteria:\n",
                   tec_skip_prefix, TEC_CYAN, tec_context.stats.filtered_tests,
                   TEC_RESET);

            const char *prefix = tec_context.options.filter_by_filename
                                     ? TEC_PRE_SPACE_SHORT "--file -f"
                                     : TEC_PRE_SPACE_SHORT "-f";
            for (size_t i = 0; i < tec_context.options.filter_count; ++i) {
                printf(TEC_PRE_SPACE_SHORT "%s %s%s%s\n", prefix, TEC_MAGENTA,
                       tec_context.options.filters[i], TEC_RESET);
            }
        }
        result = 1;
    } else if (tec_context.stats.skipped_tests > 0) {
        printf("\n%sTests passed, but some were skipped.%s\n", TEC_YELLOW,
               TEC_RESET);
        result = 0;
    } else {
        printf("\n%sAll tests passed!%s\n", TEC_GREEN, TEC_RESET);
        result = 0;
    }

cleanup:
    free(tec_context.registry.entries);
    free(tec_context.registry.suites);
    free(tec_context.options.filters);
    memset(&tec_context, 0, sizeof(tec_context_t));
    return result;
}

#define TEC_MAIN()                                                             \
    int main(int argc, char **argv) { return tec_run_all(argc, argv); }

#ifdef __cplusplus
}
#endif
#endif // TEC_IMPLEMENTATION
#endif // TEC_H
