// pal_time_stub.cpp — SDK-emitted PAL time stub for test builds.
// Provides PalGetRealtimeNs() so datetime_stubs.cpp can link
// without requiring chaos_pal.lib (not included in the SDK).
#include <chaos/pal/pal_time.h>
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
    constexpr uint64_t kFileTimeToUnixEpoch = 11644473600ULL * 10000000;
    if (t >= kFileTimeToUnixEpoch) t -= kFileTimeToUnixEpoch;
    return t * 100;
}
}
