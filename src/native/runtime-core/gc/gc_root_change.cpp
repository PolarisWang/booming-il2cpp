// gc_root_change.cpp — BGC concurrent root change tracking (G-25)
//
// Non-inline implementation of BgcRecordRootChange.  The function accesses
// thread-local storage (tls_this_thread) and must be compiled within
// chaos_runtime_core rather than inlined into callers that live in
// separate libraries (chaos_interpreter, chaos_codegen).

#include "gc_root_change.h"

namespace chaos::il2cpp::runtime_core {

void BgcRecordRootChange(void** slot, void* old_value) noexcept {
    auto* mt = threading::GetCurrentThread();
    if (mt == nullptr) return;

    // Claim a slot in the ring buffer via atomic fetch_add.
    uint32_t head = mt->root_change_head_.fetch_add(1, std::memory_order_acq_rel);
    uint32_t idx = head % threading::ManagedThread::kRootChangeBufferSize;
    mt->root_change_buffer_[idx].old_value = old_value;
    mt->root_change_buffer_[idx].slot = slot;
    // Release-store ensures the entry is visible to the BGC drainer.
    std::atomic_thread_fence(std::memory_order_release);

    // NOTE: we deliberately do NOT advance `tail` here.  tail is owned solely by
    // the STW drainer (BgcDrainRootChangeBuffer sets tail=head after consuming).
    // Forcing tail forward in the producer (previously "tail = head + 1" when
    // head - tail >= kRootChangeBufferSize) DISCARDED the un-consumed prefix of
    // the burst — old_value references that were never re-marked could let an
    // object be prematurely collected (G-25 correctness hazard), and it
    // under-reported the unconsumed count (drain returned ~buffer-tail window,
    // failing the RootChangeBufferWraparound contract that drained > 64).
    // Without the advance, head - tail == unconsumed logical writes since the
    // last drain; the drainer re-reads each ring slot's latest state, which is
    // the correct bounded-lossy ring behavior (overwritten entries are stale
    // anyway, and re-reading the latest per-slot value is safe — it may mark an
    // unreachable object, never a reachable one).
}

}  // namespace chaos::il2cpp::runtime_core
