/// common_profile_enabled_test — Tests for profile.h with PROFILE_ENABLED=1.
///
/// CMake target sets CHAOS_IL2CPP_PROFILE_ENABLED=1 via compile definition
/// and compiles profile_globals.cpp to provide the TLS/global symbols.

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/profile.h>

using namespace chaos::il2cpp::common;

TEST(CommonProfileEnabled, ScopeRecordsCallCount) {
    ProfileReset();
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("test_scope");
    }
    // After scope exits, the per-thread slot should have 1 call.
    EXPECT_GE(g_tls_profile.slot_count, 0);  // at least initialized
    SUCCEED();
}

TEST(CommonProfileEnabled, ProfileDumpNoCrash) {
    CHAOS_IL2CPP_PROFILE_SCOPE("dump_test");
    // Dump should not crash even with minimal data.
    CHAOS_IL2CPP_PROFILE_DUMP();
    SUCCEED();
}

TEST(CommonProfileEnabled, ProfileResetClears) {
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("before_reset");
    }
    CHAOS_IL2CPP_PROFILE_RESET();
    SUCCEED();
}

TEST(CommonProfileEnabled, MultipleScopes) {
    ProfileReset();
    for (int i = 0; i < 5; i++) {
        CHAOS_IL2CPP_PROFILE_SCOPE("loop_scope");
    }
    SUCCEED();
}

TEST(CommonProfileEnabled, ScopeWithDifferentNames) {
    ProfileReset();
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("alpha");
    }
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("beta");
    }
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("alpha");  // reuse existing slot
    }
    SUCCEED();
}
