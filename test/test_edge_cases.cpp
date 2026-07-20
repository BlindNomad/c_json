#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>

extern "C" {
#include "c_json_list.h"
}

#include "helpers.hpp"

TEST(EdgeCaseTest, ListInvalidArguments) {
    P_C_JSON_LIST list = c_json_list_new();
    ASSERT_NE(list, nullptr);
    int x = 1;

    EXPECT_FALSE(c_json_list_add(nullptr, &x));
    EXPECT_FALSE(c_json_list_add(list, nullptr));
    EXPECT_FALSE(c_json_list_free(nullptr, nullptr));

    c_json_list_free(list, nullptr);
}

TEST(EdgeCaseTest, ListAddMultipleItems) {
    P_C_JSON_LIST list = c_json_list_new();
    int x = 1;
    int y = 2;
    int z = 3;

    EXPECT_TRUE(c_json_list_add(list, &x));
    EXPECT_TRUE(c_json_list_add(list, &y));
    EXPECT_TRUE(c_json_list_add(list, &z));

    c_json_list_free(list, nullptr);
}

TEST(EdgeCaseTest, StringPrintInvalidArguments) {
    C_JSON json = c_json_new();
    char *str = nullptr;
    size_t size = 0;

    EXPECT_FALSE(c_json_serialize(nullptr, &str, &size, C_JSON_STR_UNFORMATTED));
    EXPECT_FALSE(c_json_serialize(json, nullptr, &size, C_JSON_STR_UNFORMATTED));
    EXPECT_FALSE(c_json_print(nullptr, C_JSON_STR_UNFORMATTED));
    EXPECT_FALSE(c_json_print(json, static_cast<E_C_JSON_STR_ESCAPE>(99)));

    c_json_free(json);
}

TEST(EdgeCaseTest, SetInvalidConfig) {
    C_JSON json = c_json_new();
    EXPECT_FALSE(c_json_set_config(json, static_cast<E_C_JSON_CONFIG>(99)));
    c_json_free(json);
}

TEST(EdgeCaseTest, VersionStringValid) {
    const char *version = c_json_version();
    ASSERT_NE(version, nullptr);
    EXPECT_STRNE(version, "");
}

TEST(EdgeCaseTest, FreeAfterToString) {
    C_JSON json = c_json_new();
    c_json_add_int(json, "a", 1);
    char *str = nullptr;
    size_t size = 0;

    EXPECT_TRUE(c_json_serialize(json, &str, &size, C_JSON_STR_UNFORMATTED));
    c_json_free(json);
}

TEST(EdgeCaseTest, SerializeUnformattedWoSpaceWoEscape) {
    C_JSON json = c_json_new();
    c_json_add_int(json, "a", 1);
    c_json_add_int(json, "b", 2);
    char *str = nullptr;
    size_t size = 0;

    EXPECT_TRUE(c_json_serialize(json, &str, &size, C_JSON_STR_UNFORMATTED_WO_SPACE_WO_ESCAPE));
    EXPECT_STREQ(str, "{\"a\":1,\"b\":2}");

    c_json_free(json);
}
