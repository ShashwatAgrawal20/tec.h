#define TEC_IMPLEMENTATION
#include "tec.h"
#include <iostream>
#include <memory>
#include <vector>

TEC(cpp_features, Vector) {
    std::vector<int> my_vector(100); // THIS WILL AUTOFREE CAUSE OF RAII(dest)

    // Dynamically allocated int (manual memory management).
    // We must remember to delete it to avoid a memory leak.
    int *xxx = new int;
    *xxx = 6969;
    try {
        TEC_ASSERT_EQ(my_vector.size(), (size_t)100);
        TEC_ASSERT_EQ(*xxx, 69); // FAILING CASE
    } catch (...) {
        std::cout << "    womp womp!" << std::endl;
        delete xxx;
        // Rethrow the exception so that:
        //   1) The test framework knows this block failed.
        //   2) Execution stops here and other assertions won't run
        //      after this failure (cause that's the intended behavior).
        throw;
    }

    // This will only run if no exception was thrown earlier.
    TEC_ASSERT(1 == 1);
}

TEC(cpp_features, SmartPointer) {
    std::unique_ptr<int[]> my_array(new int[100]);
    TEC_ASSERT(my_array != nullptr);
}

void function_that_throws() {
    throw std::runtime_error("This is an expected exception.");
}
void function_that_doesnt_throw() {}

TEC(cpp_features, catches_correct_exception_type) {
    TEC_ASSERT_THROWS(function_that_throws(), std::runtime_error);
}
TEC_XFAIL(cpp_features, fails_on_wrong_exception_type) {
    TEC_ASSERT_THROWS(function_that_throws(), std::invalid_argument);
}
TEC_XFAIL(cpp_features, fails_when_no_exception_is_thrown) {
    TEC_ASSERT_THROWS(function_that_doesnt_throw(), std::runtime_error);
}

static int raii_sentry_counter = 0;
struct RaiiSentry {
    RaiiSentry() { raii_sentry_counter++; }
    ~RaiiSentry() { raii_sentry_counter--; }
};

TEC(cpp_features, assertion_failure_is_raii_safe) {
    TEC_ASSERT_EQ(raii_sentry_counter, 0);
    try {
        RaiiSentry s;
        TEC_ASSERT_EQ(raii_sentry_counter, 1);
        TEC_ASSERT_EQ(1, 0); // This assertion throws
    } catch (...) {
        // THIS WILL PASS as the destructor would've restored the value so
        // this assertion will be logged in the passed one's
        TEC_ASSERT_EQ(raii_sentry_counter, 0);
        // Exception was caught. If the destructor ran, the counter is 0.
        // Rethrow the exception so that:
        //   1) The test framework knows this block failed.
        //   2) Execution stops here and other assertions won't run
        //      after this failure (cause that's the intended behavior).
        throw;
    }

    // THIS won't be logged cause we used throw in catch, this would've
    // logged if we forgot that throw;
    TEC_ASSERT_EQ(raii_sentry_counter, 0);
}

TEC_MAIN();
