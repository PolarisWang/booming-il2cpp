// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// ── Array stub implementations (real) ───────────────
#include "array_stubs.h"

#include <cstring>
#include "runtime_stubs/stub_common.h"
#include "gc_helpers.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

CHAOS_IL2CPP_INTPTR ChaosArrayEmpty(void) noexcept
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

void ChaosArrayCopy(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INT32 sourceIndex, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT32 destIndex, CHAOS_IL2CPP_INT32 count) noexcept
{
    if (source == 0 || dest == 0 || count <= 0) return;
    auto* src_arr = get_managed_array(source);
    auto* dst_arr = get_managed_array_mut(dest);
    auto si = static_cast<CHAOS_IL2CPP_UINTPTR>(sourceIndex);
    auto di = static_cast<CHAOS_IL2CPP_UINTPTR>(destIndex);
    auto c = static_cast<CHAOS_IL2CPP_UINTPTR>(count);
    if (si > static_cast<CHAOS_IL2CPP_UINTPTR>(src_arr->length) || c > static_cast<CHAOS_IL2CPP_UINTPTR>(src_arr->length) - si) return;
    if (di > static_cast<CHAOS_IL2CPP_UINTPTR>(dst_arr->length) || c > static_cast<CHAOS_IL2CPP_UINTPTR>(dst_arr->length) - di) return;

    if (src_arr->elements == nullptr || dst_arr->elements == nullptr) return;
    const CHAOS_IL2CPP_SIZE elem_size = sizeof(void*);
    std::memmove(
        reinterpret_cast<CHAOS_IL2CPP_UINT8*>(dst_arr->elements) + di * elem_size,
        reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(src_arr->elements) + si * elem_size,
        c * elem_size);
}

