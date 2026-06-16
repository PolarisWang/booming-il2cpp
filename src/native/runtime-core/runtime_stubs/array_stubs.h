// ── Array stub declarations ────────────────────────────────────
#pragma once

#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include <chaos/compiler_hints.h>
#include <cstring>
#include "runtime_stubs/stub_common.h"
#include "gc_helpers.h"
#include "cpu_features.h"

// MSVC uses _alloca, GCC/Clang use CHAOS_IL2CPP_ALLOCA
#if defined(_MSC_VER)
#include <malloc.h>
#define CHAOS_IL2CPP_ALLOCA(s) _alloca(s)
#else
#define CHAOS_IL2CPP_ALLOCA(s) CHAOS_IL2CPP_ALLOCA(s)
#endif

// MSVC doesn't have __builtin_memcpy — use std::memcpy instead.
#if defined(_MSC_VER)
#include <cstring>
#define __builtin_memcpy(dst, src, n) std::memcpy(dst, src, n)
#endif

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

// ── SSE2-accelerated IndexOf for byte arrays (P6) ──────────────
// Processes 16 bytes per SSE2 iteration.  Called directly from codegen
// as DirectNativeSymbol when element type is byte, avoiding the generic
// 8-byte-element path that is 8x slower per element.
#if defined(__x86_64__) || defined(_M_AMD64)
CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INT32 ChaosArrayIndexOf_Byte_Sse2(
    const CHAOS_IL2CPP_UINT8* elements, CHAOS_IL2CPP_INTPTR len,
    CHAOS_IL2CPP_UINT8 value) noexcept
{
    const __m128i val = _mm_set1_epi8(static_cast<char>(value));
    CHAOS_IL2CPP_INTPTR i = 0;
    for (; i + 15 < len; i += 16) {
        const __m128i chunk = _mm_loadu_si128(
            reinterpret_cast<const __m128i*>(elements + i));
        const __m128i cmp = _mm_cmpeq_epi8(chunk, val);
        const int mask = _mm_movemask_epi8(cmp);
        if (mask != 0) {
            // Find first set bit in 16-bit mask (faster than scalar tail for short arrays)
#if defined(_MSC_VER)
            unsigned long idx;
            _BitScanForward(&idx, static_cast<unsigned long>(mask));
            return static_cast<CHAOS_IL2CPP_INT32>(i + static_cast<CHAOS_IL2CPP_INTPTR>(idx));
#else
            return static_cast<CHAOS_IL2CPP_INT32>(i + __builtin_ctz(static_cast<unsigned>(mask)));
#endif
        }
    }
    // Scalar tail
    for (; i < len; ++i) {
        if (elements[i] == value) return static_cast<CHAOS_IL2CPP_INT32>(i);
    }
    return -1;
}

CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INT32 ChaosArrayIndexOf_Byte_Inline(
    CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_UINT8 value) noexcept
{
    if (array == 0) return -1;
    const auto* arr = get_managed_array(array);
    const auto* elements = reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(accessor_get_elements(arr));
    return ChaosArrayIndexOf_Byte_Sse2(elements, arr->length, value);
}
#endif

// Forward declarations for AVX2 copy helpers (defined later in the AVX2 block,
// but called by ChaosArrayCopy_Inline which is defined here).
#if defined(__x86_64__) || defined(_M_AMD64)
inline void Avx2BlockCopy(void* dst, const void* src, CHAOS_IL2CPP_SIZE bytes) noexcept;
inline void Avx2StreamCopy(void* dst, const void* src, CHAOS_IL2CPP_SIZE bytes) noexcept;
inline void Avx2StreamZero(void* dst, CHAOS_IL2CPP_SIZE bytes) noexcept;
#endif

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
    CHAOS_IL2CPP_SIZE bytes = c * kElemSize;
#if defined(__x86_64__) || defined(_M_AMD64)
    if (bytes > 256 && chaos::il2cpp::runtime_core::HasAvx2()) {
        Avx2StreamCopy(dst_ptr, src_ptr, bytes); return;
    }
    if (bytes > 64 && chaos::il2cpp::runtime_core::HasAvx2()) {
        Avx2BlockCopy(dst_ptr, src_ptr, bytes); return;
    }
