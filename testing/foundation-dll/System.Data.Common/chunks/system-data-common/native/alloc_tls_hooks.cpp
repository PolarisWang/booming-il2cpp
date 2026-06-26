// alloc_tls_hooks.cpp — Override GcAllocate/GcAllocateAtomic to track TLS counters.
// Compiled locally via CHAOS_NATIVE_STUBS glob; /FORCE:MULTIPLE ensures our
// definitions override the prebuilt SDK lib's versions (which don't increment
// tls_alloc_fast_bytes in the profiled allocation path).

#include <cstdio>
#include <cstdint>
#include <gc/gc_region.h>
#include <gc/gc_api.h>
#include <gc/gc_stats.h>
#include <gc/gc_stress.h>
#include <core/gc_alloc_stubs.h>
#include <profile_stats.h>

namespace chaos::il2cpp::runtime_core {

// Provide the legacy aliases too, so the linker doesn't pull in
// the SDK's gc_alloc_stubs.obj (which has GcAllocateProfiled
// without TLS counter tracking).
void* GcAllocate(CHAOS_IL2CPP_SIZE size) { return GcAllocateProfiled(size); }
void* GcAllocateAtomic(CHAOS_IL2CPP_SIZE size) { return GcAllocateAtomicProfiled(size); }

void* GcAllocateProfiled(CHAOS_IL2CPP_SIZE size) {
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

void* GcAllocateAtomicProfiled(CHAOS_IL2CPP_SIZE size) {
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

} // namespace chaos::il2cpp::runtime_core
