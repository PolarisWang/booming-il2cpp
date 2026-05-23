/// common_asan_test — Compile-time tests for asan_interface.h (non-ASan path).
///
/// In the default CHECK build (ASan disabled), all AsanUnpoison/AsanPoison
/// helpers are no-ops.  These tests verify they compile and don't crash.

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/asan_interface.h>

using namespace chaos::il2cpp::common;

TEST(CommonAsan, UnpoisonNoop) {
    int x = 42;
    AsanUnpoison(&x, sizeof(x));
    SUCCEED();
}

TEST(CommonAsan, PoisonNoop) {
    int x = 42;
    AsanPoison(&x, sizeof(x));
    SUCCEED();
}

TEST(CommonAsan, ReadPtrNoCheckNoop) {
    // AsanReadPtrNoCheck reads a pointer value from addr without ASan checks.
    int value = 42;
    int* ptr = &value;
    int* result = static_cast<int*>(AsanReadPtrNoCheck(&ptr));
    EXPECT_EQ(*result, 42);
}

TEST(CommonAsan, MacroUnpoison) {
    int buf[4] = {};
    CHAOS_IL2CPP_ASAN_UNPOISON(buf, sizeof(buf));
    SUCCEED();
}

TEST(CommonAsan, MacroPoison) {
    int buf[4] = {};
    CHAOS_IL2CPP_ASAN_POISON(buf, sizeof(buf));
    SUCCEED();
}

TEST(CommonAsan, NullPtrSafe) {
    // Asan helpers accept null — they just become a no-op.
    AsanUnpoison(nullptr, 0);
    AsanPoison(nullptr, 0);
    SUCCEED();
}
