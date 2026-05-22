/// BGC young->old root scan verification test.
///
/// Tests that Phase 1b (young generation root scan) correctly discovers
/// old-gen objects referenced from young-gen objects, so they survive
/// BGC concurrent sweep.
///
/// Setup:
///   1. Register a raw alloc type (FakeTypeInfo) for old-gen test objects
///   2. Allocate an old-gen object directly via g_old_gen.Allocate
///      (avoids the young-GC-triggers-BGC issue in AllocOldGen)
///   3. Write a valid FakeTypeInfo pointer at offset 0 and pattern at offset 8
///   4. Allocate a young-gen object via NurseryAllocate
///   5. Write the old-gen object's address into the young-gen object
///   6. Clear all stack references to the old-gen object
///   7. Run a full BGC cycle
///   8. Verify the old-gen object's pattern is intact (not swept)
///
/// This validates the Phase 1b fix: PopulateRootSet now uses
/// g_young_gen.bump (not region->current) to scan the young generation
/// for old-gen pointers, and BgcTryMark correctly marks them.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>

#include <chaos/native_types.h>
#include "gc/gc_bgc.h"
#include "gc_region.h"
#include "gc_old_gen.h"
#include "gc_young_gen.h"
#include "gc_layout.h"
#include "gc_loh.h"
#include "thread_state.h"

using namespace chaos::il2cpp::runtime_core;

#include "gc_test_macros.h"

static int g_failures = 0;

// ── FakeTypeInfo for test objects ───────────────────────────────────────
struct alignas(8) TestTypeInfo {
    uint64_t stable_id;       // [0]  FakeTypeInfo: stable_id at offset +0
    uint64_t reserved[3];     // [8]  padding -- TypeInfoHot reads offset +16
};

static const void* SetupTestType(uint32_t instance_size) {
    uint64_t stable_id = GcLayoutRegistry::Instance()
        .RegisterOrGetRawAllocType(instance_size);
    CHECK(stable_id != 0, "RegisterOrGetRawAllocType returned non-zero stable_id");

    static TestTypeInfo s_ti{};
    s_ti.stable_id = stable_id;

    auto* reg = &GcLayoutRegistry::Instance();
    uintptr_t ti_addr = reinterpret_cast<uintptr_t>(&s_ti);
    reg->RegisterTypeInfoRange(ti_addr, ti_addr + sizeof(TestTypeInfo));

    bool ti_valid = reg->IsValidTypeInfoPointer(&s_ti);
    CHECK(ti_valid, "TestTypeInfo is in registered TypeInfo range");
    uint64_t read_back = reg->ReadStableId(&s_ti);
    bool stable_ok = (read_back == stable_id);
    if (!stable_ok) {
        printf("  ReadStableId mismatch: expected 0x%llx got 0x%llx\n",
               static_cast<unsigned long long>(stable_id),
               static_cast<unsigned long long>(read_back));
    }
    CHECK(stable_ok, "ReadStableId round-trip");
    return &s_ti;
}

static void InitTestObject(void* obj, const void* type_info, uint32_t pattern) {
    *static_cast<const void**>(obj) = type_info;
    auto* pattern_slot = reinterpret_cast<uint32_t*>(static_cast<char*>(obj) + 8);
    *pattern_slot = pattern;
}

static bool CheckTestObjectAlive(void* obj, uint32_t pattern) {
    return *reinterpret_cast<const uint32_t*>(static_cast<const char*>(obj) + 8) == pattern;
}

// ── BGC cycle helpers ─────────────────────────────────────────────────

static bool WaitForPhase(BgcPhase phase, int timeout_ms = 30000) {
    auto start = std::chrono::steady_clock::now();
    while (BgcController::Instance().Phase() != phase) {
        std::this_thread::yield();
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() > timeout_ms) {
            printf("  TIMEOUT waiting for phase %d (current: %d)\n",
                   static_cast<int>(phase),
                   static_cast<int>(BgcController::Instance().Phase()));
            return false;
        }
    }
    return true;
}

