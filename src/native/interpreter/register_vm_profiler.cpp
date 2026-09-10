// register_vm_profiler.cpp — Global profiler state for the register VM
//
// Provides the process-wide VmProfilerState instance and any profiler
// utility functions that require compilation units (not just headers).

#include "register_vm_profiler.h"

namespace chaos::il2cpp::interpreter {

// Process-wide profiler state
VmProfilerState g_vm_profiler;

// ── DumpProfilerToFile ───────────────────────────────────────────────────
void DumpProfilerToFile(const char* path) noexcept {
    if (path == nullptr || path[0] == '\0') {
        g_vm_profiler.DumpAll();
        return;
    }
    FILE* fp = std::fopen(path, "w");
    if (fp == nullptr) {
        // Fallback to stderr
        g_vm_profiler.DumpAll();
        return;
    }
    // Collect and sort entries (same logic as DumpAll but to file)
    uint32_t count = 0;
    struct Entry { uintptr_t key; uint64_t calls; uint64_t cycles; uint64_t gc_bytes; };
    Entry entries[kMaxProfiledMethods];

    for (uint32_t i = 0; i < kProfilerHashSize; ++i) {
        uintptr_t key = g_vm_profiler.slots[i].method_key.load(std::memory_order_acquire);
        if (key != 0) {
            entries[count++] = { key,
                g_vm_profiler.slots[i].call_count.load(std::memory_order_relaxed),
                g_vm_profiler.slots[i].total_cycles.load(std::memory_order_relaxed),
                g_vm_profiler.slots[i].gc_alloc_bytes.load(std::memory_order_relaxed) };
        }
    }

    // Insertion sort by call_count descending
    for (uint32_t i = 1; i < count; ++i) {
        Entry tmp = entries[i];
        int32_t j = static_cast<int32_t>(i) - 1;
        while (j >= 0 && entries[j].calls < tmp.calls) {
            entries[j + 1] = entries[j];
            --j;
        }
        entries[j + 1] = tmp;
    }

    std::fprintf(fp, "── VM Profiler (top %u methods) ──\n", count);
    std::fprintf(fp, "%-20s %12s %14s %14s\n",
                 "Method", "Calls", "Cycles", "GC Bytes");
    std::fprintf(fp, "%-20s %12s %14s %14s\n",
                 "------", "-----", "------", "--------");
    for (uint32_t i = 0; i < count; ++i) {
        std::fprintf(fp, "0x%016llx %12llu %14llu %14llu\n",
                     (unsigned long long)entries[i].key,
                     (unsigned long long)entries[i].calls,
                     (unsigned long long)entries[i].cycles,
                     (unsigned long long)entries[i].gc_bytes);
    }

    std::fclose(fp);
}

// ── ResetProfiler ────────────────────────────────────────────────────────
void ResetProfiler() noexcept {
    g_vm_profiler.Reset();
}

}  // namespace chaos::il2cpp::interpreter
