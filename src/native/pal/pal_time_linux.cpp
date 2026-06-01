// pal_time_linux.cpp — Linux clock_gettime implementation

#include <chaos/pal/pal_time.h>

#include <time.h>

namespace chaos::il2cpp::pal {

uint64_t PalGetRealtimeNs() noexcept {
    struct timespec ts;
    if (::clock_gettime(CLOCK_REALTIME, &ts) == 0) {
        return static_cast<uint64_t>(ts.tv_sec) * 1'000'000'000ULL
             + static_cast<uint64_t>(ts.tv_nsec);
    }
    return 0;
}

uint64_t PalGetProcessCpuTimeNs() noexcept {
    struct timespec ts;
    if (::clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) == 0) {
        return static_cast<uint64_t>(ts.tv_sec) * 1'000'000'000ULL
             + static_cast<uint64_t>(ts.tv_nsec);
    }
    return 0;
}

uint64_t PalGetWallTimeNs() noexcept {
    struct timespec ts;
    if (::clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        return static_cast<uint64_t>(ts.tv_sec) * 1'000'000'000ULL
             + static_cast<uint64_t>(ts.tv_nsec);
    }
    return 0;
}

}  // namespace chaos::il2cpp::pal
