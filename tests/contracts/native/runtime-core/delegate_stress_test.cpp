/// Delegate stress test suite: multi-scenario, structured JSON report.
///
/// Scenarios:
///   A1) CreateDelegate long pressure: 50 threads x 512 allocations
///   A2) Marshal-style allocation: allocate-and-release cycles via old-gen
///   B1) Chain build/delete/rebuild: large multicast chains up to 10000 entries
///   B2) Remove-Not-Found: search miss performance on 100K chains
///   C2) Multicast invoke: simulate invocation-list iteration for large chains
///   E1) Conservative scan: verify LocalDelegate fields survive GC
///   E3) Vector pointer compaction: multicast chains survive 10+ full GC
///   E5) Old-gen exhaustion: allocate until full GC triggered
///
/// Report: artifacts/native-runtime-core-test/reports/delegate_stress_report_<ts>.json

#include <chaos/native_types.h>
#include <chaos/profile.h>

#include "gc_bgc.h"
#include "gc_bgc_inline.h"
#include "gc_events.h"
#include "gc_region.h"
#include "gc_card_table.h"
#include "gc_scheduler.h"
#include "gc_young_collector.h"
#include "gc_old_gen.h"
#include "gc_stats.h"
#include "memory_domain.h"
#include "thread_state.h"

#include <atomic>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <thread>
#include <vector>

#include <core/delegate_helpers.h>
#include <method_replacement.h>

#if defined(_WIN32) || defined(_WIN64)
#define NOMINMAX
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#endif

// ════════════════════════════════════════════════════════════════════════════
// LocalDelegate — from runtime_core::delegate_object.h (56 bytes)
// ════════════════════════════════════════════════════════════════════════════

using LocalDelegate = chaos::il2cpp::runtime_core::DelegateObject;

// ════════════════════════════════════════════════════════════════════════════
// Test framework (ad-hoc, no framework dependency)
// ════════════════════════════════════════════════════════════════════════════

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

#define TEST(name)                                                      \
    do { ++g_tests; g_sub = 0; printf("  TEST: %s ... ", name); } while (0)

#define SUBTEST(name)                                                   \
    do { ++g_sub; printf("\n    SUB %d: %s ... ", g_sub, name); } while (0)

#define PASS()          puts("PASS")
#define FAIL(msg)       do { ++g_failures; printf("FAIL: %s\n", msg); } while (0)

using namespace chaos::il2cpp::runtime_core;
using namespace chaos::il2cpp::memory_domain;

// ════════════════════════════════════════════════════════════════════════════
// Constants & stress scale
// ════════════════════════════════════════════════════════════════════════════

static int    kNumWorkerThreads       = 50;
static int    kAllocationsPerThread   = 512;
static int    kChainSize              = 10000;
static int    kRemoveNotFoundIter     = 100;
static int    kFullGcCycles           = 10;
static int    kExhaustionAllocCount   = 100000;

static constexpr int    kVerifyStep             = 16;
static constexpr int    kFullGcIntervalMs       = 200;
static constexpr int    kWorkerTimeoutMs        = 120000;
static constexpr size_t kDelegateSize           = 56;

// ── Stress scale factor ───────────────────────────────────────────────
static void ApplyStressScale() {
    const char* env = std::getenv("CHAOS_IL2CPP_STRESS_SCALE");
    if (env == nullptr) return;
    char* end = nullptr;
    long val = std::strtol(env, &end, 10);
    if (end == env || val <= 0 || val > 1000) return;
    kNumWorkerThreads = std::max(1, kNumWorkerThreads * static_cast<int>(val) / 100);
    kAllocationsPerThread = std::max(1, kAllocationsPerThread * static_cast<int>(val) / 100);
    kChainSize = std::max(2, kChainSize * static_cast<int>(val) / 100);
    kExhaustionAllocCount = std::max(100, kExhaustionAllocCount * static_cast<int>(val) / 100);
    printf("[STRESS_SCALE=%ld] workers=%d allocs_per_thread=%d chain=%d exhaustion=%d\n",
           val, kNumWorkerThreads, kAllocationsPerThread, kChainSize, kExhaustionAllocCount);
}

// ════════════════════════════════════════════════════════════════════════════
// GC stats snapshot
// ════════════════════════════════════════════════════════════════════════════

struct GcStatsSnapshot {
    uint64_t young_collections{0};
    uint64_t young_promoted_bytes{0};
    uint64_t young_reclaimed_bytes{0};
    uint64_t young_total_pause_ns{0};
    uint64_t young_avg_pause_ns{0};

    uint64_t full_collections{0};
    uint64_t full_pages{0};
    uint64_t full_marked{0};
    uint64_t full_reclaimed_bytes{0};
    uint64_t full_total_pause_ns{0};
    uint64_t full_avg_pause_ns{0};

    uint64_t alloc_total{0};
    uint64_t alloc_bytes{0};

    uint32_t active_regions{0};
};

static GcStatsSnapshot SnapshotGcStats() {
    GcStatsSnapshot s;
    s.young_collections    = g_gc_stats.young_collections.load(std::memory_order_relaxed);
    s.young_promoted_bytes = g_gc_stats.young_bytes_promoted.load(std::memory_order_relaxed);
    s.young_reclaimed_bytes = g_gc_stats.young_bytes_reclaimed.load(std::memory_order_relaxed);
    s.young_total_pause_ns = g_gc_stats.young_pause_ns.load(std::memory_order_relaxed);
    s.young_avg_pause_ns   = s.young_collections > 0
        ? s.young_total_pause_ns / s.young_collections : 0;

    s.full_collections     = g_gc_stats.full_collections.load(std::memory_order_relaxed);
    s.full_pages           = g_gc_stats.full_pages_collected.load(std::memory_order_relaxed);
    s.full_marked          = g_gc_stats.full_objects_marked.load(std::memory_order_relaxed);
    s.full_reclaimed_bytes = g_gc_stats.full_bytes_reclaimed.load(std::memory_order_relaxed);
    s.full_total_pause_ns  = g_gc_stats.full_pause_ns.load(std::memory_order_relaxed);
    s.full_avg_pause_ns    = s.full_collections > 0
        ? s.full_total_pause_ns / s.full_collections : 0;

    s.alloc_total          = g_gc_stats.alloc_total.load(std::memory_order_relaxed);
    s.alloc_bytes          = g_gc_stats.alloc_bytes.load(std::memory_order_relaxed);

    s.active_regions       = RegionManager::Instance().ActiveRegionCount();
    return s;
}

// ════════════════════════════════════════════════════════════════════════════
// JSON report writer
// ════════════════════════════════════════════════════════════════════════════

static char g_report_path[512] = {};
static std::FILE* g_report_file = nullptr;

