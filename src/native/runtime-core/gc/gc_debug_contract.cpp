/// gc_debug_contract.cpp — Initializes extern "C" debug contract globals.
///
/// These C-linkage variables hold addresses of GC runtime data structures,
/// allowing the chaos_gc_sos WinDbg extension to locate and read them
/// from the target process address space.

#include "gc_debug_contract.h"
#include "gc_stats.h"

namespace rc = chaos::il2cpp::runtime_core;

extern "C" {

void* g_chaos_gc_stats_ptr               = &rc::g_gc_stats;
void* g_chaos_gc_event_ring_ptr          = &rc::g_gc_event_ring[0];
int32_t g_chaos_gc_event_ring_size       = rc::kGcEventRingSize;
void* g_chaos_gc_event_ring_head_ptr     = &rc::g_gc_event_ring_head;
void* g_chaos_gc_pause_histogram_ptr     = &rc::g_gc_pause_histogram[0];
int32_t g_chaos_gc_pause_bucket_count    = rc::kGcPauseBucketCount;
void* g_chaos_gc_pause_bucket_bounds_ptr =
    const_cast<uint64_t*>(rc::kGcPauseBucketsNs);

}  // extern "C"
