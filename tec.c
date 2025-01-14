#include "tec.h"

int *fibb(int *fibo) {
    int i;
    fibo[0] = 0;
    fibo[1] = 1;
    for (i = 2; i <= 9; i++) {
        fibo[i] = fibo[i - 1] + fibo[i - 2];
    }
    return fibo;
}

int add(int a, int b) { return a + b; }

void test_fibo() {
    int expected_fibo[10] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
    int fibo[10] = {0};
    ASSERT_ARRAY_EQUAL(expected_fibo, fibb(fibo), 10);
}

void test_equal() {
    ASSERT_EQUAL(5, add(3, 2));
    ASSERT_NOT_EQUAL(3, add(1, 1));
    ASSERT_EQUAL(-2, add(-1, -1));
}

void test_strings() {
    ASSERT_STR_NOT_EQUAL("yoo", "Yoo");
    ASSERT_STR_EQUAL("yoo", "yoo");
}

void test_comparisions() {
    ASSERT_TRUE(add(3, 2) >= 5);
    ASSERT_FALSE(add(3, 2) != 5);
}

// TODO:- make the assert equal work with floats as the float comparision is
// always going to be true. one can use double as of now.
int main() {
    test_case_t tests[] =
        TEST_SUITE(TEST_FUNCTION(test_equal), TEST_FUNCTION(test_strings),
                   TEST_FUNCTION(test_comparisions), TEST_FUNCTION(test_fibo));

    tec_test_run(tests);

    // printf("%f", a);

    return 0;
}
