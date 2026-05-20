#ifndef CHAOS_IL2CPP_GC_STRESS_H_
#define CHAOS_IL2CPP_GC_STRESS_H_

/// @file gc_stress.h
/// GC Stress mode — CHAOS_GC_STRESS=1 env var forces frequent GCs for
/// testing.  Every kStressInterval-th allocation triggers a full GC.
///
/// == Usage ==
/// Set CHAOS_GC_STRESS=1 in the environment before startup, or set
/// g_gc_stress.enabled_ = true programmatically.
///
/// == Thread safety ==
/// - enabled_: set once at startup, read-only after
/// - counter_: atomic process-wide counter
/// - tls_in_gc_stress: TLS guard prevents recursive stress GC

#include <atomic>
#include <cstdlib>

// Forward declaration: chaos_gc_collect is used by the stress trigger hook.
// Full declaration in gc_helpers.h, definition in gc_region.cpp.
extern "C" void chaos_gc_collect() noexcept;

namespace chaos::il2cpp::runtime_core {

/// GC stress state (process-wide).
struct GcStressState {
    /// Whether GC stress mode is active.
    bool enabled_ = false;

    /// Process-wide allocation counter.
    std::atomic<int> counter_{0};

    /// Trigger a full GC every N allocations.
    static constexpr int kStressInterval = 1000;

    /// Initialize from CHAOS_GC_STRESS env var.
    void InitFromEnv() noexcept {
        const char* env = std::getenv("CHAOS_GC_STRESS");
        enabled_ = (env != nullptr && env[0] == '1');
    }
};

/// Process-wide GC stress state.
extern GcStressState g_gc_stress;

/// TLS guard: set to true while inside a stress-triggered GC to prevent
/// recursive re-entry (chaos_gc_collect itself performs allocations).
extern thread_local bool tls_in_gc_stress;

/// Check whether a stress GC should be triggered.
/// Non-recursive: returns false if already inside a stress GC.
inline bool GcStressShouldTrigger() noexcept {
    if (!g_gc_stress.enabled_) return false;
    if (tls_in_gc_stress) return false;
    int prev = g_gc_stress.counter_.fetch_add(1, std::memory_order_relaxed);
    return (prev % GcStressState::kStressInterval) == 0;
}

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_STRESS_H_
