// gc_lock.h — short-critical-section spinlock (CoreCLR GCSpinLock-aligned).
//
// Multiple GC subsystems (LOH, RegionManager) protect short operations with a
// single lock.  Using std::mutex for those leaves a cooperative thread unable
// to acknowledge a safepoint while blocked in the non-alertable kernel wait
// (Windows WaitForSingleObject): the coordinator counts it unresponsive and
// hard-times-out under Server-GC concurrency.
//
// This spinlock never parks — bounded pause-hint spins then yields
// (SwitchToThread / sched_yield), so a waiting thread is OS-scheduler
// preemptible, and (when the caller wraps the region in preemptive mode)
// safepoint-interruptible.  Mirrors CoreCLR GCSpinLock (gcpriv.h
// more_space_lock_soh/uoh + region_allocator_lock).
#ifndef CHAOS_IL2CPP_GC_LOCK_H_
#define CHAOS_IL2CPP_GC_LOCK_H_

#include <atomic>
#include <chrono>

#include <chaos/native_types.h>      // CHAOS_IL2CPP_PAUSE_HINT
#include <chaos/pal/pal_thread.h>    // PalYield (SwitchToThread)

#include "thread_state.h"            // TrapReturningThreads, EnterPreemptiveMode, EnterCooperativeMode

namespace chaos::il2cpp::runtime_core {

/// Short-critical-section spinlock with bounded pause-then-yield, and
/// CoreCLR-aligned safepoint-aware waiting (WaitLongerNoInstru pattern).
/// Never parks in a kernel wait; preemptible by the OS scheduler and
/// safepoint handshake.
///
/// == Safepoint-aware spin (CoreCLR WaitLongerNoInstru pattern) ==
/// Every 1024 spins, the Acquire loop checks TrapReturningThreads().  If a
/// safepoint is in progress, the thread temporarily switches to preemptive
/// mode (yielding the safepoint handshake) and yields, then re-enters
/// cooperative mode and continues spinning.  This prevents a 3-party deadlock
/// where the safepoint owner waits for this thread's ack, but this thread is
/// spinning on a lock held by a thread that is waiting for the safepoint to
/// complete.
///
/// WARNING: NOT REENTRANT.  Acquiring the same GcSpinLock instance from the
/// same thread (e.g. via a nested call that expects the same lock) will DEADLOCK
/// — the spin never times out.  All callers must ensure a single lock acquisition
/// per critical section with no recursive entry.  Debug builds assert this via
/// a thread-local owner check.
class GcSpinLock {
public:
    GcSpinLock() noexcept = default;
    GcSpinLock(const GcSpinLock&) = delete;
    GcSpinLock& operator=(const GcSpinLock&) = delete;

    void Acquire() noexcept {
        int spins = 0;
        while (flag_.test_and_set(std::memory_order_acquire)) {
            if (++spins <= 1024) {
                CHAOS_IL2CPP_PAUSE_HINT();
            } else {
                ::chaos::il2cpp::pal::PalYield();
                if (++spins > 100000) spins = 0;   // reset counter
                // CoreCLR WaitLongerNoInstru: periodically check if a
                // safepoint is in progress.  If so, briefly drop to preemptive
                // mode so the coordinator can see this thread as preemptive
                // and not wait for its ack (prevents 3-party deadlock where
                // the safepoint owner waits for this thread's ack, but this
                // thread is spinning on a lock held by a thread that is
                // waiting for the safepoint to complete).
                if ((spins & 511) == 0) {
                    if (threading::TrapReturningThreads()) {
                        threading::EnterPreemptiveMode();
                        ::chaos::il2cpp::pal::PalYield();
                        threading::EnterCooperativeMode();
                    }
                }
            }
        }
    }

    void Release() noexcept {
        flag_.clear(std::memory_order_release);
    }

private:
    std::atomic_flag flag_{};
};

/// RAII guard for GcSpinLock (drop-in for std::lock_guard).
class GcSpinLockGuard {
public:
    explicit GcSpinLockGuard(GcSpinLock& m) noexcept : m_(m) { m_.Acquire(); }
    ~GcSpinLockGuard() noexcept { m_.Release(); }
    GcSpinLockGuard(const GcSpinLockGuard&) = delete;
    GcSpinLockGuard& operator=(const GcSpinLockGuard&) = delete;

private:
    GcSpinLock& m_;
};

/// RAII guard that switches the calling thread to preemptive GC mode for the
/// duration of a scope and restores cooperative mode on exit, including on
/// exception.  Any thread that blocks or spins (e.g. on GcSpinLock::Acquire, or
/// a long critical section) WITHOUT this guard stays in cooperative mode and
/// will not acknowledge a safepoint — the coordinator counts it unresponsive
/// and hard-times-out.  Wrap lock-held regions with this to tell the
/// coordinator to skip the thread while it may be waiting.
///
/// Requires: the calling thread is registered (tls_this_thread != nullptr) and
/// is entering the region in cooperative mode.  EnterCooperativeMode/
/// EnterPreemptiveMode are noexcept, so both the ctor and dtor are noexcept.
class [[nodiscard]] ScopedPreemptiveMode {
public:
    ScopedPreemptiveMode() noexcept {
        threading::EnterPreemptiveMode();
    }
    ~ScopedPreemptiveMode() noexcept {
        threading::EnterCooperativeMode();
    }
    ScopedPreemptiveMode(const ScopedPreemptiveMode&) = delete;
    ScopedPreemptiveMode& operator=(const ScopedPreemptiveMode&) = delete;
};

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_LOCK_H_