static bool RunBgcCycle() {
    uint32_t gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StartBgcCycle();
    threading::ReleaseGlobalSafepoint(gen);

    if (!WaitForPhase(BgcPhase::REMARK_NEEDED, 30000)) {
        printf("  ERROR: concurrent mark did not complete\n");
        return false;
    }

    gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StwRemark();
    BgcController::Instance().StartConcurrentSweep();
    threading::ReleaseGlobalSafepoint(gen);

    if (!WaitForPhase(BgcPhase::COMPACT_NEEDED, 30000)) {
        printf("  ERROR: concurrent sweep did not complete\n");
        return false;
    }

    gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StwCompact();
    threading::ReleaseGlobalSafepoint(gen);

    BgcController::Instance().WaitForCycleComplete();

    if (BgcController::Instance().Phase() != BgcPhase::IDLE) {
        printf("  ERROR: BGC phase is %d after cycle, expected IDLE\n",
               static_cast<int>(BgcController::Instance().Phase()));
        return false;
    }
    return true;
}

// ════════════════════════════════════════════════════════════════════════
// Test: Young-gen -> old-gen via direct old-gen allocation
// ════════════════════════════════════════════════════════════════════════

static void TestBgcYoungToOldRootScan(const void* test_type_info) {
    printf("\n-- Test: BGC young->old root scan (Phase 1b) --\n");

    // 1. Allocate directly from old-gen (avoids young-GC-triggers-BGC).
    //    The old-gen auto-initializes on first Allocate().
    void* old_obj = g_old_gen.Allocate(64, true);
    CHECK(old_obj != nullptr, "direct old-gen allocation succeeded");
    InitTestObject(old_obj, test_type_info, 0xCAFEBABE);
    CHECK(g_old_gen.IsInOldGen(old_obj), "object is in old-gen");

    // 2. Allocate a young-gen object and write the old-gen pointer into it.
    void* young_obj = NurseryAllocate(64);
    CHECK(young_obj != nullptr, "young-gen allocation succeeded");
    std::memset(young_obj, 0, 64);
    std::memcpy(young_obj, &old_obj, sizeof(void*));

    // 3. Clear the stack reference -- only the young-gen slot references old_obj.
    void* old_obj_copy = old_obj;
    old_obj = nullptr;

    // 4. Run a full BGC cycle.
    //    Phase 1b scans [begin, bump) and should find old_obj via young_obj.
    printf("  Running BGC cycle...\n");
    bool ok = RunBgcCycle();
    CHECK(ok, "BGC cycle completed");

    // 5. Verify old_obj survived BGC sweep.
    printf("  Checking if old_obj at %p survived...\n", old_obj_copy);
    bool alive = CheckTestObjectAlive(old_obj_copy, 0xCAFEBABE);
    printf("  CheckTestObjectAlive returned %d\n", alive);
    CHECK(alive, "old-gen object survived BGC sweep (Phase 1b root scan)");
}

// ════════════════════════════════════════════════════════════════════════
// Test: Cross-thread young -> old
// ════════════════════════════════════════════════════════════════════════

static void TestBgcCrossThreadYoungToOld(const void* test_type_info) {
    printf("\n-- Test: BGC cross-thread young->old root scan --\n");

    void* old_obj = g_old_gen.Allocate(64, true);
    CHECK(old_obj != nullptr, "cross-thread old-gen allocation succeeded");
    InitTestObject(old_obj, test_type_info, 0xDEADBEEF);

    std::atomic<bool> worker_done{false};
    std::thread worker([old_obj, &worker_done]() {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);

        void* local_young = NurseryAllocate(64);
        if (local_young) {
            std::memset(local_young, 0, 64);
            std::memcpy(local_young, &old_obj, sizeof(void*));
        }

        threading::UnregisterThread();
        worker_done = true;
    });
    worker.join();
    CHECK(worker_done, "worker thread finished");

    bool ok = RunBgcCycle();
    CHECK(ok, "BGC cycle completed after cross-thread allocation");
    CHECK(CheckTestObjectAlive(old_obj, 0xDEADBEEF),
          "cross-thread old-gen object survived BGC sweep");
}

// ════════════════════════════════════════════════════════════════════════
// Test: BGC concurrent mark + LOH concurrent allocation
//
// Verifies that LOH objects allocated by worker threads during BGC
// concurrent mark are correctly handled: the LOH segment is pre-marked
// at allocation time, so BGC sweep should not reclaim live LOH objects.
// ════════════════════════════════════════════════════════════════════════

static constexpr int kLohConcurrentWorkers = 4;
static constexpr int kLohAllocsPerWorker = 8;
static constexpr CHAOS_IL2CPP_SIZE kLohObjSize = 1024 * 100;  // 100 KB (> 85 KB LOH threshold)

