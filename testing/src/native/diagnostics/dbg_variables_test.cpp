// dbg_variables_test.cpp — Variable inspection unit tests
//
// Tests DbgGetScopes, DbgGetVariables, TagToTypeName, and FormatValue.
// Without a real runtime snapshot, scope/variable queries verify
// empty-state and edge-case behavior.
// Links chaos_debugger for the variables implementation.

#include <gtest/gtest.h>
#include <debugger/dbg_variables.h>
#include <debugger/dbg_runtime.h>

#include <cstring>

namespace chaos::il2cpp::diagnostics {
namespace {

class VariablesTest : public ::testing::Test {
protected:
    void SetUp() override {
        DbgVariablesInitialize();
        DbgClearFrameSnapshot();
    }
    void TearDown() override {
        DbgVariablesShutdown();
    }
};

TEST_F(VariablesTest, EmptyScopes) {
    auto scopes = DbgGetScopes(0);
    EXPECT_TRUE(scopes.empty());
}

TEST_F(VariablesTest, NegativeFrameIdReturnsEmpty) {
    auto scopes = DbgGetScopes(-1);
    EXPECT_TRUE(scopes.empty());
}

TEST_F(VariablesTest, EmptyVariables) {
    auto vars = DbgGetVariables(1);
    EXPECT_TRUE(vars.empty());
}

TEST_F(VariablesTest, NonExistentReferenceReturnsEmpty) {
    auto vars = DbgGetVariables(999);
    EXPECT_TRUE(vars.empty());
}

// ── TagToTypeName tests ───────────────────────────────────────────────

TEST_F(VariablesTest, TagToTypeNameAllTags) {
    EXPECT_STREQ(TagToTypeName(0), "unknown");
    EXPECT_STREQ(TagToTypeName(1), "int32");
    EXPECT_STREQ(TagToTypeName(2), "int64");
    EXPECT_STREQ(TagToTypeName(3), "float");
    EXPECT_STREQ(TagToTypeName(4), "double");
    EXPECT_STREQ(TagToTypeName(5), "object");
}

TEST_F(VariablesTest, TagToTypeNameUnknown) {
    EXPECT_STREQ(TagToTypeName(99), "unknown");
    EXPECT_STREQ(TagToTypeName(255), "unknown");
}

TEST_F(VariablesTest, TagToTypeNameNullTag) {
    EXPECT_STREQ(TagToTypeName(0), "unknown");
}

// ── FormatValue tests ─────────────────────────────────────────────────

TEST_F(VariablesTest, FormatValueInt32) {
    char buf[64];
    FormatValue(42, 1, buf, sizeof(buf));
    EXPECT_STREQ(buf, "42");

    FormatValue(static_cast<uint64_t>(-1), 1, buf, sizeof(buf));
    EXPECT_STREQ(buf, "-1");
}

TEST_F(VariablesTest, FormatValueInt64) {
    char buf[64];
    int64_t val = -1234567890123LL;
    uint64_t raw;
    std::memcpy(&raw, &val, sizeof(raw));
    FormatValue(raw, 2, buf, sizeof(buf));
    EXPECT_STREQ(buf, "-1234567890123");
}

TEST_F(VariablesTest, FormatValueFloat) {
    char buf[64];
    float f = 3.14f;
    uint64_t raw = 0;
    std::memcpy(&raw, &f, sizeof(f));
    FormatValue(raw, 3, buf, sizeof(buf));
    EXPECT_NE(strstr(buf, "3.14"), nullptr);
}

TEST_F(VariablesTest, FormatValueDouble) {
    char buf[64];
    double d = 2.71828;
    uint64_t raw = 0;
    std::memcpy(&raw, &d, sizeof(d));
    FormatValue(raw, 4, buf, sizeof(buf));
    EXPECT_NE(strstr(buf, "2.71828"), nullptr);
}

TEST_F(VariablesTest, FormatValueObject) {
    char buf[64];
    FormatValue(0x12345678, 5, buf, sizeof(buf));
    EXPECT_NE(strstr(buf, "0x"), nullptr);
}

TEST_F(VariablesTest, FormatValueNullObject) {
    char buf[64];
    FormatValue(0, 5, buf, sizeof(buf));
    EXPECT_NE(strstr(buf, "0x0"), nullptr);
}

TEST_F(VariablesTest, FormatValueUnknownTag) {
    char buf[64];
    FormatValue(0xABCD, 99, buf, sizeof(buf));
    EXPECT_NE(strstr(buf, "abcd"), nullptr);
}

TEST_F(VariablesTest, FormatValueSmallBuffer) {
    char buf[4];
    FormatValue(42, 1, buf, sizeof(buf));
    // Buffer is too small for "-1\0" — just ensure no crash.
    buf[3] = '\0';
}

TEST_F(VariablesTest, FormatValueFloatZero) {
    char buf[64];
    float f = 0.0f;
    uint64_t raw = 0;
    std::memcpy(&raw, &f, sizeof(f));
    FormatValue(raw, 3, buf, sizeof(buf));
    EXPECT_NE(strstr(buf, "0"), nullptr);
}

TEST_F(VariablesTest, FormatValueLargeInt64) {
    char buf[64];
    int64_t val = 9223372036854775807LL;  // INT64_MAX
    uint64_t raw;
    std::memcpy(&raw, &val, sizeof(raw));
    FormatValue(raw, 2, buf, sizeof(buf));
    EXPECT_STREQ(buf, "9223372036854775807");
}

// ── Scope with populated snapshot ─────────────────────────────────────

TEST_F(VariablesTest, ScopesWithValidFrameId) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 2;
    snap.frames[0] = {0x06000001, 0};
    snap.frames[1] = {0x06000002, 10};

