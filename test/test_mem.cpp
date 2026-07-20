#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>

extern "C" {
#include "c_json.h"
#include "c_json_mem.h"
}

TEST(MemTest, MallocZeroAndFree) {
    void *ptr = c_json_mem_malloc(0);
    ASSERT_NE(ptr, nullptr);
    c_json_mem_free(ptr);
}

TEST(MemTest, MallocSmallBufferIsZeroed) {
    constexpr size_t kLen = 32;
    auto *buf = static_cast<unsigned char *>(c_json_mem_malloc(kLen));
    ASSERT_NE(buf, nullptr);

    for (size_t i = 0; i < kLen; i++) {
        EXPECT_EQ(buf[i], 0) << "byte " << i;
    }

    c_json_mem_free(buf);
}

TEST(MemTest, WriteReadRoundTrip) {
    constexpr size_t kLen = 16;
    auto *buf = static_cast<char *>(c_json_mem_malloc(kLen));
    ASSERT_NE(buf, nullptr);

    const char payload[] = "hello-mem-test";
    std::memcpy(buf, payload, sizeof(payload));
    EXPECT_STREQ(buf, payload);

    c_json_mem_free(buf);
}

TEST(MemTest, FreeNullIsNoOp) {
    c_json_mem_free(nullptr);
}

TEST(MemTest, MallocOverflowReturnsNull) {
    EXPECT_EQ(c_json_mem_malloc(SIZE_MAX), nullptr);
    EXPECT_EQ(c_json_mem_malloc(SIZE_MAX - sizeof(size_t) + 1), nullptr);
}

TEST(MemTest, NewUsesSecureAllocatorByDefault) {
    C_JSON json = c_json_new();
    ASSERT_NE(json, nullptr);

    EXPECT_TRUE(c_json_add_string(json, "k", "v"));

    char *out = nullptr;
    size_t sz = 0;
    EXPECT_TRUE(c_json_serialize(json, &out, &sz, C_JSON_STR_UNFORMATTED));
    ASSERT_NE(out, nullptr);
    EXPECT_NE(std::strstr(out, "\"k\""), nullptr);

    json = c_json_free(json);
    EXPECT_EQ(json, nullptr);
}
