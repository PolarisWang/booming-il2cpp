// pal_time_win32.cpp — Windows GetProcessTimes / GetSystemTimeAsFileTime implementation

#include <chaos/pal/pal_time.h>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace chaos::il2cpp::pal {

uint64_t PalGetRealtimeNs() noexcept {
    FILETIME ft;
    ::GetSystemTimeAsFileTime(&ft);
    uint64_t t = (static_cast<uint64_t>(ft.dwHighDateTime) << 32)
               | static_cast<uint64_t>(ft.dwLowDateTime);
    // FILETIME is 100-ns intervals since Jan 1, 1601.
    // Convert to ns since Unix epoch (Jan 1, 1970).
    constexpr uint64_t kFileTimeToUnixEpoch = 11644473600ULL * 10000000;
    if (t >= kFileTimeToUnixEpoch) t -= kFileTimeToUnixEpoch;
    return t * 100;  // 100-ns intervals → ns
}

uint64_t PalGetProcessCpuTimeNs() noexcept {
    FILETIME create_time, exit_time, kernel_time, user_time;
    if (!::GetProcessTimes(::GetCurrentProcess(), &create_time, &exit_time,
                           &kernel_time, &user_time)) {
        return 0;
    }
    uint64_t kernel = (static_cast<uint64_t>(kernel_time.dwHighDateTime) << 32)
                    | static_cast<uint64_t>(kernel_time.dwLowDateTime);
    uint64_t user   = (static_cast<uint64_t>(user_time.dwHighDateTime) << 32)
                    | static_cast<uint64_t>(user_time.dwLowDateTime);
    // FILETIME is in 100-ns intervals; convert to ns.
    return (kernel + user) * 100;
}

uint64_t PalGetWallTimeNs() noexcept {
    FILETIME ft;
    ::GetSystemTimeAsFileTime(&ft);
    uint64_t t = (static_cast<uint64_t>(ft.dwHighDateTime) << 32)
               | static_cast<uint64_t>(ft.dwLowDateTime);
    // FILETIME epoch: Jan 1, 1601.  Convert to Unix epoch offset.
    constexpr uint64_t kFileTimeToUnixEpoch = 11644473600ULL * 10000000;
    if (t >= kFileTimeToUnixEpoch) t -= kFileTimeToUnixEpoch;
    return t * 100;  // 100-ns intervals → ns
}

}  // namespace chaos::il2cpp::pal
