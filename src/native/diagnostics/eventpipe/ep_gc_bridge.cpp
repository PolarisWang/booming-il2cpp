// ep_gc_bridge.cpp — EventPipe GC event bridge implementation
//
// Reads real GC statistics from the global GcStats counters (g_gc_stats)
// and emits EventPipe GC events with per-event data.
//
// Data flow:
//   GcFireEvent(GcEvent) fires AFTER GcRecord*Collection() for DONE events,
//   so g_gc_stats counters are already updated.  Per-event values are
//   extracted by computing deltas from a previous-counter snapshot.
//
//   For GC_GEN1_COLLECT the event fires BEFORE GcRecordGen1Collection();
//   gen1 data uses the previous collection's stats (one-event lag).

#include "ep_gc_bridge.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

#include "ep_event.h"

#include <gc/gc_events.h>
#include <gc/gc_stats.h>

#include <cstdint>

namespace chaos::il2cpp::diagnostics {

namespace {

// ── Previous-counter snapshot for delta computation ────────────────
// GcStats accumulates totals.  To get per-event values we store the
// previous counter values and compute the delta at each callback.
// All reads happen inside a GC safepoint (STW), so no atomic races.

struct GcStatsDelta {
    uint64_t young_collections{0};
    uint64_t young_objects_promoted{0};
    uint64_t young_bytes_promoted{0};
    uint64_t young_bytes_reclaimed{0};
    uint64_t young_pause_ns{0};

    uint64_t full_collections{0};
    uint64_t full_pages_collected{0};
    uint64_t full_objects_marked{0};
    uint64_t full_bytes_reclaimed{0};
    uint64_t full_pause_ns{0};

