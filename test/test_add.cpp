#include <gtest/gtest.h>

#include <climits>
#include <cstdint>
#include <cstring>

#include "helpers.hpp"

TEST(AddTest, AddScalarsSuccess) {
    C_JSON j = c_json_new();
    ASSERT_NE(j, nullptr);

    EXPECT_TRUE(c_json_add_boolean(j, "bT", true));
    EXPECT_TRUE(c_json_add_boolean(j, "bF", false));
    EXPECT_TRUE(c_json_add_char(j, "c", static_cast<char>(-10)));
    EXPECT_TRUE(c_json_add_u_char(j, "uc", static_cast<unsigned char>(250)));
    EXPECT_TRUE(c_json_add_short(j, "s", static_cast<int16_t>(-3000)));
    EXPECT_TRUE(c_json_add_u_short(j, "us", static_cast<uint16_t>(60000)));
    EXPECT_TRUE(c_json_add_int(j, "i", static_cast<int32_t>(-1000000)));
    EXPECT_TRUE(c_json_add_u_int(j, "ui", static_cast<uint32_t>(4000000)));
    EXPECT_TRUE(c_json_add_long(j, "l", static_cast<int64_t>(-5000000000LL)));
    EXPECT_TRUE(c_json_add_u_long(j, "ul", static_cast<uint64_t>(10000000000ULL)));
    EXPECT_TRUE(c_json_add_double(j, "d", 3.1415));
    EXPECT_TRUE(c_json_add_string(j, "str", "hello"));

    bool b = false;
    int32_t i = 0;
    uint8_t uc = 0;
    EXPECT_TRUE(c_json_get_boolean(j, "bT", &b) && b);
    EXPECT_TRUE(c_json_get_boolean(j, "bF", &b) && !b);
    EXPECT_TRUE(c_json_get_int(j, "i", &i) && i == -1000000);
    EXPECT_TRUE(c_json_get_u_char(j, "uc", &uc) && uc == 250);

    c_json_free(j);
}

TEST(AddTest, AddArraysSuccess) {
    C_JSON j = c_json_new();
    ASSERT_NE(j, nullptr);

    EXPECT_TRUE(c_json_add_array_boolean(j, "ab", true));
    EXPECT_TRUE(c_json_add_array_char(j, "ac", static_cast<char>(200)));
    EXPECT_TRUE(c_json_add_array_short(j, "as", static_cast<int16_t>(-123)));
    EXPECT_TRUE(c_json_add_array_u_short(j, "aus", static_cast<uint16_t>(65530)));
    EXPECT_TRUE(c_json_add_array_int(j, "ai", static_cast<int32_t>(-1000)));
    EXPECT_TRUE(c_json_add_array_u_int(j, "aui", static_cast<uint32_t>(4000000)));
    EXPECT_TRUE(c_json_add_array_long(j, "al", static_cast<int64_t>(-9)));
    EXPECT_TRUE(c_json_add_array_u_long(j, "aul", static_cast<uint64_t>(123456789ULL)));
    EXPECT_TRUE(c_json_add_array_double(j, "ad", 1.5));
    EXPECT_TRUE(c_json_add_array_string(j, "astr", "a"));

    size_t sz = 0;
    bool b = false;
    EXPECT_TRUE(c_json_get_array_size(j, "ab", &sz) && sz == 1);
    EXPECT_TRUE(c_json_get_array_boolean(j, "ab", 0, &b) && b);

    int32_t i_val = 0;
    EXPECT_TRUE(c_json_get_array_size(j, "ai", &sz) && sz == 1);
    EXPECT_TRUE(c_json_get_array_int(j, "ai", 0, &i_val) && i_val == -1000);

    c_json_free(j);
}

TEST(AddTest, StringCopySemantics) {
    C_JSON j = c_json_new();
    ASSERT_NE(j, nullptr);

    char buf[16];
    strcpy(buf, "abcd");
    EXPECT_TRUE(c_json_add_string(j, "s", buf));
    strcpy(buf, "ZZZZ");

    char *sv = nullptr;
    EXPECT_TRUE(c_json_get_string(j, "s", &sv));
    EXPECT_STREQ(sv, "abcd");

    c_json_free(j);
}

