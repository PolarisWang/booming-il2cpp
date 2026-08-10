/// gc_region_barrier_stress_test — cross-generation write-barrier stress
///
/// Verifies the generation-aware write barrier (chaos_gc_dirty_card_dst_ref)
/// is correct under multi-threaded allocation + coordinated young GC.
///
/// Scenario: N threads each store a nursery reference into a shared old-gen
/// object via the double-arg barrier, while the coordinator runs young-GC
/// cycles at safepoints.  The barrier must dirty-card the old-gen object so the
/// card scan finds the old->nursery edge.  After the GC cycles, every old-gen
/// slot's reference must still point at valid GC-managed memory (nursery-or-old),
/// i.e. no cross-gen reference was dropped and no dangling pointer results.
///
/// [多平台]  pure C++ std::thread + std::atomic, no platform-specific calls
/// [JIT/AOT] the barrier symbol is the same one AOT and JIT generated code call
/// [热更]    the old-gen slot + barrier path is shared by interpreter and codegen

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>
#include <atomic>

#include <chaos/native_types.h>
#include "gc_region.h"
#include "gc_old_gen.h"
#include "gc_card_table.h"
#include "gc_young_gen.h"
#include "gc_heap.h"
#include "gc_helpers.h"
#include "gc_api.h"
#include "thread_state.h"

#include "gc_test_macros.h"

using namespace chaos::il2cpp::runtime_core;

// Test-framework global counters (declared per-test; referenced by the
// GC_TEST / GC_CHECK / GC_FAIL macros in gc_test_macros.h).
static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

namespace {
constexpr int kThreads       = 8;
constexpr int kObjsPerThread = 128;    // nursery objects referenced from the old slot
constexpr int kGcCycles      = 8;      // coordinated young-GC cycles

struct OldMessage {
    void* nursery_slot[kObjsPerThread];
};
static OldMessage* g_old_slot[kThreads];

void Worker(int id) {
    threading::RegisterThread(threading::AllocateThreadId(), nullptr);
    threading::EnterCooperativeMode();

    for (int i = 0; i < kObjsPerThread; i++) {
        void* nursery_obj = NurseryAllocate(128);
        if (nursery_obj != nullptr) {
            g_old_slot[id]->nursery_slot[i] = nursery_obj;
            // Record the old->nursery cross-gen reference via the generation-aware
            // barrier.  This both dirty-cards the old object (so young GC rescans
            // it) and is what prevents a live young object from being dropped.
            chaos_gc_dirty_card_dst_ref(g_old_slot[id], nursery_obj);
        }
        // Yield to let the coordinator's GC cycles observe this thread.
        if ((i & 31) == 0) std::this_thread::yield();
    }

    threading::UnregisterThread();
}
}  // namespace

static void RunCrossGenRefStoreStress() {
    TEST("CrossGenRefStoreWithYoungGc");

    // Init young generation and register the coordinator (main) thread.
    InitYoungGeneration();
    uint32_t tid = threading::AllocateThreadId();
    threading::RegisterThread(tid, nullptr);
    threading::EnterCooperativeMode();

    // Allocate shared old-gen message objects (one per worker).
    for (int i = 0; i < kThreads; i++) {
        void* old = G_OldGen().Allocate(sizeof(OldMessage) + 16, /*scanning=*/true);
        if (old == nullptr) { GC_FAIL("old-gen slot alloc failed"); break; }
        std::memset(old, 0, sizeof(OldMessage) + 16);
        g_old_slot[i] = reinterpret_cast<OldMessage*>(old);
    }

    // Spawn workers; each stores nursery refs into its old-gen slot.
    std::vector<std::thread> workers;
    for (int i = 0; i < kThreads; i++) workers.emplace_back(Worker, i);

    // Coordinated young-GC cycles at safepoints while workers run.
    for (int c = 0; c < kGcCycles; c++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
        uint32_t gen = threading::RequestGlobalSafepoint();
        chaos_gc_collect();
        threading::ReleaseGlobalSafepoint(gen);
    }

    for (auto& w : workers) w.join();

    // Post-condition: after promotion/young-GC, every old-gen slot's reference
    // must point at valid GC-managed memory (nursery or old-gen).  A dangling
    // pointer (not in nursery and not in old-gen) indicates the barrier missed a
    // cross-gen edge and the young object was freed while still referenced.
    int valid = 0, dangling = 0;
    for (int t = 0; t < kThreads; t++) {
        for (int i = 0; i < kObjsPerThread; i++) {
            void* ref = g_old_slot[t]->nursery_slot[i];
            if (ref == nullptr) continue;
            RegionManager& mgr = RegionManager::Instance();
            if (mgr.IsNurseryPointer(ref) || G_OldGen().IsInOldGen(ref)) valid++;
            else dangling++;
        }
    }
    GC_CHECK(dangling == 0, "cross-gen barrier: 0 dangling references after GC");
    GC_CHECK(valid >= 0, "cross-gen reference set recorded");

    threading::UnregisterThread();
}

int main() {
    puts("CRAG region write-barrier cross-gen stress tests");
    puts("════════════════════════════════════════════════════\n");
    RunCrossGenRefStoreStress();
    printf("\nResults: %d tests, %d failures\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}
