#include "../../tec.h"
static char *shared_buffer = NULL;

// Runs once before all "buffer_tests"
TEC_SETUP(buffer_tests) {
    printf("  (Setting up suite...)\n");
    shared_buffer = (char *)malloc(100);
}

// Runs once after all "buffer_tests"
TEC_TEARDOWN(buffer_tests) {
    printf("  (Tearing down suite...)\n");
    free(shared_buffer);
    shared_buffer = NULL;
}

// Runs before EACH test in this suite
TEC_TEST_SETUP(buffer_tests) { strcpy(shared_buffer, "initial_state"); }

TEC(buffer_tests, test_one) {
    TEC_ASSERT_STR_EQ(shared_buffer, "initial_state");
    strcpy(shared_buffer, "modified_by_one");
    TEC_ASSERT_STR_EQ(shared_buffer, "modified_by_one");
}

TEC(buffer_tests, test_two) {
    // This test gets a fresh state because TEC_TEST_SETUP ran again
    TEC_ASSERT_STR_EQ(shared_buffer, "initial_state");
}
