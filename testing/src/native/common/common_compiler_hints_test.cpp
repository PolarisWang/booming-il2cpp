/// common_compiler_hints_test — Compile tests for compiler_hints.h macros.

#include <gtest/gtest.h>
#include <chaos/common.h>

// ═══════════════════════════════════════════════════════════════════════════
// CHAOS_IL2CPP_FORCEINLINE — verify it compiles on a test function
// ═══════════════════════════════════════════════════════════════════════════

CHAOS_IL2CPP_FORCEINLINE int force_inline_fn(int x) { return x * 2; }

TEST(CommonCompilerHints, ForceInline) {
    EXPECT_EQ(4, force_inline_fn(2));
}

// ═══════════════════════════════════════════════════════════════════════════
// CHAOS_IL2CPP_LIKELY / CHAOS_IL2CPP_UNLIKELY
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonCompilerHints, Likely) {
    int x = 42;
    if (CHAOS_IL2CPP_LIKELY(x > 0)) {
        SUCCEED();
    } else {
        GTEST_FAIL() << "LIKELY branch taken incorrectly";
    }
}

TEST(CommonCompilerHints, Unlikely) {
    int x = -1;
    if (CHAOS_IL2CPP_UNLIKELY(x < 0)) {
        SUCCEED();
    } else {
        GTEST_FAIL() << "UNLIKELY branch taken incorrectly";
    }
}

TEST(CommonCompilerHints, LikelyFalse) {
    int x = 0;
    if (CHAOS_IL2CPP_LIKELY(x != 0)) {
        GTEST_FAIL() << "LIKELY was false but branch still taken";
    } else {
        SUCCEED();
    }
}

TEST(CommonCompilerHints, UnlikelyFalse) {
    int x = 1;
    if (CHAOS_IL2CPP_UNLIKELY(x == 0)) {
        GTEST_FAIL() << "UNLIKELY was false but branch still taken";
    } else {
        SUCCEED();
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// CHAOS_IL2CPP_RESTRICT — verify it compiles
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonCompilerHints, RestrictCompiles) {
    int val = 42;
    int* CHAOS_IL2CPP_RESTRICT ptr = &val;
    EXPECT_EQ(42, *ptr);
}

// ═══════════════════════════════════════════════════════════════════════════
// Macro is defined check
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonCompilerHints, MacrosDefined) {
#ifdef CHAOS_IL2CPP_FORCEINLINE
    EXPECT_TRUE(true);
#else
    GTEST_FAIL() << "CHAOS_IL2CPP_FORCEINLINE not defined";
#endif
#ifdef CHAOS_IL2CPP_LIKELY
    EXPECT_TRUE(true);
#else
    GTEST_FAIL() << "CHAOS_IL2CPP_LIKELY not defined";
#endif
#ifdef CHAOS_IL2CPP_UNLIKELY
    EXPECT_TRUE(true);
#else
    GTEST_FAIL() << "CHAOS_IL2CPP_UNLIKELY not defined";
#endif
#ifdef CHAOS_IL2CPP_RESTRICT
    EXPECT_TRUE(true);
#else
    GTEST_FAIL() << "CHAOS_IL2CPP_RESTRICT not defined";
#endif
}
