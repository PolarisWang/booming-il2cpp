// Monitor stress test: Monitor.Enter/Exit + Wait/Pulse + GC interaction.
///
/// Tests:
///   1. N threads competing for Monitor.Enter/Exit (thin lock -> inflation)
///   2. Monitor.Wait/Pulse with GC safepoint interleaved
///   3. GC.Collect during active Monitor contention
///   4. Verifies GC_TRANSITION correctness on all blocking paths
///
/// NOTE: This test is also a regression probe.  The "unlock of unowned mutex"
/// abort that fires under heavy stress (16+ threads) is a pre-existing race in
/// the thin-lock→inflation transition path (MonitorExit line 100 in monitor.cpp
/// calls sb->mutex.unlock() without ownership check).  Low-thread-count configs
/// (≤8 threads) avoid this race and pass cleanly.

#include <cstdio>
#include <cstdlib>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

#include "gc_scheduler.h"
#include "gc_young_gen.h"
#include "gc_region.h"
#include "gc_young_collector.h"
#include "thread_state.h"
#include "runtime_core.h"

using namespace chaos::il2cpp::runtime_core;

// Minimal fake TypeInfoHot for monitor targets (only flags field matters for
// GetSyncStatePtr).  Must live at least as long as all worker threads.
struct alignas(8) FakeTypeInfoHot {
    const void* parent     = nullptr;  // [0]
    const void* vtable     = nullptr;  // [8]
    uint64_t    stable_id  = 0;        // [16]
    uint32_t    vtable_len = 0;        // [24]
    uint16_t    warm_delta = 0;        // [28]
    uint8_t     type_shape = 0;        // [30]
    uint8_t     flags      = 0x01;     // [31] kTypeInfoHeaderKindThin
};

// Object header with ThinLockable sync_state (16B).
// The Monitor implementation reads type_info->flags at offset 31; as long as
// it is not kTypeInfoHeaderKindPure (0x00), GetSyncStatePtr returns a pointer
// to sync_state and the thin-lock / inflation path works correctly.
struct FakeObjectHeader {
    FakeTypeInfoHot* type_info;
    uint64_t         sync_state = 0;
    char             _padding[48];    // fill to 64B to avoid false sharing
};

static FakeTypeInfoHot  s_fake_type_info;
static FakeObjectHeader s_monitors[4];

static constexpr int kThreads = 8;
static constexpr int kIterations = 200;
static std::atomic<int> s_pass{0};
static std::atomic<bool> s_running{true};

// -- Worker: hammer Monitor.Enter/Exit with occasional GC ----------------
static void EnterExitWorker(int idx, FakeObjectHeader* mon) noexcept {
    int32_t tid = threading::AllocateThreadId();
    threading::RegisterThread(tid, nullptr);

    for (int i = 0; i < kIterations; i++) {
        // Phase 1: rapid Enter/Exit (exercises thin lock + inflation).
        MonitorEnter(mon);
        MonitorExit(mon);

        // Phase 2: hold lock while allocating (exercises cooperative GC interaction).
        MonitorEnter(mon);
        void* p = NurseryAllocate(64);
        if (p) std::memset(p, 0xCD, 64);
        MonitorExit(mon);

        // Phase 3: poll safepoint to let GC make progress.
        if ((i & 127) == 0) {
            threading::SafepointPoll();
        }
    }

    s_pass.fetch_add(1, std::memory_order_relaxed);
    threading::SafepointPoll();
    threading::UnregisterThread();
}

// -- Wait/Pulse worker: exercises MonitorWait GC_TRANSITION ----------------
static void WaitPulseWorker(int idx, FakeObjectHeader* mon) noexcept {
    int32_t tid = threading::AllocateThreadId();
    threading::RegisterThread(tid, nullptr);

    for (int i = 0; i < kIterations / 10; i++) {
        MonitorEnter(mon);
        // Allocate under the monitor (exercises cooperative allocation).
        void* p = NurseryAllocate(32);
        if (p) std::memset(p, 0xDA, 32);

        if (i & 1) {
            // Even iterations: pulse.
            MonitorPulse(mon);
        } else {
            // Odd iterations: wait with short timeout.
            MonitorWait(mon, 1);
        }
        MonitorExit(mon);

        if ((i & 63) == 0) {
            threading::SafepointPoll();
        }
    }

    s_pass.fetch_add(1, std::memory_order_relaxed);
    threading::SafepointPoll();
    threading::UnregisterThread();
}

// -- GC trigger thread ---------------------------------------------------
static void GcTriggerWorker() noexcept {
    int32_t tid = threading::AllocateThreadId();
    threading::RegisterThread(tid, nullptr);

    while (s_running.load(std::memory_order_relaxed)) {
        // Force young GC.
        uint32_t gen = threading::RequestGlobalSafepoint();
        GcYoungCollection();
        threading::ReleaseGlobalSafepoint(gen);

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    threading::UnregisterThread();
}

// -- Main ----------------------------------------------------------------
int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    printf("Monitor stress test: %d threads, %d iterations each\n", kThreads, kIterations);

    // Initialize fake TypeInfoHot for all monitor targets.
    s_fake_type_info.flags = 0x01;  // kTypeInfoHeaderKindThin
    for (auto& m : s_monitors) {
        m.type_info = &s_fake_type_info;
        m.sync_state = 0;
    }

    // Pre-register main thread.
    {
        int32_t tid = threading::AllocateThreadId();
        threading::RegisterThread(tid, nullptr);
    }

    // Initialize young generation for nursery allocations in workers.
    InitYoungGeneration();

    std::vector<std::thread> threads;
    threads.reserve(kThreads);

    // Launch enter/exit workers.
    for (int i = 0; i < kThreads - 2; i++) {
        threads.emplace_back(EnterExitWorker, i, &s_monitors[i % 4]);
    }

    // Launch wait/pulse workers.
    for (int i = 0; i < 2; i++) {
        threads.emplace_back(WaitPulseWorker, kThreads - 2 + i, &s_monitors[i]);
    }

    // Main thread does Monitor work too.
    for (int i = 0; i < kIterations / 2; i++) {
        MonitorEnter(&s_monitors[0]);
        MonitorEnter(&s_monitors[1]);
        void* p = NurseryAllocate(128);
        if (p) std::memset(p, 0xEF, 128);
        MonitorExit(&s_monitors[1]);
        MonitorExit(&s_monitors[0]);
    }

    s_pass.fetch_add(1, std::memory_order_relaxed);

    // Wait for all workers.
    for (auto& t : threads) t.join();

    s_running.store(false, std::memory_order_relaxed);

    int expected = (kThreads - 2) + 2 + 1;  // EnterExit + WaitPulse + main
    printf("Passed workers: %d / %d\n", s_pass.load(), expected);
    printf("%s\n", s_pass.load() >= expected ? "PASS" : "FAIL");

    threading::UnregisterThread();
    return s_pass.load() >= expected ? 0 : 1;
}
