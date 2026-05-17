// GC allocation stubs — GcAllocate and GcAllocateAtomic are exposed from
// gc_helpers.h but need NurseryAllocate/NurseryAllocateAtomic (gc_region.h)
// to be defined first, so this file is included later in the TU chain.

#include <chaos/profile.h>

#include "gc_region.h"
#include "gc_stats.h"

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

void* GcAllocate(CHAOS_IL2CPP_SIZE size) {
    CHAOS_IL2CPP_PROFILE_SCOPE("GcAllocate");
    void* ptr = NurseryAllocate(size);
    if (ptr) GcRecordAlloc(size, size > kMaxNurseryAlloc);
    return ptr;
}

void* GcAllocateAtomic(CHAOS_IL2CPP_SIZE size) {
    CHAOS_IL2CPP_PROFILE_SCOPE("GcAllocateAtomic");
    void* ptr = NurseryAllocateAtomic(size);
    if (ptr) GcRecordAlloc(size, size > kMaxNurseryAlloc);
    return ptr;
}

}  // namespace chaos::il2cpp::runtime_core
