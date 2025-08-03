# tec.h

A tiny, **header-only** testing library for C. Created because unit testing
shouldn't require complex setup or build system gymnastics, **tec.h** gets out of
your way so you can just write tests. Zero-setup unit testing is just one `#include` away.

> [!IMPORTANT]
> TEC is currently in the early stages of development, and while it is somewhat
> functional, it may still have some limitations and is subject to change.

## Table of Contents
- [In Action](#in-action)
- [Quick Start](#quick-start)
- [Features](#features)
- [Test Suites](#test-suites)
- [Assertion API](#assertion-api)
- [Advanced Usage](#advanced-usage)
- [Example Project & Makefile](#example-project--makefile)

***

## In Action
![TEC Test Runner Output](./example_output.png)

## Quick Start

1.  Grab `tec.h` and drop it into your project.
2.  Create a test file (e.g., `my_test.c`).
3.  Write some tests.

```c
// my_test.c
#define TEC_IMPLEMENTATION
#include "tec.h"          // The testing library

TEC(arithmetic, test_addition) {
    TEC_ASSERT_EQ(2 + 2, 4);
    TEC_ASSERT_NE(2 + 2, 5);
}

TEC(string_test, test_strings) {
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

SUITE: arithmetic (my_test.c)
  ✓ test_addition

SUITE: string_test (my_test.c)
  ✓ test_strings

================================
Tests:      2 passed, 0 failed, 0 skipped, 0 filtered (2 total)
Assertions: 3 passed, 0 failed (3 total)

All tests passed!
```
That's it. There is no step 5.

> [!CAUTION]
> TEC relies on GCC extensions like `__auto_type` and `__attribute__((constructor))`.
> This makes it highly compatible with GCC and Clang but may not work with other compilers like MSVC.

---

## Features

- **Header-Only**: Just `#include "tec.h"`. No libraries to build or link.
- **No Dependencies**: Written in standard C with common GCC/Clang extensions.
- **Simple Assertions**: A core set of ASSERT macros that cover the essentials.
- **Automatic Test Registration**: The `TEC()` macro registers tests. `TEC_MAIN()` runs them.
- **Dynamic Test Capacity**: The test registry grows as needed, so you don't have to worry about a predefined test limit.

---
## Test Suites

Tests are defined and grouped into suites using the `TEC(suite_name, test_name)` macro.

- `suite_name`: The name of the test group. All tests with the same suite_name will be grouped together in the output.
- `test_name`: The unique name of the individual test case.

```c
// All these tests will appear under the "math" suite in the output.
TEC(math, test_addition) { /* ... */ }
TEC(math, test_multiplication) { /* ... */ }

// This test will start a new "memory" suite.
TEC(memory, test_allocation) { /* ... */ }
```

---

## Assertion API

The library provides a straightforward set of assertions. On failure, it prints the file, line number, and the failed expression.

| Macro                      | Description                                | Example Usage                            |
|----------------------------|--------------------------------------------|------------------------------------------|
| `TEC_ASSERT(expression)`   | Asserts that `expression` is true.         | `TEC_ASSERT(x > 0);`                     |
| `TEC_ASSERT_EQ(a, b)`      | Asserts that `a == b`.                     | `TEC_ASSERT_EQ(result, 42);`             |
| `TEC_ASSERT_NE(a, b)`      | Asserts that `a != b`.                     | `TEC_ASSERT_NE(a, b);`                   |
| `TEC_ASSERT_STR_EQ(a, b)`  | Asserts that two strings are equal.        | `TEC_ASSERT_STR_EQ("foo", bar);`         |
| `TEC_ASSERT_NULL(ptr)`     | Asserts that pointer is `NULL`.            | `TEC_ASSERT_NULL(ptr);`                  |
| `TEC_ASSERT_NOT_NULL(ptr)` | Asserts that pointer is not `NULL`.        | `TEC_ASSERT_NOT_NULL(ptr);`              |
| `TEC_SKIP(reason)`         | Skips the current test and reports reason. | `TEC_SKIP("Not implemented yet.");`      |

---

## Advanced Usage

### Filtering Tests
You can selectively run tests by using `-f` or `--filter` followed by a substring pattern.
The filter is matched against the full test name in the format: `suite_name.test_name`

```bash
./test_runner -f math           # runs tests like math.add, math.subtract, etc.
./test_runner --filter alloc    # matches memory.alloc, buffer.alloc, etc.
./test_runner -f division -f io # runs tests matching "division" OR "io"
```
- You can provide multiple filters; tests matching any of them will be run.
- Filtering is **case-sensitive** and uses simple **substring** logic (not regex).
- If no filters are given, **all registered tests** will be executed.

### Skipping Tests
You can skip a test by placing `TEC_SKIP("reason")` at the beginning of its body.
This is useful for temporarily disabling tests for features that are not yet implemented.

```c
TEC(feature_x, test_new_functionality) {
    TEC_SKIP("This feature isn't ready for testing yet.");
    // This code will not be executed.
    TEC_ASSERT(false);
}
```

### Test Blocks with Cleanup
When an assertion fails, `tec.h` uses `longjmp` to immediately stop the test.
This can cause resource leaks if the test allocated memory or opened files.
To ensure cleanup code runs, you can wrap your assertions in a `TEC_TRY_BLOCK`.

If an assertion inside the block fails, execution will "jump" to the end of the
block, allowing the code that follows it to run. The test will still be marked as a failure.

```c
TEC(cleanup, test_with_cleanup) {
    char* buffer = malloc(100);
    TEC_ASSERT_NOT_NULL(buffer); // A failure here would skip the free() call.

    TEC_TRY_BLOCK {
        // Assertions inside this block are safe.
        strcpy(buffer, "hello");
        TEC_ASSERT_EQ(buffer[0], 'h');
        TEC_ASSERT_EQ(1, 0); // This will fail...
        TEC_ASSERT(false);   // ...and this line will never be reached.
    }

    // ...but the jump lands here, so cleanup can proceed.
    printf("Cleaning up resources...\n");
    free(buffer);
}
```

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

TEC(calculator, test_add) {
    TEC_ASSERT_EQ(add(2, 3), 5);
    TEC_ASSERT_EQ(add(-1, 1), 0);
}

TEC(calculator, test_multiply) {
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

SUITE: calculator (calculator.c)
  ✓ test_add
  ✓ test_multiply

================================
Tests:      2 passed, 0 failed, 0 skipped, 0 filtered (2 total)
Assertions: 4 passed, 0 failed (4 total)

All tests passed!
```
