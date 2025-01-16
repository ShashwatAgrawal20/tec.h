# TEC - Test C / Test Engine for C
TEC (Test C or Test Engine for C) is a lightweight, header-only testing library
designed to make C testing straightforward and efficient. It provides a clean
interface for writing and running unit tests with minimal setup.

> [!IMPORTANT]
> TEC is currently in the early stages of development, and while it is somewhat
> functional, it may still have some limitations and is subject to change.

## QUICK EXAMPLE
The only file you need from here is
[tec.h](https://github.com/ShashwatAgrawal20/tec.h/blob/main/tec.h). Just
copy-paste it to your project and start using it.
```c
// tec.c
#include "tec.h"

#include <stdint.h>

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

// NOTE: This test doesn't account for type differences between `expected` and
// `actual`.
void test_multiple_array_types() {
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
                   TEST_FUNCTION(test_comparisions), TEST_FUNCTION(test_fibo),
                   TEST_FUNCTION(test_multiple_array_types));

    tec_test_run(tests);

    return 0;
}
```
```console
make
./tec
```

## EXAMPLE OUTPUT
![EXAMPLE OUTPUT](./example_output.png)
