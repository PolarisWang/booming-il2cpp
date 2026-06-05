#ifndef CHAOS_IL2CPP_PROFILE_H_
#define CHAOS_IL2CPP_PROFILE_H_

// ── High-performance RAII profiler using RDTSC ────────────────────────────
//
// Zero per-invocation I/O: accumulates in-memory during hot loops,
// dumps all results in one shot via stderr on command.
//
// Thread-safe: uses per-thread accumulators (thread_local) so the hot path
// (PROFILE_SCOPE) involves zero shared state.  Dump/Reset aggregate across
// all registered threads via lock-free thread registry.
//
// Compile-time toggle: define CHAOS_IL2CPP_PROFILE_ENABLED=1 to enable.
// When disabled (default), all macros expand to no-ops.
//
// Usage:
//   #define CHAOS_IL2CPP_PROFILE_ENABLED 1
//   #include "chaos/profile.h"
//
//   void foo() {
//       CHAOS_IL2CPP_PROFILE_SCOPE("MySection");
//       // ... work ...
//   }
//
//   // After benchmark loop:
//   CHAOS_IL2CPP_PROFILE_DUMP();
//
// Per-invocation cost: ~30 cycles (2× RDTSC + integer add + min/max update).
// Thread safety: fully thread-safe on the hot path (per-thread data).
//               Dump/Reset are expected to be called from one thread.
//
// Optimizations:
//   - Slot lookup uses open-addressing hash table (FNV-1a) instead of O(n) scan
//   - Nested scopes tracked via thread_local depth for hierarchical dump
//   - RDTSC calibrated to nanoseconds via QueryPerformanceFrequency on first use
//   - Per-thread data eliminates all cross-thread contention on hot path

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <atomic>
#include <new>
#if defined(_MSC_VER) && (defined(__x86_64__) || defined(_M_AMD64))
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#elif defined(__x86_64__) || defined(__i386__)
#include <x86intrin.h>
#elif defined(__aarch64__)
// ARM64: use __builtin_readcyclecounter() from GCC/Clang
// (maps to cntvct_el0 via mrs instruction)
#include <sched.h>
#endif

// Expansion helper: ensures __LINE__ and other special macros expand
// before token pasting.  Without this, MSVC leaves __LINE__ unexpanded
// inside a ## helper, producing duplicate identifiers like
// __profile_scope___LINE__ instead of __profile_scope_156.
#define CHAOS_IL2CPP_PROFILE_JOIN_EXPAND(a, b) CHAOS_IL2CPP_PROFILE_JOIN(a, b)
#define CHAOS_IL2CPP_PROFILE_JOIN(a, b) a ## b

// ── Compile-time toggle ──────────────────────────────────────────────────
#ifndef CHAOS_IL2CPP_PROFILE_ENABLED
#define CHAOS_IL2CPP_PROFILE_ENABLED 0
#endif

#if CHAOS_IL2CPP_PROFILE_ENABLED

// Read current processor number via x64 KPCR (single instruction, no windows.h).
// This is the offset of ProcessorNumber in the KPCR on x64 Windows.
#if defined(_M_AMD64)
#define CHAOS_IL2CPP_CURRENT_CORE()  static_cast<uint32_t>(__readgsdword(0x20))
#elif defined(__x86_64__) || defined(__amd64__) || defined(__aarch64__)
// Linux x86_64 / ARM64: sched_getcpu() via vDSO (~20ns) is acceptable for a profiler.
// Constructor/destructor pair adds ~40ns — negligible compared to scope body.
#include <sched.h>
#define CHAOS_IL2CPP_CURRENT_CORE()  static_cast<uint32_t>(sched_getcpu())
#else
#error "profile.h: CHAOS_IL2CPP_CURRENT_CORE not implemented for this platform"
#endif