#endif
    std::memmove(dst_ptr, src_ptr, bytes);
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
#if defined(__x86_64__) || defined(_M_AMD64)
    // P5: AVX2 streaming copy for large blocks (>256 bytes)
    if (bytes > 256 && chaos::il2cpp::runtime_core::HasAvx2()) {
        Avx2StreamCopy(dst_ptr, src_ptr, bytes);
        return;
    }
    // P5: AVX2 cached copy for medium blocks (64-256 bytes)
    if (bytes > 64 && chaos::il2cpp::runtime_core::HasAvx2()) {
        Avx2BlockCopy(dst_ptr, src_ptr, bytes);
        return;
    }
#endif
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
#if defined(__x86_64__) || defined(_M_AMD64)
    if (bytes > 256 && chaos::il2cpp::runtime_core::HasAvx2()) {
        Avx2StreamCopy(dst_ptr, src_ptr, bytes); return;
    }
    if (bytes > 64 && chaos::il2cpp::runtime_core::HasAvx2()) {
        Avx2BlockCopy(dst_ptr, src_ptr, bytes); return;
    }
#endif
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
        CHAOS_IL2CPP_INTPTR elem = elements[mid];  // single load
        // Branchless bounds update — avoids branch mispredictions on comparison result
        lo = (elem < value) ? (mid + 1) : lo;
        hi = (elem > value) ? (mid - 1) : hi;
        if (elem == value) return mid;
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
        CHAOS_IL2CPP_INTPTR elem = elements[mid];
        lo = (elem < value) ? (mid + 1) : lo;
        hi = (elem > value) ? (mid - 1) : hi;
        if (elem == value) return mid;
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

// Forward declarations for AVX2 dispatch functions (defined later in the
// AVX2 block, but called by ChaosArrayIndexOf_Inline / LastIndexOf_Inline
// which are defined before the AVX2 functions).
inline CHAOS_IL2CPP_INT32 IndexOf_Avx2_Dispatch(
    const CHAOS_IL2CPP_INTPTR* elements, CHAOS_IL2CPP_INTPTR len, CHAOS_IL2CPP_INTPTR value) noexcept;
inline CHAOS_IL2CPP_INT32 LastIndexOf_Avx2_Dispatch(
    const CHAOS_IL2CPP_INTPTR* elements, CHAOS_IL2CPP_INTPTR len, CHAOS_IL2CPP_INTPTR value) noexcept;

CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INT32 ChaosArrayIndexOf_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (array == 0) return -1;
    const auto* arr = get_managed_array(array);
    const auto* elements = accessor_get_elements(arr);
