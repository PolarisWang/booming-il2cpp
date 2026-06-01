// pal_time_macos.cpp — macOS clock_gettime (with getrusage fallback)

#include <chaos/pal/pal_time.h>

#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <errno.h>

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
    // macOS 10.12+ supports CLOCK_PROCESS_CPUTIME_ID via clock_gettime.
    struct timespec ts;
    if (::clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) == 0) {
        return static_cast<uint64_t>(ts.tv_sec) * 1'000'000'000ULL
             + static_cast<uint64_t>(ts.tv_nsec);
    }
    // Fallback: getrusage (may under-report on older macOS).
    struct rusage usage;
    if (::getrusage(RUSAGE_SELF, &usage) == 0) {
        uint64_t utime = static_cast<uint64_t>(usage.ru_utime.tv_sec) * 1'000'000'000ULL
                       + static_cast<uint64_t>(usage.ru_utime.tv_usec) * 1000ULL;
        uint64_t stime = static_cast<uint64_t>(usage.ru_stime.tv_sec) * 1'000'000'000ULL
                       + static_cast<uint64_t>(usage.ru_stime.tv_usec) * 1000ULL;
        return utime + stime;
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
