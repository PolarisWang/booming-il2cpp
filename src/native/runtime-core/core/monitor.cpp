// monitor.cpp — Monitor (lock) implementation
#include "wait_handle.h"
#include "thread_state.h"
#include <thread>

namespace chaos::il2cpp::runtime_core {

bool MonitorEnter(void* monitor_target) {
    if (monitor_target == nullptr) return false;

    auto* sync_ptr = GetSyncStatePtr(monitor_target);
    if (sync_ptr == nullptr) return false;
    const int32_t tid = threading::GetCurrentThreadId();
    if (tid == 0) return false;

    const uint64_t tid_bits = static_cast<uint64_t>(tid) << kSyncThreadShift;

    uint64_t sync = *sync_ptr;
    for (;;) {
        if ((sync & kSyncInflatedBit) != 0) {
            auto* sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
            if (sb != nullptr) { sb->mutex.lock(); return true; }
            return false;
        }

        if ((sync & kSyncLockedBit) == 0) {
            const uint64_t desired = kSyncLockedBit | tid_bits;
            if (AtomicCAS(sync_ptr, sync, desired, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) {
                return true;
            }
            continue;
        }

        const int32_t owner_tid = static_cast<int32_t>((sync & ~3ull) >> kSyncThreadShift);
        if (owner_tid == tid) {
            // Tier 1D: We already hold the lock — relaxed store is safe because
            // only this thread writes the thin lock bits (inflation by another
            // thread could overwrite, but that's benign: MonitorExit will see
            // inflated bit and route to the SyncBlock correctly).
            const uint64_t recursion = (sync >> kSyncRecursionShift) + 1;
            const uint64_t desired = kSyncLockedBit | tid_bits | (recursion << kSyncRecursionShift);
            AtomicStoreRelaxed(sync_ptr, desired);
            return true;
        }

        // Tier 1C: Adaptive spin — 3-phase strategy.
        // Phase 1: 64 fast pauses (CHAOS_SPIN_HINT / _mm_pause / __yield).
        for (uint32_t spin = 0; spin < 64; ++spin) {
            CHAOS_SPIN_HINT();
            sync = *sync_ptr;
            if ((sync & kSyncLockedBit) == 0) {
                const uint64_t desired = kSyncLockedBit | tid_bits;
                if (AtomicCAS(sync_ptr, sync, desired, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) {
                    return true;
                }
                break;
            }
            if ((sync & kSyncInflatedBit) != 0) {
                auto* sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
                if (sb != nullptr) { sb->mutex.lock(); return true; }
                return false;
            }
        }

        // Phase 2: 64 pauses with periodic yield (every 8 iterations).
        for (uint32_t spin = 0; spin < 64; ++spin) {
            CHAOS_SPIN_HINT();
            if ((spin & 7) == 0) std::this_thread::yield();
            sync = *sync_ptr;
            if ((sync & kSyncLockedBit) == 0) {
                const uint64_t desired = kSyncLockedBit | tid_bits;
                if (AtomicCAS(sync_ptr, sync, desired, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) {
                    return true;
                }
                break;
            }
            if ((sync & kSyncInflatedBit) != 0) {
                auto* sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
                if (sb != nullptr) { sb->mutex.lock(); return true; }
                return false;
            }
        }

        // Phase 3: Inflate immediately — no more wasteful spinning.
        return InflateAndEnter(monitor_target);
    }
}

bool MonitorExit(void* monitor_target) {
    if (monitor_target == nullptr) return false;

    auto* sync_ptr = GetSyncStatePtr(monitor_target);
    if (sync_ptr == nullptr) return false;
    const int32_t tid = threading::GetCurrentThreadId();

    uint64_t sync = *sync_ptr;

    if ((sync & kSyncInflatedBit) != 0) {
        auto* sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
        if (sb != nullptr) { sb->mutex.unlock(); return true; }
        return false;
    }

    if ((sync & kSyncLockedBit) == 0) return false;

    const int32_t owner_tid = static_cast<int32_t>((sync & ~3ull) >> kSyncThreadShift);
    if (owner_tid != tid) return false;

    const uint64_t recursion = sync >> kSyncRecursionShift;
    if (recursion > 0) {
        const uint64_t desired = kSyncLockedBit |
            (static_cast<uint64_t>(tid) << kSyncThreadShift) |
            ((recursion - 1) << kSyncRecursionShift);
        AtomicStoreRelaxed(sync_ptr, desired);
    } else {
        AtomicStoreRelease(sync_ptr, 0);
    }
    return true;
}

bool MonitorTryEnter(void* monitor_target) {
    if (monitor_target == nullptr) return false;
    auto* sync_ptr = GetSyncStatePtr(monitor_target);
    if (sync_ptr == nullptr) return false;
    const int32_t tid = threading::GetCurrentThreadId();
    if (tid == 0) return false;
    uint64_t sync = *sync_ptr;
    for (;;) {
        if ((sync & kSyncInflatedBit) != 0) {
            auto* sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
            return sb != nullptr && sb->mutex.try_lock();
        }
        if ((sync & kSyncLockedBit) == 0) {
            const uint64_t desired = kSyncLockedBit | (static_cast<uint64_t>(tid) << kSyncThreadShift);
            if (AtomicCAS(sync_ptr, sync, desired, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) {
                return true;
            }
            continue;
        }
        return false;
    }
}

bool MonitorIsEntered(void* monitor_target) {
    if (monitor_target == nullptr) return false;
    auto* sync_ptr = GetSyncStatePtr(monitor_target);
    if (sync_ptr == nullptr) return false;
    const int32_t tid = threading::GetCurrentThreadId();
    if (tid == 0) return false;
    uint64_t sync = *sync_ptr;
    if ((sync & kSyncInflatedBit) != 0) {
        auto* sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
        if (sb == nullptr) return false;
        bool owned = sb->mutex.try_lock();
        if (owned) sb->mutex.unlock();
        return owned;
    }
    if ((sync & kSyncLockedBit) != 0) {
        const uint64_t stored_tid = (sync >> kSyncThreadShift) & 0x3FFFFFFF;
        return stored_tid == static_cast<uint64_t>(tid);
    }
    return false;
}

bool MonitorWait(void* monitor_target, int32_t timeout_ms) {
    if (monitor_target == nullptr) return false;

    auto* sync_ptr = GetSyncStatePtr(monitor_target);
    if (sync_ptr == nullptr) return false;
    uint64_t sync = *sync_ptr;
    SyncBlock* sb = nullptr;
    if ((sync & kSyncInflatedBit) != 0) {
        sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
    } else {
        if ((sync & kSyncLockedBit) == 0) return false;
        sb = AllocateSyncBlockFromPool();
        const uint32_t stripe_idx = SyncBlockStripeIndex(monitor_target);
        auto& stripe = g_sync_block_stripes[stripe_idx];
        {
            CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) guard(stripe.table_lock);
            stripe.entries[monitor_target] = sb;
            const uint64_t inflated_val = kSyncInflatedBit | reinterpret_cast<uint64_t>(sb);
            AtomicStoreRelease(sync_ptr, inflated_val);
        }
    }
    if (sb == nullptr) return false;

    // Set WaitSleepJoin after confirming sb is valid.
    auto* thread = threading::GetCurrentThread();
    if (thread) thread->managed_state = threading::ManagedThreadState::WaitSleepJoin;

    // Release the monitor before wait (Monitor.Wait semantics).
    if ((sync & kSyncInflatedBit) != 0) {
        // Inflated path: thread holds sb->mutex from MonitorEnter. Release it.
        sb->mutex.unlock();
    }
    // Thin-lock path: sync was just inflated; no sb->mutex held.

    // Lock condition mutex and wait (cond.wait atomically unlocks during sleep).
    std::unique_lock<CHAOS_IL2CPP_RECURSIVE_LOCK_MUTEX> wait_lock(sb->mutex);
    bool result;
    if (timeout_ms < 0) {
        sb->cond.wait(wait_lock);
        result = true;
    } else {
        result = sb->cond.wait_for(wait_lock,
            std::chrono::milliseconds(timeout_ms)) == std::cv_status::no_timeout;
    }
    wait_lock.unlock();

    // Re-acquire the monitor after wait completes.
    sb->mutex.lock();

    // Restore Running state after wait completes.
    if (thread) thread->managed_state = threading::ManagedThreadState::Running;
    return result;
}

bool MonitorPulse(void* monitor_target) {
    if (monitor_target == nullptr) return false;
    auto* sync_ptr = GetSyncStatePtr(monitor_target);
    if (sync_ptr == nullptr) return false;
    uint64_t sync = *sync_ptr;
    if ((sync & kSyncInflatedBit) != 0) {
        auto* sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
        if (sb != nullptr) { sb->cond.notify_one(); return true; }
    }
    return false;
}

bool MonitorPulseAll(void* monitor_target) {
    if (monitor_target == nullptr) return false;
    auto* sync_ptr = GetSyncStatePtr(monitor_target);
    if (sync_ptr == nullptr) return false;
    uint64_t sync = *sync_ptr;
    if ((sync & kSyncInflatedBit) != 0) {
        auto* sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
        if (sb != nullptr) { sb->cond.notify_all(); return true; }
    }
    return false;
}

bool ThreadSleep(int32_t timeout_ms) {
    if (timeout_ms < 0) return false;
    auto* thread = threading::tls_this_thread;
    if (thread != nullptr && thread->pending_abort.load(std::memory_order_acquire)) {
        return false;
    }
    if (thread) thread->managed_state = threading::ManagedThreadState::WaitSleepJoin;
    GC_TRANSITION_TO_PREEMPTIVE();
    if (timeout_ms == 0) {
        std::this_thread::yield();
    } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
    }
    GC_TRANSITION_TO_COOPERATIVE();
    if (thread) thread->managed_state = threading::ManagedThreadState::Running;
    return true;
}

bool SpinLockExit(void* spinlock_target) {
    if (spinlock_target == nullptr) return false;
    auto* sync_ptr = GetSyncStatePtr(spinlock_target);
    if (sync_ptr == nullptr) return false;
    const int32_t tid = threading::GetCurrentThreadId();
    uint64_t sync = *sync_ptr;
    if ((sync & kSyncLockedBit) == 0) return false;
    const uint64_t stored_tid = (sync >> kSyncThreadShift) & 0x3FFFFFFF;
    if (stored_tid != static_cast<uint64_t>(tid)) return false;
    AtomicStoreRelease(sync_ptr, 0);
    return true;
}

bool SpinLockIsHeld(void* spinlock_target) {
    if (spinlock_target == nullptr) return false;
    auto* sync_ptr = GetSyncStatePtr(spinlock_target);
    if (sync_ptr == nullptr) return false;
    return (*sync_ptr & kSyncLockedBit) != 0;
}

bool LockEnter(void* lock_target) { return MonitorEnter(lock_target); }

bool LockExit(void* lock_target) { return MonitorExit(lock_target); }

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

}  // namespace chaos::il2cpp::runtime_core

// ── extern "C" ABI exports ──────────────────────────────────────────
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