#if defined(__x86_64__) || defined(_M_AMD64)
    if (arr->length >= 4 && chaos::il2cpp::runtime_core::HasAvx2()) {
        return IndexOf_Avx2_Dispatch(elements, arr->length, value);
    }
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
    if (arr->length >= 4 && chaos::il2cpp::runtime_core::HasAvx2()) {
        return LastIndexOf_Avx2_Dispatch(elements, arr->length, value);
    }
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
    auto* e = accessor_get_elements(arr);
    CHAOS_IL2CPP_INT32 n = static_cast<CHAOS_IL2CPP_INT32>(arr->length);
    if (n < 2) return;

    // Introsort: quicksort falling back to heapsort for pathological >2*log2(n) depth.
    int max_depth = 0;
    { int t = n; while (t >>= 1) max_depth++; max_depth <<= 1; }
    // Manually-inlined insertion sort threshold
    constexpr CHAOS_IL2CPP_INT32 kInsertSortThreshold = 16;

    // Iterative quicksort using explicit stack
    struct Range { CHAOS_IL2CPP_INT32 l, h; };
    auto* stack = static_cast<Range*>(CHAOS_IL2CPP_ALLOCA(static_cast<CHAOS_IL2CPP_SIZE>(n) * sizeof(Range)));
    int sp = 0; stack[sp++] = {0, n - 1};

    while (sp > 0) {
        Range r = stack[--sp];
        CHAOS_IL2CPP_INT32 l = r.l, h = r.h;
        while (l < h) {
            if (h - l <= kInsertSortThreshold) {
                for (CHAOS_IL2CPP_INT32 i = l + 1; i <= h; ++i) {
                    CHAOS_IL2CPP_INTPTR key = e[i];
                    CHAOS_IL2CPP_INT32 j = i - 1;
                    while (j >= l && e[j] > key) { e[j + 1] = e[j]; j--; }
                    e[j + 1] = key;
                }
                break;
            }
            if (max_depth-- <= 0) {
                // Heapsort fallback for pathologically deep recursion
                CHAOS_IL2CPP_INT32 nn = h - l + 1;
                auto* base = e + l;
                for (CHAOS_IL2CPP_INT32 i = nn / 2 - 1; i >= 0; i--) {
                    CHAOS_IL2CPP_INT32 p = i;
                    while (true) {
                        CHAOS_IL2CPP_INT32 c = 2 * p + 1;
                        if (c >= nn) break;
                        if (c + 1 < nn && base[c + 1] > base[c]) c++;
                        if (base[p] >= base[c]) break;
                        CHAOS_IL2CPP_INTPTR t = base[p]; base[p] = base[c]; base[c] = t;
                        p = c;
                    }
                }
                for (CHAOS_IL2CPP_INT32 i = nn - 1; i > 0; i--) {
                    CHAOS_IL2CPP_INTPTR t = base[0]; base[0] = base[i]; base[i] = t;
                    CHAOS_IL2CPP_INT32 p = 0, sz = i;
                    while (true) {
                        CHAOS_IL2CPP_INT32 c = 2 * p + 1;
                        if (c >= sz) break;
                        if (c + 1 < sz && base[c + 1] > base[c]) c++;
                        if (base[p] >= base[c]) break;
                        t = base[p]; base[p] = base[c]; base[c] = t;
                        p = c;
                    }
                }
                break;
            }
            // Median-of-3 pivot
            CHAOS_IL2CPP_INT32 m = l + (h - l) / 2;
            if (e[l] > e[m]) { CHAOS_IL2CPP_INTPTR t = e[l]; e[l] = e[m]; e[m] = t; }
            if (e[l] > e[h]) { CHAOS_IL2CPP_INTPTR t = e[l]; e[l] = e[h]; e[h] = t; }
            if (e[m] > e[h]) { CHAOS_IL2CPP_INTPTR t = e[m]; e[m] = e[h]; e[h] = t; }
            CHAOS_IL2CPP_INTPTR pivot = e[m];
            CHAOS_IL2CPP_INT32 i = l - 1, j = h + 1;
            while (true) {
                do { i++; } while (e[i] < pivot);
                do { j--; } while (e[j] > pivot);
                if (i >= j) break;
                CHAOS_IL2CPP_INTPTR t = e[i]; e[i] = e[j]; e[j] = t;
            }
            // Recurse into smaller partition, iterate on larger (stack depth = O(log n))
            if (j - l < h - j) {
                if (j < h) stack[sp++] = {j + 1, h};
                h = j;
            } else {
                if (l < j) stack[sp++] = {l, j};
                l = j + 1;
            }
        }
    }
}

/// Sort using a managed IComparer (function pointer).
using CompareFn = CHAOS_IL2CPP_INT32 (*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR) noexcept;

