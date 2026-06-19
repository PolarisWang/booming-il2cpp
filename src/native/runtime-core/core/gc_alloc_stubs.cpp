// GC allocation stubs — GcAllocateProfiled/GcAllocateAtomicProfiled are
// non-inline variants that retain PROFILE_SCOPE + GcRecordAlloc for
// diagnostic use (CHECK builds) and non-hot-path callers.
//
// Hot-path allocation goes through GcAllocateFast/GcAllocateAtomicFast
// (__forceinline in gc_alloc_stubs.h), which skips PROFILE_SCOPE and
// uses TLS counters instead of global atomics.

#include <chaos/config.h>
#include <runtime_abi.h>
#include <chaos/profile.h>

#include "gc_region.h"
#include "gc_stats.h"
#include "gc_stress.h"
#include "gc_alloc_stubs.h"
#include "profile_stats.h"

namespace chaos::il2cpp::runtime_core {
namespace {

void* CHAOS_RUNTIME_ABI_CALL DefaultAllocate(CHAOS_IL2CPP_SIZE size, void* user_data) {
    (void)user_data;
    return NurseryAllocate(size);
}

void CHAOS_RUNTIME_ABI_CALL DefaultDeallocate(void* ptr, void* user_data) {
    (void)user_data;
    (void)ptr;
}

}  // anonymous namespace

// Forward declarations (lines 37-38 call these before they're defined)
static void* GcAllocateProfiled(CHAOS_IL2CPP_SIZE size);
static void* GcAllocateAtomicProfiled(CHAOS_IL2CPP_SIZE size);

/// Legacy symbol: GcAllocate — delegates to the profiled variant.
/// Kept for compilation units that include gc_helpers.h but not gc_alloc_stubs.h
/// (e.g., runtime_stubs files compiled separately from the unity build).
/// Hot-path callers use GcAllocateFast (__forceinline in gc_alloc_stubs.h).
void* GcAllocate(CHAOS_IL2CPP_SIZE size) { return GcAllocateProfiled(size); }
void* GcAllocateAtomic(CHAOS_IL2CPP_SIZE size) { return GcAllocateAtomicProfiled(size); }

void* GcAllocateProfiled(CHAOS_IL2CPP_SIZE size) {
    CHAOS_IL2CPP_PROFILE_SCOPE("GcAllocateProfiled");

    // GC Stress mode: force a full GC before allocation.
    if (GcStressShouldTrigger()) [[unlikely]] {
        tls_in_gc_stress = true;
        chaos_gc_collect();
        tls_in_gc_stress = false;
    }

    void* ptr = NurseryAllocate(size);
    if (ptr) {
        GcRecordAlloc(size, size > kMaxTlabAlloc);
        tls_alloc_fast_count++;
        tls_alloc_fast_bytes += size;
        ProfileRecordNurseryAlloc(static_cast<int64_t>(size));
        ProfileRecordAllocCount();
        ProfileRecordFastPath();
    }
    return ptr;
}

static void* GcAllocateAtomicProfiled(CHAOS_IL2CPP_SIZE size) {
    CHAOS_IL2CPP_PROFILE_SCOPE("GcAllocateAtomicProfiled");

    // GC Stress mode: force a full GC before allocation.
    if (GcStressShouldTrigger()) [[unlikely]] {
        tls_in_gc_stress = true;
        chaos_gc_collect();
        tls_in_gc_stress = false;
    }

    void* ptr = NurseryAllocateAtomic(size);
    if (ptr) {
        GcRecordAlloc(size, size > kMaxTlabAlloc);
        tls_alloc_fast_count++;
        tls_alloc_fast_bytes += size;
        ProfileRecordNurseryAlloc(static_cast<int64_t>(size));
        ProfileRecordAllocCount();
        ProfileRecordFastPath();
    }
    return ptr;
}

}  // namespace chaos::il2cpp::runtime_core
