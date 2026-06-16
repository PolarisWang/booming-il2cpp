// ── SIMD stubs implementations ─────────────────────────────────
// ABI exports: extern "C" for managed/NativeAOT callability.
// Each function converts the plain-byte carrier to __m128i/__m128,
// performs the SIMD operation, and stores back.
//
// x86-64 SSE/AVX intrinsics only. ARM64 NEON support TBD.
#include "numerics_carriers.h"
#include <cstdint>
#include "simd_stubs.h"

#if defined(__x86_64__) || defined(_M_AMD64)
#include <immintrin.h>
// This file uses SSE4.1 (_mm_mullo_epi32) and AVX (_mm256_*) intrinsics.
// The pragma ensures these compile even when the consumer project lacks -mavx.
#pragma GCC target("avx2,sse4.2")
#else
// Stub: non-x86 platforms compile to scalar fallback (unreachable on current targets)
#error "SIMD stubs require x86-64 (SSE2+ / AVX2)"
#endif

using V128 = chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector128Carrier;
using V256 = chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector256Carrier;

// ════════════════════════════════════════════════════════════
// Vector128 int32
// ════════════════════════════════════════════════════════════
extern "C" {

V128 ChaosSimd_V128_Add_I32(V128 a, V128 b) noexcept {
    __m128i va = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&a));
    __m128i vb = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&b));
    __m128i vr = _mm_add_epi32(va, vb);
    V128 result;
    _mm_storeu_si128(reinterpret_cast<__m128i*>(&result), vr);
    return result;
}

V128 ChaosSimd_V128_Sub_I32(V128 a, V128 b) noexcept {
    __m128i va = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&a));
    __m128i vb = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&b));
    __m128i vr = _mm_sub_epi32(va, vb);
    V128 result;
    _mm_storeu_si128(reinterpret_cast<__m128i*>(&result), vr);
    return result;
}

V128 ChaosSimd_V128_Mul_I32(V128 a, V128 b) noexcept {
    __m128i va = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&a));
    __m128i vb = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&b));
    __m128i vr = _mm_mullo_epi32(va, vb);  // SSE4.1
    V128 result;
    _mm_storeu_si128(reinterpret_cast<__m128i*>(&result), vr);
    return result;
}

V128 ChaosSimd_V128_CmpEq_I32(V128 a, V128 b) noexcept {
    __m128i va = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&a));
    __m128i vb = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&b));
    __m128i vr = _mm_cmpeq_epi32(va, vb);
    V128 result;
    _mm_storeu_si128(reinterpret_cast<__m128i*>(&result), vr);
    return result;
}

V128 ChaosSimd_V128_Shl_I32(V128 a, CHAOS_IL2CPP_INT32 b) noexcept {
    __m128i va = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&a));
    __m128i vr = _mm_slli_epi32(va, b);
    V128 result;
    _mm_storeu_si128(reinterpret_cast<__m128i*>(&result), vr);
    return result;
}

// ════════════════════════════════════════════════════════════
// Vector128 float
// ════════════════════════════════════════════════════════════

V128 ChaosSimd_V128_Add_F32(V128 a, V128 b) noexcept {
    __m128 va = _mm_loadu_ps(reinterpret_cast<const float*>(&a));
    __m128 vb = _mm_loadu_ps(reinterpret_cast<const float*>(&b));
    __m128 vr = _mm_add_ps(va, vb);
    V128 result;
    _mm_storeu_ps(reinterpret_cast<float*>(&result), vr);
    return result;
}

V128 ChaosSimd_V128_Sub_F32(V128 a, V128 b) noexcept {
    __m128 va = _mm_loadu_ps(reinterpret_cast<const float*>(&a));
    __m128 vb = _mm_loadu_ps(reinterpret_cast<const float*>(&b));
    __m128 vr = _mm_sub_ps(va, vb);
    V128 result;
    _mm_storeu_ps(reinterpret_cast<float*>(&result), vr);
    return result;
}

V128 ChaosSimd_V128_Mul_F32(V128 a, V128 b) noexcept {
    __m128 va = _mm_loadu_ps(reinterpret_cast<const float*>(&a));
    __m128 vb = _mm_loadu_ps(reinterpret_cast<const float*>(&b));
    __m128 vr = _mm_mul_ps(va, vb);
    V128 result;
    _mm_storeu_ps(reinterpret_cast<float*>(&result), vr);
    return result;
}

