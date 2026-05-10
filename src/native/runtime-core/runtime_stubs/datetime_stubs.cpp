// datetime_stubs.cpp — DateTime/TimeSpan stub implementations
#include <chaos/native_types.h>
#include <cstring>

#if defined(_WIN32)
#include <Windows.h>
#else
#include <ctime>
#endif

#include "generated_code_compat.h"

// Offset from .NET epoch (0001-01-01) to FILETIME epoch (1601-01-01).
static constexpr CHAOS_IL2CPP_INT64 kDotNetToFileTimeOffset = 504911232000000000LL;
// Offset from .NET epoch to Unix epoch (1970-01-01).
static constexpr CHAOS_IL2CPP_INT64 kDotNetToUnixEpochOffset = 621355968000000000LL;

namespace chaos::il2cpp::runtime_core {
extern "C" {

CHAOS_IL2CPP_INT64 ChaosDatetimeGetUtcNow(void) noexcept
{
#if defined(_WIN32)
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    CHAOS_IL2CPP_INT64 filetime = (static_cast<CHAOS_IL2CPP_INT64>(ft.dwHighDateTime) << 32)
                                | static_cast<CHAOS_IL2CPP_INT64>(ft.dwLowDateTime);
    return filetime + kDotNetToFileTimeOffset;
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    CHAOS_IL2CPP_INT64 ticks = static_cast<CHAOS_IL2CPP_INT64>(ts.tv_sec) * 10000000LL
                             + ts.tv_nsec / 100;
    return ticks + kDotNetToUnixEpochOffset;
#endif
}

CHAOS_IL2CPP_INT32 ChaosDatetimeGetHashCode(CHAOS_IL2CPP_INTPTR datetime) noexcept
{
    if (datetime == 0) return 0;
    CHAOS_IL2CPP_INT64 date_data;
    std::memcpy(&date_data, reinterpret_cast<const void*>(datetime), sizeof(date_data));
    CHAOS_IL2CPP_INT64 ticks = date_data & 0x3FFFFFFFFFFFFFFFLL;
    return static_cast<CHAOS_IL2CPP_INT32>(ticks)
         ^ static_cast<CHAOS_IL2CPP_INT32>(ticks >> 32);
}

// ── DateTime stubs ──
CHAOS_IL2CPP_INTPTR ChaosDateTimeToString(CHAOS_IL2CPP_INT64 dt) noexcept { (void)dt; return 0; }
CHAOS_IL2CPP_INTPTR ChaosDateTimeToStringFormat(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_INTPTR format) noexcept { (void)dt; (void)format; return 0; }
CHAOS_IL2CPP_INT64 ChaosDateTimeAddDays(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_FLOAT64 value) noexcept { (void)dt; (void)value; return 0; }
CHAOS_IL2CPP_INT64 ChaosDateTimeAddHours(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_FLOAT64 value) noexcept { (void)dt; (void)value; return 0; }
CHAOS_IL2CPP_INT64 ChaosDateTimeAddMinutes(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_FLOAT64 value) noexcept { (void)dt; (void)value; return 0; }
CHAOS_IL2CPP_INT32 ChaosDateTimeCompare(CHAOS_IL2CPP_INT64 left, CHAOS_IL2CPP_INT64 right) noexcept { (void)left; (void)right; return 0; }
CHAOS_IL2CPP_INT32 ChaosDateTimeDaysInMonth(CHAOS_IL2CPP_INT32 year, CHAOS_IL2CPP_INT32 month) noexcept { (void)year; (void)month; return 0; }

// ── TimeSpan stubs ──
CHAOS_IL2CPP_INT64 ChaosTimeSpanFromDays(CHAOS_IL2CPP_FLOAT64 value) noexcept { (void)value; return 0; }
CHAOS_IL2CPP_INT64 ChaosTimeSpanFromHours(CHAOS_IL2CPP_FLOAT64 value) noexcept { (void)value; return 0; }
CHAOS_IL2CPP_INT64 ChaosTimeSpanFromMinutes(CHAOS_IL2CPP_FLOAT64 value) noexcept { (void)value; return 0; }
CHAOS_IL2CPP_INT64 ChaosTimeSpanParse(CHAOS_IL2CPP_INTPTR value) noexcept { (void)value; return 0; }

// ── DateTime/TimeSpan ctor stubs ──
void ChaosDateTimeCtor3(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INT32 year, CHAOS_IL2CPP_INT32 month, CHAOS_IL2CPP_INT32 day) noexcept { (void)instance; (void)year; (void)month; (void)day; }
void ChaosDateTimeCtor6(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INT32 year, CHAOS_IL2CPP_INT32 month, CHAOS_IL2CPP_INT32 day, CHAOS_IL2CPP_INT32 hour, CHAOS_IL2CPP_INT32 minute, CHAOS_IL2CPP_INT32 second) noexcept { (void)instance; (void)year; (void)month; (void)day; (void)hour; (void)minute; (void)second; }
CHAOS_IL2CPP_INTPTR ChaosDateTimeParse(CHAOS_IL2CPP_INTPTR value) noexcept { (void)value; return 0; }
void ChaosTimeSpanCtor(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INT32 hours, CHAOS_IL2CPP_INT32 minutes, CHAOS_IL2CPP_INT32 seconds) noexcept { (void)instance; (void)hours; (void)minutes; (void)seconds; }

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core