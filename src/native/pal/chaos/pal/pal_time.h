#ifndef CHAOS_IL2CPP_PAL_TIME_H_
#define CHAOS_IL2CPP_PAL_TIME_H_

// ── Time measurement ──────────────────────────────────────────────────
// Platform-independent wrappers for process CPU time and wall clock time.
// ============================================================================

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

}  // namespace chaos::il2cpp::pal

#endif  // CHAOS_IL2CPP_PAL_TIME_H_
