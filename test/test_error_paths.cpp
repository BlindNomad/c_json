#include <gtest/gtest.h>

#include <climits>
#include <cstdint>
#include <cstring>

extern "C" {
#include "c_json.h"
}

#include "helpers.hpp"

TEST(ErrorPathTest, NullHandlesAndTagsOnAdd) {
    C_JSON j = c_json_new();
    ASSERT_NE(j, nullptr);
    C_JSON child = c_json_new();
    ASSERT_NE(child, nullptr);

    EXPECT_FALSE(c_json_add_boolean(nullptr, "b", true));
    EXPECT_FALSE(c_json_add_boolean(j, nullptr, true));
    EXPECT_FALSE(c_json_add_string(nullptr, "s", "x"));
    EXPECT_FALSE(c_json_add_string(j, nullptr, "x"));
    EXPECT_FALSE(c_json_add_object(nullptr, "o", child));
    EXPECT_FALSE(c_json_add_object(j, nullptr, child));
    EXPECT_FALSE(c_json_add_number(nullptr, "n", 1.0));
    EXPECT_FALSE(c_json_add_number(j, nullptr, 1.0));

    EXPECT_FALSE(c_json_add_array_boolean(nullptr, "ab", true));
    EXPECT_FALSE(c_json_add_array_boolean(j, nullptr, true));
    EXPECT_FALSE(c_json_add_array_string(nullptr, "as", "x"));
    EXPECT_FALSE(c_json_add_array_string(j, nullptr, "x"));
    EXPECT_FALSE(c_json_add_array_object(nullptr, "ao", 0, child));
    EXPECT_FALSE(c_json_add_array_object(j, nullptr, 0, child));
    EXPECT_FALSE(c_json_add_array_number(nullptr, "an", 1.0));
    EXPECT_FALSE(c_json_add_array_number(j, nullptr, 1.0));

    c_json_free(child);
    c_json_free(j);
}

TEST(ErrorPathTest, ConfigUpdateAndErrorOnTypedAdds) {
    C_JSON j = c_json_new();
    ASSERT_NE(j, nullptr);

    EXPECT_TRUE(c_json_set_config(j, C_JSON_CONFIG_UPDATE_SAME_TAG));
    EXPECT_TRUE(c_json_add_boolean(j, "b", true));
    EXPECT_TRUE(c_json_add_boolean(j, "b", false));
    EXPECT_TRUE(c_json_add_string(j, "s", "a"));
    EXPECT_TRUE(c_json_add_string(j, "s", "b"));

    C_JSON o1 = c_json_new();
    C_JSON o2 = c_json_new();
    c_json_add_int(o1, "v", 1);
    c_json_add_int(o2, "v", 2);
    EXPECT_TRUE(c_json_add_object(j, "o", o1));
    EXPECT_TRUE(c_json_add_object(j, "o", o2));
    c_json_free(o1);
    c_json_free(o2);

    bool b = true;
    char *s = nullptr;
    EXPECT_TRUE(c_json_get_boolean(j, "b", &b));
    EXPECT_FALSE(b);
    EXPECT_TRUE(c_json_get_string(j, "s", &s));
    EXPECT_STREQ(s, "b");

    EXPECT_TRUE(c_json_set_config(j, C_JSON_CONFIG_ERROR_SAME_TAG));
    EXPECT_TRUE(c_json_add_boolean(j, "b2", true));
    EXPECT_FALSE(c_json_add_boolean(j, "b2", false));
    EXPECT_TRUE(c_json_add_string(j, "s2", "a"));
    EXPECT_FALSE(c_json_add_string(j, "s2", "b"));
    C_JSON o3 = c_json_new();
    c_json_add_int(o3, "v", 3);
    EXPECT_TRUE(c_json_add_object(j, "o2", o3));
    EXPECT_FALSE(c_json_add_object(j, "o2", o3));
    c_json_free(o3);

    c_json_free(j);
}

