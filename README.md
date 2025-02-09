# TEC - Test C / Test Engine for C
TEC (Test C or Test Engine for C) is a lightweight, header-only testing library
designed to make C testing straightforward and efficient. It provides a clean
interface for writing and running unit tests with minimal setup.

> [!IMPORTANT]
> TEC is currently in the early stages of development, and while it is somewhat
> functional, it may still have some limitations and is subject to change.
> The primary focus of development is enabling support for multiple test files
> to replace the current single-file approach.

## QUICK EXAMPLE
1. Just copy-paste [tec.h](https://github.com/ShashwatAgrawal20/tec.h/blob/main/tec.h)
   to your project.
2. Include the header in your test file.
3. Write your tests.
5. Run your tests.

Here's a Quick Example:-
```c
// tec.c
#include "tec.h"

// Function to test(obv irl this is thing is gonna come from the header too)
int add(int a, int b) {
    return a + b;
}

// Test case
REGISTER_TEST(test_addition) {
    ASSERT_EQUAL(5, add(2, 3));
    ASSERT_TRUE(add(2, 2) == 4);
    ASSERT_FALSE(add(2, 2) == 5);
}

int main() {
    RUN_TESTS();
    return 0;
}
```

## Available Assertions
TEC provides several assertion macros for different testing needs:

### Basic Assertions
- `ASSERT_TRUE(condition)`: verifies that a condition is true.
- `ASSERT_FALSE(condition)`: verifies that a condition is false.

### Equality Assertions
- `ASSERT_EQUAL(expected, actual)`: compares two values for equality.
- `ASSERT_NOT_EQUAL(expected, actual)`: compares two values for inequality.

### String Assertions
- `ASSERT_STR_EQUAL(expected, actual)`: compares two strings for equality.
- `ASSERT_STR_NOT_EQUAL(expected, actual)`: compares two strings for inequality.

### Array Assertions
- `ASSERT_ARRAY_EQUAL(expected, actual, length)`: compares two arrays for equality.
- `ASSERT_PTR_VALUE_EQUAL(expected, actual, type)`: compares values behind pointer and also takes type.

## Advance Example

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
}

REGISTER_TEST(test_strings) {
    ASSERT_STR_NOT_EQUAL("yoo", "Yoo");
    ASSERT_STR_EQUAL("yoo", "yoo");
}

REGISTER_TEST(test_comparisions) {
    ASSERT_TRUE(add(3, 2) >= 5);
    ASSERT_FALSE(add(3, 2) != 5);
}

// TODO:- make the assert equal work with floats as the float comparision is
// always going to be true. one can use double as of now.
int main() {
    RUN_TESTS();

    return 0;
}
```

## Building and Running Tests
Compile your test file with your favorite C compiler:
```console
clang -Wall -Wextra tec.c -o tec
./tec
```

## EXAMPLE OUTPUT
![EXAMPLE OUTPUT](./example_output.png)
