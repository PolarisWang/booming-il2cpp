#ifndef CHAOS_IL2CPP_COMMON_UNORDERED_DENSE_H_
#define CHAOS_IL2CPP_COMMON_UNORDERED_DENSE_H_

// ── ankerl::unordered_dense::map wrapper ──────────────────────────────────
//
// A fast, densely-stored hash map based on robin-hood hashing with backward
// shift deletion.  Provides ~2-10x performance over std::unordered_map for
// most operations (insert, lookup, iteration, copy) at the cost of iterator
// and pointer instability.
//
// Version: 4.8.1
// Source: https://github.com/martinus/unordered_dense
//
// Usage:
//   #include <chaos/unordered_dense.h>
//
//   CHAOS_IL2CPP_UNORDERED_DENSE_MAP(CHAOS_IL2CPP_UINT64, CHAOS_IL2CPP_INTPTR) my_map;
//   my_map.reserve(128);
//   my_map[42] = some_ptr;
//
// Benchmark (MSVC 2022 /O2, N=100k):
//   Insert (with reserve):  0.11x of std::unordered_map  (dense wins 9x)
//   Lookup 100% hit:        0.23x                        (dense wins 4x)
//   Iteration:              0.07x                        (dense wins 14x)
//   Copy construction:      0.11x                        (dense wins 9x)
//   Erase all:              0.39x                        (dense wins 2.5x)
//   Pathological hash:      1.04x                        (tie)
//   Full report: tests/perf/unordered-map-bench/benchmark-report.md
//
// CRITICAL LIMITATIONS — read before use:
//
//   1. Iterator invalidation on rehash
//      Unlike std::unordered_map, any insert that triggers a rehash
//      invalidates ALL iterators and references.  Do NOT hold iterators
//      across insert calls.
//
//   2. Pointer / reference instability
//      Insert and erase can move elements in memory.  Pointers and
//      references to map elements are NOT stable across mutations.
//      Only use value types (or store pointers by value, e.g. raw ptrs).
//
//   3. Heterogeneous lookup NOT supported
//      std::unordered_map supports transparent hash (C++20) for
//      heterogeneous lookup (e.g. find("key"sv) on a map<string, T>).
//      unordered_dense does NOT.  Always use the exact key type.
//
//   4. Memory overhead
//      unordered_dense uses open addressing (~1 slot per entry, plus
//      max load factor ~0.875).  Memory is contiguous, unlike
//      std::unordered_map's chained buckets.  This is usually better
//      for cache locality but means the entire table is one allocation.
//
//   5. Hash function quality matters
//      Open addressing degrades with poor hash functions.  If hash is
//      constant (worst case), performance collapses to O(n) — same as
//      std::unordered_map.  For custom types, use std::hash or a
//      well-distributed hash.
//
//   6. Not suitable for:
//      - Code paths that require iterator stability across insert/erase
//      - Structures holding pointers/references to map elements
//      - Heterogeneous lookup patterns
//      - Memory-mapped / placement-new scenarios
//
//   7. Ideal for:
//      - High-frequency lookup tables (type->handler maps)
//      - Read-heavy workloads with batch inserts
//      - Copy-heavy patterns (copy is ~9x faster than std::unordered_map)
//      - Iteration-heavy patterns (contiguous memory ~14x faster)
//      - Memory-constrained environments (no per-node allocation overhead)
//
// ============================================================================

#include <ankerl/unordered_dense.h>

#include <cstdint>
#include <type_traits>

namespace chaos::il2cpp::common {

/// Identity-biased hash with lightweight avalanche for integer/enum types.
///
/// For **pointer keys**: returns the raw address (identity). Pointers from ASLR
/// have high entropy in their upper bits, which is exactly what ankerl uses for
/// bucket indexing (hash >> shifts), so no mixing is needed.
///
/// For **integer/enum keys**: applies SplitMix64 avalanche after the identity
/// cast. This ensures sequential keys (0, 1, 2...) produce well-distributed
/// upper bits, avoiding the catastrophic collapse where consecutive IDs all
/// map to bucket 0 under pure identity + is_avalanching.
///
/// Marked with is_avalanching so ankerl skips its internal wyhash mixing.
///
/// Suitable for:
///   - Pointer keys (heap-allocated, ASLR-distributed)
///   - Integer keys with any distribution (sequential, random, sparse)
///   - Already-hashed keys (FNV-1a output, bit-packed composites)
///
/// NOT suitable for:
///   - String keys (identity on address hashes the pointer, not content)
///   - Keys where the full 64-bit value has <8 bits of entropy
struct identity_hash {
    using is_avalanching = void;

    /// Integral types: identity + SplitMix64 avalanche.
    /// Without avalanche, sequential keys collapse to O(n) because ankerl
    /// extracts the bucket index from the upper bits of the hash.
    template <typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    auto operator()(T v) const noexcept -> uint64_t {
        auto x = static_cast<uint64_t>(static_cast<std::make_unsigned_t<T>>(v));
        // SplitMix64 mixing — lightweight avalanche, ~5-10 cycles on modern CPUs.
        x ^= x >> 30;
        x *= UINT64_C(0xbf58476d1ce4e5b9);
        x ^= x >> 27;
        x *= UINT64_C(0x94d049bb133111eb);
        x ^= x >> 31;
        return x;
    }

    /// Enum types: same as integral (cast to underlying type first).
    template <typename T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
    auto operator()(T v) const noexcept -> uint64_t {
        using U = std::underlying_type_t<T>;
        return operator()(static_cast<U>(v));
    }

    /// Pointer types: raw address — upper bits are ASLR-random so no
    /// avalanche needed.  This is the main performance advantage over
    /// the default ankerl hash (which applies wyhash on the pointer).
    template <typename T>
    auto operator()(T* ptr) const noexcept -> uint64_t {
        return reinterpret_cast<uint64_t>(ptr);
    }
};

} // namespace chaos::il2cpp::common

#define CHAOS_IL2CPP_UNORDERED_DENSE_MAP(K, V)            ankerl::unordered_dense::map<K, V>
#define CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(K, V)   ankerl::unordered_dense::map<K, V, chaos::il2cpp::common::identity_hash>
#define CHAOS_IL2CPP_UNORDERED_DENSE_SET(K)               ankerl::unordered_dense::set<K>

#endif // CHAOS_IL2CPP_COMMON_UNORDERED_DENSE_H_
