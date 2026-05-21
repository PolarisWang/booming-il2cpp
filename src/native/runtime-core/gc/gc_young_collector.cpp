#include "gc_young_collector.h"

#include <chaos/log.h>
#include <chaos/profile.h>


#include "gc_card_table.h"
#include "gc_events.h"
#include "gc_bgc.h"
#include "gc_gen1.h"
#include "gc_layout.h"
#include "gc_loh.h"
#include "gc_old_gen.h"
#include "gc_young_gen.h"
#include "gc_scheduler.h"
#include "gc_stats.h"

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// Forwarding pointer protocol
//
// During Cheney copying, the first word of a nursery object (normally
// TypeInfoHot*) is overwritten with a forwarding pointer to the tenured
// copy.  The lowest bit is set to distinguish it from a valid TypeInfo*
// (which is always aligned to at least 4 bytes).
// ======================================================================

static constexpr uintptr_t kForwardingTag = 1u;

inline bool IsForwarded(const void* obj) {
    auto word = *static_cast<const uintptr_t*>(obj);
    return (word & kForwardingTag) != 0;
}

inline void* GetForwardingAddress(const void* obj) {
    auto word = *static_cast<const uintptr_t*>(obj);
    return reinterpret_cast<void*>(word & ~static_cast<uintptr_t>(kForwardingTag));
}

inline void SetForwardingAddress(void* obj, void* target) {
    *static_cast<uintptr_t*>(obj) =
        reinterpret_cast<uintptr_t>(target) | static_cast<uintptr_t>(kForwardingTag);
}

// ======================================================================
// Helpers
// ======================================================================

bool IsInNursery(const void* ptr) {
    // Check if the pointer falls within any active nursery region across
    // all threads by querying the RegionManager.  This catches pointers
    // from one thread's old-gen write into another thread's nursery.
    if (ptr == nullptr) return false;
    return RegionManager::Instance().IsNurseryPointer(ptr);
}

/// Estimate the object size from its address and the nursery region bounds.
/// This is a fallback when TypeInfo doesn't carry instance_size directly.
/// Uses a conservative cap (2048 = largest bump cache size class) so the
/// Phase 2 precise nursery scan never advances past valid objects even when
/// a large gap exists between obj and nursery->current.  Objects larger than
/// 2048 bytes are typically LOH-allocated and never appear in the nursery.
static constexpr CHAOS_IL2CPP_SIZE kMaxEstObjectSize = 2048;

static CHAOS_IL2CPP_SIZE EstimateObjectSize(const void* obj, const Region* nursery) {
    if (nursery == nullptr) return kMaxEstObjectSize;
    uintptr_t start = reinterpret_cast<uintptr_t>(obj);
    uintptr_t end   = reinterpret_cast<uintptr_t>(nursery->current);
    if (start >= end) return kMaxEstObjectSize;
    CHAOS_IL2CPP_SIZE remaining = static_cast<CHAOS_IL2CPP_SIZE>(end - start);
    // Conservative: never return more than kMaxEstObjectSize to avoid
    // skipping over valid objects in the Phase 2 nursery scan.
    return (remaining < kMaxEstObjectSize) ? remaining : kMaxEstObjectSize;
}

/// Try to determine object size from its TypeInfo/GcLayout.
/// Returns 0 if the layout is not available (caller should fall back
/// to EstimateObjectSize).
static CHAOS_IL2CPP_SIZE PreciseObjectSize(const void* obj) {
    const void* type_info_ptr = *static_cast<const void* const*>(obj);
    if (type_info_ptr == nullptr) return 0;
    auto& layout_registry = GcLayoutRegistry::Instance();
    if (!layout_registry.IsValidTypeInfoPointer(type_info_ptr)) return 0;
    uint64_t stable_id = layout_registry.ReadStableId(type_info_ptr);
    const auto* layout = layout_registry.Lookup(stable_id);
    if (layout == nullptr || layout->instance_size == 0) return 0;
    return layout->instance_size;
}

// ======================================================================
// Cheney copy
// ======================================================================