// ── Portable cycle counter ──────────────────────────────────
#if defined(__aarch64__)
// ARM64: __builtin_readcyclecounter() maps to cntvct_el0 (mrs instruction).
#define CHAOS_IL2CPP_RDTSC()  __builtin_readcyclecounter()
#elif defined(__x86_64__) || defined(__i386__) || defined(_M_AMD64)
#define CHAOS_IL2CPP_RDTSC()  __rdtsc()
#else
#error "profile.h: CHAOS_IL2CPP_RDTSC not implemented for this platform"
#endif

namespace chaos::il2cpp::common {

// ── Config ───────────────────────────────────────────────
constexpr int kProfileMaxSlots   = 128;
constexpr int kProfileHashSize   = 256;  // 2× for low collision rate
constexpr int kProfileMaxThreads = 64;   // max concurrent threads tracked

// ── Per-slot accumulator ─────────────────────────────────
struct ProfileSlot {
    const char* name;
    uint64_t    total_cycles;
    uint64_t    call_count;
    uint64_t    min_cycles;
    uint64_t    max_cycles;
};

// ── Hash table entry (open addressing) ───────────────────
struct HashEntry {
    const char* name;     // nullptr = empty slot
    uint32_t    name_hash;
    int         slot_index;
};

// ── Per-thread profile data ──────────────────────────────
// Each thread gets its own instance (thread_local).  The hot path
// (ProfileScope construct/destruct) touches only this struct and never
// shares state with other threads.
struct alignas(64) ThreadProfileData {
    ProfileSlot slots[kProfileMaxSlots];
    HashEntry   hash_table[kProfileHashSize];
    int         slot_count{0};
    int         depth{0};
    int         registration_slot{-1};  // index in g_profile_threads[], -1 = unregistered
};

// ── Global thread registry (lock-free, write-once per thread) ────
// Defined in profile_globals.cpp.
extern thread_local ThreadProfileData g_tls_profile;
extern std::atomic<ThreadProfileData*> g_profile_threads[kProfileMaxThreads];
extern std::atomic<int> g_profile_thread_count;

// Retired thread data node: preserves a thread's profile data after
// the thread exits so that ProfileDump still sees its accumulators.
// Linked into a lock-free singly-linked list (retired_list_head).
struct RetiredProfileNode {
    ThreadProfileData data;
    RetiredProfileNode* next{nullptr};
};
extern std::atomic<RetiredProfileNode*> g_retired_profile_head;

// Thread registration — called once per thread on first PROFILE_SCOPE use.
//
// Thread unregistration — preserves the accumulated data in the retired
// list so that ProfileDump still sees it after thread exit, then clears
// the live registry entry to avoid dangling-pointer dereference.
inline void UnregisterThread(ThreadProfileData& data) noexcept {
    int slot = data.registration_slot;
    if (slot >= 0 && slot < kProfileMaxThreads) {
        // Snapshot into retired list before clearing the live entry.
        auto* node = new (std::nothrow) RetiredProfileNode();
        if (node != nullptr) {
            std::memcpy(&node->data, &data, sizeof(ThreadProfileData));
            node->data.registration_slot = -1;  // not re-registered
            // Lock-free push to retired list head.
            RetiredProfileNode* expected = g_retired_profile_head.load(std::memory_order_acquire);
            do {
                node->next = expected;
            } while (!g_retired_profile_head.compare_exchange_weak(
                expected, node, std::memory_order_release, std::memory_order_acquire));
        }
        g_profile_threads[slot].store(nullptr, std::memory_order_release);
        data.registration_slot = -1;
        // Decrement the global count so RegisterThread's fetch_add does not
        // drift toward overflow across scenario restarts / thread-pool recycling.
        g_profile_thread_count.fetch_sub(1, std::memory_order_relaxed);
    }
}
inline void RegisterThread(ThreadProfileData& data) noexcept {
    int idx = g_profile_thread_count.fetch_add(1, std::memory_order_relaxed);
    if (idx < kProfileMaxThreads) {
        data.registration_slot = idx;
        g_profile_threads[idx].store(&data, std::memory_order_release);
    } else {
        // Thread exceeded the fixed-size registration table.  Give the count
        // back so g_profile_thread_count stays bounded to kProfileMaxThreads.
        g_profile_thread_count.fetch_sub(1, std::memory_order_relaxed);
    }
}

// ── FNV-1a hash ─────────────────────────────────────────
inline uint32_t HashName(const char* name) noexcept {
    uint32_t hash = 2166136261u;
    while (*name) {
        hash ^= static_cast<uint8_t>(*name++);
        hash *= 16777619u;
    }
    return hash;
}

// ── Find or create slot by name (hash-accelerated) ──────
// Operates on per-thread data @a data — no shared state.
inline int FindOrCreateSlot(const char* name, ThreadProfileData& data) noexcept {
    uint32_t hash = HashName(name);
    int start = static_cast<int>(hash % kProfileHashSize);

    for (int i = 0; i < kProfileHashSize; ++i) {
        int idx = (start + i) % kProfileHashSize;
        auto& entry = data.hash_table[idx];

        if (entry.name == nullptr) {
            // Empty slot — create new entry (single-threaded, no race).
            int slot_idx = data.slot_count++;
            if (slot_idx < kProfileMaxSlots) {
                data.slots[slot_idx].name        = name;
                data.slots[slot_idx].total_cycles = 0;
                data.slots[slot_idx].call_count   = 0;
                data.slots[slot_idx].min_cycles   = ~0ULL;
                data.slots[slot_idx].max_cycles   = 0;
                entry.name       = name;
                entry.name_hash  = hash;
                entry.slot_index = slot_idx;
            }
            return (slot_idx < kProfileMaxSlots) ? slot_idx : 0;
        }

        // Hash match → verify with pointer compare (fast) or strcmp (cross-TU).
        if (entry.name_hash == hash &&
            (entry.name == name || std::strcmp(entry.name, name) == 0)) {
            return entry.slot_index;
        }
    }
    return 0;  // table full
}

// ── RDTSC → ns calibration ──────────────────────────────
extern double g_ns_per_cycle;
extern bool   g_profile_calibrated;
void CalibrateProfileTsc() noexcept;

// ── RAII scope timer ─────────────────────────────────────
class ProfileScope {
    int      slot_idx_;
    uint64_t start_;
    uint32_t core_id_;
public:
    explicit ProfileScope(const char* name) noexcept
        : slot_idx_(FindOrCreateSlot(name, g_tls_profile))
        , start_(CHAOS_IL2CPP_RDTSC())
        , core_id_(CHAOS_IL2CPP_CURRENT_CORE()) {
        // Register this thread in the global registry on first use.
        if (g_tls_profile.registration_slot < 0) {
            RegisterThread(g_tls_profile);
        }
        ++g_tls_profile.depth;
    }

