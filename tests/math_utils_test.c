#include "../include/math_utils.h"

#include "../tec.h"

TEC(test_add_positive_numbers) {
    TEC_ASSERT_EQ(add(2, 3), 5);
    TEC_ASSERT_EQ(add(10, 20), 30);
}

TEC(test_add_negative_numbers) {
    TEC_ASSERT_EQ(add(-5, -3), -8);
    TEC_ASSERT_EQ(add(-10, 5), -5);
}

TEC(test_multiply_basic) {
    TEC_ASSERT_EQ(multiply(3, 4), 12);
    TEC_ASSERT_EQ(multiply(-2, 5), -10);
    TEC_ASSERT_EQ(multiply(0, 100), 0);
}

TEC(test_divide_basic) {
    TEC_ASSERT_EQ(divide(10, 2), 5);
    TEC_ASSERT_EQ(divide(15, 3), 5);
    TEC_ASSERT_EQ(divide(7, 0), 0.0);
}

TEC(test_factorial) {
    TEC_ASSERT_EQ(factorial(0), 1);
    TEC_ASSERT_EQ(factorial(1), 1);
    TEC_ASSERT_EQ(factorial(5), 120);
    TEC_ASSERT_EQ(factorial(4), 24);
}
