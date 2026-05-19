#include "gc_scheduler.h"

#include <algorithm>
#include <chrono>
#include <cmath>

#include "gc_bgc.h"

namespace chaos::il2cpp::runtime_core {

// ── Global instance ────────────────────────────────────────────────
GcScheduler g_gc_scheduler;

// ── Allocation tracking ──────────────────────────────────────────

void GcScheduler::RecordAllocation(CHAOS_IL2CPP_SIZE bytes) noexcept {
    alloc_since_last_gc_.fetch_add(bytes, std::memory_order_relaxed);
    alloc_since_last_full_gc_.fetch_add(bytes, std::memory_order_relaxed);
    // Decrement cooldown skip counter (saturating at 0).
    auto skips = gc_cooldown_skips_.load(std::memory_order_relaxed);
    while (skips > 0) {
        if (gc_cooldown_skips_.compare_exchange_weak(skips, skips - 1,
                std::memory_order_release, std::memory_order_relaxed)) {
            break;
        }
    }
}

void GcScheduler::RecordGcCompleted() noexcept {
    // Set cooldown to skip ShouldTriggerGc for the next N allocations.
    gc_cooldown_skips_.store(kCooldownAllocations, std::memory_order_release);

    // Record completion timestamp for TryClaimGcSlot rate limiting.
    auto now = std::chrono::steady_clock::now();
    uint64_t now_ns = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            now.time_since_epoch()).count());
    last_gc_completion_ns_.store(now_ns, std::memory_order_release);
}

// ── GC rate limiting ─────────────────────────────────────────────

bool GcScheduler::TryClaimGcSlot() noexcept {
    // Take a timestamp.  Only called from slow paths (NurseryAllocateSlow),
    // so the cost of steady_clock::now() is negligible here.
    auto now = std::chrono::steady_clock::now();
    uint64_t now_ns = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            now.time_since_epoch()).count());

    uint64_t last_ns = last_gc_completion_ns_.load(std::memory_order_acquire);

    // If no GC has ever completed, or enough time has passed since the last one,
    // try to claim the slot atomically.  Only one thread succeeds.
    if (last_ns == 0 || (now_ns >= last_ns &&
        (now_ns - last_ns) >= kMinGcIntervalNs)) {
        // CAS: claim the slot by writing now_ns.  If last_ns changed between
        // our load and CAS (another thread claimed), CAS fails gracefully.
        if (last_gc_completion_ns_.compare_exchange_strong(last_ns, now_ns,
                std::memory_order_acq_rel, std::memory_order_acquire)) {
            return true;  // Slot claimed — caller may proceed with GC.
        }
        // CAS failed — another thread claimed the slot first.
    }

    return false;  // Too soon since last GC, or another thread claimed the slot.
}

// ── Collection recording ─────────────────────────────────────────

void GcScheduler::RecordYoungCollection(CHAOS_IL2CPP_SIZE nursery_used,
                                         CHAOS_IL2CPP_SIZE bytes_promoted,
                                         uint64_t /*pause_ns*/) noexcept {
    // Update EMA survival rate.
    double survival = (nursery_used > 0)
        ? static_cast<double>(bytes_promoted) / static_cast<double>(nursery_used)
        : 0.0;

    // V4-M8: Atomic read-modify-write of survival_rate via bitcast.
    uint64_t old_bits = survival_rate_bits_.load(std::memory_order_relaxed);
    double old_rate = BitsToDouble(old_bits);
    double new_rate = (1.0 - kEmaAlpha) * old_rate + kEmaAlpha * survival;
    if (new_rate < 0.0) new_rate = 0.0;
    if (new_rate > 1.0) new_rate = 1.0;
    survival_rate_bits_.store(DoubleToBits(new_rate), std::memory_order_relaxed);

    // Record nursery used for sizing.
    last_nursery_used_.store(nursery_used, std::memory_order_relaxed);

    // V4-H4: Accumulate actual allocation volume into full-GC counter.
    // nursery_used = total bytes allocated since last young GC (the entire
    // nursery content, both live and dead).  Adding this to
    // alloc_since_last_full_gc_ replaces the previous per-slow-path tracking
    // which only recorded the single allocation size (e.g., 32 bytes) instead
    // of the full nursery volume (~256 KB), making the full-GC trigger
    // effectively dead code.
    alloc_since_last_full_gc_.fetch_add(nursery_used, std::memory_order_relaxed);

    // Reset per-GC allocation counter (the young collection just processed
    // all nursery allocations).  The per-GC counter drives the young-GC
    // trigger and accumulates between young GCs via RecordAllocation calls
    // from the nursery slow path.
    alloc_since_last_gc_.store(0, std::memory_order_relaxed);
}

