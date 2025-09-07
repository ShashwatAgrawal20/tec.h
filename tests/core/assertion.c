#include "../../tec.h"

TEC(assertions, test_assert_success) {
    TEC_ASSERT(1 == 1);
    TEC_ASSERT(true);
}

TEC_XFAIL(assertions, test_assert_failure) { TEC_ASSERT(1 == 0); }

TEC(assertions, test_equality_success) {
    TEC_ASSERT_EQ(42, 42);
    TEC_ASSERT_NE(42, 99);
}

TEC_XFAIL(assertions, test_equality_failure_eq) { TEC_ASSERT_EQ(42, 99); }
TEC_XFAIL(assertions, test_equality_failure_ne) { TEC_ASSERT_NE(42, 42); }

TEC(assertions, test_comparison_success) {
    TEC_ASSERT_GT(10, 5);
    TEC_ASSERT_GE(10, 10);
    TEC_ASSERT_LT(5, 10);
    TEC_ASSERT_LE(5, 5);
}

TEC_XFAIL(assertions, test_comparison_failure_gt) { TEC_ASSERT_GT(5, 10); }
TEC_XFAIL(assertions, test_comparison_failure_le) { TEC_ASSERT_LE(10, 5); }

TEC(assertions, test_string_and_pointer_success) {
    const char *str = "hello";
    const char *null_str = NULL;
    TEC_ASSERT_STR_EQ("hello", str);
    TEC_ASSERT_NOT_NULL(str);
    TEC_ASSERT_NULL(null_str);
}

TEC_XFAIL(assertions, test_string_failure) {
    TEC_ASSERT_STR_EQ("hello", "world");
}

TEC_XFAIL(assertions, test_pointer_failure) {
    const char *str = "hello";
    TEC_ASSERT_NULL(str);
}

TEC(assertions, test_float_near_success) {
    TEC_ASSERT_FLOAT_EQ(0.1 + 0.2, 0.3);
    TEC_ASSERT_NEAR(3.1415, 3.141, 0.01);
}

TEC_XFAIL(assertions, test_float_near_failure) {
    TEC_ASSERT_NEAR(3.1415, 3.2, 0.01);
}
