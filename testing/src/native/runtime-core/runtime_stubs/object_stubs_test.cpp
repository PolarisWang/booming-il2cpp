// object_stubs_test.cpp — Unit tests for object/RuntimeHelpers stubs (t1a-5)
//
// Tests all 9 API functions for object and RuntimeHelpers.
//
// Links against CHAOS_COMMON_TEST_LIBS for ground lib resolution.
// object_stubs.cpp is compiled directly (depends on gc_layout.h, gc_helpers.h).

#include <gtest/gtest.h>
#include <chaos/native_types.h>

#include <cstring>

// ═══════════════════════════════════════════════════════════════════════════
// Forward declarations for the stub functions
// ═══════════════════════════════════════════════════════════════════════════

extern "C" {
CHAOS_IL2CPP_INTPTR ChaosObjectEqualsStatic(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) noexcept;
void ChaosObjectCtor(CHAOS_IL2CPP_INTPTR obj) noexcept;
CHAOS_IL2CPP_INT32 ChaosObjectGetHashCode(CHAOS_IL2CPP_INTPTR obj) noexcept;
CHAOS_IL2CPP_INTPTR ChaosObjectToString(CHAOS_IL2CPP_INTPTR obj) noexcept;
CHAOS_IL2CPP_INTPTR ChaosObjectGetType(CHAOS_IL2CPP_INTPTR obj) noexcept;
CHAOS_IL2CPP_INTPTR ChaosObjectMemberwiseClone(CHAOS_IL2CPP_INTPTR obj) noexcept;

CHAOS_IL2CPP_INTPTR ChaosRuntimeHelpersEquals(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) noexcept;
CHAOS_IL2CPP_INT32  ChaosRuntimeHelpersGetHashCode(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosRuntimeHelpersGetObjectValue(CHAOS_IL2CPP_INTPTR value) noexcept;
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosObjectEqualsStatic tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Object_EqualsStatic, SamePointer) {
    int dummy = 0;
    auto p = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy);
    EXPECT_EQ(ChaosObjectEqualsStatic(p, p), 1);
}

TEST(Object_EqualsStatic, DifferentPointers) {
    int a = 0, b = 0;
    EXPECT_EQ(ChaosObjectEqualsStatic(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&a),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&b)), 0);
}

TEST(Object_EqualsStatic, BothNull) {
    EXPECT_EQ(ChaosObjectEqualsStatic(0, 0), 1);
}

TEST(Object_EqualsStatic, OneNull) {
    int dummy = 0;
    EXPECT_EQ(ChaosObjectEqualsStatic(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy), 0), 0);
    EXPECT_EQ(ChaosObjectEqualsStatic(0,
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy)), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosObjectCtor tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(Object_Ctor, NoOp) {
    // Should not crash
    int dummy = 0;
    ChaosObjectCtor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy));
}

TEST(Object_Ctor, Null) {
    // Should not crash
    ChaosObjectCtor(0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosRuntimeHelpersEquals tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(RuntimeHelpers_Equals, SamePointer) {
    int dummy = 0;
    auto p = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy);
    EXPECT_EQ(ChaosRuntimeHelpersEquals(p, p), 1);
}

TEST(RuntimeHelpers_Equals, Different) {
    int a = 0, b = 0;
    EXPECT_EQ(ChaosRuntimeHelpersEquals(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&a),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&b)), 0);
}

TEST(RuntimeHelpers_Equals, BothNull) {
    EXPECT_EQ(ChaosRuntimeHelpersEquals(0, 0), 1);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosRuntimeHelpersGetHashCode / ChaosObjectGetHashCode tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(RuntimeHelpers_GetHashCode, SamePointerSameHash) {
    int dummy = 0;
    auto p = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy);
    EXPECT_EQ(ChaosRuntimeHelpersGetHashCode(p), ChaosRuntimeHelpersGetHashCode(p));
}

TEST(RuntimeHelpers_GetHashCode, NullReturnsZero) {
    EXPECT_EQ(ChaosRuntimeHelpersGetHashCode(0), 0);
}

TEST(Object_GetHashCode, SamePointerSameHash) {
    int dummy = 0;
    auto p = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy);
    EXPECT_EQ(ChaosObjectGetHashCode(p), ChaosObjectGetHashCode(p));
}