TEST(ErrorPathTest, NullArgsOnGetAndSerialize) {
    C_JSON j = c_json_new();
    ASSERT_NE(j, nullptr);
    c_json_add_string(j, "s", "hello");
    c_json_add_boolean(j, "b", true);
    c_json_add_int(j, "i", 7);
    c_json_add_array_boolean(j, "ab", true);
    c_json_add_array_string(j, "as", "x");
    c_json_add_array_int(j, "ai", 1);

    char *out = nullptr;
    size_t sz = 0;
    bool b = false;
    char *s = nullptr;
    double n = 0;
    int8_t c = 0;
    uint8_t uc = 0;
    int16_t sh = 0;
    uint16_t ush = 0;
    int32_t i = 0;
    uint32_t ui = 0;
    int64_t l = 0;
    uint64_t ul = 0;
    size_t asize = 0;

    EXPECT_FALSE(c_json_serialize(nullptr, &out, &sz, C_JSON_STR_UNFORMATTED));
    EXPECT_FALSE(c_json_serialize(j, nullptr, &sz, C_JSON_STR_UNFORMATTED));
    EXPECT_FALSE(c_json_print(nullptr, C_JSON_STR_UNFORMATTED));
    EXPECT_FALSE(c_json_set_config(nullptr, C_JSON_CONFIG_UPDATE_SAME_TAG));
    EXPECT_EQ(c_json_free(nullptr), nullptr);

    EXPECT_EQ(c_json_get_object(nullptr, "s"), nullptr);
    EXPECT_EQ(c_json_get_object(j, nullptr), nullptr);
    EXPECT_FALSE(c_json_get_string(nullptr, "s", &s));
    EXPECT_FALSE(c_json_get_string(j, nullptr, &s));
    EXPECT_FALSE(c_json_get_string(j, "s", nullptr));
    EXPECT_FALSE(c_json_get_boolean(nullptr, "b", &b));
    EXPECT_FALSE(c_json_get_boolean(j, nullptr, &b));
    EXPECT_FALSE(c_json_get_boolean(j, "b", nullptr));
    EXPECT_FALSE(c_json_get_number(j, "i", nullptr));
    EXPECT_FALSE(c_json_get_char(j, "i", nullptr));
    EXPECT_FALSE(c_json_get_u_char(j, "i", nullptr));
    EXPECT_FALSE(c_json_get_short(j, "i", nullptr));
    EXPECT_FALSE(c_json_get_u_short(j, "i", nullptr));
    EXPECT_FALSE(c_json_get_int(j, "i", nullptr));
    EXPECT_FALSE(c_json_get_u_int(j, "i", nullptr));
    EXPECT_FALSE(c_json_get_long(j, "i", nullptr));
    EXPECT_FALSE(c_json_get_u_long(j, "i", nullptr));

    EXPECT_FALSE(c_json_get_array_size(nullptr, "ai", &asize));
    EXPECT_FALSE(c_json_get_array_boolean(nullptr, "ab", 0, &b));
    EXPECT_FALSE(c_json_get_array_boolean(j, nullptr, 0, &b));
    EXPECT_FALSE(c_json_get_array_boolean(j, "ab", 0, nullptr));
    EXPECT_EQ(c_json_get_array_object(nullptr, "ai", 0), nullptr);
    EXPECT_EQ(c_json_get_array_object(j, nullptr, 0), nullptr);
    EXPECT_FALSE(c_json_get_array_string(nullptr, "as", 0, &s));
    EXPECT_FALSE(c_json_get_array_string(j, nullptr, 0, &s));
    EXPECT_FALSE(c_json_get_array_string(j, "as", 0, nullptr));
    EXPECT_FALSE(c_json_get_array_char(j, "ai", 0, nullptr));
    EXPECT_FALSE(c_json_get_array_short(j, "ai", 0, nullptr));
    EXPECT_FALSE(c_json_get_array_u_short(j, "ai", 0, nullptr));
    EXPECT_FALSE(c_json_get_array_int(j, "ai", 0, nullptr));
    EXPECT_FALSE(c_json_get_array_u_int(j, "ai", 0, nullptr));
    EXPECT_FALSE(c_json_get_array_long(j, "ai", 0, nullptr));
    EXPECT_FALSE(c_json_get_array_u_long(j, "ai", 0, nullptr));

    EXPECT_FALSE(c_json_get_char(j, "missing", &c));
    EXPECT_FALSE(c_json_get_u_char(j, "missing", &uc));
    EXPECT_FALSE(c_json_get_short(j, "missing", &sh));
    EXPECT_FALSE(c_json_get_u_short(j, "missing", &ush));
    EXPECT_FALSE(c_json_get_u_int(j, "missing", &ui));
    EXPECT_FALSE(c_json_get_long(j, "missing", &l));
    EXPECT_FALSE(c_json_get_u_long(j, "missing", &ul));
    EXPECT_FALSE(c_json_get_string(j, "missing", &s));

    (void)n;
    (void)i;
    c_json_free(j);
}

TEST(ErrorPathTest, InvalidSerializeEscape) {
    C_JSON j = c_json_new();
    ASSERT_NE(j, nullptr);
    c_json_add_int(j, "x", 1);

    char *out = nullptr;
    size_t sz = 0;
    auto bad = static_cast<E_C_JSON_STR_ESCAPE>(99);
    EXPECT_FALSE(c_json_serialize(j, &out, &sz, bad));
    EXPECT_FALSE(c_json_print(j, bad));

    c_json_free(j);
}

