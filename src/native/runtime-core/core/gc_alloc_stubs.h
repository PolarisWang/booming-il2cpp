// gc_alloc_stubs.h — forceinline fast-path GcAllocate for CHAOS_IL2CPP_NEW_GC
//
// The inline definitions here replace the out-of-line GcAllocate/GcAllocateAtomic
// from gc_alloc_stubs.cpp for hot allocation paths.  Generated code that uses
// CHAOS_IL2CPP_NEW_GC macro gets a fully inlined TLAB bump sequence with NO:
//   - PROFILE_SCOPE (profiling overhead moved to GcAllocateProfiled)
//   - Global atomic fetch_add (replaced by TLS counters, flushed at GC time)
//   - GcEtwRecordAlloc (guarded by CHAOS_IL2CPP_GC_EVENTS, only in CHECK config)
//
// The out-of-line GcAllocate in gc_alloc_stubs.cpp is kept for non-hot-path
// callers (e.g., jit_helpers.cpp) and renamed to GcAllocateProfiled — it
// retains PROFILE_SCOPE and full stats for diagnostic builds.

#ifndef CHAOS_IL2CPP_GC_ALLOC_STUBS_H_
#define CHAOS_IL2CPP_GC_ALLOC_STUBS_H_

#include <chaos/native_types.h>
#include <chaos/compiler_hints.h>

#include "gc/gc_region.h"
#include "gc/gc_stats.h"
#include "gc/gc_stress.h"
#include "profile_stats.h"

namespace chaos::il2cpp::runtime_core {

// ── TLS fast-path allocation counters ──────────────────────────────
// Replaces global atomic g_gc_stats.alloc_total/alloc_bytes on the hot path.
// Flushed to globals in FlushTlsFastStats() (called from GC collect path).
extern thread_local CHAOS_IL2CPP_SIZE tls_alloc_fast_count;
extern thread_local CHAOS_IL2CPP_SIZE tls_alloc_fast_bytes;

/// Fast-path GcAllocate — inlined at every CHAOS_IL2CPP_NEW_GC call site.
/// No PROFILE_SCOPE, no global atomics, no ETW — pure TLAB bump + zero-init.
/// ~30ns/alloc (SHIP) vs ~78ns old out-of-line path.
CHAOS_IL2CPP_FORCEINLINE void* GcAllocateFast(CHAOS_IL2CPP_SIZE size) {
    if (GcStressShouldTrigger()) [[unlikely]] {
        tls_in_gc_stress = true;
        chaos_gc_collect();
        tls_in_gc_stress = false;
    }

    void* ptr = NurseryAllocate(size);
    if (ptr) {
        tls_alloc_fast_count++;
        tls_alloc_fast_bytes += size;
#if CHAOS_IL2CPP_PROFILE_ENABLED
        ProfileRecordNurseryAlloc(static_cast<int64_t>(size));
        ProfileRecordAllocCount();
        ProfileRecordFastPath();
#endif
    }
    return ptr;
}

/// Fast-path GcAllocateAtomic — same as GcAllocateFast but for pointer-free data.
CHAOS_IL2CPP_FORCEINLINE void* GcAllocateAtomicFast(CHAOS_IL2CPP_SIZE size) {
    if (GcStressShouldTrigger()) [[unlikely]] {
        tls_in_gc_stress = true;
        chaos_gc_collect();
        tls_in_gc_stress = false;
    }

    void* ptr = NurseryAllocateAtomic(size);
    if (ptr) {
        tls_alloc_fast_count++;
        tls_alloc_fast_bytes += size;
#if CHAOS_IL2CPP_PROFILE_ENABLED
        ProfileRecordNurseryAlloc(static_cast<int64_t>(size));
        ProfileRecordAllocCount();
        ProfileRecordFastPath();
#endif
    }
    return ptr;
}

/// Fast-path GcAllocate WITHOUT zero-init — for callers that immediately
/// write every byte (e.g., CHAOS_IL2CPP_MALLOC_GC for array allocations).
CHAOS_IL2CPP_FORCEINLINE void* GcAllocateFastNoZero(CHAOS_IL2CPP_SIZE size) {
    if (GcStressShouldTrigger()) [[unlikely]] {
        tls_in_gc_stress = true;
        chaos_gc_collect();
        tls_in_gc_stress = false;
    }

    void* ptr = NurseryAllocateNoZero(size);
    if (ptr) {
        tls_alloc_fast_count++;
        tls_alloc_fast_bytes += size;
#if CHAOS_IL2CPP_PROFILE_ENABLED
        ProfileRecordNurseryAlloc(static_cast<int64_t>(size));
        ProfileRecordAllocCount();
        ProfileRecordFastPath();
#endif
    }
    return ptr;
}

/// Fast-path GcAllocateAtomic WITHOUT zero-init (atomic/pointer-free variant).
CHAOS_IL2CPP_FORCEINLINE void* GcAllocateAtomicFastNoZero(CHAOS_IL2CPP_SIZE size) {
    if (GcStressShouldTrigger()) [[unlikely]] {
        tls_in_gc_stress = true;
        chaos_gc_collect();
        tls_in_gc_stress = false;
    }

    void* ptr = NurseryAllocateAtomicNoZero(size);
    if (ptr) {
        tls_alloc_fast_count++;
        tls_alloc_fast_bytes += size;
    }
    return ptr;
}

/// Flush TLS fast allocation counters to the global GC stats.
/// Called from GC collect path (NurseryAllocateSlow, GcYoungCollection, etc.)
/// before any GC decision or stats read.
inline void FlushTlsFastStats() noexcept {
    CHAOS_IL2CPP_SIZE count = tls_alloc_fast_count;
    CHAOS_IL2CPP_SIZE bytes = tls_alloc_fast_bytes;
    if (count > 0 || bytes > 0) {
        g_gc_stats.alloc_total.fetch_add(count, std::memory_order_relaxed);
        g_gc_stats.alloc_bytes.fetch_add(bytes, std::memory_order_relaxed);
        // Also flush to the per-thread total used by benchmark/profiling API
        // (chaos_gc_get_allocated_bytes_for_current_thread), so that fast-path
        // allocations are visible even after a GC resets the TLS counters.
        extern thread_local CHAOS_IL2CPP_INT64 tls_total_allocated_bytes;
        tls_total_allocated_bytes += static_cast<CHAOS_IL2CPP_INT64>(bytes);
        tls_alloc_fast_count = 0;
        tls_alloc_fast_bytes = 0;
    }
}

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_ALLOC_STUBS_H_
