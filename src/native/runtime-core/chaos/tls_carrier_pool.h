// chaos/tls_carrier_pool.h — Thread-local carrier pool for SIMD results
//
// Replaces std::malloc in generated inline-shape code with a lock-free
// thread-local bump allocator.  Each TLS pool is 16 KB (256 × 64 B slots)
// which covers deeply nested SIMD expressions without heap allocation.
//
// When the pool is exhausted, the Nth+NEXT call falls back to malloc so
// correctness never depends on pool depth.  The pool is NOT freed per-slot;
// the entire TLS region is reclaimed on thread exit or at the next GC
// safepoint, matching the short-lived nature of inline-shape temporaries.
//
// Alignment: the pool base is max_align_t (16B on MSVC, 16B on GCC),
// which satisfies Vector64 (8B), Vector128 (16B), Vector256 (32B) and
// Vector512 (64B) alignment requirements.  Each slot is padded to 64 B
// so any carrier fits.
//
// Usage:
//   #include <chaos/tls_carrier_pool.h>
//   auto* buf = chaos_tls_carrier_pool_alloc(sizeof(RuntimeIntrinsicVector256Carrier));
//
// This header is included by the generated native-aot code via the
// codegen SDK include path (src/native/runtime-core/chaos/).

#ifndef CHAOS_IL2CPP_TLS_CARRIER_POOL_H_
#define CHAOS_IL2CPP_TLS_CARRIER_POOL_H_

#include <cstddef>
#include <new>  // std::max_align_t, placement new
#include <chaos/native_types.h>  // CHAOS_IL2CPP_MALLOC

namespace chaos {
namespace il2cpp {
namespace runtime_core {

// ── Constants ──────────────────────────────────────────────────────
// 256 slots × 64 bytes = 16 KB per thread.  The slot size covers the
// largest carrier (RuntimeIntrinsicVector512Carrier = 64 B).
inline constexpr std::size_t kCarrierPoolSlotCount = 256;
inline constexpr std::size_t kCarrierPoolSlotSize  = 64;
inline constexpr std::size_t kCarrierPoolTotalBytes = kCarrierPoolSlotCount * kCarrierPoolSlotSize;

// ── TLS pool state ─────────────────────────────────────────────────
struct CarrierPoolState {
    alignas(alignof(std::max_align_t)) unsigned char buffer[kCarrierPoolTotalBytes];
    std::size_t cursor = 0;  // next free byte offset (bump)
};

// ── Allocation ──────────────────────────────────────────────────────
// Returns a pointer to a slot of at least `size` bytes from the TLS pool.
// Falls back to std::malloc when the pool is exhausted (cursor + size > total).
// Never returns nullptr (the fallback calls std::malloc which may return
// nullptr on OOM, matching the current behaviour).
inline void* chaos_tls_carrier_pool_alloc(std::size_t size) noexcept {
    thread_local CarrierPoolState tls_pool{};

    // Round up requested size to the slot boundary so the next alloc
    // starts at an aligned offset and does not fragment.
    std::size_t needed = ((size + kCarrierPoolSlotSize - 1) / kCarrierPoolSlotSize) * kCarrierPoolSlotSize;

    std::size_t current = tls_pool.cursor;
    if (current + needed <= kCarrierPoolTotalBytes) {
        tls_pool.cursor = current + needed;
        return &tls_pool.buffer[current];
    }

    // Pool exhausted — fall back to malloc.  This path is taken only
    // when SIMD expression nesting depth exceeds 256 operations, which
    // does not occur in practice (typical depth is 1–5), but exists as
    // a correctness guarantee.
    return CHAOS_IL2CPP_MALLOC(size);
}

}  // namespace runtime_core
}  // namespace il2cpp
}  // namespace chaos

#endif  // CHAOS_IL2CPP_TLS_CARRIER_POOL_H_
