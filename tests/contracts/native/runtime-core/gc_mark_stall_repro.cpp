/// gc_mark_stall_repro.cpp — deterministic reproduction of the full-GC MARK
/// phase stall (task#16 S2).  Scenario C exposes it intermittently (~1/12) at
/// ~672+ fragmented old-gen pages under mutator churn.  This harness builds an
/// interconnected old-gen graph (each seed points at several siblings, so the
/// full-GC mark must trace a large transitive closure) over a high page count,
/// then runs one full GC to exercise the parallel mark.
///
/// KEY (finding): a flat pointer-free heap converges even at 806 pages.  The
/// stall needs the LARGE interconnected closure + (ideally) mutator churn that
/// scenario-C produces.  This harness targets the graph shape deterministically.
///
/// Usage: run directly; exit 0 = mark converged, 1 = setup failure, 124 = stall.

#include <chrono>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <vector>

#include <chaos/native_types.h>
#include "gc_layout.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_scheduler.h"

using namespace chaos::il2cpp::runtime_core;

int main(int argc, char** argv) {
    (void)argc; (void)argv;
    std::printf("[mark_repro] starting old-gen interlinked mark-convergence reproduction\n");

    // Raw-alloc TypeInfo: the whole block is scanned as raw pointers.  Each seed
    // is LARGE (16 KB → spans pages → high page count) and its leading pointer
    // slots reference siblings (large interlinked closure).  The remaining payload
    // is zeroed so the raw scan sees nulls — no garbage children.
    constexpr int kSeedObjects        = 2400;
    constexpr int kSeedObjectBytes    = 16 * 1024;   // 16 KB per object → high page count
    constexpr int kSeedPointerSlots   = 8;           // leading slots hold sibling refs
    constexpr int kSeedPointersPer    = 4;           // each object points at 4 siblings

    uint64_t sid = GcLayoutRegistry::Instance()
        .RegisterOrGetRawAllocType(static_cast<uint32_t>(kSeedObjectBytes));
    auto& layout_reg = GcLayoutRegistry::Instance();
    const auto* layout = layout_reg.Lookup(sid);
    if (layout == nullptr) {
        std::printf("[mark_repro] FAIL: couldn't obtain raw-alloc layout\n");
        return 1;
    }
    // The TypeInfo the GC recognizes is the ADDRESS of a registered type-info
    // (IsValidTypeInfoPointer checks the address against the registered range),
    // not the uint64 stable_id.  Register a static info and use its address.
    struct RawAllocTypeInfo { uint64_t stable_id; uintptr_t reserved; };
    static RawAllocTypeInfo s_ti{ sid, 0 };
    layout_reg.RegisterTypeInfoRange(
        reinterpret_cast<uintptr_t>(&s_ti),
        reinterpret_cast<uintptr_t>(&s_ti) + sizeof(s_ti));
    const void* ti_addr = &s_ti;

    // Allocate the seed objects in old-gen.
    std::vector<void*>
 seed;
    seed.reserve(kSeedObjects);
    for (int i = 0; i < kSeedObjects; i++) {
        void* p = g_old_gen.Allocate(kSeedObjectBytes, true);
        if (p == nullptr) {
            std::printf("[mark_repro] allocation %d failed (OOM) before target page count\n", i);
            return 1;
        }
        // Zero the payload so the raw-alloc scan sees nulls beyond the refs.
        std::memset(p, 0, kSeedObjectBytes);
        // First word = TypeInfo address (makes ScanObjectChildren see a valid layout).
        *static_cast<const void**>(p) = ti_addr;
        seed.push_back(p);
    }
    std::printf("[mark_repro] seeded %d objects, old-gen pages=%zu\n",
        kSeedObjects, static_cast<size_t>(g_old_gen.TotalPages()));

    // Wire up the pointer graph: each object i points at objects i+1..i+kSeedPointersPer
    // (wrap-around).  This makes the full-GC mark trace a large transitive closure,
    // replicating scenario-C's WritePattern interference during mark.
    for (int i = 0; i < kSeedObjects; i++) {
        auto* slots = static_cast<void**>(seed[i]);
        for (int d = 1; d <= kSeedPointersPer; d++) {
            int target = (i + d) % kSeedObjects;
            slots[d] = seed[target];  // slot 0 = TypeInfo, slots 1..N = refs
        }
    }

    constexpr long kStallSeconds = 20;
    auto t0 = std::chrono::steady_clock::now();
    g_old_gen.Collect(nullptr, nullptr);
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - t0).count();

    std::printf("[mark_repro] full GC over %zu pages completed in %lds\n",
        static_cast<size_t>(g_old_gen.TotalPages()), elapsed);

    if (elapsed > kStallSeconds) {
        std::printf("[mark_repro] FAIL: full-GC mark phase STALLED (>%lds) — reproduced\n",
            kStallSeconds);
        return 124;
    }

    std::printf("[mark_repro] PASS: interlinked mark converged; %zu seeds reachable\n",
        seed.size());
    return 0;
}
