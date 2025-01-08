#include "tec.h"

int add(int a, int b) { return a + b; }

void singular_test() {
    ASSERT_EQUAL(3, add(3, 2));
    ASSERT_EQUAL(2, add(1, 1));
    ASSERT_EQUAL(-2, add(-1, -1));
    ASSERT_TRUE(add(3, 2) > 5);
    ASSERT_FALSE(add(3, 2) == 5);
    ASSERT_STR_EQUAL("moron", "Moron");
    ASSERT_STR_EQUAL("moron", "moron");
}

// TODO:- we need to have some sort of array of function pointers in the struct
// itself and have a another utility function called `run_tests()` which will
// iterate over those and execute them OR maybe we can just rename the
// `init_test()` function and have the test cases run inside that only that way
// we'll only have to define the array of functions and seperately call the
// `run_tests()`?
int main() {
    init_test(singular_test);

    return 0;
}