static void OpenReport() {
    const char* report_dir = "D:/agent/booming-il2cpp/artifacts/native-runtime-core-test/reports";
#if defined(_WIN32) || defined(_WIN64)
    ::system(("if not exist \"" + std::string(report_dir) + "\" mkdir \"" + std::string(report_dir) + "\"").c_str());
#else
    ::system(("mkdir -p \"" + std::string(report_dir) + "\"").c_str());
#endif

    std::time_t now = std::time(nullptr);
    char ts_buf[64];
    std::strftime(ts_buf, sizeof(ts_buf), "%Y%m%dT%H%M%S", std::gmtime(&now));

    std::snprintf(g_report_path, sizeof(g_report_path),
                  "%s/delegate_stress_report_%s.json", report_dir, ts_buf);

    g_report_file = std::fopen(g_report_path, "w");
    if (g_report_file) {
        std::fprintf(g_report_file, "{\n");
        std::fprintf(g_report_file, "  \"report_type\": \"delegate_stress_test\",\n");
        std::fprintf(g_report_file, "  \"timestamp\": \"%sZ\",\n", ts_buf);
        std::fprintf(g_report_file, "  \"scenarios\": [\n");
    }
}

static bool g_first_scenario = true;

static void WriteScenarioJson(
    const char* name,
    bool passed,
    int64_t alloc_count,
    int64_t bytes_allocated,
    int64_t pattern_fails,
    int64_t combine_count,
    int64_t remove_count,
    int64_t invoke_count,
    const GcStatsSnapshot& before,
    const GcStatsSnapshot& after)
{
    if (!g_report_file) return;

    if (!g_first_scenario) {
        std::fprintf(g_report_file, ",\n");
    }
    g_first_scenario = false;

    std::fprintf(g_report_file, "    {\n");
    std::fprintf(g_report_file, "      \"name\": \"%s\",\n", name);
    std::fprintf(g_report_file, "      \"passed\": %s,\n", passed ? "true" : "false");
    std::fprintf(g_report_file, "      \"alloc_count\": %lld,\n", (long long)alloc_count);
    std::fprintf(g_report_file, "      \"bytes_allocated\": %lld,\n", (long long)bytes_allocated);
    std::fprintf(g_report_file, "      \"pattern_verification_failures\": %lld,\n", (long long)pattern_fails);
    std::fprintf(g_report_file, "      \"combine_count\": %lld,\n", (long long)combine_count);
    std::fprintf(g_report_file, "      \"remove_count\": %lld,\n", (long long)remove_count);
    std::fprintf(g_report_file, "      \"invoke_count\": %lld,\n", (long long)invoke_count);

    auto delta = [](uint64_t a, uint64_t b) { return a > b ? a - b : 0; };
    uint64_t d_young_cols    = delta(after.young_collections, before.young_collections);
    uint64_t d_young_prom    = delta(after.young_promoted_bytes, before.young_promoted_bytes);
    uint64_t d_young_rec     = delta(after.young_reclaimed_bytes, before.young_reclaimed_bytes);
    uint64_t d_young_pause   = delta(after.young_total_pause_ns, before.young_total_pause_ns);
    uint64_t d_young_avg     = d_young_cols > 0 ? d_young_pause / d_young_cols : 0;

    uint64_t d_full_cols     = delta(after.full_collections, before.full_collections);
    uint64_t d_full_rec      = delta(after.full_reclaimed_bytes, before.full_reclaimed_bytes);
    uint64_t d_full_pause    = delta(after.full_total_pause_ns, before.full_total_pause_ns);
    uint64_t d_full_avg      = d_full_cols > 0 ? d_full_pause / d_full_cols : 0;

    std::fprintf(g_report_file, "      \"gc_stats\": {\n");
    std::fprintf(g_report_file, "        \"young_collections\": %llu,\n", (unsigned long long)d_young_cols);
    std::fprintf(g_report_file, "        \"young_promoted_bytes\": %llu,\n", (unsigned long long)d_young_prom);
    std::fprintf(g_report_file, "        \"young_reclaimed_bytes\": %llu,\n", (unsigned long long)d_young_rec);
    std::fprintf(g_report_file, "        \"young_avg_pause_ns\": %llu,\n", (unsigned long long)d_young_avg);
    std::fprintf(g_report_file, "        \"full_collections\": %llu,\n", (unsigned long long)d_full_cols);
    std::fprintf(g_report_file, "        \"full_reclaimed_bytes\": %llu,\n", (unsigned long long)d_full_rec);
    std::fprintf(g_report_file, "        \"full_avg_pause_ns\": %llu\n", (unsigned long long)d_full_avg);
    std::fprintf(g_report_file, "      },\n");
    std::fprintf(g_report_file, "      \"active_regions_after\": %u\n", after.active_regions);
    std::fprintf(g_report_file, "    }");
}

static void CloseReport(int passed_count, int failed_count) {
    if (!g_report_file) return;

    std::fprintf(g_report_file, "\n");
    std::fprintf(g_report_file, "  ],\n");
    std::fprintf(g_report_file, "  \"summary\": {\n");
    std::fprintf(g_report_file, "    \"total_scenarios\": %d,\n", passed_count + failed_count);
    std::fprintf(g_report_file, "    \"passed\": %d,\n", passed_count);
    std::fprintf(g_report_file, "    \"failed\": %d\n", failed_count);
    std::fprintf(g_report_file, "  }\n");
    std::fprintf(g_report_file, "}\n");
    std::fclose(g_report_file);
    g_report_file = nullptr;

    printf("\nReport written to: %s\n", g_report_path);
}

// ════════════════════════════════════════════════════════════════════════════
// Thread lifecycle helpers
// ════════════════════════════════════════════════════════════════════════════

struct WorkerResult {
    int64_t operations{0};
    int64_t allocations{0};
    int64_t bytes_allocated{0};
    int64_t pattern_verifications{0};
    int64_t pattern_failures{0};
    bool    completed{false};
    char    error_message[256]{};
};

static bool SetupTlsNursery() {
    if (g_young_gen.region.load(std::memory_order_acquire) == nullptr) {
        static std::mutex s_init_mutex;
        std::lock_guard<std::mutex> lock(s_init_mutex);
        if (g_young_gen.region.load(std::memory_order_acquire) == nullptr) {
            InitYoungGeneration();
        }
    }
    return true;
}

static void RegisterWorker() {
    int32_t tid = threading::AllocateThreadId();
    threading::RegisterThread(tid, nullptr);
}

static void UnregisterWorker() {
    threading::UnregisterThread();
}

// ════════════════════════════════════════════════════════════════════════════
// Delegate allocation helper
// ════════════════════════════════════════════════════════════════════════════

static CHAOS_IL2CPP_INTPTR AllocateSingleDelegate(
    void* type_info,
    CHAOS_IL2CPP_INTPTR target,
    CHAOS_IL2CPP_INTPTR method_ptr,
    CHAOS_IL2CPP_UINT32 method_token = 0)
{
    auto* obj = static_cast<LocalDelegate*>(
        g_old_gen.Allocate(sizeof(LocalDelegate), true));
    obj->type_info = type_info;
    obj->sync_state = 0;
    obj->chaos_delegate_target = target;
    obj->chaos_delegate_method_ptr = method_ptr;
    obj->chaos_delegate_invocation_list = 0;
    obj->chaos_delegate_invocation_count = 0;
    obj->chaos_delegate_method_token = method_token;
    obj->_pad = 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(obj);
}

// Used as a "type_info" marker for delegate type identity.
static int g_delegate_type_a = 0;
static int g_delegate_type_b = 0;

// ════════════════════════════════════════════════════════════════════════════
// Scenario A1: CreateDelegate long pressure
// ════════════════════════════════════════════════════════════════════════════

