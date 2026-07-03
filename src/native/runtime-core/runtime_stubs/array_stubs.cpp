// ABI exports: extern "C" linkage for managed/NativeAOT callability.
// Inline implementation bodies moved here from array_stubs.h to avoid
// C2084 "function already has a body" when array_stubs.h is included
// from multiple translation units (via chaos_pch.h -> chaos_runtime_host.h).

// ── Array stub implementations (real) ───────────────
#include "array_stubs.h"

#include <cstring>
#include "runtime_stubs/stub_common.h"
#include "gc_helpers.h"

// HasAvx2() implementation (declared in cpu_features.h, included via array_stubs.h).
// Defined here rather than in cpu_features.cpp to keep it in the same translation
// unit as the AVX2-calling code (avoids needing a separate compilation unit).
#if defined(__x86_64__) || defined(_M_AMD64)

#include <cstdint>

#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
#include <cpuid.h>
#endif

namespace chaos { namespace il2cpp { namespace runtime_core {

bool HasAvx2() noexcept
{
    static bool s_checked = false;
    static bool s_has_avx2 = false;
    if (s_checked) [[likely]]
        return s_has_avx2;

#if defined(_MSC_VER)
    int cpuinfo[4] = {};
    __cpuidex(cpuinfo, 1, 0);
    const bool has_osxsave = (cpuinfo[2] & (1 << 27)) != 0;
    if (!has_osxsave) { s_checked = true; s_has_avx2 = false; return false; }

    const uint64_t xcr0 = _xgetbv(0);
    if ((xcr0 & 6) != 6) { s_checked = true; s_has_avx2 = false; return false; }

    __cpuidex(cpuinfo, 7, 0);
    s_has_avx2 = (cpuinfo[1] & (1 << 5)) != 0;
#elif defined(__GNUC__) || defined(__clang__)
    unsigned int eax, ebx, ecx, edx;
    __get_cpuid(1, &eax, &ebx, &ecx, &edx);
    const bool has_osxsave = (ecx & (1 << 27)) != 0;
    if (!has_osxsave) { s_checked = true; s_has_avx2 = false; return false; }

    uint32_t xcr0_low = 0, xcr0_high = 0;
    __asm__ volatile("xgetbv" : "=a"(xcr0_low), "=d"(xcr0_high) : "c"(0));
    const uint64_t xcr0 = (static_cast<uint64_t>(xcr0_high) << 32) | xcr0_low;
    if ((xcr0 & 6) != 6) { s_checked = true; s_has_avx2 = false; return false; }

    __get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx);
    s_has_avx2 = (ebx & (1 << 5)) != 0;
#else
    s_checked = true;
    s_has_avx2 = false;
#endif
    s_checked = true;
    return s_has_avx2;
}

}}} // namespace chaos::il2cpp::runtime_core

#endif // __x86_64__ || _M_AMD64

