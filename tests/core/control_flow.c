#include "../../tec.h"

TEC(control_flow, this_test_should_be_skipped) {
    TEC_SKIP("Testing the TEC_SKIP functionality.");
    // This assertion should never run. If it does, the test fails.
    TEC_ASSERT(false);
}

TEC_XFAIL(control_flow, this_xfail_should_fail_as_expected) {
    // This is the correct use of XFAIL.
    // The test fails, so the overall result for this test is a PASS.
    TEC_ASSERT_EQ(1, 0);
}

// NOTE: The following test is INTENTIONALLY broken.
// When you run the test suite, this test MUST be reported as FAILED.
// This is the meta-test: it proves that XFAIL correctly reports an
// unexpected success as a failure of the test suite.
TEC_XFAIL(control_flow,
          this_xfail_should_unexpectedly_pass_and_fail_the_suite) {
    // This assertion passes...
    TEC_ASSERT_EQ(1, 1);
    // ...which means the XFAIL contract is broken, and the runner MUST
    // report this specific test as a FAILURE.

    // USING THIS SO THE SUMMARY DOESN'T LOOK THAT UGLY
    // TEC_ASSERT_NE(1, 1);
}
