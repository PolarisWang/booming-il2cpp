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
    heaps_ = std::make_unique<GcHeapContext[]>(static_cast<size_t>(heap_count_));

    for (int i = 0; i < heap_count_; i++) {
        auto& heap = heaps_[i];
        heap.heap_id = i;
        heap.numa_node = i;

        // Initialize per-heap old-gen with a unique address hint to avoid
        // virtual address conflicts between heaps.
        uintptr_t heap_hint = static_cast<uintptr_t>(0x200000000ULL +
            static_cast<uintptr_t>(i) * 0x40000000ULL);
        heap.old_gen.Init(heap_hint);
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

}  // namespace chaos::il2cpp::runtime_core
