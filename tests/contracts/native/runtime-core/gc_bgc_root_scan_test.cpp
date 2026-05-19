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

#include <chaos/native_types.h>
#include "gc/gc_bgc.h"
#include "gc_region.h"
#include "gc_old_gen.h"
#include "gc_young_gen.h"
#include "gc_layout.h"
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

    BgcController::Instance().Stop();
    threading::UnregisterThread();

    printf("\n== Results: 2 tests, %d failures ==\n", g_failures);
    return g_failures > 0 ? 1 : 0;
}
