/// @file gc_heap_manager.cpp
/// GcHeapManager implementation — heap lifecycle and thread mapping.

#include "gc_heap_manager.h"

#include <cstdio>
#include <cstdlib>

#include "gc_old_gen.h"
#include "gc_young_gen.h"

namespace chaos::il2cpp::runtime_core {

// ── TLS pointer definition ───────────────────────────────────────────
thread_local GcHeapContext* tls_current_heap = nullptr;

// ── GcHeapManager ────────────────────────────────────────────────────

void GcHeapManager::Initialize(int num_heaps) noexcept {
#if CHAOS_IL2CPP_GC_SERVER
    // Auto-detect NUMA node count if not specified.
    if (num_heaps <= 0) {
        GcNumaInit();
        num_heaps = GcNumaNodeCount();
        if (num_heaps <= 0) num_heaps = 1;
    }

    heap_count_ = num_heaps;
    heaps_ = std::make_unique<std::unique_ptr<GcHeapContext>[]>(static_cast<size_t>(heap_count_));

    for (int i = 0; i < heap_count_; i++) {
        auto heap = std::make_unique<GcHeapContext>();
        heap->heap_id = i;
        heap->numa_node = i;

        // Initialize per-heap old-gen with a unique address hint to avoid
        // virtual address conflicts between heaps.
        uintptr_t heap_hint = static_cast<uintptr_t>(0x200000000ULL +
            static_cast<uintptr_t>(i) * 0x40000000ULL);
        heap->old_gen.Init(heap_hint);
        heaps_[i] = std::move(heap);
    }

    std::printf("[GC] GcHeapManager initialized: %d heaps\n", heap_count_);
#else
    (void)num_heaps;
    heap_count_ = 1;
#endif  // CHAOS_IL2CPP_GC_SERVER
}

int GcHeapManager::HeapForCurrentThread() noexcept {
#if CHAOS_IL2CPP_GC_SERVER
    int node = GcNumaCurrentNode();
    if (node < 0 || node >= heap_count_) {
        node = 0;
    }
    return node;
#else
    return 0;
#endif  // CHAOS_IL2CPP_GC_SERVER
}

bool GcHeapManager::AdjustHeapCount(int new_count) noexcept {
#if CHAOS_IL2CPP_GC_SERVER
    if (new_count < 1) new_count = 1;
    if (new_count > kMaxServerHeaps) new_count = kMaxServerHeaps;
    if (new_count == heap_count_) return false;

    // Grow/shrink by reallocating the pointer array and moving the owning
    // unique_ptr handles (movable), never the GcHeapContext objects (they are
    // non-copyable/non-movable and keep stable addresses).  Threads re-bind by
    // NUMA node → index on their next HeapForCurrentThread, so a grow exposes
    // the new heaps and a shrink remaps threads off removed nodes to heap 0
    // (clamp in HeapForCurrentThread).  Caller must hold a safepoint so no
    // mutator is mid-allocation across the reallocation.
    auto new_heaps = std::make_unique<std::unique_ptr<GcHeapContext>[]>(static_cast<size_t>(new_count));
    const int keep = (new_count < heap_count_) ? new_count : heap_count_;
    for (int i = 0; i < keep; i++) {
        new_heaps[i] = std::move(heaps_[i]);     // move the owning handle
    }
    for (int i = keep; i < new_count; i++) {
        auto heap = std::make_unique<GcHeapContext>();
        heap->heap_id = i;
        heap->numa_node = i;
        uintptr_t heap_hint = static_cast<uintptr_t>(0x200000000ULL +
            static_cast<uintptr_t>(i) * 0x40000000ULL);
        heap->old_gen.Init(heap_hint);
        new_heaps[i] = std::move(heap);
    }
    heaps_ = std::move(new_heaps);
    heap_count_ = new_count;
    std::printf("[GC] GcHeapManager::AdjustHeapCount -> %d heaps\n", heap_count_);
    return true;
#else
    (void)new_count;
    return false;   // WKS: heap count is fixed at 1
#endif  // CHAOS_IL2CPP_GC_SERVER
}

}  // namespace chaos::il2cpp::runtime_core
