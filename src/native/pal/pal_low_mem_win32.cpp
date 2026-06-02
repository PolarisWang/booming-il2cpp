// pal_low_mem_win32.cpp — Windows low-memory notification via
// CreateMemoryResourceNotification + WaitForMultipleObjects.

#include <chaos/pal/pal_low_mem.h>

#define NOMINMAX
#include <windows.h>

namespace chaos::il2cpp::pal {

struct PalLowMemMonitor {
    HANDLE low_mem_handle;   // Memory resource notification handle
    HANDLE shutdown_event;   // Manual-reset event for shutdown signaling
};

PalLowMemMonitor* PalLowMemCreate() noexcept {
    HANDLE h = ::CreateMemoryResourceNotification(LowMemoryResourceNotification);
    if (!h || h == INVALID_HANDLE_VALUE) return nullptr;

    HANDLE hShutdown = ::CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!hShutdown) {
        ::CloseHandle(h);
        return nullptr;
    }

    auto* monitor = new PalLowMemMonitor{h, hShutdown};
    return monitor;
}

int PalLowMemWait(PalLowMemMonitor* monitor, int /*timeout_ms*/) noexcept {
    HANDLE handles[2] = {monitor->low_mem_handle, monitor->shutdown_event};
    DWORD result = ::WaitForMultipleObjects(2, handles, FALSE, INFINITE);

    if (result == WAIT_OBJECT_0 || result == WAIT_OBJECT_0 + 1) return 1;  // Woken (low-mem or shutdown)
    return -1;                                                               // Error
}

void PalLowMemSignalShutdown(PalLowMemMonitor* monitor) noexcept {
    ::SetEvent(monitor->shutdown_event);
}

void PalLowMemDestroy(PalLowMemMonitor* monitor) noexcept {
    if (!monitor) return;
    ::CloseHandle(monitor->low_mem_handle);
    ::CloseHandle(monitor->shutdown_event);
    delete monitor;
}

}  // namespace chaos::il2cpp::pal
