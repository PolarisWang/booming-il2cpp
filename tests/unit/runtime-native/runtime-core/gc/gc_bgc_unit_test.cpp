/// gc_bgc_unit_test.cpp — BGC subsystem unit tests (GoogleTest).
///
/// Tests SATB buffer lifecycle, BgcController phase transitions,
/// and worker deque operations.  These do NOT require a full GC
/// initialization — they test BGC internals in isolation.

#include <cstdint>
#include <cstring>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include "gc_bgc.h"
#include "gc_bgc_inline.h"
#include "gc_layout.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "thread_state.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

namespace {

// ── BgcUnitTest fixture ───────────────────────────────────────
// Minimal setup: just enough to test BGC internals.
// Does NOT start the BGC thread or initialize old-gen.

struct BgcUnitTest : ::testing::Test {
    void SetUp() override {
        auto& ctrl = BgcController::Instance();
        ctrl.ResetForTest();
    }

    void TearDown() override {
        auto& ctrl = BgcController::Instance();
        ctrl.ResetForTest();
    }
};

}  // anonymous namespace

// ═══════════════════════════════════════════════════════════════
// SATB buffer tests
// ═══════════════════════════════════════════════════════════════

/// Allocate a SATB buffer, fill a few entries, flush to global queue,
/// then verify the global queue reflects the flushed entries.
TEST_F(BgcUnitTest, SatbBufferAllocAndFlush) {
    auto& ctrl = BgcController::Instance();

    int idx = ctrl.AllocateSatbBuffer();
    ASSERT_GE(idx, 0);
    ASSERT_LT(idx, 256);

    auto& buf = ctrl.GetSatbBuffer(idx);
    EXPECT_EQ(buf.count.load(std::memory_order_relaxed), 0u);

    void* dummy_objects[] = {
        reinterpret_cast<void*>(uintptr_t(0x1000)),
        reinterpret_cast<void*>(uintptr_t(0x2000)),
        reinterpret_cast<void*>(uintptr_t(0x3000)),
    };
    for (int i = 0; i < 3; i++) {
        buf.entries[i] = dummy_objects[i];
    }
    buf.count.store(3, std::memory_order_release);

    ctrl.FlushSatbBuffer(buf.entries, 3);
    buf.count.store(0, std::memory_order_release);

    EXPECT_EQ(ctrl.GetGlobalSatbCount(), 3u);

    // Flush with 0 entries — should be a no-op.
    ctrl.FlushSatbBuffer(buf.entries, 0);
    EXPECT_EQ(ctrl.GetGlobalSatbCount(), 3u);
}

/// Fill a SATB buffer to capacity (kBgcSatbBufferSize = 1024 entries),
/// flush to global queue, verify count.
TEST_F(BgcUnitTest, SatbBufferFullFlush) {
    auto& ctrl = BgcController::Instance();
    int idx = ctrl.AllocateSatbBuffer();
    ASSERT_GE(idx, 0);

    auto& buf = ctrl.GetSatbBuffer(idx);
    for (uint32_t i = 0; i < kBgcSatbBufferSize; i++) {
        buf.entries[i] = reinterpret_cast<void*>(uintptr_t(0x1000 + i));
    }
    buf.count.store(kBgcSatbBufferSize, std::memory_order_release);

    ctrl.FlushSatbBuffer(buf.entries, kBgcSatbBufferSize);
    buf.count.store(0, std::memory_order_release);

    EXPECT_EQ(ctrl.GetGlobalSatbCount(),
              static_cast<CHAOS_IL2CPP_SIZE>(kBgcSatbBufferSize));
}

/// Use the inline barrier path: set g_bgc_is_marking, write through the
/// barrier, verify the SATB buffer captures the old value.
TEST_F(BgcUnitTest, SatbPreWriteBarrierCapture) {
    g_bgc_is_marking.store(true, std::memory_order_release);
    auto& ctrl = BgcController::Instance();
    ctrl.ResetForTest();
    tls_satb_buffer_index = -1;
    tls_satb_registered = false;

    void* old_obj = reinterpret_cast<void*>(uintptr_t(0xBEEF));
    void* slot_storage = old_obj;
    void** slot = &slot_storage;

    BgcSatbPreWriteBarrier(slot);
    *slot = reinterpret_cast<void*>(uintptr_t(0xCAFE));

    EXPECT_EQ(*slot, reinterpret_cast<void*>(uintptr_t(0xCAFE)));

    int pool_idx = tls_satb_buffer_index;
    ASSERT_GE(pool_idx, 0);
    auto& buf = ctrl.GetSatbBuffer(pool_idx);
    uint32_t cnt = buf.count.load(std::memory_order_acquire);
    EXPECT_EQ(cnt, 1u);
    EXPECT_EQ(buf.entries[0], old_obj);

    ctrl.FlushSatbBuffer(buf.entries, cnt);
    buf.count.store(0, std::memory_order_release);
    EXPECT_EQ(ctrl.GetGlobalSatbCount(), 1u);

    g_bgc_is_marking.store(false, std::memory_order_release);
}

