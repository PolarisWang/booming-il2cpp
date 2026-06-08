#ifndef CHAOS_IL2CPP_PAL_TIME_H_
#define CHAOS_IL2CPP_PAL_TIME_H_

// ── Time measurement ──────────────────────────────────────────────────
// Platform-independent wrappers for process CPU time and wall clock time.
// ============================================================================

#include <ctime>

#include <chaos/pal/pal_types.h>

namespace chaos::il2cpp::pal {

/// Wall clock time in nanoseconds since the Unix epoch (1970-01-01).
/// Uses CLOCK_REALTIME on POSIX, GetSystemTimeAsFileTime on Windows.
/// Unlike PalGetWallTimeNs, this is not monotonic and may jump due to
/// NTP adjustments.
uint64_t PalGetRealtimeNs() noexcept;

/// Total CPU time consumed by the process (all threads), in nanoseconds.
/// Returns 0 if the platform does not support process-level CPU time.
uint64_t PalGetProcessCpuTimeNs() noexcept;

/// Monotonic wall clock time in nanoseconds since an unspecified epoch.
/// Never goes backwards, suitable for interval measurement.
uint64_t PalGetWallTimeNs() noexcept;

/// Convert @a clock (time_t) to a thread-safe @a result (struct tm).
/// Equivalent to localtime_r on POSIX, localtime_s on Win32.
/// @param clock  Calendar time (UTC seconds since epoch).
/// @param result  Output buffer (caller-allocated).
inline void PalLocalTime(const time_t* clock, struct tm* result) noexcept {
#if defined(_WIN32)
    ::localtime_s(result, clock);
#else
    ::localtime_r(clock, result);
#endif
}

}  // namespace chaos::il2cpp::pal

#endif  // CHAOS_IL2CPP_PAL_TIME_H_
