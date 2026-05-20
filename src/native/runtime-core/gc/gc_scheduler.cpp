#include "gc_scheduler.h"

#include <algorithm>
#include <chrono>
#include <cmath>

#include "gc_bgc.h"
#include "gc_api.h"
#include "gc_young_gen.h"

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

    // Track EMA of promoted bytes for survivor sizing.
    {
        CHAOS_IL2CPP_SIZE old_avg = avg_promoted_bytes_.load(std::memory_order_relaxed);
        CHAOS_IL2CPP_SIZE new_avg = static_cast<CHAOS_IL2CPP_SIZE>(
            (1.0 - kEmaAlpha) * static_cast<double>(old_avg) +
            kEmaAlpha * static_cast<double>(bytes_promoted));
        avg_promoted_bytes_.store(new_avg, std::memory_order_relaxed);
    }

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

    // ── Dynamic promotion threshold ──────────────────────────────
    // Adjust how many GC cycles objects stay in the survivor area
    // before being promoted to old gen, based on survival rate:
    //   Survival < 10%: threshold=3 (most objects die young, filter aggressively)
    //   Survival 10-30%: threshold=2 (moderate filtering)
    //   Survival > 30%: threshold=1 (objects live long, promote faster)
    int new_threshold;
    if (new_rate < 0.10) {
        new_threshold = 3;
    } else if (new_rate < 0.30) {
        new_threshold = 2;
    } else {
        new_threshold = 1;
    }
    g_young_gen.promotion_age_threshold_.store(new_threshold, std::memory_order_release);
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

// ── Gen1 budget tracking (DEAD CODE) ────────────────────────────────
// These functions write Gen1 budget counters that are never read by
// DecideCollection().  The actual Gen1 trigger is in GcYoungCollection
// Phase 4 via promotion_age_threshold_ (gc_young_gen.h).
// Kept for API compatibility; see gc_scheduler.h for context.

void GcScheduler::RecordGen1Collection(CHAOS_IL2CPP_SIZE bytes_promoted,
                                        CHAOS_IL2CPP_SIZE objects_in_gen1,
                                        uint64_t pause_ns) noexcept {
    gen1_alloc_since_last_gc_.store(0, std::memory_order_relaxed);
    gen0_since_last_gen1_gc_.store(0, std::memory_order_relaxed);

    // Update Gen1 EMA survival rate (object-count-based proxy).
    if (objects_in_gen1 > 0) {
        double survival = static_cast<double>(bytes_promoted) /
            static_cast<double>(objects_in_gen1);
        uint64_t old_bits = gen1_survival_rate_bits_.load(std::memory_order_relaxed);
        double old_rate = BitsToDouble(old_bits);
        double new_rate = (1.0 - kEmaAlpha) * old_rate + kEmaAlpha * survival;
        if (new_rate < 0.0) new_rate = 0.0;
        if (new_rate > 1.0) new_rate = 1.0;
        gen1_survival_rate_bits_.store(DoubleToBits(new_rate), std::memory_order_relaxed);

        // Accumulate pause time tracking.
        gen1_total_pause_ns_.fetch_add(pause_ns, std::memory_order_relaxed);
        gen1_total_promoted_bytes_.fetch_add(bytes_promoted, std::memory_order_relaxed);
    }

    // ── Compute recommended promotion age ──
    // Base threshold from Gen1 EMA survival rate:
    //   High survival (>60%) → threshold=1: most objects survive Gen1,
    //     minimize Gen2 copy cost by promoting fast.
    //   Moderate (20-60%)    → threshold=2: some filtering benefit.
    //   Low survival (<20%)  → threshold=3: most die in Gen1, keep them
    //     there longer for efficient filtering before Gen2 promotion.
    double gen1_survival = BitsToDouble(
        gen1_survival_rate_bits_.load(std::memory_order_relaxed));
    int threshold = kGen1MinPromotionAge;

    if (gen1_survival > 0.60) {
        threshold = kGen1MinPromotionAge;  // 1 — promote fast
    } else if (gen1_survival > 0.20) {
        threshold = 2;  // moderate filtering
    } else {
        threshold = 3;  // aggressive filtering
    }

    // Pause-time penalty: if Gen1 pause per promoted byte is high,
    // nudge the threshold up to reduce promotion frequency (fewer
    // objects per Gen1 collection = shorter pause).
    uint64_t total_pause = gen1_total_pause_ns_.load(std::memory_order_relaxed);
    CHAOS_IL2CPP_SIZE total_promoted =
        gen1_total_promoted_bytes_.load(std::memory_order_relaxed);
    if (total_promoted > 0 && total_pause > 0) {
        uint64_t ns_per_byte = total_pause / total_promoted;
        if (ns_per_byte > kGen1MaxNsPerByte) {
            // High pause cost: increase threshold, capped at max.
            threshold = std::min(threshold + 1, kGen1MaxPromotionAge);
        }
    }

    scheduler_recommended_threshold_.store(threshold, std::memory_order_release);
}

