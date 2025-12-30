#include "../../tec.h"

/*
 * Tests for macro-based auto-registration and symbol naming.
 * These tests validate observable behavior and generated function names
 * via __func__, not linker symbol tables.
 */
static int suite_setup_calls = 0;
static int suite_teardown_calls = 0;
static int test_setup_calls = 0;
static int test_teardown_calls = 0;

TEC_SETUP(registration) {
    suite_setup_calls++;
    TEC_ASSERT_STR_EQ(__func__, "tec_setup_registration");
}

TEC_TEARDOWN(registration) {
    suite_teardown_calls++;
    TEC_ASSERT_STR_EQ(__func__, "tec_teardown_registration");
}

TEC_TEST_SETUP(registration) {
    test_setup_calls++;
    TEC_ASSERT_STR_EQ(__func__, "tec_test_setup_registration");
}

TEC_TEST_TEARDOWN(registration) {
    test_teardown_calls++;
    TEC_ASSERT_STR_EQ(__func__, "tec_test_teardown_registration");
}

TEC(registration, t01_basic) {
    TEC_ASSERT_STR_EQ(__func__, "tec_registration_t01_basic");

    TEC_ASSERT_EQ(suite_setup_calls, 1);
    TEC_ASSERT_EQ(test_setup_calls, 1);
}

TEC(registration, t02_has_unique_name) {
    TEC_ASSERT_STR_EQ(__func__, "tec_registration_t02_has_unique_name");
    TEC_ASSERT_EQ(suite_setup_calls, 1);
    TEC_ASSERT_EQ(test_setup_calls, 2);
}

TEC_XFAIL(registration, t03_xfail_is_correct) {
    TEC_ASSERT_STR_EQ(__func__, "tec_registration_t03_xfail_is_correct");
    TEC_ASSERT(0);
}

TEC(registration, t90_registry_metadata) {
    size_t found = 0;
    for (size_t i = 0; i < tec_context.registry.tec_count; ++i) {
        tec_entry_t *e = &tec_context.registry.entries[i];
        if (strcmp(e->suite, "registration") == 0 &&
            strcmp(e->name, "t01_basic") == 0) {
            found++;
            TEC_ASSERT_FALSE(e->xfail);
            TEC_ASSERT_FUNC_NOT_NULL(e->func);
        }
        if (strcmp(e->suite, "registration") == 0 &&
            strcmp(e->name, "t03_xfail_is_correct") == 0) {
            found++;
            TEC_ASSERT_TRUE(e->xfail);
            TEC_ASSERT_FUNC_NOT_NULL(e->func);
        }
    }
    TEC_ASSERT_EQ(found, (size_t)2);
}

TEC(registration, t99_final_count) {
    TEC_ASSERT_EQ(suite_setup_calls, 1);
    TEC_ASSERT_EQ(suite_teardown_calls, 0);
    TEC_ASSERT_EQ(test_setup_calls, 5);
    TEC_ASSERT_EQ(test_teardown_calls, 4);
}