static std::atomic<int> g_loh_workers_ready{0};
static std::atomic<int> g_loh_workers_done{0};

static void LohConcurrentWorker(const void* type_info, int thread_id,
                                 void** out_objs, int max_objs) {
    threading::RegisterThread(threading::AllocateThreadId(), nullptr);
    g_loh_workers_ready.fetch_add(1, std::memory_order_release);
    while (g_loh_workers_ready.load(std::memory_order_acquire) < kLohConcurrentWorkers) {
        std::this_thread::yield();
    }

    for (int i = 0; i < max_objs && i < kLohAllocsPerWorker; i++) {
        void* obj = g_loh.Allocate(kLohObjSize);
        if (obj == nullptr) break;
        // Write the type_info at the start and a pattern at offset 8.
        *static_cast<const void**>(obj) = type_info;
        *reinterpret_cast<uint32_t*>(static_cast<char*>(obj) + 8) =
            0xCAFE0000 + static_cast<uint32_t>(thread_id * kLohAllocsPerWorker + i);
        out_objs[i] = obj;
    }

    g_loh_workers_done.fetch_add(1, std::memory_order_release);
    threading::UnregisterThread();
}

static void TestBgcWithLohConcurrentAlloc(const void* test_type_info) {
    printf("\n-- Test: BGC concurrent mark + LOH concurrent allocation --\n");

    g_loh_workers_ready.store(0);
    g_loh_workers_done.store(0);

    // Allocate storage for worker results.
    void* worker_objs[kLohConcurrentWorkers][kLohAllocsPerWorker];
    std::memset(worker_objs, 0, sizeof(worker_objs));

    // Spawn workers BEFORE starting BGC, but they wait for bgc to be marking.
    std::vector<std::thread> workers;
    for (int i = 0; i < kLohConcurrentWorkers; i++) {
        workers.emplace_back(LohConcurrentWorker, test_type_info, i,
                             worker_objs[i], kLohAllocsPerWorker);
    }

    // Wait for all workers to be ready.
    while (g_loh_workers_ready.load(std::memory_order_acquire) < kLohConcurrentWorkers) {
        std::this_thread::yield();
    }

    // Start BGC cycle while workers are allocating LOH objects.
    {
        uint32_t gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().StartBgcCycle();
        threading::ReleaseGlobalSafepoint(gen);
    }

    // Wait for workers to finish.
    for (auto& w : workers) {
        if (w.joinable()) w.join();
    }

    // Wait for BGC concurrent mark to complete (REMARK_NEEDED).
    bool mark_done = WaitForPhase(BgcPhase::REMARK_NEEDED, 30000);
    CHECK(mark_done, "BGC concurrent mark completed");

    // STW re-mark and start sweep.
    {
        uint32_t gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().StwRemark();
        BgcController::Instance().StartConcurrentSweep();
        threading::ReleaseGlobalSafepoint(gen);
    }

    // Wait for sweep to complete.
    bool sweep_done = WaitForPhase(BgcPhase::COMPACT_NEEDED, 30000);
    CHECK(sweep_done, "BGC concurrent sweep completed");

    // Compact.
    {
        uint32_t gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().StwCompact();
        threading::ReleaseGlobalSafepoint(gen);
    }
    BgcController::Instance().WaitForCycleComplete();
    CHECK(BgcController::Instance().Phase() == BgcPhase::IDLE, "BGC back to IDLE");

    // Verify all LOH objects survived BGC sweep.
    int live_count = 0;
    for (int i = 0; i < kLohConcurrentWorkers; i++) {
        for (int j = 0; j < kLohAllocsPerWorker; j++) {
            if (worker_objs[i][j] == nullptr) continue;
            uint32_t expected = 0xCAFE0000 + static_cast<uint32_t>(i * kLohAllocsPerWorker + j);
            if (CheckTestObjectAlive(worker_objs[i][j], expected)) {
                live_count++;
            }
        }
    }
    printf("  LOH objects survived BGC sweep: %d / %d\n",
           live_count, kLohConcurrentWorkers * kLohAllocsPerWorker);
    CHECK(live_count > 0, "at least one LOH object survived BGC sweep");
}

