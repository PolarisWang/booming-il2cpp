// ── Array stub declarations ────────────────────────────────────
#pragma once

#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include <chaos/compiler_hints.h>
#include <cstring>
#include "runtime_stubs/stub_common.h"
#include "gc_helpers.h"
#include "cpu_features.h"

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

// ── ABI exports (chaos_runtime_stubs) ──────────────────────────
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

// ── Inline implementation wrappers (extern "C" linkage) ──
// These are used by the AOT codegen when DirectNativeSymbol resolves to
// ChaosArrayXxx_Inline.  Their implementations are in array_stubs.cpp
// (not inline in the header) to avoid C2084 when the header is included
// from multiple translation units (via chaos_pch.h → chaos_runtime_host.h).
// extern "C" linkage avoids C++ name mangling, so codegen calling these from
// any namespace (chaos::il2cpp::codegen::*) resolves correctly at link time.

CHAOS_IL2CPP_INTPTR ChaosArrayEmpty_Inline(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosArrayNew1D_Inline(const TypeInfo* array_type_info, const TypeInfo* element_type_info, CHAOS_IL2CPP_UINT8 element_type_shape, CHAOS_IL2CPP_INTPTR length) noexcept;
void ChaosArrayClear_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 count) noexcept;

#if defined(__x86_64__) || defined(_M_AMD64)
CHAOS_IL2CPP_INT32 ChaosArrayIndexOf_Byte_Sse2(
    const CHAOS_IL2CPP_UINT8* elements, CHAOS_IL2CPP_INTPTR len,
    CHAOS_IL2CPP_UINT8 value) noexcept;
CHAOS_IL2CPP_INT32 ChaosArrayIndexOf_Byte_Inline(
    CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_UINT8 value) noexcept;
#endif

void ChaosArrayCopy_Inline(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INT32 sourceIndex, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT32 destIndex, CHAOS_IL2CPP_INT32 count) noexcept;
void ChaosArrayCopy_Unsafe_Inline(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INT32 sourceIndex, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT32 destIndex, CHAOS_IL2CPP_INT32 count) noexcept;
void ChaosArrayCopy3_Inline(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT32 count) noexcept;
void ChaosArrayCopy3_Unsafe_Inline(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT32 count) noexcept;

CHAOS_IL2CPP_INT32 ChaosArrayBinarySearch_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32 ChaosArrayBinarySearchRange_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 length, CHAOS_IL2CPP_INTPTR value) noexcept;

#if defined(__x86_64__) || defined(_M_AMD64)
CHAOS_IL2CPP_INT32 IndexOf_Sse2(
    const CHAOS_IL2CPP_INTPTR* elements, CHAOS_IL2CPP_INTPTR len, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32 LastIndexOf_Sse2(
    const CHAOS_IL2CPP_INTPTR* elements, CHAOS_IL2CPP_INTPTR len, CHAOS_IL2CPP_INTPTR value) noexcept;
#endif

CHAOS_IL2CPP_INT32 ChaosArrayIndexOf_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32 ChaosArrayLastIndexOf_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept;

void ChaosArraySort_Inline(CHAOS_IL2CPP_INTPTR array) noexcept;
void ChaosArraySortWithComparer_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR comparer) noexcept;

void ChaosArrayReverse_Inline(CHAOS_IL2CPP_INTPTR array) noexcept;
CHAOS_IL2CPP_INTPTR ChaosArrayGetValue_Inline(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index) noexcept;

// ── AVX2-accelerated Reverse/IndexOf/Copy (P2/P4/P5) ───────────
#if defined(__x86_64__) || defined(_M_AMD64)
#include <immintrin.h>
#if defined(__GNUC__) || defined(__clang__)
#define CHAOS_IL2CPP_TARGET_AVX2 __attribute__((target("avx2")))
#else
#define CHAOS_IL2CPP_TARGET_AVX2
#endif
CHAOS_IL2CPP_TARGET_AVX2 void Reverse_Avx2_Dispatch(CHAOS_IL2CPP_INTPTR* elements, CHAOS_IL2CPP_INT32 n) noexcept;
CHAOS_IL2CPP_TARGET_AVX2 CHAOS_IL2CPP_INT32 IndexOf_Avx2_Dispatch(const CHAOS_IL2CPP_INTPTR* elements, CHAOS_IL2CPP_INTPTR len, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_TARGET_AVX2 CHAOS_IL2CPP_INT32 LastIndexOf_Avx2_Dispatch(const CHAOS_IL2CPP_INTPTR* elements, CHAOS_IL2CPP_INTPTR len, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_TARGET_AVX2 void Avx2BlockCopy(void* dst, const void* src, CHAOS_IL2CPP_SIZE bytes) noexcept;
CHAOS_IL2CPP_TARGET_AVX2 void Avx2StreamCopy(void* dst, const void* src, CHAOS_IL2CPP_SIZE bytes) noexcept;
CHAOS_IL2CPP_TARGET_AVX2 void Avx2StreamZero(void* dst, CHAOS_IL2CPP_SIZE bytes) noexcept;
#undef CHAOS_IL2CPP_TARGET_AVX2
#endif

}  // extern "C"