namespace chaos::il2cpp::runtime_core {
extern "C" {

CHAOS_IL2CPP_INTPTR ChaosArrayEmpty(void) noexcept
{
    return ChaosArrayEmpty_Inline();
}

void ChaosArrayCopy(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INT32 sourceIndex, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT32 destIndex, CHAOS_IL2CPP_INT32 count) noexcept
{
    ChaosArrayCopy_Inline(source, sourceIndex, dest, destIndex, count);
}

void ChaosArrayCopy3(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT32 count) noexcept
{
    ChaosArrayCopy3_Inline(source, dest, count);
}

CHAOS_IL2CPP_INTPTR ChaosArrayCreateInstance(CHAOS_IL2CPP_INTPTR elementType, CHAOS_IL2CPP_INT32 length) noexcept
{
    auto type_info = reinterpret_cast<const TypeInfo*>(elementType);
    // AutoTestGenerator creates value type array test subjects via
    // Array.CreateInstance(Type, int) — e.g. System.Int32[].
    const auto elemTypeShape = static_cast<CHAOS_IL2CPP_UINT8>(type_info->type_shape);
    return ChaosArrayNew1D_Inline(type_info, type_info, elemTypeShape, length);
}

CHAOS_IL2CPP_INTPTR ChaosArrayCreateInstance2D(CHAOS_IL2CPP_INTPTR elementType, CHAOS_IL2CPP_INT32 length1, CHAOS_IL2CPP_INT32 length2) noexcept
{
    auto type_info = reinterpret_cast<const TypeInfo*>(elementType);
    auto elem_type_shape = static_cast<CHAOS_IL2CPP_UINT8>(type_info->type_shape);
    auto length = static_cast<CHAOS_IL2CPP_INTPTR>(length1) * static_cast<CHAOS_IL2CPP_INTPTR>(length2);
    // 2D arrays are stored as flat 1D arrays
    return ChaosArrayNew1D_Inline(type_info, type_info, elem_type_shape, length);
}

CHAOS_IL2CPP_INT32 ChaosArrayBinarySearch(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept
{
    return ChaosArrayBinarySearch_Inline(array, value);
}

CHAOS_IL2CPP_INT32 ChaosArrayBinarySearchRange(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 length, CHAOS_IL2CPP_INTPTR value) noexcept
{
    return ChaosArrayBinarySearchRange_Inline(array, index, length, value);
}

CHAOS_IL2CPP_INT32 ChaosArrayIndexOf(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept
{
    return ChaosArrayIndexOf_Inline(array, value);
}

CHAOS_IL2CPP_INT32 ChaosArrayLastIndexOf(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept
{
    return ChaosArrayLastIndexOf_Inline(array, value);
}

void ChaosArraySort(CHAOS_IL2CPP_INTPTR array) noexcept
{
    ChaosArraySort_Inline(array);
}

void ChaosArraySortWithComparer(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR comparer) noexcept
{
    ChaosArraySortWithComparer_Inline(array, comparer);
}

void ChaosArrayReverse(CHAOS_IL2CPP_INTPTR array) noexcept
{
    ChaosArrayReverse_Inline(array);
}

CHAOS_IL2CPP_INTPTR ChaosArrayGetValue(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index) noexcept
{
    return ChaosArrayGetValue_Inline(array, index);
}

CHAOS_IL2CPP_INTPTR ChaosArrayNew1D(const TypeInfo* array_type_info, const TypeInfo* element_type_info, CHAOS_IL2CPP_UINT8 element_type_shape, CHAOS_IL2CPP_INTPTR length) noexcept
{
    return ChaosArrayNew1D_Inline(array_type_info, element_type_info, element_type_shape, length);
}

CHAOS_IL2CPP_INTPTR ChaosBitConverterGetBytes(CHAOS_IL2CPP_INTPTR unused, CHAOS_IL2CPP_INT32 value) noexcept
{
    // Allocate full ManagedArrayAccessor header + 4 bytes of element data.
    // BitConverter.GetBytes returns a byte[] that later callers (ToInt32, ToDouble
    // etc.) access via get_managed_array/accessor_get_elements, which expect a
    // complete ManagedArrayAccessor structure (32 bytes header + elements).
    // GcAllocateAtomic(4) alone would cause out-of-bounds reads in callers.
    auto* ptr = static_cast<CHAOS_IL2CPP_UINT8*>(GcAllocateAtomic(sizeof(ManagedArrayAccessor) + sizeof(int32_t)));
    if (ptr == nullptr) return 0;
    auto* arr = reinterpret_cast<ManagedArrayAccessor*>(ptr);
    arr->header_data[0] = 0;
    arr->element_type_shape = 0;
    arr->element_type_info = nullptr;
    arr->length = 1;
    auto* elements = accessor_get_elements(arr);
    std::memcpy(elements, &value, sizeof(value));
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}

CHAOS_IL2CPP_INT32 ChaosBitConverterToInt32(CHAOS_IL2CPP_INTPTR byteArray, CHAOS_IL2CPP_INT32 startIndex) noexcept
{
    if (byteArray == 0) return 0;
    const auto* arr = get_managed_array(byteArray);
    const auto* elements = reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(accessor_get_elements(arr));
    CHAOS_IL2CPP_INT32 result = 0;
    std::memcpy(&result, elements + startIndex, sizeof(result));
    return result;
}

double ChaosBitConverterToDouble(CHAOS_IL2CPP_INTPTR byteArray, CHAOS_IL2CPP_INT32 startIndex) noexcept
{
    if (byteArray == 0) return 0.0;
    const auto* arr = get_managed_array(byteArray);
    const auto* elements = reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(accessor_get_elements(arr));
    double result = 0.0;
    std::memcpy(&result, elements + startIndex, sizeof(result));
    return result;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core

// ── Inline implementation bodies (moved from array_stubs.h) ──
// These are defined here rather than in the header to prevent C2084
// when the header is included from multiple translation units.
// The linker resolves all references to the single definition below.
using chaos::il2cpp::runtime_core::GcAllocateAtomic;

// ── SSE2-accelerated IndexOf for byte arrays (P6) ──────────────
#if defined(__x86_64__) || defined(_M_AMD64)
CHAOS_IL2CPP_INT32 ChaosArrayIndexOf_Byte_Sse2(
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
#if defined(_MSC_VER)
            unsigned long idx;
            _BitScanForward(&idx, static_cast<unsigned long>(mask));
            return static_cast<CHAOS_IL2CPP_INT32>(i + static_cast<CHAOS_IL2CPP_INTPTR>(idx));
#else
            return static_cast<CHAOS_IL2CPP_INT32>(i + __builtin_ctz(static_cast<unsigned>(mask)));
#endif
        }
    }
    for (; i < len; ++i) {
        if (elements[i] == value) return static_cast<CHAOS_IL2CPP_INT32>(i);
    }
    return -1;
}

CHAOS_IL2CPP_INT32 ChaosArrayIndexOf_Byte_Inline(
    CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_UINT8 value) noexcept
{
    if (array == 0) return -1;
    const auto* arr = get_managed_array(array);
    const auto* elements = reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(accessor_get_elements(arr));
    return ChaosArrayIndexOf_Byte_Sse2(elements, arr->length, value);
}
#endif

CHAOS_IL2CPP_INTPTR ChaosArrayEmpty_Inline(void) noexcept
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

void ChaosArrayCopy_Inline(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INT32 sourceIndex, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT32 destIndex, CHAOS_IL2CPP_INT32 count) noexcept
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

void ChaosArrayCopy_Unsafe_Inline(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INT32 sourceIndex, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT32 destIndex, CHAOS_IL2CPP_INT32 count) noexcept
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
        Avx2StreamCopy(dst_ptr, src_ptr, bytes);
        return;
    }
    if (bytes > 64 && chaos::il2cpp::runtime_core::HasAvx2()) {
        Avx2BlockCopy(dst_ptr, src_ptr, bytes);
        return;
    }
#endif
    std::memmove(dst_ptr, src_ptr, bytes);
}

void ChaosArrayCopy3_Inline(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT32 count) noexcept
{
    ChaosArrayCopy_Inline(source, 0, dest, 0, count);
}

void ChaosArrayCopy3_Unsafe_Inline(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT32 count) noexcept
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

CHAOS_IL2CPP_INT32 ChaosArrayBinarySearch_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (array == 0) return -1;
    const auto* arr = get_managed_array(array);
    const auto* elements = accessor_get_elements(arr);
    auto len = static_cast<CHAOS_IL2CPP_INT32>(arr->length);
    CHAOS_IL2CPP_INT32 lo = 0;
    CHAOS_IL2CPP_INT32 hi = len - 1;
    while (lo <= hi) {
        CHAOS_IL2CPP_INT32 mid = lo + (hi - lo) / 2;
        CHAOS_IL2CPP_INTPTR elem = elements[mid];
        lo = (elem < value) ? (mid + 1) : lo;
        hi = (elem > value) ? (mid - 1) : hi;
        if (elem == value) return mid;
    }
    return ~lo;
}

CHAOS_IL2CPP_INT32 ChaosArrayBinarySearchRange_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 length, CHAOS_IL2CPP_INTPTR value) noexcept
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