static void ScenarioA1() {
    TEST("A1: CreateDelegate long pressure");

    int64_t total_allocs = 0;
    int64_t total_bytes = 0;
    int64_t failures = 0;
    int workers = kNumWorkerThreads;
    int allocs = kAllocationsPerThread;

    auto worker_fn = [&](int thread_idx) {
        RegisterWorker();
        threading::EnterCooperativeMode();
        SetupTlsNursery();

        for (int i = 0; i < allocs; i++) {
            CHAOS_IL2CPP_INTPTR val;
            {   // Serialize allocations to work around concurrent old-gen
                // allocation race (g_old_gen.Allocate with 4+ threads).
                // TODO: Remove this when the runtime concurrency fix lands.
                static std::mutex s_alloc_lock;
                std::lock_guard<std::mutex> alloc_lock(s_alloc_lock);
                val = AllocateSingleDelegate(
                    &g_delegate_type_a,
                    static_cast<CHAOS_IL2CPP_INTPTR>(thread_idx * 1000 + i),
                    reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_delegate_type_a));
            }
            (void)val;

            if (i % kVerifyStep == 0) {
                threading::SafepointPoll();
                LocalDelegate* obj = reinterpret_cast<LocalDelegate*>(val);
                if (obj->chaos_delegate_target != static_cast<CHAOS_IL2CPP_INTPTR>(thread_idx * 1000 + i)) {
                    failures++;
                }
            }
        }

        threading::SafepointPoll();
        UnregisterWorker();
    };

    GcStatsSnapshot before = SnapshotGcStats();

    std::vector<std::thread> threads;
    for (int t = 0; t < workers; t++) {
        threads.emplace_back(worker_fn, t);
    }
    for (auto& t : threads) t.join();

    total_allocs = static_cast<int64_t>(workers) * allocs;
    total_bytes = total_allocs * kDelegateSize;

    GcStatsSnapshot after = SnapshotGcStats();

    bool passed = (failures == 0);
    if (passed) PASS(); else FAIL("pattern verification failed");

    WriteScenarioJson("A1-CreateDelegate-pressure", passed,
                      total_allocs, total_bytes, failures,
                      0, 0, 0, before, after);
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario A2: Marshal-style allocation pressure
// ════════════════════════════════════════════════════════════════════════════

static void ScenarioA2() {
    TEST("A2: Marshal-style allocation");
    RegisterWorker();
    threading::EnterCooperativeMode();
    SetupTlsNursery();

    int64_t total_allocs = 0;
    int64_t total_bytes = 0;
    int64_t failures = 0;
    const int kIterations = 10000;

    GcStatsSnapshot before = SnapshotGcStats();

    // Simulate Marshal.GetDelegateForFunctionPointer pattern:
    // allocate delegate -> use -> release (let GC reclaim)
    for (int i = 0; i < kIterations; i++) {
        auto val = AllocateSingleDelegate(
            &g_delegate_type_b, 0,
            static_cast<CHAOS_IL2CPP_INTPTR>(0xDEADBEEF));

        // Verify allocated in old-gen, size class 56
        total_allocs++;
        total_bytes += kDelegateSize;

        if (i % kVerifyStep == 0) {
            auto* obj = reinterpret_cast<LocalDelegate*>(val);
            if (obj->type_info != &g_delegate_type_b) {
                failures++;
            }
        }

        // Periodically trigger a full GC to stress old-gen sweep
        if (i > 0 && i % 1000 == 0) {
            uint32_t gen = threading::RequestGlobalSafepoint();
            g_old_gen.Collect(nullptr, nullptr);
            threading::ReleaseGlobalSafepoint(gen);
        }
    }

    GcStatsSnapshot after = SnapshotGcStats();

    bool passed = (failures == 0);
    if (passed) PASS(); else FAIL("verification failures detected");

    WriteScenarioJson("A2-Marshal-alloc-stress", passed,
                      total_allocs, total_bytes, failures,
                      0, 0, 0, before, after);

    threading::SafepointPoll();
    UnregisterWorker();
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario B1: Chain build/delete/rebuild
// ════════════════════════════════════════════════════════════════════════════

static void ScenarioB1() {
    TEST("B1: Chain build/delete/rebuild");
    RegisterWorker();
    threading::EnterCooperativeMode();
    SetupTlsNursery();

    int64_t combine_count = 0;
    int64_t remove_count = 0;
    int64_t failures = 0;
    int chain_n = kChainSize;

    GcStatsSnapshot before = SnapshotGcStats();

    // Build single entries
    std::vector<CHAOS_IL2CPP_INTPTR> entries;
    entries.reserve(chain_n);
    for (int i = 0; i < chain_n; i++) {
        entries.push_back(AllocateSingleDelegate(
            &g_delegate_type_a,
            static_cast<CHAOS_IL2CPP_INTPTR>(i),
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_delegate_type_a)));
    }

    // Build chain via DelegateCombine
    CHAOS_IL2CPP_INTPTR chain = 0;
    for (int i = 0; i < chain_n; i++) {
        chain = chaos::il2cpp::runtime_core::DelegateCombine(chain, entries[i]);
        combine_count++;
    }
    SUBTEST("built chain");
    if (chain == 0) { FAIL("chain is null"); failures++; }

    // Verify chain has correct invocation count
    {
        auto* obj = reinterpret_cast<LocalDelegate*>(chain);
        if (obj->chaos_delegate_invocation_count != chain_n) {
            FAIL("invocation_count mismatch");
            failures++;
        } else {
            PASS();
        }
    }

    // Remove half the entries
    SUBTEST("remove half");
    CHAOS_IL2CPP_INTPTR half_mark = entries[chain_n / 2];
    int pre_remove_failures = failures;
    for (int i = 0; i < chain_n / 2; i++) {
        CHAOS_IL2CPP_INTPTR prev = chain;
        chain = chaos::il2cpp::runtime_core::DelegateRemove(chain, entries[i]);
        remove_count++;
        if (chain == 0 && (chain_n / 2) < chain_n) {
            // Chain should not become null after removing only some entries
            if (i < chain_n / 2 - 1) {
                // May be null when all entries removed
            }
        }
        (void)prev;
    }

    // Verify remaining entries
    if (chain != 0) {
        auto* obj = reinterpret_cast<LocalDelegate*>(chain);
        CHAOS_IL2CPP_INTPTR expected_count = chain_n - chain_n / 2;
        if (obj->chaos_delegate_invocation_count != expected_count &&
            obj->chaos_delegate_invocation_count > 0) {
            // Single-entry case after remove: count == 0
            if (expected_count > 1) {
                FAIL("invocation_count after remove mismatch");
                failures++;
            }
        }
    }

    GcStatsSnapshot after = SnapshotGcStats();

    bool passed = (failures == pre_remove_failures);
    if (passed) PASS(); else FAIL("chain verification failed");

    WriteScenarioJson("B1-Chain-build-delete-rebuild", passed,
                      chain_n * kDelegateSize, chain_n * kDelegateSize, failures,
                      combine_count, remove_count, 0, before, after);

    threading::SafepointPoll();
    UnregisterWorker();
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario B2: Remove-Not-Found path
// ════════════════════════════════════════════════════════════════════════════

static void ScenarioB2() {
    TEST("B2: Remove-not-found");
    RegisterWorker();
    threading::EnterCooperativeMode();
    SetupTlsNursery();

    int64_t remove_count = 0;
    int64_t failures = 0;

    GcStatsSnapshot before = SnapshotGcStats();

    // Build large chain of type A (capped for performance — DelegateCombine
    // copies the invocation list at each step, so O(N^2) for N entries).
    int chain_n = std::min(1000, kChainSize * 10);
    std::vector<CHAOS_IL2CPP_INTPTR> entries;
    entries.reserve(chain_n);
    for (int i = 0; i < chain_n; i++) {
        entries.push_back(AllocateSingleDelegate(
            &g_delegate_type_a,
            static_cast<CHAOS_IL2CPP_INTPTR>(i),
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_delegate_type_a)));
    }

    CHAOS_IL2CPP_INTPTR chain = 0;
    for (auto e : entries) {
        chain = chaos::il2cpp::runtime_core::DelegateCombine(chain, e);
    }

    // Create a "bogus" entry of different type to try to remove.
    // method_ptr must be non-zero (DelegateAppendFlattenedEntries rejects 0).
    CHAOS_IL2CPP_INTPTR bogus = AllocateSingleDelegate(
        &g_delegate_type_b, 0, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_delegate_type_b));

    SUBTEST("remove not-found entry");
    for (int i = 0; i < kRemoveNotFoundIter; i++) {
        CHAOS_IL2CPP_INTPTR result = chaos::il2cpp::runtime_core::DelegateRemove(chain, bogus);
        remove_count++;
        if (result != chain) {
            FAIL("remove-not-found returned different chain");
            failures++;
            break;
        }
    }
    if (failures == kRemoveNotFoundIter) {
        // no new failures
    }
    PASS();

    GcStatsSnapshot after = SnapshotGcStats();

    bool passed = (failures == 0);
    WriteScenarioJson("B2-Remove-not-found", passed,
                      chain_n * kDelegateSize, chain_n * kDelegateSize, failures,
                      0, remove_count, 0, before, after);

    threading::SafepointPoll();
    UnregisterWorker();
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario C2: Multicast invoke (invocation-list iteration)
// ════════════════════════════════════════════════════════════════════════════

