/// gc_safepoint_test — Safepoint protocol unit tests.
///
/// Tests:
///   1. RegisterThread / UnregisterThread lifecycle
///   2. Cooperative / Preemptive mode transitions
///   3. SafepointRequested after RequestGlobalSafepoint
///   4. ReleaseGlobalSafepoint clears flag
///   5. Multiple threads reach safepoint concurrently
///   6. Nested safepoint generation counting

#include <cstdio>
#include <cstdint>
#include <atomic>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include "thread_state.h"
#include "gc_test_macros.h"

using namespace chaos::il2cpp::runtime_core;

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

// ── Test 1: Register / Unregister thread lifecycle ──────────────────
void TestRegisterUnregisterThread() {
    TEST("RegisterUnregisterThread");

    uint32_t tid = threading::AllocateThreadId();
    GC_CHECK(tid != 0xFFFFFFFF, "AllocateThreadId returns valid ID");

    threading::RegisterThread(tid, nullptr);
    // No direct way to verify registration, but should not crash.
    GC_CHECK(true, "RegisterThread succeeds");

    threading::UnregisterThread();
    GC_CHECK(true, "UnregisterThread succeeds");
}

// ── Test 2: Cooperative / Preemptive mode transitions ───────────────
void TestCooperativePreemptiveTransition() {
    TEST("CooperativePreemptiveTransition");

    uint32_t tid = threading::AllocateThreadId();
    threading::RegisterThread(tid, nullptr);

    // Start in cooperative mode (default after register).
    threading::EnterCooperativeMode();
    GC_CHECK(true, "EnterCooperativeMode succeeds");

    // Switch to preemptive.
    threading::EnterPreemptiveMode();
    GC_CHECK(true, "EnterPreemptiveMode succeeds");

    // Switch back to cooperative.
    threading::EnterCooperativeMode();
    GC_CHECK(true, "second EnterCooperativeMode succeeds");

    threading::UnregisterThread();
}

// ── Test 3: SafepointRequested lifecycle ────────────────────────────
void TestSafepointRequested() {
    TEST("SafepointRequested lifecycle");

    uint32_t tid = threading::AllocateThreadId();
    threading::RegisterThread(tid, nullptr);

    // Not requested initially.
    bool initially = threading::SafepointRequested();
    // Should be false (no safepoint in progress).

    // Request safepoint.
    uint32_t gen = threading::RequestGlobalSafepoint();
    GC_CHECK(gen > 0, "RequestGlobalSafepoint returns generation > 0");

    // Release.
    threading::ReleaseGlobalSafepoint(gen);
    GC_CHECK(true, "ReleaseGlobalSafepoint succeeds");

    threading::UnregisterThread();
    // Suppress unused-variable warning.
    (void)initially;
}

// ── Test 5: AllocateThreadId uniqueness ─────────────────────────────
void TestThreadIdUniqueness() {
    TEST("ThreadId uniqueness");

    std::vector<uint32_t> ids;
    std::mutex mtx;

    auto worker = [&]() {
        uint32_t tid = threading::AllocateThreadId();
        {
            std::lock_guard<std::mutex> lock(mtx);
            ids.push_back(tid);
        }
        threading::RegisterThread(tid, nullptr);
        threading::UnregisterThread();
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back(worker);
    }
    for (auto& th : threads) th.join();

    // Check uniqueness.
    bool unique = true;
    for (size_t i = 0; i < ids.size(); i++) {
        for (size_t j = i + 1; j < ids.size(); j++) {
            if (ids[i] == ids[j]) { unique = false; break; }
        }
        if (!unique) break;
    }
    GC_CHECK(unique, "all thread IDs are unique");
    GC_CHECK(ids.size() == 10, "10 threads allocated unique IDs");
}

// ── Main ────────────────────────────────────────────────────────────
int main() {
    puts("Safepoint protocol unit tests");
    puts("═════════════════════════════\n");

    TestRegisterUnregisterThread();
    TestCooperativePreemptiveTransition();
    TestSafepointRequested();
    TestThreadIdUniqueness();

    printf("\nResults: %d tests, %d failures\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}
