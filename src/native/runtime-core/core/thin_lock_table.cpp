// thin_lock_table.cpp — Global thin-lock state table
//
// Replaces per-object sync_state with a 64-way striped hash table.
// Design rationale and risk analysis in:
//   wiki/03-功能模块/06-il2cpp核心架构/04-历史决策/20-TypeSystem-Phase1-ObjectHeader优化.md

#include "thin_lock_table.h"
#include "gc_region.h"
#include "forbid_suspend.h"

#include <chaos/log.h>
#include <chaos/unordered_dense.h>

namespace chaos::il2cpp::runtime_core {

// ── Singleton ──────────────────────────────────────────────────────────────

ThinLockTable& ThinLockTable::Instance() noexcept {
    static ThinLockTable s_instance;
    return s_instance;
}

ThinLockTable::ThinLockTable() noexcept {
    // Stripes are default-constructed (mutex + empty map).
}

// ── Lock helpers (file-local, same as sync_mutex.cpp) ──────────────────────

#if defined(_MSC_VER)
    #include <intrin.h>
    #pragma intrinsic(_InterlockedCompareExchange64, _InterlockedExchange64)
    inline uint64_t ThinAtomicLoadRelaxed(const uint64_t* p) noexcept {
        return *const_cast<volatile uint64_t*>(p);
    }
    inline void ThinAtomicStoreRelease(uint64_t* p, uint64_t val) noexcept {
        _InterlockedExchange64(reinterpret_cast<volatile LONG64*>(p), static_cast<LONG64>(val));
    }
    inline bool ThinAtomicCAS(uint64_t* p, uint64_t& expected, uint64_t desired) noexcept {
        LONG64 prev = _InterlockedCompareExchange64(
            reinterpret_cast<volatile LONG64*>(p), static_cast<LONG64>(desired), static_cast<LONG64>(expected));
        if (prev == static_cast<LONG64>(expected)) return true;
        expected = static_cast<uint64_t>(prev);
        return false;
    }
#else
    inline uint64_t ThinAtomicLoadRelaxed(const uint64_t* p) noexcept {
        return __atomic_load_n(p, __ATOMIC_RELAXED);
    }
    inline void ThinAtomicStoreRelease(uint64_t* p, uint64_t val) noexcept {
        __atomic_store_n(p, val, __ATOMIC_RELEASE);
    }
    inline bool ThinAtomicCAS(uint64_t* p, uint64_t& expected, uint64_t desired) noexcept {
        return __atomic_compare_exchange_n(p, &expected, desired, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED);
    }
#endif

// ── Core operations ────────────────────────────────────────────────────────

bool ThinLockTable::TryLock(void* obj, int32_t tid) noexcept {
    auto& stripe = stripes_[StripeIndex(obj)];
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) guard(stripe.mutex);

    auto it = stripe.entries.find(obj);
    if (it == stripe.entries.end()) {
        // First lock: insert new entry.
        const uint64_t tid_bits = static_cast<uint64_t>(tid) << kThinThreadShift;
        stripe.entries[obj] = kThinLockedBit | tid_bits;
        return true;
    }

    uint64_t sync = it->second;

    // Check if already inflated.
    if ((sync & kThinInflatedBit) != 0) {
        return false;  // Caller must handle via SyncBlock.
    }

    // Check if we already hold the lock (re-entrant).
    const int32_t owner_tid = static_cast<int32_t>((sync & ~3ull) >> kThinThreadShift);
    if (owner_tid == tid) {
        const uint64_t recursion = (sync >> kThinRecursionShift) + 1;
        it->second = kThinLockedBit |
            (static_cast<uint64_t>(tid) << kThinThreadShift) |
            (recursion << kThinRecursionShift);
        return true;
    }

    // Another thread holds the lock — contention.
    return false;
}

bool ThinLockTable::Unlock(void* obj, int32_t tid) noexcept {
    auto& stripe = stripes_[StripeIndex(obj)];
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) guard(stripe.mutex);

    auto it = stripe.entries.find(obj);
    if (it == stripe.entries.end()) {
        return false;  // Not in table — may be inflated or never locked.
    }

    uint64_t sync = it->second;

    // Check if inflated — unlock handled by SyncBlock caller.
    if ((sync & kThinInflatedBit) != 0) {
        return false;
    }

    // Ownership check.
    const int32_t owner_tid = static_cast<int32_t>((sync & ~3ull) >> kThinThreadShift);
    if (owner_tid != tid) {
        return false;  // Not the owner.
    }

    // Decrement recursion or release.
    const uint64_t recursion = sync >> kThinRecursionShift;
    if (recursion > 0) {
        it->second = kThinLockedBit |
            (static_cast<uint64_t>(tid) << kThinThreadShift) |
            ((recursion - 1) << kThinRecursionShift);
        return false;  // Still held (recursion > 0).
    }

    // Fully released — remove entry.
    stripe.entries.erase(it);
    return true;
}

