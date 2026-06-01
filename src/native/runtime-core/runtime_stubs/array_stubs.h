// ── Array stub declarations ────────────────────────────────────
#pragma once

#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include <chaos/compiler_hints.h>
#include <cstring>
#include "runtime_stubs/stub_common.h"
#include "gc_helpers.h"

extern "C" {

CHAOS_IL2CPP_INTPTR ChaosArrayEmpty(void) noexcept;
void ChaosArrayCopy(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INT32 sourceIndex, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT32 destIndex, CHAOS_IL2CPP_INT32 count) noexcept;
void ChaosArrayCopy3(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT32 count) noexcept;
CHAOS_IL2CPP_INTPTR ChaosArrayCreateInstance(CHAOS_IL2CPP_INTPTR elementType, CHAOS_IL2CPP_INT32 length) noexcept;
CHAOS_IL2CPP_INTPTR ChaosArrayCreateInstance2D(CHAOS_IL2CPP_INTPTR elementType, CHAOS_IL2CPP_INT32 length1, CHAOS_IL2CPP_INT32 length2) noexcept;
CHAOS_IL2CPP_INT32 ChaosArrayBinarySearch(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32 ChaosArrayBinarySearchRange(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 length, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32 ChaosArrayIndexOf(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32 ChaosArrayLastIndexOf(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept;
void ChaosArraySort(CHAOS_IL2CPP_INTPTR array) noexcept;
void ChaosArraySortWithComparer(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR comparer) noexcept;
void ChaosArrayReverse(CHAOS_IL2CPP_INTPTR array) noexcept;
CHAOS_IL2CPP_INTPTR ChaosArrayGetValue(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index) noexcept;
CHAOS_IL2CPP_INTPTR ChaosArrayNew1D(const TypeInfo* array_type_info, const TypeInfo* element_type_info, CHAOS_IL2CPP_UINT8 element_type_shape, CHAOS_IL2CPP_INTPTR length) noexcept;
CHAOS_IL2CPP_INTPTR ChaosBitConverterGetBytes(CHAOS_IL2CPP_INTPTR unused, CHAOS_IL2CPP_INT32 value) noexcept;
CHAOS_IL2CPP_INT32 ChaosBitConverterToInt32(CHAOS_IL2CPP_INTPTR byteArray, CHAOS_IL2CPP_INT32 startIndex) noexcept;
double ChaosBitConverterToDouble(CHAOS_IL2CPP_INTPTR byteArray, CHAOS_IL2CPP_INT32 startIndex) noexcept;

}  // extern "C"

// ── Inline implementation wrappers (C++ linkage, forceinline) ──
// These are used by the AOT codegen when DirectNativeSymbol resolves to
// ChaosArrayXxx_Inline, enabling the compiler to inline the function body
// at each call site. The extern "C" ABI exports in array_stubs.cpp delegate
// to these so both paths share a single implementation.
using chaos::il2cpp::runtime_core::GcAllocateAtomic;

CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INTPTR ChaosArrayEmpty_Inline(void) noexcept
{
    static ManagedArrayAccessor* s_empty = nullptr;
    if (s_empty != nullptr) [[likely]]
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_empty);

    auto* storage = static_cast<CHAOS_IL2CPP_UINT8*>(
        GcAllocateAtomic(sizeof(ManagedArrayAccessor)));
    if (storage == nullptr) return 0;
    auto* arr = reinterpret_cast<ManagedArrayAccessor*>(storage);
    std::memset(arr, 0, sizeof(ManagedArrayAccessor));
    arr->element_type_shape = 0;
    arr->length = 0;
    s_empty = arr;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}

CHAOS_IL2CPP_FORCEINLINE void ChaosArrayCopy_Inline(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INT32 sourceIndex, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT32 destIndex, CHAOS_IL2CPP_INT32 count) noexcept
{
    if (source == 0 || dest == 0 || count <= 0) return;
    auto* src_arr = get_managed_array(source);
    auto* dst_arr = get_managed_array_mut(dest);
    auto si = static_cast<CHAOS_IL2CPP_UINTPTR>(sourceIndex);
    auto di = static_cast<CHAOS_IL2CPP_UINTPTR>(destIndex);
    auto c = static_cast<CHAOS_IL2CPP_UINTPTR>(count);
    if (si > static_cast<CHAOS_IL2CPP_UINTPTR>(src_arr->length) || c > static_cast<CHAOS_IL2CPP_UINTPTR>(src_arr->length) - si) return;
    if (di > static_cast<CHAOS_IL2CPP_UINTPTR>(dst_arr->length) || c > static_cast<CHAOS_IL2CPP_UINTPTR>(dst_arr->length) - di) return;

    const CHAOS_IL2CPP_SIZE elem_size = sizeof(void*);
    std::memmove(
        reinterpret_cast<CHAOS_IL2CPP_UINT8*>(accessor_get_elements(dst_arr)) + di * elem_size,
        reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(accessor_get_elements(src_arr)) + si * elem_size,
        c * elem_size);
}

CHAOS_IL2CPP_FORCEINLINE void ChaosArrayCopy3_Inline(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT32 count) noexcept
{
    ChaosArrayCopy_Inline(source, 0, dest, 0, count);
}

CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INT32 ChaosArrayBinarySearch_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (array == 0) return -1;
    const auto* arr = get_managed_array(array);
    const auto* elements = accessor_get_elements(arr);
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

CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INT32 ChaosArrayBinarySearchRange_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 length, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (array == 0 || index < 0 || length < 0) return -1;
    const auto* arr = get_managed_array(array);
    auto len = static_cast<CHAOS_IL2CPP_INT32>(arr->length);
    if (index >= len || length > len - index) return -1;
    const auto* elements = accessor_get_elements(arr);
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

CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INT32 ChaosArrayIndexOf_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (array == 0) return -1;
    const auto* arr = get_managed_array(array);
    for (CHAOS_IL2CPP_INTPTR i = 0; i < arr->length; ++i) {
        if (accessor_get_elements(arr)[i] == value) return static_cast<CHAOS_IL2CPP_INT32>(i);
    }
    return -1;
}

CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INT32 ChaosArrayLastIndexOf_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (array == 0) return -1;
    const auto* arr = get_managed_array(array);
    CHAOS_IL2CPP_INT32 i = static_cast<CHAOS_IL2CPP_INT32>(arr->length) - 1;
    for (; i >= 0; --i) {
        if (accessor_get_elements(arr)[i] == value) return i;
    }
    return -1;
}

CHAOS_IL2CPP_FORCEINLINE void ChaosArraySort_Inline(CHAOS_IL2CPP_INTPTR array) noexcept
{
    if (array == 0) return;
    auto* arr = get_managed_array_mut(array);
    CHAOS_IL2CPP_INT32 n = static_cast<CHAOS_IL2CPP_INT32>(arr->length);
    for (CHAOS_IL2CPP_INT32 i = 1; i < n; ++i) {
        CHAOS_IL2CPP_INTPTR key = accessor_get_elements(arr)[i];
        CHAOS_IL2CPP_INT32 j = i - 1;
        while (j >= 0 && accessor_get_elements(arr)[j] > key) {
            accessor_get_elements(arr)[j + 1] = accessor_get_elements(arr)[j];
            --j;
        }
        accessor_get_elements(arr)[j + 1] = key;
    }
}

CHAOS_IL2CPP_FORCEINLINE void ChaosArraySortWithComparer_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR comparer) noexcept
{
    (void)comparer;
    ChaosArraySort_Inline(array);
}

