#include <gtest/gtest.h>

#include <climits>
#include <cstdint>
#include <cstring>

#include "helpers.hpp"

TEST(CoverageTest, SerializeFormattedAndUnformattedWoSpace) {
    C_JSON json = c_json_new();
    c_json_add_string(json, "nome", "teste");
    c_json_add_int(json, "valor", 42);

    char *formatted = nullptr;
    size_t size = 0;
    EXPECT_TRUE(c_json_serialize(json, &formatted, &size, C_JSON_STR_FORMATTED));
    ASSERT_NE(formatted, nullptr);
    EXPECT_NE(strstr(formatted, "\n"), nullptr);

    char *compact = nullptr;
    EXPECT_TRUE(c_json_serialize(json, &compact, &size, C_JSON_STR_UNFORMATTED_WO_SPACE));
    ASSERT_NE(compact, nullptr);
    EXPECT_STREQ(compact, "{\"nome\":\"teste\",\"valor\":42}");

    c_json_free(json);
}

TEST(CoverageTest, PrintValidArguments) {
    C_JSON json = c_json_new();
    c_json_add_int(json, "x", 1);
    EXPECT_TRUE(c_json_print(json, C_JSON_STR_FORMATTED));
    EXPECT_TRUE(c_json_print(json, C_JSON_STR_UNFORMATTED));
    c_json_free(json);
}

TEST(CoverageTest, GetNumericTypesSuccess) {
    C_JSON json = c_json_new();
    c_json_add_char(json, "c", static_cast<char>(-5));
    c_json_add_short(json, "s", static_cast<int16_t>(-100));
    c_json_add_u_short(json, "us", static_cast<uint16_t>(500));
    c_json_add_number(json, "n", 99.5);

    int8_t c = 0;
    int16_t s = 0;
    uint16_t us = 0;
    double n = 0;

    EXPECT_TRUE(c_json_get_char(json, "c", &c));
    EXPECT_EQ(c, -5);
    EXPECT_TRUE(c_json_get_short(json, "s", &s));
    EXPECT_EQ(s, -100);
    EXPECT_TRUE(c_json_get_u_short(json, "us", &us));
    EXPECT_EQ(us, 500U);
    EXPECT_TRUE(c_json_get_number(json, "n", &n));
    EXPECT_NEAR(n, 99.5, 0.001);

    c_json_free(json);
}

TEST(CoverageTest, GetArrayNumericTypesSuccess) {
    C_JSON json = c_json_new();
    c_json_add_array_char(json, "ac", static_cast<char>(10));
    c_json_add_array_u_char(json, "auc", static_cast<unsigned char>(200));
    c_json_add_array_long(json, "al", static_cast<int64_t>(-999));
    c_json_add_array_u_long(json, "aul", static_cast<uint64_t>(8888));
    c_json_add_array_double(json, "ad", 2.718);
    c_json_add_array_number(json, "an", 42.0);

    char c = 0;
    int64_t l = 0;
    uint64_t ul = 0;
    double d = 0;
    double n = 0;

    EXPECT_TRUE(c_json_get_array_char(json, "ac", 0, &c));
    EXPECT_EQ(c, 10);
    char uc_val = 0;
    EXPECT_TRUE(c_json_get_array_char(json, "auc", 0, &uc_val));
    EXPECT_EQ(static_cast<unsigned char>(uc_val), 200U);
    EXPECT_TRUE(c_json_get_array_long(json, "al", 0, &l));
    EXPECT_EQ(l, -999);
    EXPECT_TRUE(c_json_get_array_u_long(json, "aul", 0, &ul));
    EXPECT_EQ(ul, 8888ULL);
    EXPECT_TRUE(c_json_get_array_double(json, "ad", 0, &d));
    EXPECT_NEAR(d, 2.718, 0.001);
    EXPECT_TRUE(c_json_get_array_number(json, "an", 0, &n));
    EXPECT_NEAR(n, 42.0, 0.001);

    c_json_free(json);
}

