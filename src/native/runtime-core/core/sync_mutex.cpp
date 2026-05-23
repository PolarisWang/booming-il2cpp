#include <chaos/unordered_dense.h>
#include <chaos/log.h>

#include "gc_region.h"
#include "thread_state.h"
#include "forbid_suspend.h"
#include "thin_lock_table.h"

namespace chaos::il2cpp::runtime_core {

// ── SyncBlock struct (must be at namespace scope to match the
// forward declaration in thin_lock_table.h) ───────────────────────────

struct SyncBlock {
    const TypeInfoHot*                     type_info = nullptr;
    CHAOS_IL2CPP_RECURSIVE_LOCK_MUTEX    mutex;
    std::condition_variable_any          cond;
    std::atomic<int32_t>                 owner_tid{0};  // ThreadId that owns mutex, 0 = none

    /// Number of times the owning thread has entered (reentrancy tracking
    /// for inflated SyncBlock path).  Incremented in MonitorEnter,
    /// decremented in MonitorExit.  The mutex is physically released only
    /// when recursion reaches 0.
    std::atomic<uint32_t>                recursion{0};

    /// Number of threads currently waiting on cond (MonitorWait).
    /// Used to gauge PulseAll fan-out.
    std::atomic<uint32_t>                wait_count{0};

    /// Monotonically increasing pulse generation counter.
    /// Incremented by MonitorPulseAll to notify_one and track which pulses
    /// have been delivered.  Each waiter snapshots pulse_count before
    /// waiting; after waking, if pulse_count has advanced, the waiter
    /// chains the signal by calling notify_one for the next waiter.
    std::atomic<uint32_t>                pulse_count{0};
};

namespace {

// Old SyncBlock stripe infrastructure — kept for DrainSyncBlocksForDomain
// (domain unload path).  New ThinLockTable-based code uses ThinLockTable::Inflate
// instead of g_sync_block_stripes.

constexpr uint32_t kSyncBlockStripes  = 64;

struct SyncBlockStripe {
    CHAOS_IL2CPP_MUTEX                              table_lock;
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(void*, SyncBlock*)   entries;
};

SyncBlockStripe g_sync_block_stripes[kSyncBlockStripes];

// SyncBlock pool — shared by both old and new inflation paths.
constexpr uint32_t kSyncBlockPoolSize = 128;
SyncBlock g_sync_block_pool[kSyncBlockPoolSize];
std::atomic<uint32_t> g_sync_block_pool_next{0};

bool IsLikelyMetadataTokenHandle(MethodInfoHandle method) {
    const CHAOS_IL2CPP_UINTPTR raw_method = static_cast<CHAOS_IL2CPP_UINTPTR>(method);
    if (raw_method == 0u) return false;
    if (sizeof(CHAOS_IL2CPP_UINTPTR) > sizeof(CHAOS_IL2CPP_UINT32) && (raw_method >> 32) != 0u) return false;
    return raw_method <= static_cast<CHAOS_IL2CPP_UINTPTR>(0x2BFFFFFFu);
}

}  // anonymous namespace

// ── AllocateSyncBlockFromPool ───────────────────────────────────────────────

SyncBlock* AllocateSyncBlockFromPool() noexcept {
    threading::ForbidSuspendScope forbid;
    uint32_t idx = g_sync_block_pool_next.fetch_add(1, std::memory_order_relaxed);
    if (idx < kSyncBlockPoolSize) {
        return &g_sync_block_pool[idx];
    }
    return new SyncBlock();
}

// ── InflateAndEnter (ThinLockTable path) ────────────────────────────────────

bool InflateAndEnter(void* obj) noexcept {
    auto* sb = AllocateSyncBlockFromPool();
    if (sb == nullptr) return false;

    auto& table = ThinLockTable::Instance();
    SyncBlock* actual = table.Inflate(obj, sb);

    // Lock the SyncBlock stored in the table and set ownership.
    // Whether we won the inflation race or another thread did, we
    // contend on the published SyncBlock.
    actual->mutex.lock();
    actual->owner_tid.store(threading::GetCurrentThreadId(), std::memory_order_relaxed);
    actual->recursion.store(1, std::memory_order_relaxed);

    // If our sb lost the race, it remains in the pool (never individually
    // freed — same leak semantics as old code; pool entries are reusable
    // in theory but the pool is only 128 entries deep).
    return true;
}

// ======================================================================
// DrainSyncBlocksForDomain — drain ThinLockTable entries for domain unload
// ======================================================================
void DrainSyncBlocksForDomain(CHAOS_IL2CPP_UINT32 domain_id) noexcept {
    // Delegates to ThinLockTable which now owns all lock state
    // (both thin and inflated entries). SyncBlock pool entries are never
    // individually freed — the pool is a fixed array + heap fallback.
    uint32_t removed = ThinLockTable::Instance().DrainForDomain(domain_id);
    if (removed > 0) {
        CHAOS_IL2CPP_LOG_DEBUG_M("SyncBlock", "drain_domain id={0} removed={1}",
            domain_id, removed);
    }
}

}  // namespace chaos::il2cpp::runtime_core