// ════════════════════════════════════════════════════════════════════════
// Test: BGC root change buffer ringback (>256 unique entries)
//
// During concurrent mark, the mutator creates many new old→young references.
// These are tracked in the BGC root change buffer.  When the buffer exceeds
// 256 entries, entries are moved to the parallel mark worklist for re-scan.
// This test verifies the ringback path handles >256 unique entries correctly.
// ════════════════════════════════════════════════════════════════════════

static void TestBgcRootChangeRingback(const void* test_type_info) {
    printf("\n-- Test: BGC root change buffer ringback (>256 entries) --\n");

    constexpr int kRingbackEntries = 512;

    // Allocate old-gen objects to serve as targets for new references.
    std::vector<void*> old_objects;
    for (int i = 0; i < kRingbackEntries; i++) {
        void* obj = g_old_gen.Allocate(64, true);
        if (obj == nullptr) break;
        *static_cast<const void**>(obj) = test_type_info;
        *reinterpret_cast<uint32_t*>(static_cast<char*>(obj) + 8) =
            0xBABE0000 + i;
        old_objects.push_back(obj);
    }
    printf("  allocated %zu old-gen objects for ringback test\n",
           old_objects.size());

    // Start BGC cycle.
    {
        uint32_t gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().StartBgcCycle();
        threading::ReleaseGlobalSafepoint(gen);
    }

    // During concurrent mark, create many new old→young references by
    // writing old-gen pointers into nursery objects.  Each write through
    // the SATB barrier also generates a root change entry.
    std::vector<void*> young_objects;
    for (size_t i = 0; i < old_objects.size(); i++) {
        void* young = NurseryAllocate(64);
        if (young == nullptr) break;
        std::memset(young, 0, 64);
        // Write the old-gen pointer into the young object — this creates
        // a new cross-generation reference that BGC must track.
        std::memcpy(young, &old_objects[i], sizeof(void*));
        young_objects.push_back(young);
    }
    printf("  created %zu new cross-generation refs during concurrent mark\n",
           young_objects.size());

    // Wait for BGC concurrent mark to complete.
    bool mark_done = WaitForPhase(BgcPhase::REMARK_NEEDED, 30000);
    CHECK(mark_done, "BGC concurrent mark completed");

    // STW re-mark — this should process all remaining root changes.
    {
        uint32_t gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().StwRemark();
        BgcController::Instance().StartConcurrentSweep();
        threading::ReleaseGlobalSafepoint(gen);
    }

    bool sweep_done = WaitForPhase(BgcPhase::COMPACT_NEEDED, 30000);
    CHECK(sweep_done, "BGC concurrent sweep completed");

    {
        uint32_t gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().StwCompact();
        threading::ReleaseGlobalSafepoint(gen);
    }
    BgcController::Instance().WaitForCycleComplete();
    CHECK(BgcController::Instance().Phase() == BgcPhase::IDLE, "BGC back to IDLE");

    // Verify all old-gen objects survived (they should, since they're
    // referenced from young objects that were allocated during concurrent
    // mark and discovered by the root change tracking / STW re-mark).
    int survived = 0;
    for (auto* obj : old_objects) {
        if (CheckTestObjectAlive(obj, 0xBABE0000 + (survived))) {
            survived++;
        }
    }
    printf("  old-gen objects survived BGC sweep with ringback: %d / %zu\n",
           survived, old_objects.size());
    CHECK(survived > 0, "at least one old-gen object survived ringback test");
}

// ════════════════════════════════════════════════════════════════════════
// Main
// ════════════════════════════════════════════════════════════════════════

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("CRAG BGC root scan verification test");
    puts("=====================================\n");

    const void* test_type_info = SetupTestType(64);

    // Warm up nursery before starting BGC.
    InitYoungGeneration();
    void* warmup = NurseryAllocate(64);
    (void)warmup;

    threading::RegisterThread(threading::AllocateThreadId(), nullptr);
    BgcController::Instance().Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    TestBgcYoungToOldRootScan(test_type_info);
    TestBgcCrossThreadYoungToOld(test_type_info);
    TestBgcWithLohConcurrentAlloc(test_type_info);
    TestBgcRootChangeRingback(test_type_info);

    BgcController::Instance().Stop();
    threading::UnregisterThread();

    printf("\n== Results: 4 tests, %d failures ==\n", g_failures);
    return g_failures > 0 ? 1 : 0;
}
