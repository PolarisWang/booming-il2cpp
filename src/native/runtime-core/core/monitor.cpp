namespace chaos::il2cpp::runtime_core {
namespace {

bool MonitorEnter(void* monitor_target) {
    if (monitor_target == nullptr) return false;

    auto* sync_ptr = GetSyncStatePtr(monitor_target);
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
            const uint64_t recursion = (sync >> kSyncRecursionShift) + 1;
            const uint64_t desired = kSyncLockedBit | tid_bits | (recursion << kSyncRecursionShift);
            if (AtomicCAS(sync_ptr, sync, desired, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
                return true;
            }
            continue;
        }

        for (uint32_t spin = 0; spin < kSyncBlockSpinMax; ++spin) {
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

        return InflateAndEnter(monitor_target, sync);
    }
}

bool MonitorExit(void* monitor_target) {
    if (monitor_target == nullptr) return false;

    auto* sync_ptr = GetSyncStatePtr(monitor_target);
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
    const int32_t tid = threading::GetCurrentThreadId();
    if (tid == 0) return false;
    uint64_t sync = *sync_ptr;
    if ((sync & kSyncInflatedBit) != 0) return sync != 0;
    if ((sync & kSyncLockedBit) != 0) {
        const uint64_t stored_tid = (sync >> kSyncThreadShift) & 0x3FFFFFFF;
        return stored_tid == static_cast<uint64_t>(tid);
    }
    return false;
}

bool MonitorWait(void* monitor_target, int32_t timeout_ms) {
    if (monitor_target == nullptr) return false;
    auto* sync_ptr = GetSyncStatePtr(monitor_target);
    uint64_t sync = *sync_ptr;
    SyncBlock* sb = nullptr;
    if ((sync & kSyncInflatedBit) != 0) {
        sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
    } else {
        if ((sync & kSyncLockedBit) == 0) return false;
        sb = new SyncBlock();
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
    if (timeout_ms < 0) {
        sb->cond.wait(sb->mutex);
        return true;
    }
    return sb->cond.wait_for(sb->mutex,
        std::chrono::milliseconds(timeout_ms)) == std::cv_status::no_timeout;
}

bool MonitorPulse(void* monitor_target) {
    if (monitor_target == nullptr) return false;
    auto* sync_ptr = GetSyncStatePtr(monitor_target);
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
    GC_TRANSITION_TO_PREEMPTIVE();
    if (timeout_ms == 0) {
        std::this_thread::yield();
    } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
    }
    GC_TRANSITION_TO_COOPERATIVE();
    return true;
}

bool SpinLockExit(void* spinlock_target) {
    if (spinlock_target == nullptr) return false;
    auto* sync_ptr = GetSyncStatePtr(spinlock_target);
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
    return (*GetSyncStatePtr(spinlock_target) & kSyncLockedBit) != 0;
}

bool LockEnter(void* lock_target) { return MonitorEnter(lock_target); }

bool LockExit(void* lock_target) { return MonitorExit(lock_target); }

bool WaitHandleSet(void* /*wait_handle*/) { return false; }

bool WaitHandleReset(void* /*wait_handle*/) { return false; }

}  // anonymous namespace
}  // namespace chaos::il2cpp::runtime_core