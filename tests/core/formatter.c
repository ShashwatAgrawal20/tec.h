#include "../../tec.h"

TEC(formatter_macros, test_signed_integers) {
    int8_t i8 = INT8_MIN;
    int16_t i16 = INT16_MIN;
    int32_t i32 = INT32_MIN;
    int64_t i64 = INT64_MIN;

    TEC_ASSERT_EQ(i8, (int8_t)-128);
    TEC_ASSERT_EQ(i16, (int16_t)-32768);
    TEC_ASSERT_EQ(i32, (int32_t)(-2147483647 - 1));
    TEC_ASSERT_EQ(i64, (int64_t)(-9223372036854775807LL - 1LL));
}

TEC(formatter_macros, test_unsigned_integers) {
    uint8_t u8 = UINT8_MAX;
    uint16_t u16 = UINT16_MAX;
    uint32_t u32 = UINT32_MAX;

    TEC_ASSERT_EQ(u8, (uint8_t)255);
    TEC_ASSERT_EQ(u16, (uint16_t)65535);
    TEC_ASSERT_EQ(u32, (uint32_t)4294967295);
}

TEC(formatter_macros, test_large_unsigned_types) {
    size_t s_t = (size_t)-1;
    TEC_ASSERT_EQ(s_t, (size_t)-1);

// The uint64_t type is only distinct from size_t on 32-bit systems.
// On 64-bit systems, the _Generic macro omits this case, and a uint64_t
// value will be correctly handled by the size_t case. This test ensures
// it works on both architectures.
#ifndef TEC_64BIT_SYSTEM_SIZE_T_CONFLICT_UINT64
    uint64_t u64 = UINT64_MAX;
    TEC_ASSERT_EQ(u64, (uint64_t)18446744073709551615ULL);
#endif
}

TEC(formatter_macros, test_floating_point_types) {
    float f = 3.14f;
    double d = 2.71828;
    long double ld = 1.61803398875L;

    TEC_ASSERT_FLOAT_EQ(f, 3.14f);
    TEC_ASSERT_FLOAT_EQ(d, 2.71828);
    TEC_ASSERT_FLOAT_EQ(ld, 1.61803398875L);
}

TEC(formatter_macros, test_pointer_types) {
    const char *cstr = "hello";
    char buffer[] = "world";
    char *pstr = buffer;
    int x = 42;
    void *vptr = &x;

    TEC_ASSERT_STR_EQ(cstr, "hello");
    TEC_ASSERT_STR_EQ(pstr, "world");
    TEC_ASSERT_EQ(vptr, (void *)&x);
}

TEC_XFAIL(formatter_macros, verify_uint64_formatting_on_failure) {
    uint64_t u64 = 18446744073709551615ULL; // UINT64_MAX
    TEC_ASSERT_EQ(u64, (uint64_t)0);
}

TEC_XFAIL(formatter_macros, verify_default_case_formatting) {
    struct SomeRandomStruct {
        int a;
    } my_struct = {1};
    TEC_ASSERT_EQ(my_struct.a, 2);
}
