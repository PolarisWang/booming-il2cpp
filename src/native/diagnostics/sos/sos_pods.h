/// sos_pods.h — Platform-neutral POD mirrors of GC runtime structs.
///
/// These structs are the SOS extension's *contract* with the GC runtime: the
/// debugger reads raw target-process memory and reinterprets it through these
/// layouts, so every field order/size here must match the real structs in
/// src/native/runtime-core/gc/gc_stats.h byte for byte.
///
/// They deliberately live in their own header rather than inside gc_sos.cpp:
///
///   1. The contract is verifiable on EVERY platform.  sos_test.cpp asserts
///      these PODs against the real structs, and that check is worth running
///      wherever the runtime is built — a layout drift is a runtime bug
///      regardless of which debugger you happen to use.  Keeping the PODs in
///      gc_sos.cpp forced the test to unity-build that whole TU, which dragged
///      the WinDbg/DbgEng implementation (and <windows.h>) into Linux builds
///      and broke them.
///
///   2. Separation of contract from mechanism.  gc_sos.cpp/jit_sos.cpp are the
///      WinDbg *implementation*; these layouts are not.
///
/// Nothing here may depend on DbgEng or on any Windows header.  Only the
/// implementation files do.

#ifndef CHAOS_IL2CPP_SOS_PODS_H_
#define CHAOS_IL2CPP_SOS_PODS_H_

#include <cstdint>

// ══════════════════════════════════════════════════════════════════════════
// POD mirror of GcStats (gc_stats.h)
//
// std::atomic<uint64_t> is layout-compatible with uint64_t on the
// implementations we target, so the atomic fields are mirrored as plain
// uint64_t.  Field order and sizes must track GcStats exactly — sos_test.cpp
// enforces that at compile/test time.
// ══════════════════════════════════════════════════════════════════════════

#pragma pack(push, 8)
struct GcStatsPod {
    uint64_t young_collections;
    uint64_t young_objects_promoted;
    uint64_t young_bytes_promoted;
    uint64_t young_bytes_reclaimed;
    uint64_t young_cards_scanned;
    uint64_t young_pause_ns;
    uint64_t full_collections;
    uint64_t full_pages_collected;
    uint64_t full_objects_marked;
    uint64_t full_bytes_reclaimed;
    uint64_t full_finalizers_run;
    uint64_t finalization_pending_count;
    uint64_t full_pause_ns;
    uint64_t gen1_collections;
    uint64_t gen1_objects_promoted;
    uint64_t gen1_bytes_promoted;
    uint64_t gen1_bytes_reclaimed;
    uint64_t gen1_pause_ns;
    int32_t last_compacted;
    int32_t last_concurrent;
    int32_t last_gc_generation;
    uint32_t _padding0;
    uint64_t gc_index;
    uint64_t alloc_total;
    uint64_t alloc_bytes;
    uint64_t alloc_oversized;
};
#pragma pack(pop)

// GcEventEntry — ring buffer entry layout (mirrors gc_stats.h).
struct GcEventEntryPod {
    uint8_t  is_full_gc;   // bool (1 byte) + 7 padding
    uint8_t  _pad[7];
    uint64_t pause_ns;
    uint64_t objects_processed;
    uint64_t bytes_reclaimed;
};

// ══════════════════════════════════════════════════════════════════════════
// Constants matching the GC runtime
//
// kGcRingSize / kGcBucketCount are asserted equal to the runtime's own
// kGcEventRingSize / kGcPauseBucketCount by sos_test.cpp.
// ══════════════════════════════════════════════════════════════════════════

static constexpr int kGcRingSize     = 64;
static constexpr int kGcBucketCount  = 6;

// Human-readable pause-time bucket labels, parallel to the runtime's bucket
// bounds.  Lives here (not in the WinDbg command that prints it) because it is
// part of what sos_test.cpp verifies.
static const char* kBucketNames[] = {
    "0-1 ms", "1-5 ms", "5-10 ms", "10-50 ms",
    "50-100 ms", "100+ ms"
};

#endif  // CHAOS_IL2CPP_SOS_PODS_H_
