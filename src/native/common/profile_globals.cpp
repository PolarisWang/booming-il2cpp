// profile_globals.cpp -- Definitions for profile global variables.
//
// Only compiled when CHAOS_IL2CPP_PROFILE_ENABLED=1.  The header declares
// these as extern in the enabled branch; this file provides the definitions.
// When profiling is disabled the inline definitions in profile.h suffice.

#include "chaos/profile.h"

// Guard: only define when the #if CHAOS_IL2CPP_PROFILE_ENABLED branch is active.
#if CHAOS_IL2CPP_PROFILE_ENABLED

namespace chaos::il2cpp::common {

// Defined here (not inline in the header) because thread_local variables with
// complex struct types (ProfileSlot array, hash table) have significant TLS
// overhead.  Keeping them in a single .cpp ensures only one TU pays the cost.
thread_local ThreadProfileData g_tls_profile;

// Thread registry: fixed-size lock-free array indexed by atomic counter.
std::atomic<ThreadProfileData*> g_profile_threads[kProfileMaxThreads]{};

// Atomic counter for thread registration index.
std::atomic<int> g_profile_thread_count{0};

// Retired thread data linked-list head (lock-free SList).
std::atomic<RetiredProfileNode*> g_retired_profile_head{nullptr};

// RDTSC calibration state.
double g_ns_per_cycle{0.0};
bool g_profile_calibrated{false};

} // namespace chaos::il2cpp::common

#endif // CHAOS_IL2CPP_PROFILE_ENABLED