#if defined(__x86_64__) || defined(_M_AMD64)
CHAOS_IL2CPP_INT32 IndexOf_Sse2(
    const CHAOS_IL2CPP_INTPTR* elements, CHAOS_IL2CPP_INTPTR len, CHAOS_IL2CPP_INTPTR value) noexcept
{
    const int lo32 = static_cast<int>(static_cast<CHAOS_IL2CPP_UINTPTR>(value));
    const int hi32 = static_cast<int>(static_cast<CHAOS_IL2CPP_UINTPTR>(value) >> 32);
    const __m128i val = _mm_setr_epi32(lo32, hi32, lo32, hi32);

    CHAOS_IL2CPP_INTPTR i = 0;
    for (; i + 1 < len; i += 2) {
        const __m128i chunk = _mm_loadu_si128(
            reinterpret_cast<const __m128i*>(elements + i));
        const __m128i cmp = _mm_cmpeq_epi32(chunk, val);
        const int mask = _mm_movemask_ps(_mm_castsi128_ps(cmp));
        if ((mask & 3) == 3) return static_cast<CHAOS_IL2CPP_INT32>(i);
        if ((mask & 0xC) == 0xC) return static_cast<CHAOS_IL2CPP_INT32>(i + 1);
    }
    if (i < len && elements[i] == value) return static_cast<CHAOS_IL2CPP_INT32>(i);
    return -1;
}

CHAOS_IL2CPP_INT32 LastIndexOf_Sse2(
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
        if ((mask & 0xC) == 0xC) return static_cast<CHAOS_IL2CPP_INT32>(i + 1);
        if ((mask & 3) == 3) return static_cast<CHAOS_IL2CPP_INT32>(i);
    }
    if (i > 0 && elements[0] == value) return 0;
    return -1;
}
#endif

CHAOS_IL2CPP_INT32 ChaosArrayIndexOf_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept
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

CHAOS_IL2CPP_INT32 ChaosArrayLastIndexOf_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept
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

void ChaosArraySort_Inline(CHAOS_IL2CPP_INTPTR array) noexcept
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

