// ── SIMD stubs declarations ─────────────────────────────────────
// ABI exports: extern "C" linkage for managed/NativeAOT callability.
// Uses SSE/AVX hardware intrinsics (x86-64) for Vector128/256
// operations that were previously lane-by-lane scalar fallbacks.
#pragma once

#include "numerics_carriers.h"

using ChaosSimdV128 = chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector128Carrier;
using ChaosSimdV256 = chaos::il2cpp::numerics_carriers::RuntimeIntrinsicVector256Carrier;

extern "C" {

// ── Vector128 arithmetic ──
ChaosSimdV128 ChaosSimd_V128_Add_I32(ChaosSimdV128 a, ChaosSimdV128 b) noexcept;
ChaosSimdV128 ChaosSimd_V128_Sub_I32(ChaosSimdV128 a, ChaosSimdV128 b) noexcept;
ChaosSimdV128 ChaosSimd_V128_Mul_I32(ChaosSimdV128 a, ChaosSimdV128 b) noexcept;
ChaosSimdV128 ChaosSimd_V128_Add_F32(ChaosSimdV128 a, ChaosSimdV128 b) noexcept;
ChaosSimdV128 ChaosSimd_V128_Sub_F32(ChaosSimdV128 a, ChaosSimdV128 b) noexcept;
ChaosSimdV128 ChaosSimd_V128_Mul_F32(ChaosSimdV128 a, ChaosSimdV128 b) noexcept;
ChaosSimdV128 ChaosSimd_V128_Add_F64(ChaosSimdV128 a, ChaosSimdV128 b) noexcept;
ChaosSimdV128 ChaosSimd_V128_Sub_F64(ChaosSimdV128 a, ChaosSimdV128 b) noexcept;
ChaosSimdV128 ChaosSimd_V128_Mul_F64(ChaosSimdV128 a, ChaosSimdV128 b) noexcept;

// ── Vector128 bitwise (type-agnostic) ──
ChaosSimdV128 ChaosSimd_V128_And(ChaosSimdV128 a, ChaosSimdV128 b) noexcept;
ChaosSimdV128 ChaosSimd_V128_Or(ChaosSimdV128 a, ChaosSimdV128 b) noexcept;
ChaosSimdV128 ChaosSimd_V128_Xor(ChaosSimdV128 a, ChaosSimdV128 b) noexcept;

// ── Vector128 comparison ──
ChaosSimdV128 ChaosSimd_V128_CmpEq_I32(ChaosSimdV128 a, ChaosSimdV128 b) noexcept;
ChaosSimdV128 ChaosSimd_V128_CmpEq_F32(ChaosSimdV128 a, ChaosSimdV128 b) noexcept;

// ── Vector128 shift ──
ChaosSimdV128 ChaosSimd_V128_Shl_I32(ChaosSimdV128 a, ChaosSimdV128 b) noexcept;

// ── Vector256 arithmetic ──
ChaosSimdV256 ChaosSimd_V256_Add_I32(ChaosSimdV256 a, ChaosSimdV256 b) noexcept;
ChaosSimdV256 ChaosSimd_V256_Sub_I32(ChaosSimdV256 a, ChaosSimdV256 b) noexcept;
ChaosSimdV256 ChaosSimd_V256_Mul_I32(ChaosSimdV256 a, ChaosSimdV256 b) noexcept;
ChaosSimdV256 ChaosSimd_V256_Add_F32(ChaosSimdV256 a, ChaosSimdV256 b) noexcept;
ChaosSimdV256 ChaosSimd_V256_Sub_F32(ChaosSimdV256 a, ChaosSimdV256 b) noexcept;
ChaosSimdV256 ChaosSimd_V256_Mul_F32(ChaosSimdV256 a, ChaosSimdV256 b) noexcept;
ChaosSimdV256 ChaosSimd_V256_Add_F64(ChaosSimdV256 a, ChaosSimdV256 b) noexcept;
ChaosSimdV256 ChaosSimd_V256_Sub_F64(ChaosSimdV256 a, ChaosSimdV256 b) noexcept;
ChaosSimdV256 ChaosSimd_V256_Mul_F64(ChaosSimdV256 a, ChaosSimdV256 b) noexcept;

// ── Vector256 bitwise (type-agnostic) ──
ChaosSimdV256 ChaosSimd_V256_And(ChaosSimdV256 a, ChaosSimdV256 b) noexcept;
ChaosSimdV256 ChaosSimd_V256_Or(ChaosSimdV256 a, ChaosSimdV256 b) noexcept;
ChaosSimdV256 ChaosSimd_V256_Xor(ChaosSimdV256 a, ChaosSimdV256 b) noexcept;

// ── Vector256 comparison ──
ChaosSimdV256 ChaosSimd_V256_CmpEq_I32(ChaosSimdV256 a, ChaosSimdV256 b) noexcept;
ChaosSimdV256 ChaosSimd_V256_CmpEq_F32(ChaosSimdV256 a, ChaosSimdV256 b) noexcept;

// ── Vector256 shift ──
ChaosSimdV256 ChaosSimd_V256_Shl_I32(ChaosSimdV256 a, ChaosSimdV256 b) noexcept;

}  // extern "C"