TEST(CoverageTest, GetArrayObjectSuccess) {
    C_JSON json = c_json_new();
    C_JSON child = c_json_new();
    c_json_add_string(child, "id", "item-1");
    EXPECT_TRUE(c_json_add_array_object(json, "items", 0, child));
    c_json_free(child);

    C_JSON got = c_json_get_array_object(json, "items", 0);
    ASSERT_NE(got, nullptr);
    char *id = nullptr;
    EXPECT_TRUE(c_json_get_string(got, "id", &id));
    EXPECT_STREQ(id, "item-1");

    c_json_free(json);
}

TEST(CoverageTest, FreeReturnsNull) {
    C_JSON json = c_json_new();
    c_json_add_int(json, "a", 1);
    json = c_json_free(json);
    EXPECT_EQ(json, nullptr);
}

TEST(CoverageTest, ConfigAllowSameTagWithStrings) {
    C_JSON json = c_json_new();
    EXPECT_TRUE(c_json_set_config(json, C_JSON_CONFIG_ALLOW_SAME_TAG));
    EXPECT_TRUE(c_json_add_string(json, "tag", "primeiro"));
    EXPECT_TRUE(c_json_add_string(json, "tag", "segundo"));

    char *out = dump_json_to_string(json);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(count_substr(out, "\"tag\":"), 2U);

    c_json_free(json);
}

TEST(CoverageTest, ConfigAllowSameTagWithObjects) {
    C_JSON json = c_json_new();
    EXPECT_TRUE(c_json_set_config(json, C_JSON_CONFIG_ALLOW_SAME_TAG));

    C_JSON obj1 = c_json_new();
    c_json_add_int(obj1, "v", 1);
    C_JSON obj2 = c_json_new();
    c_json_add_int(obj2, "v", 2);

    EXPECT_TRUE(c_json_add_object(json, "obj", obj1));
    EXPECT_TRUE(c_json_add_object(json, "obj", obj2));
    c_json_free(obj1);
    c_json_free(obj2);

    char *out = dump_json_to_string(json);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(count_substr(out, "\"obj\":"), 2U);

    c_json_free(json);
}

TEST(CoverageTest, GetCharArrayOverflow) {
    C_JSON json = c_json_new();
    c_json_add_array_double(json, "arr", static_cast<double>(UINT8_MAX) + 1);

    char c = 0;
    EXPECT_FALSE(c_json_get_array_char(json, "arr", 0, &c));

    c_json_free(json);
}

TEST(CoverageTest, GetUIntAndULongSuccess) {
    C_JSON json = c_json_new();
    c_json_add_u_int(json, "ui", 4000000U);
    c_json_add_u_long(json, "ul", 10000000000ULL);
    c_json_add_array_u_int(json, "aui", 123U);

    uint32_t ui = 0;
    uint64_t ul = 0;
    uint32_t aui = 0;

    EXPECT_TRUE(c_json_get_u_int(json, "ui", &ui));
    EXPECT_EQ(ui, 4000000U);
    EXPECT_TRUE(c_json_get_u_long(json, "ul", &ul));
    EXPECT_EQ(ul, 10000000000ULL);
    EXPECT_TRUE(c_json_get_array_u_int(json, "aui", 0, &aui));
    EXPECT_EQ(aui, 123U);

    c_json_free(json);
}

TEST(CoverageTest, ParserReplacesExistingContent) {
    C_JSON json = c_json_new();
    EXPECT_TRUE(c_json_parser(json, "{\"a\":1}"));
    EXPECT_TRUE(c_json_parser(json, "{\"b\":2}"));

    int32_t val = 0;
    EXPECT_FALSE(c_json_get_int(json, "a", &val));
    EXPECT_TRUE(c_json_get_int(json, "b", &val));
    EXPECT_EQ(val, 2);

    c_json_free(json);
}