void GcScheduler::RecordGen1Allocation(CHAOS_IL2CPP_SIZE bytes) noexcept {
    gen1_alloc_since_last_gc_.fetch_add(bytes, std::memory_order_relaxed);
}

// ── Collection decision ──────────────────────────────────────────

GcCollectionKind GcScheduler::DecideCollection() const noexcept {
    // 0. NO_GC_REGION: no collection allowed.
    if (GcIsInNoGcRegion()) {
        return GcCollectionKind::NONE;
    }

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

    // Compute memory pressure ratio: how full the heap is relative to
    // the full-GC trigger.  >1.0 means we've exceeded the normal full GC
    // threshold but the BGC might be busy or deferred.
    float pressure_ratio = (heap_est > 0)
        ? static_cast<float>(alloc_full) / (static_cast<float>(heap_est) * kFullTriggerMultiplier)
        : 0.0f;

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

    // 3b. High memory pressure: total allocation since last full GC is
    // very high (>3× heap est) even though we're below the normal full
    // GC threshold (checked above).  Tighten the young GC trigger to
    // reduce old-gen promotion rate under sustained allocation.
    CHAOS_IL2CPP_SIZE alloc = alloc_since_last_gc_.load(std::memory_order_relaxed);
    CHAOS_IL2CPP_SIZE last_nursery = last_nursery_used_.load(std::memory_order_relaxed);

    // Scale young threshold by pressure: at pressure_ratio=1.0 (normal),
    // use kYoungTriggerMultiplier=2.0.  At pressure_ratio=2.0 (very high),
    // tighten to 1.0× for aggressive young GC pacing.
    float scaled_young_multiplier = kYoungTriggerMultiplier;
    if (pressure_ratio > 0.5f) {
        // Linear scale from 2.0× at 0.5 pressure to 1.0× at 2.0 pressure.
        scaled_young_multiplier = kYoungTriggerMultiplier * (1.5f - pressure_ratio * 0.5f);
        if (scaled_young_multiplier < 1.0f) scaled_young_multiplier = 1.0f;
    }

    if (alloc > static_cast<CHAOS_IL2CPP_SIZE>(last_nursery * scaled_young_multiplier)) {
        return GcCollectionKind::YOUNG;
    }

    // 3c. Extreme pressure: even if per-cycle alloc is modest, the
    // cumulative allocation since last full GC has grown beyond
    // kHighPressureTriggerMultiplier × heap.  Pre-emptively trigger
    // a young GC to slow the accumulation.
    if (heap_est > 0 &&
        alloc_full > static_cast<CHAOS_IL2CPP_SIZE>(heap_est * kHighPressureTriggerMultiplier) &&
        alloc > last_nursery * 0.5f) {
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

    // Adaptive multiplier based on survival rate:
    //   Survival > 50%: 3× — grow aggressively to buy more time between GCs
    //   Survival 10-50%: 2× — standard headroom
    //   Survival < 10%: 1.5× — shrink, most objects die young anyway
    double multiplier;
    if (survival > 0.50) {
        multiplier = 3.0;
    } else if (survival > 0.10) {
        multiplier = 2.0;
    } else {
        multiplier = 1.5;
    }

    // Allocation-rate boost: if threads are allocating fast, a larger nursery
    // reduces safepoint frequency.  Use total allocation since last GC as proxy.
    CHAOS_IL2CPP_SIZE last_used = last_nursery_used_.load(std::memory_order_relaxed);
    CHAOS_IL2CPP_SIZE alloc = alloc_since_last_gc_.load(std::memory_order_relaxed);
    double alloc_ratio = (last_used > 0)
        ? static_cast<double>(alloc) / static_cast<double>(last_used)
        : 0.0;

    // When allocation exceeds the young trigger threshold (2×), boost nursery
    // proportionally up to 1.5× extra.  This prevents high-throughput scenarios
    // (e.g., 100-thread stress test) from hitting safepoints too frequently.
    if (alloc_ratio > 2.0) {
        double boost = 1.0 + std::min((alloc_ratio - 2.0) * 0.25, 0.5);
        multiplier *= boost;
    }

    double target = survival * multiplier * static_cast<double>(last_used);

    // Fallback: if last_used is 0 or survival is near 0, use max nursery size
    // to reduce GC frequency (no point doing frequent GCs on an empty nursery).
    if (target < static_cast<double>(kMinNurserySize)) {
        target = static_cast<double>(kDefaultNurserySize);
    }

    CHAOS_IL2CPP_SIZE size = static_cast<CHAOS_IL2CPP_SIZE>(target);

    // ── Old-gen fragmentation discount ───────────────────────────
    // When old-gen fragmentation is high, shrink nursery to reduce
    // the promotion rate.  This gives old gen more collection cycles
    // to compact before new objects arrive.
    float frag = OldGenFragmentation();
    if (frag > 0.50f) {
        size = static_cast<CHAOS_IL2CPP_SIZE>(size * 0.50);
    } else if (frag > 0.30f) {
        size = static_cast<CHAOS_IL2CPP_SIZE>(size * 0.75);
    }

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

// ── Survivor sizing ────────────────────────────────────────────────

CHAOS_IL2CPP_SIZE GcScheduler::RecommendedSurvivorSize() const noexcept {
    // Base size: enough to hold at least 2 young-GC promotion cycles.
    // avg_promoted_bytes_ is the EMA of bytes promoted from nursery to
    // survivor per young GC.  Multiply by 2 for headroom (the Gen1
    // collection trigger is at threshold N, which can be up to 12, but
    // in practice the near-full occupancy check triggers earlier).
    CHAOS_IL2CPP_SIZE avg = avg_promoted_bytes_.load(std::memory_order_relaxed);
    double base = static_cast<double>(avg > 0 ? avg : kDefaultSurvivorSize / 4);
    base = base * 2.0;  // two cycles of headroom

    // Adjust by Gen1 survival rate:
    //   High survival (>60%) → shrink: objects pass through to Gen2
    //     quickly, less filtering capacity needed.
    //   Low survival (<30%)  → grow: lots of filtering happening,
    //     keep more space for dead objects to accumulate.
    double gen1_survival = BitsToDouble(
        gen1_survival_rate_bits_.load(std::memory_order_relaxed));
    if (gen1_survival > 0.60) {
        base *= 0.75;
    } else if (gen1_survival < 0.30 && gen1_survival > 0.0) {
        base *= 1.50;
    }

    // Round to page size (64 KB).
    constexpr CHAOS_IL2CPP_SIZE kSurvivorAlign = 64 * 1024;
    CHAOS_IL2CPP_SIZE size = static_cast<CHAOS_IL2CPP_SIZE>(base);
    size = (size + kSurvivorAlign - 1) & ~(kSurvivorAlign - 1);

    // Clamp to [kMinSurvivorSize, kMaxSurvivorSize].
    if (size < kMinSurvivorSize) size = kMinSurvivorSize;
    if (size > kMaxSurvivorSize) size = kMaxSurvivorSize;

    return size;
}

}  // namespace chaos::il2cpp::runtime_core