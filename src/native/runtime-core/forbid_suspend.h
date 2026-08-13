#ifndef CHAOS_IL2CPP_FORBID_SUSPEND_H_
#define CHAOS_IL2CPP_FORBID_SUSPEND_H_

#include <chaos/native_types.h>
#include <chaos/log.h>

#include <chrono>
#include <cstdint>

// Pull in tls_this_thread / ManagedThread for the write-barrier critical
// section scope below.  thread_state.h does not include this header, so there
// is no include cycle.
#include "thread_state.h"

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

/// RAII guard that makes a store→card write-barrier critical section
/// non-preemptible from the safepoint coordinator's perspective.
///
/// Aligns CRAG's write barrier with CoreCLR's poll-free-leaf semantics: the
/// safepoint coordinator must observe store+card as one atomic step.  This
/// scope (entered BEFORE the object store, exited AFTER the card is dirtied):
///   1. increments tls_forbid_suspend_depth — so SafepointPoll acks-and-
///      continues (no deadlock) instead of PalEventWait-blocking mid-window;
///   2. publishes cross-thread ManagedThread::barrier_inflight=1 on entry and
///      0 (release) on exit — the coordinator waits for 0 before Phase-1, so a
///      store is never scanned with its card still clean.
///
/// CRITICAL: only the store + card-table-dirty go inside this scope.  Do NOT
/// wrap the SATB pre-write barrier (BgcSatbPreWriteBarrier can allocate via
/// AllocateSatbBuffer / flush, which may request an emergency full GC) or any
/// C++ heap allocation (resize) inside it — allocation can trigger a safepoint.
class BarrierCriticalSectionScope {
public:
    BarrierCriticalSectionScope() noexcept {
        ++tls_forbid_suspend_depth;                       // anti-deadlock: ack-and-continue
        if (auto* t = tls_this_thread; t != nullptr)
            t->barrier_inflight.store(1, std::memory_order_relaxed);
    }
    ~BarrierCriticalSectionScope() noexcept {
        if (auto* t = tls_this_thread; t != nullptr)
            t->barrier_inflight.store(0, std::memory_order_release);  // release orders the card-set
        --tls_forbid_suspend_depth;
    }

    BarrierCriticalSectionScope(const BarrierCriticalSectionScope&) = delete;
    BarrierCriticalSectionScope& operator=(const BarrierCriticalSectionScope&) = delete;
    BarrierCriticalSectionScope(BarrierCriticalSectionScope&&) = delete;
    BarrierCriticalSectionScope& operator=(BarrierCriticalSectionScope&&) = delete;
};

}  // namespace chaos::il2cpp::runtime_core::threading

#endif  // CHAOS_IL2CPP_FORBID_SUSPEND_H_
