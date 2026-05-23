#ifndef CHAOS_IL2CPP_THIN_LOCK_TABLE_H_
#define CHAOS_IL2CPP_THIN_LOCK_TABLE_H_

// ThinLockTable — global stripe hash table for thin-lock state
//
// Replaces the per-object sync_state field in ThinLockableHeader (16B → 8B).
// Lock state is stored in a 64-way striped hash table keyed by object pointer.
// Only objects that have been locked (<5%) have entries.
//
// Encoding (same as current sync_state in sync_mutex.cpp):
//   bits[0]     = locked
//   bits[1]     = inflated
//   bits[31:2]  = thread_id (30 bits)
//   bits[63:32] = recursion (32 bits)
//
// GC interface:
//   GetLockedObjects() — returns span of locked object pointers for pinning
//   RemoveObject() — called after GC relocation to re-key (or after unlock)

#include <chaos/native_types.h>
#include <cstdint>
#include <cstddef>

namespace chaos::il2cpp::runtime_core {

// Forward declarations
struct SyncBlock;

// ── Thin lock entry state (same encoding as current sync_state) ────────────
constexpr uint64_t kThinLockedBit      = 1ull << 0;
constexpr uint64_t kThinInflatedBit    = 1ull << 1;
constexpr uint64_t kThinThreadShift    = 2;
constexpr uint64_t kThinRecursionShift = 32;

// ── Thin lock table (64-way striped) ──────────────────────────────────────
constexpr uint32_t kThinLockStripes = 64;

/// Maximum number of pre-allocated thin lock entries.
/// Each entry is 16 bytes (pointer + value) plus hash table overhead.
/// 1024 entries ≈ ~32KB overhead — negligible.
constexpr uint32_t kThinLockEntryPoolSize = 1024;

class ThinLockTable {
public:
    /// Try to acquire the thin lock. Returns true if acquired.
    /// On failure (contention): returns false so caller can inflate.
    bool TryLock(void* obj, int32_t tid) noexcept;

    /// Release the thin lock. Returns true if fully released.
    /// Returns false if still held (recursion > 0) or already inflated.
    bool Unlock(void* obj, int32_t tid) noexcept;

    /// Try to enter (non-blocking). Returns true if acquired.
    bool TryEnter(void* obj, int32_t tid) noexcept;

    /// Check if the current thread holds the lock.
    bool IsEntered(void* obj, int32_t tid) noexcept;

    /// Read the current sync value for an object.
    /// Used by MonitorWait to snapshot before inflation.
    uint64_t ReadSyncValue(void* obj) noexcept;

    /// Inflate: remove thin-lock entry and return the SyncBlock.
    /// Returns nullptr if already inflated by another thread.
    SyncBlock* Inflate(void* obj, SyncBlock* sb) noexcept;

    /// Remove an entry (called during MonitorWait inflation).
    void RemoveEntry(void* obj) noexcept;

    // ── GC interface ──────────────────────────────────────────────────

    /// Return the number of currently locked objects (for pinning).
    uint32_t GetLockedCount() noexcept;

    /// Copy locked object pointers into @a out buffer (up to capacity).
    /// Returns the number written.
    uint32_t GetLockedObjects(void** out, uint32_t capacity) noexcept;

    /// Check if a specific object is currently locked (has a table entry).
    bool IsLocked(const void* obj) noexcept;

    /// Re-key an entry after GC relocation.
    /// @a old_addr is the pre-GC address, @a new_addr is the post-GC address.
    /// No-op if the object is not in the table.
    void RelocateEntry(void* old_addr, void* new_addr) noexcept;

    /// Remove all entries for objects in a given domain (domain unload).
    /// Returns the number of entries removed.
    uint32_t DrainForDomain(uint32_t domain_id) noexcept;

    /// Singleton access.
    static ThinLockTable& Instance() noexcept;

private:
    ThinLockTable() noexcept;
    ~ThinLockTable() = default;
    ThinLockTable(const ThinLockTable&) = delete;
    ThinLockTable& operator=(const ThinLockTable&) = delete;

    struct Stripe {
        CHAOS_IL2CPP_MUTEX                      mutex;
        CHAOS_IL2CPP_UNORDERED_DENSE_MAP(uint64_t, uint64_t) entries;  // key = ptr_as_uint64, value = sync_state
    };

    Stripe stripes_[kThinLockStripes];

    inline uint32_t StripeIndex(void* obj) const noexcept {
        return (reinterpret_cast<uintptr_t>(obj) >> 3) % kThinLockStripes;
    }
};

}  // namespace chaos::il2cpp::runtime_core

#endif // CHAOS_IL2CPP_THIN_LOCK_TABLE_H_
