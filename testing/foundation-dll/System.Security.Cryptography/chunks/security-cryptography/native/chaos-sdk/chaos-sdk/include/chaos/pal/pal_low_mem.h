// pal_low_mem.h — Low-memory notification abstraction
#pragma once

#include <cstdint>

namespace chaos::il2cpp::pal {

/// Opaque handle for low-memory monitoring.
/// On Windows: wraps a Memory Resource Notification handle + shutdown event.
/// On Linux: wraps a PSI fd + shutdown pipe fds.
struct PalLowMemMonitor;

/// Create a low-memory monitor handle.
/// On Windows: calls CreateMemoryResourceNotification(LowMemoryResourceNotification).
/// On Linux: opens /proc/pressure/memory and creates a self-shutdown pipe.
/// Returns nullptr if the platform does not support low-memory monitoring.
PalLowMemMonitor* PalLowMemCreate() noexcept;

/// Wait for a low-memory signal or shutdown.
/// @returns >0 on signal (low-memory or shutdown), 0 on timeout, -1 on error.
/// The caller must check its own shutdown flag to distinguish signal vs shutdown.
int PalLowMemWait(PalLowMemMonitor* monitor, int timeout_ms) noexcept;

/// Signal shutdown to wake a waiting PalLowMemWait call.
void PalLowMemSignalShutdown(PalLowMemMonitor* monitor) noexcept;

/// Destroy the monitor and release all handles.
void PalLowMemDestroy(PalLowMemMonitor* monitor) noexcept;

}  // namespace chaos::il2cpp::pal
