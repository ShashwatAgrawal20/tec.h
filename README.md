# TEC - Test C / Test Engine for C
TEC (Test C or Test Engine for C) is a lightweight, header-only testing library
designed to make C testing straightforward and efficient. It provides a clean
interface for writing and running unit tests with minimal setup.

> [!IMPORTANT]
> TEC is currently in the early stages of development, and while it is somewhat
> functional, it may still have some limitations and is subject to change.

## QUICK EXAMPLE
The only file you need from here is
[nob.h](https://github.com/ShashwatAgrawal20/tec.h/blob/main/tec.h). Just
copy-paste it to your project and start using it.
```c
// tec.c
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
    tec_test_run(TEST_FUNCTIONS(test_equal, test_strings, test_comparisions));
    return 0;
}
```
```console
make
./tec
```