CHAOS_IL2CPP_FORCEINLINE void ChaosArraySortWithComparer_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR comparer) noexcept
{
    if (array == 0 || comparer == 0) return;
    auto* arr = get_managed_array_mut(array);
    auto* e = accessor_get_elements(arr);
    CHAOS_IL2CPP_INT32 n = static_cast<CHAOS_IL2CPP_INT32>(arr->length);
    if (n < 2) return;
    auto cmp = reinterpret_cast<CompareFn>(comparer);

    int max_depth = 0;
    { int t = n; while (t >>= 1) max_depth++; max_depth <<= 1; }
    constexpr CHAOS_IL2CPP_INT32 kInsertSortThreshold = 16;
    struct Range { CHAOS_IL2CPP_INT32 l, h; };
    auto* stack = static_cast<Range*>(CHAOS_IL2CPP_ALLOCA(static_cast<CHAOS_IL2CPP_SIZE>(n) * sizeof(Range)));
    int sp = 0; stack[sp++] = {0, n - 1};

    while (sp > 0) {
        Range r = stack[--sp];
        CHAOS_IL2CPP_INT32 l = r.l, h = r.h;
        while (l < h) {
            if (h - l <= kInsertSortThreshold) {
                for (CHAOS_IL2CPP_INT32 i = l + 1; i <= h; ++i) {
                    CHAOS_IL2CPP_INTPTR key = e[i];
                    CHAOS_IL2CPP_INT32 j = i - 1;
                    while (j >= l && cmp(e[j], key) > 0) { e[j + 1] = e[j]; j--; }
                    e[j + 1] = key;
                }
                break;
            }
            if (max_depth-- <= 0) {
                CHAOS_IL2CPP_INT32 nn = h - l + 1;
                auto* base = e + l;
                for (CHAOS_IL2CPP_INT32 i = nn / 2 - 1; i >= 0; i--) {
                    CHAOS_IL2CPP_INT32 p = i;
                    while (true) {
                        CHAOS_IL2CPP_INT32 c = 2 * p + 1;
                        if (c >= nn) break;
                        if (c + 1 < nn && cmp(base[c + 1], base[c]) > 0) c++;
                        if (cmp(base[p], base[c]) >= 0) break;
                        CHAOS_IL2CPP_INTPTR t = base[p]; base[p] = base[c]; base[c] = t;
                        p = c;
                    }
                }
                for (CHAOS_IL2CPP_INT32 i = nn - 1; i > 0; i--) {
                    CHAOS_IL2CPP_INTPTR t = base[0]; base[0] = base[i]; base[i] = t;
                    CHAOS_IL2CPP_INT32 p = 0, sz = i;
                    while (true) {
                        CHAOS_IL2CPP_INT32 c = 2 * p + 1;
                        if (c >= sz) break;
                        if (c + 1 < sz && cmp(base[c + 1], base[c]) > 0) c++;
                        if (cmp(base[p], base[c]) >= 0) break;
                        t = base[p]; base[p] = base[c]; base[c] = t;
                        p = c;
                    }
                }
                break;
            }
            CHAOS_IL2CPP_INT32 m = l + (h - l) / 2;
            if (cmp(e[l], e[m]) > 0) { CHAOS_IL2CPP_INTPTR t = e[l]; e[l] = e[m]; e[m] = t; }
            if (cmp(e[l], e[h]) > 0) { CHAOS_IL2CPP_INTPTR t = e[l]; e[l] = e[h]; e[h] = t; }
            if (cmp(e[m], e[h]) > 0) { CHAOS_IL2CPP_INTPTR t = e[m]; e[m] = e[h]; e[h] = t; }
            CHAOS_IL2CPP_INTPTR pivot = e[m];
            CHAOS_IL2CPP_INT32 i = l - 1, j = h + 1;
            while (true) {
                do { i++; } while (cmp(e[i], pivot) < 0);
                do { j--; } while (cmp(e[j], pivot) > 0);
                if (i >= j) break;
                CHAOS_IL2CPP_INTPTR t = e[i]; e[i] = e[j]; e[j] = t;
            }
            if (j - l < h - j) {
                if (j < h) stack[sp++] = {j + 1, h};
                h = j;
            } else {
                if (l < j) stack[sp++] = {l, j};
                l = j + 1;
            }
        }
    }
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

// ── AVX2-accelerated IndexOf / LastIndexOf (P4+) ────────────
// 256-bit vectors process 4 × 64-bit pointers per iteration
// (2x throughput vs SSE2's 2 per iteration). Runtime-dispatched
// via HasAvx2() — GCC/Clang need target("avx2") attribute.
CHAOS_IL2CPP_TARGET_AVX2
inline CHAOS_IL2CPP_INT32 IndexOf_Avx2_Dispatch(
    const CHAOS_IL2CPP_INTPTR* elements, CHAOS_IL2CPP_INTPTR len, CHAOS_IL2CPP_INTPTR value) noexcept
{
    const __m256i val = _mm256_set1_epi64x(static_cast<long long>(value));
    CHAOS_IL2CPP_INTPTR i = 0;
    for (; i + 3 < len; i += 4) {
        const __m256i chunk = _mm256_loadu_si256(
            reinterpret_cast<const __m256i*>(elements + i));
        const __m256i cmp = _mm256_cmpeq_epi64(chunk, val);
        const int mask = _mm256_movemask_pd(_mm256_castsi256_pd(cmp));
        if (mask & 0x1) return static_cast<CHAOS_IL2CPP_INT32>(i);
        if (mask & 0x2) return static_cast<CHAOS_IL2CPP_INT32>(i + 1);
        if (mask & 0x4) return static_cast<CHAOS_IL2CPP_INT32>(i + 2);
        if (mask & 0x8) return static_cast<CHAOS_IL2CPP_INT32>(i + 3);
    }
    for (; i < len; ++i) {
        if (elements[i] == value) return static_cast<CHAOS_IL2CPP_INT32>(i);
    }
    return -1;
}

CHAOS_IL2CPP_TARGET_AVX2
inline CHAOS_IL2CPP_INT32 LastIndexOf_Avx2_Dispatch(
    const CHAOS_IL2CPP_INTPTR* elements, CHAOS_IL2CPP_INTPTR len, CHAOS_IL2CPP_INTPTR value) noexcept
{
    const __m256i val = _mm256_set1_epi64x(static_cast<long long>(value));
    CHAOS_IL2CPP_INTPTR i = len;
    while (i >= 4) {
        i -= 4;
        const __m256i chunk = _mm256_loadu_si256(
            reinterpret_cast<const __m256i*>(elements + i));
        const __m256i cmp = _mm256_cmpeq_epi64(chunk, val);
        const int mask = _mm256_movemask_pd(_mm256_castsi256_pd(cmp));
        if (mask & 0x8) return static_cast<CHAOS_IL2CPP_INT32>(i + 3);
        if (mask & 0x4) return static_cast<CHAOS_IL2CPP_INT32>(i + 2);
        if (mask & 0x2) return static_cast<CHAOS_IL2CPP_INT32>(i + 1);
        if (mask & 0x1) return static_cast<CHAOS_IL2CPP_INT32>(i);
    }
    for (; i > 0; --i) {
        if (elements[i - 1] == value) return static_cast<CHAOS_IL2CPP_INT32>(i - 1);
    }
    return -1;
}

// ── AVX2-accelerated Copy (P5) ───────────────────────────────
// Streaming copy for large memory blocks (>256 bytes). Uses non-temporal
// stores to avoid cache pollution when copying large arrays that won't
// be read again soon.  64-256 byte blocks use cached vector stores.
CHAOS_IL2CPP_TARGET_AVX2
inline void Avx2BlockCopy(void* dst, const void* src, CHAOS_IL2CPP_SIZE bytes) noexcept
{
    CHAOS_IL2CPP_SIZE i = 0;
    // 256-bit (32 byte) loop
    for (; i + 32 <= bytes; i += 32) {
        __m256i chunk = _mm256_loadu_si256(
            reinterpret_cast<const __m256i*>(static_cast<const CHAOS_IL2CPP_UINT8*>(src) + i));
        _mm256_storeu_si256(
            reinterpret_cast<__m256i*>(static_cast<CHAOS_IL2CPP_UINT8*>(dst) + i), chunk);
    }
    // 128-bit tail
    for (; i + 16 <= bytes; i += 16) {
        __m128i chunk = _mm_loadu_si128(
            reinterpret_cast<const __m128i*>(static_cast<const CHAOS_IL2CPP_UINT8*>(src) + i));
        _mm_storeu_si128(
            reinterpret_cast<__m128i*>(static_cast<CHAOS_IL2CPP_UINT8*>(dst) + i), chunk);
    }
    // Scalar tail
    if (i < bytes) {
        CHAOS_IL2CPP_UINT64 tmp;
        CHAOS_IL2CPP_SIZE tail = bytes - i;
        __builtin_memcpy(&tmp, static_cast<const CHAOS_IL2CPP_UINT8*>(src) + i, tail);
        __builtin_memcpy(static_cast<CHAOS_IL2CPP_UINT8*>(dst) + i, &tmp, tail);
    }
}

CHAOS_IL2CPP_TARGET_AVX2
inline void Avx2StreamCopy(void* dst, const void* src, CHAOS_IL2CPP_SIZE bytes) noexcept
{
    CHAOS_IL2CPP_SIZE i = 0;
    // Non-temporal streaming stores — avoid cache pollution
    for (; i + 32 <= bytes; i += 32) {
        __m256i chunk = _mm256_loadu_si256(
            reinterpret_cast<const __m256i*>(static_cast<const CHAOS_IL2CPP_UINT8*>(src) + i));
        _mm256_stream_si256(
            reinterpret_cast<__m256i*>(static_cast<CHAOS_IL2CPP_UINT8*>(dst) + i), chunk);
    }
    // 128-bit tail (use cached stores for last bytes)
    for (; i + 16 <= bytes; i += 16) {
        __m128i chunk = _mm_loadu_si128(
            reinterpret_cast<const __m128i*>(static_cast<const CHAOS_IL2CPP_UINT8*>(src) + i));
        _mm_storeu_si128(
            reinterpret_cast<__m128i*>(static_cast<CHAOS_IL2CPP_UINT8*>(dst) + i), chunk);
    }
    if (i < bytes) {
        CHAOS_IL2CPP_UINT64 tmp;
        CHAOS_IL2CPP_SIZE tail = bytes - i;
        __builtin_memcpy(&tmp, static_cast<const CHAOS_IL2CPP_UINT8*>(src) + i, tail);
        __builtin_memcpy(static_cast<CHAOS_IL2CPP_UINT8*>(dst) + i, &tmp, tail);
    }
    _mm_sfence();  // Ensure streaming stores are visible
}

// AVX2 streaming zero — non-temporal stores for large clears (>256 bytes).
// Uses _mm256_setzero_si256 to avoid cache pollution when clearing large arrays.
CHAOS_IL2CPP_TARGET_AVX2
inline void Avx2StreamZero(void* dst, CHAOS_IL2CPP_SIZE bytes) noexcept
{
    const __m256i zero = _mm256_setzero_si256();
    CHAOS_IL2CPP_SIZE i = 0;
    for (; i + 32 <= bytes; i += 32) {
        _mm256_stream_si256(
            reinterpret_cast<__m256i*>(static_cast<CHAOS_IL2CPP_UINT8*>(dst) + i), zero);
    }
    for (; i + 16 <= bytes; i += 16) {
        _mm_storeu_si128(
            reinterpret_cast<__m128i*>(static_cast<CHAOS_IL2CPP_UINT8*>(dst) + i), _mm_setzero_si128());
    }
    if (i < bytes) {
        CHAOS_IL2CPP_UINT64 tmp = 0;
        CHAOS_IL2CPP_SIZE tail = bytes - i;
        __builtin_memcpy(static_cast<CHAOS_IL2CPP_UINT8*>(dst) + i, &tmp, tail);
    }
    _mm_sfence();
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

// ── P4: Clear with small-clear fast path ─────────────────────────
// For small arrays (≤64 bytes, 8 pointers), use inline stores instead
// of memset to avoid function call overhead. Benchmarks use ≤4 elements
// (32 bytes), where memset call overhead dominates (Clear: 194.1ns vs
// .NET 8 123.7ns = 0.64x before optimization).
CHAOS_IL2CPP_FORCEINLINE void ChaosArrayClear_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 count) noexcept
{
    if (array == 0 || count <= 0) return;
    auto* arr = get_managed_array_mut(array);
    if (index < 0) return;
    auto uindex = static_cast<CHAOS_IL2CPP_UINTPTR>(index);
    auto ucount = static_cast<CHAOS_IL2CPP_UINTPTR>(count);
    if (uindex > static_cast<CHAOS_IL2CPP_UINTPTR>(arr->length) || ucount > (static_cast<CHAOS_IL2CPP_UINTPTR>(arr->length) - uindex)) return;

    constexpr CHAOS_IL2CPP_SIZE kElemSize = sizeof(CHAOS_IL2CPP_INTPTR);
    auto* dst = reinterpret_cast<CHAOS_IL2CPP_UINT8*>(accessor_get_elements(arr)) + uindex * kElemSize;
    CHAOS_IL2CPP_SIZE bytes = ucount * kElemSize;

    // Small-clear: inline stores avoid memset call overhead
    if (CHAOS_IL2CPP_LIKELY(bytes <= 64)) {
        // For very small clears (≤8 bytes), use a single store
        if (bytes <= 8) {
            CHAOS_IL2CPP_INTPTR zero = 0;
            memcpy(dst, &zero, bytes);
            return;
        }
        // For medium clears (9-64 bytes), use memset which compilers
        // expand to rep stosq or aligned vector stores when inlined.
        memset(dst, 0, bytes);
        return;
    }
#if defined(__x86_64__) || defined(_M_AMD64)
    // P6: AVX2 streaming zero for large clears (>256 bytes)
    if (bytes > 256 && chaos::il2cpp::runtime_core::HasAvx2()) {
        Avx2StreamZero(dst, bytes);
        return;
    }
#endif
    std::memset(dst, 0, bytes);
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