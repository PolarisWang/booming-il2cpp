namespace chaos::il2cpp::runtime_core {
namespace {

// ── Atomic helpers for uint64_t (sync_state) ──────────────────────────
#if defined(_MSC_VER)
    #include <intrin.h>
    #pragma intrinsic(_InterlockedCompareExchange64, _InterlockedExchange64)
    #define __ATOMIC_RELAXED 0
    #define __ATOMIC_ACQUIRE 0
    #define __ATOMIC_RELEASE 0
    inline uint64_t AtomicLoadRelaxed(const uint64_t* p) noexcept {
        return *const_cast<volatile uint64_t*>(p);
    }
    inline uint64_t AtomicLoadAcquire(const uint64_t* p) noexcept {
        uint64_t v = *const_cast<volatile uint64_t*>(p);
        _ReadWriteBarrier();
        return v;
    }
    inline void AtomicStoreRelease(uint64_t* p, uint64_t val) noexcept {
        _InterlockedExchange64(reinterpret_cast<volatile LONG64*>(p), static_cast<LONG64>(val));
    }
    inline void AtomicStoreRelaxed(uint64_t* p, uint64_t val) noexcept {
        *const_cast<volatile uint64_t*>(p) = val;
    }
    inline bool AtomicCAS(uint64_t* p, uint64_t& expected, uint64_t desired, int = 0, int = 0) noexcept {
        LONG64 prev = _InterlockedCompareExchange64(
            reinterpret_cast<volatile LONG64*>(p), static_cast<LONG64>(desired), static_cast<LONG64>(expected));
        if (prev == static_cast<LONG64>(expected)) return true;
        expected = static_cast<uint64_t>(prev);
        return false;
    }
#else
    inline uint64_t AtomicLoadRelaxed(const uint64_t* p) noexcept {
        return __atomic_load_n(p, __ATOMIC_RELAXED);
    }
    inline uint64_t AtomicLoadAcquire(const uint64_t* p) noexcept {
        return __atomic_load_n(p, __ATOMIC_ACQUIRE);
    }
    inline void AtomicStoreRelease(uint64_t* p, uint64_t val) noexcept {
        __atomic_store_n(p, val, __ATOMIC_RELEASE);
    }
    inline void AtomicStoreRelaxed(uint64_t* p, uint64_t val) noexcept {
        __atomic_store_n(p, val, __ATOMIC_RELAXED);
    }
    inline bool AtomicCAS(uint64_t* p, uint64_t& expected, uint64_t desired,
                          int success_order = __ATOMIC_ACQUIRE, int failure_order = __ATOMIC_RELAXED) noexcept {
        return __atomic_compare_exchange_n(p, &expected, desired, false, success_order, failure_order);
    }
#endif

// Thin lock / SyncBlock constants
constexpr uint64_t kSyncLockedBit     = 1ull << 0;
constexpr uint64_t kSyncInflatedBit   = 1ull << 1;
constexpr uint64_t kSyncThreadShift   = 2;
constexpr uint64_t kSyncRecursionShift = 32;

constexpr uint32_t kSyncBlockStripes  = 64;
constexpr uint32_t kSyncBlockSpinMax  = 1000;

struct SyncBlock {
    const TypeInfoHot*                     type_info = nullptr;
    CHAOS_IL2CPP_RECURSIVE_LOCK_MUTEX    mutex;
    std::condition_variable_any          cond;
};

struct SyncBlockStripe {
    CHAOS_IL2CPP_MUTEX                              table_lock;
    CHAOS_IL2CPP_UNORDERED_MAP(void*, SyncBlock*)   entries;
};

SyncBlockStripe g_sync_block_stripes[kSyncBlockStripes];

inline uint32_t SyncBlockStripeIndex(void* obj) noexcept {
    return (reinterpret_cast<uintptr_t>(obj) >> 3) % kSyncBlockStripes;
}

static bool InflateAndEnter(void* obj, uint64_t current_sync) noexcept {
    const uint32_t stripe_idx = SyncBlockStripeIndex(obj);
    auto& stripe = g_sync_block_stripes[stripe_idx];

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) guard(stripe.table_lock);

    auto* sync_ptr = GetSyncStatePtr(obj);
    uint64_t sync = *sync_ptr;
    if ((sync & kSyncInflatedBit) != 0) {
        const auto* sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
    if (sb != nullptr) {
        // O(1) lookup via unordered_map::find instead of O(n) linear scan.
        auto it = stripe.entries.find(obj);
        if (it != stripe.entries.end() && it->second != nullptr) {
            it->second->mutex.lock();
            return true;
        }
        return false;
    }
    }

    auto* sb = new SyncBlock();
    stripe.entries[obj] = sb;

    const uint64_t inflated_val = kSyncInflatedBit | reinterpret_cast<uint64_t>(sb);
    AtomicStoreRelease(sync_ptr, inflated_val);

    sb->mutex.lock();
    return true;
}

bool IsLikelyMetadataTokenHandle(MethodInfoHandle method) {
    const CHAOS_IL2CPP_UINTPTR raw_method = static_cast<CHAOS_IL2CPP_UINTPTR>(method);
    if (raw_method == 0u) return false;
    if (sizeof(CHAOS_IL2CPP_UINTPTR) > sizeof(CHAOS_IL2CPP_UINT32) && (raw_method >> 32) != 0u) return false;
    return raw_method <= static_cast<CHAOS_IL2CPP_UINTPTR>(0x2BFFFFFFu);
}

}  // anonymous namespace
}  // namespace chaos::il2cpp::runtime_core