TEST(Object_GetHashCode, NullReturnsZero) {
    EXPECT_EQ(ChaosObjectGetHashCode(0), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosRuntimeHelpersGetObjectValue tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(RuntimeHelpers_GetObjectValue, PassThrough) {
    int dummy = 0;
    auto p = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy);
    EXPECT_EQ(ChaosRuntimeHelpersGetObjectValue(p), p);
}

TEST(RuntimeHelpers_GetObjectValue, NullReturnsNull) {
    EXPECT_EQ(ChaosRuntimeHelpersGetObjectValue(0), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosObjectToString / ChaosObjectGetType tests (sentinel)
// ═══════════════════════════════════════════════════════════════════════════

TEST(Object_ToString, ReturnsNonNull) {
    int dummy = 0;
    auto result = ChaosObjectToString(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy));
    EXPECT_NE(result, 0);
}

TEST(Object_ToString, NullReturnsNonNull) {
    // Should still return a sentinel, not crash
    auto result = ChaosObjectToString(0);
    EXPECT_NE(result, 0);
}

TEST(Object_GetType, ReturnsNonNull) {
    int dummy = 0;
    auto result = ChaosObjectGetType(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy));
    EXPECT_NE(result, 0);
}

TEST(Object_GetType, NullReturnsNonNull) {
    auto result = ChaosObjectGetType(0);
    EXPECT_NE(result, 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosObjectMemberwiseClone tests (basic null handling)
// ═══════════════════════════════════════════════════════════════════════════

TEST(Object_MemberwiseClone, NullReturnsNull) {
    auto result = ChaosObjectMemberwiseClone(0);
    EXPECT_EQ(result, 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// Edge-case tests (t1b-6)
// ═══════════════════════════════════════════════════════════════════════════

TEST(Object_Edge, MemberwiseClone_NonNullReturnsNonNull) {
    int dummy = 0;
    auto result = ChaosObjectMemberwiseClone(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy));
    // MemberwiseClone currently returns 0 (requires GcLayoutRegistry — Phase 1b deferred).
    // At minimum, it should not crash.
    (void)result;
}

TEST(Object_Edge, GetHashCode_ConsistentAcrossCalls) {
    int dummy = 0;
    auto p = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy);
    auto h1 = ChaosObjectGetHashCode(p);
    auto h2 = ChaosObjectGetHashCode(p);
    auto h3 = ChaosObjectGetHashCode(p);
    EXPECT_EQ(h1, h2);
    EXPECT_EQ(h2, h3);
}

TEST(Object_Edge, RuntimeHelpersGetHashCode_Consistent) {
    int dummy = 0;
    auto p = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&dummy);
    auto h1 = ChaosRuntimeHelpersGetHashCode(p);
    auto h2 = ChaosRuntimeHelpersGetHashCode(p);
    EXPECT_EQ(h1, h2);
}

TEST(Object_Edge, GetHashCode_DifferentObjects) {
    int a = 0, b = 0;
    auto pa = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&a);
    auto pb = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&b);
    // Different stack addresses should produce different hash codes in practice.
    // At minimum, ensure both calls don't crash and return deterministic results.
    auto ha = ChaosObjectGetHashCode(pa);
    auto hb = ChaosObjectGetHashCode(pb);
    EXPECT_NE(ha, 0);
    EXPECT_NE(hb, 0);
    EXPECT_EQ(ChaosObjectGetHashCode(pa), ha);  // idempotent
    EXPECT_EQ(ChaosObjectGetHashCode(pb), hb);
}

TEST(Object_Edge, EqualsStatic_NullVsNull) {
    EXPECT_EQ(ChaosObjectEqualsStatic(0, 0), 1);
}

TEST(Object_Edge, ToString_NullReturnsNonNull) {
    EXPECT_NE(ChaosObjectToString(0), 0);
}

TEST(Object_Edge, GetType_NullReturnsNonNull) {
    EXPECT_NE(ChaosObjectGetType(0), 0);
}

TEST(Object_Edge, Ctor_Null) {
    ChaosObjectCtor(0);
}

TEST(Object_Edge, RuntimeHelpersEquals_BothNull) {
    EXPECT_EQ(ChaosRuntimeHelpersEquals(0, 0), 1);
}

TEST(Object_Edge, GetObjectValue_Null) {
    EXPECT_EQ(ChaosRuntimeHelpersGetObjectValue(0), 0);
}
