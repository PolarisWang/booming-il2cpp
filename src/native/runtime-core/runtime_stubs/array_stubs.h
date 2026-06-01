// ── Array stub declarations ────────────────────────────────────
#pragma once

#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include <chaos/compiler_hints.h>
#include <cstring>
#include "runtime_stubs/stub_common.h"
#include "gc_helpers.h"
#include "cpu_features.h"

// SSE2 is x86_64 baseline — always available, no runtime detection needed.
#if defined(__x86_64__) || defined(_M_AMD64)
#include <emmintrin.h>
#endif

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

    constexpr CHAOS_IL2CPP_SIZE kElemSize = sizeof(CHAOS_IL2CPP_INTPTR);
    auto* dst_ptr = reinterpret_cast<CHAOS_IL2CPP_UINT8*>(accessor_get_elements(dst_arr)) + di * kElemSize;
    const auto* src_ptr = reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(accessor_get_elements(src_arr)) + si * kElemSize;
    std::memmove(dst_ptr, src_ptr, c * kElemSize);
}

/// Unsafe variant for AOT codegen DirectNativeSymbol calls, where codegen
/// guarantees valid indices. No bounds checks — only null + count<=0 guard.
/// Integrates P3 small-copy inline: ≤32 bytes uses __builtin_memcpy (avoids
/// memmove function call overhead for tiny copies).
CHAOS_IL2CPP_FORCEINLINE void ChaosArrayCopy_Unsafe_Inline(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INT32 sourceIndex, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT32 destIndex, CHAOS_IL2CPP_INT32 count) noexcept
{
    if (source == 0 || dest == 0 || count <= 0) return;
    auto* src_arr = get_managed_array(source);
    auto* dst_arr = get_managed_array_mut(dest);
    auto si = static_cast<CHAOS_IL2CPP_UINTPTR>(sourceIndex);
    auto di = static_cast<CHAOS_IL2CPP_UINTPTR>(destIndex);
    auto c = static_cast<CHAOS_IL2CPP_UINTPTR>(count);

    constexpr CHAOS_IL2CPP_SIZE kElemSize = sizeof(CHAOS_IL2CPP_INTPTR);
    auto* dst_ptr = reinterpret_cast<CHAOS_IL2CPP_UINT8*>(accessor_get_elements(dst_arr)) + di * kElemSize;
    const auto* src_ptr = reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(accessor_get_elements(src_arr)) + si * kElemSize;
    CHAOS_IL2CPP_SIZE bytes = c * kElemSize;

    // P3: small-copy inline — avoid memmove function call for ≤32B (4 pointers)
    if (CHAOS_IL2CPP_LIKELY(bytes <= 32)) {
        if (bytes <= 8) {
            CHAOS_IL2CPP_INTPTR tmp;
            __builtin_memcpy(&tmp, src_ptr, bytes);
            __builtin_memcpy(dst_ptr, &tmp, bytes);
            return;
        }
        __builtin_memcpy(dst_ptr, src_ptr, bytes);
        return;
    }
    std::memmove(dst_ptr, src_ptr, bytes);
}

CHAOS_IL2CPP_FORCEINLINE void ChaosArrayCopy3_Inline(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT32 count) noexcept
{
    ChaosArrayCopy_Inline(source, 0, dest, 0, count);
}

