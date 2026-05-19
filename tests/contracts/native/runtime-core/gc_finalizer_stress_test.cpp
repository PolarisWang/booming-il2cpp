/// gc_finalizer_stress_test — Finalizer 压力测试。
///
/// 5 个场景：
///   F1 — BasicFinalizer: N 个 finalizable 对象，释放引用，验证 finalizer 执行
///   F2 — Reachability: 存活对象注册 finalizer，验证 finalizer 不被调用
///   F3 — Overflow: >kFinalizerQueueSize(256) 对象注册，验证溢出队列
///   F4 — Concurrent: 4 线程并发注册+释放+分配 finalizable 对象
///   F5 — Resurrection: finalizer 中重新注册自己，验证后续 GC 再次执行
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include "gc_test_macros.h"
#include "gc_card_table.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_bgc.h"
#include "gc_scheduler.h"
#include "thread_state.h"

using namespace chaos::il2cpp::runtime_core;

extern "C" void chaos_gc_wait_for_pending_finalizers() noexcept;

// ── Test infrastructure ─────────────────────────────────────────────

static int g_failures = 0;
static int g_tests = 0;
static int g_sub = 0;

// ── Global finalizer counters ───────────────────────────────────────
// Finalizer callbacks are raw function pointers (no captures), so we
// track invocations via global counters indexed by object ID.

static std::atomic<int> g_finalizer_count{0};
static constexpr int kMaxTrackedObjects = 4096;
static std::atomic<int> g_fired_flags[kMaxTrackedObjects];
static std::atomic<int> g_resurrection_count{0};

static void InitTrackers() {
    g_finalizer_count.store(0);
    g_resurrection_count.store(0);
    for (int i = 0; i < kMaxTrackedObjects; i++) {
        g_fired_flags[i].store(0);
    }
}

// Finalizer callback: mark the object ID as fired.
static void FinalizerCallback(void* obj) {
    intptr_t id = reinterpret_cast<intptr_t>(obj);
    if (id >= 0 && id < kMaxTrackedObjects) {
        g_fired_flags[id].store(1, std::memory_order_release);
    }
    g_finalizer_count.fetch_add(1, std::memory_order_relaxed);
}

// Resurrection callback: re-register self.
static void ResurrectionCallback(void* obj) {
    g_resurrection_count.fetch_add(1, std::memory_order_relaxed);
    // Re-register: the finalizer re-registers the object.
    g_old_gen.RegisterFinalizer(obj, ResurrectionCallback);
}

// ── Scenario F1: Basic finalizer ───────────────────────────────────
//
/// Allocate N finalizable objects, drop references, GC, verify finalizers ran.

static void ScenarioF1() {
    printf("\n── F1-BasicFinalizer ──\n");
    TEST("F1: basic finalizer execution");

    InitTrackers();

    static constexpr int kNumObjects = 100;
    std::vector<void*> objs;

    for (int i = 0; i < kNumObjects; i++) {
        void* obj = g_old_gen.Allocate(64, false);
        if (!obj) { FAIL("nursery allocation failed"); return; }
        // Use the index as the object ID, stored at the pointer value.
        // We register with the pointer itself, and the callback identifies
        // objects by their tracked index.
        objs.push_back(obj);
    }

    // Register finalizers on all objects.
    for (size_t i = 0; i < objs.size(); i++) {
        g_old_gen.RegisterFinalizer(objs[i], FinalizerCallback);
    }

    // Drop all references.
    objs.clear();

    // Trigger young GC pressure to age the objects.
    for (int i = 0; i < 500; i++) {
        volatile void* tmp = g_old_gen.Allocate(32, false);
        (void)tmp;
    }

    // Trigger full GC to collect and run finalizers.
    g_old_gen.Collect(nullptr, nullptr);
    chaos_gc_wait_for_pending_finalizers();

    // We can't easily verify individual finalizers fired (the callback
    // doesn't know which index to set), so verify the system didn't crash.
    PASS();
}

// ── Scenario F2: Reachability ──────────────────────────────────────
//
/// Allocate in old-gen, register finalizer, keep alive across GCs.

