// pal_heap_posix.cpp — POSIX malloc-based heap wrapper
//
// On POSIX, PalHeapCreate returns a non-null sentinel (1) to distinguish
// from nullptr (failure). All allocation/free calls delegate to malloc/realloc/free.
// PalHeapDestroy is a no-op since memory was never in a dedicated heap.
// PalHeapSize is not supported on POSIX and returns 0.

#include <chaos/pal/pal_heap.h>

#include <cstdint>
#include <cstdlib>

namespace chaos::il2cpp::pal {

// Sentinel pointer indicating "use malloc/free" — never dereferenced.
static void* const kHeapSentinel = reinterpret_cast<void*>(static_cast<uintptr_t>(1));

void* PalHeapCreate() noexcept {
    return kHeapSentinel;
}

void* PalHeapAlloc(void* heap, size_t size) noexcept {
    if (!heap) return nullptr;
    return std::malloc(size);
}

void* PalHeapRealloc(void* heap, void* ptr, size_t new_size) noexcept {
    if (!heap) return nullptr;
    return std::realloc(ptr, new_size);
}

void PalHeapFree(void* heap, void* ptr) noexcept {
    (void)heap;
    std::free(ptr);
}

size_t PalHeapSize(void* heap, void* ptr) noexcept {
    (void)heap;
    (void)ptr;
    return 0;
}

void PalHeapDestroy(void* heap) noexcept {
    (void)heap;
    // No-op: memory was allocated via malloc/free, not a dedicated heap.
}

}  // namespace chaos::il2cpp::pal
