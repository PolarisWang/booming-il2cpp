// datetime_stubs.cpp — DateTime/TimeSpan stub implementations
#include <chaos/native_types.h>
#include <cstdio>
#include <cstring>

#if defined(_WIN32)
#include <Windows.h>
#else
#include <ctime>
#endif

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "gc_helpers.h"

// .NET DateTime ticks: 100-nanosecond intervals since 0001-01-01
static constexpr CHAOS_IL2CPP_INT64 kTicksPerMillisecond = 10000LL;
static constexpr CHAOS_IL2CPP_INT64 kTicksPerSecond = 10000000LL;
static constexpr CHAOS_IL2CPP_INT64 kTicksPerMinute = 600000000LL;
static constexpr CHAOS_IL2CPP_INT64 kTicksPerHour = 36000000000LL;
static constexpr CHAOS_IL2CPP_INT64 kTicksPerDay = 864000000000LL;

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

// ── Internal helpers ──

static constexpr int kDaysInMonth[13] = {
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static bool is_leap_year(CHAOS_IL2CPP_INT32 year) noexcept
{
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

static CHAOS_IL2CPP_INT64 date_to_ticks(CHAOS_IL2CPP_INT32 year, CHAOS_IL2CPP_INT32 month, CHAOS_IL2CPP_INT32 day,
                                         CHAOS_IL2CPP_INT32 hour, CHAOS_IL2CPP_INT32 minute, CHAOS_IL2CPP_INT32 second) noexcept
{
    int doy = day;
    for (int m = 1; m < month; ++m) doy += kDaysInMonth[m];
    if (is_leap_year(year) && month > 2) ++doy;

    CHAOS_IL2CPP_INT64 y = static_cast<CHAOS_IL2CPP_INT64>(year - 1);
    CHAOS_IL2CPP_INT64 total_days = y * 365 + y / 4 - y / 100 + y / 400 + (doy - 1);

    return total_days * kTicksPerDay
         + static_cast<CHAOS_IL2CPP_INT64>(hour) * kTicksPerHour
         + static_cast<CHAOS_IL2CPP_INT64>(minute) * kTicksPerMinute
         + static_cast<CHAOS_IL2CPP_INT64>(second) * kTicksPerSecond;
}

// ── DateTime real implementations ──
CHAOS_IL2CPP_INT64 ChaosDateTimeAddDays(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    return dt + static_cast<CHAOS_IL2CPP_INT64>(value * static_cast<CHAOS_IL2CPP_FLOAT64>(kTicksPerDay));
}

CHAOS_IL2CPP_INT64 ChaosDateTimeAddHours(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    return dt + static_cast<CHAOS_IL2CPP_INT64>(value * static_cast<CHAOS_IL2CPP_FLOAT64>(kTicksPerHour));
}

CHAOS_IL2CPP_INT64 ChaosDateTimeAddMinutes(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    return dt + static_cast<CHAOS_IL2CPP_INT64>(value * static_cast<CHAOS_IL2CPP_FLOAT64>(kTicksPerMinute));
}

CHAOS_IL2CPP_INT32 ChaosDateTimeCompare(CHAOS_IL2CPP_INT64 left, CHAOS_IL2CPP_INT64 right) noexcept
{
    if (left < right) return -1;
    if (left > right) return 1;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosDateTimeDaysInMonth(CHAOS_IL2CPP_INT32 year, CHAOS_IL2CPP_INT32 month) noexcept
{
    if (month < 1 || month > 12) return 0;
    if (month == 2 && is_leap_year(year)) return 29;
    return kDaysInMonth[month];
}

// ── TimeSpan real implementations ──
CHAOS_IL2CPP_INT64 ChaosTimeSpanFromDays(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT64>(value * static_cast<CHAOS_IL2CPP_FLOAT64>(kTicksPerDay));
}

CHAOS_IL2CPP_INT64 ChaosTimeSpanFromHours(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT64>(value * static_cast<CHAOS_IL2CPP_FLOAT64>(kTicksPerHour));
}

CHAOS_IL2CPP_INT64 ChaosTimeSpanFromMinutes(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT64>(value * static_cast<CHAOS_IL2CPP_FLOAT64>(kTicksPerMinute));
}

// ── DateTime/TimeSpan ctor real implementations ──
void ChaosDateTimeCtor3(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INT32 year, CHAOS_IL2CPP_INT32 month, CHAOS_IL2CPP_INT32 day) noexcept
{
    if (instance == 0) return;
    CHAOS_IL2CPP_INT64 ticks = date_to_ticks(year, month, day, 0, 0, 0);
    std::memcpy(reinterpret_cast<void*>(instance), &ticks, sizeof(ticks));
}

void ChaosDateTimeCtor6(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INT32 year, CHAOS_IL2CPP_INT32 month, CHAOS_IL2CPP_INT32 day,
                         CHAOS_IL2CPP_INT32 hour, CHAOS_IL2CPP_INT32 minute, CHAOS_IL2CPP_INT32 second) noexcept
{
    if (instance == 0) return;
    CHAOS_IL2CPP_INT64 ticks = date_to_ticks(year, month, day, hour, minute, second);
    std::memcpy(reinterpret_cast<void*>(instance), &ticks, sizeof(ticks));
}

void ChaosTimeSpanCtor(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INT32 hours, CHAOS_IL2CPP_INT32 minutes, CHAOS_IL2CPP_INT32 seconds) noexcept
{
    if (instance == 0) return;
    CHAOS_IL2CPP_INT64 ticks = static_cast<CHAOS_IL2CPP_INT64>(hours) * kTicksPerHour
                             + static_cast<CHAOS_IL2CPP_INT64>(minutes) * kTicksPerMinute
                             + static_cast<CHAOS_IL2CPP_INT64>(seconds) * kTicksPerSecond;
    std::memcpy(reinterpret_cast<void*>(instance), &ticks, sizeof(ticks));
}

// ── TimeSpanParse (format: "d.HH:mm:ss" or "HH:mm:ss") ──
CHAOS_IL2CPP_INT64 ChaosTimeSpanParse(CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (value == 0) return 0;
    const char* s = stub_string_data(reinterpret_cast<const void*>(value));
    if (s == nullptr) return 0;

    int parts[4] = {0, 0, 0, 0}; // days, hours, minutes, seconds
    int pi = 0;
    bool negative = false;

    // Skip leading whitespace
    while (*s == ' ' || *s == '\t') ++s;
    if (*s == '-') { negative = true; ++s; }
    if (*s == '+') ++s;

    // Parse up to 4 colon-separated components
    while (*s && pi < 4) {
        // Parse digits
        int val = 0;
        while (*s >= '0' && *s <= '9') {
            val = val * 10 + (*s - '0');
            ++s;
        }
        parts[pi++] = val;
        if (*s == ':') { ++s; continue; }
        break;
    }

    // Skip trailing whitespace
    while (*s == ' ' || *s == '\t') ++s;
    if (*s != '\0') return 0; // garbage after parse

    CHAOS_IL2CPP_INT64 ticks;
    if (pi == 4) {
        // d.HH:mm:ss
        ticks = static_cast<CHAOS_IL2CPP_INT64>(parts[0]) * kTicksPerDay
              + static_cast<CHAOS_IL2CPP_INT64>(parts[1]) * kTicksPerHour
              + static_cast<CHAOS_IL2CPP_INT64>(parts[2]) * kTicksPerMinute
              + static_cast<CHAOS_IL2CPP_INT64>(parts[3]) * kTicksPerSecond;
    } else if (pi >= 2) {
        // HH:mm or HH:mm:ss (index 0 = hours, 1 = minutes, 2 = seconds)
        ticks = static_cast<CHAOS_IL2CPP_INT64>(parts[0]) * kTicksPerHour
              + static_cast<CHAOS_IL2CPP_INT64>(parts[1]) * kTicksPerMinute
              + (pi >= 3 ? static_cast<CHAOS_IL2CPP_INT64>(parts[2]) * kTicksPerSecond : 0);
    } else {
        return 0;
    }

    return negative ? -ticks : ticks;
}

// ── DateTimeParse (reverse of ToString format: "yyyy-MM-dd HH:mm:ss") ──
// Returns ticks as Int64 (DateTime ABI carrier is Int64).
CHAOS_IL2CPP_INT64 ChaosDateTimeParse(CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (value == 0) return 0;
    const char* s = stub_string_data(reinterpret_cast<const void*>(value));
    if (s == nullptr) return 0;

    // Skip leading whitespace
    while (*s == ' ' || *s == '\t') ++s;

    // Expected: "yyyy-MM-dd" or "yyyy-MM-ddTHH:mm:ss" or "yyyy-MM-dd HH:mm:ss"
    int y = 0, M = 0, d = 0, h = 0, m = 0, sec = 0;

    // Year (4 digits)
    for (int i = 0; i < 4; ++i) {
        if (*s >= '0' && *s <= '9') { y = y * 10 + (*s - '0'); ++s; }
        else return 0;
    }
    if (*s != '-') return 0; ++s;
    // Month
    while (*s >= '0' && *s <= '9') { M = M * 10 + (*s - '0'); ++s; }
    if (M < 1 || M > 12) return 0;
    if (*s != '-') return 0; ++s;
    // Day
    while (*s >= '0' && *s <= '9') { d = d * 10 + (*s - '0'); ++s; }
    if (d < 1 || d > 31) return 0;

    // Check for time portion: 'T' or ' ' separator
    if (*s == 'T' || *s == ' ') {
        ++s;
        // Hour
        while (*s >= '0' && *s <= '9') { h = h * 10 + (*s - '0'); ++s; }
        if (*s != ':') return 0; ++s;
        // Minute
        while (*s >= '0' && *s <= '9') { m = m * 10 + (*s - '0'); ++s; }
        if (*s == ':') {
            ++s;
            while (*s >= '0' && *s <= '9') { sec = sec * 10 + (*s - '0'); ++s; }
        }
    }

    // Skip trailing whitespace
    while (*s == ' ' || *s == '\t') ++s;
    if (*s != '\0') return 0; // garbage

    return date_to_ticks(y, M, d, h, m, sec);
}

// ── Internal helper: ticks → date components ──

static void ticks_to_date(CHAOS_IL2CPP_INT64 ticks,
                           CHAOS_IL2CPP_INT32* year, CHAOS_IL2CPP_INT32* month, CHAOS_IL2CPP_INT32* day,
                           CHAOS_IL2CPP_INT32* hour, CHAOS_IL2CPP_INT32* minute, CHAOS_IL2CPP_INT32* second) noexcept
{
    // Extract time-of-day from ticks
    CHAOS_IL2CPP_INT64 time_of_day = ticks % kTicksPerDay;
    if (time_of_day < 0) time_of_day += kTicksPerDay;
    *hour   = static_cast<CHAOS_IL2CPP_INT32>(time_of_day / kTicksPerHour);
    *minute = static_cast<CHAOS_IL2CPP_INT32>((time_of_day % kTicksPerHour) / kTicksPerMinute);
    *second = static_cast<CHAOS_IL2CPP_INT32>((time_of_day % kTicksPerMinute) / kTicksPerSecond);

    // Days since 0001-01-01
    CHAOS_IL2CPP_INT64 days = ticks / kTicksPerDay;
    if (ticks < 0) --days; // floor division for negative

    // Algorithm: year from days, then month/day
    CHAOS_IL2CPP_INT64 y = days / 365;
    CHAOS_IL2CPP_INT64 remaining = days - (y * 365 + y / 4 - y / 100 + y / 400);
    while (remaining < 0) { --y; remaining = days - (y * 365 + y / 4 - y / 100 + y / 400); }
    while (true) {
        CHAOS_IL2CPP_INT64 next_y = y + 1;
        CHAOS_IL2CPP_INT64 next_days = next_y * 365 + next_y / 4 - next_y / 100 + next_y / 400;
        if (days >= next_days) { y = next_y; continue; }
        break;
    }

    CHAOS_IL2CPP_INT32 y_int = static_cast<CHAOS_IL2CPP_INT32>(y) + 1; // year is 1-based
    CHAOS_IL2CPP_INT64 day_of_year = remaining; // 0-based

    // Month lookup
    bool leap = is_leap_year(y_int);
    int doy = static_cast<int>(day_of_year);
    int m;
    for (m = 1; m <= 12; ++m) {
        int dim = kDaysInMonth[m];
        if (m == 2 && leap) dim = 29;
        if (doy < dim) break;
        doy -= dim;
    }

    *year = y_int;
    *month = m;
    *day = doy + 1; // convert to 1-based
}

// ── DateTime ToString implementations ──
CHAOS_IL2CPP_INTPTR ChaosDateTimeToString(CHAOS_IL2CPP_INT64 dt) noexcept
{
    // Default format: "yyyy-MM-dd HH:mm:ss"
    CHAOS_IL2CPP_INT32 y, M, d, h, m, s;
    ticks_to_date(dt, &y, &M, &d, &h, &m, &s);

    char buf[64];
    int len = std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
                            static_cast<int>(y), static_cast<int>(M), static_cast<int>(d),
                            static_cast<int>(h), static_cast<int>(m), static_cast<int>(s));
    if (len < 0) return 0;

    auto clen = static_cast<CHAOS_IL2CPP_UINTPTR>(len);
    auto* result = static_cast<StubStringHeader*>(
        GcAllocateAtomic(sizeof(StubStringHeader) + clen + 1));
    if (result == nullptr) return 0;
    result->type = 0;
    result->byte_count = clen;
    std::memcpy(result + 1, buf, clen);
    reinterpret_cast<char*>(result + 1)[clen] = '\0';
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

CHAOS_IL2CPP_INTPTR ChaosDateTimeToStringFormat(CHAOS_IL2CPP_INT64 dt, CHAOS_IL2CPP_INTPTR format) noexcept
{
    CHAOS_IL2CPP_INT32 y, M, d, h, m, s;
    ticks_to_date(dt, &y, &M, &d, &h, &m, &s);

    // Parse format string (ASCII-only)
    const char* fmt = nullptr;
    CHAOS_IL2CPP_UINTPTR fmt_len = 0;
    char fmt_buf[2] = {0};
    if (format != 0) {
        auto* fhdr = reinterpret_cast<const StubStringHeader*>(format);
        fmt = stub_string_data(reinterpret_cast<const void*>(format));
        fmt_len = fhdr->byte_count;
    }

    // Determine format pattern
    char pattern = 'G'; // default general
    if (fmt != nullptr && fmt_len > 0) {
        // Single-char format specifier
        char c = fmt[0];
        if (c == 'd' || c == 'D' || c == 't' || c == 'T' || c == 's' || c == 'o' || c == 'O' || c == 'g' || c == 'G')
            pattern = c;
    }

    char buf[64];
    int len = -1;

    switch (pattern) {
    case 'd': // short date: yyyy-MM-dd
        len = std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d",
                            static_cast<int>(y), static_cast<int>(M), static_cast<int>(d));
        break;
    case 't': // short time: HH:mm:ss
    case 'T': // long time
        len = std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d",
                            static_cast<int>(h), static_cast<int>(m), static_cast<int>(s));
        break;
    case 's': // sortable: yyyy-MM-ddTHH:mm:ss
        len = std::snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d",
                            static_cast<int>(y), static_cast<int>(M), static_cast<int>(d),
                            static_cast<int>(h), static_cast<int>(m), static_cast<int>(s));
        break;
    case 'o': case 'O': // round-trip
        len = std::snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d.0000000",
                            static_cast<int>(y), static_cast<int>(M), static_cast<int>(d),
                            static_cast<int>(h), static_cast<int>(m), static_cast<int>(s));
        break;
    case 'g': // general short
        len = std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
                            static_cast<int>(y), static_cast<int>(M), static_cast<int>(d),
                            static_cast<int>(h), static_cast<int>(m), static_cast<int>(s));
        break;
    default: // 'G' or unknown — general long
        len = std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
                            static_cast<int>(y), static_cast<int>(M), static_cast<int>(d),
                            static_cast<int>(h), static_cast<int>(m), static_cast<int>(s));
        break;
    }

    if (len < 0) return 0;

    auto clen = static_cast<CHAOS_IL2CPP_UINTPTR>(len);
    auto* result = static_cast<StubStringHeader*>(
        GcAllocateAtomic(sizeof(StubStringHeader) + clen + 1));
    if (result == nullptr) return 0;
    result->type = 0;
    result->byte_count = clen;
    std::memcpy(result + 1, buf, clen);
    reinterpret_cast<char*>(result + 1)[clen] = '\0';
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core