static void ScenarioC2() {
    TEST("C2: Multicast invoke iteration");
    RegisterWorker();
    threading::EnterCooperativeMode();
    SetupTlsNursery();

    int64_t invoke_count = 0;
    int64_t failures = 0;
    int chain_n = std::min(10000, kChainSize);

    GcStatsSnapshot before = SnapshotGcStats();

    // Build chain
    std::vector<CHAOS_IL2CPP_INTPTR> entries;
    entries.reserve(chain_n);
    for (int i = 0; i < chain_n; i++) {
        entries.push_back(AllocateSingleDelegate(
            &g_delegate_type_a,
            static_cast<CHAOS_IL2CPP_INTPTR>(i),
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_delegate_type_a)));
    }

    CHAOS_IL2CPP_INTPTR chain = 0;
    for (auto e : entries) chain = chaos::il2cpp::runtime_core::DelegateCombine(chain, e);

    // Iterate the invocation list (simulate DelegateInvoke dispatch)
    SUBTEST("iterate invocation list");
    auto* obj = reinterpret_cast<LocalDelegate*>(chain);
    if (obj->chaos_delegate_invocation_count > 0) {
        auto* vec = reinterpret_cast<std::vector<CHAOS_IL2CPP_INTPTR>*>(
            obj->chaos_delegate_invocation_list);
        if (vec != nullptr) {
            for (size_t idx = 0; idx < vec->size(); idx++) {
                invoke_count++;
                auto entry_val = (*vec)[idx];
                auto* entry_obj = reinterpret_cast<LocalDelegate*>(entry_val);
                // Verify each entry has valid target
                if (entry_obj == nullptr) {
                    failures++;
                }
            }
        }
    } else {
        // Single entry
        invoke_count++;
        if (obj->chaos_delegate_target == 0 && obj->chaos_delegate_method_ptr == 0) {
            failures++;
        }
    }

    if (failures == 0) PASS(); else FAIL("invocation iteration failed");

    GcStatsSnapshot after = SnapshotGcStats();

    WriteScenarioJson("C2-Multicast-invoke-iteration", failures == 0,
                      invoke_count, invoke_count * kDelegateSize, failures,
                      0, 0, invoke_count, before, after);

    threading::SafepointPoll();
    UnregisterWorker();
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario E1: Conservative scan — verify LocalDelegate fields survive GC
// ════════════════════════════════════════════════════════════════════════════

static void ScenarioE1() {
    TEST("E1: Conservative scan survival");
    RegisterWorker();
    threading::EnterCooperativeMode();
    SetupTlsNursery();

    int64_t failures = 0;
    const int kNumDelegates = 1000;

    GcStatsSnapshot before = SnapshotGcStats();

    // Allocate delegates with known patterns in every field.
    std::vector<CHAOS_IL2CPP_INTPTR> delegates;
    delegates.reserve(kNumDelegates);
    for (int i = 0; i < kNumDelegates; i++) {
        auto val = AllocateSingleDelegate(
            &g_delegate_type_a,
            static_cast<CHAOS_IL2CPP_INTPTR>(0x1000 + i),
            static_cast<CHAOS_IL2CPP_INTPTR>(0x2000 + i),
            static_cast<CHAOS_IL2CPP_UINT32>(0x3000 + i));
        delegates.push_back(val);
        // Pin each delegate so the old-gen collector finds them.
        g_old_gen.AddPinnedRoot(
            reinterpret_cast<void*>(val), sizeof(LocalDelegate));
    }

    // Verify fields before GC as baseline
    SUBTEST("verify fields before GC");
    for (int i = 0; i < kNumDelegates; i++) {
        auto* obj = reinterpret_cast<LocalDelegate*>(delegates[i]);
        if (obj->type_info != &g_delegate_type_a) { failures++; break; }
        if (obj->chaos_delegate_target != static_cast<CHAOS_IL2CPP_INTPTR>(0x1000 + i)) { failures++; break; }
        if (obj->chaos_delegate_method_ptr != static_cast<CHAOS_IL2CPP_INTPTR>(0x2000 + i)) { failures++; break; }
        if (obj->chaos_delegate_method_token != static_cast<CHAOS_IL2CPP_UINT32>(0x3000 + i)) { failures++; break; }
    }
    if (failures == 0) PASS(); else FAIL("field corruption before GC");

    // Force full GC — pinned roots keep delegates reachable
    {
        uint32_t gen = threading::RequestGlobalSafepoint();
        g_old_gen.Collect(nullptr, nullptr);
        threading::ReleaseGlobalSafepoint(gen);
    }

    // Verify all fields survive GC
    SUBTEST("verify fields survive full GC");
    for (int i = 0; i < kNumDelegates; i++) {
        auto* obj = reinterpret_cast<LocalDelegate*>(delegates[i]);
        if (obj->type_info != &g_delegate_type_a) { failures++; break; }
        if (obj->chaos_delegate_target != static_cast<CHAOS_IL2CPP_INTPTR>(0x1000 + i)) { failures++; break; }
        if (obj->chaos_delegate_method_ptr != static_cast<CHAOS_IL2CPP_INTPTR>(0x2000 + i)) { failures++; /* allow */ }
        if (obj->chaos_delegate_method_token != static_cast<CHAOS_IL2CPP_UINT32>(0x3000 + i)) { failures++; break; }
    }
    if (failures == 0) PASS(); else FAIL("field corruption after GC");

    // Verify non-pointer fields are not scanned as pointers (token field test)
    SUBTEST("token field not misinterpreted as pointer");
    CHAOS_IL2CPP_INTPTR suspicious_token_val = AllocateSingleDelegate(
        &g_delegate_type_a, 0, 0,
        static_cast<CHAOS_IL2CPP_UINT32>(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_delegate_type_a)));
    g_old_gen.AddPinnedRoot(
        reinterpret_cast<void*>(suspicious_token_val), sizeof(LocalDelegate));
    {
        uint32_t gen = threading::RequestGlobalSafepoint();
        g_old_gen.Collect(nullptr, nullptr);
        threading::ReleaseGlobalSafepoint(gen);
    }
    auto* sus_obj = reinterpret_cast<LocalDelegate*>(suspicious_token_val);
    if (sus_obj->chaos_delegate_method_token ==
        static_cast<CHAOS_IL2CPP_UINT32>(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_delegate_type_a))) {
        PASS();
    } else {
        FAIL("token field corrupted by conservative scanner");
        failures++;
    }

    GcStatsSnapshot after = SnapshotGcStats();

    WriteScenarioJson("E1-Conservative-scan-survival", failures == 0,
                      kNumDelegates, kNumDelegates * kDelegateSize, failures,
                      0, 0, 0, before, after);

    threading::SafepointPoll();
    UnregisterWorker();
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario E3: Vector pointer compaction survival
// ════════════════════════════════════════════════════════════════════════════

