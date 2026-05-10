// ── Interlocked / Volatile stub declarations ───────────────────
#pragma once

void    ChaosInterlockedMemoryBarrier(void) noexcept;
CHAOS_IL2CPP_INT64  ChaosInterlockedReadInt64(CHAOS_IL2CPP_INT64* location) noexcept;
CHAOS_IL2CPP_INT64  ChaosInterlockedIncrementInt64(CHAOS_IL2CPP_INT64* location) noexcept;
CHAOS_IL2CPP_INT64  ChaosInterlockedDecrementInt64(CHAOS_IL2CPP_INT64* location) noexcept;
CHAOS_IL2CPP_INT64  ChaosInterlockedExchangeInt64(CHAOS_IL2CPP_INT64* location, CHAOS_IL2CPP_INT64 value) noexcept;
CHAOS_IL2CPP_INT32  ChaosInterlockedCompareExchangeInt32(CHAOS_IL2CPP_INT32* location, CHAOS_IL2CPP_INT32 value, CHAOS_IL2CPP_INT32 comparand) noexcept;
CHAOS_IL2CPP_INT64  ChaosInterlockedCompareExchangeInt64(CHAOS_IL2CPP_INT64* location, CHAOS_IL2CPP_INT64 value, CHAOS_IL2CPP_INT64 comparand) noexcept;
void    ChaosInterlockedStoreNoBarrier(CHAOS_IL2CPP_INT64* location, CHAOS_IL2CPP_INT64 value) noexcept;
CHAOS_IL2CPP_INT32  ChaosVolatileRead(CHAOS_IL2CPP_INTPTR ptr) noexcept;