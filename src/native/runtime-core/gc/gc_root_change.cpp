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

    // If we wrapped around and caught up to tail, advance tail to avoid
    // overwriting unread entries (stale entries will be re-read, which is
    // safe — just redundant work).
    uint32_t tail = mt->root_change_tail_.load(std::memory_order_acquire);
    if (head - tail >= threading::ManagedThread::kRootChangeBufferSize) {
        mt->root_change_tail_.store(head + 1, std::memory_order_release);
    }
}

}  // namespace chaos::il2cpp::runtime_core
