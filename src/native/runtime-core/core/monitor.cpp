// monitor.cpp — Monitor (lock) implementation backed by ThinLockTable
//
// Type System Phase 1: sync_state moved from object header to ThinLockTable.
// MonitorEnter/Exit operate on ThinLockTable entries instead of direct field CAS.
// SyncBlock (contention, MonitorWait/Pulse) is unchanged — entries stored in
// ThinLockTable with kThinInflatedBit set and SyncBlock* as the value.

#include "thin_lock_table.h"
#include "wait_handle.h"
#include "thread_state.h"
#include "generated_code_compat.h"
#include <thread>

namespace chaos::il2cpp::runtime_core {

// ── Helpers ─────────────────────────────────────────────────────────────────

static SyncBlock* GetSyncBlockFromEntry(uint64_t entry_value) noexcept {
    if ((entry_value & kThinInflatedBit) == 0) return nullptr;
    return reinterpret_cast<SyncBlock*>(entry_value & ~3ull);
}

// ── MonitorEnter ────────────────────────────────────────────────────────────

bool MonitorEnter(void* monitor_target) {
    if (monitor_target == nullptr) return false;

    auto& table = ThinLockTable::Instance();
    const int32_t tid = threading::GetCurrentThreadId();
    if (tid == 0) return false;

    // Phase 1: Try thin lock (fast path).
    if (table.TryLock(monitor_target, tid)) {
        return true;
    }

    // Phase 1A: Check if already inflated — contend on SyncBlock.
    uint64_t sync = table.ReadSyncValue(monitor_target);
    if (sync & kThinInflatedBit) {
        auto* sb = GetSyncBlockFromEntry(sync);
        if (sb != nullptr) {
            const int32_t tid_local = threading::GetCurrentThreadId();
            if (sb->owner_tid.load(std::memory_order_acquire) != tid_local) {
                // First entry from this thread — must lock the mutex.
                sb->mutex.lock();
                sb->recursion.store(1, std::memory_order_relaxed);
            } else {
                // Reentrant entry — already own the mutex.
                sb->recursion.fetch_add(1, std::memory_order_relaxed);
            }
            sb->owner_tid.store(tid_local, std::memory_order_relaxed);
            return true;
        }
        return false;
    }

    // Phase 2: Adaptive spin — 3-phase strategy.
    // Phase 2: 64 fast pauses (CHAOS_SPIN_HINT).
    for (uint32_t spin = 0; spin < 64; ++spin) {
        CHAOS_SPIN_HINT();
        if (table.TryLock(monitor_target, tid)) {
            return true;
        }
        sync = table.ReadSyncValue(monitor_target);
        if (sync & kThinInflatedBit) {
            auto* sb = GetSyncBlockFromEntry(sync);
            if (sb != nullptr) {
                const int32_t tid_spin = threading::GetCurrentThreadId();
                if (sb->owner_tid.load(std::memory_order_acquire) != tid_spin) {
                    sb->mutex.lock();
                    sb->recursion.store(1, std::memory_order_relaxed);
                } else {
                    sb->recursion.fetch_add(1, std::memory_order_relaxed);
                }
                sb->owner_tid.store(tid_spin, std::memory_order_relaxed);
                return true;
            }
            return false;
        }
    }

    // Phase 2: 64 pauses with periodic yield.
    for (uint32_t spin = 0; spin < 64; ++spin) {
        CHAOS_SPIN_HINT();
        if ((spin & 7) == 0) std::this_thread::yield();
        if (table.TryLock(monitor_target, tid)) {
            return true;
        }
        sync = table.ReadSyncValue(monitor_target);
        if (sync & kThinInflatedBit) {
            auto* sb = GetSyncBlockFromEntry(sync);
            if (sb != nullptr) {
                const int32_t tid_spin = threading::GetCurrentThreadId();
                if (sb->owner_tid.load(std::memory_order_acquire) != tid_spin) {
                    sb->mutex.lock();
                    sb->recursion.store(1, std::memory_order_relaxed);
                } else {
                    sb->recursion.fetch_add(1, std::memory_order_relaxed);
                }
                sb->owner_tid.store(tid_spin, std::memory_order_relaxed);
                return true;
            }
            return false;
        }
    }

    // Phase 3: Inflate immediately — no more wasteful spinning.
    return InflateAndEnter(monitor_target);
}

// ── MonitorExit ─────────────────────────────────────────────────────────────

bool MonitorExit(void* monitor_target) {
    if (monitor_target == nullptr) return false;

    auto& table = ThinLockTable::Instance();
    const int32_t tid = threading::GetCurrentThreadId();

    // Try thin-lock unlock first.
    if (table.Unlock(monitor_target, tid)) {
        return true;  // Fully released.
    }

    // Unlock returned false — check if inflated.
    uint64_t sync = table.ReadSyncValue(monitor_target);
    if (sync & kThinInflatedBit) {
        auto* sb = GetSyncBlockFromEntry(sync);
        if (sb != nullptr) {
            // Ownership check.
            int32_t expected_tid = sb->owner_tid.load(std::memory_order_acquire);
            if (expected_tid != 0 && expected_tid == threading::GetCurrentThreadId()) {
                uint32_t remaining = sb->recursion.fetch_sub(1, std::memory_order_release);
                if (remaining <= 1) {
                    // Physical release — last nested exit.
                    sb->owner_tid.store(0, std::memory_order_release);
                    sb->mutex.unlock();
                }
            }
            return true;
        }
        return false;
    }

    // Not in table and not inflated — not locked.
    return false;
}

// ── MonitorTryEnter ─────────────────────────────────────────────────────────

bool MonitorTryEnter(void* monitor_target) {
    if (monitor_target == nullptr) return false;
    auto& table = ThinLockTable::Instance();
    const int32_t tid = threading::GetCurrentThreadId();
    if (tid == 0) return false;

    // Fast path: try thin-lock entry.
    if (table.TryEnter(monitor_target, tid)) {
        return true;
    }

    // Check if inflated — try SyncBlock::try_lock.
    uint64_t sync = table.ReadSyncValue(monitor_target);
    if (sync & kThinInflatedBit) {
        auto* sb = GetSyncBlockFromEntry(sync);
        if (sb != nullptr && sb->mutex.try_lock()) {
            sb->owner_tid.store(threading::GetCurrentThreadId(), std::memory_order_relaxed);
            return true;
        }
    }

    return false;
}

// ── MonitorIsEntered ────────────────────────────────────────────────────────

bool MonitorIsEntered(void* monitor_target) {
    if (monitor_target == nullptr) return false;
    auto& table = ThinLockTable::Instance();
    const int32_t tid = threading::GetCurrentThreadId();
    if (tid == 0) return false;

    // Check thin-lock table first.
    if (table.IsEntered(monitor_target, tid)) {
        return true;
    }

    // Check if inflated — read owner_tid.
    uint64_t sync = table.ReadSyncValue(monitor_target);
    if (sync & kThinInflatedBit) {
        auto* sb = GetSyncBlockFromEntry(sync);
        if (sb != nullptr) {
            return sb->owner_tid.load(std::memory_order_acquire) == threading::GetCurrentThreadId();
        }
    }

    return false;
}

// ── MonitorWait ─────────────────────────────────────────────────────────────

bool MonitorWait(void* monitor_target, int32_t timeout_ms) {
    if (monitor_target == nullptr) return false;

    auto& table = ThinLockTable::Instance();
    const int32_t tid = threading::GetCurrentThreadId();

    uint64_t sync = table.ReadSyncValue(monitor_target);

    SyncBlock* sb = nullptr;
    if (sync & kThinInflatedBit) {
        sb = GetSyncBlockFromEntry(sync);
    } else {
        // Not inflated — must inflate for MonitorWait.
        if ((sync & kThinLockedBit) == 0) return false;  // Not locked.

        sb = AllocateSyncBlockFromPool();

        // Update ThinLockTable entry to inflated.
        auto* existing = table.Inflate(monitor_target, sb);
        if (existing != sb) {
            // Another thread already inflated — use existing SyncBlock.
            sb = existing;
        }
    }

    if (sb == nullptr) return false;

    // Check abort/interrupt before blocking.
    auto* thread = threading::GetCurrentThread();
    if (thread != nullptr) {
        if (thread->pending_abort.load(std::memory_order_acquire)) {
            thread->pending_abort.store(false, std::memory_order_release);
            throw chaos_managed_exception{kManagedExceptionThreadAbort};
        }
        if (thread->pending_interrupt.load(std::memory_order_acquire)) {
            thread->pending_interrupt.store(false, std::memory_order_release);
            throw chaos_managed_exception{kManagedExceptionThreadInterrupt};
        }
    }

    // Set WaitSleepJoin after confirming sb is valid and no abort/interrupt.
    if (thread) threading::SetThreadState(*thread, threading::ManagedThreadState::WaitSleepJoin);

    // Release the monitor before wait (Monitor.Wait semantics).
    int32_t owner = sb->owner_tid.load(std::memory_order_acquire);
    if (owner == threading::GetCurrentThreadId()) {
        sb->owner_tid.store(0, std::memory_order_release);
        sb->mutex.unlock();
    }  // If we don't own it, the inflater does — skip unlock.

    // Lock condition mutex and wait (cond.wait atomically unlocks during sleep).
    std::unique_lock<CHAOS_IL2CPP_RECURSIVE_LOCK_MUTEX> wait_lock(sb->mutex);

    // Increment wait_count for chain-signal tracking.
    sb->wait_count.fetch_add(1, std::memory_order_relaxed);

    bool result;
    if (timeout_ms < 0) {
        sb->cond.wait(wait_lock);
        result = true;
    } else {
        result = sb->cond.wait_for(wait_lock,
            std::chrono::milliseconds(timeout_ms)) == std::cv_status::no_timeout;
    }

    // Decrement wait_count after waking.
    uint32_t remaining = sb->wait_count.fetch_sub(1, std::memory_order_relaxed) - 1;

    // Chain-signal: if woken by a pulse (not timeout) and more waiters
    // remain, wake the next one.
    if (result && remaining > 0) {
        sb->cond.notify_one();
    }

    wait_lock.unlock();

    // Re-acquire the monitor after wait completes.
    sb->mutex.lock();
    sb->owner_tid.store(threading::GetCurrentThreadId(), std::memory_order_relaxed);

    // Restore Running state after wait completes.
    if (thread) threading::SetThreadState(*thread, threading::ManagedThreadState::Running);
    return result;
}

// ── MonitorPulse ────────────────────────────────────────────────────────────

bool MonitorPulse(void* monitor_target) {
    if (monitor_target == nullptr) return false;

    auto& table = ThinLockTable::Instance();
    uint64_t sync = table.ReadSyncValue(monitor_target);

    if (sync & kThinInflatedBit) {
        auto* sb = GetSyncBlockFromEntry(sync);
        if (sb != nullptr) { sb->cond.notify_one(); return true; }
    }
    return false;
}

// ── MonitorPulseAll ─────────────────────────────────────────────────────────

bool MonitorPulseAll(void* monitor_target) {
    if (monitor_target == nullptr) return false;

    auto& table = ThinLockTable::Instance();
    uint64_t sync = table.ReadSyncValue(monitor_target);

    if (sync & kThinInflatedBit) {
        auto* sb = GetSyncBlockFromEntry(sync);
        if (sb != nullptr) {
            // Chain-signal PulseAll: increment pulse_count, then wake one
            // waiter.  Each woken thread calls notify_one for the next.
            uint32_t wc = sb->wait_count.load(std::memory_order_acquire);
            if (wc > 0) {
                sb->pulse_count.fetch_add(1, std::memory_order_release);
                sb->cond.notify_one();
            } else {
                sb->cond.notify_all();
            }
            return true;
        }
    }
    return false;
}

// ── ThreadSleep ─────────────────────────────────────────────────────────────

bool ThreadSleep(int32_t timeout_ms) {
    if (timeout_ms < 0) return false;
    auto* thread = threading::tls_this_thread;
    if (thread != nullptr) {
        if (thread->pending_abort.load(std::memory_order_acquire)) {
            thread->pending_abort.store(false, std::memory_order_release);
            throw chaos_managed_exception{kManagedExceptionThreadAbort};
        }
        if (thread->pending_interrupt.load(std::memory_order_acquire)) {
            thread->pending_interrupt.store(false, std::memory_order_release);
            throw chaos_managed_exception{kManagedExceptionThreadInterrupt};
        }
    }
    if (thread) threading::SetThreadState(*thread, threading::ManagedThreadState::WaitSleepJoin);
    GC_TRANSITION_TO_PREEMPTIVE();
    if (timeout_ms == 0) {
        std::this_thread::yield();
    } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
    }
    GC_TRANSITION_TO_COOPERATIVE();
    if (thread) threading::SetThreadState(*thread, threading::ManagedThreadState::Running);
    return true;
}

