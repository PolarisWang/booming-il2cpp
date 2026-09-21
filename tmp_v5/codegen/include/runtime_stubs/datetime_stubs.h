// ── DateTime / TimeSpan stub declarations ─────────────────────
#pragma once

CHAOS_IL2CPP_INT64 ChaosDatetimeGetUtcNow(void) noexcept;
CHAOS_IL2CPP_INT32 ChaosDatetimeGetHashCode(CHAOS_IL2CPP_INTPTR datetime) noexcept;

CHAOS_IL2CPP_INTPTR ChaosDateTimeToString(CHAOS_IL2CPP_INT64 dt) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDateTimeToStringFormat(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_INTPTR format) noexcept;
CHAOS_IL2CPP_INT64 ChaosDateTimeAddDays(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_INT64 ChaosDateTimeAddHours(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_INT64 ChaosDateTimeAddMinutes(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_INT32 ChaosDateTimeCompare(CHAOS_IL2CPP_INT64 left, CHAOS_IL2CPP_INT64 right) noexcept;
CHAOS_IL2CPP_INT32 ChaosDateTimeDaysInMonth(CHAOS_IL2CPP_INT32 year, CHAOS_IL2CPP_INT32 month) noexcept;

CHAOS_IL2CPP_INT64 ChaosTimeSpanFromDays(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_INT64 ChaosTimeSpanFromHours(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_INT64 ChaosTimeSpanFromMinutes(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_INT64 ChaosTimeSpanParse(CHAOS_IL2CPP_INTPTR value) noexcept;

void ChaosDateTimeCtor3(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INT32 year, CHAOS_IL2CPP_INT32 month, CHAOS_IL2CPP_INT32 day) noexcept;
void ChaosDateTimeCtor6(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INT32 year, CHAOS_IL2CPP_INT32 month, CHAOS_IL2CPP_INT32 day, CHAOS_IL2CPP_INT32 hour, CHAOS_IL2CPP_INT32 minute, CHAOS_IL2CPP_INT32 second) noexcept;
CHAOS_IL2CPP_INT64 ChaosDateTimeParse(CHAOS_IL2CPP_INTPTR value) noexcept;
void ChaosTimeSpanCtor(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INT32 hours, CHAOS_IL2CPP_INT32 minutes, CHAOS_IL2CPP_INT32 seconds) noexcept;