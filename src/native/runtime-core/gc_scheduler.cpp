#include "gc_scheduler.h"

#include <algorithm>
#include <cmath>

namespace chaos::il2cpp::runtime_core {

// ── Global instance ────────────────────────────────────────────────
GcScheduler g_gc_scheduler;

// ── Allocation tracking ──────────────────────────────────────────

void GcScheduler::RecordAllocation(CHAOS_IL2CPP_SIZE bytes) noexcept {
    alloc_since_last_gc_.fetch_add(bytes, std::memory_order_relaxed);
    alloc_since_last_full_gc_.fetch_add(bytes, std::memory_order_relaxed);
}

// ── Collection recording ─────────────────────────────────────────

void GcScheduler::RecordYoungCollection(CHAOS_IL2CPP_SIZE nursery_used,
                                         CHAOS_IL2CPP_SIZE bytes_promoted,
                                         uint64_t /*pause_ns*/) noexcept {
    // Update EMA survival rate.
    double survival = (nursery_used > 0)
        ? static_cast<double>(bytes_promoted) / static_cast<double>(nursery_used)
        : 0.0;
    survival_rate_ = (1.0 - kEmaAlpha) * survival_rate_ + kEmaAlpha * survival;

    // Clamp to [0, 1].
    if (survival_rate_ < 0.0) survival_rate_ = 0.0;
    if (survival_rate_ > 1.0) survival_rate_ = 1.0;

    // Record nursery used for sizing.
    last_nursery_used_.store(nursery_used, std::memory_order_relaxed);

    // Reset per-GC allocation counter (the young collection just processed
    // all nursery allocations).
    alloc_since_last_gc_.store(0, std::memory_order_relaxed);
}

void GcScheduler::RecordFullCollection(uint64_t /*pause_ns*/) noexcept {
    // Reset full-GC allocation counter.
    alloc_since_last_full_gc_.store(0, std::memory_order_relaxed);

    // Clear the full-GC request flag.
    full_gc_requested_.store(false, std::memory_order_relaxed);
}

// ── Collection decision ──────────────────────────────────────────

GcCollectionKind GcScheduler::DecideCollection() const noexcept {
    // 1. Full GC requested by another thread?
    if (full_gc_requested_.load(std::memory_order_acquire)) {
        return GcCollectionKind::FULL;
    }

    // 2. Allocation since last full GC exceeds threshold?
    CHAOS_IL2CPP_SIZE alloc_full = alloc_since_last_full_gc_.load(std::memory_order_relaxed);
    CHAOS_IL2CPP_SIZE heap_est = estimated_heap_size_.load(std::memory_order_relaxed);
    if (heap_est > 0 && alloc_full > static_cast<CHAOS_IL2CPP_SIZE>(heap_est * kFullTriggerMultiplier)) {
        return GcCollectionKind::FULL;
    }

    // 3. Allocation since last young GC exceeds threshold?
    CHAOS_IL2CPP_SIZE alloc = alloc_since_last_gc_.load(std::memory_order_relaxed);
    CHAOS_IL2CPP_SIZE last_nursery = last_nursery_used_.load(std::memory_order_relaxed);
    if (alloc > static_cast<CHAOS_IL2CPP_SIZE>(last_nursery * kYoungTriggerMultiplier)) {
        return GcCollectionKind::YOUNG;
    }

    return GcCollectionKind::NONE;
}

bool GcScheduler::IsFullGcRequested() const noexcept {
    return full_gc_requested_.load(std::memory_order_acquire);
}

void GcScheduler::RequestFullGc() noexcept {
    full_gc_requested_.store(true, std::memory_order_release);
}

// ── Nursery sizing ───────────────────────────────────────────────

CHAOS_IL2CPP_SIZE GcScheduler::RecommendedNurserySize() const noexcept {
    double survival = survival_rate_;

    // Target: 2× the last nursery's survived bytes gives us headroom
    // before the next GC triggers.
    CHAOS_IL2CPP_SIZE last_used = last_nursery_used_.load(std::memory_order_relaxed);
    double target = survival * 2.0 * static_cast<double>(last_used);

    // Fallback: if last_used is 0 or survival is near 0, use default.
    if (target < static_cast<double>(kMinNurserySize)) {
        target = static_cast<double>(kDefaultNurserySize);
    }

    CHAOS_IL2CPP_SIZE size = static_cast<CHAOS_IL2CPP_SIZE>(target);

    // Clamp to [kMinNurserySize, kMaxNurserySize].
    if (size < kMinNurserySize) size = kMinNurserySize;
    if (size > kMaxNurserySize) size = kMaxNurserySize;

    // Round up to page size (4 KB) for aligned VirtualAlloc.
    constexpr CHAOS_IL2CPP_SIZE kPageSize = 4 * 1024;
    size = (size + kPageSize - 1) & ~(kPageSize - 1);

    return size;
}

CHAOS_IL2CPP_SIZE GcScheduler::TotalAllocatedSinceLastGC() const noexcept {
    return alloc_since_last_gc_.load(std::memory_order_relaxed);
}

}  // namespace chaos::il2cpp::runtime_core