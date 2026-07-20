#ifndef TEST_HELPERS_HPP_
#define TEST_HELPERS_HPP_

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

extern "C" {
#include "c_json.h"
}

inline size_t count_substr(const char *s, const char *pat) {
    if (!s || !pat || !*pat) {
        return 0;
    }
    size_t n = 0;
    const size_t plen = strlen(pat);
    const char *p = s;
    while ((p = strstr(p, pat)) != nullptr) {
        n++;
        p += plen;
    }
    return n;
}

inline char *dump_json_to_string(C_JSON j) {
    char *out = nullptr;
    size_t sz = 0;
    if (!c_json_serialize(j, &out, &sz, C_JSON_STR_UNFORMATTED)) {
        return nullptr;
    }
    EXPECT_NE(out, nullptr);
    return out;
}

inline C_JSON create_test_json() {
    C_JSON json = c_json_new();
    EXPECT_NE(json, nullptr);

    c_json_add_boolean(json, "b_true", true);
    c_json_add_string(json, "str", "hello");
    c_json_add_int(json, "i", -1000000);
    c_json_add_double(json, "d", 3.1415);

    for (int j = 0; j < 3; j++) {
        c_json_add_array_int(json, "arr_int", j + 1000);
        char buf[32];
        snprintf(buf, sizeof(buf), "str%d", j);
        c_json_add_array_string(json, "arr_str", buf);
    }

    C_JSON child = c_json_new();
    c_json_add_string(child, "child_key", "child_value");
    c_json_add_object(json, "obj", child);
    c_json_free(child);

    return json;
}

#endif  // TEST_HELPERS_HPP_