void GcScheduler::RecordFullCollection(CHAOS_IL2CPP_SIZE total_heap_bytes, uint64_t /*pause_ns*/) noexcept {
    // Reset full-GC allocation counter.
    alloc_since_last_full_gc_.store(0, std::memory_order_relaxed);

    // Reset page count growth counter.
    page_count_growth_.store(0, std::memory_order_relaxed);

    // Update estimated heap size from the actual total heap footprint.
    // This enables DecideCollection() to trigger full GC when allocation
    // exceeds kFullTriggerMultiplier * estimated_heap_size_.
    // Using the actual heap size (old-gen page usage) rather than the
    // young-collection-level allocation counter, which was ~1000× too
    // small and would prevent the scheduler from ever triggering full GC.
    estimated_heap_size_.store(total_heap_bytes > 0 ? total_heap_bytes : kDefaultNurserySize,
                               std::memory_order_relaxed);

    // Clear the full-GC request flag.
    full_gc_requested_.store(false, std::memory_order_relaxed);
}

void GcScheduler::RecordPageCountGrowth(int delta) noexcept {
    page_count_growth_.fetch_add(delta, std::memory_order_relaxed);
}

void GcScheduler::ResetPageCountGrowth() noexcept {
    page_count_growth_.store(0, std::memory_order_relaxed);
}

// ── Collection decision ──────────────────────────────────────────

GcCollectionKind GcScheduler::DecideCollection() const noexcept {
    // 1. Full GC requested by another thread?
    if (full_gc_requested_.load(std::memory_order_acquire)) {
        return GcCollectionKind::FULL;
    }

    // 2. Page count growth threshold exceeded?
    // Catches rapid page allocation bursts before the byte-based trigger reacts.
    if (page_count_growth_.load(std::memory_order_relaxed) >= kMaxPageGrowthThreshold) {
        auto& bgc = BgcController::Instance();
        if (!bgc.IsBusy()) {
            return GcCollectionKind::FULL_BGC;
        }
        return GcCollectionKind::FULL;
    }

    // 3. Snapshot multi-variable state for consistent decision.
    CHAOS_IL2CPP_SIZE alloc_full = alloc_since_last_full_gc_.load(std::memory_order_relaxed);
    CHAOS_IL2CPP_SIZE heap_est = estimated_heap_size_.load(std::memory_order_relaxed);

    if (heap_est > 0 && alloc_full > static_cast<CHAOS_IL2CPP_SIZE>(heap_est * kFullTriggerMultiplier)) {
        // Full collection threshold exceeded.  Prefer BGC over STW when:
        //   - BGC thread is running
        //   - BGC is not already busy with a cycle
        //   - This is not a forced full GC (emergency)
        auto& bgc = BgcController::Instance();
        if (!bgc.IsBusy()) {
            return GcCollectionKind::FULL_BGC;
        }
        // BGC is busy — fall back to STW full (emergency).
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
    // V4-M8: Atomic read of survival_rate via bitcast.
    double survival = BitsToDouble(survival_rate_bits_.load(std::memory_order_relaxed));

    // Target: 2× the last nursery's survived bytes gives us headroom
    // before the next GC triggers.
    CHAOS_IL2CPP_SIZE last_used = last_nursery_used_.load(std::memory_order_relaxed);
    double target = survival * 2.0 * static_cast<double>(last_used);

    // Fallback: if last_used is 0 or survival is near 0, use max nursery size
    // to reduce GC frequency (no point doing frequent GCs on an empty nursery).
    if (target < static_cast<double>(kMinNurserySize)) {
        // Survival rate near zero → prefer max nursery for fewer GC cycles.
        // The default size (256 KB) causes excessive safepoint overhead in
        // high-throughput scenarios like the 100-thread stress test.
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