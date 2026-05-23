/// common_config_test — Unit tests for config.h build configuration system.

#include <gtest/gtest.h>
#include <chaos/common.h>

using namespace chaos::il2cpp::common;

// ═══════════════════════════════════════════════════════════════════════════
// Build configuration tier
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonConfig, BuildModeCheck) {
    // Under test build we should always be in CHECK mode
#if defined(CHAOS_IL2CPP_CONFIG_CHECK)
    EXPECT_TRUE(true);
#else
    GTEST_FAIL() << "Expected CHAOS_IL2CPP_CONFIG_CHECK to be defined";
#endif
}

TEST(CommonConfig, ExactlyOneConfig) {
    int count = 0;
#ifdef CHAOS_IL2CPP_CONFIG_CHECK
    count++;
#endif
#ifdef CHAOS_IL2CPP_CONFIG_PROFILE
    count++;
#endif
#ifdef CHAOS_IL2CPP_CONFIG_SHIP
    count++;
#endif
    EXPECT_EQ(1, count);
}

// ═══════════════════════════════════════════════════════════════════════════
// Platform detection
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonConfig, PlatformWindows) {
#ifdef CHAOS_IL2CPP_TARGET_PLATFORM_WINDOWS
    EXPECT_TRUE(true);
#else
    GTEST_FAIL() << "Expected CHAOS_IL2CPP_TARGET_PLATFORM_WINDOWS";
#endif
}

TEST(CommonConfig, ExactlyOnePlatform) {
    int count = 0;
#ifdef CHAOS_IL2CPP_TARGET_PLATFORM_WINDOWS
    count++;
#endif
#ifdef CHAOS_IL2CPP_TARGET_PLATFORM_ANDROID
    count++;
#endif
#ifdef CHAOS_IL2CPP_TARGET_PLATFORM_IOS
    count++;
#endif
#ifdef CHAOS_IL2CPP_TARGET_PLATFORM_MACOS
    count++;
#endif
#ifdef CHAOS_IL2CPP_TARGET_PLATFORM_LINUX
    count++;
#endif
    EXPECT_EQ(1, count);
}

// ═══════════════════════════════════════════════════════════════════════════
// Exception handling mode
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonConfig, EhModeWin32Seh) {
#ifdef CHAOS_IL2CPP_EH_WIN32_SEH
    EXPECT_TRUE(true);
#else
    GTEST_FAIL() << "Expected CHAOS_IL2CPP_EH_WIN32_SEH on Windows";
#endif
}

TEST(CommonConfig, ExactlyOneEhMode) {
    int count = 0;
#ifdef CHAOS_IL2CPP_EH_SETJMP
    count++;
#endif
#ifdef CHAOS_IL2CPP_EH_CPP_THROW
    count++;
#endif
#ifdef CHAOS_IL2CPP_EH_WIN32_SEH
    count++;
#endif
    EXPECT_EQ(1, count);
}

// ═══════════════════════════════════════════════════════════════════════════
// Default log level
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonConfig, DefaultLogLevel) {
    // CHECK mode → level 3
    EXPECT_EQ(3, CHAOS_IL2CPP_DEFAULT_LOG_LEVEL);
}

// ═══════════════════════════════════════════════════════════════════════════
// Trace and profile enabled
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonConfig, TraceEnabled) {
#ifdef CHAOS_IL2CPP_TRACE_ENABLED
    EXPECT_TRUE(true);
#else
    GTEST_FAIL() << "Expected CHAOS_IL2CPP_TRACE_ENABLED in CHECK mode";
#endif
}

TEST(CommonConfig, ProfileNotEnabledInCheck) {
    // PROFILE_ENABLED should NOT be set in CHECK mode by default
#ifdef CHAOS_IL2CPP_PROFILE_ENABLED
    // This test only passes if profile is not enabled in CHECK mode
    // If CHAOS_IL2CPP_PROFILE_ENABLED was force-defined, this may fail
    GTEST_FAIL() << "CHAOS_IL2CPP_PROFILE_ENABLED should not be set in CHECK mode";
#else
    EXPECT_TRUE(true);
#endif
}

// ═══════════════════════════════════════════════════════════════════════════
// CHAOS_IL2CPP_ASSERT
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonConfig, AssertPass) {
    // Assert with true condition must not trigger
    CHAOS_IL2CPP_ASSERT(true);
    SUCCEED();
}

TEST(CommonConfig, AssertFail) {
    // Assert with false condition must abort
    EXPECT_DEATH(CHAOS_IL2CPP_ASSERT(false), "");
}
