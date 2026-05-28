#ifndef CHAOS_IL2CPP_FORBID_SUSPEND_H_
#define CHAOS_IL2CPP_FORBID_SUSPEND_H_

#include <chaos/native_types.h>
#include <chaos/log.h>

#include <chrono>
#include <cstdint>

namespace chaos::il2cpp::runtime_core::threading {

/// Thread-local depth counter for ForbidSuspendScope.
/// When > 0, SafepointPoll acknowledges the safepoint but does NOT wait,
/// allowing the critical section to complete without deadlock.
extern thread_local int32_t tls_forbid_suspend_depth;

/// Check whether suspend is currently forbidden on this thread.
inline bool IsSuspendForbidden() noexcept {
    return tls_forbid_suspend_depth > 0;
}

/// RAII guard that prevents the calling thread from blocking at a GC safepoint.
///
/// Use when holding runtime locks that the GC may also need (e.g. SyncBlock
/// table lock, thread registry during iteration, safepoint owner CAS).
///
/// When a safepoint is requested while forbid_depth > 0:
///   - The thread acknowledges the safepoint (sets suspend_ack) so the GC
///     does not wait indefinitely.
///   - The thread returns immediately without spinning/waiting, allowing
///     the critical section to complete.
///   - When the last ForbidSuspendScope exits, the depth returns to 0 and
///     the next SafepointPoll will properly wait if the safepoint is still
///     active.
///
/// Usage:
///   {
///       ForbidSuspendScope forbid;
///       // critical region: holds lock the GC may need
///   }  // destructor restores depth; polls safepoint if now at depth 0
class ForbidSuspendScope {
public:
    ForbidSuspendScope() noexcept
#ifdef CHAOS_IL2CPP_DEBUG
        : enter_tick_(FastTickMs())
#endif
    {
        ++tls_forbid_suspend_depth;
    }

    ~ForbidSuspendScope() noexcept {
        --tls_forbid_suspend_depth;
#ifdef CHAOS_IL2CPP_DEBUG
        if (tls_forbid_suspend_depth == 0) {
            uint64_t elapsed = FastTickMs() - enter_tick_;
            if (elapsed > 10) {
                CHAOS_IL2CPP_LOG_WARN_M("THREAD",
                    "ForbidSuspendScope held %llu ms (possible long critical section)",
                    static_cast<unsigned long long>(elapsed));
            }
        }
#endif
    }

    // Non-copyable, non-movable.
    ForbidSuspendScope(const ForbidSuspendScope&) = delete;
    ForbidSuspendScope& operator=(const ForbidSuspendScope&) = delete;
    ForbidSuspendScope(ForbidSuspendScope&&) = delete;
    ForbidSuspendScope& operator=(ForbidSuspendScope&&) = delete;

private:
#ifdef CHAOS_IL2CPP_DEBUG
    uint64_t enter_tick_;
#endif

    static uint64_t FastTickMs() noexcept {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
    }
};

}  // namespace chaos::il2cpp::runtime_core::threading

#endif  // CHAOS_IL2CPP_FORBID_SUSPEND_H_