    ~ProfileScope() noexcept {
        --g_tls_profile.depth;
        // Discard sample if thread migrated to a different core — RDTSC
        // deltas across cores can be wildly inaccurate (up to ms-level skew).
        if (CHAOS_IL2CPP_CURRENT_CORE() != core_id_) return;

        uint64_t elapsed = CHAOS_IL2CPP_RDTSC() - start_;
        auto& slot = g_tls_profile.slots[slot_idx_];
        slot.total_cycles += elapsed;
        ++slot.call_count;
        if (elapsed < slot.min_cycles) slot.min_cycles = elapsed;
        if (elapsed > slot.max_cycles) slot.max_cycles = elapsed;
    }
};

// ── Dump accumulated profile data (hierarchical + ns) ────
inline void ProfileDump() noexcept {
    // Lazy TSC calibration on first dump (avoid Sleep(1) in hot path).
    if (!g_profile_calibrated) {
        CalibrateProfileTsc();
    }

    std::fprintf(stderr, "\n=== PROFILE DUMP ===\n");
    double ns_per = (g_ns_per_cycle > 0.0) ? g_ns_per_cycle : 1.0;
    std::fprintf(stderr, "PROFILE|CALIBRATION|ns_per_cycle=%.6f\n", ns_per);

    // Per-thread data, registered via atomic count.
    // Each thread has its own slot indices; we aggregate by name.
    int thread_count = g_profile_thread_count.load(std::memory_order_acquire);
    if (thread_count > kProfileMaxThreads) thread_count = kProfileMaxThreads;

    // Temporary aggregation table (name-keyed, O(n*m) — fine for dump).
    struct AggSlot { const char* name; uint64_t total; uint64_t count; uint64_t minv; uint64_t maxv; };
    AggSlot agg[kProfileMaxSlots]{};
    int agg_count = 0;

    for (int ti = 0; ti < thread_count; ++ti) {
        auto* data = g_profile_threads[ti].load(std::memory_order_acquire);
        if (data == nullptr) continue;

        for (int si = 0; si < data->slot_count; ++si) {
            const auto& src = data->slots[si];
            if (src.name == nullptr || src.call_count == 0) continue;

            // Find or create in aggregation table.
            int ai = 0;
            for (; ai < agg_count; ++ai) {
                if (agg[ai].name == src.name ||
                    std::strcmp(agg[ai].name, src.name) == 0) break;
            }
            if (ai == agg_count) {
                if (ai >= kProfileMaxSlots) break;  // aggregation full
                agg[ai].name  = src.name;
                agg[ai].total = 0;
                agg[ai].count = 0;
                agg[ai].minv  = ~0ULL;
                agg[ai].maxv  = 0;
                ++agg_count;
            }
            agg[ai].total += src.total_cycles;
            agg[ai].count += src.call_count;
            if (src.min_cycles < agg[ai].minv) agg[ai].minv = src.min_cycles;
            if (src.max_cycles > agg[ai].maxv) agg[ai].maxv = src.max_cycles;
        }
    }

    // Aggregate data from retired (exited) threads, then free the nodes
    // to prevent unbounded memory growth in long-lived processes.
    auto* retired = g_retired_profile_head.exchange(nullptr, std::memory_order_acq_rel);
    for (auto* node = retired; node != nullptr; node = node->next) {
        for (int si = 0; si < node->data.slot_count; ++si) {
            const auto& src = node->data.slots[si];
            if (src.name == nullptr || src.call_count == 0) continue;

            int ai = 0;
            for (; ai < agg_count; ++ai) {
                if (agg[ai].name == src.name ||
                    std::strcmp(agg[ai].name, src.name) == 0) break;
            }
            if (ai == agg_count) {
                if (ai >= kProfileMaxSlots) break;
                agg[ai].name  = src.name;
                agg[ai].total = 0;
                agg[ai].count = 0;
                agg[ai].minv  = ~0ULL;
                agg[ai].maxv  = 0;
                ++agg_count;
            }
            agg[ai].total += src.total_cycles;
            agg[ai].count += src.call_count;
            if (src.min_cycles < agg[ai].minv) agg[ai].minv = src.min_cycles;
            if (src.max_cycles > agg[ai].maxv) agg[ai].maxv = src.max_cycles;
        }
    }
    // Free retired node list after aggregation.
    while (retired != nullptr) {
        auto* next = retired->next;
        delete retired;
        retired = next;
    }

    for (int i = 0; i < agg_count; ++i) {
        const auto& s = agg[i];
        if (s.count == 0) continue;
        double avg_cyc = static_cast<double>(s.total) / s.count;
        double avg_ns  = avg_cyc * ns_per;
        double total_ns = static_cast<double>(s.total) * ns_per;
        std::fprintf(stderr,
            "PROFILE|%s|avg=%.0f|avg_ns=%.0f|min=%llu|max=%llu|count=%llu|total_ns=%.0f\n",
            s.name, avg_cyc, avg_ns,
            static_cast<unsigned long long>(s.minv),
            static_cast<unsigned long long>(s.maxv),
            static_cast<unsigned long long>(s.count),
            total_ns);
    }
    std::fprintf(stderr, "=== PROFILE END ===\n");
    std::fflush(stderr);
}

// ── Reset all accumulators across all threads ────────────
inline void ProfileReset() noexcept {
    int thread_count = g_profile_thread_count.load(std::memory_order_acquire);
    if (thread_count > kProfileMaxThreads) thread_count = kProfileMaxThreads;

    for (int ti = 0; ti < thread_count; ++ti) {
        auto* data = g_profile_threads[ti].load(std::memory_order_acquire);
        if (data == nullptr) continue;

        // Clear slots but keep registration_slot intact so the thread
        // doesn't need to re-register on the next scope.
        data->slot_count = 0;
        data->depth      = 0;
        for (int i = 0; i < kProfileMaxSlots; ++i) {
            data->slots[i].name        = nullptr;
            data->slots[i].total_cycles = 0;
            data->slots[i].call_count   = 0;
            data->slots[i].min_cycles   = 0;
            data->slots[i].max_cycles   = 0;
        }
        for (int i = 0; i < kProfileHashSize; ++i) {
            data->hash_table[i].name       = nullptr;
            data->hash_table[i].name_hash  = 0;
            data->hash_table[i].slot_index = -1;
        }
    }

    // Reset registry — threads will re-register on next use.
    g_profile_thread_count.store(0, std::memory_order_release);
    for (int ti = 0; ti < kProfileMaxThreads; ++ti) {
        g_profile_threads[ti].store(nullptr, std::memory_order_release);
    }

    // Re-register the calling thread on next PROFILE_SCOPE usage.
    // Without this, g_tls_profile still has its old registration_slot
    // (e.g. 0) but g_profile_threads[0] is now nullptr — data would
    // accumulate in TLS but be invisible to ProfileDump.
    g_tls_profile.registration_slot = -1;

    // Free retired nodes — data is intentionally lost on explicit reset.
    while (auto* old_head = g_retired_profile_head.exchange(nullptr,
            std::memory_order_acq_rel)) {
        while (old_head != nullptr) {
            auto* next = old_head->next;
            delete old_head;
            old_head = next;
        }
    }
}

} // namespace chaos::il2cpp::common

