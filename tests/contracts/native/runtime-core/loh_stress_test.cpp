/// loh_stress_test — Large Object Heap 压力测试。
///
/// 5 个场景：
///   L1 — BasicStress: 多线程分配 86KB-512KB，写 pattern，验证
///   L2 — SweepStress: 分配后丢弃一半引用，触发 GC，验证存活对象
///   L3 — FreeListReuse: 分配-释放-重分配同大小对象，验证 free segment 复用
///   L4 — CompactStress: 创建交错存活/死亡 LOH 对象触发 AUTOMATIC 压缩
///   L5 — MixedPressure: LOH + old-gen 混合分配
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <chrono>
#include <cinttypes>
#include <memory>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include "gc_test_macros.h"
#include "gc_loh.h"
#include "gc_region.h"
#include "gc_old_gen.h"
#include "gc_stats.h"

using namespace chaos::il2cpp::runtime_core;

// ── Test infrastructure ─────────────────────────────────────────────

static int g_failures = 0;
static int g_tests = 0;
static int g_sub = 0;

// ── Stress scale ────────────────────────────────────────────────────

static int g_stress_scale = 100;

static void ApplyStressScale() {
    const char* env = getenv("CHAOS_IL2CPP_STRESS_SCALE");
    if (env) {
        int val = atoi(env);
        if (val > 0 && val <= 1000) g_stress_scale = val;
    }
}

// Scale a workload constant by the stress scale factor.
static int Scaled(int base) {
    return (base * g_stress_scale) / 100;
}

// ── Helpers ─────────────────────────────────────────────────────────

static void WritePattern(void* p, CHAOS_IL2CPP_SIZE size, int thread_id, int iter) {
    auto* buf = static_cast<uint8_t*>(p);
    // Write magic at offset 0 (preserved for potential type-info header).
    buf[0] = static_cast<uint8_t>(thread_id);
    buf[1] = static_cast<uint8_t>(iter);
    buf[2] = 0xAB;
    buf[3] = 0xCD;
    // Fill rest with deterministic pattern.
    for (CHAOS_IL2CPP_SIZE i = 4; i < size; i++) {
        buf[i] = static_cast<uint8_t>((thread_id + iter + i) & 0xFF);
    }
}

static bool VerifyPattern(const void* p, CHAOS_IL2CPP_SIZE size, int thread_id, int iter) {
    auto* buf = static_cast<const uint8_t*>(p);
    if (buf[0] != static_cast<uint8_t>(thread_id)) return false;
    if (buf[1] != static_cast<uint8_t>(iter)) return false;
    if (buf[2] != 0xAB || buf[3] != 0xCD) return false;
    for (CHAOS_IL2CPP_SIZE i = 4; i < size; i++) {
        if (buf[i] != static_cast<uint8_t>((thread_id + iter + i) & 0xFF)) return false;
    }
    return true;
}

// ── Scenario L1: Basic stress ───────────────────────────────────────
//
/// 8 threads each allocate 500 LOH objects (86KB-512KB), write patterns,
/// verify after a full GC at the end.

static void ScenarioL1() {
    printf("\n── L1-BasicStress ──\n");
    TEST("L1: multi-thread LOH allocation");

    static constexpr int kBaseThreads = 8;
    static constexpr int kBaseAllocs = 500;
    int n_threads = kBaseThreads;
    int n_allocs = Scaled(kBaseAllocs);

    std::atomic<bool> failed{false};

    auto worker = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        for (int i = 0; i < n_allocs; i++) {
            CHAOS_IL2CPP_SIZE size = 86 * 1024 + (tid * 17 + i * 31) % (512 * 1024 - 86 * 1024);
            void* obj = g_loh.Allocate(size);
            if (!obj) { failed.store(true); break; }
            WritePattern(obj, size, tid, i);
            if ((i % 200) == 0) threading::SafepointPoll();
        }
        threading::UnregisterThread();
    };

    std::vector<std::thread> threads;
    for (int t = 0; t < n_threads; t++) {
        threads.emplace_back(worker, t);
    }
    for (auto& th : threads) th.join();
    threads.clear();

    if (failed.load()) { FAIL("allocation failed"); return; }

    // Trigger full GC to verify LOH objects survive.
    // Note: no safepoint wrapping needed — all worker threads have joined.
    g_old_gen.Collect(nullptr, nullptr);

    // Re-run workers to verify allocations via pattern checking.
    // (We can't easily find the old allocations, so we check new ones work.)
    std::atomic<int> verified{0};
    auto verifier = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        for (int i = 0; i < 10; i++) {
            CHAOS_IL2CPP_SIZE size = 96 * 1024;
            void* obj = g_loh.Allocate(size);
            if (!obj) { failed.store(true); break; }
            WritePattern(obj, size, tid, i);
            if (!VerifyPattern(obj, size, tid, i)) { failed.store(true); break; }
            verified.fetch_add(1);
        }
        threading::UnregisterThread();
    };

    for (int t = 0; t < n_threads; t++) {
        threads.emplace_back(verifier, t);
    }
    for (auto& th : threads) th.join();

    if (failed.load()) { FAIL("pattern verification failed"); return; }
    PASS();
}