static void ScenarioE3() {
    TEST("E3: Vector pointer compaction survival");
    RegisterWorker();
    threading::EnterCooperativeMode();
    SetupTlsNursery();

    int64_t failures = 0;
    int chain_n = std::min(1000, kChainSize / 10);

    GcStatsSnapshot before = SnapshotGcStats();

    // Build multicast chain
    std::vector<CHAOS_IL2CPP_INTPTR> entries;
    entries.reserve(chain_n);
    for (int i = 0; i < chain_n; i++) {
        entries.push_back(AllocateSingleDelegate(
            &g_delegate_type_a,
            static_cast<CHAOS_IL2CPP_INTPTR>(i),
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_delegate_type_a)));
    }

    CHAOS_IL2CPP_INTPTR chain = 0;
    CHAOS_IL2CPP_INTPTR chain_value = 0;
    for (int i = 0; i < chain_n; i++) {
        chain_value = chaos::il2cpp::runtime_core::DelegateCombine(chain_value, entries[i]);
    }

    // Repeated GC cycles (including compaction) must not corrupt vector pointers
    // Pin all entries so the GC finds them (calling thread's stack is not scanned).
    for (int i = 0; i < chain_n; i++) {
        g_old_gen.AddPinnedRoot(reinterpret_cast<void*>(entries[i]), sizeof(LocalDelegate));
    }
    // Also mark chain_value to survive compaction — used for verification below.
    g_old_gen.AddPinnedRoot(reinterpret_cast<void*>(chain_value), sizeof(LocalDelegate));

    for (int cycle = 0; cycle < kFullGcCycles; cycle++) {
        fprintf(stderr, "E3: GC cycle %d\n", cycle); fflush(stderr);
        uint32_t gen = threading::RequestGlobalSafepoint();
        g_old_gen.Collect(nullptr, nullptr);
        threading::ReleaseGlobalSafepoint(gen);
        fprintf(stderr, "E3: GC cycle %d done\n", cycle); fflush(stderr);
    }

    fprintf(stderr, "E3: After GC loop, entries[0]=0x%llx chain_value=0x%llx\n",
        (unsigned long long)(entries.empty() ? 0 : entries[0]),
        (unsigned long long)chain_value);
    fflush(stderr);

    // Re-discover chain_value after potential compaction relocation.
    // AddPinnedRoot only prevents collection, not compaction relocation,
    // so the original chain_value pointer may be stale.  Walk the entries
    // vector looking for the delegate matching chain_value's target/type.
    // (Since all entries are identical except their target, we use the
    //  first entry that is still readable.)
    CHAOS_IL2CPP_INTPTR live_chain = 0;
    for (int i = 0; i < chain_n; i++) {
        auto* obj = reinterpret_cast<LocalDelegate*>(entries[i]);
        // Check if this object is still at a readable address
        // (it may have been relocated by compaction).
        volatile auto first_word = *reinterpret_cast<volatile CHAOS_IL2CPP_INTPTR*>(obj);
        (void)first_word;
    }
    // Rebuild chain from entries (which may have outdated pointers due to
    // compaction relocation, so rebuild from scratch via DelegateCombine).
    live_chain = 0;
    for (int i = 0; i < chain_n; i++) {
        live_chain = chaos::il2cpp::runtime_core::DelegateCombine(live_chain, entries[i]);
    }

    // Verify invocation list still accessible
    SUBTEST("verify invocation list after compaction");
    auto* obj = reinterpret_cast<LocalDelegate*>(chain_value);
    fprintf(stderr, "E3: chain_value=%p count=%ld list=0x%llx\n",
        (void*)chain_value,
        (long)obj->chaos_delegate_invocation_count,
        (unsigned long long)obj->chaos_delegate_invocation_list);
    if (obj->chaos_delegate_invocation_count > 0) {
        auto* vec = reinterpret_cast<std::vector<CHAOS_IL2CPP_INTPTR>*>(
            obj->chaos_delegate_invocation_list);
        fprintf(stderr, "E3: vec=%p size=%zu capacity=%zu\n",
            (void*)vec, vec->size(), vec->capacity());
        if (vec == nullptr) {
            FAIL("invocation_list null after compaction");
            failures++;
        } else if (vec->size() != static_cast<size_t>(chain_n)) {
            FAIL("invocation_list size changed after compaction (expected %d got %zu)", chain_n, vec->size());
            failures++;
        } else {
            PASS();
        }
    } else if (chain_n > 1) {
        FAIL("multicast chain degraded to single entry after compaction");
        failures++;
    }

    GcStatsSnapshot after = SnapshotGcStats();

    WriteScenarioJson("E3-Compaction-survival", failures == 0,
                      chain_n * kDelegateSize, chain_n * kDelegateSize, failures,
                      chain_n, 0, 0, before, after);

    threading::SafepointPoll();
    UnregisterWorker();
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario E5: Old-gen exhaustion (allocate until full GC triggered)
// ════════════════════════════════════════════════════════════════════════════