TEST(AddTest, ObjectCopySemantics) {
    C_JSON parent = c_json_new();
    C_JSON child = c_json_new();
    ASSERT_NE(parent, nullptr);
    ASSERT_NE(child, nullptr);

    c_json_add_string(child, "k", "v");
    EXPECT_TRUE(c_json_add_object(parent, "obj", child));
    c_json_free(child);

    C_JSON got = c_json_get_object(parent, "obj");
    ASSERT_NE(got, nullptr);

    char *sv = nullptr;
    EXPECT_TRUE(c_json_get_string(got, "k", &sv));
    EXPECT_STREQ(sv, "v");

    c_json_free(parent);
}

TEST(AddTest, ConfigAllowSameTag) {
    C_JSON j = c_json_new();
    EXPECT_TRUE(c_json_set_config(j, C_JSON_CONFIG_ALLOW_SAME_TAG));
    EXPECT_TRUE(c_json_add_int(j, "x", 1));
    EXPECT_TRUE(c_json_add_int(j, "x", 2));

    char *out = dump_json_to_string(j);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(count_substr(out, "\"x\":"), 2U);

    c_json_free(j);
}

TEST(AddTest, ConfigUpdateSameTag) {
    C_JSON j = c_json_new();
    EXPECT_TRUE(c_json_set_config(j, C_JSON_CONFIG_UPDATE_SAME_TAG));
    EXPECT_TRUE(c_json_add_int(j, "x", 1));
    EXPECT_TRUE(c_json_add_int(j, "x", 2));

    int32_t v = 0;
    EXPECT_TRUE(c_json_get_int(j, "x", &v));
    EXPECT_EQ(v, 2);

    char *out = dump_json_to_string(j);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(count_substr(out, "\"x\":"), 1U);

    c_json_free(j);
}

TEST(AddTest, ConfigErrorOnSameTag) {
    C_JSON j = c_json_new();
    EXPECT_TRUE(c_json_set_config(j, C_JSON_CONFIG_ERROR_SAME_TAG));
    EXPECT_TRUE(c_json_add_int(j, "x", 1));
    EXPECT_FALSE(c_json_add_int(j, "x", 2));

    int32_t v = 0;
    EXPECT_TRUE(c_json_get_int(j, "x", &v));
    EXPECT_EQ(v, 1);

    c_json_free(j);
}

TEST(AddTest, AddInvalidArgumentsFailure) {
    C_JSON j = c_json_new();
    ASSERT_NE(j, nullptr);

    EXPECT_FALSE(c_json_add_int(nullptr, "x", 1));
    EXPECT_FALSE(c_json_add_array_int(nullptr, "ai", 1));
    EXPECT_FALSE(c_json_add_int(j, nullptr, 1));
    EXPECT_FALSE(c_json_add_array_int(j, nullptr, 1));
    EXPECT_FALSE(c_json_add_string(j, "s", nullptr));
    EXPECT_FALSE(c_json_add_array_string(j, "as", nullptr));
    EXPECT_FALSE(c_json_add_object(j, "o", nullptr));
    EXPECT_FALSE(c_json_add_array_object(j, "aobj", 0, nullptr));

    c_json_free(j);
}

TEST(AddTest, AddArrayToNonArrayTagFailure) {
    C_JSON json = c_json_new();
    c_json_add_int(json, "nao_e_um_array", 123);

    EXPECT_FALSE(c_json_add_array_string(json, "nao_e_um_array", "teste"));
    EXPECT_FALSE(c_json_add_array_boolean(json, "nao_e_um_array", true));
    EXPECT_FALSE(c_json_add_array_number(json, "nao_e_um_array", 456.7));

    c_json_free(json);
}

TEST(AddTest, AddArrayObjectToNonArrayTagFailure) {
    C_JSON json = c_json_new();
    c_json_add_int(json, "nao_e_um_array", 123);

    C_JSON obj = c_json_new();
    EXPECT_FALSE(c_json_add_array_object(json, "nao_e_um_array", 0, obj));
    c_json_free(obj);
    c_json_free(json);
}