    uint64_t gen1_collections{0};
    uint64_t gen1_objects_promoted{0};
    uint64_t gen1_bytes_promoted{0};
    uint64_t gen1_bytes_reclaimed{0};
    uint64_t gen1_pause_ns{0};
};

GcStatsDelta g_prev;

// ── GC event callback ─────────────────────────────────────────────
// Fires inside GC safepoint (STW).  Must be non-blocking.

void EpGcCallback(runtime_core::GcEvent event, void* /*user_data*/) noexcept {
    using namespace runtime_core;

    switch (event) {
    case runtime_core::GcEvent::GC_START:
        EpEmitGcStart(static_cast<uint32_t>(
            g_gc_stats.last_gc_generation.load(std::memory_order_relaxed)));
        break;

    case runtime_core::GcEvent::GC_END: {
        uint64_t cur_reclaimed = g_gc_stats.full_bytes_reclaimed.load(std::memory_order_relaxed);
        uint64_t cur_pause     = g_gc_stats.full_pause_ns.load(std::memory_order_relaxed);
        uint64_t delta_reclaimed = cur_reclaimed - g_prev.full_bytes_reclaimed;
        uint64_t delta_pause     = cur_pause - g_prev.full_pause_ns;
        g_prev.full_bytes_reclaimed = cur_reclaimed;
        g_prev.full_pause_ns = cur_pause;
        EpEmitGcEnd(delta_pause, delta_reclaimed);
        break;
    }

    case runtime_core::GcEvent::GC_YOUNG_START:
        // Nursery used is available locally in the GC code but not stored
        // globally.  Use 0 as the GC knows its own nursery size.
        EpEmitGcYoungStart(0);
        break;

    case runtime_core::GcEvent::GC_YOUNG_DONE: {
        uint64_t cur_objects  = g_gc_stats.young_objects_promoted.load(std::memory_order_relaxed);
        uint64_t cur_bytes_pr = g_gc_stats.young_bytes_promoted.load(std::memory_order_relaxed);
        uint64_t cur_bytes_re = g_gc_stats.young_bytes_reclaimed.load(std::memory_order_relaxed);
        uint64_t cur_pause    = g_gc_stats.young_pause_ns.load(std::memory_order_relaxed);

        uint64_t delta_objects  = cur_objects - g_prev.young_objects_promoted;
        uint64_t delta_bytes_pr = cur_bytes_pr - g_prev.young_bytes_promoted;
        uint64_t delta_bytes_re = cur_bytes_re - g_prev.young_bytes_reclaimed;
        uint64_t delta_pause    = cur_pause - g_prev.young_pause_ns;

        g_prev.young_objects_promoted = cur_objects;
        g_prev.young_bytes_promoted   = cur_bytes_pr;
        g_prev.young_bytes_reclaimed  = cur_bytes_re;
        g_prev.young_pause_ns         = cur_pause;

        EpEmitGcYoungEnd(delta_pause, delta_objects, delta_bytes_pr, delta_bytes_re);
        break;
    }

    case runtime_core::GcEvent::GC_FULL_START: {
        uint32_t page_count = static_cast<uint32_t>(
            g_gc_stats.full_pages_collected.load(std::memory_order_relaxed));
        EpEmitGcFullStart(page_count);
        break;
    }

    case runtime_core::GcEvent::GC_FULL_DONE: {
        uint64_t cur_reclaimed  = g_gc_stats.full_bytes_reclaimed.load(std::memory_order_relaxed);
        uint64_t cur_marked     = g_gc_stats.full_objects_marked.load(std::memory_order_relaxed);
        uint64_t cur_pages      = g_gc_stats.full_pages_collected.load(std::memory_order_relaxed);
        uint64_t cur_pause      = g_gc_stats.full_pause_ns.load(std::memory_order_relaxed);

        uint64_t delta_reclaimed = cur_reclaimed - g_prev.full_bytes_reclaimed;
        uint64_t delta_marked    = cur_marked - g_prev.full_objects_marked;
        uint64_t delta_pages     = cur_pages - g_prev.full_pages_collected;
        uint64_t delta_pause     = cur_pause - g_prev.full_pause_ns;

        g_prev.full_bytes_reclaimed = cur_reclaimed;
        g_prev.full_objects_marked  = cur_marked;
        g_prev.full_pages_collected = cur_pages;
        g_prev.full_pause_ns        = cur_pause;

        EpEmitGcFullEnd(delta_pause, delta_reclaimed, delta_marked, delta_pages);
        break;
    }

    case runtime_core::GcEvent::GC_OOM:
        EpEmitGcOom();
        break;

    case runtime_core::GcEvent::GC_GEN1_COLLECT: {
        // GC_GEN1_COLLECT fires inside GcGen1Collection() at the START,
        // BEFORE GcRecordGen1Collection() is called by the parent.
        // Use the PREVIOUS collection's delta (since the cumulative
        // counters still reflect prior collections at this point).
        uint64_t cur_collections   = g_gc_stats.gen1_collections.load(std::memory_order_relaxed);
        uint64_t cur_objects       = g_gc_stats.gen1_objects_promoted.load(std::memory_order_relaxed);
        uint64_t cur_bytes_pr      = g_gc_stats.gen1_bytes_promoted.load(std::memory_order_relaxed);
        uint64_t cur_bytes_re      = g_gc_stats.gen1_bytes_reclaimed.load(std::memory_order_relaxed);
        uint64_t cur_pause         = g_gc_stats.gen1_pause_ns.load(std::memory_order_relaxed);

        uint64_t delta_objects   = cur_objects - g_prev.gen1_objects_promoted;
        uint64_t delta_bytes_pr  = cur_bytes_pr - g_prev.gen1_bytes_promoted;
        uint64_t delta_bytes_re  = cur_bytes_re - g_prev.gen1_bytes_reclaimed;
        uint64_t delta_pause     = cur_pause - g_prev.gen1_pause_ns;

        // Update previous snapshot NOW (before GcRecordGen1Collection).
        // The delta will reflect the PREVIOUS gen1 collection's data.
        g_prev.gen1_objects_promoted  = cur_objects;
        g_prev.gen1_bytes_promoted    = cur_bytes_pr;
        g_prev.gen1_bytes_reclaimed   = cur_bytes_re;
        g_prev.gen1_pause_ns          = cur_pause;
        g_prev.gen1_collections       = cur_collections;

        EpEmitGcGen1Collect(delta_pause, delta_objects, delta_bytes_re);
        break;
    }

    default:
        // MARK_DONE, SWEEP_DONE, COMPACT_DONE: not emitted via EventPipe
        // (these are internal GC phase markers, not diagnostic events).
        break;
    }
}

}  // anonymous namespace

void EpGcBridgeInitialize() noexcept {
    runtime_core::GcRegisterEventCallback(EpGcCallback, nullptr);
}

void EpGcBridgeShutdown() noexcept {
    // GcEventCallbacks cannot be individually unregistered in the current
    // API.  The GC will stop firing during shutdown naturally.
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EVENTPIPE
