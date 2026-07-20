#include <gtest/gtest.h>

#include <climits>
#include <cstdint>
#include <cstring>

#include "helpers.hpp"

TEST(GetTest, GetScalarsSuccess) {
    C_JSON json = create_test_json();

    bool b = false;
    EXPECT_TRUE(c_json_get_boolean(json, "b_true", &b));
    EXPECT_TRUE(b);

    int32_t i = 0;
    EXPECT_TRUE(c_json_get_int(json, "i", &i));
    EXPECT_EQ(i, -1000000);

    double d = 0;
    EXPECT_TRUE(c_json_get_double(json, "d", &d));
    EXPECT_NEAR(d, 3.1415, 0.0001);

    char *str = nullptr;
    EXPECT_TRUE(c_json_get_string(json, "str", &str));
    EXPECT_STREQ(str, "hello");

    c_json_free(json);
}

TEST(GetTest, GetArraysSuccess) {
    C_JSON json = create_test_json();

    size_t size = 0;
    EXPECT_TRUE(c_json_get_array_size(json, "arr_int", &size));
    EXPECT_EQ(size, 3U);

    for (uint32_t idx = 0; idx < size; idx++) {
        int32_t i_val = 0;
        EXPECT_TRUE(c_json_get_array_int(json, "arr_int", idx, &i_val));
        EXPECT_EQ(i_val, static_cast<int32_t>(idx + 1000));

        char *s_val = nullptr;
        char buf[32];
        snprintf(buf, sizeof(buf), "str%u", idx);
        EXPECT_TRUE(c_json_get_array_string(json, "arr_str", idx, &s_val));
        EXPECT_STREQ(s_val, buf);
    }

    c_json_free(json);
}

TEST(GetTest, GetObjectSuccess) {
    C_JSON json = create_test_json();

    C_JSON obj = c_json_get_object(json, "obj");
    ASSERT_NE(obj, nullptr);

    char *value = nullptr;
    EXPECT_TRUE(c_json_get_string(obj, "child_key", &value));
    EXPECT_STREQ(value, "child_value");

    c_json_free(json);
}

TEST(GetTest, GetScalarsFailures) {
    C_JSON json = create_test_json();

    bool b = false;
    int32_t i = 0;
    char *s = nullptr;

    EXPECT_FALSE(c_json_get_boolean(json, "tag_nao_existe", &b));
    EXPECT_FALSE(c_json_get_int(json, "tag_nao_existe", &i));
    EXPECT_FALSE(c_json_get_boolean(json, "str", &b));
    EXPECT_FALSE(c_json_get_int(json, "b_true", &i));
    EXPECT_FALSE(c_json_get_string(json, "i", &s));
    EXPECT_EQ(c_json_get_object(json, "i"), nullptr);

    c_json_free(json);
}

TEST(GetTest, GetArraysFailuresAndOutOfBounds) {
    C_JSON json = create_test_json();

    size_t size = 0;
    int32_t i = 0;

    EXPECT_FALSE(c_json_get_array_size(json, "str", &size));
    EXPECT_FALSE(c_json_get_array_int(json, "str", 0, &i));
    EXPECT_FALSE(c_json_get_array_int(json, "arr_int", 3, &i));
    EXPECT_FALSE(c_json_get_array_int(json, "arr_int", 99, &i));

    c_json_free(json);
}

TEST(GetTest, GetInvalidArguments) {
    C_JSON json = create_test_json();
    int32_t i = 0;
    size_t s = 0;

    EXPECT_FALSE(c_json_get_int(nullptr, "i", &i));
    EXPECT_EQ(c_json_get_object(nullptr, "obj"), nullptr);
    EXPECT_FALSE(c_json_get_int(json, nullptr, &i));
    EXPECT_FALSE(c_json_get_array_size(json, nullptr, &s));
    EXPECT_FALSE(c_json_get_int(json, "i", nullptr));
    EXPECT_FALSE(c_json_get_boolean(json, "b_true", nullptr));
    EXPECT_FALSE(c_json_get_array_size(json, "arr_int", nullptr));

    c_json_free(json);
}

TEST(GetTest, GetNumericOverflowAndConversionFailures) {
    int8_t c = 0;
    uint8_t uc = 0;
    int16_t s = 0;
    uint16_t us = 0;
    int64_t l = 0;
    uint64_t ul = 0;
    C_JSON json = c_json_new();

    c_json_add_number(json, "overflow_char", static_cast<double>(INT8_MAX) + 1);
    c_json_add_number(json, "underflow_char", static_cast<double>(INT8_MIN) - 1);
    c_json_add_number(json, "overflow_ushort", static_cast<double>(UINT16_MAX) + 1);
    c_json_add_number(json, "neg_for_uint", -1.0);

    c_json_add_array_double(json, "arr_overflow_u_char", static_cast<double>(UINT8_MAX) + 1);
    c_json_add_array_double(json, "arr_underflow_char", static_cast<double>(INT8_MIN) - 1);
    c_json_add_array_double(json, "arr_neg_for_uint", -1.0);
    c_json_add_array_double(json, "arr_overflow_ushort", static_cast<double>(UINT16_MAX) + 1);
    c_json_add_array_double(json, "arr_float_for_int", 123.45);

    EXPECT_FALSE(c_json_get_char(json, "overflow_char", &c));
    EXPECT_FALSE(c_json_get_char(json, "underflow_char", &c));
    EXPECT_FALSE(c_json_get_u_char(json, "neg_for_uint", &uc));
    EXPECT_FALSE(c_json_get_u_short(json, "overflow_ushort", &us));

    EXPECT_TRUE(c_json_get_array_short(json, "arr_underflow_char", 0, &s));
    EXPECT_FALSE(c_json_get_array_u_short(json, "arr_overflow_ushort", 0, &us));
    EXPECT_FALSE(c_json_get_array_u_short(json, "arr_float_for_int", 0, &us));

    c_json_parser(json,
                  "{ \"too_big\": 9223372036854775807.0, \"too_small\": -9223372036854775808.0, "
                  "\"too_big_u\": 18446744073709551615.0 }");

    EXPECT_FALSE(c_json_get_long(json, "too_big", &l));
    EXPECT_TRUE(c_json_get_long(json, "too_small", &l));
    EXPECT_FALSE(c_json_get_u_long(json, "too_big_u", &ul));

    c_json_free(json);
}

TEST(GetTest, GetArrayObjectOutOfBounds) {
    C_JSON json = create_test_json();

    C_JSON obj = c_json_get_array_object(json, "arr_int", 3);
    EXPECT_EQ(obj, nullptr);

    c_json_free(json);
}
