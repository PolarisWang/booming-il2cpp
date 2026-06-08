// ── Char stub declarations ────────────────────────────────────
#pragma once

CHAOS_IL2CPP_INT32 ChaosCharIsDigit(CHAOS_IL2CPP_INT32 c) noexcept;
CHAOS_IL2CPP_INT32 ChaosCharIsLetter(CHAOS_IL2CPP_INT32 c) noexcept;
CHAOS_IL2CPP_INT32 ChaosCharIsWhiteSpace(CHAOS_IL2CPP_INT32 c) noexcept;

// CharUnicodeInfo helpers with managed string + index (delegate to char version)
CHAOS_IL2CPP_INT32 ChaosCharUnicodeInfoGetDigitValueString(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 index) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosCharUnicodeInfoGetNumericValueString(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 index) noexcept;