// ── String stub declarations ───────────────────────────────────
#pragma once

extern "C" {

CHAOS_IL2CPP_INTPTR ChaosStringContains(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringJoinSs(CHAOS_IL2CPP_INTPTR separator, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringStartsWith(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringEndsWith(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringToLower(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringToUpper(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringTrim(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringReplace(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR old_value, CHAOS_IL2CPP_INTPTR new_value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringSubstring(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR start_index, CHAOS_IL2CPP_INTPTR length) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringRemove(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR start_index, CHAOS_IL2CPP_INTPTR length) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringFormatArray(CHAOS_IL2CPP_INTPTR format_str, CHAOS_IL2CPP_INTPTR args_array) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringFormat1(CHAOS_IL2CPP_INTPTR format_str, CHAOS_IL2CPP_INTPTR arg0) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringFormat2(CHAOS_IL2CPP_INTPTR format_str, CHAOS_IL2CPP_INTPTR arg0, CHAOS_IL2CPP_INTPTR arg1) noexcept;
CHAOS_IL2CPP_INT32 ChaosStringIndexOf(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32 ChaosStringCompare(CHAOS_IL2CPP_INTPTR str_a, CHAOS_IL2CPP_INTPTR str_b) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringSplit(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR separator) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringFastAllocate(CHAOS_IL2CPP_INTPTR length) noexcept;
void    ChaosStringAppend(CHAOS_IL2CPP_INTPTR builder, CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR arg) noexcept;

CHAOS_IL2CPP_INTPTR ChaosFormattablestringFactoryCreate(CHAOS_IL2CPP_INTPTR format, CHAOS_IL2CPP_INTPTR args) noexcept;

CHAOS_IL2CPP_INT32  ChaosStringGetLength(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_UINT16 ChaosStringGetChars(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INT32 index) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringIsNullOrEmpty(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringConcat2(CHAOS_IL2CPP_INTPTR a, CHAOS_IL2CPP_INTPTR b) noexcept;

}  // extern "C"