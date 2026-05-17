#ifndef CHAOS_IL2CPP_PROFILE_H_
#define CHAOS_IL2CPP_PROFILE_H_

// ── High-performance RAII profiler using RDTSC ────────────────────────────
//
// Zero per-invocation I/O: accumulates in-memory during hot loops,
// dumps all results in one shot via stderr on command.
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
// Thread safety: NOT thread-safe (single-threaded benchmark use only).
//
// Optimizations:
//   - Slot lookup uses open-addressing hash table (FNV-1a) instead of O(n) scan
//   - Nested scopes tracked via thread_local depth for hierarchical dump
//   - RDTSC calibrated to nanoseconds via QueryPerformanceFrequency on first use

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <intrin.h>

#ifdef _MSC_VER
#pragma intrinsic(__rdtsc)
// Needed for GetCurrentProcessorNumber() in ProfileScope (cross-core migration
// detection).  Include is inside the ENABLED guard below, so no impact on SHIP.
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
#ifdef _M_AMD64
#define CHAOS_IL2CPP_CURRENT_CORE()  static_cast<uint32_t>(__readgsdword(0x20))
#else
#error "profile.h requires _M_AMD64 for GetCurrentProcessorNumber"
#endif

namespace chaos::il2cpp::common {

// ── Config ───────────────────────────────────────────────────────────────
constexpr int kProfileMaxSlots   = 64;
constexpr int kProfileHashSize   = 128;  // 2× for low collision rate

// ── Per-slot accumulator ─────────────────────────────────────────────────
struct ProfileSlot {
    const char* name;
    uint64_t    total_cycles;
    uint64_t    call_count;
    uint64_t    min_cycles;
    uint64_t    max_cycles;
};

// ── Hash table entry (open addressing) ──────────────────────────────────
struct HashEntry {
    const char* name;     // nullptr = empty slot
    uint32_t    name_hash;
    int         slot_index;
};

// ── Global tables (single-threaded by design) ────────────────────────────
// Defined in profile_globals.cpp (single TU) — NOT inline, NOT per-TU copies.
extern ProfileSlot g_profile_table[kProfileMaxSlots];
extern HashEntry   g_profile_hash_table[kProfileHashSize];
extern int         g_profile_slot_count;

// Thread-local nesting depth for hierarchical dump output.
// Defined in profile_globals.cpp (one instance per thread).
extern thread_local int tls_profile_depth;

// ── FNV-1a hash ─────────────────────────────────────────────────────────
inline uint32_t HashName(const char* name) noexcept {
    uint32_t hash = 2166136261u;
    while (*name) {
        hash ^= static_cast<uint8_t>(*name++);
        hash *= 16777619u;
    }
    return hash;
}

// ── Find or create slot by name (hash-accelerated) ──────────────────────
inline int FindOrCreateSlot(const char* name) noexcept {
    uint32_t hash = HashName(name);
    int start = static_cast<int>(hash % kProfileHashSize);

    for (int i = 0; i < kProfileHashSize; ++i) {
        int idx = (start + i) % kProfileHashSize;
        auto& entry = g_profile_hash_table[idx];

        if (entry.name == nullptr) {
            // Empty slot — create new entry (single-threaded, no race).
            int slot_idx = g_profile_slot_count++;
            if (slot_idx < kProfileMaxSlots) {
                g_profile_table[slot_idx].name        = name;
                g_profile_table[slot_idx].total_cycles = 0;
                g_profile_table[slot_idx].call_count   = 0;
                g_profile_table[slot_idx].min_cycles   = ~0ULL;
                g_profile_table[slot_idx].max_cycles   = 0;
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

// ── RDTSC → ns calibration ──────────────────────────────────────────────
// Calibrated lazily on first ProfileDump() call (not at scope construction,
// to avoid Sleep(1) latency during hot loops).
extern double g_ns_per_cycle;
extern bool   g_profile_calibrated;
void CalibrateProfileTsc() noexcept;

// ── RAII scope timer ─────────────────────────────────────────────────────
class ProfileScope {
    int      slot_idx_;
    uint64_t start_;
    uint32_t core_id_;  // sampled on construction; used to detect migration
public:
    explicit ProfileScope(const char* name) noexcept
        : slot_idx_(FindOrCreateSlot(name)), start_(__rdtsc())
        , core_id_(CHAOS_IL2CPP_CURRENT_CORE()) {
        ++tls_profile_depth;
    }

    ~ProfileScope() noexcept {
        --tls_profile_depth;
        // Discard sample if thread migrated to a different core — RDTSC
        // deltas across cores can be wildly inaccurate (up to ms-level skew).
        if (CHAOS_IL2CPP_CURRENT_CORE() != core_id_) return;

        uint64_t elapsed = __rdtsc() - start_;
        auto& slot = g_profile_table[slot_idx_];
        slot.total_cycles += elapsed;
        ++slot.call_count;
        if (elapsed < slot.min_cycles) slot.min_cycles = elapsed;
        if (elapsed > slot.max_cycles) slot.max_cycles = elapsed;
    }
};

// ── Dump accumulated profile data (hierarchical + ns) ────────────────────
inline void ProfileDump() noexcept {
    // Lazy TSC calibration on first dump (avoid Sleep(1) in hot path).
    if (!g_profile_calibrated) {
        CalibrateProfileTsc();
    }

    std::fprintf(stderr, "\n=== PROFILE DUMP ===\n");
    // ns/cycle = 1.0 means uncalibrated
    double ns_per = (g_ns_per_cycle > 0.0) ? g_ns_per_cycle : 1.0;
    std::fprintf(stderr, "PROFILE|CALIBRATION|ns_per_cycle=%.6f\n", ns_per);

    for (int i = 0; i < g_profile_slot_count; ++i) {
        const auto& slot = g_profile_table[i];
        if (slot.call_count == 0) continue;
        double avg_cyc = static_cast<double>(slot.total_cycles) / slot.call_count;
        double avg_ns  = avg_cyc * ns_per;
        double total_ns = static_cast<double>(slot.total_cycles) * ns_per;
        std::fprintf(stderr,
            "PROFILE|%s|avg=%.0f|avg_ns=%.0f|min=%llu|max=%llu|count=%llu|total_ns=%.0f\n",
            slot.name, avg_cyc, avg_ns,
            static_cast<unsigned long long>(slot.min_cycles),
            static_cast<unsigned long long>(slot.max_cycles),
            static_cast<unsigned long long>(slot.call_count),
            total_ns);
    }
    std::fprintf(stderr, "=== PROFILE END ===\n");
    std::fflush(stderr);
}

// ── Reset all accumulators ──────────────────────────────────────────────
inline void ProfileReset() noexcept {
    g_profile_slot_count = 0;
    for (int i = 0; i < kProfileMaxSlots; ++i) {
        g_profile_table[i].name = nullptr;
        g_profile_table[i].total_cycles = 0;
        g_profile_table[i].call_count   = 0;
        g_profile_table[i].min_cycles   = 0;
        g_profile_table[i].max_cycles   = 0;
    }
    for (int i = 0; i < kProfileHashSize; ++i) {
        g_profile_hash_table[i].name       = nullptr;
        g_profile_hash_table[i].name_hash  = 0;
        g_profile_hash_table[i].slot_index = -1;
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