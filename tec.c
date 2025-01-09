#include "tec.h"

int add(int a, int b) { return a + b; }

void test_equal() {
    ASSERT_EQUAL(3, add(3, 2));
    ASSERT_EQUAL(2, add(1, 1));
    ASSERT_EQUAL(-2, add(-1, -1));
}

void test_strings() {
    ASSERT_STR_EQUAL("moron", "Moron");
    ASSERT_STR_EQUAL("moron", "moron");
}

void test_comparisions() {
    ASSERT_TRUE(add(3, 2) >= 5);
    ASSERT_FALSE(add(3, 2) == 5);
}

int main() {
    void (*test_functions[])(void) =
        TEST_FUNCTIONS(test_equal, test_strings, test_comparisions);
    tec_test_run(test_functions,
                 (sizeof(test_functions) / sizeof(test_functions[0])));
    return 0;
}
