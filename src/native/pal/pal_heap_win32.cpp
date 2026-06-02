// pal_heap_win32.cpp — Win32 Heap API wrapper

#include <chaos/pal/pal_heap.h>

#define NOMINMAX
#include <windows.h>

namespace chaos::il2cpp::pal {

void* PalHeapCreate() noexcept {
    return ::HeapCreate(0u, 0u, 0u);
}

void* PalHeapAlloc(void* heap, size_t size) noexcept {
    if (!heap) return nullptr;
    return ::HeapAlloc(static_cast<HANDLE>(heap), 0u, size);
}

void* PalHeapRealloc(void* heap, void* ptr, size_t new_size) noexcept {
    if (!heap) return nullptr;
    return ::HeapReAlloc(static_cast<HANDLE>(heap), 0u, ptr, new_size);
}

void PalHeapFree(void* heap, void* ptr) noexcept {
    if (heap && ptr) {
        ::HeapFree(static_cast<HANDLE>(heap), 0u, ptr);
    }
}

size_t PalHeapSize(void* heap, void* ptr) noexcept {
    if (!heap || !ptr) return 0;
    return ::HeapSize(static_cast<HANDLE>(heap), 0u, ptr);
}

void PalHeapDestroy(void* heap) noexcept {
    if (heap) {
        ::HeapDestroy(static_cast<HANDLE>(heap));
    }
}

}  // namespace chaos::il2cpp::pal