bool ThinLockTable::TryEnter(void* obj, int32_t tid) noexcept {
    auto& stripe = stripes_[StripeIndex(obj)];
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) guard(stripe.mutex);

    auto it = stripe.entries.find(obj);
    if (it == stripe.entries.end()) {
        // Not locked — acquire.
        const uint64_t tid_bits = static_cast<uint64_t>(tid) << kThinThreadShift;
        stripe.entries[obj] = kThinLockedBit | tid_bits;
        return true;
    }

    uint64_t sync = it->second;
    if ((sync & kThinInflatedBit) != 0) {
        return false;  // Inflated — SyncBlock handles it.
    }

    const int32_t owner_tid = static_cast<int32_t>((sync & ~3ull) >> kThinThreadShift);
    if (owner_tid == tid) {
        // Re-entrant.
        const uint64_t recursion = (sync >> kThinRecursionShift) + 1;
        it->second = kThinLockedBit |
            (static_cast<uint64_t>(tid) << kThinThreadShift) |
            (recursion << kThinRecursionShift);
        return true;
    }

    return false;  // Held by another thread.
}

bool ThinLockTable::IsEntered(void* obj, int32_t tid) noexcept {
    auto& stripe = stripes_[StripeIndex(obj)];
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) guard(stripe.mutex);

    auto it = stripe.entries.find(obj);
    if (it == stripe.entries.end()) {
        return false;
    }

    uint64_t sync = it->second;
    if ((sync & kThinInflatedBit) != 0) {
        return false;  // Inflated — SyncBlock caller checks owner_tid.
    }

    if ((sync & kThinLockedBit) == 0) {
        return false;
    }

    const int32_t owner_tid = static_cast<int32_t>((sync & ~3ull) >> kThinThreadShift);
    return owner_tid == tid;
}

uint64_t ThinLockTable::ReadSyncValue(void* obj) noexcept {
    auto& stripe = stripes_[StripeIndex(obj)];
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) guard(stripe.mutex);

    auto it = stripe.entries.find(obj);
    if (it == stripe.entries.end()) {
        return 0;  // Not locked.
    }
    return it->second;
}

// ── Inflation ──────────────────────────────────────────────────────────────

SyncBlock* ThinLockTable::Inflate(void* obj, SyncBlock* sb) noexcept {
    auto& stripe = stripes_[StripeIndex(obj)];
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) guard(stripe.mutex);

    auto it = stripe.entries.find(obj);
    if (it == stripe.entries.end()) {
        // Not in thin-lock table — another thread already inflated.
        // Return sb so caller can contend on the existing SyncBlock.
        return sb;
    }

    uint64_t sync = it->second;
    if ((sync & kThinInflatedBit) != 0) {
        // Already inflated (race) — return existing SyncBlock.
        return sb;
    }

    // Remove thin-lock entry.
    stripe.entries.erase(it);

    // Publish SyncBlock. The caller is responsible for locking sb->mutex
    // and storing the inflated bit in the SyncBlock structure.
    // Return the SyncBlock that was allocated by the caller.
    return sb;
}

void ThinLockTable::RemoveEntry(void* obj) noexcept {
    auto& stripe = stripes_[StripeIndex(obj)];
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) guard(stripe.mutex);
    stripe.entries.erase(obj);
}

// ── GC interface ───────────────────────────────────────────────────────────

uint32_t ThinLockTable::GetLockedCount() const noexcept {
    uint32_t count = 0;
    for (uint32_t i = 0; i < kThinLockStripes; i++) {
        // Lock-free read: approximate count, used for pinning budget estimation.
        count += static_cast<uint32_t>(stripes_[i].entries.size());
    }
    return count;
}

uint32_t ThinLockTable::GetLockedObjects(void** out, uint32_t capacity) const noexcept {
    uint32_t written = 0;
    for (uint32_t i = 0; i < kThinLockStripes && written < capacity; i++) {
        auto& stripe = stripes_[i];
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) guard(stripe.mutex);

        for (auto& [obj, sync] : stripe.entries) {
            if (written >= capacity) break;
            // Only include thin-locked entries (not inflated).
            if ((sync & kThinInflatedBit) == 0) {
                out[written++] = obj;
            }
        }
    }
    return written;
}

bool ThinLockTable::IsLocked(const void* obj) const noexcept {
    auto& stripe = stripes_[const_cast<void*>(obj)];
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) guard(stripe.mutex);

    auto it = stripe.entries.find(const_cast<void*>(obj));
    if (it == stripe.entries.end()) return false;

    return (it->second & kThinLockedBit) != 0;
}

void ThinLockTable::RelocateEntry(void* old_addr, void* new_addr) noexcept {
    if (old_addr == new_addr) return;

    auto& stripe = stripes_[StripeIndex(old_addr)];
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) guard(stripe.mutex);

    auto it = stripe.entries.find(old_addr);
    if (it == stripe.entries.end()) return;

    // Re-key: insert under new address, remove old entry.
    uint64_t sync = it->second;
    stripe.entries.erase(it);
    stripe.entries[new_addr] = sync;
}

void ThinLockTable::DrainForDomain(uint32_t domain_id) noexcept {
    uint32_t removed = 0;
    RegionManager& mgr = RegionManager::Instance();

    for (uint32_t i = 0; i < kThinLockStripes; i++) {
        auto& stripe = stripes_[i];
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(stripe.mutex);

        auto it = stripe.entries.begin();
        while (it != stripe.entries.end()) {
            if (it->first != nullptr && mgr.IsInDomain(domain_id, it->first)) {
                it = stripe.entries.erase(it);
                removed++;
            } else {
                ++it;
            }
        }
    }

    if (removed > 0) {
        CHAOS_IL2CPP_LOG_DEBUG_M("ThinLockTable", "drain_domain id={0} removed={1}",
            domain_id, removed);
    }
}

}  // namespace chaos::il2cpp::runtime_core
