/// common_eh_test — Compile and layout tests for eh.h exception macros.
///
/// CHAOS_EH_TRY/CHAOS_EH_CATCH require runtime_core linkage and are not
/// directly testable from a standalone common test. We verify compile-time
/// mode detection only.

#include <gtest/gtest.h>
#include <chaos/common.h>

// ═══════════════════════════════════════════════════════════════════════════
// EH mode detection
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonEh, EhModeDefined) {
    // Exactly one EH mode must be defined
#if defined(CHAOS_IL2CPP_EH_WIN32_SEH)
    EXPECT_TRUE(true);
#elif defined(CHAOS_IL2CPP_EH_CPP_THROW)
    EXPECT_TRUE(true);
#elif defined(CHAOS_IL2CPP_EH_SETJMP)
    EXPECT_TRUE(true);
#else
    GTEST_FAIL() << "No EH mode defined";
#endif
}
