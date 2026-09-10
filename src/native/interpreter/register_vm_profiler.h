// register_vm_profiler.h — Per-method profiling counters for the register VM
//
// Provides lightweight per-method call count, CPU time, and GC allocation
// tracking for hot-update (interpreter) methods.  Disabled by default;
// enabled by defining CHAOS_IL2CPP_VM_PROFILER_ENABLED=1 before include.
//
// Design:
//   - Fixed-size hash table (kMaxProfiledMethods = 4096) keyed by method address
//   - Per-method counters: call_count, total_cycles, gc_alloc_bytes
//   - Thread-safe accumulation (atomic add on hot path)
//   - O(1) lookup via method address → slot mapping
//   - DumpAll() prints a sorted table to stderr

#ifndef CHAOS_IL2CPP_REGISTER_VM_PROFILER_H_
#define CHAOS_IL2CPP_REGISTER_VM_PROFILER_H_

#include <cstdint>
#include <cstdio>
#include <atomic>
#include <cstring>

namespace chaos::il2cpp::interpreter {

// ── Compile-time toggle ──────────────────────────────────────────────────
#ifndef CHAOS_IL2CPP_VM_PROFILER_ENABLED
#define CHAOS_IL2CPP_VM_PROFILER_ENABLED 0
#endif

// ── Constants ────────────────────────────────────────────────────────────
static constexpr uint32_t kMaxProfiledMethods = 4096;
static constexpr uint32_t kProfilerHashSize   = 4096;  // must be power of 2

// ── Per-method profiling slot ────────────────────────────────────────────
struct alignas(64) ProfiledMethodSlot {
    std::atomic<uintptr_t> method_key{0};    // method address / identifier (0 = empty)
    std::atomic<uint64_t>  call_count{0};    // number of invocations
    std::atomic<uint64_t>  total_cycles{0};  // accumulated RDTSC cycles
    std::atomic<uint64_t>  gc_alloc_bytes{0}; // GC bytes allocated by this method
};

// ── Global profiler state ────────────────────────────────────────────────
struct VmProfilerState {
    ProfiledMethodSlot slots[kMaxProfiledMethods];
    std::atomic<uint32_t> slot_count{0};

    /// Find or create a slot for the given method_key.
    /// Returns the slot index on success, or -1 if full.
    int32_t FindOrCreateSlot(uintptr_t method_key) noexcept {
        if (method_key == 0) return -1;

        // Linear probing hash
        uint32_t idx = static_cast<uint32_t>(method_key ^ (method_key >> 16)) & (kProfilerHashSize - 1);
        for (uint32_t probe = 0; probe < kProfilerHashSize; ++probe) {
            uint32_t slot = (idx + probe) & (kProfilerHashSize - 1);
            uintptr_t expected = 0;
            // Try to claim empty slot
            if (slots[slot].method_key.load(std::memory_order_acquire) == 0) {
                if (slots[slot].method_key.compare_exchange_weak(expected, method_key,
                        std::memory_order_release, std::memory_order_acquire)) {
                    slot_count.fetch_add(1, std::memory_order_relaxed);
                    return static_cast<int32_t>(slot);
                }
            }
            // Found existing entry
            if (slots[slot].method_key.load(std::memory_order_acquire) == method_key) {
                return static_cast<int32_t>(slot);
            }
        }
        return -1;  // full
    }

    /// Record one call with its cycle count.
    void RecordCall(int32_t slot_idx, uint64_t cycles,
                    uint64_t gc_bytes = 0) noexcept {
        if (slot_idx < 0) return;
        slots[slot_idx].call_count.fetch_add(1, std::memory_order_relaxed);
        slots[slot_idx].total_cycles.fetch_add(cycles, std::memory_order_relaxed);
        if (gc_bytes > 0) {
            slots[slot_idx].gc_alloc_bytes.fetch_add(gc_bytes, std::memory_order_relaxed);
        }
    }