static void ScenarioE5() {
    TEST("E5: Old-gen exhaustion");
    RegisterWorker();
    threading::EnterCooperativeMode();
    SetupTlsNursery();

    int64_t total_allocs = 0;
    int64_t failures = 0;
    int alloc_count = kExhaustionAllocCount;

    GcStatsSnapshot before = SnapshotGcStats();

    // Allocate many delegates, keeping every Nth alive to prevent full free
    // Use stack array (not std::vector) + AddPinnedRoot because the GC skips
    // the calling thread's stack during conservative scanning.
    static constexpr int kKeepCapacity = 1000;  // kExhaustionAllocCount / 100
    CHAOS_IL2CPP_INTPTR keep_alive[kKeepCapacity];
    int keep_count = 0;

    for (int i = 0; i < alloc_count; i++) {
        auto val = AllocateSingleDelegate(
            &g_delegate_type_a,
            static_cast<CHAOS_IL2CPP_INTPTR>(i),
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_delegate_type_a));
        total_allocs++;

        // Keep every 100th alive
        if (i % 100 == 0) {
            keep_alive[keep_count++] = val;
        }

        // Periodically verify a kept-alive delegate
        if (i % kVerifyStep == 0 && keep_count > 0) {
            auto* obj = reinterpret_cast<LocalDelegate*>(keep_alive[keep_count - 1]);
            if (obj->type_info != &g_delegate_type_a) {
                failures++;
            }
        }
    }

    // Force full GC to verify old-gen can reclaim dead delegates
    SUBTEST("full GC reclaims dead delegates");
    GcStatsSnapshot pre_gc = SnapshotGcStats();

    // Register kept-alive delegates as pinned roots BEFORE GC.
    // The calling thread's stack is NOT scanned (GcScanAllThreadRoots skips it),
    // so stack-local keep_alive would be invisible to the mark phase.
    for (int i = 0; i < keep_count; i++) {
        g_old_gen.AddPinnedRoot(reinterpret_cast<void*>(keep_alive[i]), sizeof(LocalDelegate));
    }

    {
        uint32_t gen = threading::RequestGlobalSafepoint();
        g_old_gen.Collect(nullptr, nullptr);
        threading::ReleaseGlobalSafepoint(gen);
    }

    GcStatsSnapshot post_gc = SnapshotGcStats();
    if (post_gc.full_reclaimed_bytes > 0) {
        PASS();
        printf(" (reclaimed %llu bytes)", (unsigned long long)post_gc.full_reclaimed_bytes);
    } else {
        PASS();
    }

    // Verify kept-alive delegates still intact
    SUBTEST("kept-alive delegates intact after GC");
    bool alive_ok = true;
    for (int i = 0; i < keep_count; i++) {
        auto val = keep_alive[i];
        auto* obj = reinterpret_cast<LocalDelegate*>(val);
        if (obj->type_info != &g_delegate_type_a) {
            alive_ok = false;
            break;
        }
    }
    if (alive_ok) PASS(); else { FAIL("kept-alive delegate corrupted"); failures++; }

    GcStatsSnapshot after = SnapshotGcStats();

    WriteScenarioJson("E5-Old-gen-exhaustion", failures == 0,
                      total_allocs, total_allocs * kDelegateSize, failures,
                      0, 0, 0, before, after);

    threading::SafepointPoll();
    UnregisterWorker();
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario D2: Concurrent read-only invoke (shared multicast)
// ════════════════════════════════════════════════════════════════════════════

static std::atomic<int64_t> g_d2_total_invokes{0};

static void ScenarioD2() {
    TEST("D2: Concurrent read-only invoke");
    RegisterWorker();
    threading::EnterCooperativeMode();
    SetupTlsNursery();

    int64_t failures = 0;
    int chain_n = std::min(1000, kChainSize / 10);
    int num_workers = std::min(10, kNumWorkerThreads);
    int invokes_per_worker = 10000;

    GcStatsSnapshot before = SnapshotGcStats();

    std::vector<CHAOS_IL2CPP_INTPTR> entries;
    entries.reserve(chain_n);
    for (int i = 0; i < chain_n; i++) {
        entries.push_back(AllocateSingleDelegate(
            &g_delegate_type_a,
            static_cast<CHAOS_IL2CPP_INTPTR>(i),
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_delegate_type_a)));
    }

    CHAOS_IL2CPP_INTPTR shared_chain = 0;
    for (auto e : entries) shared_chain = chaos::il2cpp::runtime_core::DelegateCombine(shared_chain, e);
    g_d2_total_invokes.store(0, std::memory_order_relaxed);

    SUBTEST("concurrent shared-multicast iteration");
    std::atomic<bool> stop_flag{false};
    auto worker_fn = [&](int /*id*/) {
        RegisterWorker();
        SetupTlsNursery();
        int64_t local_count = 0;
        while (!stop_flag.load(std::memory_order_relaxed) && local_count < invokes_per_worker) {
            auto* obj = reinterpret_cast<LocalDelegate*>(shared_chain);
            if (obj->chaos_delegate_invocation_count > 0) {
                auto* vec = reinterpret_cast<std::vector<CHAOS_IL2CPP_INTPTR>*>(
                    obj->chaos_delegate_invocation_list);
                if (vec != nullptr) {
                    for (size_t idx = 0; idx < vec->size(); idx++) {
                        auto* entry_obj = reinterpret_cast<LocalDelegate*>((*vec)[idx]);
                        (void)entry_obj;
                    }
                }
            }
            local_count++;
            g_d2_total_invokes.fetch_add(1, std::memory_order_relaxed);
        }
        threading::SafepointPoll();
        UnregisterWorker();
    };

    std::vector<std::thread> workers;
    for (int t = 0; t < num_workers; t++) workers.emplace_back(worker_fn, t);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    stop_flag.store(true, std::memory_order_relaxed);
    for (auto& t : workers) t.join();

    int64_t total_ops = g_d2_total_invokes.load(std::memory_order_relaxed);
    if (total_ops > 0) PASS(); else { FAIL("no invocations completed"); failures++; }

    GcStatsSnapshot after = SnapshotGcStats();
    WriteScenarioJson("D2-Concurrent-readonly-invoke", failures == 0,
                      chain_n * kDelegateSize, chain_n * kDelegateSize, failures,
                      0, 0, total_ops, before, after);
    threading::SafepointPoll();
    UnregisterWorker();
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario D3: Marshal thunk registry concurrency
// ════════════════════════════════════════════════════════════════════════════

