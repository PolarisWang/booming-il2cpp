// pal_heap.h — Heap abstraction for domain memory allocation
#pragma once

#include <cstddef>

namespace chaos::il2cpp::pal {

/// Create a private heap for domain memory allocations.
/// On Windows: calls HeapCreate.  On POSIX: returns a sentinel (uses malloc/free).
/// Returns nullptr on failure.
void* PalHeapCreate() noexcept;

/// Allocate memory from a heap created by PalHeapCreate.
void* PalHeapAlloc(void* heap, size_t size) noexcept;

/// Reallocate memory within a heap.
void* PalHeapRealloc(void* heap, void* ptr, size_t new_size) noexcept;

/// Free memory allocated from a heap.
void PalHeapFree(void* heap, void* ptr) noexcept;

/// Get the usable size of a heap allocation. Returns 0 if unknown.
size_t PalHeapSize(void* heap, void* ptr) noexcept;

/// Destroy a heap and all its allocations.
void PalHeapDestroy(void* heap) noexcept;

}  // namespace chaos::il2cpp::pal
