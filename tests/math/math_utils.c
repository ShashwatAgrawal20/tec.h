#include "../include/math_utils.h"

#include "../tec.h"

TEC(test_add_positive_numbers) {
    int result = add(2, 3);
    TEC_ASSERT_EQ(result, 5);
    TEC_ASSERT(result > 0);
    TEC_ASSERT_NE(result, 0);

    result = add(10, 20);
    TEC_ASSERT_EQ(result, 30);
    TEC_ASSERT(result >= 30);
    TEC_ASSERT_NE(result, 0);
}

TEC(test_add_negative_numbers) {
    int result = add(-5, -3);
    TEC_ASSERT_EQ(result, -8);
    TEC_ASSERT(result < 0);
    TEC_ASSERT_NE(result++, 0);

    result = add(-10, 5);
    TEC_ASSERT_EQ(result, -5);
    TEC_ASSERT(result < 0);
    TEC_ASSERT_NE(result, 0);
}

TEC(test_multiply_basic) {
    int result = multiply(3, 4);
    TEC_ASSERT_EQ(result, 12);
    TEC_ASSERT(result % 2 == 0);
    TEC_ASSERT_NE(result, 0);

    result = multiply(-2, 5);
    TEC_ASSERT_EQ(result, -10);
    TEC_ASSERT(result < 0);
    TEC_ASSERT_NE(result, 0);

    result = multiply(0, 100);
    TEC_ASSERT_EQ(result, 0);
    TEC_ASSERT(result <= 0);
}

TEC(test_divide_basic) {
    int result = divide(10, 2);
    TEC_ASSERT_EQ(result, 5);
    TEC_ASSERT(result > 0);
    TEC_ASSERT_NE(result, 0);

    result = divide(15, 3);
    TEC_ASSERT_EQ(result, 5);
    TEC_ASSERT(result == 5);

    result = divide(7, 0);
    TEC_ASSERT_EQ(result, 0);
    TEC_ASSERT(result == 0);
}

TEC(test_factorial) {
    int result = factorial(0);
    TEC_ASSERT_EQ(result, 1);
    TEC_ASSERT_NE(result, 0);

    result = factorial(1);
    TEC_ASSERT_EQ(result, 1);
    TEC_ASSERT(result >= 1);

    result = factorial(5);
    TEC_ASSERT_NE(result, 0);
    TEC_ASSERT(result > 100);
    TEC_ASSERT_EQ(result, 120);

    result = factorial(4);
    TEC_ASSERT_EQ(result, 24);
    TEC_ASSERT(result % 4 == 0);
}
