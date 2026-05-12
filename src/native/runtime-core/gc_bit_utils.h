#ifndef CHAOS_IL2CPP_GC_BIT_UTILS_H_
#define CHAOS_IL2CPP_GC_BIT_UTILS_H_

#include <chaos/native_types.h>

#include <cstdint>

// ======================================================================
// gc_bit_utils.h — platform-portable CTZ, bitmap iteration, prefetch
//
// Provides the low-level bit-scanning primitives used by the mark-sweep
// collector's CTZ-based sweep (word-at-a-time bitmap scan) and by the
// parallel mark pass (atomic bitmap CAS).
//
// == CTZ (Count Trailing Zeros) ==
//   GcCtz64(word)       → index of lowest set bit, or 64 if word==0
//
// == Bitmap iteration ==
//   GcForEachSetBit(word, [&](int bit_idx) { ... })
//   GcForEachZeroBit(word, [&](int bit_idx) { ... })
//
// == Prefetch ==
//   CHAOS_IL2CPP_PREFETCH_READ(addr)  — prefetch for read
// ======================================================================

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// CTZ intrinsic
// ======================================================================

/// Returns the index of the lowest set bit in @a word, or 64 if @a word is 0.
inline int GcCtz64(uint64_t word) noexcept {
    if (word == 0) return 64;
#if defined(_MSC_VER) && !defined(__clang__)
    unsigned long index;
    _BitScanForward64(&index, word);
    return static_cast<int>(index);
#else
    return static_cast<int>(__builtin_ctzll(word));
#endif
}

// ======================================================================
// Bitmap iteration helpers
// ======================================================================

/// Invoke @a visitor(bit_index) for every set bit in @a word.
template <typename Fn>
inline void GcForEachSetBit(uint64_t word, Fn&& visitor) noexcept {
    while (word != 0) {
        int bit = GcCtz64(word);
        visitor(bit);
        word &= word - 1;  // clear lowest set bit
    }
}

/// Invoke @a visitor(bit_index) for every zero bit in @a word.
template <typename Fn>
inline void GcForEachZeroBit(uint64_t word, Fn&& visitor) noexcept {
    GcForEachSetBit(~word, std::forward<Fn>(visitor));
}

}  // namespace chaos::il2cpp::runtime_core

// ======================================================================
// Prefetch macro
// ======================================================================

#if defined(__clang__) || defined(__GNUC__)
    #define CHAOS_IL2CPP_PREFETCH_READ(addr)  __builtin_prefetch(addr, 0, 3)
#elif defined(_MSC_VER)
    #include <intrin.h>
    #define CHAOS_IL2CPP_PREFETCH_READ(addr)  _mm_prefetch(                    \
        reinterpret_cast<char const*>(addr), _MM_HINT_T0)
#else
    #define CHAOS_IL2CPP_PREFETCH_READ(addr)  ((void)(addr))
#endif

#endif  // CHAOS_IL2CPP_GC_BIT_UTILS_H_