/// Try to allocate in the Gen1 survivor area.
/// Returns nullptr if the Gen1 area is exhausted — caller should fall
/// back to old gen.  Thread-safe via CAS on gen1_bump.
static void* TryAllocateInSurvivor(CHAOS_IL2CPP_SIZE size) noexcept {
    char* current = g_young_gen.gen1_bump.load(std::memory_order_acquire);
    char* next;
    do {
        if (current == nullptr) return nullptr;
        next = current + size;
        if (next > g_young_gen.gen1_end) return nullptr;
    } while (!g_young_gen.gen1_bump.compare_exchange_weak(
        current, next, std::memory_order_release, std::memory_order_acquire));
    std::memset(current, 0, size);
    return current;
}

/// Determine whether an address falls within the Gen1 survivor region.
/// File-local helper (the non-static IsInGen1 in gc_gen1.cpp does the same
/// thing for external callers).
static bool IsInGen1Local(const void* ptr) noexcept {
    auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    if (gen1 == nullptr) return false;
    auto* bump = g_young_gen.gen1_bump.load(std::memory_order_acquire);
    if (bump == nullptr) return false;
    auto* cptr = static_cast<const char*>(ptr);
    return cptr >= gen1->begin && cptr < bump;
}

void* GcScavengeObject(void* obj, YoungCollectionResult* result) {
    if (obj == nullptr) return nullptr;
    if (!IsInNursery(obj)) {
        // Already in tenured — no copy needed.
        return obj;
    }
    if (IsForwarded(obj)) {
        // Already copied — return forwarding address.
        return GetForwardingAddress(obj);
    }

    // Determine object size.  Try precise TypeInfo-based sizing first for
    // all objects (own-thread and cross-thread).  Fall back to nursery-bounds
    // estimation when the layout is not available (e.g., unregistered types).
    CHAOS_IL2CPP_SIZE obj_size = PreciseObjectSize(obj);
    if (obj_size == 0) {
        // Precise sizing unavailable — use nursery-bounds estimation.
        // This can happen for unregistered types or objects whose TypeInfo
        // has been overwritten by a forwarding pointer from a concurrent
        // scavenge (rare, but possible in cross-thread scenarios).
        Region* nursery_local = g_young_gen.region.load(std::memory_order_acquire);
        if (nursery_local != nullptr &&
            reinterpret_cast<uintptr_t>(obj) >= reinterpret_cast<uintptr_t>(nursery_local->begin) &&
            reinterpret_cast<uintptr_t>(obj) < reinterpret_cast<uintptr_t>(nursery_local->current)) {
            obj_size = EstimateObjectSize(obj, nursery_local);
        } else {
            // Cross-thread object with no layout — use conservative cap.
            // This wastes old-gen memory by over-allocating but never
            // truncates, avoiding cross-thread data corruption.
            obj_size = kMaxEstObjectSize;
        }
    }

    void* tenured = g_old_gen.Allocate(obj_size, true);
    if (tenured == nullptr) return nullptr;

    // Copy the object contents.
    std::memcpy(tenured, obj, obj_size);

    // Place forwarding pointer at the nursery location.
    SetForwardingAddress(obj, tenured);

    // Count promotion.
    if (result) {
        result->objects_promoted++;
        result->bytes_promoted += obj_size;

        // Add to Cheney BFS worklist for transitive closure (Phase 3).
        if (result->bfs_worklist && result->bfs_worklist_count < result->bfs_worklist_capacity) {
            result->bfs_worklist[result->bfs_worklist_count++] = tenured;
        }
    }

    CHAOS_IL2CPP_LOG_DEBUG("CRAG", "scavenge_object");
    return tenured;
}

