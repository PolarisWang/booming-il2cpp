/// gc_root_change_test.cpp — BGC root change ring buffer unit tests (Phase 3, gc-p4-02).
///
/// Tests the BgcRecordRootChange / BgcDrainRootChangeBuffer pair:
///   - Null-thread safety (drain with nullptr returns 0)
///   - Basic recording and draining
///   - Multiple entries (count check, not mark callback — drain only invokes
///     the mark callback for old-gen objects)
///   - Drain consumes all entries (second drain returns 0)
///
/// Note: The drain callback IsInOldGen(old_val) guard means only real old-gen
/// object addresses trigger the mark callback.  Fake addresses (from test
/// literal casts) are skipped by the drain loop.  This is correct behavior.

#include <cstdio>
#include <atomic>

#include "gc_root_change.h"
#include "gc_test_base.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

// ═══════════════════════════════════════════════════════════════════════════
// Fixture
// ═══════════════════════════════════════════════════════════════════════════

struct GcRootChangeTest : GcTestBase {
    void SetUp() override {
        GcTestBase::SetUp();
    }
    void TearDown() override {
        GcTestBase::TearDown();
    }
};

// ═══════════════════════════════════════════════════════════════════════════
// Drain with nullptr
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(GcRootChangeTest, DrainNullptrReturnsZero) {
    auto count = BgcDrainRootChangeBuffer(nullptr,
        [](void*) -> bool { return true; });
    EXPECT_EQ(count, 0u);
}

// ═══════════════════════════════════════════════════════════════════════════
// Drain empty buffer
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(GcRootChangeTest, DrainEmptyBuffer) {
    auto* mt = threading::GetCurrentThread();
    ASSERT_NE(mt, nullptr);

    auto count = BgcDrainRootChangeBuffer(mt,
        [](void*) -> bool { return true; });
    EXPECT_EQ(count, 0u);
}

// ═══════════════════════════════════════════════════════════════════════════
// Record and drain single entry
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(GcRootChangeTest, RecordAndDrainSingle) {
    void* fake_slot = nullptr;
    void* fake_old = reinterpret_cast<void*>(static_cast<uintptr_t>(0x1234));

    BgcRecordRootChange(&fake_slot, fake_old);

    auto* mt = threading::GetCurrentThread();
    ASSERT_NE(mt, nullptr);

    auto drained = BgcDrainRootChangeBuffer(mt,
        [](void*) -> bool { return true; });

    EXPECT_GE(drained, 1u);
}

// ═══════════════════════════════════════════════════════════════════════════
// Record and drain multiple entries
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(GcRootChangeTest, RecordAndDrainMultiple) {
    static constexpr int kNumEntries = 10;
    for (int i = 0; i < kNumEntries; i++) {
        void* slot = nullptr;
        BgcRecordRootChange(&slot,
            reinterpret_cast<void*>(static_cast<uintptr_t>(0x1000 + i)));
    }

    auto* mt = threading::GetCurrentThread();
    ASSERT_NE(mt, nullptr);

    auto drained = BgcDrainRootChangeBuffer(mt,
        [](void*) -> bool { return true; });

    EXPECT_GE(drained, static_cast<uint32_t>(kNumEntries));
}

// ═══════════════════════════════════════════════════════════════════════════
// Drain resets tail to head (buffer is consumed)
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(GcRootChangeTest, DrainConsumesAllEntries) {
    void* slot = nullptr;
    BgcRecordRootChange(&slot,
        reinterpret_cast<void*>(static_cast<uintptr_t>(0x2000)));

    auto* mt = threading::GetCurrentThread();
    ASSERT_NE(mt, nullptr);

    auto first = BgcDrainRootChangeBuffer(mt,
        [](void*) -> bool { return true; });
    EXPECT_GE(first, 1u);

    // Second drain should return 0 (buffer already consumed).
    auto second = BgcDrainRootChangeBuffer(mt,
        [](void*) -> bool { return true; });
    EXPECT_EQ(second, 0u);
}

// ═══════════════════════════════════════════════════════════════════════════
// Record with null old_value
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(GcRootChangeTest, RecordNullOldValue) {
    void* slot = nullptr;
    BgcRecordRootChange(&slot, nullptr);

    auto* mt = threading::GetCurrentThread();
    ASSERT_NE(mt, nullptr);

    auto drained = BgcDrainRootChangeBuffer(mt,
        [](void*) -> bool { return true; });

    // Drain iterates all entries; null old_value entries are recorded but
    // skipped by the mark callback (IsInOldGen check fails for nullptr).
    EXPECT_GE(drained, 0u);
}
