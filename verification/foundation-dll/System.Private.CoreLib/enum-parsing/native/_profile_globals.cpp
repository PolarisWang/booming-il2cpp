#include <chaos/config.h>
#include <chaos/profile.h>

#if CHAOS_IL2CPP_PROFILE_ENABLED

#include <windows.h>

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
    LARGE_INTEGER freq;
    if (!QueryPerformanceFrequency(&freq) || freq.QuadPart == 0) {
        g_ns_per_cycle = 1.0;  // fallback: report cycles
        g_profile_calibrated = true;
        return;
    }

    // Sleep(1) gives a reliable ~1-2ms interval that both RDTSC and QPC
    // can measure with good SNR.  No busy-wait, no CPU-frequency assumption.
    LARGE_INTEGER start, end;
    uint64_t tsc_start = __rdtsc();
    QueryPerformanceCounter(&start);
    Sleep(1);
    uint64_t tsc_end = __rdtsc();
    QueryPerformanceCounter(&end);

    uint64_t tsc_elapsed = tsc_end - tsc_start;
    uint64_t qpc_elapsed = end.QuadPart - start.QuadPart;
    if (tsc_elapsed == 0 || qpc_elapsed == 0) {
        g_ns_per_cycle = 1.0;
        g_profile_calibrated = true;
        return;
    }
    double seconds = static_cast<double>(qpc_elapsed) / freq.QuadPart;
    g_ns_per_cycle = (seconds * 1e9) / tsc_elapsed;
    g_profile_calibrated = true;
}

} // namespace chaos::il2cpp::common

#endif // CHAOS_IL2CPP_PROFILE_ENABLED