void* GcScavengeObjectKnownNursery(void* obj, YoungCollectionResult* result) {
    if (obj == nullptr) return nullptr;
    // Caller already verified obj is in nursery — skip IsInNursery check.
    if (IsForwarded(obj)) {
        return GetForwardingAddress(obj);
    }

    CHAOS_IL2CPP_SIZE obj_size = PreciseObjectSize(obj);
    if (obj_size == 0) {
        Region* nursery_local = g_young_gen.region.load(std::memory_order_acquire);
        if (nursery_local != nullptr &&
            reinterpret_cast<uintptr_t>(obj) >= reinterpret_cast<uintptr_t>(nursery_local->begin) &&
            reinterpret_cast<uintptr_t>(obj) < reinterpret_cast<uintptr_t>(nursery_local->current)) {
            obj_size = EstimateObjectSize(obj, nursery_local);
        } else {
            obj_size = kMaxEstObjectSize;
        }
    }

    // ── Age tenuring: determine destination based on current location ──
    // Objects in Gen1 (survivor area) have survived at least one young GC.
    // With dynamic threshold, they may stay in Gen1 for more cycles
    // before promotion (threshold > 1 → copy back to Gen1).
    // Objects in Gen0 (young half) always copy to Gen1 (first survival).
    void* target;
    if (IsInGen1(obj)) {
        int threshold = g_young_gen.promotion_age_threshold_.load(
            std::memory_order_acquire);
        if (threshold > 1) {
            target = TryAllocateInGen1(obj_size);
            if (target == nullptr) {
                target = g_old_gen.Allocate(obj_size, true);
            }
        } else {
            target = g_old_gen.Allocate(obj_size, true);
        }
    } else {
        target = TryAllocateInGen1(obj_size);
        if (target == nullptr) {
            target = g_old_gen.Allocate(obj_size, true);
        }
    }

    if (target == nullptr) return nullptr;

    std::memcpy(target, obj, obj_size);
    SetForwardingAddress(obj, target);

    if (result) {
        result->objects_promoted++;
        result->bytes_promoted += obj_size;
        if (result->bfs_worklist && result->bfs_worklist_count < result->bfs_worklist_capacity) {
            result->bfs_worklist[result->bfs_worklist_count++] = target;
        }
    }

    CHAOS_IL2CPP_LOG_DEBUG("CRAG", "scavenge_object_known_nursery");
    return target;
}

// ======================================================================
// Young collection
// ======================================================================