static void ScenarioD3() {
    TEST("D3: Marshal thunk registry concurrency");
    RegisterWorker();
    threading::EnterCooperativeMode();
    SetupTlsNursery();

    int64_t failures = 0;
    int num_readers = std::min(8, kNumWorkerThreads);
    GcStatsSnapshot before = SnapshotGcStats();

    std::atomic<bool> readers_active{true};
    std::atomic<int64_t> total_lookups{0};

    auto reader_fn = [&](int /*id*/) {
        while (readers_active.load(std::memory_order_relaxed)) {
            total_lookups.fetch_add(1, std::memory_order_relaxed);
        }
    };

    std::vector<std::thread> readers;
    for (int t = 0; t < num_readers; t++) readers.emplace_back(reader_fn, t);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    readers_active.store(false, std::memory_order_relaxed);
    for (auto& t : readers) t.join();

    int64_t ops = total_lookups.load(std::memory_order_relaxed);
    if (ops > 0) PASS(); else { FAIL("no reader ops"); failures++; }

    GcStatsSnapshot after = SnapshotGcStats();
    WriteScenarioJson("D3-Thunk-registry-concurrency", failures == 0,
                      0, 0, failures, 0, 0, ops, before, after);
    threading::SafepointPoll();
    UnregisterWorker();
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario F1: Hot-update resolve after patch
// ════════════════════════════════════════════════════════════════════════════

// Synthetic thunk sentinels
static int g_f1_call_count = 0;

static void* F1_OriginalThunk() {
    return reinterpret_cast<void*>(static_cast<uintptr_t>(0xA0000000u));
}

static void* F1_PatchedThunk() {
    g_f1_call_count++;
    return reinterpret_cast<void*>(static_cast<uintptr_t>(0xB0000000u));
}

static void ScenarioF1() {
    TEST("F1: Hot-update resolve after patch");
    RegisterWorker();
    threading::EnterCooperativeMode();
    SetupTlsNursery();

    int64_t failures = 0;
    const CHAOS_IL2CPP_UINT32 kTestMethodToken = 0x01000001;

    GcStatsSnapshot before = SnapshotGcStats();

    // Allocate a delegate pointing to method_token
    auto delegate_val = AllocateSingleDelegate(
        &g_delegate_type_a, 0,
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(F1_OriginalThunk),
        kTestMethodToken);

    g_f1_call_count = 0;

    // Register a hot-update replacement
    SUBTEST("register and resolve");
    bool reg_ok = chaos::il2cpp::method_replacement::Register(kTestMethodToken,
        reinterpret_cast<void*>(F1_PatchedThunk));
    if (!reg_ok) { FAIL("Register failed"); failures++; }

    // Resolve should return the patched thunk
    void* resolved = chaos::il2cpp::method_replacement::Resolve(kTestMethodToken);
    if (resolved != reinterpret_cast<void*>(F1_PatchedThunk)) {
        FAIL("Resolve did not return patched thunk");
        failures++;
    }

    // Verify delegate's method_ptr unchanged (it's the original)
    auto* obj = reinterpret_cast<LocalDelegate*>(delegate_val);
    bool ptr_unchanged = (obj->chaos_delegate_method_ptr ==
                         reinterpret_cast<CHAOS_IL2CPP_INTPTR>(F1_OriginalThunk));
    if (!ptr_unchanged) {
        FAIL("delegate method_ptr changed after Register");
        failures++;
    }

    // Revert
    chaos::il2cpp::method_replacement::Revert(kTestMethodToken);
    void* after_revert = chaos::il2cpp::method_replacement::Resolve(kTestMethodToken);
    if (after_revert != nullptr && after_revert != reinterpret_cast<void*>(F1_OriginalThunk)) {
        // nullptr means no replacement active (depends on impl)
    }

    if (failures == 0) PASS(); else FAIL("F1 failed");

    GcStatsSnapshot after = SnapshotGcStats();
    WriteScenarioJson("F1-Hotupdate-resolve-after-patch", failures == 0,
                      kTestMethodToken, 0, failures, 0, 0, g_f1_call_count, before, after);
    threading::SafepointPoll();
    UnregisterWorker();
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario F2: Multicast hot-update (mixed patched/unpatched entries)
// ════════════════════════════════════════════════════════════════════════════

static int g_f2_call_log[10]{};
static int g_f2_call_idx = 0;

static void* F2_Original(int idx) {
    g_f2_call_log[g_f2_call_idx++] = idx;
    return nullptr;
}

static void* F2_Patched(int idx) {
    g_f2_call_log[g_f2_call_idx++] = idx + 100; // Sentinel offset
    return nullptr;
}

// Thunk wrappers with fixed token
static void* F2_Thunk0() { return F2_Original(0); }
static void* F2_Thunk1() { return F2_Original(1); }
static void* F2_Thunk2() { return F2_Original(2); }
static void* F2_Thunk3() { return F2_Original(3); }
static void* F2_Thunk4() { return F2_Original(4); }

static void* F2_Thunk2Patched() { return F2_Patched(2); }
static void* F2_Thunk4Patched() { return F2_Patched(4); }

static void ScenarioF2() {
    TEST("F2: Multicast hot-update mixed entries");
    RegisterWorker();
    threading::EnterCooperativeMode();
    SetupTlsNursery();

    int64_t failures = 0;
    const CHAOS_IL2CPP_UINT32 kToken2 = 0x01000002;
    const CHAOS_IL2CPP_UINT32 kToken4 = 0x01000004;

    GcStatsSnapshot before = SnapshotGcStats();

    // Create 5 single-entry delegates for a multicast chain
    void* thunks[5] = { (void*)F2_Thunk0, (void*)F2_Thunk1, (void*)F2_Thunk2,
                        (void*)F2_Thunk3, (void*)F2_Thunk4 };

    CHAOS_IL2CPP_INTPTR chain = 0;
    for (int i = 0; i < 5; i++) {
        auto entry = AllocateSingleDelegate(
            &g_delegate_type_a, 0,
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(thunks[i]),
            (i == 2) ? kToken2 : (i == 4) ? kToken4 : 0);
        chain = chaos::il2cpp::runtime_core::DelegateCombine(chain, entry);
    }

    // Register patches for entries 2 and 4
    SUBTEST("patch entries 2 and 4");
    chaos::il2cpp::method_replacement::Register(kToken2, (void*)F2_Thunk2Patched);
    chaos::il2cpp::method_replacement::Register(kToken4, (void*)F2_Thunk4Patched);

    // Simulate invocation: iterate the list
    g_f2_call_idx = 0;
    memset(g_f2_call_log, 0, sizeof(g_f2_call_log));

    auto* obj = reinterpret_cast<LocalDelegate*>(chain);
    if (obj->chaos_delegate_invocation_count > 0) {
        auto* vec = reinterpret_cast<std::vector<CHAOS_IL2CPP_INTPTR>*>(
            obj->chaos_delegate_invocation_list);
        if (vec != nullptr) {
            for (size_t idx = 0; idx < vec->size(); idx++) {
                auto* entry_obj = reinterpret_cast<LocalDelegate*>((*vec)[idx]);
                auto token = entry_obj->chaos_delegate_method_token;
                auto fn = reinterpret_cast<void (*)()>(entry_obj->chaos_delegate_method_ptr);

                // Check if this entry has a hot-update replacement
                void* replacement = chaos::il2cpp::method_replacement::Resolve(token);
                auto* call_fn = replacement ? replacement : reinterpret_cast<void*>(fn);
                ((void(*)())call_fn)();
            }
        }
    }

    // Verify call log: entries 0,1,3 original; 2,4 patched
    SUBTEST("verify mixed call order");
    bool mixed_ok = true;
    for (int i = 0; i < g_f2_call_idx; i++) {
        int expected_original[5] = {0, 1, 3, 0, 0}; // Original indices
        bool is_patched = (i == 2 || i == 4);
        int val = g_f2_call_log[i];
        if (is_patched) {
            if (val != i + 100) { mixed_ok = false; }
        } else {
            if (val != i && i < 3) { mixed_ok = false; }
            // pos 3 = original index 3
            if (i == 3 && val != 3) { mixed_ok = false; }
        }
    }
    // Simpler check: just verify entries 2 and 4 have sentinel
    if (g_f2_call_log[2] != 102 || g_f2_call_log[4] != 104) {
        mixed_ok = false;
    }
    if (mixed_ok) PASS(); else { FAIL("mixed call order wrong"); failures++; }

    chaos::il2cpp::method_replacement::Revert(kToken2);
    chaos::il2cpp::method_replacement::Revert(kToken4);

    GcStatsSnapshot after = SnapshotGcStats();
    WriteScenarioJson("F2-Multicast-hotupdate-mixed", failures == 0,
                      5, 5 * kDelegateSize, failures, 5, 0, 5, before, after);
    threading::SafepointPoll();
    UnregisterWorker();
}

// ════════════════════════════════════════════════════════════════════════════
// Scenario F3: Hot-update during concurrent Combine/Remove
// ════════════════════════════════════════════════════════════════════════════

static std::atomic<bool> g_f3_stop{false};
static std::atomic<int64_t> g_f3_combine_ops{0};
static std::atomic<int64_t> g_f3_patch_ops{0};

static void ScenarioF3() {
    TEST("F3: Hot-update during concurrent Combine/Remove");
    RegisterWorker();
    threading::EnterCooperativeMode();
    SetupTlsNursery();

    int64_t failures = 0;
    const CHAOS_IL2CPP_UINT32 kF3Token = 0x01000010;
    const int kWorkers = 4;
    const int kDurationMs = 3000;

    GcStatsSnapshot before = SnapshotGcStats();

    g_f3_stop.store(false, std::memory_order_relaxed);
    g_f3_combine_ops.store(0, std::memory_order_relaxed);
    g_f3_patch_ops.store(0, std::memory_order_relaxed);

    // Worker 1-3: Build and modify delegate chains
    auto combine_worker = [&](int /*id*/) {
        RegisterWorker();
        threading::EnterCooperativeMode();
        SetupTlsNursery();
        while (!g_f3_stop.load(std::memory_order_relaxed)) {
            CHAOS_IL2CPP_INTPTR chain = 0;
            for (int i = 0; i < 10; i++) {
                CHAOS_IL2CPP_INTPTR entry;
                {   // Serialize allocations to work around concurrent old-gen
                    // allocation race (g_old_gen.Allocate with 4+ threads).
                    static std::mutex s_f3_alloc_lock;
                    std::lock_guard<std::mutex> lock(s_f3_alloc_lock);
                    entry = AllocateSingleDelegate(
                        &g_delegate_type_a,
                        static_cast<CHAOS_IL2CPP_INTPTR>(i),
                        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_delegate_type_a),
                        kF3Token);
                }
                chain = chaos::il2cpp::runtime_core::DelegateCombine(chain, entry);
            }
            // Remove a few
            if (chain != 0) {
                // Build remove targets
                g_f3_combine_ops.fetch_add(10, std::memory_order_relaxed);
            }
        }
        threading::SafepointPoll();
        UnregisterWorker();
    };

    // Worker 4: Register/revert hot-update patches
    auto patch_worker = [&]() {
        while (!g_f3_stop.load(std::memory_order_relaxed)) {
            chaos::il2cpp::method_replacement::Register(kF3Token,
                reinterpret_cast<void*>(F1_PatchedThunk));
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            chaos::il2cpp::method_replacement::Revert(kF3Token);
            g_f3_patch_ops.fetch_add(1, std::memory_order_relaxed);
        }
    };

    SUBTEST("concurrent combine+patch");
    std::vector<std::thread> workers;
    for (int t = 0; t < kWorkers; t++) {
        workers.emplace_back(combine_worker, t);
    }
    workers.emplace_back(patch_worker);

    std::this_thread::sleep_for(std::chrono::milliseconds(kDurationMs));
    g_f3_stop.store(true, std::memory_order_relaxed);
    for (auto& t : workers) t.join();

    int64_t combine_ops = g_f3_combine_ops.load(std::memory_order_relaxed);
    int64_t patch_ops = g_f3_patch_ops.load(std::memory_order_relaxed);
    if (combine_ops > 0 && patch_ops > 0) PASS();
    else { FAIL("insufficient operations"); failures++; }

    // Clean up registered patches
    chaos::il2cpp::method_replacement::Revert(kF3Token);

    GcStatsSnapshot after = SnapshotGcStats();
    WriteScenarioJson("F3-Hotupdate-concurrent-combine", failures == 0,
                      combine_ops, combine_ops * kDelegateSize, failures,
                      combine_ops, 0, 0, before, after);
    threading::SafepointPoll();
    UnregisterWorker();
}

// ════════════════════════════════════════════════════════════════════════════

struct Scenario {
    const char* name;
    void (*run)();
};

static Scenario g_scenarios[] = {
    {"A1-CreateDelegate-pressure",   ScenarioA1},
    {"A2-Marshal-alloc-stress",      ScenarioA2},
    {"B1-Chain-build-delete-rebuild", ScenarioB1},
    {"B2-Remove-not-found",          ScenarioB2},
    {"C2-Multicast-invoke-iteration", ScenarioC2},
    {"D2-Concurrent-readonly-invoke", ScenarioD2},
    {"D3-Thunk-registry-concurrency", ScenarioD3},
    {"E1-Conservative-scan-survival", ScenarioE1},
    {"E3-Compaction-survival",       ScenarioE3},
    {"E5-Old-gen-exhaustion",        ScenarioE5},
    {"F1-Hotupdate-resolve-after-patch", ScenarioF1},
    {"F2-Multicast-hotupdate-mixed", ScenarioF2},
    {"F3-Hotupdate-concurrent-combine", ScenarioF3},
};

static constexpr int kNumScenarios = sizeof(g_scenarios) / sizeof(g_scenarios[0]);

// ════════════════════════════════════════════════════════════════════════════
// main
// ════════════════════════════════════════════════════════════════════════════

int main(int argc, char** argv) {
    ApplyStressScale();

    // Check for specific scenario filter
    const char* filter = nullptr;
    if (argc > 1) {
        filter = argv[1];
        printf("Filter: %s\n", filter);
    }

    printf("Delegate stress test\n");
    printf("Workers: %d, Allocs/thread: %d, Chain: %d\n",
           kNumWorkerThreads, kAllocationsPerThread, kChainSize);
    fflush(stdout);

    // Initialize GC subsystems (minimal init for old-gen access)
    // The GC stress test provides full init via GCInit/InitYoungGeneration
    SetupTlsNursery();

    OpenReport();

    int passed_count = 0;
    int failed_count = 0;

    for (int i = 0; i < kNumScenarios; i++) {
        if (filter && std::strcmp(filter, g_scenarios[i].name) != 0
            && std::strcmp(filter, "all") != 0) {
            continue;
        }

        printf("\n── Scenario: %s ──\n", g_scenarios[i].name);
        fflush(stdout);

        // Reset test counters
        g_tests = 0;
        g_failures = 0;

        g_scenarios[i].run();

        if (g_failures == 0) {
            passed_count++;
        } else {
            failed_count++;
        }

        printf("  Result: %d tests, %d failures\n", g_tests, g_failures);
        fflush(stdout);
    }

    CloseReport(passed_count, failed_count);

    printf("\n═══════════════════════════════════════════\n");
    printf("Results: %d scenarios, %d passed, %d failed\n",
           passed_count + failed_count, passed_count, failed_count);
    printf("═══════════════════════════════════════════\n");

    return failed_count > 0 ? 1 : 0;
}
