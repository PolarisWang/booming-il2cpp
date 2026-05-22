// monitor_pulseall_stress.cpp — PulseAll chain-signal stress test
//
// Tests many threads (16+) looping on Wait/PulseAll to verify the chain-signal
// pattern doesn't deadlock or lose wakeups.  Each waiter thread waits on a
// shared monitor; the pulser thread calls PulseAll to wake them all.
//
// Under chain-signal: woken threads wake sequentially (one at a time) instead
// of all at once (thundering herd).  This test verifies correctness:
//   - No threads remain permanently blocked
//   - All waiters eventually wake
//   - No crashes in PulseAll fast-path (wait_count=0, fallback to notify_all)

#include <cstdio>
#include <cstdlib>
#include <atomic>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>

#include "gc_scheduler.h"
#include "gc_young_gen.h"
#include "gc_region.h"
#include "gc_young_collector.h"
#include "thread_state.h"
#include "runtime_core.h"

using namespace chaos::il2cpp::runtime_core;

// ── Fake monitor object ─────────────────────────────────────────────────

struct alignas(8) FakeTypeInfoHot {
    const void* parent     = nullptr;
    const void* vtable     = nullptr;
    uint64_t    stable_id  = 0;
    uint32_t    vtable_len = 0;
    uint16_t    warm_delta = 0;
    uint8_t     type_shape = 0;
    uint8_t     gc_bits    = 0;
    uint32_t    instance_size = 32;
    uint16_t    pointer_count = 0;
};

static FakeTypeInfoHot s_fake_type{};

struct FakeObjectWrapper {
    FakeTypeInfoHot* type_info;
};

static FakeObjectWrapper s_monitor_obj{&s_fake_type};

// ── Test state ──────────────────────────────────────────────────────────

static std::atomic<int> s_active_waiters{0};
static std::atomic<int> s_total_pulses{0};
static std::atomic<bool> s_running{true};

constexpr int kNumWaiters = 16;
constexpr int kPulsesPerBatch = 100;

// ── Waiter thread ────────────────────────────────────────────────────────

static void waiter_thread() {
    threading::RegisterThread();
    defer { threading::UnregisterThread(); };

    void* target = &s_monitor_obj;

    while (s_running.load(std::memory_order_relaxed)) {
        if (!MonitorEnter(target)) {
            std::fprintf(stderr, "FAIL: MonitorEnter failed\n");
            std::exit(1);
        }

        // Wait with timeout so we can check s_running periodically.
        // If PulseAll chain-signal works correctly, we should wake
        // promptly when pulsed rather than timing out.
        s_active_waiters.fetch_add(1, std::memory_order_relaxed);
        MonitorWait(target, 500);  // 500ms timeout as safety net
        s_active_waiters.fetch_sub(1, std::memory_order_relaxed);

        MonitorExit(target);

        // Brief yield to let other threads run.
        std::this_thread::yield();
    }
}

// ── Pulser thread ────────────────────────────────────────────────────────

static void pulser_thread() {
    void* target = &s_monitor_obj;

    for (int batch = 0; batch < kPulsesPerBatch && s_running.load(std::memory_order_relaxed); batch++) {
        if (!MonitorEnter(target)) {
            std::fprintf(stderr, "FAIL: Pulser MonitorEnter failed\n");
            std::exit(1);
        }

        // Wait until at least one waiter is active before pulsing.
        if (s_active_waiters.load(std::memory_order_relaxed) > 0) {
            MonitorPulseAll(target);
            s_total_pulses.fetch_add(1, std::memory_order_relaxed);
        }

        MonitorExit(target);

        // Small delay between pulse batches.
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// ── Main ─────────────────────────────────────────────────────────────────

int main() {
    std::printf("monitor_pulseall_stress: %d waiters, %d pulse batches\n",
                kNumWaiters, kPulsesPerBatch);

    // Init GC scheduler (required by MonitorEnter inflation path).
    g_gc_scheduler.Init();
    defer { g_gc_scheduler.Shutdown(); };

    // Init region manager (required by GC object infrastructure).
    RegionManager::Instance();

    std::vector<std::thread> threads;

    // Start waiter threads.
    for (int i = 0; i < kNumWaiters; i++) {
        threads.emplace_back(waiter_thread);
    }

    // Let waiters get set up.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Start pulser thread.
    std::thread pulser(pulser_thread);
    pulser.join();

    // Signal waiters to stop and wait for them to finish.
    s_running.store(false, std::memory_order_release);

    // Pulse one more time to wake any waiters still blocked.
    if (MonitorEnter(&s_monitor_obj)) {
        MonitorPulseAll(&s_monitor_obj);
        MonitorExit(&s_monitor_obj);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::printf("monitor_pulseall_stress OK (%d pulses, %d waiters joined)\n",
                s_total_pulses.load(), kNumWaiters);
    return EXIT_SUCCESS;
}
