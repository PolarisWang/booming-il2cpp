// Profile test — verifies compile-time zero-overhead when CHAOS_IL2CPP_PROFILE_ENABLED=0.
// PROFILE_SCOPE, PROFILE_DUMP, PROFILE_RESET should all compile to no-ops.
#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/profile.h>

TEST(CommonProfile, MacrosCompileWithDisabledProfile) {
    // These should compile as no-ops without linking profile globals
    CHAOS_IL2CPP_PROFILE_SCOPE("test_scope");
    CHAOS_IL2CPP_PROFILE_DUMP();
    CHAOS_IL2CPP_PROFILE_RESET();
    EXPECT_TRUE(true);
}

TEST(CommonProfile, NullProfileScopeIsConstexpr) {
    // Verify NullProfileScope is usable as a constexpr expression
    constexpr chaos::il2cpp::common::NullProfileScope scope("compile_time");
    (void)scope;
    EXPECT_TRUE(true);
}
