// gc_etw.h — ETW (Event Tracing for Windows) provider for CRAG GC
//
// Provides Windows ETW instrumentation for GC lifecycle events so that
// Xperf, WPR, PerfView, and other ETW consumers can observe GC behavior.
//
// On non-Windows platforms all functions compile to no-ops with zero
// runtime cost.  On Windows, guarded by CHAOS_IL2CPP_GC_EVENTS (default ON).
//
// Pattern: follows threadpool_events.h/cpp classic ETW approach:
//   - EventRegister/EventWrite/EventUnregister (not TraceLogging)
//   - Lazy provider init on first use
//   - Platform-guarded with #if defined(_WIN32)

#ifndef CHAOS_IL2CPP_GC_ETW_H_
#define CHAOS_IL2CPP_GC_ETW_H_

#include <cstdint>

namespace chaos::il2cpp::runtime_core {

// ── Provider identity ────────────────────────────────────────────────
// GUID: {3A5C4B6D-8E9F-4A2B-9C1D-7E8F0A3B6C5D}
// Used for: CRAG GC diagnostic events (young/full/gen1 collection).
// Keyword: 0x1 = GC lifecycle events.
// Level:   4 (Informational).

/// Initialize the GC ETW provider.
/// Registers with the Windows tracing system. Idempotent (safe to call
/// multiple times — only first call registers).
void GcEtwInitialize() noexcept;

/// Shutdown the GC ETW provider.
/// Unregisters from the Windows tracing system.
void GcEtwShutdown() noexcept;

// ── Event emission functions ─────────────────────────────────────────
// Each corresponds to a GcEvent firing point in the GC code.
// Payload parameters carry the data available at that call site.

/// GCStart: a full GC cycle has begun.
/// @param generation  The generation being collected (1=gen1, 2=full).
void GcEtwFireGcStart(uint32_t generation) noexcept;

/// GCEnd: a full GC cycle has completed.
/// @param pause_ns         Total pause time in nanoseconds.
/// @param reclaimed_bytes  Total bytes reclaimed.
void GcEtwFireGcEnd(uint64_t pause_ns, uint64_t reclaimed_bytes) noexcept;

/// GCYoungStart: a young GC (nursery collection) has begun.
/// @param nursery_used  Bytes used in the nursery before collection.
void GcEtwFireGcYoungStart(uint64_t nursery_used) noexcept;

/// GCYoungEnd: a young GC has completed.
void GcEtwFireGcYoungEnd(
    uint64_t pause_ns,
    uint64_t objects_promoted,
    uint64_t bytes_promoted,
    uint64_t bytes_reclaimed) noexcept;

/// GCFullStart: a full GC is about to begin marking.
/// @param page_count  Number of old-gen pages to collect.
void GcEtwFireGcFullStart(uint32_t page_count) noexcept;

/// GCFullEnd: a full GC has completed.
void GcEtwFireGcFullEnd(
    uint64_t pause_ns,
    uint64_t reclaimed_bytes,
    uint64_t objects_marked,
    uint64_t pages_collected) noexcept;

/// GCOOM: an out-of-memory condition occurred (allocation failed after GC).
void GcEtwFireGcOom() noexcept;

/// GCGen1Collect: a Gen1 mark-sweep collection occurred.
void GcEtwFireGcGen1Collect(
    uint64_t pause_ns,
    uint64_t objects_promoted,
    uint64_t bytes_reclaimed) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_ETW_H_