TEST(ErrorPathTest, TypedGetOverflowsAndNonIntegers) {
    C_JSON j = c_json_new();
    ASSERT_NE(j, nullptr);

    c_json_add_number(j, "short_hi", static_cast<double>(INT16_MAX) + 1);
    c_json_add_number(j, "short_lo", static_cast<double>(INT16_MIN) - 1);
    c_json_add_number(j, "ushort_lo", -1.0);
    c_json_add_number(j, "int_hi", static_cast<double>(INT32_MAX) + 1.0);
    c_json_add_number(j, "int_lo", static_cast<double>(INT32_MIN) - 1.0);
    c_json_add_number(j, "uint_hi", static_cast<double>(UINT32_MAX) + 1.0);
    c_json_add_number(j, "uint_lo", -1.0);
    c_json_add_number(j, "floaty", 1.5);
    c_json_add_number(j, "uchar_hi", static_cast<double>(UINT8_MAX) + 1);
    c_json_add_number(j, "uchar_float", 1.5);

    int16_t sh = 0;
    uint16_t ush = 0;
    int32_t i = 0;
    uint32_t ui = 0;
    int8_t c = 0;
    uint8_t uc = 0;
    int64_t l = 0;
    uint64_t ul = 0;

    EXPECT_FALSE(c_json_get_short(j, "short_hi", &sh));
    EXPECT_FALSE(c_json_get_short(j, "short_lo", &sh));
    EXPECT_FALSE(c_json_get_short(j, "floaty", &sh));
    EXPECT_FALSE(c_json_get_u_short(j, "ushort_lo", &ush));
    EXPECT_FALSE(c_json_get_u_short(j, "floaty", &ush));
    EXPECT_FALSE(c_json_get_int(j, "int_hi", &i));
    EXPECT_FALSE(c_json_get_int(j, "int_lo", &i));
    EXPECT_FALSE(c_json_get_int(j, "floaty", &i));
    EXPECT_FALSE(c_json_get_u_int(j, "uint_hi", &ui));
    EXPECT_FALSE(c_json_get_u_int(j, "uint_lo", &ui));
    EXPECT_FALSE(c_json_get_u_int(j, "floaty", &ui));
    EXPECT_FALSE(c_json_get_char(j, "floaty", &c));
    EXPECT_FALSE(c_json_get_u_char(j, "uchar_hi", &uc));
    EXPECT_FALSE(c_json_get_u_char(j, "uchar_float", &uc));

    c_json_add_number(j, "long_hi", 1e20);
    c_json_add_number(j, "long_lo", -1e20);
    c_json_add_number(j, "ulong_hi", 1e20);
    c_json_add_number(j, "ulong_lo", -1.0);
    EXPECT_FALSE(c_json_get_long(j, "long_hi", &l));
    EXPECT_FALSE(c_json_get_long(j, "long_lo", &l));
    EXPECT_FALSE(c_json_get_u_long(j, "ulong_hi", &ul));
    EXPECT_FALSE(c_json_get_u_long(j, "ulong_lo", &ul));

    c_json_free(j);
}

TEST(ErrorPathTest, ArrayGetTypeAndBoundsFailures) {
    C_JSON j = c_json_new();
    ASSERT_NE(j, nullptr);

    c_json_add_int(j, "not_array", 1);
    c_json_add_array_int(j, "ai", 10);
    c_json_add_array_string(j, "as", "hi");
    c_json_add_array_boolean(j, "ab", true);

    C_JSON child = c_json_new();
    c_json_add_int(child, "v", 1);
    c_json_add_array_object(j, "ao", 0, child);
    c_json_free(child);

    bool b = false;
    char *s = nullptr;
    char c = 0;
    int16_t sh = 0;
    uint16_t ush = 0;
    int32_t i = 0;
    uint32_t ui = 0;
    int64_t l = 0;
    uint64_t ul = 0;
    double n = 0;

    EXPECT_FALSE(c_json_get_array_boolean(j, "not_array", 0, &b));
    EXPECT_FALSE(c_json_get_array_boolean(j, "ab", 5, &b));
    EXPECT_FALSE(c_json_get_array_boolean(j, "ai", 0, &b));
    EXPECT_FALSE(c_json_get_array_string(j, "not_array", 0, &s));
    EXPECT_FALSE(c_json_get_array_string(j, "as", 5, &s));
    EXPECT_FALSE(c_json_get_array_string(j, "ai", 0, &s));
    EXPECT_EQ(c_json_get_array_object(j, "not_array", 0), nullptr);
    EXPECT_EQ(c_json_get_array_object(j, "ai", 0), nullptr);

    EXPECT_FALSE(c_json_get_array_number(j, "as", 0, &n));
    EXPECT_FALSE(c_json_get_array_char(j, "missing", 0, &c));
    EXPECT_FALSE(c_json_get_array_short(j, "missing", 0, &sh));
    EXPECT_FALSE(c_json_get_array_u_short(j, "missing", 0, &ush));
    EXPECT_FALSE(c_json_get_array_int(j, "missing", 0, &i));
    EXPECT_FALSE(c_json_get_array_u_int(j, "missing", 0, &ui));
    EXPECT_FALSE(c_json_get_array_long(j, "missing", 0, &l));
    EXPECT_FALSE(c_json_get_array_u_long(j, "missing", 0, &ul));

    c_json_free(j);
}

