#include "tec.h"

#include <stdint.h>

#include "./ex_test_utility_fn/math_utils.h"

REGISTER_TEST(test_fibo) {
    int expected_fibo[10] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
    int fibo[10] = {0};
    ASSERT_ARRAY_EQUAL(expected_fibo, fibb(fibo), 10);
}

// NOTE: This test doesn't account for type differences between `expected` and
// `actual`.
REGISTER_TEST(test_multiple_array_types) {
    int expected_int_arr[10] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
    int actual_int_arr[10] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
    float expected_float_arr[3] = {1.1, 2.2, 3.3};
    float actual_float_arr[3] = {1.1, 2.2, 3.3};
    double expected_double_arr[3] = {1.1, 2.2, 3.3};
    double actual_double_arr[3] = {1.1, 2.2, 3.3};
    size_t expected_uint32_arr[3] = {0xff, 1, 0};
    size_t actual_uint32_arr[3] = {255, 1, 0.0f};
    ASSERT_ARRAY_EQUAL(expected_int_arr, actual_int_arr, 10);
    ASSERT_ARRAY_EQUAL(expected_float_arr, actual_float_arr, 3);
    ASSERT_ARRAY_EQUAL(expected_double_arr, actual_double_arr, 3);
    ASSERT_ARRAY_EQUAL(expected_uint32_arr, actual_uint32_arr, 3);
}

REGISTER_TEST(test_equal) {
    ASSERT_EQUAL(5, add(3, 2));
    ASSERT_NOT_EQUAL(3, add(1, 1));
    ASSERT_EQUAL(-2, add(-1, -1));
    ASSERT_EQUAL(NULL, NULL);
}

REGISTER_TEST(test_strings) {
    ASSERT_STR_NOT_EQUAL("yoo", "Yoo");
    ASSERT_STR_EQUAL("yoo", "yoo");
}

REGISTER_TEST(test_comparisions) {
    ASSERT_TRUE(add(3, 2) >= 5);
    ASSERT_FALSE(add(3, 2) != 5);
}

REGISTER_TEST(test_pointer) {
    int expected = 42069;
    int actual = 69;
    int correct_actual = 42069;
    ASSERT_PTR_VALUE_EQUAL(&expected, &actual, int);
    ASSERT_PTR_VALUE_EQUAL(&expected, &correct_actual, int);
}

// TODO:- make the assert equal work with floats as the float comparision is
// always going to be true. one can use double as of now.
int main() {
    // test_case_t tests[] = TEST_SUITE(
    //     TEST_FUNCTION(test_equal), TEST_FUNCTION(test_strings),
    //     TEST_FUNCTION(test_comparisions), TEST_FUNCTION(test_fibo),
    //     TEST_FUNCTION(test_multiple_array_types),
    //     TEST_FUNCTION(test_pointer));

    RUN_TESTS();

    return 0;
}