#define CHAOS_IL2CPP_PROFILE_SCOPE(name)                                  \
    ::chaos::il2cpp::common::ProfileScope                                   \
        CHAOS_IL2CPP_PROFILE_JOIN_EXPAND(__profile_scope_, __LINE__)(name)

#define CHAOS_IL2CPP_PROFILE_DUMP()                                       \
    ::chaos::il2cpp::common::ProfileDump()

#define CHAOS_IL2CPP_PROFILE_RESET()                                      \
    ::chaos::il2cpp::common::ProfileReset()

#else // CHAOS_IL2CPP_PROFILE_ENABLED == 0

// ── No-op stubs: compiler eliminates all profile code ─────────────────────
namespace chaos::il2cpp::common {
struct NullProfileScope {
    // constexpr + (void)name avoids embedding the string literal in .rdata
    // when profiling is disabled.  The compiler fully eliminates this.
    constexpr explicit NullProfileScope(const char* name) noexcept {
        (void)name;
    }
};

// Stub thread data — referenced by thread_state.cpp for UnregisterThread.
// When profiling is disabled, both Register/UnregisterThread are no-ops.
struct NullThreadProfileData { int registration_slot{-1}; };
inline thread_local NullThreadProfileData g_tls_profile{};
inline void UnregisterThread(NullThreadProfileData&) noexcept {}
inline void RegisterThread(NullThreadProfileData&) noexcept {}

// NullProfileScope constructor is constexpr → compiler eliminates
// both the object and the string argument entirely.
} // namespace chaos::il2cpp::common

#define CHAOS_IL2CPP_PROFILE_SCOPE(name)                                  \
    ::chaos::il2cpp::common::NullProfileScope                               \
        CHAOS_IL2CPP_PROFILE_JOIN_EXPAND(__prof_noop_, __LINE__)(name)

#define CHAOS_IL2CPP_PROFILE_DUMP()                                       \
    ((void)0)

#define CHAOS_IL2CPP_PROFILE_RESET()                                      \
    ((void)0)

#endif // CHAOS_IL2CPP_PROFILE_ENABLED

#endif // CHAOS_IL2CPP_PROFILE_H_
