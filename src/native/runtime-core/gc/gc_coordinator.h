#ifndef CHAOS_IL2CPP_GC_COORDINATOR_H_
#define CHAOS_IL2CPP_GC_COORDINATOR_H_

/// @file gc_coordinator.h
/// GcCoordinator — multi-heap GC coordination for Server GC mode (G-4).
///
/// In Server GC mode (CHAOS_IL2CPP_GC_SERVER=1), when any heap triggers a
/// GC, all heaps must participate.  The coordinator serializes GC entry
/// (via a global safepoint), then executes per-heap GC on the GcWorkerPool.
///
/// In WKS mode (CHAOS_IL2CPP_GC_SERVER=0), the coordinator is a no-op
/// pass-through — the existing single-heap chaos_gc_collect() path runs
/// unchanged.

#include <cstdint>

#include "gc_heap_manager.h"
#include "gc_worker_pool.h"

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// GcCoordinator — global GC orchestration
// ======================================================================

class GcCoordinator {
public:
    /// Singleton access.
    static GcCoordinator& Instance() noexcept {
        static GcCoordinator coord;
        return coord;
    }

    GcCoordinator(const GcCoordinator&) = delete;
    GcCoordinator& operator=(const GcCoordinator&) = delete;

    // ── GC request ─────────────────────────────────────────────────

    /// Request a full GC across all heaps.
    /// In WKS mode: executes the existing chaos_gc_collect() path directly.
    /// In Server GC mode: acquires safepoint, runs per-heap GC on the
    /// worker pool, releases safepoint.
    void RequestGlobalGc() noexcept;

    /// Check if a GC is currently in progress across all heaps.
    bool IsGcActive() const noexcept {
        return gc_active_.load(std::memory_order_acquire);
    }

private:
    GcCoordinator() = default;
    ~GcCoordinator() = default;

    /// Server GC path: execute GC on every heap under safepoint.
    void ExecuteMultiHeapGc() noexcept;

    /// WKS path: delegate to the existing single-heap GC.  No-op here;
    /// the caller (chaos_gc_collect) already handles this.
    void ExecuteSingleHeapGc() noexcept {}

    std::atomic<bool> gc_active_{false};
};

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_COORDINATOR_H_
