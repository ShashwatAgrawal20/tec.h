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
#include "my_functions.h" // Your code to test
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
$gcc my_test.c my_functions.c -o test_runner && ./test_runner

================================
         C Test Runner
================================

bs.c
  ✓ test_addition
  ✓ test_strings

================================
Tests: 2 total, 2 passed, 0 failed
Assertions: 3 total, 3 passed, 0 failed

All tests passed!
```

That's it. There is no step 5.

---

## Features

- **Header-Only**: Just `#include "tec.h"`. No libraries to build or link.
- **No Dependencies**: Written in standard C.
- **Simple Assertions**: A core set of ASSERT macros that cover the essentials.
- **Automatic Test Registration**: The `TEC()` macro registers tests. `TEC_MAIN()` runs them.

---

## Assertion API

The library provides a straightforward set of assertions. On failure, it prints the file, line number, and the failed expression.

- `TEC_ASSERT(expression)` – Asserts that `expression` is true.
- `TEC_ASSERT_EQ(a, b)` – Asserts that `a == b`.
- `TEC_ASSERT_NE(a, b)` – Asserts that `a != b`.
- `TEC_ASSERT_STR_EQ(a, b)` – Asserts that two strings are equal.
- `TEC_ASSERT_STR_NE(a, b)` – Asserts that two strings are not equal.

---

## Example Project & Makefile

For projects with multiple source files, a Makefile keeps things tidy. The included Makefile is designed to be "hands-off."

### Project Structure
```
.
├── include/
│   └── my_lib.h
├── src/
│   └── my_lib.c
├── tests/
│   └── my_lib_test.c
├── Makefile
└── tec.h
```

### Automated Testing

The provided Makefile automatically:
- Finds your `*_test.c` files
- Generates `test_runner.c`
- Compiles the test binary
- Runs the tests
- Cleans up afterward

**Usage:**

```bash
$ make test
Generating test runner...
gcc -Wall -Wextra -Iinclude -o tests/test_runner tests/test_runner.c build/my_lib.o
./tests/test_runner
...
...
```
