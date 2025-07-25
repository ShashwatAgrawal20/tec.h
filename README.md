# tec.h

A tiny, header-only testing library for C.

Created because unit testing in C shouldn't require a complex setup, external dependencies, or build system gymnastics. This is a single header file that gets out of your way so you can just write tests.

> [!IMPORTANT]
> TEC is currently in the early stages of development, and while it is somewhat
> functional, it may still have some limitations and is subject to change.

***

## Quick Start

1.  Grab `tec.h` and drop it into your project.
2.  Create a test file (e.g., `my_test.c`).
3.  Write some tests.

```c
// my_test.c
#define TEC_IMPLEMENTATION
#include "tec.h"          // The testing library

TEC(test_addition) {
    TEC_ASSERT_EQ(2 + 2, 4);
    TEC_ASSERT_NE(2 + 2, 5);
}

TEC(test_strings) {
    const char* str = "hello";
    TEC_ASSERT_STR_EQ(str, "hello");
}

TEC_MAIN()
```

4. Compile and run it:

```bash
$gcc my_test.c -o test_runner && ./test_runner

================================
         C Test Runner
================================

my_test.c
  ✓ test_addition
  ✓ test_strings

================================
Tests: 2 total, 2 passed, 0 failed
Assertions: 3 total, 3 passed, 0 failed

All tests passed!
```
That's it. There is no step 5.

> [!CAUTION]
> TEC relies on the `__auto_type` GCC extension internally for type inference in assertions.
> This may be switched to `typeof` to improve compatibility with Clang.

---

## Features

- **Header-Only**: Just `#include "tec.h"`. No libraries to build or link.
- **No Dependencies**: Written in standard C.
- **Simple Assertions**: A core set of ASSERT macros that cover the essentials.
- **Automatic Test Registration**: The `TEC()` macro registers tests. `TEC_MAIN()` runs them.

---

## Assertion API

The library provides a straightforward set of assertions. On failure, it prints the file, line number, and the failed expression.

| Macro                      | Description                            | Example Usage                    |
|----------------------------|----------------------------------------|----------------------------------|
| `TEC_ASSERT(expression)`   | Asserts that `expression` is true.     | `TEC_ASSERT(x > 0);`             |
| `TEC_ASSERT_EQ(a, b)`      | Asserts that `a == b`.                 | `TEC_ASSERT_EQ(result, 42);`     |
| `TEC_ASSERT_NE(a, b)`      | Asserts that `a != b`.                 | `TEC_ASSERT_NE(a, b);`           |
| `TEC_ASSERT_STR_EQ(a, b)`  | Asserts that two strings are equal.    | `TEC_ASSERT_STR_EQ("foo", bar);` |
| `TEC_ASSERT_NULL(ptr)`     | Asserts that pointer is `NULL`.        | `TEC_ASSERT_NULL(ptr);`          |
| `TEC_ASSERT_NOT_NULL(ptr)` | Asserts that pointer is not `NULL`.    | `TEC_ASSERT_NOT_NULL(ptr);`      |

---

## Example Project & Makefile

For projects with multiple source files, a Makefile keeps things tidy. The included Makefile is designed to be "hands-off."

### Project Structure
```bash
my_project/
├── include/
│   └── calculator.h
├── src/
│   ├── calculator.c
│   └── main.c
├── tests/
│   ├── test_calculator.c
│   └── test_runner.c
├── Makefile
└── tec.h
```

> [!TIP]
> The build system automatically excludes your application's main()
> function when compiling tests, so you can have both `src/main.c` and
> `tests/test_runner.c` without conflicts.

### Test Runner Setup
```c
// tests/test_runner.c
#define TEC_IMPLEMENTATION
#include "../tec.h"
TEC_MAIN()
```

### Individual Test File

```c
// tests/test_calculator.c
#include "calculator.h"
#include "../tec.h"

TEC(test_add) {
    TEC_ASSERT_EQ(add(2, 3), 5);
    TEC_ASSERT_EQ(add(-1, 1), 0);
}

TEC(test_multiply) {
    TEC_ASSERT_EQ(multiply(3, 4), 12);
    TEC_ASSERT_EQ(multiply(0, 100), 0);
}
```

### Automated Testing

The provided Makefile automatically:
- Finds your `test` files
- Compiles the test binary
- Runs the tests

**Usage:**

```bash
$ make test
gcc -Wall -Wextra -pedantic -Iinclude -c tests/calculator.c -o build/tests/calculator.o
gcc -Wall -Wextra -pedantic -Iinclude -c tests/test_runner.c -o build/tests/test_runner.o
gcc -Wall -Wextra -pedantic -Iinclude -c src/calculator.c -o build/src/calculator.o
gcc -Wall -Wextra -pedantic -Iinclude -o test_runner build/tests/calculator.o build/tests/test_runner.o build/src/calculator.o
Running tests...
./test_runner
================================
         C Test Runner
================================

calculator.c
  ✓ test_add
  ✓ test_multiply

================================
Tests: 2 total, 2 passed, 0 failed
Assertions: 4 total, 4 passed, 0 failed

All tests passed!
```

> [!NOTE]
> By default, the number of tests is limited to 1024 (hardcoded).
> If you need to increase this limit, you can override it by defining `TEC_MAX_TESTS` before including the header:
>
> This approach may change in the future. Switching to dynamic allocation is being considered,
> as a fixed size may be too restrictive for larger test suites or unnecessarily large for smaller projects.
> ```c
> #define TEC_MAX_TEST UPDATED_NUMBER
> #include "../tec.h"
> ```
