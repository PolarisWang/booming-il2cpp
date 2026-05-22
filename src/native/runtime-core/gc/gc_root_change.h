#ifndef CHAOS_IL2CPP_GC_ROOT_CHANGE_H_
#define CHAOS_IL2CPP_GC_ROOT_CHANGE_H_

/// @file gc_root_change.h
/// BGC concurrent root change tracking (G-25).
///
/// During BGC concurrent mark, mutator threads may overwrite root slots
/// (static fields, GCHandle targets, etc.) that were already scanned in
/// the initial root set.  If a root slot held the only reference to an
/// object, overwriting it could let that object be collected prematurely.
///
/// This module provides a per-thread ring buffer that records root slot
/// modifications.  At the STW re-mark pause, the buffer is drained and
/// any newly-unreachable objects are (re-)marked to ensure they survive
/// the current BGC cycle.
///
/// == Usage ==
/// - Mutator: call BgcRecordRootChange(slot, old_value) in the write barrier
///   for root slots.  Lightweight: ring buffer CAS, < 5 ns typical.
/// - BGC re-mark: call BgcDrainRootChangeBuffer(mt) for each registered
///   thread during StwRemark.  For each entry, if old_value is still
///   live, re-mark it.

#include <atomic>
#include <cstdint>

#include "gc_old_gen.h"
#include "thread_state.h"

namespace chaos::il2cpp::runtime_core {

/// Record that a root slot @a slot was overwritten, and the previous value
/// at that slot was @a old_value.
///
/// Called by the root write barrier during BGC concurrent mark.
/// Thread-safe: uses atomic CAS on the ring buffer head.
///
/// @param slot      The root slot that was overwritten.
/// @param old_value The object reference that was in the slot before the write.
void BgcRecordRootChange(void** slot, void* old_value) noexcept;

/// Drain the root change buffer for thread @a mt.
///
/// For each unconsumed entry, if @a old_value is in old-gen and not yet
/// marked, mark it now (it may have been the only reference keeping the
/// object alive).
///
/// Called during STW re-mark (under safepoint — no concurrent mutators).
///
/// @param mt    The thread whose buffer to drain.
/// @param mark  Callback to mark an object as reachable (e.g. G_OldGen().BgcTryMark).
/// @return Number of entries drained.
template <typename MarkFn>
inline CHAOS_IL2CPP_SIZE BgcDrainRootChangeBuffer(threading::ManagedThread* mt,
                                                    MarkFn&& mark) noexcept {
    if (mt == nullptr) return 0;

    uint32_t tail = mt->root_change_tail_.load(std::memory_order_acquire);
    uint32_t head = mt->root_change_head_.load(std::memory_order_acquire);
    uint32_t count = head - tail;

    if (count == 0) return 0;

    uint32_t drained = 0;
    for (uint32_t i = tail; i < head; i++) {
        uint32_t idx = i % threading::ManagedThread::kRootChangeBufferSize;
        auto& entry = mt->root_change_buffer_[idx];
        void* old_val = entry.old_value;
        if (old_val != nullptr && G_OldGen().IsInOldGen(old_val) && mark(old_val)) {
            // Successfully re-marked an object that was potentially orphaned.
        }
        drained++;
    }

    mt->root_change_tail_.store(head, std::memory_order_release);
    return drained;
}

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_ROOT_CHANGE_H_