V128 ChaosSimd_V128_CmpEq_F32(V128 a, V128 b) noexcept {
    __m128 va = _mm_loadu_ps(reinterpret_cast<const float*>(&a));
    __m128 vb = _mm_loadu_ps(reinterpret_cast<const float*>(&b));
    __m128 vr = _mm_cmpeq_ps(va, vb);
    V128 result;
    _mm_storeu_ps(reinterpret_cast<float*>(&result), vr);
    return result;
}

// ════════════════════════════════════════════════════════════
// Vector128 double
// ════════════════════════════════════════════════════════════

V128 ChaosSimd_V128_Add_F64(V128 a, V128 b) noexcept {
    __m128d va = _mm_loadu_pd(reinterpret_cast<const double*>(&a));
    __m128d vb = _mm_loadu_pd(reinterpret_cast<const double*>(&b));
    __m128d vr = _mm_add_pd(va, vb);
    V128 result;
    _mm_storeu_pd(reinterpret_cast<double*>(&result), vr);
    return result;
}

V128 ChaosSimd_V128_Sub_F64(V128 a, V128 b) noexcept {
    __m128d va = _mm_loadu_pd(reinterpret_cast<const double*>(&a));
    __m128d vb = _mm_loadu_pd(reinterpret_cast<const double*>(&b));
    __m128d vr = _mm_sub_pd(va, vb);
    V128 result;
    _mm_storeu_pd(reinterpret_cast<double*>(&result), vr);
    return result;
}

V128 ChaosSimd_V128_Mul_F64(V128 a, V128 b) noexcept {
    __m128d va = _mm_loadu_pd(reinterpret_cast<const double*>(&a));
    __m128d vb = _mm_loadu_pd(reinterpret_cast<const double*>(&b));
    __m128d vr = _mm_mul_pd(va, vb);
    V128 result;
    _mm_storeu_pd(reinterpret_cast<double*>(&result), vr);
    return result;
}

// ════════════════════════════════════════════════════════════
// Vector128 bitwise (type-agnostic)
// ════════════════════════════════════════════════════════════

V128 ChaosSimd_V128_And(V128 a, V128 b) noexcept {
    __m128i va = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&a));
    __m128i vb = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&b));
    __m128i vr = _mm_and_si128(va, vb);
    V128 result;
    _mm_storeu_si128(reinterpret_cast<__m128i*>(&result), vr);
    return result;
}

V128 ChaosSimd_V128_Or(V128 a, V128 b) noexcept {
    __m128i va = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&a));
    __m128i vb = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&b));
    __m128i vr = _mm_or_si128(va, vb);
    V128 result;
    _mm_storeu_si128(reinterpret_cast<__m128i*>(&result), vr);
    return result;
}

V128 ChaosSimd_V128_Xor(V128 a, V128 b) noexcept {
    __m128i va = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&a));
    __m128i vb = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&b));
    __m128i vr = _mm_xor_si128(va, vb);
    V128 result;
    _mm_storeu_si128(reinterpret_cast<__m128i*>(&result), vr);
    return result;
}

// ════════════════════════════════════════════════════════════
// Vector256 int32
// ════════════════════════════════════════════════════════════

V256 ChaosSimd_V256_Add_I32(V256 a, V256 b) noexcept {
    __m256i va = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&a));
    __m256i vb = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&b));
    __m256i vr = _mm256_add_epi32(va, vb);
    V256 result;
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(&result), vr);
    return result;
}

V256 ChaosSimd_V256_Sub_I32(V256 a, V256 b) noexcept {
    __m256i va = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&a));
    __m256i vb = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&b));
    __m256i vr = _mm256_sub_epi32(va, vb);
    V256 result;
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(&result), vr);
    return result;
}

V256 ChaosSimd_V256_Mul_I32(V256 a, V256 b) noexcept {
    __m256i va = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&a));
    __m256i vb = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&b));
    __m256i vr = _mm256_mullo_epi32(va, vb);
    V256 result;
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(&result), vr);
    return result;
}

V256 ChaosSimd_V256_CmpEq_I32(V256 a, V256 b) noexcept {
    __m256i va = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&a));
    __m256i vb = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&b));
    __m256i vr = _mm256_cmpeq_epi32(va, vb);
    V256 result;
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(&result), vr);
    return result;
}

V256 ChaosSimd_V256_Shl_I32(V256 a, CHAOS_IL2CPP_INT32 b) noexcept {
    __m256i va = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&a));
    __m256i vr = _mm256_slli_epi32(va, b);
    V256 result;
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(&result), vr);
    return result;
}