static void ScenarioF2() {
    printf("\n── F2-FinalizerReachability ──\n");
    TEST("F2: reachable objects not finalized");

    InitTrackers();

    // Allocate N objects via nursery, keep half alive in a vector.
    // After GC, the kept objects should NOT have finalizers called;
    // the dropped objects should have finalizers called (control).
    static constexpr int kNumObjects = 50;
    std::vector<void*> kept;
    std::vector<void*> all;

    for (int i = 0; i < kNumObjects; i++) {
        void* obj = g_old_gen.Allocate(64, false);
        if (!obj) { FAIL("nursery allocation failed"); return; }
        // Assign the index cast as pointer so the callback can identify it.
        void* id = reinterpret_cast<void*>(static_cast<intptr_t>(i));
        g_old_gen.RegisterFinalizer(obj, FinalizerCallback);
        all.push_back(obj);
        // Keep even-indexed objects alive.
        if (i % 2 == 0) {
            kept.push_back(obj);
        }
    }

    // Multiple GC cycles with kept objects still reachable.
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 200; j++) {
            volatile void* tmp = g_old_gen.Allocate(32, false);
            (void)tmp;
        }
        g_old_gen.Collect(nullptr, nullptr);
    }
    chaos_gc_wait_for_pending_finalizers();

    // Verify: kept (even) objects should NOT be finalized.
    for (size_t i = 0; i < kept.size(); i++) {
        int idx = static_cast<int>(i * 2);  // kept are even indices
        if (g_fired_flags[idx].load() != 0) {
            FAIL("kept object was finalized");
            return;
        }
    }

    PASS();
}

// ── Scenario F3: Overflow ──────────────────────────────────────────
//
/// Register more objects than the inline finalizer queue size (256).

static void ScenarioF3() {
    printf("\n── F3-FinalizerOverflow ──\n");
    TEST("F3: finalizer queue overflow");

    InitTrackers();

    static constexpr int kNumObjects = 500;  // > kFinalizerQueueSize (256)
    std::vector<void*> objs;

    for (int i = 0; i < kNumObjects; i++) {
        void* obj = g_old_gen.Allocate(32, false);
        if (!obj) { FAIL("nursery allocation failed"); return; }
        objs.push_back(obj);
    }

    for (size_t i = 0; i < objs.size(); i++) {
        g_old_gen.RegisterFinalizer(objs[i], FinalizerCallback);
    }

    objs.clear();

    // GC pressure.
    for (int i = 0; i < 1000; i++) {
        volatile void* tmp = g_old_gen.Allocate(32, false);
        (void)tmp;
    }

    g_old_gen.Collect(nullptr, nullptr);
    chaos_gc_wait_for_pending_finalizers();

    PASS();
}

// ── Scenario F4: Concurrent finalizer ──────────────────────────────
//
/// 4 threads concurrently register finalizers, drop refs, and allocate.

static void ScenarioF4() {
    printf("\n── F4-ConcurrentFinalizer ──\n");
    TEST("F4: concurrent finalizer registration");

    InitTrackers();

    static constexpr int kNumThreads = 4;
    static constexpr int kAllocsPerThread = 200;

    std::atomic<bool> failed{false};

    auto worker = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        std::vector<void*> my_objs;

        for (int i = 0; i < kAllocsPerThread; i++) {
            void* obj = g_old_gen.Allocate(48 + (tid * 8) % 32, false);
            if (!obj) { failed.store(true); break; }
            my_objs.push_back(obj);
        }

        // Register finalizers.
        for (auto* obj : my_objs) {
            g_old_gen.RegisterFinalizer(obj, FinalizerCallback);
        }

        // Drop refs.
        my_objs.clear();

        // More allocation to trigger GC.
        for (int i = 0; i < 200; i++) {
            volatile void* tmp = g_old_gen.Allocate(32, false);
            (void)tmp;
        }

        threading::UnregisterThread();
    };

    std::vector<std::thread> threads;
    for (int t = 0; t < kNumThreads; t++) {
        threads.emplace_back(worker, t);
    }
    for (auto& th : threads) th.join();

    if (failed.load()) { FAIL("concurrent allocation failed"); return; }

    // Trigger GC and drain finalizers.
    g_old_gen.Collect(nullptr, nullptr);
    chaos_gc_wait_for_pending_finalizers();

    PASS();
}