/// Unsafe 3-param Copy: inlined directly — no delegation overhead.
CHAOS_IL2CPP_FORCEINLINE void ChaosArrayCopy3_Unsafe_Inline(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT32 count) noexcept
{
    if (source == 0 || dest == 0 || count <= 0) return;
    auto* src_arr = get_managed_array(source);
    auto* dst_arr = get_managed_array_mut(dest);

    constexpr CHAOS_IL2CPP_SIZE kElemSize = sizeof(CHAOS_IL2CPP_INTPTR);
    auto* dst_ptr = reinterpret_cast<CHAOS_IL2CPP_UINT8*>(accessor_get_elements(dst_arr));
    const auto* src_ptr = reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(accessor_get_elements(src_arr));
    CHAOS_IL2CPP_SIZE bytes = static_cast<CHAOS_IL2CPP_SIZE>(count) * kElemSize;

    if (CHAOS_IL2CPP_LIKELY(bytes <= 32)) {
        if (bytes <= 8) {
            CHAOS_IL2CPP_INTPTR tmp;
            __builtin_memcpy(&tmp, src_ptr, bytes);
            __builtin_memcpy(dst_ptr, &tmp, bytes);
            return;
        }
        __builtin_memcpy(dst_ptr, src_ptr, bytes);
        return;
    }
    std::memmove(dst_ptr, src_ptr, bytes);
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

// ── SSE2-accelerated IndexOf / LastIndexOf (P1) ─────────────
// SSE2 is x86_64 baseline. _mm_cmpeq_epi32 compares 4x int32 per cycle;
// we check paired 32-bit halves for each 64-bit element (two elements
// per 128-bit vector).
#if defined(__x86_64__) || defined(_M_AMD64)
CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INT32 IndexOf_Sse2(
    const CHAOS_IL2CPP_INTPTR* elements, CHAOS_IL2CPP_INTPTR len, CHAOS_IL2CPP_INTPTR value) noexcept
{
    // Broadcast 64-bit value as two paired 32-bit lanes
    const int lo32 = static_cast<int>(static_cast<CHAOS_IL2CPP_UINTPTR>(value));
    const int hi32 = static_cast<int>(static_cast<CHAOS_IL2CPP_UINTPTR>(value) >> 32);
    const __m128i val = _mm_setr_epi32(lo32, hi32, lo32, hi32);

    CHAOS_IL2CPP_INTPTR i = 0;
    for (; i + 1 < len; i += 2) {
        const __m128i chunk = _mm_loadu_si128(
            reinterpret_cast<const __m128i*>(elements + i));
        const __m128i cmp = _mm_cmpeq_epi32(chunk, val);
        const int mask = _mm_movemask_ps(_mm_castsi128_ps(cmp));
        // mask bits: [elem1_hi(3), elem1_lo(2), elem0_hi(1), elem0_lo(0)]
        if ((mask & 3) == 3) return static_cast<CHAOS_IL2CPP_INT32>(i);
        if ((mask & 0xC) == 0xC) return static_cast<CHAOS_IL2CPP_INT32>(i + 1);
    }
    if (i < len && elements[i] == value) return static_cast<CHAOS_IL2CPP_INT32>(i);
    return -1;
}

CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INT32 LastIndexOf_Sse2(
    const CHAOS_IL2CPP_INTPTR* elements, CHAOS_IL2CPP_INTPTR len, CHAOS_IL2CPP_INTPTR value) noexcept
{
    const int lo32 = static_cast<int>(static_cast<CHAOS_IL2CPP_UINTPTR>(value));
    const int hi32 = static_cast<int>(static_cast<CHAOS_IL2CPP_UINTPTR>(value) >> 32);
    const __m128i val = _mm_setr_epi32(lo32, hi32, lo32, hi32);

    CHAOS_IL2CPP_INTPTR i = len;
    while (i >= 2) {
        i -= 2;
        const __m128i chunk = _mm_loadu_si128(
            reinterpret_cast<const __m128i*>(elements + i));
        const __m128i cmp = _mm_cmpeq_epi32(chunk, val);
        const int mask = _mm_movemask_ps(_mm_castsi128_ps(cmp));
        // mask bits: [elem1_hi(3), elem1_lo(2), elem0_hi(1), elem0_lo(0)]
        if ((mask & 0xC) == 0xC) return static_cast<CHAOS_IL2CPP_INT32>(i + 1);
        if ((mask & 3) == 3) return static_cast<CHAOS_IL2CPP_INT32>(i);
    }
    if (i > 0 && elements[0] == value) return 0;
    return -1;
}
#endif

CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INT32 ChaosArrayIndexOf_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (array == 0) return -1;
    const auto* arr = get_managed_array(array);
    const auto* elements = accessor_get_elements(arr);
#if defined(__x86_64__) || defined(_M_AMD64)
    return IndexOf_Sse2(elements, arr->length, value);
#else
    for (CHAOS_IL2CPP_INTPTR i = 0; i < arr->length; ++i) {
        if (elements[i] == value) return static_cast<CHAOS_IL2CPP_INT32>(i);
    }
    return -1;
#endif
}

CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INT32 ChaosArrayLastIndexOf_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (array == 0) return -1;
    const auto* arr = get_managed_array(array);
    const auto* elements = accessor_get_elements(arr);
#if defined(__x86_64__) || defined(_M_AMD64)
    return LastIndexOf_Sse2(elements, arr->length, value);
#else
    CHAOS_IL2CPP_INT32 i = static_cast<CHAOS_IL2CPP_INT32>(arr->length) - 1;
    for (; i >= 0; --i) {
        if (elements[i] == value) return i;
    }
    return -1;
#endif
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

// ── AVX2-accelerated Reverse (P2) ──────────────────────────────
// Process 4 elements per iteration using 256-bit SIMD.
// Requires runtime CPUID check — AVX2 is NOT x86_64 baseline.
// GCC/Clang need __attribute__((target("avx2"))) because AVX2 code is
// conditionally dispatched at runtime; MSVC accepts AVX intrinsics
// without a global /arch:AVX2 flag.
#if defined(__x86_64__) || defined(_M_AMD64)
#include <immintrin.h>

#if defined(__GNUC__) || defined(__clang__)
#define CHAOS_IL2CPP_TARGET_AVX2 __attribute__((target("avx2")))
#else
#define CHAOS_IL2CPP_TARGET_AVX2
#endif

// Standalone AVX2 reverse — not inlined (GCC rejects inlining AVX2
// intrinsics into callers compiled without -mavx2). Called via runtime dispatch.
CHAOS_IL2CPP_TARGET_AVX2
inline void Reverse_Avx2_Dispatch(
    CHAOS_IL2CPP_INTPTR* elements, CHAOS_IL2CPP_INT32 n) noexcept
{
    CHAOS_IL2CPP_INT32 i = 0;
    CHAOS_IL2CPP_INT32 j = n - 1;
    // Process 4 elements per iteration
    while (i + 3 < j) {
        __m256i left = _mm256_loadu_si256(
            reinterpret_cast<const __m256i*>(elements + i));
        __m256i right = _mm256_loadu_si256(
            reinterpret_cast<const __m256i*>(elements + j - 3));

        const __m256i rev_idx = _mm256_setr_epi32(6, 7, 4, 5, 2, 3, 0, 1);
        __m256i left_rev = _mm256_permutevar8x32_epi32(left, rev_idx);
        __m256i right_rev = _mm256_permutevar8x32_epi32(right, rev_idx);

        _mm256_storeu_si256(reinterpret_cast<__m256i*>(elements + i), right_rev);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(elements + j - 3), left_rev);
        i += 4; j -= 4;
    }
    // Scalar tail for remaining elements (< 8 remaining)
    while (i < j) {
        CHAOS_IL2CPP_INTPTR tmp = elements[i];
        elements[i] = elements[j];
        elements[j] = tmp;
        ++i; --j;
    }
}
#undef CHAOS_IL2CPP_TARGET_AVX2
#endif

CHAOS_IL2CPP_FORCEINLINE void ChaosArrayReverse_Inline(CHAOS_IL2CPP_INTPTR array) noexcept
{
    if (array == 0) return;
    auto* arr = get_managed_array_mut(array);
    CHAOS_IL2CPP_INT32 n = static_cast<CHAOS_IL2CPP_INT32>(arr->length);
    if (n <= 1) return;
    auto* elements = accessor_get_elements(arr);

#if defined(__x86_64__) || defined(_M_AMD64)
    if (chaos::il2cpp::runtime_core::HasAvx2()) {
        Reverse_Avx2_Dispatch(elements, n);
        return;
    }
#endif
    // Scalar fallback
    CHAOS_IL2CPP_INT32 i = 0;
    CHAOS_IL2CPP_INT32 j = n - 1;
    while (i < j) {
        CHAOS_IL2CPP_INTPTR tmp = elements[i];
        elements[i] = elements[j];
        elements[j] = tmp;
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