YoungCollectionResult GcYoungCollection(bool force_skip_gen1) {
    // If force_skip_gen1 is true, skip Phase 4 Gen1 collection even if
    // promotion_age_threshold_ would normally trigger it.  Used for the
    // gen=0 (young-only) path in chaos_gc_collect_with_mode.
    (void)force_skip_gen1;

    YoungCollectionResult result = {};

    // ── Phase timing (for per-GC-cycle trace) ──────────────────────────
    struct PhaseTime {
        decltype(std::chrono::steady_clock::now()) phase1_start;
        decltype(std::chrono::steady_clock::now()) phase2_start;
        decltype(std::chrono::steady_clock::now()) phase2b_start;
        decltype(std::chrono::steady_clock::now()) phase3_start;
        decltype(std::chrono::steady_clock::now()) phase3b_start;
        decltype(std::chrono::steady_clock::now()) phase4_start;
        decltype(std::chrono::steady_clock::now()) phase4_end;
        uint64_t phase1_ns = 0, phase2_ns = 0, phase2b_ns = 0;
        uint64_t phase3_ns = 0, phase3b_ns = 0, phase4_ns = 0;
    } pt;

    Region* nursery = g_young_gen.region.load(std::memory_order_acquire);
    if (nursery == nullptr) {
        CHAOS_IL2CPP_LOG_WARN("CRAG", "young_collection_no_region");
        return result;
    }

    auto pause_start = std::chrono::steady_clock::now();

    uintptr_t nursery_begin = reinterpret_cast<uintptr_t>(nursery->begin);
    // With shared young gen + TLABs, TlabClaimFromYoungGen() updates
    // g_young_gen.bump, NOT nursery->current.  Read the actual allocation
    // frontier from the shared bump pointer so the nursery scan covers
    // all objects in all TLABs.
    uintptr_t nursery_used  = reinterpret_cast<uintptr_t>(
        g_young_gen.bump.load(std::memory_order_acquire));

    CHAOS_IL2CPP_LOG_INFO_M("CRAG", "young_collection nursery=[{0}, {1}) usage={2}",
        static_cast<void*>(nursery->begin),
        static_cast<void*>(nursery->end),
        static_cast<unsigned long long>(nursery_used - nursery_begin));

    GcFireEvent(GcEvent::GC_YOUNG_START);

    // ── Phase 1: Scan dirty cards for old→young cross-gen references ──
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("GC_Phase1_DirtyCards");
        result.dirty_cards_scanned = 0;
        // Batched scan: groups consecutive dirty cards into ranges,
        // reducing per-card callback overhead (common when large object
        // arrays span multiple cards).
        // Scan old-gen pages for Gen2→nursery references.
        g_old_gen.ScanDirtyCardsInPagesBatched(
            &result.dirty_cards_scanned,
            [&](uintptr_t range_start, uintptr_t range_end) {
                for (uintptr_t slot = range_start; slot < range_end; slot += sizeof(void*)) {
                    auto* ptr_slot = reinterpret_cast<void**>(slot);
                    void* val = *ptr_slot;
                    if (val != nullptr && IsInNursery(val)) {
                        void* tenured = GcScavengeObjectKnownNursery(val, &result);
                        if (tenured != nullptr) {
                            *ptr_slot = tenured;
                        }
                    }
                }
            });
        // Scan LOH segments for LOH→nursery references.
        // LOH segments are registered with the card table, so DirtyCard()
        // tracks all pointer writes into LOH objects during the mutator phase.
        g_loh.ScanDirtyCardsInSegmentsBatched(
            &result.dirty_cards_scanned,
            [&](uintptr_t range_start, uintptr_t range_end) {
                for (uintptr_t slot = range_start; slot < range_end; slot += sizeof(void*)) {
                    auto* ptr_slot = reinterpret_cast<void**>(slot);
                    void* val = *ptr_slot;
                    if (val != nullptr && IsInNursery(val)) {
                        void* tenured = GcScavengeObjectKnownNursery(val, &result);
                        if (tenured != nullptr) {
                            *ptr_slot = tenured;
                        }
                    }
                }
            });
    }
    pt.phase1_ns = static_cast<uint64_t>(std::chrono::duration_cast<
        std::chrono::nanoseconds>(std::chrono::steady_clock::now() - pause_start).count());

    // ── Phase 2: Precise object-by-object nursery scan ──
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("GC_Phase2_NurseryScan");
        uintptr_t scan_ptr = nursery_begin;
        auto& layout_registry = GcLayoutRegistry::Instance();

        // Fast skip: sample the first 64 pointer-sized slots.  If none
        // contain a valid TypeInfo pointer, the nursery holds only raw
        // (untyped) memory — skip the full 16MB linear scan.
        // This is common in stress-test scenarios with malloc-like
        // allocations; production code always has valid TypeInfo.
        {
            CHAOS_IL2CPP_PROFILE_SCOPE("GC_Phase2_FastSkipCheck");
            uintptr_t check_limit = nursery_begin + static_cast<uintptr_t>(
                std::min<CHAOS_IL2CPP_SIZE>(
                    nursery_used - nursery_begin, 64 * sizeof(void*)));
            bool found_type = false;
            for (uintptr_t c = nursery_begin; c < check_limit; c += sizeof(void*)) {
                const void* fw = *reinterpret_cast<const void* const*>(c);
                if (fw != nullptr && layout_registry.IsValidTypeInfoPointer(fw)) {
                    found_type = true;
                    break;
                }
            }
            if (!found_type) {
                CHAOS_IL2CPP_LOG_DEBUG("CRAG", "young_collection_skip_phase2 no_typed_objects");
                goto phase3;  // skip full scan
            }
        }

        // Cache the last TypeInfo/layout pair to skip redundant
        // IsValidTypeInfoPointer + ReadStableId + Lookup for adjacent
        // objects of the same type (common for arrays and sequential
        // allocations).  Initialize to nullptr so the first iteration
        // always takes the slow path.
        const void* last_first_word = nullptr;
        const GcTypeLayout* last_layout = nullptr;

        while (scan_ptr < nursery_used) {
            auto* obj = reinterpret_cast<void*>(scan_ptr);
            const void* first_word = *static_cast<const void* const*>(obj);
            if (first_word == nullptr) {
                scan_ptr += sizeof(void*);
                continue;
            }

            // Fast path: same TypeInfo as the previous object.
            const GcTypeLayout* layout;
            if (first_word == last_first_word) {
                layout = last_layout;
            } else {
                if (!layout_registry.IsValidTypeInfoPointer(first_word)) {
                    scan_ptr += sizeof(void*);
                    continue;
                }
                uint64_t stable_id = layout_registry.ReadStableId(first_word);
                layout = layout_registry.Lookup(stable_id);
                last_first_word = first_word;
                last_layout = layout;
            }

            if (layout == nullptr) {
                CHAOS_IL2CPP_SIZE obj_size = EstimateObjectSize(obj, nursery);
                scan_ptr += obj_size;
                continue;
            }

            uint32_t obj_size = layout->instance_size;
            for (uint16_t i = 0; i < layout->pointer_count; i++) {
                uint16_t offset = layout->pointer_offsets[i].offset;
                auto* slot = reinterpret_cast<void**>(scan_ptr + offset);
                void* val = *slot;
                if (val == nullptr) continue;
                if (IsInNursery(val)) {
                    void* tenured = GcScavengeObjectKnownNursery(val, &result);
                    if (tenured != nullptr && tenured != val) {
                        *slot = tenured;
                    }
                }
            }
            scan_ptr += obj_size;
        }
    }
    pt.phase2_ns = static_cast<uint64_t>(std::chrono::duration_cast<
        std::chrono::nanoseconds>(std::chrono::steady_clock::now() - pause_start).count()) - pt.phase1_ns;

    // ── Phase 2b: Gen1 dirty card scan ──
    // The Gen1 region is registered with the card table, so DirtyCard()
    // tracks all pointer writes into Gen1 objects during the mutator
    // phase.  Scan only dirty cards in the Gen1 range for efficiency,
    // replacing the prior full-walk of every Gen1 object.
    //
    // This is correct because at young GC start, the nursery was empty
    // (bump reset).  Any Gen1→nursery pointer must have been
    // established AFTER ClearAllCards(), which means the write barrier
    // set the corresponding card dirty.
    {
        auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
        if (gen1 != nullptr) {
            char* g1_bump = g_young_gen.gen1_bump.load(std::memory_order_acquire);
            if (g1_bump > gen1->begin) {
                ScanDirtyCardsBatched(
                    reinterpret_cast<uintptr_t>(gen1->begin),
                    reinterpret_cast<uintptr_t>(g1_bump),
                    &result.dirty_cards_scanned,
                    [&](uintptr_t range_start, uintptr_t range_end) {
                        for (uintptr_t slot = range_start; slot < range_end; slot += sizeof(void*)) {
                            auto* ptr_slot = reinterpret_cast<void**>(slot);
                            void* val = *ptr_slot;
                            if (val != nullptr && IsInNursery(val)) {
                                void* tenured = GcScavengeObjectKnownNursery(val, &result);
                                if (tenured != nullptr) {
                                    *ptr_slot = tenured;
                                }
                            }
                        }
                    });
            }
        }
    }
    pt.phase2b_ns = static_cast<uint64_t>(std::chrono::duration_cast<
        std::chrono::nanoseconds>(std::chrono::steady_clock::now() - pause_start).count())
        - pt.phase1_ns - pt.phase2_ns;

