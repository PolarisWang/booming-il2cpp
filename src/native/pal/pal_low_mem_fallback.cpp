// pal_low_mem_fallback.cpp — Fallback no-op low-memory monitor
// Used on platforms without low-memory notification support.

#include <chaos/pal/pal_low_mem.h>

namespace chaos::il2cpp::pal {

PalLowMemMonitor* PalLowMemCreate() noexcept {
    return nullptr;
}

int PalLowMemWait(PalLowMemMonitor* /*monitor*/, int /*timeout_ms*/) noexcept {
    return -1;
}

void PalLowMemSignalShutdown(PalLowMemMonitor* /*monitor*/) noexcept {
}

void PalLowMemDestroy(PalLowMemMonitor* /*monitor*/) noexcept {
}

}  // namespace chaos::il2cpp::pal