CHAOS_IL2CPP_INTPTR ChaosArrayCreateInstance(CHAOS_IL2CPP_INTPTR elementType, CHAOS_IL2CPP_INT32 length) noexcept
{
    (void)elementType;
    (void)length;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

CHAOS_IL2CPP_INT32 ChaosArrayBinarySearch(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (array == 0) return -1;
    const auto* arr = get_managed_array(array);
    if (arr->elements == nullptr) return -1;
    const auto* elements = arr->elements;
    auto len = static_cast<CHAOS_IL2CPP_INT32>(arr->length);
    CHAOS_IL2CPP_INT32 lo = 0;
    CHAOS_IL2CPP_INT32 hi = len - 1;
    while (lo <= hi) {
        CHAOS_IL2CPP_INT32 mid = lo + (hi - lo) / 2;
        if (elements[mid] < value) {
            lo = mid + 1;
        } else if (elements[mid] > value) {
            hi = mid - 1;
        } else {
            return mid;
        }
    }
    return ~lo;
}

/// BinarySearch(array, index, length, value) — searches a sub-range of the array.
CHAOS_IL2CPP_INT32 ChaosArrayBinarySearchRange(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 length, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (array == 0 || index < 0 || length < 0) return -1;
    const auto* arr = get_managed_array(array);
    if (arr->elements == nullptr) return -1;
    auto len = static_cast<CHAOS_IL2CPP_INT32>(arr->length);
    if (index >= len || length > len - index) return -1;
    const auto* elements = arr->elements;
    CHAOS_IL2CPP_INT32 lo = index;
    CHAOS_IL2CPP_INT32 hi = index + length - 1;
    while (lo <= hi) {
        CHAOS_IL2CPP_INT32 mid = lo + (hi - lo) / 2;
        if (elements[mid] < value) {
            lo = mid + 1;
        } else if (elements[mid] > value) {
            hi = mid - 1;
        } else {
            return mid;
        }
    }
    return ~lo;
}

CHAOS_IL2CPP_INT32 ChaosArrayIndexOf(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (array == 0) return -1;
    const auto* arr = get_managed_array(array);
    if (arr->elements == nullptr) return -1;
    for (CHAOS_IL2CPP_INTPTR i = 0; i < arr->length; ++i) {
        if (arr->elements[i] == value) return static_cast<CHAOS_IL2CPP_INT32>(i);
    }
    return -1;
}

CHAOS_IL2CPP_INT32 ChaosArrayLastIndexOf(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (array == 0) return -1;
    const auto* arr = get_managed_array(array);
    if (arr->elements == nullptr) return -1;
    CHAOS_IL2CPP_INT32 i = static_cast<CHAOS_IL2CPP_INT32>(arr->length) - 1;
    for (; i >= 0; --i) {
        if (arr->elements[i] == value) return i;
    }
    return -1;
}

void ChaosArraySort(CHAOS_IL2CPP_INTPTR array) noexcept
{
    if (array == 0) return;
    auto* arr = get_managed_array_mut(array);
    if (arr->elements == nullptr) return;
    CHAOS_IL2CPP_INT32 n = static_cast<CHAOS_IL2CPP_INT32>(arr->length);
    for (CHAOS_IL2CPP_INT32 i = 1; i < n; ++i) {
        CHAOS_IL2CPP_INTPTR key = arr->elements[i];
        CHAOS_IL2CPP_INT32 j = i - 1;
        while (j >= 0 && arr->elements[j] > key) {
            arr->elements[j + 1] = arr->elements[j];
            --j;
        }
        arr->elements[j + 1] = key;
    }
}

void ChaosArrayReverse(CHAOS_IL2CPP_INTPTR array) noexcept
{
    if (array == 0) return;
    auto* arr = get_managed_array_mut(array);
    if (arr->length <= 1 || arr->elements == nullptr) return;
    CHAOS_IL2CPP_INT32 i = 0;
    CHAOS_IL2CPP_INT32 j = static_cast<CHAOS_IL2CPP_INT32>(arr->length) - 1;
    while (i < j) {
        CHAOS_IL2CPP_INTPTR tmp = arr->elements[i];
        arr->elements[i] = arr->elements[j];
        arr->elements[j] = tmp;
        ++i; --j;
    }
}

CHAOS_IL2CPP_INTPTR ChaosArrayGetValue(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index) noexcept
{
    if (array == 0) return 0;
    const auto* arr = get_managed_array(array);
    CHAOS_IL2CPP_UINTPTR uindex = static_cast<CHAOS_IL2CPP_UINTPTR>(index);
    if (uindex >= static_cast<CHAOS_IL2CPP_UINTPTR>(arr->length) || arr->elements == nullptr) return 0;
    return arr->elements[uindex];
}

CHAOS_IL2CPP_INTPTR ChaosBitConverterGetBytes(CHAOS_IL2CPP_INTPTR unused, CHAOS_IL2CPP_INT32 value) noexcept
{
    (void)unused;
    using namespace chaos::il2cpp::runtime_core;
    // Allocate ManagedArrayAccessor (56 bytes) + 4-element byte buffer
    auto* storage = static_cast<CHAOS_IL2CPP_UINT8*>(GcAllocateAtomic(sizeof(ManagedArrayAccessor)));
    if (storage == nullptr) return 0;
    auto* arr = reinterpret_cast<ManagedArrayAccessor*>(storage);
    std::memset(arr, 0, sizeof(ManagedArrayAccessor));
    arr->element_type_shape = 2;  // value type
    arr->length = 4;

    // Allocate element buffer (4 bytes)
    auto* elements = static_cast<CHAOS_IL2CPP_UINT8*>(GcAllocateAtomic(4));
    if (elements == nullptr) return 0;
    elements[0] = static_cast<CHAOS_IL2CPP_UINT8>(value & 0xFF);
    elements[1] = static_cast<CHAOS_IL2CPP_UINT8>((value >> 8) & 0xFF);
    elements[2] = static_cast<CHAOS_IL2CPP_UINT8>((value >> 16) & 0xFF);
    elements[3] = static_cast<CHAOS_IL2CPP_UINT8>((value >> 24) & 0xFF);
    arr->elements = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(elements);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core