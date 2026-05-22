#ifndef CHAOS_IL2CPP_FORBID_SUSPEND_H_
#define CHAOS_IL2CPP_FORBID_SUSPEND_H_

#include <chaos/native_types.h>

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
    ForbidSuspendScope() noexcept {
        ++tls_forbid_suspend_depth;
    }

    ~ForbidSuspendScope() noexcept {
        --tls_forbid_suspend_depth;
    }

    // Non-copyable, non-movable.
    ForbidSuspendScope(const ForbidSuspendScope&) = delete;
    ForbidSuspendScope& operator=(const ForbidSuspendScope&) = delete;
    ForbidSuspendScope(ForbidSuspendScope&&) = delete;
    ForbidSuspendScope& operator=(ForbidSuspendScope&&) = delete;
};

}  // namespace chaos::il2cpp::runtime_core::threading

#endif  // CHAOS_IL2CPP_FORBID_SUSPEND_H_
