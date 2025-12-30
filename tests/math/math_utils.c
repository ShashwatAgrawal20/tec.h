#include "../../include/math_utils.h"

#include "../../tec.h"

TEC_XFAIL(framework, teardown_with_cleanup) {
    // TEC_SKIP("Fails intentionally to demonstrate try/teardown.");
    int *value = NULL;

    // TRY BLOCK: Contains setup and assertions.
    // If an assert fails, execution jumps past this block.
    TEC_TRY_BLOCK({
        value = (int *)malloc(sizeof(int));
        TEC_ASSERT_NOT_NULL(value);
        *value = 69;
        TEC_ASSERT_EQ(*value, 420);
    });

    // FINALLY BLOCK: This code is guaranteed to run.
    // No keyword is needed; it's simply the code that follows the try block.
    free(value);
    // printf("Teardown complete: memory freed\n");
}

TEC(numerical, test_comparisons) {
    int a = 69;
    int b = 420;
    // TEC_ASSERT_GT(a, b); /* fails */
    // TEC_ASSERT_GE(69, 420); /* fails */
    TEC_ASSERT_GT(b, a);
    TEC_ASSERT_LT(-5, 0);
    TEC_ASSERT_GE(100, 100);
    TEC_ASSERT_LE(-99, -99);
}

TEC(float, precision) {
    TEC_ASSERT_NEAR(0.1 + 0.2, 0.3, 3e-15);
    TEC_ASSERT_FLOAT_EQ(0.1 + 0.2, 0.3);
}

TEC(mathutils, addition) {
    const int five = 5;
    TEC_ASSERT_EQ(add(2, 3), five);
    TEC_ASSERT_EQ(add(-10, 5), -5);

    TEC_ASSERT_NE(add(2, 2), 5);
}

TEC(mathutils, numerical_comparisons) {
    int five = multiply(5, 1);
    int ten = multiply(5, 2);

    TEC_ASSERT(ten > five);
    TEC_ASSERT(five < ten);
    TEC_ASSERT(ten >= ten);
    TEC_ASSERT(five <= five);
}

TEC(mathutils, multiply) {
    TEC_ASSERT_EQ(multiply(3, 4), 12);
    TEC_ASSERT(multiply(5, 2) % 2 == 0);
    TEC_ASSERT_NE(multiply(1, 1), 0);

    int result = multiply(-2, 5);
    TEC_ASSERT_EQ(result, -10);
    TEC_ASSERT(result < 0);
    TEC_ASSERT_NE(result, 0);

    result = multiply(0, 100);
    TEC_ASSERT_EQ(result, 0);
    TEC_ASSERT(result <= 0);
}

TEC(mathutils, division) {
    TEC_ASSERT_EQ(divide(10, 2), 5);
    TEC_ASSERT(divide(9, 3) > 0);

    int result = divide(15, 3);
    TEC_ASSERT_EQ(result, 5);

    result = divide(0, 5);
    TEC_ASSERT_EQ(result, 0);

    // Edge case: divide by zero
    result = divide(7, 0);
    TEC_ASSERT_EQ(result, 0);
}

TEC(mathutils, factorial) {
    TEC_ASSERT_EQ(factorial(0), 1); // Edge case: 0! = 1
    TEC_ASSERT_EQ(factorial(5), 120);
    TEC_ASSERT_EQ(factorial(10), 3628800);

    // Test for invalid input (negative numbers)
    TEC_ASSERT_EQ(factorial(-5), 1);
}

TEC(logic, booleans_act_right) {
    TEC_ASSERT(1);
    TEC_ASSERT(!0);
    TEC_ASSERT_EQ(1 == 1, (bool)1);
    TEC_ASSERT_NE(1 == 0, (bool)1);
}
