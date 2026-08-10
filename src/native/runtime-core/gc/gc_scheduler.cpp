#include "gc_scheduler.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <mutex>

#include "gc_bgc.h"
#include "gc_api.h"
#include "gc_gen1.h"
#include "gc_young_gen.h"
#include "gc_heap.h"

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
    G_YoungGen().promotion_age_threshold_.store(new_threshold, std::memory_order_release);

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

void GcScheduler::RecordGen1Collection(CHAOS_IL2CPP_SIZE bytes_promoted,
                                        CHAOS_IL2CPP_SIZE objects_in_gen1,
                                        uint64_t pause_ns) noexcept {
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

void GcScheduler::RecordGen1Allocation(CHAOS_IL2CPP_SIZE /*bytes*/) noexcept {
    // Gen1 budget tracking was removed in Phase 2a cleanup.
    // The gen1_alloc_since_last_gc_ field was deleted — this function
    // is retained for API compatibility (called from TryAllocateInGen1).
}

// ── Collection decision ──────────────────────────────────────────

BgcScope GcScheduler::DecideBgcScope() noexcept {
    BgcScope scope = BgcScope::GEN2_ONLY;

    Region* gen1 = G_YoungGen().gen1_region.load(std::memory_order_acquire);
    if (gen1 != nullptr) {
        auto* s_bump = G_YoungGen().gen1_bump.load(std::memory_order_acquire);
        CHAOS_IL2CPP_SIZE s_used = static_cast<CHAOS_IL2CPP_SIZE>(
            (s_bump ? s_bump : gen1->begin) -
            gen1->begin);
        CHAOS_IL2CPP_SIZE s_capacity = static_cast<CHAOS_IL2CPP_SIZE>(
            G_YoungGen().gen1_end - gen1->begin);

        if (s_capacity > 0) {
            float occupancy = static_cast<float>(s_used) /
                static_cast<float>(s_capacity);
            double gen1_survival = BitsToDouble(
                gen1_survival_rate_bits_.load(std::memory_order_relaxed));

            // Rule 1: Gen1 occupancy > 80% AND survival < 50% → collect Gen1+Gen2.
            if (occupancy > 0.80f && gen1_survival < 0.50) {
                scope = BgcScope::GEN1_GEN2;
            }
            // Rule 2: Gen2 fragmentation > 30% AND Gen1 survival > 60% → Gen2 only.
            float frag = OldGenFragmentation();
            if (frag > 0.30f && gen1_survival > 0.60) {
                scope = BgcScope::GEN2_ONLY;
            }

            CHAOS_IL2CPP_LOG_DEBUG_M("CRAG",
                "bgc_scope={0} gen1_occ={1:.2f} gen1_surv={2:.2f} gen2_frag={3:.2f}",
                static_cast<int>(scope), occupancy, gen1_survival,
                static_cast<double>(frag));
        }
    }

    bgc_scope_.store(static_cast<uint8_t>(scope), std::memory_order_release);
    RecordBgcScopeDecision(scope);
    return scope;
}

void GcScheduler::RecordBgcScopeDecision(BgcScope scope) noexcept {
    if (scope == BgcScope::GEN1_GEN2) {
        bgc_gen1_gen2_count_.fetch_add(1, std::memory_order_relaxed);
    } else {
        bgc_gen2_only_count_.fetch_add(1, std::memory_order_relaxed);
    }
}

void GcScheduler::RecordBgcGen1Promote(CHAOS_IL2CPP_SIZE bytes) noexcept {
    bgc_gen1_promote_bytes_.fetch_add(bytes, std::memory_order_relaxed);
}

void GcScheduler::RecordBgcGen1Keep(CHAOS_IL2CPP_SIZE bytes) noexcept {
    bgc_gen1_keep_bytes_.fetch_add(bytes, std::memory_order_relaxed);
}

GcCollectionKind GcScheduler::DecideCollection() const noexcept {
    // 0. NO_GC_REGION: no collection allowed.
    if (GcIsInNoGcRegion()) {
        return GcCollectionKind::NONE;
    }

    // 0b. Snapshot latency mode for consistent decisions.
    auto mode = latency_mode_.load(std::memory_order_acquire);

    // In low-latency modes, prefer FULL_BGC over STW FULL whenever BGC is
    // available.  If BGC is busy, return NONE rather than blocking mutators.
    bool prefer_bgc = (mode == GcLatencyMode::LOW_LATENCY ||
                       mode == GcLatencyMode::SUSTAINED_LOW_LATENCY);
    auto& bgc = BgcController::Instance();

    // Provisional (high-memory-pressure) mode: align CoreCLR's provisional
    // degradation (gcpriv.h:4324) — under memory pressure never defer to BGC
    // or NONE; force a blocking STOP-THE-WORLD FULL so memory is reclaimed
    // promptly instead of waiting on the concurrent background mark.
    bool provisional = provisional_mode_.load(std::memory_order_acquire);
    if (provisional) {
        prefer_bgc = false;
    }

    // 1. Full GC requested by another thread?
    if (full_gc_requested_.load(std::memory_order_acquire)) {
        SetLastTriggerReason(
            provisional ? GcTriggerReason::PROVISIONAL : GcTriggerReason::EXPLICIT_REQUEST);
        if (prefer_bgc && g_bgc_enabled && !bgc.IsBusy()) {
            return GcCollectionKind::FULL_BGC;
        }
        return GcCollectionKind::FULL;
    }

    // 2. Page count growth threshold exceeded?
    // Catches rapid page allocation bursts before the byte-based trigger reacts.
    if (page_count_growth_.load(std::memory_order_relaxed) >= kMaxPageGrowthThreshold) {
        SetLastTriggerReason(
            provisional ? GcTriggerReason::PROVISIONAL : GcTriggerReason::PAGE_GROWTH);
        if (g_bgc_enabled && !bgc.IsBusy() && !provisional) {
            return GcCollectionKind::FULL_BGC;
        }
        if (prefer_bgc && !provisional) {
            return GcCollectionKind::NONE;  // wait for BGC rather than STW
        }
        return GcCollectionKind::FULL;
    }

    // 3. Snapshot multi-variable state for consistent decision.
    CHAOS_IL2CPP_SIZE alloc_full = alloc_since_last_full_gc_.load(std::memory_order_relaxed);
    CHAOS_IL2CPP_SIZE heap_est = estimated_heap_size_.load(std::memory_order_relaxed);

    // 3a. External memory pressure contribution.
    // Treat outstanding external (unmanaged) pressure as additional managed
    // allocation for GC triggering.  This ensures that native allocations
    // (textures, audio, etc.) drive GC proactively rather than waiting for
    // managed allocation to cross the threshold on its own.
    CHAOS_IL2CPP_INT64 ext_pressure = external_memory_pressure_.load(std::memory_order_relaxed);
    if (ext_pressure > 0 && heap_est > 0) {
        CHAOS_IL2CPP_INT64 adaptive_threshold = std::max(
            kMinExternalPressureThreshold,
            static_cast<CHAOS_IL2CPP_INT64>(
                static_cast<float>(heap_est) * kExternalPressureRatio));
        if (ext_pressure > adaptive_threshold) {
            // Only count excess above the adaptive threshold to avoid
            // double-triggering: the edge trigger in AddExternalMemoryPressure
            // already requested a full GC when the threshold was first crossed.
            // The scheduler will pick up the request via full_gc_requested_
            // below.  Here we fold the excess into alloc_full for the
            // continuous-feedback path, so that sustained high pressure
            // keeps GC active even if the one-shot full_gc_requested_ flag
            // was consumed by a previous GC cycle.
            CHAOS_IL2CPP_INT64 excess = ext_pressure - adaptive_threshold;
            // Cap at 2x heap to prevent extreme spikes from runaway GC.
            CHAOS_IL2CPP_INT64 capped = std::min(
                excess, static_cast<CHAOS_IL2CPP_INT64>(
                    static_cast<float>(heap_est) * 2.0f));
            alloc_full += static_cast<CHAOS_IL2CPP_SIZE>(capped);
        }
    }

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
        if (g_bgc_enabled && !bgc.IsBusy()) {
            return GcCollectionKind::FULL_BGC;
        }
        // BGC is busy — in low-latency mode, wait for BGC rather than STW.
        if (prefer_bgc) {
            return GcCollectionKind::NONE;
        }
        // External pressure triggering full GC?
        bool ext_triggered = (ext_pressure > 0 && heap_est > 0 &&
            static_cast<CHAOS_IL2CPP_INT64>(alloc_full) > ext_pressure);
        SetLastTriggerReason(
            ext_triggered
                ? (provisional ? GcTriggerReason::PROVISIONAL : GcTriggerReason::EXTERNAL_PRESSURE)
                : (provisional ? GcTriggerReason::PROVISIONAL : GcTriggerReason::ALLOC_PRESSURE));
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

void GcScheduler::AddExternalMemoryPressure(CHAOS_IL2CPP_INT64 bytes) noexcept {
    if (bytes <= 0) return;

    auto prev = external_memory_pressure_.fetch_add(bytes, std::memory_order_relaxed);
    auto current = prev + bytes;

    // Compute adaptive threshold: at least kMinExternalPressureThreshold,
    // but also scale with the managed heap so small heaps aren't over-triggered.
    CHAOS_IL2CPP_SIZE heap_est = estimated_heap_size_.load(std::memory_order_relaxed);
    CHAOS_IL2CPP_INT64 adaptive_threshold = std::max(
        kMinExternalPressureThreshold,
        static_cast<CHAOS_IL2CPP_INT64>(static_cast<float>(heap_est) * kExternalPressureRatio));

    // Edge trigger: crossed threshold upward → request a full GC.
    if (prev < adaptive_threshold && current >= adaptive_threshold) {
        RequestFullGc();
        CHAOS_IL2CPP_LOG_DEBUG_M("GC_API",
            "external_memory_pressure triggered GC: total=%lld threshold=%lld",
            static_cast<long long>(current),
            static_cast<long long>(adaptive_threshold));
    }
}

void GcScheduler::RemoveExternalMemoryPressure(CHAOS_IL2CPP_INT64 bytes) noexcept {
    if (bytes <= 0) return;

    auto prev = external_memory_pressure_.fetch_sub(bytes, std::memory_order_relaxed);
    // Saturate at 0 (can't have negative pressure).
    if (prev < bytes) {
        external_memory_pressure_.store(0, std::memory_order_relaxed);
    }

    CHAOS_IL2CPP_LOG_DEBUG_M("GC_API",
        "external_memory_pressure removed: delta=%lld total=%lld",
        static_cast<long long>(bytes),
        static_cast<long long>(external_memory_pressure_.load(std::memory_order_relaxed)));
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

// ── Gen1 sizing ───────────────────────────────────────────────────

CHAOS_IL2CPP_SIZE GcScheduler::RecommendedGen1Size() const noexcept {
    // Base: enough to hold at least 2 young-GC promotion cycles.
    // Use the EMA average of nursery→Gen1 promotion bytes (avg_promoted_bytes_).
    // Multiply by 2 for headroom.
    CHAOS_IL2CPP_SIZE avg = avg_promoted_bytes_.load(std::memory_order_relaxed);
    double base = static_cast<double>(avg > 0 ? avg : kDefaultGen1Size / 4);
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

    // Round to Gen1 region alignment (64 KB).
    constexpr CHAOS_IL2CPP_SIZE kGen1Align = 64 * 1024;
    CHAOS_IL2CPP_SIZE size = static_cast<CHAOS_IL2CPP_SIZE>(base);
    size = (size + kGen1Align - 1) & ~(kGen1Align - 1);

    // Clamp to [kMinGen1Size, kMaxGen1Size].
    if (size < kMinGen1Size) size = kMinGen1Size;
    if (size > kMaxGen1Size) size = kMaxGen1Size;

    return size;
}

// ── Full GC notification ───────────────────────────────────────────

void GcScheduler::EnableFullGcNotification() noexcept {
    std::lock_guard<std::mutex> lock(notification_mutex_);
    fullgc_notification_enabled_.store(true, std::memory_order_release);
    fullgc_approach_signaled_ = false;
    fullgc_complete_signaled_ = false;
}

void GcScheduler::DisableFullGcNotification() noexcept {
    std::lock_guard<std::mutex> lock(notification_mutex_);
    fullgc_notification_enabled_.store(false, std::memory_order_release);
    fullgc_approach_signaled_ = false;
    fullgc_complete_signaled_ = false;
    approach_cv_.notify_all();
    complete_cv_.notify_all();
}

void GcScheduler::SignalFullGcApproach() noexcept {
    if (!fullgc_notification_enabled_.load(std::memory_order_acquire)) return;
    std::lock_guard<std::mutex> lock(notification_mutex_);
    fullgc_approach_signaled_ = true;
    approach_cv_.notify_all();
}

void GcScheduler::SignalFullGcComplete() noexcept {
    if (!fullgc_notification_enabled_.load(std::memory_order_acquire)) return;
    std::lock_guard<std::mutex> lock(notification_mutex_);
    fullgc_complete_signaled_ = true;
    complete_cv_.notify_all();
}

bool GcScheduler::WaitForFullGcApproach(int32_t timeout_ms) noexcept {
    if (!g_bgc_enabled) return false;
    std::unique_lock<std::mutex> lock(notification_mutex_);
    if (!fullgc_notification_enabled_.load(std::memory_order_acquire)) return false;
    fullgc_approach_signaled_ = false;
    if (timeout_ms <= 0) {
        approach_cv_.wait(lock, [this]() {
            return fullgc_approach_signaled_ || !fullgc_notification_enabled_.load(std::memory_order_acquire);
        });
    } else {
        approach_cv_.wait_for(lock, std::chrono::milliseconds(timeout_ms), [this]() {
            return fullgc_approach_signaled_ || !fullgc_notification_enabled_.load(std::memory_order_acquire);
        });
    }
    return fullgc_approach_signaled_;
}

bool GcScheduler::WaitForFullGcComplete(int32_t timeout_ms) noexcept {
    if (!g_bgc_enabled) return false;
    std::unique_lock<std::mutex> lock(notification_mutex_);
    if (!fullgc_notification_enabled_.load(std::memory_order_acquire)) return false;
    fullgc_complete_signaled_ = false;
    if (timeout_ms <= 0) {
        complete_cv_.wait(lock, [this]() {
            return fullgc_complete_signaled_ || !fullgc_notification_enabled_.load(std::memory_order_acquire);
        });
    } else {
        complete_cv_.wait_for(lock, std::chrono::milliseconds(timeout_ms), [this]() {
            return fullgc_complete_signaled_ || !fullgc_notification_enabled_.load(std::memory_order_acquire);
        });
    }
    return fullgc_complete_signaled_;
}

}  // namespace chaos::il2cpp::runtime_core