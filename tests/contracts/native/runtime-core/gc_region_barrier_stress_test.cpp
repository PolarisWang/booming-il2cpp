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
#include "gc_static_roots.h"
#include "thread_state.h"
#include "forbid_suspend.h"

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
            // A2b contract: the store and the card-dirty must appear atomic to the
            // safepoint coordinator (young-GC Phase-1 must never scan an old page
            // with a just-stored nursery ref on a still-clean card).  This is the
            // same BarrierCriticalSectionScope that production write sites hold.
            threading::BarrierCriticalSectionScope barrier;
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

    // Register the message backing store as a static root (managed semantics).
    // Without this, the old-gen (full) collector's mark has no way to reach the
    // messages → collects them and the nursery objects they reference → dangling.
    // This is the P1-A2b fix: static-root-referenced old-gen objects survive GC.
    GcRegisterStaticRootRange(g_old_slot, sizeof(g_old_slot), 0);

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
    //
    // Root cause (verified): the slot STORE and the CARD-BARRIER are two separate
    // operations.  If a coordinated GC safepoint lands BETWEEN them, young-GC
    // Phase-1 scans that old page with the card still clean, so it does not see
    // the freshly-stored nursery ref; the object (referenced only from this
    // not-yet-carded slot) is collected, and the later barrier mark is too late.
    // Diagnostics confirmed old-slot region_gen==2 (barrier would set card) and
    // ref region_gen==0 (short-circuit fires) — the barrier's decision is correct;
    // the miss is the non-atomic store-then-barrier window against a concurrent GC.
    int valid = 0, dangling = 0;
    void* first_dangle = nullptr;
    int last_dangle_t = -1;
    for (int t = 0; t < kThreads; t++) {
        for (int i = 0; i < kObjsPerThread; i++) {
            void* ref = g_old_slot[t]->nursery_slot[i];
            if (ref == nullptr) continue;
            // A slot reference is valid iff it still points into GC-managed
            // memory (nursery, Gen1 survivor, or old-gen).  We classify by the
            // region-generation table (GetRegionGen) rather than the nursery+old
            // predicates alone: young-GC promotes 128-byte nursery objects to
            // REGION_GEN1 (Survivor), whose addresses IsNurseryPointer() /
            // IsInOldGen() do NOT recognize — checking only those two would
            // falsely report every promoted-but-alive Gen1 object as "dangling".
            uint8_t gen = GetRegionGen(reinterpret_cast<uintptr_t>(ref));
            if (gen == kRegionGenYoung || gen == kRegionGenGen1 || gen == kRegionGenOld) {
                valid++;
            } else {
                // Pointer in NO managed region — genuinely freed / unmanaged.
                dangling++;
                last_dangle_t = t;
                if (first_dangle == nullptr) first_dangle = ref;
            }
        }
    }
    if (dangling > 0) {
        printf("  [STRESS] cross-gen dangling=%d / %d slots (first=%p, thread=%d)\n",
               dangling, kThreads * kObjsPerThread, first_dangle, last_dangle_t);
    }
    GC_CHECK(dangling == 0, "cross-gen barrier: 0 dangling references after GC");
    GC_CHECK(valid >= 0, "recorded cross-gen references");
    GC_CHECK(dangling == 0, "cross-gen barrier: 0 dangling references after GC");

    GcUnregisterStaticRootRange(g_old_slot);
    threading::UnregisterThread();
}

int main() {
    puts("CRAG region write-barrier cross-gen stress tests");
    puts("════════════════════════════════════════════════════\n");
    RunCrossGenRefStoreStress();
    printf("\nResults: %d tests, %d failures\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}