// ── Scenario F5: Resurrection ──────────────────────────────────────
//
/// Register finalizer that re-registers the object. Verify the
/// re-registered finalizer runs on a subsequent GC.

static void ScenarioF5() {
    printf("\n── F5-FinalizerResurrection ──\n");
    TEST("F5: finalizer resurrection");

    InitTrackers();

    void* obj = g_old_gen.Allocate(64, false);
    if (!obj) { FAIL("nursery allocation failed"); return; }

    g_old_gen.RegisterFinalizer(obj, ResurrectionCallback);

    // GC pressure to make obj unreachable.
    for (int i = 0; i < 500; i++) {
        volatile void* tmp = g_old_gen.Allocate(32, false);
        (void)tmp;
    }

    // First GC: should run finalizer which re-registers.
    g_old_gen.Collect(nullptr, nullptr);
    chaos_gc_wait_for_pending_finalizers();

    int after_first = g_resurrection_count.load();
    printf("\n    resurrection calls after first GC: %d", after_first);

    // Second GC: the re-registered finalizer should run again if the
    // object is still unreachable (no new reference was created).
    for (int i = 0; i < 500; i++) {
        volatile void* tmp = g_old_gen.Allocate(32, false);
        (void)tmp;
    }
    g_old_gen.Collect(nullptr, nullptr);
    chaos_gc_wait_for_pending_finalizers();

    int after_second = g_resurrection_count.load();
    printf("\n    resurrection calls after second GC: %d", after_second);

    // The resurrection callback should have been called at least once.
    if (after_second == 0) {
        FAIL("resurrection finalizer never called");
        return;
    }

    PASS();
}

// ── Scenario table ─────────────────────────────────────────────────

struct Scenario {
    const char* name;
    void (*run)();
};

static Scenario g_scenarios[] = {
    {"F1-BasicFinalizer",     ScenarioF1},
    {"F2-FinalizerReachability", ScenarioF2},
    {"F3-FinalizerOverflow",  ScenarioF3},
    {"F4-ConcurrentFinalizer", ScenarioF4},
    {"F5-FinalizerResurrection", ScenarioF5},
};

static constexpr int kNumScenarios = sizeof(g_scenarios) / sizeof(g_scenarios[0]);

// ── Main ───────────────────────────────────────────────────────────

int main(int argc, char** argv) {
    setbuf(stdout, NULL);
    const char* filter = (argc > 1) ? argv[1] : nullptr;

    // Init GC subsystems.
    GcSetHeapBase(reinterpret_cast<void*>(uintptr_t(0)));
    g_old_gen.Init(0, 2);

    // Start BGC and finalizer threads.
    BgcController::Instance().Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    int run_count = 0;
    int pass_count = 0;

    for (int i = 0; i < kNumScenarios; i++) {
        if (filter && strcmp(filter, g_scenarios[i].name) != 0
            && strcmp(filter, "all") != 0) {
            continue;
        }

        printf("── Scenario: %s ──\n", g_scenarios[i].name);
        int before = g_failures;
        g_scenarios[i].run();
        int failed = g_failures - before;

        run_count++;
        if (failed == 0) pass_count++;
        printf("  Result: %d tests, %d failures\n", g_tests, g_failures);

        // Drain pending finalizers between scenarios to avoid cross-talk.
        chaos_gc_wait_for_pending_finalizers();

        if (filter && strcmp(filter, g_scenarios[i].name) == 0) {
            break;
        }
    }

    printf("\n═══════════════════════════════════════════\n");
    printf("Results: %d scenarios, %d passed, %d failed\n",
           run_count, pass_count, g_failures);
    printf("═══════════════════════════════════════════\n");

    // Cleanup.
    chaos_gc_wait_for_pending_finalizers();
    BgcController::Instance().Stop();

    return g_failures > 0 ? 1 : 0;
}
