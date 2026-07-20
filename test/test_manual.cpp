#include <gtest/gtest.h>

#include <cstring>

#include "helpers.hpp"

#define JSON_PAYLOAD                                                                                                   \
    "{\"scope\":\"transaction\",\"action\":\"crypto\",\"data\":\"{\\\"scope\\\": \\\"transaction\\\", "                \
    "\\\"action\\\": \\\"crypto\\\", \\\"data\\\": {\\\"iv\\\": \\\"JUzMsOJN71LhFqaL\\\", \\\"cryptogram\\\": "        \
    "\\\"u7VY2tuSfW+T2ANzR50XMiPd45Oay8UZ5/iMx/"                                                                       \
    "CivjQ5HyHXLmX6Syje6qXcvaZIh5wddleDrkYu75SP+VRx6WWnDqaO1v2X8cVbD3L3zMGmlqUTtGEAHSkqw713A9bsQwWk52Anf814ZozEIxMT/"  \
    "iQuLiX/JrHGhQaoauIg/K+wi0G9KX05UIi/rn9l82kSA/"                                                                    \
    "i3dhYwz5pM2FXZNGVgTgf0oa8jakkWIDhwdd6Q9ZnRC0fY3ymtKiSxmiiDYfv+OS2cn8Ucf8Q+xMjwHfIPrWb3kQTs8YBTrP6frEhGUng=\\\", " \
    "\\\"tag\\\": \\\"cdNVDmfLXN+YA2Pf4i/EOw==\\\"}}\"}"

TEST(ManualTest, GetObjectFromParsedPayload) {
    C_JSON json_crypt = c_json_new();
    ASSERT_NE(json_crypt, nullptr);
    EXPECT_TRUE(c_json_parser(json_crypt, JSON_PAYLOAD));

    char *str_action = nullptr;
    char *str_scope = nullptr;
    EXPECT_TRUE(c_json_get_string(json_crypt, "action", &str_action));
    EXPECT_TRUE(c_json_get_string(json_crypt, "scope", &str_scope));

    c_json_free(json_crypt);
}

TEST(ManualTest, AddArrayObjects) {
    C_JSON json_string = c_json_new();
    ASSERT_NE(json_string, nullptr);

    C_JSON obj_01 = c_json_new();
    c_json_add_string(obj_01, "nome", "Produto 01");

    C_JSON obj_02 = c_json_new();
    c_json_add_string(obj_02, "nome", "Produto 02");

    C_JSON obj_03 = c_json_new();
    c_json_add_string(obj_03, "nome", "Produto 03");

    c_json_add_string(json_string, "pedido_id", "PED-5544");
    EXPECT_TRUE(c_json_add_array_object(json_string, "produtos", 0, obj_01));
    EXPECT_TRUE(c_json_add_array_object(json_string, "produtos", 0, obj_02));
    EXPECT_TRUE(c_json_add_array_object(json_string, "produtos", 0, obj_03));

    c_json_free(obj_01);
    c_json_free(obj_02);
    c_json_free(obj_03);

    EXPECT_TRUE(c_json_print(json_string, C_JSON_STR_UNFORMATTED));

    size_t size = 0;
    EXPECT_TRUE(c_json_get_array_size(json_string, "produtos", &size));
    EXPECT_EQ(size, 3U);

    C_JSON got = c_json_get_array_object(json_string, "produtos", 1);
    ASSERT_NE(got, nullptr);
    char *nome = nullptr;
    EXPECT_TRUE(c_json_get_string(got, "nome", &nome));
    EXPECT_STREQ(nome, "Produto 02");

    c_json_free(json_string);
}
