#ifndef CHAOS_IL2CPP_GC_HEAP_MANAGER_H_
#define CHAOS_IL2CPP_GC_HEAP_MANAGER_H_

/// @file gc_heap_manager.h
/// GcHeapManager — owns the array of per-NUMA-node heaps.
///
/// In Server GC mode, the heap manager initializes N heaps (one per NUMA
/// node), binds each heap's allocation to its node's virtual memory, and
/// provides thread-to-heap mapping via HeapForCurrentThread().
///
/// In WKS mode (CHAOS_IL2CPP_GC_SERVER=0), this file is unused — the
/// traditional globals (g_young_gen, g_old_gen, etc.) remain the sole
/// heap state.

#include <cstdint>
#include <memory>

#include "gc_heap.h"
#include "gc_numa.h"

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// GcHeapManager — server-mode heap lifecycle and thread mapping
// ======================================================================

class GcHeapManager {
public:
    /// Singleton access (lazily constructed on first call).
    static GcHeapManager& Instance() noexcept {
        static GcHeapManager mgr;
        return mgr;
    }

    GcHeapManager(const GcHeapManager&) = delete;
    GcHeapManager& operator=(const GcHeapManager&) = delete;

    // ── Lifecycle ──────────────────────────────────────────────────

    /// Initialize all heaps.  Called once at runtime startup.
    /// @param num_heaps  0 = auto-detect from NUMA nodes / CPU topology.
    void Initialize(int num_heaps = 0) noexcept;

    /// Return the number of active heaps.
    int HeapCount() const noexcept { return heap_count_; }

    /// GC-N9 (=M3B): dynamically grow/shrink the heap set at runtime.
    /// Server GC only (WKS no-op returns false).  Growing allocates a larger
    /// per-heap array and preserves existing heap contexts; shrinking reduces
    /// the count (clamped to >= 1) so the pool scales with load.  Called under
    /// a safepoint so no thread is mid-allocation across a reallocation.
    /// @param new_count  Target number of heaps (clamped to [1, kMaxServerHeaps]).
    /// @return true if the heap count changed, false if WKS mode or no-op.
    bool AdjustHeapCount(int new_count) noexcept;

    /// Access a specific heap by index.
    GcHeapContext& GetHeap(int heap_id) noexcept {
        return *heaps_[heap_id];
    }

    // ── Thread mapping ─────────────────────────────────────────────

    /// Determine which heap the calling thread should use, based on its
    /// current NUMA node.  Falls back to heap 0 if NUMA is unavailable.
    int HeapForCurrentThread() noexcept;

    // ── Iteration ──────────────────────────────────────────────────

    /// Apply @a fn to each heap in order.
    template <typename Fn>
    void ForEachHeap(Fn&& fn) noexcept {
        for (int i = 0; i < heap_count_; i++) {
            fn(i, *heaps_[i]);
        }
    }

private:
    GcHeapManager() = default;
    ~GcHeapManager() = default;

#if CHAOS_IL2CPP_GC_SERVER
    /// Bounds for AdjustHeapCount: never more than this many server heaps.
    static constexpr int kMaxServerHeaps = 64;
#endif

    int heap_count_{0};
    // Array of owning pointers to each heap context.  GcHeapContext is
    // non-copyable/non-movable (contains mutexes + vectors), so heap objects
    // live at stable addresses and GC-N9's grow/shrink only reallocates the
    // pointer array (never moves a live heap).
    std::unique_ptr<std::unique_ptr<GcHeapContext>[]> heaps_;
};

// ── Thread helpers ────────────────────────────────────────────────

/// Bind the calling thread to its assigned heap based on NUMA node.
/// No-op in WKS mode (tls_current_heap is unused).
inline void SetThreadHeap() noexcept {
#if CHAOS_IL2CPP_GC_SERVER
    auto& mgr = GcHeapManager::Instance();
    tls_current_heap = &mgr.GetHeap(mgr.HeapForCurrentThread());
#endif
}

/// Clear the calling thread's heap binding.
/// No-op in WKS mode.
inline void ClearThreadHeap() noexcept {
#if CHAOS_IL2CPP_GC_SERVER
    tls_current_heap = nullptr;
#endif
}

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_HEAP_MANAGER_H_
