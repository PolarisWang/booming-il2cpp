// pal_time_fallback.cpp — stub for unsupported platforms

#include <chaos/pal/pal_time.h>

namespace chaos::il2cpp::pal {

uint64_t PalGetRealtimeNs() noexcept {
    return 0;  // Unsupported platform
}

uint64_t PalGetProcessCpuTimeNs() noexcept {
    return 0;  // Unsupported platform
}

uint64_t PalGetWallTimeNs() noexcept {
    return 0;  // Unsupported platform
}

}  // namespace chaos::il2cpp::pal
