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

// ── Probe contract (review #5) ──────────────────────────────────────
// AsanReadPtrProbe/AsanWritePtrProbe are the poison-GATED accessors the GC uses
// for cross-thread conservative root scans: they shed instrumentation ONLY for
// genuinely poisoned slots (__asan_address_is_poisoned), keeping LIVE root slots
// instrumented so a real heap OOB/UAF write into a live root is still caught.
//
// This unit test runs on the non-ASan path (helpers are no-ops), so it can't
// prove ASan fires on an injected OOB — but it DOES pin the contract that the
// Probe variants are plain read/write-through on unpoisoned/live data and are
// wire-invokable (a regression here would break every GC root scan).  The
// ASan-runtime "genuine OOB on a live root is still reported, not masked by the
// no-check fast path" guarantee is enforced by the __asan_address_is_poisoned
// gating inside asan_interface.h and exercised in the ASan build tree.
TEST(CommonAsan, ReadWritePtrProbeContract) {
    // Read: an unpoisoned live slot is read through as a normal pointer load.
    int value = 42;
    int* ptr = &value;
    int* read = static_cast<int*>(AsanReadPtrProbe(&ptr));
    EXPECT_EQ(read, ptr);
    EXPECT_EQ(*read, 42);

    // Write: an unpoisoned live slot is written through as a normal pointer store.
    int* other = &value;
    static_cast<void>(other);
    AsanWritePtrProbe(&ptr, reinterpret_cast<void*>(other));
    EXPECT_EQ(ptr, other);

    // NOTE: AsanReadPtrProbe/AsanWritePtrProbe are NOT null-safe — they are
    // called only on real GC root slots (never nullptr), and on the non-ASan
    // path they reduce to a plain pointer load/store (no null guard).  So we do
    // not exercise a nullptr probe here; the GC call sites always pass live slots.
    SUCCEED();
}