void ChaosArraySortWithComparer_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR comparer) noexcept
{
    (void)comparer;
    ChaosArraySort_Inline(array);
}

// ── AVX2-accelerated Reverse (P2) ──────────────────────────────
#if defined(__x86_64__) || defined(_M_AMD64)
#include <immintrin.h>

#if defined(__GNUC__) || defined(__clang__)
#define CHAOS_IL2CPP_TARGET_AVX2 __attribute__((target("avx2")))
#else
#define CHAOS_IL2CPP_TARGET_AVX2
#endif

CHAOS_IL2CPP_TARGET_AVX2
void Reverse_Avx2_Dispatch(
    CHAOS_IL2CPP_INTPTR* elements, CHAOS_IL2CPP_INT32 n) noexcept
{
    CHAOS_IL2CPP_INT32 i = 0;
    CHAOS_IL2CPP_INT32 j = n - 1;
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
    while (i < j) {
        CHAOS_IL2CPP_INTPTR tmp = elements[i];
        elements[i] = elements[j];
        elements[j] = tmp;
        ++i; --j;
    }
}

CHAOS_IL2CPP_TARGET_AVX2
CHAOS_IL2CPP_INT32 IndexOf_Avx2_Dispatch(
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
CHAOS_IL2CPP_INT32 LastIndexOf_Avx2_Dispatch(
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

CHAOS_IL2CPP_TARGET_AVX2
void Avx2BlockCopy(void* dst, const void* src, CHAOS_IL2CPP_SIZE bytes) noexcept
{
    CHAOS_IL2CPP_SIZE i = 0;
    for (; i + 32 <= bytes; i += 32) {
        __m256i chunk = _mm256_loadu_si256(
            reinterpret_cast<const __m256i*>(static_cast<const CHAOS_IL2CPP_UINT8*>(src) + i));
        _mm256_storeu_si256(
            reinterpret_cast<__m256i*>(static_cast<CHAOS_IL2CPP_UINT8*>(dst) + i), chunk);
    }
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
}

CHAOS_IL2CPP_TARGET_AVX2
void Avx2StreamCopy(void* dst, const void* src, CHAOS_IL2CPP_SIZE bytes) noexcept
{
    CHAOS_IL2CPP_SIZE i = 0;
    for (; i + 32 <= bytes; i += 32) {
        __m256i chunk = _mm256_loadu_si256(
            reinterpret_cast<const __m256i*>(static_cast<const CHAOS_IL2CPP_UINT8*>(src) + i));
        _mm256_stream_si256(
            reinterpret_cast<__m256i*>(static_cast<CHAOS_IL2CPP_UINT8*>(dst) + i), chunk);
    }
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
    _mm_sfence();
}

CHAOS_IL2CPP_TARGET_AVX2
void Avx2StreamZero(void* dst, CHAOS_IL2CPP_SIZE bytes) noexcept
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

void ChaosArrayReverse_Inline(CHAOS_IL2CPP_INTPTR array) noexcept
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
    CHAOS_IL2CPP_INT32 i = 0;
    CHAOS_IL2CPP_INT32 j = n - 1;
    while (i < j) {
        CHAOS_IL2CPP_INTPTR tmp = elements[i];
        elements[i] = elements[j];
        elements[j] = tmp;
        ++i; --j;
    }
}

CHAOS_IL2CPP_INTPTR ChaosArrayGetValue_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index) noexcept
{
    if (array == 0) return 0;
    const auto* arr = get_managed_array(array);
    CHAOS_IL2CPP_UINTPTR uindex = static_cast<CHAOS_IL2CPP_UINTPTR>(index);
    if (uindex >= static_cast<CHAOS_IL2CPP_UINTPTR>(arr->length)) return 0;
    return accessor_get_elements(arr)[uindex];
}

void ChaosArrayClear_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 count) noexcept
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

    if (CHAOS_IL2CPP_LIKELY(bytes <= 64)) {
        if (bytes <= 8) {
            CHAOS_IL2CPP_INTPTR zero = 0;
            memcpy(dst, &zero, bytes);
            return;
        }
        memset(dst, 0, bytes);
        return;
    }
#if defined(__x86_64__) || defined(_M_AMD64)
    if (bytes > 256 && chaos::il2cpp::runtime_core::HasAvx2()) {
        Avx2StreamZero(dst, bytes);
        return;
    }
#endif
    std::memset(dst, 0, bytes);
}

CHAOS_IL2CPP_INTPTR ChaosArrayNew1D_Inline(
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
    *reinterpret_cast<const TypeInfo**>(arr->header_data) = array_type_info;
    arr->element_type_shape = element_type_shape;
    arr->element_type_info = element_type_info;
    arr->length = static_cast<CHAOS_IL2CPP_INTPTR>(len32);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}
