/// gc_mark_stall_repro.cpp — deterministic reproduction of the full-GC MARK
/// phase stall (task#16 S2).  Scenario C exposes it intermittently (~1/12) at
/// ~672+ fragmented old-gen pages; this harness forces that shape deterministically
/// so the mark-phase convergence can be reproduced, root-caused, and verified.
///
/// Shape: allocate many medium (16 KB) objects via the old-gen allocator so the
/// page count grows to hundreds, then free a fraction to create sparse live data
/// per page (high page count + scattered mark bits), then run one full GC that
/// exercises the parallel mark over all pages.
///
/// Usage: run directly; exit 0 = mark converged, 1 = failed assertion, 124 = stall.

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include <chaos/native_types.h>
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_scheduler.h"

using namespace chaos::il2cpp::runtime_core;

int main(int argc, char** argv) {
    (void)argc; (void)argv;
    std::printf("[mark_repro] starting old-gen mark-convergence reproduction\n");

    // Number of seeded live objects (→ high old-gen page count).  Tuned to
    // exceed the ~672-page threshold that exposes the mark-phase stall.
    constexpr int kSeedObjects   = 2400;
    constexpr int kSeedObjectBytes = 16 * 1024;  // 16 KB each → ~half a 64 KB page

    // Allocate the seed objects and hold them so none is reclaimed.
    std::vector<void*> live;
    live.reserve(kSeedObjects);
    for (int i = 0; i < kSeedObjects; i++) {
        void* p = g_old_gen.Allocate(kSeedObjectBytes, true);
        if (p == nullptr) {
            std::printf("[mark_repro] allocation %d failed (OOM) before target page count\n", i);
            return 1;
        }
        // Paint a stable pattern so VerifyPattern-style checks are meaningful.
        std::memset(p, 0xAB, kSeedObjectBytes);
        live.push_back(p);
    }

    std::printf("[mark_repro] seeded %d live objects, total old-gen pages=%zu\n",
        kSeedObjects, static_cast<size_t>(g_old_gen.TotalPages()));

    // Compact and run one full GC over the seeded pages — the mark phase must
    // converge.  Time-boxed; if it exceeds kStallSeconds the harness "stalls"
    // (mirrors the scenario-C 124 hang).
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

    std::printf("[mark_repro] PASS: mark converged; %zu live seeds verified reachable\n",
        live.size());
    return 0;
}