CHAOS_IL2CPP_FORCEINLINE void ChaosArrayReverse_Inline(CHAOS_IL2CPP_INTPTR array) noexcept
{
    if (array == 0) return;
    auto* arr = get_managed_array_mut(array);
    if (arr->length <= 1) return;
    CHAOS_IL2CPP_INT32 i = 0;
    CHAOS_IL2CPP_INT32 j = static_cast<CHAOS_IL2CPP_INT32>(arr->length) - 1;
    while (i < j) {
        CHAOS_IL2CPP_INTPTR tmp = accessor_get_elements(arr)[i];
        accessor_get_elements(arr)[i] = accessor_get_elements(arr)[j];
        accessor_get_elements(arr)[j] = tmp;
        ++i; --j;
    }
}

CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INTPTR ChaosArrayGetValue_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index) noexcept
{
    if (array == 0) return 0;
    const auto* arr = get_managed_array(array);
    CHAOS_IL2CPP_UINTPTR uindex = static_cast<CHAOS_IL2CPP_UINTPTR>(index);
    if (uindex >= static_cast<CHAOS_IL2CPP_UINTPTR>(arr->length)) return 0;
    return accessor_get_elements(arr)[uindex];
}

// ── Array allocation helper (forceinline) ──────────────────────────
// Centralizes the newarr allocation pattern emitted by codegen for EVERY
// array creation site. Replaces ~10 lines of inline code with a single
// function call, reducing generated code bloat and icache pressure.
// The caller provides array_type_info (&chaos_type_info_managed_array.hot),
// element_type_info (the element MethodTable*), element_type_shape, and length.
// This function handles bounds check, size computation, allocation, and
// header initialization in one place.
CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INTPTR ChaosArrayNew1D_Inline(
    const TypeInfo* array_type_info,
    const TypeInfo* element_type_info,
    CHAOS_IL2CPP_UINT8 element_type_shape,
    CHAOS_IL2CPP_INTPTR length) noexcept
{
    const auto len32 = static_cast<CHAOS_IL2CPP_INT32>(length);
    if (len32 < 0)
    {
        CHAOS_IL2CPP_FAIL_FAST();
    }
    const auto total_size = sizeof(ManagedArrayAccessor) + static_cast<CHAOS_IL2CPP_SIZE>(len32) * sizeof(CHAOS_IL2CPP_INTPTR);
    auto* arr = static_cast<ManagedArrayAccessor*>(GcAllocateAtomic(total_size));
    if (arr == nullptr) return 0;
    // Initialize header: first 8 bytes = type_info pointer (matches ThinLockableHeader layout)
    *reinterpret_cast<const TypeInfo**>(arr->header_data) = array_type_info;
    arr->element_type_shape = element_type_shape;
    arr->element_type_info = element_type_info;
    arr->length = static_cast<CHAOS_IL2CPP_INTPTR>(len32);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}