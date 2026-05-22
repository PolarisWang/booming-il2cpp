/// @file gc_coordinator.cpp
/// GcCoordinator implementation — multi-heap GC orchestration.
///
/// Design: Young generation and Gen1 are SHARED across all heaps (single
/// nursery + single survivor space).  Only old gen, LOH, and scheduler are
/// per-heap.  This avoids the complexity of per-heap root scanning and
/// card-table partitioning while still enabling parallel old-gen GC.

#include "gc_coordinator.h"

#include <chaos/log.h>

#include "gc_gen1.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_young_collector.h"
#include "gc_young_gen.h"
#include "gc_helpers.h"
#include "thread_state.h"

namespace chaos::il2cpp::runtime_core {

void GcCoordinator::RequestGlobalGc() noexcept {
    // Prevent re-entrant GC requests.
    bool expected = false;
    if (!gc_active_.compare_exchange_strong(expected, true,
            std::memory_order_acq_rel, std::memory_order_acquire)) {
        CHAOS_IL2CPP_LOG_DEBUG("GC", "coordinator: gc already active, skipping");
        return;
    }

#if CHAOS_IL2CPP_GC_SERVER
    ExecuteMultiHeapGc();
#else
    (void)0;  // WKS: chaos_gc_collect handles this directly.
#endif

    gc_active_.store(false, std::memory_order_release);
}

#if CHAOS_IL2CPP_GC_SERVER

void GcCoordinator::ExecuteMultiHeapGc() noexcept {
    CHAOS_IL2CPP_LOG_DEBUG("GC", "coordinator: multi-heap GC start");

    auto& mgr = GcHeapManager::Instance();
    int n_heaps = mgr.HeapCount();

    // Acquire global safepoint to freeze all managed threads.
    uint32_t gen = threading::RequestGlobalSafepoint();

    // ── Phase 1: Young GC + Gen1 (shared, run once) ────────────────
    // Young generation and Gen1 are shared across all heaps.
    Region* young_region = g_young_gen.region.load(std::memory_order_acquire);
    void* bump = g_young_gen.bump.load(std::memory_order_acquire);
    if (young_region != nullptr && bump > young_region->begin) {
        GcYoungCollection();
    }
    Region* gen1_region = g_young_gen.gen1_region.load(std::memory_order_acquire);
    if (gen1_region != nullptr) {
        char* s_bump = g_young_gen.gen1_bump.load(std::memory_order_acquire);
        if (s_bump > gen1_region->begin) {
            GcGen1Collection();
        }
    }

    // ── Phase 2: Old-gen full GC per heap (parallel) ──────────────
    if (n_heaps <= 1) {
        mgr.ForEachHeap([](int /*id*/, GcHeapContext& ctx) {
            tls_current_heap = &ctx;
            G_OldGen().Collect(nullptr, nullptr);
        });
    } else {
        GcWorkerPool::Instance().RunWorkers(n_heaps, [&](int worker_idx) {
            if (worker_idx < n_heaps) {
                tls_current_heap = &mgr.GetHeap(worker_idx);
                G_OldGen().Collect(nullptr, nullptr);
            }
        });
    }

    // Restore this thread's heap binding.
    SetThreadHeap();

    // Release safepoint — threads resume.
    threading::ReleaseGlobalSafepoint(gen);

    CHAOS_IL2CPP_LOG_DEBUG("GC", "coordinator: multi-heap GC complete");
}

#endif  // CHAOS_IL2CPP_GC_SERVER

}  // namespace chaos::il2cpp::runtime_core
