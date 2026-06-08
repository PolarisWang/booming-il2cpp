#include <chaos/config.h>
#include <chaos/profile.h>
#include <chaos/pal/pal_time.h>
#include <chaos/pal/pal_thread.h>

#if CHAOS_IL2CPP_PROFILE_ENABLED

namespace chaos::il2cpp::common {

// Per-thread profile data — one instance per thread, zero-initialized on first
// access.  Shared across all translation units (external linkage).
thread_local ThreadProfileData g_tls_profile;

// Global thread registry: fixed-size array indexed by registration_slot.
// Threads register themselves on first PROFILE_SCOPE use via atomic fetch_add,
// making enumeration lock-free for ProfileDump/ProfileReset.
std::atomic<ThreadProfileData*> g_profile_threads[kProfileMaxThreads]{};
std::atomic<int> g_profile_thread_count{0};

// Retired thread data list: lock-free singly-linked list preserving profile
// data from exited threads so ProfileDump still sees their accumulators.
std::atomic<RetiredProfileNode*> g_retired_profile_head{nullptr};

// TSC calibration state (lazily calibrated on first ProfileDump).
double g_ns_per_cycle  = 0.0;
bool   g_profile_calibrated = false;

void CalibrateProfileTsc() noexcept {
    uint64_t tsc_start = __rdtsc();
    uint64_t start_ns = pal::PalGetWallTimeNs();
    pal::PalSleepMs(1);
    uint64_t tsc_end = __rdtsc();
    uint64_t end_ns = pal::PalGetWallTimeNs();

    uint64_t tsc_elapsed = tsc_end - tsc_start;
    uint64_t ns_elapsed = end_ns - start_ns;
    if (tsc_elapsed == 0 || ns_elapsed == 0) {
        g_ns_per_cycle = 1.0;
    } else {
        g_ns_per_cycle = static_cast<double>(ns_elapsed) / tsc_elapsed;
    }
    g_profile_calibrated = true;
}

} // namespace chaos::il2cpp::common

#endif // CHAOS_IL2CPP_PROFILE_ENABLED
