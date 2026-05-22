#ifndef CHAOS_IL2CPP_GC_HEAP_H_
#define CHAOS_IL2CPP_GC_HEAP_H_

/// @file gc_heap.h
/// Per-heap state bundle and accessor functions for Server GC mode (G-4).
///
/// In WKS mode (CHAOS_IL2CPP_GC_SERVER=0, the default), accessor functions
/// G_YoungGen(), G_OldGen(), etc. return the single process-wide global
/// references — zero overhead, same as direct global access.
///
/// In Server GC mode (CHAOS_IL2CPP_GC_SERVER=1), each thread picks a heap
/// based on NUMA node affinity via tls_current_heap, and the accessors
/// route through that per-heap instance.

#include <cstdint>

#include "gc_gen1.h"
#include "gc_loh.h"
#include "gc_old_gen.h"
#include "gc_scheduler.h"
#include "gc_young_gen.h"

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// GcHeapContext — per-heap state bundle
// ======================================================================

/// Per-heap GC state.  In Server GC mode, each NUMA node gets its own
/// heap context with independent young gen, old gen, LOH, and scheduler.
struct GcHeapContext {
    int heap_id;                    ///< 0..num_heaps-1
    int numa_node;                  ///< NUMA node this heap is bound to

    // Per-heap GC subsystems
    YoungGeneration young_gen;      ///< Nursery + TLAB management
    MarkSweepOldGen old_gen;        ///< Tenured generation mark-sweep
    LargeObjectHeap loh;            ///< Large object heap (objects > 85 KB)
    GcScheduler scheduler;          ///< GC triggering decisions
    Gen1State gen1_state;           ///< Gen1 collection state
};

// ======================================================================
// TLS pointer — which heap does the current thread use?
// ======================================================================

/// Per-thread pointer to the assigned heap context.
/// Set during thread attach (RegisterThread / InitThreadForGC) based on
/// the thread's current NUMA node.  nullptr for non-GC threads.
/// Always non-null for threads that call GcAllocate().
extern thread_local GcHeapContext* tls_current_heap;

// ======================================================================
// Accessor functions — route to the correct heap instance
// ======================================================================

// ── WKS mode (default) — direct global access, zero overhead ──────────
#if !CHAOS_IL2CPP_GC_SERVER

inline YoungGeneration& G_YoungGen() noexcept { return g_young_gen; }
inline MarkSweepOldGen& G_OldGen() noexcept { return g_old_gen; }
inline LargeObjectHeap& G_Loh() noexcept { return g_loh; }
inline GcScheduler& G_Scheduler() noexcept { return g_gc_scheduler; }
inline Gen1State& G_Gen1State() noexcept { return g_gen1_state; }

// ── Server GC mode — young gen + Gen1 are SHARED, rest per-heap ────────
#else

// Young generation and Gen1 survivor space are shared across all heaps.
// Only OldGen, LOH, and Scheduler are per-NUMA-node.
inline YoungGeneration& G_YoungGen() noexcept { return g_young_gen; }
inline MarkSweepOldGen& G_OldGen() noexcept { return tls_current_heap->old_gen; }
inline LargeObjectHeap& G_Loh() noexcept { return tls_current_heap->loh; }
inline GcScheduler& G_Scheduler() noexcept { return tls_current_heap->scheduler; }
inline Gen1State& G_Gen1State() noexcept { return g_gen1_state; }

#endif  // CHAOS_IL2CPP_GC_SERVER

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_HEAP_H_
