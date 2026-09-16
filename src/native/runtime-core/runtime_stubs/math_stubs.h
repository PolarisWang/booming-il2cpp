// ── Math stub declarations ─────────────────────────────────────
#pragma once

CHAOS_IL2CPP_INT32 ChaosMathAbsInt32(CHAOS_IL2CPP_INT32 value) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathAbsDouble(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathCeiling(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathFloor(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathRound(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathRound2(CHAOS_IL2CPP_FLOAT64 value, CHAOS_IL2CPP_INT32 digits) noexcept;
CHAOS_IL2CPP_INT32 ChaosMathMaxInt32(CHAOS_IL2CPP_INT32 a, CHAOS_IL2CPP_INT32 b) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathMaxDouble(CHAOS_IL2CPP_FLOAT64 a, CHAOS_IL2CPP_FLOAT64 b) noexcept;
CHAOS_IL2CPP_INT32 ChaosMathMinInt32(CHAOS_IL2CPP_INT32 a, CHAOS_IL2CPP_INT32 b) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathMinDouble(CHAOS_IL2CPP_FLOAT64 a, CHAOS_IL2CPP_FLOAT64 b) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathPow(CHAOS_IL2CPP_FLOAT64 x, CHAOS_IL2CPP_FLOAT64 y) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathSin(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathCos(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_INT64 ChaosMathBigMul(CHAOS_IL2CPP_INT32 a, CHAOS_IL2CPP_INT32 b) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathSqrt(CHAOS_IL2CPP_FLOAT64 value) noexcept;

// ── BitOperations stubs (compiler intrinsics) ─────────────────
CHAOS_IL2CPP_INT32  ChaosBitOpsPopCount32(CHAOS_IL2CPP_UINT32 value) noexcept;
CHAOS_IL2CPP_INT32  ChaosBitOpsPopCount64(CHAOS_IL2CPP_UINT64 value) noexcept;
CHAOS_IL2CPP_INT32  ChaosBitOpsLeadingZeroCount32(CHAOS_IL2CPP_UINT32 value) noexcept;
CHAOS_IL2CPP_INT32  ChaosBitOpsLeadingZeroCount64(CHAOS_IL2CPP_UINT64 value) noexcept;
CHAOS_IL2CPP_INT32  ChaosBitOpsTrailingZeroCount32(CHAOS_IL2CPP_UINT32 value) noexcept;
CHAOS_IL2CPP_INT32  ChaosBitOpsTrailingZeroCount64(CHAOS_IL2CPP_UINT64 value) noexcept;
CHAOS_IL2CPP_UINT32 ChaosBitOpsRotateLeft32(CHAOS_IL2CPP_UINT32 value, CHAOS_IL2CPP_INT32 offset) noexcept;
CHAOS_IL2CPP_UINT64 ChaosBitOpsRotateLeft64(CHAOS_IL2CPP_UINT64 value, CHAOS_IL2CPP_INT32 offset) noexcept;
CHAOS_IL2CPP_UINT32 ChaosBitOpsRotateRight32(CHAOS_IL2CPP_UINT32 value, CHAOS_IL2CPP_INT32 offset) noexcept;
CHAOS_IL2CPP_UINT64 ChaosBitOpsRotateRight64(CHAOS_IL2CPP_UINT64 value, CHAOS_IL2CPP_INT32 offset) noexcept;
CHAOS_IL2CPP_UINT32 ChaosBitOpsCrc32CByte(CHAOS_IL2CPP_UINT32 crc, CHAOS_IL2CPP_UINT8 data) noexcept;
CHAOS_IL2CPP_UINT32 ChaosBitOpsCrc32CUInt16(CHAOS_IL2CPP_UINT32 crc, CHAOS_IL2CPP_UINT16 data) noexcept;
CHAOS_IL2CPP_UINT32 ChaosBitOpsCrc32CUInt32(CHAOS_IL2CPP_UINT32 crc, CHAOS_IL2CPP_UINT32 data) noexcept;
CHAOS_IL2CPP_UINT32 ChaosBitOpsCrc32CUInt64(CHAOS_IL2CPP_UINT32 crc, CHAOS_IL2CPP_UINT64 data) noexcept;
CHAOS_IL2CPP_UINT32 ChaosBitOpsRoundUpToPowerOf232(CHAOS_IL2CPP_UINT32 value) noexcept;
CHAOS_IL2CPP_UINT64 ChaosBitOpsRoundUpToPowerOf264(CHAOS_IL2CPP_UINT64 value) noexcept;
CHAOS_IL2CPP_INT32  ChaosBitOpsIsPow232(CHAOS_IL2CPP_UINT32 value) noexcept;
CHAOS_IL2CPP_INT32  ChaosBitOpsIsPow264(CHAOS_IL2CPP_UINT64 value) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathExp(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathLog(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathLog10(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathTan(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathAtan(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathAtan2(CHAOS_IL2CPP_FLOAT64 y, CHAOS_IL2CPP_FLOAT64 x) noexcept;


CHAOS_IL2CPP_FLOAT64 ChaosMathTruncate(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathRound2Params(CHAOS_IL2CPP_FLOAT64 x, CHAOS_IL2CPP_INT32 digits) noexcept;
CHAOS_IL2CPP_INT32 ChaosMathSignDouble(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_INT32 ChaosMathSignInt32(CHAOS_IL2CPP_INT32 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathAcos(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathAsin(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathCosh(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathSinh(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathTanh(CHAOS_IL2CPP_FLOAT64 x) noexcept;

// ── Math/Double IEEE-754 special functions (real extern C wrappers) ──
// These give the header's inline math_kernel_helpers implementations C
// linkage so codegen can forward to them.  Before these existed the subjects
// (System.Double::BitDecrement etc.) fell through to the zero-argument
// ChaosExternalRuntimeFallback catch-all which has no operand access and
// returned 0 — fabricating false-green comparisons.
CHAOS_IL2CPP_FLOAT64 ChaosMathBitDecrement(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathBitIncrement(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathCopySign(CHAOS_IL2CPP_FLOAT64 x, CHAOS_IL2CPP_FLOAT64 y) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathScaleB(CHAOS_IL2CPP_FLOAT64 x, CHAOS_IL2CPP_INT32 n) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathMaxMagnitude(CHAOS_IL2CPP_FLOAT64 x, CHAOS_IL2CPP_FLOAT64 y) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathMinMagnitude(CHAOS_IL2CPP_FLOAT64 x, CHAOS_IL2CPP_FLOAT64 y) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathIEEERemainder(CHAOS_IL2CPP_FLOAT64 x, CHAOS_IL2CPP_FLOAT64 y) noexcept;
CHAOS_IL2CPP_INT32    ChaosMathILogB(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathReciprocalEstimate(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathReciprocalSqrtEstimate(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathAcosh(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathAsinh(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathAtanh(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathCbrt(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathHypot(CHAOS_IL2CPP_FLOAT64 x, CHAOS_IL2CPP_FLOAT64 y) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathExp2(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathExpM1(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathLog2(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathLog1p(CHAOS_IL2CPP_FLOAT64 x) noexcept;

// ── Single (float) counterparts — System.Single::/System.MathF:: ─────
// Same special functions at float width; the F suffix denotes the overload.
CHAOS_IL2CPP_FLOAT32 ChaosMathBitDecrementF(CHAOS_IL2CPP_FLOAT32 value) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathBitIncrementF(CHAOS_IL2CPP_FLOAT32 value) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathCopySignF(CHAOS_IL2CPP_FLOAT32 x, CHAOS_IL2CPP_FLOAT32 y) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathScaleBF(CHAOS_IL2CPP_FLOAT32 x, CHAOS_IL2CPP_INT32 n) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathMaxMagnitudeF(CHAOS_IL2CPP_FLOAT32 x, CHAOS_IL2CPP_FLOAT32 y) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathMinMagnitudeF(CHAOS_IL2CPP_FLOAT32 x, CHAOS_IL2CPP_FLOAT32 y) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathIEEERemainderF(CHAOS_IL2CPP_FLOAT32 x, CHAOS_IL2CPP_FLOAT32 y) noexcept;
CHAOS_IL2CPP_INT32    ChaosMathILogBF(CHAOS_IL2CPP_FLOAT32 value) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathReciprocalEstimateF(CHAOS_IL2CPP_FLOAT32 value) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathReciprocalSqrtEstimateF(CHAOS_IL2CPP_FLOAT32 value) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathAcoshF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathAsinhF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathAtanhF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathCbrtF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathHypotF(CHAOS_IL2CPP_FLOAT32 x, CHAOS_IL2CPP_FLOAT32 y) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathExp2F(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathExpM1F(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathLog2F(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathLog1pF(CHAOS_IL2CPP_FLOAT32 x) noexcept;

// ── Pi-scaled trig variants (System.Double::/System.Single::*Pi) ──
CHAOS_IL2CPP_FLOAT64 ChaosMathAcosPi(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathAsinPi(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathAtanPi(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathAtan2Pi(CHAOS_IL2CPP_FLOAT64 y, CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathCosPi(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathSinPi(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathTanPi(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathAcosPiF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathAsinPiF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathAtanPiF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathAtan2PiF(CHAOS_IL2CPP_FLOAT32 y, CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathCosPiF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathSinPiF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathTanPiF(CHAOS_IL2CPP_FLOAT32 x) noexcept;

// ── Classic Math/Double one-arg families — Single counterparts ───────
CHAOS_IL2CPP_FLOAT32 ChaosMathExpF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathExp10F(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathExp10M1F(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathLogF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathLog10F(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathLog10P1F(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathLog2M1F(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathCoshF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathSinhF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathTanhF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathAcosF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathAsinF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathAtanF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathAtan2F(CHAOS_IL2CPP_FLOAT32 y, CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathTanF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathSinF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathCosF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathPowF(CHAOS_IL2CPP_FLOAT32 x, CHAOS_IL2CPP_FLOAT32 y) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathSqrtF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathCeilingF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathFloorF(CHAOS_IL2CPP_FLOAT32 x) noexcept;
CHAOS_IL2CPP_FLOAT32 ChaosMathTruncateF(CHAOS_IL2CPP_FLOAT32 x) noexcept;

// ── Exp10 / Log10P1 / Log2M1 double-width (no prior native at all) ───
CHAOS_IL2CPP_FLOAT64 ChaosMathExp10(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathExp10M1(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathLog10P1(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathLog2M1(CHAOS_IL2CPP_FLOAT64 x) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosMathLogP1(CHAOS_IL2CPP_FLOAT64 x) noexcept;

// ── Math::Ceiling/Floor/Round/Truncate(System.Decimal) ─────────────
// DecimalCarrier* in, DecimalCarrier* out. Declared here (reachable by the
// generated AOT C++ that forwards via SimpleForward) for a visible symbol.
CHAOS_IL2CPP_INTPTR    ChaosMathDecimalCeiling(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept;
CHAOS_IL2CPP_INTPTR    ChaosMathDecimalFloor(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept;
CHAOS_IL2CPP_INTPTR    ChaosMathDecimalRound(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept;
CHAOS_IL2CPP_INTPTR    ChaosMathDecimalTruncate(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept;

// ── Decimal::Add/Subtract/Multiply/Divide/Remainder/Negate(Decimal) ─
// DecimalCarrier* in, DecimalCarrier* out. Declared here (reachable by the
// generated AOT C++ that forwards via SimpleForward) so the symbol is visible.
CHAOS_IL2CPP_INTPTR    ChaosDecimalAdd(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
CHAOS_IL2CPP_INTPTR    ChaosDecimalSubtract(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
CHAOS_IL2CPP_INTPTR    ChaosDecimalMultiply(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
CHAOS_IL2CPP_INTPTR    ChaosDecimalDivide(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
CHAOS_IL2CPP_INTPTR    ChaosDecimalRemainder(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
CHAOS_IL2CPP_INTPTR    ChaosDecimalNegate(CHAOS_IL2CPP_INTPTR carrier_ptr) noexcept;

// ── Decimal::CopySign/MaxMagnitude/MinMagnitude(Decimal, Decimal) ────
CHAOS_IL2CPP_INTPTR    ChaosDecimalCopySign(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
CHAOS_IL2CPP_INTPTR    ChaosDecimalMaxMagnitude(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;
CHAOS_IL2CPP_INTPTR    ChaosDecimalMinMagnitude(CHAOS_IL2CPP_INTPTR left_ptr, CHAOS_IL2CPP_INTPTR right_ptr) noexcept;


