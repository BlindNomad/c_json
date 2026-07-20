#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>

#include "helpers.hpp"

TEST(ParserTest, ParseAllDataTypesSuccess) {
    C_JSON json = c_json_new();
    ASSERT_NE(json, nullptr);

    const char *json_str =
        "{"
        "\"name\": \"Jose\","
        "\"age\": 30,"
        "\"is_active\": true,"
        "\"balance\": 123.45,"
        "\"permissions\": [\"read\", \"write\"],"
        "\"address\": {"
        "  \"street\": \"Rua Principal\","
        "  \"number\": 100"
        "}"
        "}";

    EXPECT_TRUE(c_json_parser(json, json_str));

    char *name = nullptr;
    EXPECT_TRUE(c_json_get_string(json, "name", &name));
    EXPECT_STREQ(name, "Jose");

    int32_t age = 0;
    EXPECT_TRUE(c_json_get_int(json, "age", &age));
    EXPECT_EQ(age, 30);

    bool is_active = false;
    EXPECT_TRUE(c_json_get_boolean(json, "is_active", &is_active));
    EXPECT_TRUE(is_active);

    size_t perm_size = 0;
    EXPECT_TRUE(c_json_get_array_size(json, "permissions", &perm_size));
    EXPECT_EQ(perm_size, 2U);

    char *permission = nullptr;
    EXPECT_TRUE(c_json_get_array_string(json, "permissions", 1, &permission));
    EXPECT_STREQ(permission, "write");

    C_JSON address = c_json_get_object(json, "address");
    ASSERT_NE(address, nullptr);
    int32_t number = 0;
    EXPECT_TRUE(c_json_get_int(address, "number", &number));
    EXPECT_EQ(number, 100);

    c_json_free(json);
}

TEST(ParserTest, ParseEmptyObjectSuccess) {
    C_JSON json = c_json_new();
    ASSERT_NE(json, nullptr);

    EXPECT_TRUE(c_json_parser(json, "{}"));

    int32_t val = 0;
    EXPECT_FALSE(c_json_get_int(json, "any_key", &val));

    c_json_free(json);
}

TEST(ParserTest, ParseMalformedJsonFailure) {
    const char *invalid_json_list[] = {"{ \"key\": \"value\" ", "{ \"key\": \"value\", }", "{ key: \"value\" }",
                                       "[1, 2, 3"};
    const size_t num_tests = sizeof(invalid_json_list) / sizeof(invalid_json_list[0]);

    for (size_t i = 0; i < num_tests; i++) {
        C_JSON json = c_json_new();
        ASSERT_NE(json, nullptr);
        EXPECT_FALSE(c_json_parser(json, invalid_json_list[i]));
        c_json_free(json);
    }
}

TEST(ParserTest, ParseInvalidArgumentsFailure) {
    C_JSON json = c_json_new();
    ASSERT_NE(json, nullptr);
    const char *valid_json_str = "{\"a\":1}";

    EXPECT_FALSE(c_json_parser(nullptr, valid_json_str));
    EXPECT_FALSE(c_json_parser(json, nullptr));
    EXPECT_FALSE(c_json_parser(json, ""));

    c_json_free(json);
}
