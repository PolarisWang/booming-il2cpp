// ── Guid stub declarations ─────────────────────────────────────
#pragma once

CHAOS_IL2CPP_INTPTR ChaosGuidNewGuid(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosGuidParse(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32  ChaosGuidGetHashCode(CHAOS_IL2CPP_INTPTR guid) noexcept;
CHAOS_IL2CPP_INTPTR ChaosGuidToString(CHAOS_IL2CPP_INTPTR guid) noexcept;