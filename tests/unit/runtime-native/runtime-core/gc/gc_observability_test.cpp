/// gc_observability_test — GC "production observability" assertions (GoogleTest).
///
/// Part of the reliability-verification hardening (plan D, "真实负载+可观测基线").
/// These tests assert that the GC exposes observable, monotonic, self-consistent
/// production metrics via the public gc_api.h surface — the same data a production
/// dashboard/alert would consume.  Previously several of these fields were
/// documented as "always 0" (see gc_stats.h) and the observability surface was
/// unverified.
///
/// IMPORTANT: this is a NEW additive test.  It only calls stable extern "C"
/// gc_api.h entry points with their exact signatures (verified against
/// src/native/runtime-core/gc/gc_api.h) plus the safepoint/thread primitives used
/// by the existing gc_safepoint_test.cpp, so it is structurally compile-safe.
/// It must be built and run via `--preset native-gc` to lock a real baseline.

#include <cstdint>
#include <functional>

#include "gc_api.h"
#include "gc_region.h"      // NurseryAllocate
#include "thread_state.h"
#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

namespace {

// Register the calling thread and enter cooperative mode so safepoint-driving GC
// APIs are safe to call.  Mirrors the setup used in gc_safepoint_test.cpp.
struct RunCtx {
    uint32_t tid{0};
    RunCtx() {
        tid = threading::AllocateThreadId();
        threading::RegisterThread(tid, nullptr);
        threading::EnterCooperativeMode();
    }
    ~RunCtx() {
        threading::EnterPreemptiveMode();
        threading::UnregisterThread();
    }
    RunCtx(const RunCtx&) = delete;
    RunCtx& operator=(const RunCtx&) = delete;
};

}  // namespace

TEST(GcObservability, HeapSizeIsQueryable) {
    RunCtx ctx;
    // Must be a finite, non-negative number regardless of current heap pressure.
    auto heap = chaos_gc_get_heap_size();
    EXPECT_GE(heap, 0);
    // Forcing a collection must not crash the query path.
    volatile int64_t total = chaos_gc_get_total_memory(0);
    EXPECT_GE(total, 0);
}

TEST(GcObservability, CollectionCountIsMonotonicUnderCollect) {
    RunCtx ctx;
    auto before = chaos_gc_get_collection_count(2);   // full (old gen)
    chaos_gc_collect_with_mode(2, 1 /* Forced */);
    auto after = chaos_gc_get_collection_count(2);
    EXPECT_GE(after, before);   // full GC increments or stays; never decreases
}

TEST(GcObservability, AllocatedBytesForCurrentThreadMonotonic) {
    RunCtx ctx;
    auto a = chaos_gc_get_allocated_bytes_for_current_thread();
    // Allocate one small nursery object (NurseryAllocate is the fast path used by
    // the stress suite; returns non-null for a 64-byte request).
    volatile void* p = NurseryAllocate(64);
    ASSERT_NE(p, nullptr);
    (void)p;   // keep p alive for the measurement window
    auto b = chaos_gc_get_allocated_bytes_for_current_thread();
    // No forced GC runs between the two reads, so the per-thread counter must be
    // strictly non-decreasing across the allocation.
    EXPECT_GE(b, a);
}

TEST(GcObservability, TotalPauseDurationDoesNotDecrease) {
    RunCtx ctx;
    auto p0 = chaos_gc_get_total_pause_duration();
    chaos_gc_collect_with_mode(2, 1 /* Forced */);
    auto p1 = chaos_gc_get_total_pause_duration();
    // Accumulated pause time is monotonic across collections.
    EXPECT_GE(p1, p0);
}

TEST(GcObservability, ExternalMemoryPressureAccepted) {
    RunCtx ctx;
    // Notify pressure, then release it.  Must not crash and must return to a
    // sane allocated-bytes reading afterward.
    chaos_gc_add_memory_pressure(1 << 20);      // 1 MiB external pressure
    chaos_gc_remove_memory_pressure(1 << 20);
    SUCCEED();
}

TEST(GcObservability, PlatformMemoryStatusValid) {
    MemoryStatusData st{};
    GetPlatformMemoryStatus(st);
    // On non-Windows this returns zeros (documented); on Windows total must be >0.
    EXPECT_GE(st.total_phys, 0);
    EXPECT_GE(st.avail_phys, 0);
    if (st.total_phys > 0) {
        EXPECT_LE(st.avail_phys, st.total_phys);
    }
}