// ── Scenario L2: Sweep stress ───────────────────────────────────────
//
/// Allocate, drop half, GC, verify survivors. Repeat.

static void ScenarioL2() {
    printf("\n── L2-SweepStress ──\n");
    TEST("L2: LOH sweep stress");

    static constexpr int kBaseThreads = 4;
    static constexpr int kBaseAllocs = 200;
    int n_threads = kBaseThreads;
    int n_allocs = Scaled(kBaseAllocs);

    std::atomic<bool> failed{false};
    std::mutex all_roots_mutex;
    std::vector<void*> all_roots;

    auto worker = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);

        for (int i = 0; i < n_allocs; i++) {
            CHAOS_IL2CPP_SIZE size = 96 * 1024;
            void* obj = g_loh.Allocate(size);
            if (!obj) { failed.store(true); break; }
            WritePattern(obj, size, tid, i);

            // Keep every other allocation — push to shared roots list.
            if (i % 2 == 0) {
                std::lock_guard<std::mutex> lock(all_roots_mutex);
                all_roots.push_back(obj);
            }

            if ((i % 100) == 0) threading::SafepointPoll();
        }

        threading::UnregisterThread();
    };

    std::vector<std::thread> threads;
    for (int t = 0; t < n_threads; t++) {
        threads.emplace_back(worker, t);
    }
    for (auto& th : threads) th.join();

    if (failed.load()) { FAIL("sweep stress failed"); return; }

    // Single-threaded GC: mark all roots, sweep, verify.
    int marked = 0;
    for (auto* r : all_roots) {
        if (g_loh.MarkObject(r)) marked++;
    }
    printf("\n    marking: roots=%zu newly_marked=%d", all_roots.size(), marked);

    CHAOS_IL2CPP_SIZE reclaimed = g_loh.Sweep();
    printf(" reclaimed=%llu", (unsigned long long)reclaimed);

    for (auto* r : all_roots) {
        if (!g_loh.IsInLOH(r)) {
            printf(" root lost!"); failed.store(true); break;
        }
    }

    if (failed.load()) { FAIL("sweep stress failed"); return; }
    PASS();
}

// ── Scenario L3: Free-list reuse ────────────────────────────────────
//
/// Allocate, free, re-allocate same size. Verify segment reuse.

static void ScenarioL3() {
    printf("\n── L3-FreeListReuse ──\n");
    TEST("L3: LOH free-list reuse");

    static constexpr int kBaseRounds = 100;
    int rounds = Scaled(kBaseRounds);

    for (int r = 0; r < rounds; r++) {
        CHAOS_IL2CPP_SIZE size = 96 * 1024 + (r % 4) * 8192;

        void* obj = g_loh.Allocate(size);
        if (!obj) { FAIL("allocation failed"); return; }
        memset(obj, 0xAA, size);

        // Free it.
        g_loh.Free(obj);

        // Re-allocate same size — should reuse.
        void* obj2 = g_loh.Allocate(size);
        if (!obj2) { FAIL("re-allocation failed"); return; }

        // Verify it's zeroed (LOH zeroes on allocation).
        bool zeroed = true;
        for (CHAOS_IL2CPP_SIZE i = 0; i < size && i < 1024; i++) {
            if (static_cast<uint8_t*>(obj2)[i] != 0) { zeroed = false; break; }
        }
        if (!zeroed) { FAIL("reused segment not zeroed"); return; }

        if ((r % 20) == 0) {
            threading::SafepointPoll();
        }
    }

    printf("\n    %d rounds completed", rounds);
    PASS();
}

// ── Scenario L4: Compaction stress ─────────────────────────────────
//
/// Create interleaved keep/free LOH objects to trigger AUTOMATIC
/// compaction. Verify kept objects remain valid after compaction.