    auto scopes = DbgGetScopes(0);
    ASSERT_EQ(scopes.size(), 1u);
    EXPECT_EQ(scopes[0].name, "Locals");
    EXPECT_EQ(scopes[0].variables_reference, 1);

    DbgClearFrameSnapshot();
}

// ── FormatValue edge cases ───────────────────────────────────────────────

TEST_F(VariablesTest, FormatValueFloatNegative) {
    char buf[64];
    float f = -3.14f;
    uint64_t raw = 0;
    std::memcpy(&raw, &f, sizeof(f));
    FormatValue(raw, 3, buf, sizeof(buf));
    EXPECT_NE(strstr(buf, "-3.14"), nullptr);
}

TEST_F(VariablesTest, FormatValueDoubleNegative) {
    char buf[64];
    double d = -2.71828;
    uint64_t raw = 0;
    std::memcpy(&raw, &d, sizeof(d));
    FormatValue(raw, 4, buf, sizeof(buf));
    EXPECT_NE(strstr(buf, "-2.71828"), nullptr);
}

TEST_F(VariablesTest, FormatValueFloatLarge) {
    char buf[64];
    float f = 1.0e10f;
    uint64_t raw = 0;
    std::memcpy(&raw, &f, sizeof(f));
    FormatValue(raw, 3, buf, sizeof(buf));
    EXPECT_GT(strlen(buf), 0u);
}

TEST_F(VariablesTest, FormatValueInt32Max) {
    char buf[64];
    int32_t val = INT32_MAX;
    uint64_t raw;
    std::memcpy(&raw, &val, sizeof(val));
    FormatValue(raw, 1, buf, sizeof(buf));
    EXPECT_STREQ(buf, "2147483647");
}

TEST_F(VariablesTest, FormatValueInt32Min) {
    char buf[64];
    int32_t val = INT32_MIN;
    uint64_t raw;
    std::memcpy(&raw, &val, sizeof(val));
    FormatValue(raw, 1, buf, sizeof(buf));
    EXPECT_STREQ(buf, "-2147483648");
}

TEST_F(VariablesTest, FormatValueObjectNonZero) {
    char buf[64];
    FormatValue(0xDEADBEEF, 5, buf, sizeof(buf));
    // Format is 0x%p → uppercase hex on MSVC, full pointer width.
    EXPECT_NE(strstr(buf, "0x"), nullptr);
    // Should contain some hex digits (could be uppercase on MSVC).
    EXPECT_GT(strlen(buf), 2u);
}

TEST_F(VariablesTest, FormatValueObjectLargeAddress) {
    char buf[64];
    FormatValue(static_cast<uint64_t>(0x7FFE12345678), 5, buf, sizeof(buf));
    EXPECT_NE(strstr(buf, "0x"), nullptr);
}

// ── Snapshot with populated locals ──────────────────────────────────

TEST_F(VariablesTest, ScopesWithMultipleFrames) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 3;
    snap.frames[0] = {0x06000001, 0};
    snap.frames[1] = {0x06000002, 10};
    snap.frames[2] = {0x06000003, 20};

    // Each valid frame should return a scope.
    auto scopes0 = DbgGetScopes(0);
    ASSERT_EQ(scopes0.size(), 1u);

    auto scopes1 = DbgGetScopes(1);
    ASSERT_EQ(scopes1.size(), 1u);

    auto scopes2 = DbgGetScopes(2);
    ASSERT_EQ(scopes2.size(), 1u);

    DbgClearFrameSnapshot();
}

