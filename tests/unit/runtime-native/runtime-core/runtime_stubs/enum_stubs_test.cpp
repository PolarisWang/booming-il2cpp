// enum_stubs_test.cpp — Unit tests for enum_stubs.cpp (T0-3)
//
// Tests the enum stub API surface for null/zero-input safety.
// Full functional testing of enum operations requires the runtime's
// reflection system and is covered by integration tests.
//
// Links against CHAOS_COMMON_TEST_LIBS for resolution of enum_stubs
// symbols (extern "C" linkage from the prebuilt lib).

#include <gtest/gtest.h>
#include <chaos/native_types.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "runtime_stubs/stub_common.h"

using namespace chaos::il2cpp::common;

// ── Extern "C" declarations matching enum_stubs.h ──
extern "C" {
CHAOS_IL2CPP_INT32 ChaosEnumIsDefined(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosEnumGetName(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosEnumGetNames(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosEnumGetValues(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosEnumParse(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name) noexcept;
CHAOS_IL2CPP_INTPTR ChaosEnumParseWithIgnoreCase(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INT32 ignoreCase) noexcept;
CHAOS_IL2CPP_INTPTR ChaosEnumFormat(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INTPTR format) noexcept;
CHAOS_IL2CPP_INTPTR ChaosEnumToString(CHAOS_IL2CPP_INTPTR this_obj) noexcept;
CHAOS_IL2CPP_INTPTR ChaosEnumToStringRaw(CHAOS_IL2CPP_INTPTR type_handle, CHAOS_IL2CPP_INT64 raw_value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosEnumToStringWithFormat(CHAOS_IL2CPP_INTPTR this_obj, CHAOS_IL2CPP_INTPTR format_str) noexcept;
CHAOS_IL2CPP_INT32 ChaosEnumTryParse(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INTPTR result) noexcept;
CHAOS_IL2CPP_INT32 ChaosEnumTryParseWithIgnoreCase(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INT32 ignoreCase, CHAOS_IL2CPP_INTPTR result) noexcept;
}  // extern "C"

// ═══════════════════════════════════════════════════════════════════════════
// Null/zero-input safety tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(EnumStubs, IsDefined_NullType) {
    EXPECT_EQ(ChaosEnumIsDefined(0, 0), 0);
}

TEST(EnumStubs, GetName_NullType) {
    EXPECT_EQ(ChaosEnumGetName(0, 0), 0);
}

TEST(EnumStubs, GetNames_NullType) {
    EXPECT_EQ(ChaosEnumGetNames(0), 0);
}

TEST(EnumStubs, GetValues_NullType) {
    EXPECT_EQ(ChaosEnumGetValues(0), 0);
}

TEST(EnumStubs, Parse_NullType) {
    EXPECT_EQ(ChaosEnumParse(0, 0), 0);
}

TEST(EnumStubs, Parse_NullName) {
    EXPECT_EQ(ChaosEnumParse(static_cast<CHAOS_IL2CPP_INTPTR>(1), 0), 0);
}

TEST(EnumStubs, ParseWithIgnoreCase_NullType) {
    EXPECT_EQ(ChaosEnumParseWithIgnoreCase(0, 0, 0), 0);
}

TEST(EnumStubs, Format_NullType) {
    EXPECT_EQ(ChaosEnumFormat(0, 0, 0), 0);
}

TEST(EnumStubs, ToString_NullObject) {
    EXPECT_EQ(ChaosEnumToString(0), 0);
}

TEST(EnumStubs, ToStringRaw_NullHandle) {
    EXPECT_EQ(ChaosEnumToStringRaw(0, 0), 0);
}

TEST(EnumStubs, ToStringWithFormat_NullObject) {
    EXPECT_EQ(ChaosEnumToStringWithFormat(0, 0), 0);
}

TEST(EnumStubs, TryParse_NullType) {
    CHAOS_IL2CPP_INTPTR result_storage = 0;
    CHAOS_IL2CPP_INTPTR result_addr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&result_storage);
    EXPECT_EQ(ChaosEnumTryParse(0, 0, result_addr), 0);
}

TEST(EnumStubs, TryParse_NullResultOut) {
    EXPECT_EQ(ChaosEnumTryParse(static_cast<CHAOS_IL2CPP_INTPTR>(1),
                                static_cast<CHAOS_IL2CPP_INTPTR>(1), 0), 0);
}

TEST(EnumStubs, TryParseWithIgnoreCase_NullType) {
    CHAOS_IL2CPP_INTPTR result_storage = 0;
    CHAOS_IL2CPP_INTPTR result_addr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&result_storage);
    EXPECT_EQ(ChaosEnumTryParseWithIgnoreCase(0, 0, 0, result_addr), 0);
}

TEST(EnumStubs, TryParseWithIgnoreCase_NullResultOut) {
    EXPECT_EQ(ChaosEnumTryParseWithIgnoreCase(static_cast<CHAOS_IL2CPP_INTPTR>(1),
                                               static_cast<CHAOS_IL2CPP_INTPTR>(1),
                                               0, 0), 0);
}

// Note: No InvalidType tests.  enum_stubs.cpp is a full implementation that
// dereferences the type handle as a real pointer.  Passing an arbitrary
// integer (e.g. 42) causes undefined behavior (SEH access violation).
// Null-guard (type==0) coverage above is sufficient since type handles
// always come from the runtime's reflection system in production.

TEST(EnumStubs, Format_NullValue) {
    EXPECT_EQ(ChaosEnumFormat(static_cast<CHAOS_IL2CPP_INTPTR>(1), 0, 0), 0);
}
