#ifndef CHAOS_IL2CPP_GC_BGC_INLINE_H_
#define CHAOS_IL2CPP_GC_BGC_INLINE_H_

/// @file gc_bgc_inline.h
/// Inline SATB pre-write barrier for codegen-emitted stfld/stobj.
///
/// This header is included by generated code (or the interpreter dispatch)
/// to emit the pre-write barrier before every managed-pointer field store.
///
/// The barrier records the old object reference before it is overwritten,
/// preserving the SATB invariant: all objects reachable at the start of
/// concurrent mark remain reachable from the GC's perspective.
///
/// == Usage (codegen) ==
/// Before:  *slot = new_value;
/// Emit:    BgcSatbPreWriteBarrier(slot);
///          *slot = new_value;
///
/// == Fast-path check ==
/// The barrier starts with a single relaxed atomic load of g_bgc_is_marking.
/// In the common case (not marking), this compiles to a single load + branch
/// and returns immediately.  The slow path (marking) calls into the
/// controller to store the old value in the thread-local SATB buffer.

#include <atomic>
#include <cstddef>

#include "gc_bgc.h"

namespace chaos::il2cpp::runtime_core {

/// Inline SATB pre-write barrier.
///
/// @param slot  Address of the managed pointer field being overwritten.
///              Must point to a word-aligned location (typically void**).
///
/// Thread-safe: reads g_bgc_is_marking with relaxed ordering (correct
/// because false positives are safe — we may record an extra entry in
/// the brief window between the flag being set and the store executing;
/// false negatives are prevented by the STW re-mark phase which drains
/// all remaining SATB entries).
inline void BgcSatbPreWriteBarrier(void** slot) noexcept {
    // Fast path: barrier is inactive unless concurrent mark is running.
    if (!g_bgc_is_marking.load(std::memory_order_relaxed)) [[likely]]
        return;

    // Read the old value (the one about to be overwritten).
    void* old_obj = *slot;
    if (old_obj == nullptr) [[likely]]
        return;

    // Get or allocate the current thread's SATB buffer from the global pool.
    // Uses thread_local int index (~4 bytes) instead of a 4KB thread_local buffer.
    int pool_idx = tls_satb_buffer_index;
    if (pool_idx < 0) [[unlikely]] {
        pool_idx = BgcController::Instance().AllocateSatbBuffer();
        if (pool_idx < 0) return;  // OOM — skip barrier (rare, safe with SATB).
        tls_satb_buffer_index = pool_idx;
    }
    auto& buf = BgcController::Instance().GetSatbBuffer(pool_idx);
    uint32_t idx = buf.count.load(std::memory_order_relaxed);

    // One-time registration of this thread's SATB buffer for STW re-mark.
    if (idx == 0 && !tls_satb_registered) [[unlikely]] {
        BgcController::Instance().RegisterThreadSatbBuffer(&buf);
        tls_satb_registered = true;
    }

    if (idx < static_cast<uint32_t>(kBgcSatbBufferSize)) [[likely]] {
        buf.entries[idx] = old_obj;
        buf.count.store(idx + 1, std::memory_order_release);
    } else {
        // Buffer full — flush to global queue, then retry.
        BgcFlushSatbBuffer(buf.entries, kBgcSatbBufferSize);
        buf.count.store(0, std::memory_order_release);
        // Retry: store in now-empty buffer.
        buf.entries[0] = old_obj;
        buf.count.store(1, std::memory_order_release);
    }
}

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_BGC_INLINE_H_
