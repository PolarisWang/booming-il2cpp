#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/json_reader.h>
#include <cstring>

using namespace chaos::il2cpp::json;

TEST(CommonJsonReader, ParseNull) {
    auto result = JsonParser::Parse("null", 4);
    EXPECT_TRUE(result.value.IsNull());
    EXPECT_EQ(nullptr, result.error);
}

TEST(CommonJsonReader, ParseTrue) {
    auto result = JsonParser::Parse("true", 4);
    EXPECT_TRUE(result.value.IsBool());
    EXPECT_TRUE(result.value.AsBool());
    EXPECT_EQ(nullptr, result.error);
}

TEST(CommonJsonReader, ParseFalse) {
    auto result = JsonParser::Parse("false", 5);
    EXPECT_TRUE(result.value.IsBool());
    EXPECT_FALSE(result.value.AsBool());
}

TEST(CommonJsonReader, ParseInt) {
    auto result = JsonParser::Parse("42", 2);
    EXPECT_TRUE(result.value.IsInt64());
    EXPECT_TRUE(result.value.AsInt64() == 42);
}

TEST(CommonJsonReader, ParseNegativeInt) {
    auto result = JsonParser::Parse("-123", 4);
    EXPECT_TRUE(result.value.IsInt64());
    EXPECT_TRUE(result.value.AsInt64() == -123);
}

TEST(CommonJsonReader, ParseDouble) {
    auto result = JsonParser::Parse("3.14", 4);
    EXPECT_TRUE(result.value.IsDouble());
    EXPECT_DOUBLE_EQ(3.14, result.value.AsDouble());
}

TEST(CommonJsonReader, ParseScientificDouble) {
    auto result = JsonParser::Parse("1.5e2", 5);
    EXPECT_TRUE(result.value.IsDouble());
    EXPECT_DOUBLE_EQ(150.0, result.value.AsDouble());
}

TEST(CommonJsonReader, ParseString) {
    auto result = JsonParser::Parse("\"hello\"", 7);
    EXPECT_TRUE(result.value.IsString());
    EXPECT_TRUE(result.value.string_length == 5);
    EXPECT_EQ(0, std::memcmp("hello", result.value.string_value, 5));
}

TEST(CommonJsonReader, ParseStringWithEscapes) {
    auto result = JsonParser::Parse("\"he\\nlo\"", 8);
    EXPECT_TRUE(result.value.IsString());
    EXPECT_TRUE(result.value.string_length == 5);
    EXPECT_EQ('h', result.value.string_value[0]);
    EXPECT_EQ('\n', result.value.string_value[2]);
    EXPECT_TRUE(result.value.owns_string);
    JsonFreeString(result.value);
}

TEST(CommonJsonReader, ParseEmptyObject) {
    auto result = JsonParser::Parse("{}", 2);
    EXPECT_TRUE(result.value.IsObject());
    EXPECT_EQ(nullptr, result.error);
}

TEST(CommonJsonReader, ParseEmptyArray) {
    auto result = JsonParser::Parse("[]", 2);
    EXPECT_TRUE(result.value.IsArray());
}

TEST(CommonJsonReader, ParseSimpleObject) {
    const char* json = R"({"key": "value"})";
    auto result = JsonParser::Parse(json, std::strlen(json));
    EXPECT_TRUE(result.value.IsObject());
    EXPECT_EQ(nullptr, result.error);

    auto found = JsonParser::FindKey(result.value, "key");
    EXPECT_TRUE(found.IsString());
    EXPECT_EQ(0, std::memcmp("value", found.string_value, 5));
}

TEST(CommonJsonReader, ParseObjectFindMissingKey) {
    const char* json = R"({"a": 1})";
    auto result = JsonParser::Parse(json, std::strlen(json));
    auto found = JsonParser::FindKey(result.value, "nonexistent");
    EXPECT_TRUE(found.IsNull());
}

TEST(CommonJsonReader, ParseSimpleArray) {
    const char* json = R"([1, 2, 3])";
    auto result = JsonParser::Parse(json, std::strlen(json));
    EXPECT_TRUE(result.value.IsArray());

    int count = 0;
    int sum = 0;
    JsonParser::ForEachArrayElement(result.value, [&](size_t, const JsonValue& elem) {
        ++count;
        sum += static_cast<int>(elem.AsInt64());
        return true;
    });
    EXPECT_TRUE(count == 3);
    EXPECT_TRUE(sum == 6);
}

TEST(CommonJsonReader, ParseNestedObject) {
    const char* json = R"({"outer": {"inner": 42}})";
    auto result = JsonParser::Parse(json, std::strlen(json));
    EXPECT_TRUE(result.value.IsObject());

    auto outer = JsonParser::FindKey(result.value, "outer");
    EXPECT_TRUE(outer.IsObject());
    auto inner = JsonParser::FindKey(outer, "inner");
    EXPECT_TRUE(inner.IsInt64());
    EXPECT_TRUE(inner.AsInt64() == 42);
}

TEST(CommonJsonReader, ParseUnterminatedString) {
    auto result = JsonParser::Parse("\"unterminated", 13);
    EXPECT_NE(nullptr, result.error);
}

TEST(CommonJsonReader, ParseTrailingCommaObject) {
    const char* json = R"({"a": 1,})";
    auto result = JsonParser::Parse(json, std::strlen(json));
    EXPECT_TRUE(result.value.IsObject());
}

TEST(CommonJsonReader, ForEachArrayElementNonArray) {
    JsonValue not_array;
    int count = 0;
    bool ok = JsonParser::ForEachArrayElement(not_array, [&](size_t, const JsonValue&) {
        ++count;
        return true;
    });
    EXPECT_FALSE(ok);
    EXPECT_TRUE(count == 0);
}

TEST(CommonJsonReader, ForEachArrayElementEmptyArray) {
    const char* json = "[]";
    auto result = JsonParser::Parse(json, 2);
    int count = 0;
    JsonParser::ForEachArrayElement(result.value, [&](size_t, const JsonValue&) {
        ++count;
        return true;
    });
    EXPECT_TRUE(count == 0);
}

TEST(CommonJsonReader, FindKeyNonObject) {
    JsonValue not_object;
    auto found = JsonParser::FindKey(not_object, "key");
    EXPECT_TRUE(found.IsNull());
}
