// ── Random stub declarations ───────────────────────────────────
#pragma once

void    ChaosRandomNextBytes(CHAOS_IL2CPP_INTPTR rng, CHAOS_IL2CPP_INTPTR buffer) noexcept;
CHAOS_IL2CPP_INT64  ChaosRandomNextDouble(CHAOS_IL2CPP_INTPTR rng) noexcept;
CHAOS_IL2CPP_INT32  ChaosRandomNext(CHAOS_IL2CPP_INTPTR rng) noexcept;
CHAOS_IL2CPP_INT32  ChaosRandomNextMax(CHAOS_IL2CPP_INTPTR rng, CHAOS_IL2CPP_INT32 maxValue) noexcept;