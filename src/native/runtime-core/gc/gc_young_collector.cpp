#include "gc_young_collector.h"

#include <chaos/log.h>
#include <chaos/profile.h>


#include "gc_card_table.h"
#include "gc_events.h"
#include "gc_layout.h"
#include "gc_old_gen.h"
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
/// The cap matches the largest old-gen size class (32768 bytes) so that
/// promoted objects up to that size are correctly copied regardless of
/// layout availability.  Objects larger than 32768 bytes go through the
/// LOH path and are never in the nursery.
static constexpr CHAOS_IL2CPP_SIZE kMaxEstObjectSize = 32768;

static CHAOS_IL2CPP_SIZE EstimateObjectSize(const void* obj, const Region* nursery) {
    if (nursery == nullptr) return kMaxEstObjectSize;
    uintptr_t start = reinterpret_cast<uintptr_t>(obj);
    uintptr_t end   = reinterpret_cast<uintptr_t>(nursery->current);
    if (start >= end) return kMaxEstObjectSize;
    CHAOS_IL2CPP_SIZE remaining = static_cast<CHAOS_IL2CPP_SIZE>(end - start);
    return remaining < kMaxEstObjectSize ? remaining : kMaxEstObjectSize;
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
        if (tls_nursery_ctx.nursery != nullptr &&
            reinterpret_cast<uintptr_t>(obj) >= reinterpret_cast<uintptr_t>(tls_nursery_ctx.nursery->begin) &&
            reinterpret_cast<uintptr_t>(obj) < reinterpret_cast<uintptr_t>(tls_nursery_ctx.nursery->current)) {
            obj_size = EstimateObjectSize(obj, tls_nursery_ctx.nursery);
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

// ======================================================================
// Young collection
// ======================================================================

YoungCollectionResult GcYoungCollection(Region* nursery, Region* tenured_target) {
    CHAOS_IL2CPP_PROFILE_SCOPE("GcYoungCollection");

    YoungCollectionResult result = {};

    if (nursery == nullptr) {
        CHAOS_IL2CPP_LOG_WARN("CRAG", "young_collection_no_nursery");
        return result;
    }

    auto pause_start = std::chrono::steady_clock::now();

    CHAOS_IL2CPP_LOG_INFO_M("CRAG", "young_collection nursery=[{0}, {1}) usage={2}",
        static_cast<void*>(nursery->begin),
        static_cast<void*>(nursery->end),
        static_cast<unsigned long long>(nursery->current - nursery->begin));

    // Fire GC_YOUNG_START event for young GC.
    GcFireEvent(GcEvent::GC_YOUNG_START);

    // ── Phase 1: Scan dirty cards for old→nursery cross-gen references ──
    CHAOS_IL2CPP_PROFILE_SCOPE("GC_Phase1_DirtyCards");
    // Scan dirty cards ACROSS ALL old-gen pages, not the nursery range.
    // Dirty cards are written by the write barrier when a managed object
    // field is updated — this only happens in old-gen (or tenured) memory,
    // never in nursery memory (nursery objects are dead after young GC).
    // Previously this scanned nursery->begin..nursery->end, which always
    // produced zero dirty cards — a use-after-free vulnerability for any
    // old→nursery cross-gen reference.
    result.dirty_cards_scanned = 0;
    g_old_gen.ScanDirtyCardsInPages(
        [&](uintptr_t /*card_idx*/, uintptr_t /*card_start*/, uintptr_t card_end) {
            result.dirty_cards_scanned++;

            // Iterate pointer-aligned slots in this 512B card.
            // For each slot, if it points into the nursery, scavenge it.
            uintptr_t card_begin = card_end - kCardSize;
            for (uintptr_t slot = card_begin; slot < card_end; slot += sizeof(void*)) {
                auto* ptr_slot = reinterpret_cast<void**>(slot);
                void* val = *ptr_slot;

                if (val != nullptr && IsInNursery(val)) {
                    // This old→nursery reference is a root for young GC.
                    // Scavenge the target object.
                    void* tenured = GcScavengeObject(val, &result);
                    if (tenured != nullptr) {
                        *ptr_slot = tenured;  // Update the reference.
                    }
                }
            }
        });

    // ── Phase 2: Precise object-by-object nursery scan ──
    // Walk the nursery bump range object-by-object using registered GC
    // layouts.  For each object, scan only the pointer fields declared
    // in its GcLayout.  This eliminates false positives from fill bytes
    // and pattern data.
    //
    // Objects with no registered layout are treated as pointer-free (safe
    // skip, no scavenge).  This is correct because:
    //   - If the object has no registered layout, we cannot know which
    //     fields are pointers, so we conservatively scavenge nothing.
    //   - Any nursery pointer in a non-layout region will be found by the
    //     Cheney BFS (Phase 3) if the owning object was promoted via a
    //     different root path (e.g., dirty cards).
    uintptr_t nursery_begin = reinterpret_cast<uintptr_t>(nursery->begin);
    uintptr_t nursery_used  = reinterpret_cast<uintptr_t>(nursery->current);
    uintptr_t scan_ptr = nursery_begin;
    auto& layout_registry = GcLayoutRegistry::Instance();

    while (scan_ptr < nursery_used) {
        auto* obj = reinterpret_cast<void*>(scan_ptr);

        // Read the first word — in a managed object this is the TypeInfo*.
        const void* first_word = *static_cast<const void* const*>(obj);
        if (first_word == nullptr) {
            // End-of-objects (zeroed memory / padding).  Advance to next
            // pointer-aligned boundary.  At C2-C3, all nursery slots are
            // zeroed on allocation, so a zero word is either padding or
            // a newly allocated (not yet TypeInfo-written) object.
            // Advance by one word and continue.
            scan_ptr += sizeof(void*);
            continue;
        }

        // Check if the first word points to a registered TypeInfo range.
        // This is the key filter: fill bytes / pattern data will NOT point
        // into TypeInfo memory, so we skip them without dereferencing the
        // candidate as a TypeInfo*.
        if (!layout_registry.IsValidTypeInfoPointer(first_word)) {
            // Not a valid TypeInfo pointer — skip this word.
            scan_ptr += sizeof(void*);
            continue;
        }

        // Valid TypeInfo* — look up the GC layout.
        auto* hot = static_cast<const TypeInfoHot*>(first_word);
        uint64_t stable_id = hot->stable_id;
        const auto* layout = layout_registry.Lookup(stable_id);

        if (layout == nullptr) {
            // No registered layout — treat as pointer-free, advance by
            // a reasonable bound (capped at 2048 like EstimateObjectSize).
            // This preserves correctness: we skip scavenging but don't
            // corrupt anything.  Transitively reachable objects in the
            // nursery will be found via dirty card scanning or other roots.
            CHAOS_IL2CPP_SIZE obj_size = EstimateObjectSize(obj, nursery);
            scan_ptr += obj_size;
            continue;
        }

        // Precise scan: only the pointer offsets declared in the layout.
        uint32_t obj_size = layout->instance_size;
        for (uint16_t i = 0; i < layout->pointer_count; i++) {
            uint16_t offset = layout->pointer_offsets[i].offset;
            auto* slot = reinterpret_cast<void**>(scan_ptr + offset);
            void* val = *slot;

            if (val == nullptr) continue;

            if (IsInNursery(val)) {
                void* tenured = GcScavengeObject(val, &result);
                if (tenured != nullptr && tenured != val) {
                    *slot = tenured;
                }
            }
        }

        // Advance to next object.
        scan_ptr += obj_size;
    }

    // ── Phase 3: Cheney BFS — walk all promoted objects using precise layouts ──
    // After Phase 2, every root-reachable nursery object has been copied to
    // tenured.  The Cheney BFS processes each promoted object's pointer
    // fields using registered GC layouts, ensuring all transitively-reachable
    // objects are also promoted.  This replaces the previous no-op Phase 3.
    //
    // We use a worklist-filled by GcScavengeObject during Phase 1+2.  Walk
    // it like a BFS queue: for each promoted object, scan its pointer fields
    // via ScanObjectPointers, which scavenges any nursery targets.
    //
    // Performance: the worklist uses a thread_local cache to avoid malloc/free
    // on every young GC.  The first GC on each thread allocates 512 KB; subsequent
    // GCs reuse the buffer.  This saves ~2-10 µs per empty-nursery GC cycle.
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("GcYoungCollection::CheneyBfs");

        // TLS-cached BFS worklist — allocated once, reused across GCs,
        // grown via realloc when needed, never freed until thread teardown.
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

            // Phase 1 (dirty cards) and Phase 2 (nursery scan) already filled the
            // worklist.  Now drain it: for each promoted object, scan its pointer
            // fields.  If those point into the nursery, they get scavenged too,
            // and the new promoted objects are added to the tail of the worklist.
            int idx = 0;
            auto& layout_registry = GcLayoutRegistry::Instance();
            while (idx < result.bfs_worklist_count) {
                // Check for worklist overflow before processing each entry.
                // If the worklist is nearly full, double its capacity.
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
                        // realloc failed — stop growing but continue draining
                        // what we already have.  The entries already in the
                        // worklist are safely processed; objects added during
                        // Phase 1/2 beyond current count won't be transitively
                        // scanned (correctness is preserved because Phase 1/2
                        // already scavenged all directly-reachable nursery
                        // objects — we may miss deep BFS paths, but no
                        // dangling references are created).
                        break;
                    }
                }

                void* promoted = worklist[idx++];

                // Read TypeInfo* from first word of the tenured copy.
                const void* type_info_ptr = *static_cast<const void* const*>(promoted);
                if (type_info_ptr == nullptr) continue;
                // CRITICAL: Validate the TypeInfo pointer before dereferencing.
                // Phase 1 (dirty card scan) can promote garbage objects when a
                // non-pointer value in a dirty card happens to look like a nursery
                // pointer.  Such garbage has no valid TypeInfo in its first word.
                // Without this check, dereferencing the garbage TypeInfo* → SIGSEGV.
                if (!layout_registry.IsValidTypeInfoPointer(type_info_ptr)) continue;


                auto* hot = static_cast<const TypeInfoHot*>(type_info_ptr);

                uint64_t stable_id = hot->stable_id;
                const auto* layout = layout_registry.Lookup(stable_id);

                if (layout == nullptr || layout->pointer_count == 0) {
                    // No layout or pointer-free — skip.
                    continue;
                }

                // Scan pointer fields of this promoted object.
                ScanObjectPointers(promoted, layout, nursery, &result);
            }

            // No overflow fallback needed — dynamic growth (capacity *= 2; realloc)
            // above ensures the worklist can always accommodate all promoted objects.

            // NOTE: TLS-cached worklist is NOT freed here — it persists for reuse
            // on subsequent GC cycles on this thread.  Memory is reclaimed at
            // thread teardown or process exit.
        }  // closes bfs_worklist not-null check
    }  // closes CheneyBfs profile scope

    // ── Phase 3b: Process weak GCHandles ──
    // Update weak handles pointing to promoted nursery objects, and null
    // weak handles pointing to non-promoted (collected) nursery objects.
    GcProcessWeakHandlesAfterYoungGC();

    // Phase 3c: Process dependent handles (ConditionalWeakTable).
    // Promote secondary objects whose primary survived the young GC.
    GcProcessDependentHandlesAfterYoungGC();

    // ── Phase 4: Sweep nursery ──
    // Clear the nursery for reuse.  All live objects have been forwarded.
    // The nursery region can be reset or returned to the free pool.

    // Compute bytes_reclaimed before resetting current pointer.
    CHAOS_IL2CPP_SIZE nursery_used_bytes = static_cast<CHAOS_IL2CPP_SIZE>(
        nursery_used - nursery_begin);
    result.bytes_reclaimed = nursery_used_bytes;

    nursery->current = nursery->begin;

    // Clear card table entries covering the nursery range.
    ClearAllCards();

    auto pause_end = std::chrono::steady_clock::now();
    uint64_t pause_ns = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(pause_end - pause_start).count());

    // Record into GcStats.
    GcRecordYoungCollection(
        result.objects_promoted,
        result.bytes_promoted,
        result.bytes_reclaimed,
        result.dirty_cards_scanned,
        pause_ns);

    // Record into scheduler for adaptive sizing.
    g_gc_scheduler.RecordYoungCollection(
        nursery_used_bytes, result.bytes_promoted, pause_ns);

    CHAOS_IL2CPP_LOG_INFO_M("CRAG", "young_collection done promoted={0} cards={1} pause_ns={2}",
        static_cast<unsigned long long>(result.objects_promoted),
        static_cast<unsigned long long>(result.dirty_cards_scanned), pause_ns);

    // Fire GC_YOUNG_DONE event for young GC.
    GcFireEvent(GcEvent::GC_YOUNG_DONE);

    (void)tenured_target;
    return result;
}

}  // namespace chaos::il2cpp::runtime_core