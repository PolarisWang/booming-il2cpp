#ifndef CHAOS_IL2CPP_GC_DEBUG_CONTRACT_H_
#define CHAOS_IL2CPP_GC_DEBUG_CONTRACT_H_

/// gc_debug_contract.h — CRAG GC debug data contract for SOS extension
///
/// Defines extern "C" globals that the chaos_gc_sos WinDbg extension finds
/// via IDebugSymbols::GetOffsetByName().  These provide stable C-linkage
/// entry points to locate GC runtime data structures in the target process.
///
/// The SOS extension reads these pointer values, then calls ReadVirtual to
/// reconstruct GC state using the struct layouts from gc_stats.h.

#include <cstdint>

extern "C" {

/// Pointer to the GcStats instance (chaos::il2cpp::runtime_core::g_gc_stats).
/// SOS reads this to find and interpret all GC counters.
extern void* g_chaos_gc_stats_ptr;

/// Pointer to the GC event ring buffer first element.
/// Ring entries are GcEventEntry structs.
extern void* g_chaos_gc_event_ring_ptr;

/// Number of entries in the event ring buffer (kGcEventRingSize = 64).
extern int32_t g_chaos_gc_event_ring_size;

/// Pointer to the ring buffer head index (std::atomic<int>).
extern void* g_chaos_gc_event_ring_head_ptr;

/// Pointer to the pause histogram array first element (std::atomic<uint64_t>[]).
extern void* g_chaos_gc_pause_histogram_ptr;

/// Number of histogram buckets (kGcPauseBucketCount = 6).
extern int32_t g_chaos_gc_pause_bucket_count;

/// Pointer to the bucket bounds array (kGcPauseBucketsNs, uint64_t[]).
extern void* g_chaos_gc_pause_bucket_bounds_ptr;

}  // extern "C"

#endif  // CHAOS_IL2CPP_GC_DEBUG_CONTRACT_H_
