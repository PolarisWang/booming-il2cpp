// ── Interlocked / Volatile stub declarations ───────────────────
#pragma once

void    ChaosInterlockedMemoryBarrier(void) noexcept;
CHAOS_IL2CPP_INT64  ChaosInterlockedReadInt64(CHAOS_IL2CPP_INTPTR location) noexcept;
CHAOS_IL2CPP_INT64  ChaosInterlockedIncrementInt64(CHAOS_IL2CPP_INTPTR location) noexcept;
CHAOS_IL2CPP_INT64  ChaosInterlockedDecrementInt64(CHAOS_IL2CPP_INTPTR location) noexcept;
CHAOS_IL2CPP_INT64  ChaosInterlockedExchangeInt64(CHAOS_IL2CPP_INTPTR location, CHAOS_IL2CPP_INT64 value) noexcept;
CHAOS_IL2CPP_INT32  ChaosInterlockedCompareExchangeInt32(CHAOS_IL2CPP_INTPTR location, CHAOS_IL2CPP_INT32 value, CHAOS_IL2CPP_INT32 comparand) noexcept;
CHAOS_IL2CPP_INT64  ChaosInterlockedCompareExchangeInt64(CHAOS_IL2CPP_INTPTR location, CHAOS_IL2CPP_INT64 value, CHAOS_IL2CPP_INT64 comparand) noexcept;
void    ChaosInterlockedStoreNoBarrier(CHAOS_IL2CPP_INTPTR location, CHAOS_IL2CPP_INT64 value) noexcept;
CHAOS_IL2CPP_INT32  ChaosVolatileRead(CHAOS_IL2CPP_INTPTR ptr) noexcept;
void    ChaosVolatileWrite(CHAOS_IL2CPP_INTPTR ptr, CHAOS_IL2CPP_INT32 value) noexcept;

// Reference-typed volatile read (`Thread.VolatileRead(ref object)`).
//
// Deliberately NOT an alias of ChaosVolatileRead: that returns INT32 and would
// truncate a 64-bit object reference.  The managed signature is `T VolatileRead(ref T)`,
// so the carrier here is pointer-sized.
CHAOS_IL2CPP_INTPTR  ChaosVolatileReadObject(CHAOS_IL2CPP_INTPTR ptr) noexcept;

// ── Int32 / UInt32 / UInt64 arithmetic ─────────────────────────
//
// These were defined in interlocked_stubs.cpp but never declared here, so no
// shape could name them and every Int32/UInt32/UInt64 overload of
// Interlocked.Increment/Decrement/Exchange fell through to the zero-argument
// external-runtime catch-all, which returns 0.
//
// The failure mode: the shape registry only ever registered the Int64 forms
// (`Interlocked::Increment:(System.Int64&)`), so a callee such as
// `Interlocked::Increment:System.Int32(System.Int32&)` matched no shape at all
// and lowered to `chaos_external_runtime_..._Increment_System_Int32_...__()`.
// Against `Assert.AreEqual(1, Interlocked.Increment(ref x))` the 0 is a
// silent wrong answer, not a crash — see the threading chunk's `failed`
// bucket.
//
// UInt32/UInt64 get their own entries rather than aliasing the signed ones:
// the signed helpers wrap at the C++ type's width, which is the same bit
// pattern here, but naming them separately keeps the shape registry readable
// and avoids implying a signedness guarantee the ABI does not carry.

CHAOS_IL2CPP_INT32   ChaosInterlockedIncrementInt32(CHAOS_IL2CPP_INTPTR location) noexcept;
CHAOS_IL2CPP_INT32   ChaosInterlockedDecrementInt32(CHAOS_IL2CPP_INTPTR location) noexcept;
CHAOS_IL2CPP_INT32   ChaosInterlockedExchangeInt32(CHAOS_IL2CPP_INTPTR location, CHAOS_IL2CPP_INT32 value) noexcept;
CHAOS_IL2CPP_INT32   ChaosInterlockedAddInt32(CHAOS_IL2CPP_INTPTR location, CHAOS_IL2CPP_INT32 value) noexcept;
CHAOS_IL2CPP_INT64   ChaosInterlockedAddInt64(CHAOS_IL2CPP_INTPTR location, CHAOS_IL2CPP_INT64 value) noexcept;
CHAOS_IL2CPP_UINT32  ChaosInterlockedOrUInt32(CHAOS_IL2CPP_INTPTR location, CHAOS_IL2CPP_UINT32 value) noexcept;

CHAOS_IL2CPP_UINT32  ChaosInterlockedIncrementUInt32(CHAOS_IL2CPP_INTPTR location) noexcept;
CHAOS_IL2CPP_UINT32  ChaosInterlockedDecrementUInt32(CHAOS_IL2CPP_INTPTR location) noexcept;
CHAOS_IL2CPP_UINT64  ChaosInterlockedIncrementUInt64(CHAOS_IL2CPP_INTPTR location) noexcept;
CHAOS_IL2CPP_UINT64  ChaosInterlockedDecrementUInt64(CHAOS_IL2CPP_INTPTR location) noexcept;