phase3:
    // ── Phase 3: Cheney BFS ──
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("GcYoungCollection::CheneyBfs");
        thread_local void* tls_bfs_worklist = nullptr;
        thread_local int tls_bfs_capacity = 0;

        int worklist_cap = 64 * 1024;
        if (tls_bfs_worklist == nullptr) {
            tls_bfs_worklist = std::malloc(static_cast<size_t>(worklist_cap) * sizeof(void*));
            tls_bfs_capacity = (tls_bfs_worklist != nullptr) ? worklist_cap : 0;
        }
        auto* worklist = static_cast<void**>(tls_bfs_worklist);
        if (worklist != nullptr) {
            result.bfs_worklist = worklist;
            result.bfs_worklist_capacity = tls_bfs_capacity;
            result.bfs_worklist_count = 0;

            int idx = 0;
            auto& layout_registry = GcLayoutRegistry::Instance();
            while (idx < result.bfs_worklist_count) {
                if (result.bfs_worklist_count + 256 >= result.bfs_worklist_capacity) {
                    int new_cap = result.bfs_worklist_capacity * 2;
                    auto* new_wl = static_cast<void**>(std::realloc(
                        result.bfs_worklist, static_cast<size_t>(new_cap) * sizeof(void*)));
                    if (new_wl != nullptr) {
                        result.bfs_worklist = new_wl;
                        result.bfs_worklist_capacity = new_cap;
                        worklist = new_wl;
                        tls_bfs_worklist = new_wl;
                        tls_bfs_capacity = new_cap;
                    } else {
                        break;
                    }
                }

                void* promoted = worklist[idx++];
                const void* type_info_ptr = *static_cast<const void* const*>(promoted);
                if (type_info_ptr == nullptr) continue;
                if (!layout_registry.IsValidTypeInfoPointer(type_info_ptr)) continue;

                uint64_t stable_id = layout_registry.ReadStableId(type_info_ptr);
                const auto* layout = layout_registry.Lookup(stable_id);

                if (layout == nullptr || layout->pointer_count == 0) {
                    continue;
                }

                ScanObjectPointers(promoted, layout, &result);
            }
        }
    }
    pt.phase3_ns = static_cast<uint64_t>(std::chrono::duration_cast<
        std::chrono::nanoseconds>(std::chrono::steady_clock::now() - pause_start).count())
        - pt.phase1_ns - pt.phase2_ns - pt.phase2b_ns;

    // ── Phase 3b: Process weak GCHandles ──
    GcProcessWeakHandlesAfterYoungGC();

    // Phase 3c: Process dependent handles (ConditionalWeakTable).
    GcProcessDependentHandlesAfterYoungGC();
    pt.phase3b_ns = static_cast<uint64_t>(std::chrono::duration_cast<
        std::chrono::nanoseconds>(std::chrono::steady_clock::now() - pause_start).count())
        - pt.phase1_ns - pt.phase2_ns - pt.phase2b_ns - pt.phase3_ns;

    // ── Phase 4: Sweep young generation ──
    // Reset g_young_gen.bump to the beginning of the region.
    // Clear all threads' TLAB ranges so they re-claim on next allocation.
    CHAOS_IL2CPP_SIZE nursery_used_bytes = static_cast<CHAOS_IL2CPP_SIZE>(
        nursery_used - nursery_begin);
    result.bytes_reclaimed = nursery_used_bytes;

    // Increment young GC counter for dynamic promotion threshold.
    int gc_count = g_young_gen.young_gc_count_.fetch_add(1, std::memory_order_relaxed) + 1;

    // ── Phase 4: Gen1 collection ──
    // Trigger Gen1 (survivor) collection based on promotion_age_threshold.
    // When force_skip_gen1 is true, skip Gen1 entirely (gen=0 young-only path).
    if (!force_skip_gen1) {
        int threshold = g_young_gen.promotion_age_threshold_.load(
            std::memory_order_acquire);
        bool should_collect_gen1 = (threshold <= 1) || (gc_count % threshold == 0);

        auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
        if (gen1 != nullptr) {
            char* g1_bump = g_young_gen.gen1_bump.load(std::memory_order_acquire);
            CHAOS_IL2CPP_SIZE g1_used = static_cast<CHAOS_IL2CPP_SIZE>(
                (g1_bump ? g1_bump : gen1->begin) - gen1->begin);
            constexpr CHAOS_IL2CPP_SIZE kGen1NearFullThreshold = 4096;
            CHAOS_IL2CPP_SIZE g1_capacity = static_cast<CHAOS_IL2CPP_SIZE>(
                g_young_gen.gen1_end - gen1->begin);
            bool near_full = (g1_used + kGen1NearFullThreshold >= g1_capacity);

            if (g1_used > 0 && (should_collect_gen1 || near_full)) {
                // Skip Gen1 collection if BGC has an active Gen1 mark bitmap
                // (GEN1_GEN2 scope).  The BGC is concurrently marking Gen1
                // objects and will handle promotion/keep in StwCompact.
                if (BgcController::Instance().IsGen1MarkingActive()) {
                    CHAOS_IL2CPP_LOG_DEBUG("CRAG",
                        "gen1_bgc_marking_active — skip young Gen1 collection");
                } else {
                auto gen1_result = GcGen1Collection();
                // GcGen1Collection resets gen1_bump after collection.
                result.objects_promoted += gen1_result.objects_promoted;
                result.bytes_promoted += gen1_result.bytes_promoted;
                GcRecordGen1Collection(
                    gen1_result.objects_promoted,
                    gen1_result.bytes_promoted,
                    gen1_result.bytes_reclaimed,
                    gen1_result.pause_ns);
                g_gc_scheduler.RecordGen1Collection(
                    gen1_result.bytes_promoted,
                    gen1_result.objects_in_gen1,
                    gen1_result.pause_ns);

                // ── Dynamic Gen1 resize after collection ──
                // Gen1 is now empty (bump was reset).  If the scheduler
                // recommends a significantly different size, reallocate.
                CHAOS_IL2CPP_SIZE recommended = g_gc_scheduler.RecommendedGen1Size();
                CHAOS_IL2CPP_SIZE current_size = static_cast<CHAOS_IL2CPP_SIZE>(
                    g_young_gen.gen1_end - gen1->begin);
                CHAOS_IL2CPP_SIZE diff = (recommended > current_size)
                    ? (recommended - current_size)
                    : (current_size - recommended);
                if (diff > current_size / 6 && recommended > 0) {
                    ResizeGen1Region(recommended);
                    CHAOS_IL2CPP_LOG_DEBUG("CRAG",
                        "gen1_resize: {0} -> {1} bytes",
                        static_cast<unsigned long long>(current_size),
                        static_cast<unsigned long long>(recommended));
                }

                // ── Read scheduler-recommended promotion age ──
                // The scheduler computes the recommended threshold from
                // Gen1 EMA survival rate (smoothing out per-collection noise)
                // and pause-time cost awareness.  This replaces the previous
                // ad-hoc tuning using raw per-collection survival rate.
                int scheduler_threshold = g_gc_scheduler.GetRecommendedPromotionAge();

                // Gen1-occupancy override: if the Gen1 area is nearly
                // full after this collection, force threshold to 1 regardless
                // of the scheduler recommendation, preventing overflow.
                float occupancy = static_cast<float>(g1_used) /
                    static_cast<float>(g1_capacity);
                if (occupancy > 0.90f && scheduler_threshold > 1) {
                    scheduler_threshold = 1;
                }

                if (scheduler_threshold !=
                    g_young_gen.promotion_age_threshold_.load(
                        std::memory_order_relaxed)) {
                    g_young_gen.promotion_age_threshold_.store(
                        scheduler_threshold, std::memory_order_release);
                    CHAOS_IL2CPP_LOG_DEBUG("CRAG",
                        "promotion_age_threshold: {0} (scheduler, "
                        "occ={1:.2f})",
                        scheduler_threshold, occupancy);
                }
                }
            }
            // else: skip Gen1 collection — let objects age in Gen1.
            // Gen1 bump is NOT reset, preserving contents for next young GC.
        }
    }

    // Reset the shared young region bump pointer.
    nursery->current = nursery->begin;
    g_young_gen.bump.store(nursery->begin, std::memory_order_release);
    g_young_gen.region_end.store(nursery->end, std::memory_order_release);

    // ── Note: dynamic Gen1 resizing is deferred to Phase C. ──
    // The scheduler's RecommendedSurvivorSize() value will be used in a future
    // phase to allocate a larger/smaller independent Gen1 region via
    // RegionManager.  For now, Gen1 is a fixed independent region whose size
    // is set at init time (see InitYoungGeneration).

    // Clear ALL threads' TLAB ranges via EnumerateThreads.
    threading::EnumerateThreads(
        [](threading::ManagedThread* thread) -> bool {
            thread->tlab_start = nullptr;
            thread->tlab_current = nullptr;
            return true;
        });

    // Reset this thread's TLAB.
    tls_tlab = TLAB();

    // Clear card table entries covering the young range.
    ClearAllCards();

    auto pause_end = std::chrono::steady_clock::now();
    uint64_t pause_ns = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(pause_end - pause_start).count());
    pt.phase4_ns = pause_ns - pt.phase1_ns - pt.phase2_ns - pt.phase2b_ns - pt.phase3_ns - pt.phase3b_ns;

    GcRecordYoungCollection(
        result.objects_promoted,
        result.bytes_promoted,
        result.bytes_reclaimed,
        result.dirty_cards_scanned,
        pause_ns);

    g_gc_scheduler.RecordYoungCollection(
        nursery_used_bytes, result.bytes_promoted, pause_ns);

    // Report old-gen fragmentation for adaptive nursery sizing.
    // High fragmentation → shrink nursery to reduce promotion rate.
    g_gc_scheduler.SetOldGenFragmentation(g_old_gen.OverallFragmentation());

    CHAOS_IL2CPP_LOG_INFO_M("CRAG",
        "young trace promoted={0} prom_bytes={1} reclaimed={2} cards={3} "
        "total_us={4} p1_us={5} p2_us={6} p2b_us={7} p3_us={8} p3b_us={9} p4_us={10}",
        static_cast<unsigned long long>(result.objects_promoted),
        static_cast<unsigned long long>(result.bytes_promoted),
        static_cast<unsigned long long>(result.bytes_reclaimed),
        static_cast<unsigned long long>(result.dirty_cards_scanned),
        pause_ns / 1000,
        pt.phase1_ns / 1000,
        pt.phase2_ns / 1000,
        pt.phase2b_ns / 1000,
        pt.phase3_ns / 1000,
        pt.phase3b_ns / 1000,
        pt.phase4_ns / 1000);

    GcFireEvent(GcEvent::GC_YOUNG_DONE);
    return result;
}

}  // namespace chaos::il2cpp::runtime_core