// ═══════════════════════════════════════════════════════════════
// Phase transition tests
// ═══════════════════════════════════════════════════════════════

TEST_F(BgcUnitTest, BgcInitialPhaseIsIdle) {
    auto& ctrl = BgcController::Instance();
    EXPECT_EQ(ctrl.Phase(), BgcPhase::IDLE);
}

// ═══════════════════════════════════════════════════════════════
// S1: BGC-thread survival gating (task#16) — no phantom concurrent mark
// ═══════════════════════════════════════════════════════════════

/// Regression for task#16: without the StartBgcCycle !bgc_running_ guard, a
/// scheduler-initiated BGC cycle could set phase_=CONCURRENT_MARK and
/// g_bgc_is_marking=true even when BgcController::Start() was never called
/// (bgc_running_==false) — e.g. the standalone gc_stress_test whose main()
/// skips RuntimeInit.  Every young GC then calls PauseForYoungGc and spins on
/// bgc_paused_ forever because no BGC thread exists to ack it (the 2/5 HANG).
///
/// StartBgcCycle() must bail at the !bgc_running_ gate WITHOUT touching phase_
/// or g_bgc_is_marking, so the phantom concurrent mark never forms.
TEST_F(BgcUnitTest, StartBgcCycleNoThreadDoesNotEnterMarking) {
    auto& ctrl = BgcController::Instance();

    // The BgcUnitTest fixture never calls Start(), so bgc_running_==false
    // (the production pre-Start invariant — the same state as the stress test
    // entrypoint).  Record pre-call phase; the gate must leave it untouched.
    BgcPhase before = ctrl.Phase();
    EXPECT_EQ(before, BgcPhase::IDLE);
    bool marking_before = g_bgc_is_marking.load(std::memory_order_acquire);
    EXPECT_FALSE(marking_before);

    ctrl.StartBgcCycle();  // must bail at the !bgc_running_ gate

    EXPECT_EQ(ctrl.Phase(), BgcPhase::IDLE)
        << "StartBgcCycle with no running BGC thread leaked a phantom phase";
    EXPECT_FALSE(g_bgc_is_marking.load(std::memory_order_acquire))
        << "phantom concurrent mark must not form without a live BGC thread";
}

TEST_F(BgcUnitTest, BgcPhaseQueries) {
    auto& ctrl = BgcController::Instance();
    EXPECT_FALSE(ctrl.IsBusy());
    EXPECT_FALSE(ctrl.IsMarking());
    EXPECT_FALSE(ctrl.IsRemarkNeeded());
    EXPECT_FALSE(ctrl.IsCompactNeeded());
    EXPECT_EQ(ctrl.Phase(), BgcPhase::IDLE);
}

TEST_F(BgcUnitTest, BgcResetForTestClearsState) {
    auto& ctrl = BgcController::Instance();
    EXPECT_FALSE(ctrl.IsBusy());
    EXPECT_EQ(ctrl.Phase(), BgcPhase::IDLE);
    EXPECT_EQ(ctrl.GetGlobalSatbCount(), 0u);
}

// ═══════════════════════════════════════════════════════════════
// Worker deque tests
// ═══════════════════════════════════════════════════════════════

/// DrainWorkerDeque returns 0 for an empty deque.
TEST_F(BgcUnitTest, WorkerDequeEmptyDrain) {
    auto& ctrl = BgcController::Instance();
    CHAOS_IL2CPP_SIZE drained = ctrl.DrainWorkerDeque(0);
    EXPECT_EQ(drained, 0u);
}

/// PushWorkerDequeEntry/GetWorkerDequeSize roundtrip.
TEST_F(BgcUnitTest, WorkerDequePushAndSize) {
    auto& ctrl = BgcController::Instance();
    EXPECT_EQ(ctrl.GetWorkerDequeSize(0), 0u);
    EXPECT_EQ(ctrl.GetWorkerDequeSize(1), 0u);

    ctrl.PushWorkerDequeEntry(0, reinterpret_cast<void*>(uintptr_t(1)));
    ctrl.PushWorkerDequeEntry(0, reinterpret_cast<void*>(uintptr_t(2)));
    EXPECT_EQ(ctrl.GetWorkerDequeSize(0), 2u);
    EXPECT_EQ(ctrl.GetWorkerDequeSize(1), 0u);
}