TEST(ErrorPathTest, ArrayTypedNumericSuccessAndOverflow) {
    C_JSON j = c_json_new();
    ASSERT_NE(j, nullptr);

    c_json_add_array_u_short(j, "aus", static_cast<uint16_t>(40000));
    c_json_add_array_short(j, "ash", static_cast<int16_t>(-1000));
    c_json_add_array_u_int(j, "aui", 3000000000u);
    c_json_add_array_int(j, "ai", -50);
    c_json_add_array_double(j, "ahi", static_cast<double>(INT16_MAX) + 1);
    c_json_add_array_double(j, "alo", static_cast<double>(INT16_MIN) - 1);
    c_json_add_array_double(j, "aint_hi", static_cast<double>(INT32_MAX) + 1024.0);
    c_json_add_array_double(j, "aint_lo", static_cast<double>(INT32_MIN) - 1024.0);
    c_json_add_array_double(j, "afloat", 2.5);
    c_json_add_array_double(j, "aneg", -1.0);
    c_json_add_array_double(j, "abig", 1e20);

    uint16_t ush = 0;
    int16_t sh = 0;
    uint32_t ui = 0;
    int32_t i = 0;
    int64_t l = 0;
    uint64_t ul = 0;
    char c = 0;

    EXPECT_TRUE(c_json_get_array_u_short(j, "aus", 0, &ush));
    EXPECT_EQ(ush, 40000);
    EXPECT_TRUE(c_json_get_array_short(j, "ash", 0, &sh));
    EXPECT_EQ(sh, -1000);
    EXPECT_TRUE(c_json_get_array_u_int(j, "aui", 0, &ui));
    EXPECT_EQ(ui, 3000000000u);
    EXPECT_TRUE(c_json_get_array_int(j, "ai", 0, &i));
    EXPECT_EQ(i, -50);

    EXPECT_FALSE(c_json_get_array_short(j, "ahi", 0, &sh));
    EXPECT_FALSE(c_json_get_array_short(j, "alo", 0, &sh));
    EXPECT_FALSE(c_json_get_array_short(j, "afloat", 0, &sh));
    EXPECT_FALSE(c_json_get_array_u_short(j, "aneg", 0, &ush));
    EXPECT_FALSE(c_json_get_array_int(j, "aint_hi", 0, &i));
    EXPECT_FALSE(c_json_get_array_int(j, "aint_lo", 0, &i));
    EXPECT_FALSE(c_json_get_array_int(j, "afloat", 0, &i));
    EXPECT_FALSE(c_json_get_array_u_int(j, "aneg", 0, &ui));
    EXPECT_FALSE(c_json_get_array_u_int(j, "afloat", 0, &ui));
    EXPECT_FALSE(c_json_get_array_u_int(j, "abig", 0, &ui));
    EXPECT_FALSE(c_json_get_array_long(j, "abig", 0, &l));
    EXPECT_FALSE(c_json_get_array_long(j, "afloat", 0, &l));
    EXPECT_FALSE(c_json_get_array_u_long(j, "aneg", 0, &ul));
    EXPECT_FALSE(c_json_get_array_u_long(j, "abig", 0, &ul));
    EXPECT_FALSE(c_json_get_array_u_long(j, "afloat", 0, &ul));
    EXPECT_FALSE(c_json_get_array_char(j, "alo", 0, &c));
    EXPECT_FALSE(c_json_get_array_char(j, "afloat", 0, &c));

    c_json_free(j);
}

TEST(ErrorPathTest, VerifyExistViaDuplicateAndMissingObject) {
    C_JSON j = c_json_new();
    ASSERT_NE(j, nullptr);

    EXPECT_EQ(c_json_get_object(j, "nope"), nullptr);
    EXPECT_FALSE(c_json_get_string(j, "nope", nullptr));

    c_json_free(j);
}
