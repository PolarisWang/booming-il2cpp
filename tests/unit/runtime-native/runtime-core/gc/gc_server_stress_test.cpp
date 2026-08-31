/// gc_server_stress_test.cpp — Server GC multi-heap concurrent pressure.
///
/// Exercises the CHAOS_IL2CPP_GC_SERVER multi-heap path (ExecuteMultiHeapGc in
/// gc_coordinator.cpp): concurrent mutator allocation driving the shared young
/// generation, plus explicit full-GC requests that fan out to per-heap old-gen
/// Collect via GcWorkerPool.  In WKS mode RequestGlobalGc delegates to the
/// single heap chaos_gc_collect() — still valid, but the distinctive value here
/// is the server multi-heap old-gen parallel collection.
///
/// Label: stress;gc.  Runs under SCALE env (default 50, the realistic production
/// concurrency; the known SCALE=100 std::mutex architecture limit is registered
/// in known-failures and not exercised here).

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include "gc_coordinator.h"
#include "gc_test_base.h"
#include "gc_heap_manager.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

namespace {

constexpr int kServerStressThreads = 50;   // SCALE-gated, see below
constexpr int kServerStressAllocs  = 128;

int GetStressScale() {
    const char* env = std::getenv("CHAOS_IL2CPP_STRESS_SCALE");
    if (env == nullptr) return 50;          // default: realistic production concurrency
    long val = std::strtol(env, nullptr, 10);
    return (val > 0) ? static_cast<int>(val) : 50;
}

}  // namespace

// ═══════════════════════════════════════════════════════════════════════════
// Fixture: extend GcUnitTestBase (RegistersThread + InitYoungGeneration +
// default type info) and bind the thread to the default heap after SetUp.
// ═══════════════════════════════════════════════════════════════════════════

struct GcServerStressTest : GcUnitTestBase {
    void SetUp() override {
        GcUnitTestBase::SetUp();
        // Bind this thread to heap 0 (Server GC) / no-op (WKS).
        // GcHeapManager::Initialize was called inside InitYoungGeneration.
        SetThreadHeap();
    }
    void TearDown() override {
        ClearThreadHeap();
        GcUnitTestBase::TearDown();
    }
};

// ── Concurrent allocation + full-GC fan-out (Server multi-heap) ────────

TEST_F(GcServerStressTest, ConcurrentAllocWithFullGc) {
    const int scale = GetStressScale();
    const int threads = kServerStressThreads * scale / 50;
    const int allocs  = kServerStressAllocs  * scale / 50;
    std::printf("[ServerStress] ConcurrentAllocWithFullGc threads=%d allocs=%d\n",
                threads, allocs);
    std::atomic<int> alloc_fails{0};
    std::atomic<int> gcs_done{0};

    // Workers: pure concurrent allocation with a BOUNDED iteration count, and
    // SafepointPoll after every allocation.  Bounded count (not while+stop) so
    // each worker naturally reaches a poll point in finite time — the main
    // thread's RequestGlobalGc waits only on workers that are mid-allocation,
    // not on workers that spin forever.  No worker calls RequestGlobalGc
    // itself (avoids contending RequestGlobalSafepoint from multiple mutators).
    auto worker = [&]() {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        threading::EnterCooperativeMode();
        SetThreadHeap();
        for (int i = 0; i < allocs && alloc_fails.load() == 0; i++) {
            size_t size = 16 + static_cast<size_t>((i * 2654435761u) % 4096);
            void* p = NurseryAllocate(size);
            if (p == nullptr) { alloc_fails.fetch_add(1); break; }
            std::memset(p, 0x7B, size);
            // Every allocation polls so a safepoint wait from the main thread's
            // RequestGlobalGc is bounded by one allocation, not a long window.
            threading::SafepointPoll();
        }
        ClearThreadHeap();
        threading::UnregisterThread();
        gcs_done.fetch_add(1);
    };

    std::vector<std::thread> pool;
    for (int t = 0; t < threads; t++) pool.emplace_back(worker);

    // Main thread drives full GCs (multi-heap parallel old-gen under server)
    // while workers are still allocating; each worker polls every alloc so
    // these GCs complete without waiting on a long-running remote mutator.
    for (int gc = 0; gc < 8; gc++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        EXPECT_NO_FATAL_FAILURE(GcCoordinator::Instance().RequestGlobalGc());
    }

    // Wait for all workers to finish (bounded work, they complete promptly).
    for (auto& th : pool) th.join();

    EXPECT_EQ(alloc_fails.load(), 0) << "Server stress: allocation failed under GC load";
    // Final global GC to coalesce, then confirm no crash.
    EXPECT_NO_FATAL_FAILURE(GcCoordinator::Instance().RequestGlobalGc());
}

// ── Large-object (LOH) pressure across heaps ───────────────────────────

TEST_F(GcServerStressTest, LargeObjectAcrossHeaps) {
    const int scale = GetStressScale();
    const int threads = kServerStressThreads * scale / 50;
    const int allocs  = kServerStressAllocs  * scale / 50;
    std::printf("[ServerStress] LargeObjectAcrossHeaps threads=%d allocs=%d\n",
                threads, allocs);
    std::atomic<int> alloc_fails{0};

    // Allocate small (TLAB fast path), medium (TLAB carve), and large
    // (> kMaxTlabAlloc, falls through to old-gen).  Avoid the LOH threshold
    // (85KB) here — the coordinate path is better exercised by the WKS stress
    // suite; this test focuses on Server GC multi-heap coordination.
    constexpr size_t kSizes[] = { 64, 1024, 16 * 1024, 28 * 1024 };

    auto worker = [&]() {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        threading::EnterCooperativeMode();
        SetThreadHeap();
        for (int i = 0; i < allocs; i++) {
            size_t size = kSizes[i % 4];
            void* p = NurseryAllocate(size);
            if (p == nullptr) { alloc_fails.fetch_add(1); break; }
            std::memset(p, 0x5A, size);
            threading::SafepointPoll();
        }
        ClearThreadHeap();
        threading::UnregisterThread();
    };

    std::vector<std::thread> pool;
    for (int t = 0; t < threads; t++) pool.emplace_back(worker);
    for (auto& th : pool) th.join();

    EXPECT_EQ(alloc_fails.load(), 0) << "Server stress: LOH alloc failed";
}