// ════════════════════════════════════════════════════════════
// Vector256 float
// ════════════════════════════════════════════════════════════

V256 ChaosSimd_V256_Add_F32(V256 a, V256 b) noexcept {
    __m256 va = _mm256_loadu_ps(reinterpret_cast<const float*>(&a));
    __m256 vb = _mm256_loadu_ps(reinterpret_cast<const float*>(&b));
    __m256 vr = _mm256_add_ps(va, vb);
    V256 result;
    _mm256_storeu_ps(reinterpret_cast<float*>(&result), vr);
    return result;
}

V256 ChaosSimd_V256_Sub_F32(V256 a, V256 b) noexcept {
    __m256 va = _mm256_loadu_ps(reinterpret_cast<const float*>(&a));
    __m256 vb = _mm256_loadu_ps(reinterpret_cast<const float*>(&b));
    __m256 vr = _mm256_sub_ps(va, vb);
    V256 result;
    _mm256_storeu_ps(reinterpret_cast<float*>(&result), vr);
    return result;
}

V256 ChaosSimd_V256_Mul_F32(V256 a, V256 b) noexcept {
    __m256 va = _mm256_loadu_ps(reinterpret_cast<const float*>(&a));
    __m256 vb = _mm256_loadu_ps(reinterpret_cast<const float*>(&b));
    __m256 vr = _mm256_mul_ps(va, vb);
    V256 result;
    _mm256_storeu_ps(reinterpret_cast<float*>(&result), vr);
    return result;
}

V256 ChaosSimd_V256_CmpEq_F32(V256 a, V256 b) noexcept {
    __m256 va = _mm256_loadu_ps(reinterpret_cast<const float*>(&a));
    __m256 vb = _mm256_loadu_ps(reinterpret_cast<const float*>(&b));
    __m256 vr = _mm256_cmp_ps(va, vb, _CMP_EQ_OQ);
    V256 result;
    _mm256_storeu_ps(reinterpret_cast<float*>(&result), vr);
    return result;
}

// ════════════════════════════════════════════════════════════
// Vector256 double
// ════════════════════════════════════════════════════════════

V256 ChaosSimd_V256_Add_F64(V256 a, V256 b) noexcept {
    __m256d va = _mm256_loadu_pd(reinterpret_cast<const double*>(&a));
    __m256d vb = _mm256_loadu_pd(reinterpret_cast<const double*>(&b));
    __m256d vr = _mm256_add_pd(va, vb);
    V256 result;
    _mm256_storeu_pd(reinterpret_cast<double*>(&result), vr);
    return result;
}

V256 ChaosSimd_V256_Sub_F64(V256 a, V256 b) noexcept {
    __m256d va = _mm256_loadu_pd(reinterpret_cast<const double*>(&a));
    __m256d vb = _mm256_loadu_pd(reinterpret_cast<const double*>(&b));
    __m256d vr = _mm256_sub_pd(va, vb);
    V256 result;
    _mm256_storeu_pd(reinterpret_cast<double*>(&result), vr);
    return result;
}

V256 ChaosSimd_V256_Mul_F64(V256 a, V256 b) noexcept {
    __m256d va = _mm256_loadu_pd(reinterpret_cast<const double*>(&a));
    __m256d vb = _mm256_loadu_pd(reinterpret_cast<const double*>(&b));
    __m256d vr = _mm256_mul_pd(va, vb);
    V256 result;
    _mm256_storeu_pd(reinterpret_cast<double*>(&result), vr);
    return result;
}

// ════════════════════════════════════════════════════════════
// Vector256 bitwise (type-agnostic)
// ════════════════════════════════════════════════════════════

V256 ChaosSimd_V256_And(V256 a, V256 b) noexcept {
    __m256i va = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&a));
    __m256i vb = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&b));
    __m256i vr = _mm256_and_si256(va, vb);
    V256 result;
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(&result), vr);
    return result;
}

V256 ChaosSimd_V256_Or(V256 a, V256 b) noexcept {
    __m256i va = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&a));
    __m256i vb = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&b));
    __m256i vr = _mm256_or_si256(va, vb);
    V256 result;
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(&result), vr);
    return result;
}

V256 ChaosSimd_V256_Xor(V256 a, V256 b) noexcept {
    __m256i va = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&a));
    __m256i vb = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&b));
    __m256i vr = _mm256_xor_si256(va, vb);
    V256 result;
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(&result), vr);
    return result;
}

}  // extern "C"
