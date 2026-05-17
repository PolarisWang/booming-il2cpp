#include "gc_young_collector.h"

#include <chaos/log.h>
#include <chaos/profile.h>


#include "gc_card_table.h"
#include "gc_events.h"
#include "gc_layout.h"
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
    auto* hot = static_cast<const TypeInfoHot*>(type_info_ptr);
    uint64_t stable_id = hot->stable_id;
    const auto* layout = layout_registry.Lookup(stable_id);
    if (layout == nullptr || layout->instance_size == 0) return 0;
    return layout->instance_size;
}

// ======================================================================
// Cheney copy
// ======================================================================

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

    void* tenured = g_old_gen.Allocate(obj_size, true);
    if (tenured == nullptr) return nullptr;

    std::memcpy(tenured, obj, obj_size);
    SetForwardingAddress(obj, tenured);

    if (result) {
        result->objects_promoted++;
        result->bytes_promoted += obj_size;
        if (result->bfs_worklist && result->bfs_worklist_count < result->bfs_worklist_capacity) {
            result->bfs_worklist[result->bfs_worklist_count++] = tenured;
        }
    }

    CHAOS_IL2CPP_LOG_DEBUG("CRAG", "scavenge_object_known_nursery");
    return tenured;
}

// ======================================================================
// Young collection
// ======================================================================

YoungCollectionResult GcYoungCollection() {
    CHAOS_IL2CPP_PROFILE_SCOPE("GcYoungCollection");

    YoungCollectionResult result = {};

    Region* nursery = g_young_gen.region.load(std::memory_order_acquire);
    if (nursery == nullptr) {
        CHAOS_IL2CPP_LOG_WARN("CRAG", "young_collection_no_region");
        return result;
    }

    auto pause_start = std::chrono::steady_clock::now();

    uintptr_t nursery_begin = reinterpret_cast<uintptr_t>(nursery->begin);
    uintptr_t nursery_used  = reinterpret_cast<uintptr_t>(nursery->current);

    CHAOS_IL2CPP_LOG_INFO_M("CRAG", "young_collection nursery=[{0}, {1}) usage={2}",
        static_cast<void*>(nursery->begin),
        static_cast<void*>(nursery->end),
        static_cast<unsigned long long>(nursery->current - nursery->begin));

    GcFireEvent(GcEvent::GC_YOUNG_START);

    // ── Phase 1: Scan dirty cards for old→young cross-gen references ──
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("GC_Phase1_DirtyCards");
        result.dirty_cards_scanned = 0;
        g_old_gen.ScanDirtyCardsInPages(
            [&](uintptr_t /*card_idx*/, uintptr_t /*card_start*/, uintptr_t card_end) {
                result.dirty_cards_scanned++;
                uintptr_t card_begin = card_end - kCardSize;
                for (uintptr_t slot = card_begin; slot < card_end; slot += sizeof(void*)) {
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

    // ── Phase 2: Precise object-by-object nursery scan ──
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("GC_Phase2_NurseryScan");
        uintptr_t scan_ptr = nursery_begin;
        auto& layout_registry = GcLayoutRegistry::Instance();

        while (scan_ptr < nursery_used) {
            auto* obj = reinterpret_cast<void*>(scan_ptr);
            const void* first_word = *static_cast<const void* const*>(obj);
            if (first_word == nullptr) {
                scan_ptr += sizeof(void*);
                continue;
            }
            if (!layout_registry.IsValidTypeInfoPointer(first_word)) {
                scan_ptr += sizeof(void*);
                continue;
            }
            auto* hot = static_cast<const TypeInfoHot*>(first_word);
            uint64_t stable_id = hot->stable_id;
            const auto* layout = layout_registry.Lookup(stable_id);

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

                auto* hot = static_cast<const TypeInfoHot*>(type_info_ptr);
                uint64_t stable_id = hot->stable_id;
                const auto* layout = layout_registry.Lookup(stable_id);

                if (layout == nullptr || layout->pointer_count == 0) {
                    continue;
                }

                ScanObjectPointers(promoted, layout, &result);
            }
        }
    }

    // ── Phase 3b: Process weak GCHandles ──
    GcProcessWeakHandlesAfterYoungGC();

    // Phase 3c: Process dependent handles (ConditionalWeakTable).
    GcProcessDependentHandlesAfterYoungGC();

    // ── Phase 4: Sweep young generation ──
    // Reset g_young_gen.bump to the beginning of the region.
    // Clear all threads' TLAB ranges so they re-claim on next allocation.
    CHAOS_IL2CPP_SIZE nursery_used_bytes = static_cast<CHAOS_IL2CPP_SIZE>(
        nursery_used - nursery_begin);
    result.bytes_reclaimed = nursery_used_bytes;

    // Reset the shared young region bump pointer.
    nursery->current = nursery->begin;
    g_young_gen.bump.store(nursery->begin, std::memory_order_release);
    g_young_gen.region_end.store(nursery->end, std::memory_order_release);

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

    GcRecordYoungCollection(
        result.objects_promoted,
        result.bytes_promoted,
        result.bytes_reclaimed,
        result.dirty_cards_scanned,
        pause_ns);

    g_gc_scheduler.RecordYoungCollection(
        nursery_used_bytes, result.bytes_promoted, pause_ns);

    CHAOS_IL2CPP_LOG_INFO_M("CRAG", "young_collection done promoted={0} cards={1} pause_ns={2}",
        static_cast<unsigned long long>(result.objects_promoted),
        static_cast<unsigned long long>(result.dirty_cards_scanned), pause_ns);

    GcFireEvent(GcEvent::GC_YOUNG_DONE);
    return result;
}

}  // namespace chaos::il2cpp::runtime_core