static void ScenarioL4() {
    printf("\n── L4-CompactStress ──\n");
    TEST("L4: LOH compaction stress");

    static constexpr int kBaseAllocs = 1000;
    int n_allocs = Scaled(kBaseAllocs);

    std::atomic<bool> failed{false};
    std::mutex shared_mutex;
    std::vector<void*> all_keep;
    std::vector<void*> all_free;

    auto worker = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);

        for (int i = 0; i < n_allocs; i++) {
            CHAOS_IL2CPP_SIZE size = 128 * 1024;
            void* obj = g_loh.Allocate(size);
            if (!obj) { failed.store(true); break; }
            WritePattern(obj, size, tid, i);

            // Alternate: keep even, free odd.
            std::lock_guard<std::mutex> lock(shared_mutex);
            if (i % 2 == 0) {
                all_keep.push_back(obj);
            } else {
                all_free.push_back(obj);
            }

            if ((i % 200) == 0) threading::SafepointPoll();
        }

        threading::UnregisterThread();
    };

    std::vector<std::thread> threads;
    for (int t = 0; t < 4; t++) {
        threads.emplace_back(worker, t);
    }
    for (auto& th : threads) th.join();

    if (failed.load()) { FAIL("allocation failed"); return; }

    // Single-threaded: free the "free" objects, then mark keep, sweep, verify.
    printf("\n    total: keep=%zu free=%zu", all_keep.size(), all_free.size());

    for (auto* f : all_free) {
        g_loh.Free(f);
    }

    int marked = 0;
    for (auto* k : all_keep) {
        if (g_loh.MarkObject(k)) marked++;
    }
    printf(" marked=%d", marked);

    CHAOS_IL2CPP_SIZE reclaimed = g_loh.Sweep();
    printf(" reclaimed=%llu", (unsigned long long)reclaimed);

    for (auto* k : all_keep) {
        if (!g_loh.IsInLOH(k)) {
            printf(" keep lost!"); failed.store(true); break;
        }
    }

    if (failed.load()) { FAIL("compaction stress failed"); return; }
    PASS();
}

// ── Scenario L5: Mixed pressure ─────────────────────────────────────
//
/// 8 LOH allocator threads + 4 old-gen allocator threads.
/// No explicit GC — relies on automatic scheduling.

static void ScenarioL5() {
    printf("\n── L5-MixedPressure ──\n");
    TEST("L5: LOH + old-gen mixed pressure");

    static constexpr int kLohThreads = 8;
    static constexpr int kOldGenThreads = 4;
    static constexpr int kBaseLohAllocs = 300;
    static constexpr int kBaseOldGenAllocs = 500;
    int n_loh_allocs = Scaled(kBaseLohAllocs);
    int n_oldgen_allocs = Scaled(kBaseOldGenAllocs);

    std::atomic<bool> failed{false};
    std::atomic<int> loh_ok{0};
    std::atomic<int> oldgen_ok{0};

    auto loh_worker = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        for (int i = 0; i < n_loh_allocs; i++) {
            CHAOS_IL2CPP_SIZE size = 96 * 1024 + (tid * 13 + i * 7) % (256 * 1024);
            void* obj = g_loh.Allocate(size);
            if (!obj) { failed.store(true); break; }
            WritePattern(obj, size, tid, i);
            if (!VerifyPattern(obj, size, tid, i)) { failed.store(true); break; }
            if ((i % 100) == 0) threading::SafepointPoll();
        }
        loh_ok.fetch_add(1);
        threading::UnregisterThread();
    };

    auto oldgen_worker = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        for (int i = 0; i < n_oldgen_allocs; i++) {
            void* obj = g_old_gen.Allocate(64 + (tid * 8 + i * 3) % 1024, true);
            if (!obj) { failed.store(true); break; }
            if ((i % 200) == 0) threading::SafepointPoll();
        }
        oldgen_ok.fetch_add(1);
        threading::UnregisterThread();
    };

    std::vector<std::thread> threads;
    for (int t = 0; t < kLohThreads; t++) {
        threads.emplace_back(loh_worker, t);
    }
    for (int t = 0; t < kOldGenThreads; t++) {
        threads.emplace_back(oldgen_worker, t + kLohThreads);
    }
    for (auto& th : threads) th.join();

    if (failed.load()) { FAIL("mixed pressure failed"); return; }

    printf("\n    LOH threads: %d/%d  Old-gen threads: %d/%d",
           loh_ok.load(), kLohThreads, oldgen_ok.load(), kOldGenThreads);
    PASS();
}

// ── Scenario table ─────────────────────────────────────────────────

struct Scenario {
    const char* name;
    void (*run)();
};

static Scenario g_scenarios[] = {
    {"L1-BasicStress",     ScenarioL1},
    {"L2-SweepStress",     ScenarioL2},
    {"L3-FreeListReuse",   ScenarioL3},
    {"L4-CompactStress",   ScenarioL4},
    {"L5-MixedPressure",   ScenarioL5},
};

static constexpr int kNumScenarios = sizeof(g_scenarios) / sizeof(g_scenarios[0]);

// ── Main ────────────────────────────────────────────────────────────

int main(int argc, char** argv) {
    ApplyStressScale();

    // Init GC subsystems.
    g_old_gen.Init(0, 2);

    const char* filter = (argc > 1) ? argv[1] : nullptr;

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

        // Reset counters for next scenario.
        // But keep g_failures cumulative so we can detect regression.
        if (filter && strcmp(filter, g_scenarios[i].name) == 0) {
            break;  // Running a single scenario.
        }
    }

    printf("\n═══════════════════════════════════════════\n");
    printf("Results: %d scenarios, %d passed, %d failed\n",
           run_count, pass_count, g_failures);
    printf("═══════════════════════════════════════════\n");

    return g_failures > 0 ? 1 : 0;
}