// ── SpinLockExit (unused by monitors, kept for API compatibility) ──────────

bool SpinLockExit(void* spinlock_target) {
    // SpinLock is not supported via ThinLockTable — use MonitorExit.
    return MonitorExit(spinlock_target);
}

bool SpinLockIsHeld(void* spinlock_target) {
    return MonitorIsEntered(spinlock_target);
}

// ── LockEnter/LockExit (aliases) ───────────────────────────────────────────

bool LockEnter(void* lock_target) { return MonitorEnter(lock_target); }
bool LockExit(void* lock_target) { return MonitorExit(lock_target); }

// ── WaitHandleSet/Reset ────────────────────────────────────────────────────

bool WaitHandleSet(void* wait_handle) {
    if (wait_handle == nullptr) return false;
    uint32_t handle_id = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(wait_handle));
    return threading::WaitHandleSet(handle_id);
}

bool WaitHandleReset(void* wait_handle) {
    if (wait_handle == nullptr) return false;
    uint32_t handle_id = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(wait_handle));
    return threading::WaitHandleReset(handle_id);
}

// ── InflateAndEnter (need access to SyncBlock pool) ───────────────────────
// Defined in sync_mutex.cpp — forward declaration.
extern bool InflateAndEnter(void* obj) noexcept;

}  // namespace chaos::il2cpp::runtime_core

// ── extern "C" ABI exports ──────────────────────────────────────────────────
extern "C" {
namespace chaos::il2cpp::runtime_core {

CHAOS_IL2CPP_INT32 ChaosMonitorTryEnter(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INT32 timeout) noexcept
{
    if (obj == 0) return 0;
    void* monitor_target = reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_INTPTR>(obj));

    if (timeout == 0) {
        return MonitorTryEnter(monitor_target) ? 1 : 0;
    }

    if (timeout < 0) {
        MonitorEnter(monitor_target);
        return 1;
    }

    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout);
    do {
        if (MonitorTryEnter(monitor_target)) return 1;
        std::this_thread::yield();
    } while (std::chrono::steady_clock::now() < deadline);

    return 0;
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"