    /// Dump all profiled methods to stderr, sorted by call_count descending.
    void DumpAll() noexcept {
        // Collect non-empty slots
        uint32_t count = 0;
        struct Entry { uintptr_t key; uint64_t calls; uint64_t cycles; uint64_t gc_bytes; };
        Entry entries[kMaxProfiledMethods];

        for (uint32_t i = 0; i < kProfilerHashSize; ++i) {
            uintptr_t key = slots[i].method_key.load(std::memory_order_acquire);
            if (key != 0) {
                entries[count++] = { key,
                    slots[i].call_count.load(std::memory_order_relaxed),
                    slots[i].total_cycles.load(std::memory_order_relaxed),
                    slots[i].gc_alloc_bytes.load(std::memory_order_relaxed) };
            }
        }

        // Simple insertion sort by call_count descending
        for (uint32_t i = 1; i < count; ++i) {
            Entry tmp = entries[i];
            int32_t j = static_cast<int32_t>(i) - 1;
            while (j >= 0 && entries[j].calls < tmp.calls) {
                entries[j + 1] = entries[j];
                --j;
            }
            entries[j + 1] = tmp;
        }

        std::fprintf(stderr, "── VM Profiler (top %u methods) ──\n", count);
        std::fprintf(stderr, "%-20s %12s %14s %14s\n",
                     "Method", "Calls", "Cycles", "GC Bytes");
        std::fprintf(stderr, "%-20s %12s %14s %14s\n",
                     "------", "-----", "------", "--------");
        for (uint32_t i = 0; i < count; ++i) {
            std::fprintf(stderr, "0x%016llx %12llu %14llu %14llu\n",
                         (unsigned long long)entries[i].key,
                         (unsigned long long)entries[i].calls,
                         (unsigned long long)entries[i].cycles,
                         (unsigned long long)entries[i].gc_bytes);
        }
    }

    /// Reset all counters.
    void Reset() noexcept {
        for (uint32_t i = 0; i < kProfilerHashSize; ++i) {
            slots[i].method_key.store(0, std::memory_order_release);
            slots[i].call_count.store(0, std::memory_order_relaxed);
            slots[i].total_cycles.store(0, std::memory_order_relaxed);
            slots[i].gc_alloc_bytes.store(0, std::memory_order_relaxed);
        }
        slot_count.store(0, std::memory_order_relaxed);
    }
};

// ── Global profiler instance ─────────────────────────────────────────────
// One process-wide profiler state.  Thread-safe via atomic slots.
extern VmProfilerState g_vm_profiler;

// ── RAII profiler scope for per-method tracking ──────────────────────────
class VmProfileScope {
public:
    VmProfileScope(uintptr_t method_key) noexcept
        : slot_idx_(-1)
        , start_cycles_(0) {
#if CHAOS_IL2CPP_VM_PROFILER_ENABLED
        // ARM64 not yet supported for VM profiler (no __rdtsc).
        // Use CHAOS_IL2CPP_PROFILE_SCOPE instead for ARM64 profiling.
        #if !defined(__x86_64__) && !defined(__i386__) && !defined(_M_AMD64)
        #error "register_vm_profiler.h: CHAOS_IL2CPP_VM_PROFILER_ENABLED requires x86 RDTSC"
        #endif
        if (method_key != 0) {
            slot_idx_ = g_vm_profiler.FindOrCreateSlot(method_key);
            start_cycles_ = __rdtsc();
        }
#endif
    }

    ~VmProfileScope() noexcept {
#if CHAOS_IL2CPP_VM_PROFILER_ENABLED
        if (slot_idx_ >= 0) {
            uint64_t elapsed = __rdtsc() - start_cycles_;
            g_vm_profiler.RecordCall(slot_idx_, elapsed, 0);
        }
#endif
    }

    VmProfileScope(const VmProfileScope&) = delete;
    VmProfileScope& operator=(const VmProfileScope&) = delete;

private:
    int32_t  slot_idx_;
    uint64_t start_cycles_;
};

/// Dump profiler data to a file (or stderr if path is null/empty).
void DumpProfilerToFile(const char* path) noexcept;

/// Reset all profiler counters to zero.
void ResetProfiler() noexcept;

}  // namespace chaos::il2cpp::interpreter

#endif  // CHAOS_IL2CPP_REGISTER_VM_PROFILER_H_
