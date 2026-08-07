/// common_json_reader_fuzz_test — Fuzz test for json_reader with random inputs.

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/json_reader.h>
#include <cstdlib>
#include <cstring>
#include <vector>

using namespace chaos::il2cpp::json;

// Generate a random byte sequence of given length
static std::vector<char> random_bytes(size_t len) {
    std::vector<char> buf(len);
    for (size_t i = 0; i < len; i++) {
        buf[i] = static_cast<char>(std::rand() % 256);
    }
    return buf;
}

TEST(CommonJsonReaderFuzz, RandomShortInputs) {
    std::srand(42);
    for (int i = 0; i < 500; i++) {
        auto buf = random_bytes(static_cast<size_t>(std::rand() % 64 + 1));
        auto result = JsonParser::Parse(buf.data(), static_cast<int>(buf.size()));
        (void)result;  // Should not crash regardless of input
    }
    SUCCEED();
}

TEST(CommonJsonReaderFuzz, RandomLongInputs) {
    std::srand(123);
    for (int i = 0; i < 200; i++) {
        auto buf = random_bytes(static_cast<size_t>(std::rand() % 1024 + 1));
        auto result = JsonParser::Parse(buf.data(), static_cast<int>(buf.size()));
        (void)result;
    }
    SUCCEED();
}

TEST(CommonJsonReaderFuzz, SpecificBoundaries) {
    // Empty string
    auto r1 = JsonParser::Parse("", 0);
    (void)r1;

    // Just opening brace
    auto r2 = JsonParser::Parse("{", 1);
    (void)r2;

    // Just closing brace
    auto r3 = JsonParser::Parse("}", 1);
    (void)r3;

    // Unterminated string
    auto r4 = JsonParser::Parse("\"hello", 6);
    (void)r4;

    // Trailing comma
    auto r5 = JsonParser::Parse("[1,2,]", 6);
    (void)r5;

    // Duplicate key
    auto r6 = JsonParser::Parse("{\"a\":1,\"a\":2}", 12);
    (void)r6;

    // Deeply nested
    std::string nested;
    for (int i = 0; i < 100; i++) nested += "{";
    for (int i = 0; i < 100; i++) nested += "}";
    auto r7 = JsonParser::Parse(nested.c_str(), static_cast<int>(nested.size()));
    (void)r7;

    SUCCEED();
}
