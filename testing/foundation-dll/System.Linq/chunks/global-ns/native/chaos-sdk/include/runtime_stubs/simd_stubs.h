// simd_stubs.h — Native SIMD stub implementations for AOT codegen
#pragma once
#include <cstdint>
#include <chaos/native_types.h>

#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
#include <x86intrin.h>
#endif

namespace chaos::il2cpp::simd {

// ── Helper: reinterpret intptr ABI pairs as __m128i ────────────────
inline __m128i Load128(const CHAOS_IL2CPP_INTPTR* ptr) noexcept {
    alignas(16) uint64_t tmp[2] = {static_cast<uint64_t>(ptr[0]), static_cast<uint64_t>(ptr[1])};
    return _mm_loadu_si128(reinterpret_cast<const __m128i*>(tmp));
}

inline void Store128(CHAOS_IL2CPP_INTPTR* ptr, __m128i val) noexcept {
    alignas(16) uint64_t tmp[2];
    _mm_storeu_si128(reinterpret_cast<__m128i*>(tmp), val);
    ptr[0] = static_cast<CHAOS_IL2CPP_INTPTR>(tmp[0]);
    ptr[1] = static_cast<CHAOS_IL2CPP_INTPTR>(tmp[1]);
}

// ── Vector128.Add ─────────────────────────────────────────────────
inline CHAOS_IL2CPP_INTPTR Vector128Add(
    CHAOS_IL2CPP_INTPTR a_low, CHAOS_IL2CPP_INTPTR a_high,
    CHAOS_IL2CPP_INTPTR b_low, CHAOS_IL2CPP_INTPTR b_high,
    uint8_t elem_type) noexcept
{
    alignas(16) CHAOS_IL2CPP_INTPTR args_a[2] = {a_low, a_high};
    alignas(16) CHAOS_IL2CPP_INTPTR args_b[2] = {b_low, b_high};
    __m128i va = Load128(args_a), vb = Load128(args_b);
    __m128i vr;
    switch (elem_type) {
    case 0:  vr = _mm_add_epi8(va, vb);  break;
    case 1:  vr = _mm_add_epi16(va, vb); break;
    case 2:  vr = _mm_add_epi32(va, vb); break;
    case 3:  vr = _mm_add_epi64(va, vb); break;
    case 4:  vr = _mm_castps_si128(_mm_add_ps(_mm_castsi128_ps(va), _mm_castsi128_ps(vb))); break;
    case 5:  vr = _mm_castpd_si128(_mm_add_pd(_mm_castsi128_pd(va), _mm_castsi128_pd(vb))); break;
    default: return 0;
    }
    alignas(16) CHAOS_IL2CPP_INTPTR result[2];
    Store128(result, vr);
    return result[0];
}

// ── Vector128.Sub / Mul / And / Or / Xor ─────────────────────────
inline CHAOS_IL2CPP_INTPTR Vector128Sub(
    CHAOS_IL2CPP_INTPTR a_low, CHAOS_IL2CPP_INTPTR a_high,
    CHAOS_IL2CPP_INTPTR b_low, CHAOS_IL2CPP_INTPTR b_high,
    uint8_t elem_type) noexcept
{
    alignas(16) CHAOS_IL2CPP_INTPTR args_a[2] = {a_low, a_high}, args_b[2] = {b_low, b_high};
    __m128i va = Load128(args_a), vb = Load128(args_b), vr;
    switch (elem_type) {
    case 0:  vr = _mm_sub_epi8(va, vb);  break;
    case 1:  vr = _mm_sub_epi16(va, vb); break;
    case 2:  vr = _mm_sub_epi32(va, vb); break;
    case 3:  vr = _mm_sub_epi64(va, vb); break;
    case 4:  vr = _mm_castps_si128(_mm_sub_ps(_mm_castsi128_ps(va), _mm_castsi128_ps(vb))); break;
    case 5:  vr = _mm_castpd_si128(_mm_sub_pd(_mm_castsi128_pd(va), _mm_castsi128_pd(vb))); break;
    default: return 0;
    }
    alignas(16) CHAOS_IL2CPP_INTPTR result[2];
    Store128(result, vr);
    return result[0];
}

inline CHAOS_IL2CPP_INTPTR Vector128Mul(
    CHAOS_IL2CPP_INTPTR a_low, CHAOS_IL2CPP_INTPTR a_high,
    CHAOS_IL2CPP_INTPTR b_low, CHAOS_IL2CPP_INTPTR b_high,
    uint8_t elem_type) noexcept
{
    alignas(16) CHAOS_IL2CPP_INTPTR args_a[2] = {a_low, a_high}, args_b[2] = {b_low, b_high};
    __m128i va = Load128(args_a), vb = Load128(args_b), vr;
    switch (elem_type) {
    case 1:  vr = _mm_mullo_epi16(va, vb); break;
    case 2:  vr = _mm_mullo_epi32(va, vb); break;
    case 4:  vr = _mm_castps_si128(_mm_mul_ps(_mm_castsi128_ps(va), _mm_castsi128_ps(vb))); break;
    case 5:  vr = _mm_castpd_si128(_mm_mul_pd(_mm_castsi128_pd(va), _mm_castsi128_pd(vb))); break;
    default: return 0;
    }
    alignas(16) CHAOS_IL2CPP_INTPTR result[2];
    Store128(result, vr);
    return result[0];
}

inline CHAOS_IL2CPP_INTPTR Vector128And(
    CHAOS_IL2CPP_INTPTR a_low, CHAOS_IL2CPP_INTPTR a_high,
    CHAOS_IL2CPP_INTPTR b_low, CHAOS_IL2CPP_INTPTR b_high) noexcept
{
    alignas(16) CHAOS_IL2CPP_INTPTR args_a[2] = {a_low, a_high}, args_b[2] = {b_low, b_high};
    __m128i vr = _mm_and_si128(Load128(args_a), Load128(args_b));
    alignas(16) CHAOS_IL2CPP_INTPTR result[2];
    Store128(result, vr);
    return result[0];
}

inline CHAOS_IL2CPP_INTPTR Vector128Or(
    CHAOS_IL2CPP_INTPTR a_low, CHAOS_IL2CPP_INTPTR a_high,
    CHAOS_IL2CPP_INTPTR b_low, CHAOS_IL2CPP_INTPTR b_high) noexcept
{
    alignas(16) CHAOS_IL2CPP_INTPTR args_a[2] = {a_low, a_high}, args_b[2] = {b_low, b_high};
    __m128i vr = _mm_or_si128(Load128(args_a), Load128(args_b));
    alignas(16) CHAOS_IL2CPP_INTPTR result[2];
    Store128(result, vr);
    return result[0];
}

inline CHAOS_IL2CPP_INTPTR Vector128Xor(
    CHAOS_IL2CPP_INTPTR a_low, CHAOS_IL2CPP_INTPTR a_high,
    CHAOS_IL2CPP_INTPTR b_low, CHAOS_IL2CPP_INTPTR b_high) noexcept
{
    alignas(16) CHAOS_IL2CPP_INTPTR args_a[2] = {a_low, a_high}, args_b[2] = {b_low, b_high};
    __m128i vr = _mm_xor_si128(Load128(args_a), Load128(args_b));
    alignas(16) CHAOS_IL2CPP_INTPTR result[2];
    Store128(result, vr);
    return result[0];
}

}  // namespace chaos::il2cpp::simd

