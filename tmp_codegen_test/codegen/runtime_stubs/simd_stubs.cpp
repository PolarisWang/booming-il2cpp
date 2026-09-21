// ── SIMD stubs implementations ─────────────────────────────────
// ABI exports: extern "C" for managed/NativeAOT callability.
// Uses RuntimeIntrinsicVector128Carrier::value (__m128i) directly
// to pass SIMD vectors in XMM registers without memory round-trips.
//
// x86-64 SSE/AVX intrinsics only. ARM64 NEON support TBD.
#include "numerics_carriers.h"
#include <cstdint>
#include "simd_stubs.h"

#if defined(__x86_64__) || defined(_M_AMD64)
#include <immintrin.h>
#pragma GCC target("avx2,sse4.2")
#else
#error "SIMD stubs require x86-64 (SSE2+ / AVX2)"
#endif

using V128 = chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector128Carrier;
using V256 = chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector256Carrier;

// ════════════════════════════════════════════════════════════
// Vector128 int32
// ════════════════════════════════════════════════════════════
extern "C" {

V128 ChaosSimd_V128_Add_I32(V128 a, V128 b) noexcept {
    return { _mm_add_epi32(a.value, b.value) };
}

V128 ChaosSimd_V128_Sub_I32(V128 a, V128 b) noexcept {
    return { _mm_sub_epi32(a.value, b.value) };
}

V128 ChaosSimd_V128_Mul_I32(V128 a, V128 b) noexcept {
    return { _mm_mullo_epi32(a.value, b.value) };
}

V128 ChaosSimd_V128_CmpEq_I32(V128 a, V128 b) noexcept {
    return { _mm_cmpeq_epi32(a.value, b.value) };
}

V128 ChaosSimd_V128_Shl_I32(ChaosSimdV128 a, CHAOS_IL2CPP_INT32 b) noexcept {
    return { _mm_slli_epi32(a.value, b) };
}

// ════════════════════════════════════════════════════════════
// Vector128 float
// ════════════════════════════════════════════════════════════

V128 ChaosSimd_V128_Add_F32(V128 a, V128 b) noexcept {
    return { _mm_castps_si128(_mm_add_ps(_mm_castsi128_ps(a.value), _mm_castsi128_ps(b.value))) };
}

V128 ChaosSimd_V128_Sub_F32(V128 a, V128 b) noexcept {
    return { _mm_castps_si128(_mm_sub_ps(_mm_castsi128_ps(a.value), _mm_castsi128_ps(b.value))) };
}

V128 ChaosSimd_V128_Mul_F32(V128 a, V128 b) noexcept {
    return { _mm_castps_si128(_mm_mul_ps(_mm_castsi128_ps(a.value), _mm_castsi128_ps(b.value))) };
}

V128 ChaosSimd_V128_CmpEq_F32(V128 a, V128 b) noexcept {
    return { _mm_castps_si128(_mm_cmpeq_ps(_mm_castsi128_ps(a.value), _mm_castsi128_ps(b.value))) };
}

// ════════════════════════════════════════════════════════════
// Vector128 double
// ════════════════════════════════════════════════════════════

V128 ChaosSimd_V128_Add_F64(V128 a, V128 b) noexcept {
    return { _mm_castpd_si128(_mm_add_pd(_mm_castsi128_pd(a.value), _mm_castsi128_pd(b.value))) };
}

V128 ChaosSimd_V128_Sub_F64(V128 a, V128 b) noexcept {
    return { _mm_castpd_si128(_mm_sub_pd(_mm_castsi128_pd(a.value), _mm_castsi128_pd(b.value))) };
}

V128 ChaosSimd_V128_Mul_F64(V128 a, V128 b) noexcept {
    return { _mm_castpd_si128(_mm_mul_pd(_mm_castsi128_pd(a.value), _mm_castsi128_pd(b.value))) };
}

// ════════════════════════════════════════════════════════════
// Vector128 bitwise (type-agnostic)
// ════════════════════════════════════════════════════════════

V128 ChaosSimd_V128_And(V128 a, V128 b) noexcept {
    return { _mm_and_si128(a.value, b.value) };
}

V128 ChaosSimd_V128_Or(V128 a, V128 b) noexcept {
    return { _mm_or_si128(a.value, b.value) };
}

V128 ChaosSimd_V128_Xor(V128 a, V128 b) noexcept {
    return { _mm_xor_si128(a.value, b.value) };
}

// ════════════════════════════════════════════════════════════
// Vector256 int32
// ════════════════════════════════════════════════════════════

V256 ChaosSimd_V256_Add_I32(V256 a, V256 b) noexcept {
    return { _mm256_add_epi32(a.value, b.value) };
}

V256 ChaosSimd_V256_Sub_I32(V256 a, V256 b) noexcept {
    return { _mm256_sub_epi32(a.value, b.value) };
}

V256 ChaosSimd_V256_Mul_I32(V256 a, V256 b) noexcept {
    return { _mm256_mullo_epi32(a.value, b.value) };
}

V256 ChaosSimd_V256_CmpEq_I32(V256 a, V256 b) noexcept {
    return { _mm256_cmpeq_epi32(a.value, b.value) };
}

V256 ChaosSimd_V256_Shl_I32(ChaosSimdV256 a, CHAOS_IL2CPP_INT32 b) noexcept {
    return { _mm256_slli_epi32(a.value, b) };
}

// ════════════════════════════════════════════════════════════
// Vector256 float
// ════════════════════════════════════════════════════════════

V256 ChaosSimd_V256_Add_F32(V256 a, V256 b) noexcept {
    return { _mm256_castps_si256(_mm256_add_ps(_mm256_castsi256_ps(a.value), _mm256_castsi256_ps(b.value))) };
}

V256 ChaosSimd_V256_Sub_F32(V256 a, V256 b) noexcept {
    return { _mm256_castps_si256(_mm256_sub_ps(_mm256_castsi256_ps(a.value), _mm256_castsi256_ps(b.value))) };
}

V256 ChaosSimd_V256_Mul_F32(V256 a, V256 b) noexcept {
    return { _mm256_castps_si256(_mm256_mul_ps(_mm256_castsi256_ps(a.value), _mm256_castsi256_ps(b.value))) };
}

V256 ChaosSimd_V256_CmpEq_F32(V256 a, V256 b) noexcept {
    return { _mm256_castps_si256(_mm256_cmp_ps(_mm256_castsi256_ps(a.value), _mm256_castsi256_ps(b.value), _CMP_EQ_OQ)) };
}

// ════════════════════════════════════════════════════════════
// Vector256 double
// ════════════════════════════════════════════════════════════

V256 ChaosSimd_V256_Add_F64(V256 a, V256 b) noexcept {
    return { _mm256_castpd_si256(_mm256_add_pd(_mm256_castsi256_pd(a.value), _mm256_castsi256_pd(b.value))) };
}

V256 ChaosSimd_V256_Sub_F64(V256 a, V256 b) noexcept {
    return { _mm256_castpd_si256(_mm256_sub_pd(_mm256_castsi256_pd(a.value), _mm256_castsi256_pd(b.value))) };
}

V256 ChaosSimd_V256_Mul_F64(V256 a, V256 b) noexcept {
    return { _mm256_castpd_si256(_mm256_mul_pd(_mm256_castsi256_pd(a.value), _mm256_castsi256_pd(b.value))) };
}

// ════════════════════════════════════════════════════════════
// Vector256 bitwise (type-agnostic)
// ════════════════════════════════════════════════════════════

V256 ChaosSimd_V256_And(V256 a, V256 b) noexcept {
    return { _mm256_and_si256(a.value, b.value) };
}

V256 ChaosSimd_V256_Or(V256 a, V256 b) noexcept {
    return { _mm256_or_si256(a.value, b.value) };
}

V256 ChaosSimd_V256_Xor(V256 a, V256 b) noexcept {
    return { _mm256_xor_si256(a.value, b.value) };
}

}  // extern "C"