TEST_F(VariablesTest, ScopesFrameIdBeyondSnapshot) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 1;
    snap.frames[0] = {0x06000001, 0};

    // Frame ID 5 is beyond snapshot.
    auto scopes = DbgGetScopes(5);
    EXPECT_TRUE(scopes.empty());

    DbgClearFrameSnapshot();
}

TEST_F(VariablesTest, VariablesWithPopulatedLocals) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 1;
    snap.frames[0] = {0x06000001, 0};

    snap.local_count = 3;
    snap.locals[0] = {0, 42, 1};      // int32
    snap.locals[1] = {1, static_cast<uint64_t>(-1LL), 2};  // int64
    snap.locals[2] = {2, 0, 5};        // null object

    auto vars = DbgGetVariables(1);
    ASSERT_EQ(vars.size(), 3u);

    EXPECT_EQ(vars[0].name, "local_0");
    EXPECT_EQ(vars[0].value, "42");
    EXPECT_EQ(vars[0].type, "int32");
    EXPECT_EQ(vars[0].variables_reference, 0);

    EXPECT_EQ(vars[1].name, "local_1");
    EXPECT_EQ(vars[1].value, "-1");
    EXPECT_EQ(vars[1].type, "int64");

    EXPECT_EQ(vars[2].name, "local_2");
    // tag 5 (object) with value 0: format is "0x%p" → full-width 0x on x64
    EXPECT_NE(vars[2].value.find("0x"), std::string::npos);
    EXPECT_EQ(vars[2].type, "object");

    DbgClearFrameSnapshot();
}

TEST_F(VariablesTest, VariablesWithFloatAndDoubleLocals) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 1;
    snap.frames[0] = {0x06000001, 0};

    snap.local_count = 2;

    float f = 1.5f;
    uint64_t f_raw = 0;
    std::memcpy(&f_raw, &f, sizeof(f));
    snap.locals[0] = {0, f_raw, 3};  // float

    double d = 3.14159;
    uint64_t d_raw = 0;
    std::memcpy(&d_raw, &d, sizeof(d));
    snap.locals[1] = {1, d_raw, 4};  // double

    auto vars = DbgGetVariables(1);
    ASSERT_EQ(vars.size(), 2u);

    EXPECT_NE(vars[0].value.find("1.5"), std::string::npos);
    EXPECT_EQ(vars[0].type, "float");

    EXPECT_NE(vars[1].value.find("3.14159"), std::string::npos);
    EXPECT_EQ(vars[1].type, "double");

    DbgClearFrameSnapshot();
}

TEST_F(VariablesTest, VariablesWithObjectReference) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 1;
    snap.frames[0] = {0x06000001, 0};

    snap.local_count = 1;
    snap.locals[0] = {0, 0x12345678, 5};  // object with non-zero address

    auto vars = DbgGetVariables(1);
    ASSERT_EQ(vars.size(), 1u);
    EXPECT_EQ(vars[0].type, "object");
    EXPECT_GT(vars[0].variables_reference, 0);  // should have child reference

    DbgClearFrameSnapshot();
}

TEST_F(VariablesTest, VariablesZeroLocals) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 1;
    snap.frames[0] = {0x06000001, 0};
    snap.local_count = 0;  // no locals

    auto vars = DbgGetVariables(1);
    EXPECT_TRUE(vars.empty());

    DbgClearFrameSnapshot();
}

TEST_F(VariablesTest, VariablesReferenceAfterFrameClear) {
    // Get an object reference first.
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 1;
    snap.frames[0] = {0x06000001, 0};
    snap.local_count = 1;
    snap.locals[0] = {0, 0x12345678, 5};

    auto vars = DbgGetVariables(1);
    ASSERT_EQ(vars.size(), 1u);
    int obj_ref = vars[0].variables_reference;
    EXPECT_GT(obj_ref, 0);

    // Clear snapshot — variable references remain in the registry.
    DbgClearFrameSnapshot();
    // Merely verify the ref ID was assigned without trying to expand it.
    EXPECT_GT(obj_ref, 0);
}

TEST_F(VariablesTest, NegativeVariablesReference) {
    auto vars = DbgGetVariables(-1);
    EXPECT_TRUE(vars.empty());
}

TEST_F(VariablesTest, VariablesReferenceZero) {
    auto vars = DbgGetVariables(0);
    EXPECT_TRUE(vars.empty());
}

TEST_F(VariablesTest, VariablesReferenceLargeNumber) {
    auto vars = DbgGetVariables(999999);
    EXPECT_TRUE(vars.empty());
}


}  // namespace
}  // namespace chaos::il2cpp::diagnostics