// ── Codegen-facing SIMD stubs (carrier-typed API) ────────────────
// These are called by inline shapes emitted by the AOT codegen
// (System.Runtime.Intrinsics.Vector128/256 operations).
// Types aliases match the .cpp implementations in simd_stubs.cpp.
#include "numerics_carriers.h"

extern "C" {
using V128 = chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector128Carrier;
using V256 = chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector256Carrier;
using ChaosSimdV128 = V128;  // backward compat (some SDK copies use this name)
using ChaosSimdV256 = V256;  // backward compat

// Vector128
V128 ChaosSimd_V128_Add_I32(V128 a, V128 b) noexcept;
V128 ChaosSimd_V128_Sub_I32(V128 a, V128 b) noexcept;
V128 ChaosSimd_V128_Mul_I32(V128 a, V128 b) noexcept;
V128 ChaosSimd_V128_And(V128 a, V128 b) noexcept;
V128 ChaosSimd_V128_Or(V128 a, V128 b) noexcept;
V128 ChaosSimd_V128_Xor(V128 a, V128 b) noexcept;

// Vector256
V256 ChaosSimd_V256_Add_I32(V256 a, V256 b) noexcept;
V256 ChaosSimd_V256_Sub_I32(V256 a, V256 b) noexcept;
V256 ChaosSimd_V256_Mul_I32(V256 a, V256 b) noexcept;
V256 ChaosSimd_V256_And(V256 a, V256 b) noexcept;
V256 ChaosSimd_V256_Or(V256 a, V256 b) noexcept;
V256 ChaosSimd_V256_Xor(V256 a, V256 b) noexcept;
}  // extern "C"
