#include "tec.h"

int add(int a, int b) { return a + b; }

void test_equal() {
    ASSERT_EQUAL(5, add(3, 2));
    ASSERT_NOT_EQUAL(3, add(1, 1));
    ASSERT_EQUAL(-2, add(-1, -1));
}

void test_strings() {
    ASSERT_STR_EQUAL("yoo", "Yoo");
    ASSERT_STR_EQUAL("yoo", "yoo");
}

void test_comparisions() {
    ASSERT_TRUE(add(3, 2) >= 5);
    ASSERT_FALSE(add(3, 2) != 5);
}

// TODO:- make the assert equal work with floats as the float comparision is
// always going to be true. one can use double as of now.
int main() {
    void (*test_functions[])(void) =
        TEST_FUNCTIONS(test_equal, test_strings, test_comparisions);
    tec_test_run(test_functions,
                 (sizeof(test_functions) / sizeof(test_functions[0])));
    return 0;
}
