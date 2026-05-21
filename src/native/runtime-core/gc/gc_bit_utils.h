#ifndef CHAOS_IL2CPP_GC_BIT_UTILS_H_
#define CHAOS_IL2CPP_GC_BIT_UTILS_H_

#include <chaos/native_types.h>

#include <cstdint>

// ── SIMD card table scan ──────────────────────────────────────────
// GcSegmentHasDirtyCards checks 128 card bytes for any non-zero entry.
// Uses SSE2 on x64 (128-bit SIMD: 8 iterations of 16 bytes) or
// NEON on ARM64.  Falls back to byte-by-byte when SIMD is unavailable.

#if defined(_MSC_VER) && (defined(_M_AMD64) || defined(_M_X64))
  #include <intrin.h>
  #define CHAOS_IL2CPP_HAS_SSE2 1
#elif defined(__SSE2__)
  #include <emmintrin.h>
  #define CHAOS_IL2CPP_HAS_SSE2 1
#else
  #define CHAOS_IL2CPP_HAS_SSE2 0
#endif

#if defined(__ARM_NEON) || defined(__aarch64__) || defined(_M_ARM64)
  #include <arm_neon.h>
  #define CHAOS_IL2CPP_HAS_NEON 1
#else
  #define CHAOS_IL2CPP_HAS_NEON 0
#endif

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

/// Returns the number of 1-bits in @a word (population count / Hamming weight).
inline int GcPopCount64(uint64_t word) noexcept {
#if defined(_MSC_VER) && !defined(__clang__)
    return static_cast<int>(__popcnt64(word));
#else
    return static_cast<int>(__builtin_popcountll(word));
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

// ======================================================================
// Card table SIMD scan
// ======================================================================

/// Check whether any of the 128 card bytes in @a cards is non-zero.
/// Returns true when at least one dirty card is found.
/// SSE2 path: 8 iterations of 16-byte SIMD loads.
/// NEON path: 16 iterations of 8-byte SIMD loads.
/// Fallback: byte-by-byte scan.
inline bool GcSegmentHasDirtyCards(const uint8_t* cards) noexcept {
#if CHAOS_IL2CPP_HAS_SSE2
    // Process 16 bytes at a time using SSE2.
    for (int i = 0; i < 128; i += 16) {
        __m128i chunk = _mm_loadu_si128(
            reinterpret_cast<const __m128i*>(cards + i));
        __m128i zero = _mm_setzero_si128();
        __m128i cmp = _mm_cmpeq_epi8(chunk, zero);
        int mask = _mm_movemask_epi8(cmp);
        if (mask != 0xFFFF) {
            return true;  // at least one byte != 0
        }
    }
    return false;
#elif CHAOS_IL2CPP_HAS_NEON
    // Process 8 bytes at a time using NEON.
    for (int i = 0; i < 128; i += 8) {
        uint8x8_t chunk = vld1_u8(cards + i);
        uint8x8_t zero = vdup_n_u8(0);
        uint8x8_t cmp = vceq_u8(chunk, zero);
        uint64_t mask = vget_lane_u64(vreinterpret_u64_u8(cmp), 0);
        if (mask != 0xFFFFFFFFFFFFFFFFULL) {
            return true;  // at least one byte != 0
        }
    }
    return false;
#else
    // Non-SIMD fallback: byte-by-byte.
    for (int i = 0; i < 128; i++) {
        if (cards[i] != 0) return true;
    }
    return false;
#endif
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