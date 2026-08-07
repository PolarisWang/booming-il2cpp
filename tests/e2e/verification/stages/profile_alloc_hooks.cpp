// profile_alloc_hooks.cpp — Overrides GcAllocateProfiled/GcAllocateAtomicProfiled
// to increment TLS fast-path allocation counters (tls_alloc_fast_bytes)
// for benchmark/profile use.  Compiled locally and linked via /FORCE:MULTIPLE
// to override the prebuilt SDK lib's stubs (which were compiled without
// CHAOS_IL2CPP_PROFILE_ENABLED and have ProfileRecord* inlined as no-ops).

#include <cstdint>
#include <gc/gc_api.h>
#include <gc/gc_region.h>
#include <gc/gc_stats.h>
#include <gc/gc_stress.h>
#include <profile_stats.h>
#include <core/gc_alloc_stubs.h>

namespace chaos::il2cpp::runtime_core {

void* GcAllocateProfiled(CHAOS_IL2CPP_SIZE size) {
    if (GcStressShouldTrigger()) [[unlikely]] {
        tls_in_gc_stress = true;
        chaos_gc_collect();
        tls_in_gc_stress = false;
    }

    void* ptr = NurseryAllocate(size);
    if (ptr) {
        GcRecordAlloc(size, size > kMaxTlabAlloc);
        ProfileRecordNurseryAlloc(static_cast<int64_t>(size));
        ProfileRecordAllocCount();
        ProfileRecordFastPath();
        tls_alloc_fast_count++;
        tls_alloc_fast_bytes += size;
    } else {
        std::fprintf(stderr, "[alloc-hook] GcAllocateProfiled(%zu) -> NULL (no TLAB space)\n", size);
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
        ProfileRecordNurseryAlloc(static_cast<int64_t>(size));
        ProfileRecordAllocCount();
        ProfileRecordFastPath();
        tls_alloc_fast_count++;
        tls_alloc_fast_bytes += size;
    } else {
        std::fprintf(stderr, "[alloc-hook] GcAllocateAtomicProfiled(%zu) -> NULL\n", size);
    }
    return ptr;
}

} // namespace chaos::il2cpp::runtime_core
