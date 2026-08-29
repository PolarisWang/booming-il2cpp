#include "gc_young_collector.h"

#include <chaos/asan_interface.h>
#include <chaos/log.h>
#include <chaos/profile.h>


#include "gc_card_table.h"
#include "gc_events.h"
#include "gc_etw.h"
#include "gc_bgc.h"
#include "gc_gen1.h"
#include "gc_layout.h"
#include "gc_loh.h"
#include "gc_old_gen.h"
#include "gc_young_gen.h"
#include "gc_scheduler.h"
#include "gc_stats.h"
#include "gc_heap.h"
#include "gc_diagnostics.h"
#include <thread_state.h>

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace chaos::il2cpp::runtime_core {

// Thread-local BFS worklist cache for young collection, shared across the
// setup block (before Phase 1) and the Phase 3 BFS loop.  Allocated on first
// use at 64K entries; grown via realloc when capacity is exceeded.
namespace {
thread_local void* tls_bfs_worklist = nullptr;
thread_local int tls_bfs_capacity = 0;
}

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
///
/// NOTE (2026-08-29): this cap must stay SMALL (2048).  Raising it (e.g. to a
/// 64 KB page) made GcScavengeObject copy [obj, obj+obj_size) into old-gen,
/// which for an object near the nursery edge reads past nursery->committed and
/// crashes.  When Phase-2 encounters a TypeInfo-valid but layout-unregistered
/// object it now jumps straight to nursery_used (see gc_young_collector.cpp
/// Phase-2), so this function is only consulted for promotion sizing — and for
/// that path a 2048-byte allocation is the safe, bounded choice (never copies
/// beyond the nursery).  Objects without a registered layout do not exist in
/// production (all managed types carry a GcTypeLayout via RegisterTypeInfo).
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
    // Variable-size array: total = header_size + element_size * length
    if (layout->element_size > 0) {
        CHAOS_IL2CPP_INTPTR length = 0;
        std::memcpy(&length, static_cast<const uint8_t*>(obj) + layout->length_offset, sizeof(length));
        return layout->instance_size + layout->element_size * static_cast<CHAOS_IL2CPP_SIZE>(length);
    }
    return layout->instance_size;
}

// ======================================================================
// Cheney copy
// ======================================================================

/// Try to allocate in the Gen1 survivor area.
/// Returns nullptr if the Gen1 area is exhausted — caller should fall
/// back to old gen.  Thread-safe via CAS on gen1_bump.
static void* TryAllocateInSurvivor(CHAOS_IL2CPP_SIZE size) noexcept {
    char* current = G_YoungGen().gen1_bump.load(std::memory_order_acquire);
    char* next;
    do {
        if (current == nullptr) return nullptr;
        next = current + size;
        if (next > G_YoungGen().gen1_end) return nullptr;
    } while (!G_YoungGen().gen1_bump.compare_exchange_weak(
        current, next, std::memory_order_release, std::memory_order_acquire));
    std::memset(current, 0, size);
    return current;
}

/// Determine whether an address falls within the Gen1 survivor region.
/// File-local helper (the non-static IsInGen1 in gc_gen1.cpp does the same
/// thing for external callers).
static bool IsInGen1Local(const void* ptr) noexcept {
    auto* gen1 = G_YoungGen().gen1_region.load(std::memory_order_acquire);
    if (gen1 == nullptr) return false;
    auto* bump = G_YoungGen().gen1_bump.load(std::memory_order_acquire);
    if (bump == nullptr) return false;
    auto* cptr = static_cast<const char*>(ptr);
    return cptr >= gen1->begin && cptr < bump;
}

void* GcScavengeObject(void* obj, YoungCollectionResult* result) {
    if (obj == nullptr) return nullptr;
    if (!IsInNursery(obj)) {
        // Already in tenured — no copy needed.
        // (The IsInNursery gate IS the condemned filter for a young collection:
        //  only nursery refs are promoted.  condemned_gen_num is tracked for
        //  deeper Gen1/old collections but the young path uses the precise range.)
        return obj;
    }
    if (IsForwarded(obj)) {
        // Already copied — return forwarding address.
        return GetForwardingAddress(obj);
    }

    // If the promotion limit was reached, stop promoting.  The object
    // remains in the nursery and is treated as unreachable — its memory
    // will be reclaimed when the nursery is reset.
    if (result && result->timed_out) [[unlikely]] {
        return nullptr;
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
        Region* nursery_local = G_YoungGen().region.load(std::memory_order_acquire);
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

    void* tenured = G_OldGen().Allocate(obj_size, true);
    if (tenured == nullptr) return nullptr;

    // Copy the object contents.
    std::memcpy(tenured, obj, obj_size);

    // Place forwarding pointer at the nursery location.
    SetForwardingAddress(obj, tenured);

    // Re-register finalizer at the promoted address.
    // The TypeInfo* was copied by memcpy into tenured — read from there.
    // Validate the TypeInfo pointer before dereferencing — objects sized via
    // the EstimateObjectSize fallback (e.g., stress tests without valid layout)
    // have garbage in the first word and must not be treated as TypeInfo.
    if (auto* ti_ptr = *static_cast<const TypeInfoHot* const*>(tenured)) {
        auto& reg = GcLayoutRegistry::Instance();
        if (reg.IsValidTypeInfoPointer(ti_ptr)) [[likely]] {
            if (ti_ptr->flags & kTypeInfoHasFinalizer) [[unlikely]] {
                if (auto* cb = reg.LookupFinalizer(ti_ptr->stable_id)) {
                    G_OldGen().RegisterFinalizer(tenured, cb);
                }
            }
        }
    }

    // Count promotion.
    if (result) {
        result->objects_promoted++;
        result->bytes_promoted += obj_size;

        // Check kMaxPromoteObjects guard — stop promoting once the limit
        // is reached.  Beyond this limit, remaining nursery objects are
        // treated as unreachable (not promoted).
        if (result->objects_promoted >= kMaxPromoteObjects) [[unlikely]] {
            result->timed_out = true;
        }

        // Add to Cheney BFS worklist for transitive closure (Phase 3).
        if (!result->timed_out && result->bfs_worklist) {
            if (result->bfs_worklist_count >= result->bfs_worklist_capacity) {
                int new_cap = result->bfs_worklist_capacity * 2;
                auto* new_wl = static_cast<void**>(std::realloc(
                    result->bfs_worklist, static_cast<size_t>(new_cap) * sizeof(void*)));
                if (new_wl != nullptr) {
                    result->bfs_worklist = new_wl;
                    result->bfs_worklist_capacity = new_cap;
                    tls_bfs_worklist = new_wl;
                    tls_bfs_capacity = new_cap;
                }
            }
            if (result->bfs_worklist_count < result->bfs_worklist_capacity) {
                result->bfs_worklist[result->bfs_worklist_count++] = tenured;
            }
        }
    }

    CHAOS_IL2CPP_LOG_DEBUG("CRAG", "scavenge_object");
    return tenured;
}

void* GcScavengeObjectKnownNursery(void* obj, YoungCollectionResult* result) {
    if (obj == nullptr) return nullptr;
    // Caller already verified obj is in nursery — skip IsInNursery check.
    // M9-A2 / M10: activate the condemned-generation filter.  A GC condemns every
    // gen <= condemned_gen_num; objects with region_gen NEWER than the condemned
    // gen are not promoted/marked here (they belong to an older-gen collection).
    // For a young GC condemned=gen1(1) → nursery(0)/gen1(1) are processed, gen2+
    // skipped.  For a gen0-only collection condemned=young(0) → nursery processes,
    // a gen1 object (region-gen 1 > 0) is skipped.  Note: no `condemned>0` guard —
    // kRegionGenYoung(0) is a legitimate condemned value.
    if (result != nullptr) {
        if (GetRegionGen(reinterpret_cast<uintptr_t>(obj)) > result->condemned_gen_num) {
            return obj;  // newer than condemned — leave for the appropriate collection
        }
    }
    if (IsForwarded(obj)) {
        return GetForwardingAddress(obj);
    }

    // If the promotion limit was reached, stop promoting.
    if (result && result->timed_out) [[unlikely]] {
        return nullptr;
    }

    CHAOS_IL2CPP_SIZE obj_size = PreciseObjectSize(obj);
    if (obj_size == 0) {
        Region* nursery_local = G_YoungGen().region.load(std::memory_order_acquire);
        if (nursery_local != nullptr &&
            reinterpret_cast<uintptr_t>(obj) >= reinterpret_cast<uintptr_t>(nursery_local->begin) &&
            reinterpret_cast<uintptr_t>(obj) < reinterpret_cast<uintptr_t>(nursery_local->current)) {
            obj_size = EstimateObjectSize(obj, nursery_local);
        } else {
            obj_size = kMaxEstObjectSize;
        }
    }

    // ── Age tenuring: determine destination based on source generation ──
    // M9-A2: consult the region-generation tag (not just location) so the gen1
    // value drives the destination.  A gen0 (nursery, region_gen=0) object always
    // promotes to gen1 (first survival).  A gen1 (region_gen=1) object has
    // survived: with the dynamic threshold > 1 it stays in gen1, else it promotes
    // to gen2 (old).  gen2 (old) objects never reach this scavenge.
    const uint8_t src_gen = GetRegionGen(reinterpret_cast<uintptr_t>(obj));
    void* target;
    if (src_gen == kRegionGenGen1 || IsInGen1(obj)) {  // gen1 (survivor)
        int threshold = G_YoungGen().promotion_age_threshold_.load(
            std::memory_order_acquire);
        if (threshold > 1) {
            target = TryAllocateInGen1(obj_size);
            if (target == nullptr) {
                target = G_OldGen().Allocate(obj_size, true);
            }
        } else {
            target = G_OldGen().Allocate(obj_size, true);
        }
    } else {                                          // gen0 (nursery) → gen1
        target = TryAllocateInGen1(obj_size);
        if (target == nullptr) {
            target = G_OldGen().Allocate(obj_size, true);
        }
    }

    if (target == nullptr) return nullptr;

    std::memcpy(target, obj, obj_size);
    SetForwardingAddress(obj, target);

    // Re-register finalizer at the promoted address.
    auto& known_layout = GcLayoutRegistry::Instance();
    if (auto* ti_ptr = *static_cast<const TypeInfoHot* const*>(target)) {
        if (known_layout.IsValidTypeInfoPointer(ti_ptr)) [[likely]] {
            if (ti_ptr->flags & kTypeInfoHasFinalizer) [[unlikely]] {
                if (auto* cb = known_layout.LookupFinalizer(ti_ptr->stable_id)) {
                    G_OldGen().RegisterFinalizer(target, cb);
                }
            }
        }
    }

    if (result) {
        result->objects_promoted++;
        result->bytes_promoted += obj_size;

        // Check kMaxPromoteObjects guard.
        if (result->objects_promoted >= kMaxPromoteObjects) [[unlikely]] {
            result->timed_out = true;
        }

        if (!result->timed_out && result->bfs_worklist) {
            if (result->bfs_worklist_count >= result->bfs_worklist_capacity) {
                int new_cap = result->bfs_worklist_capacity * 2;
                auto* new_wl = static_cast<void**>(std::realloc(
                    result->bfs_worklist, static_cast<size_t>(new_cap) * sizeof(void*)));
                if (new_wl != nullptr) {
                    result->bfs_worklist = new_wl;
                    result->bfs_worklist_capacity = new_cap;
                    tls_bfs_worklist = new_wl;
                    tls_bfs_capacity = new_cap;
                }
            }
            if (result->bfs_worklist_count < result->bfs_worklist_capacity) {
                result->bfs_worklist[result->bfs_worklist_count++] = target;
            }
        }
    }

    CHAOS_IL2CPP_LOG_DEBUG("CRAG", "scavenge_object_known_nursery");
    return target;
}

// ======================================================================
// Young collection
// ======================================================================

YoungCollectionResult GcYoungCollection(bool force_skip_gen1) {
    // If BGC is in concurrent mark, pause BGC before running young GC
    // to avoid races between forwarding pointer writes and BGC's concurrent
    // object scan.  BGC resumes after young GC completes.
    bool bgc_was_paused = false;
    if (g_bgc_is_marking.load(std::memory_order_acquire)) [[unlikely]] {
        CHAOS_IL2CPP_LOG_DEBUG("CRAG", "young_collection_pausing_bgc");
        BgcController::Instance().PauseForYoungGc();
        bgc_was_paused = true;
    }

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

    Region* nursery = G_YoungGen().region.load(std::memory_order_acquire);
    if (nursery == nullptr) {
        CHAOS_IL2CPP_LOG_WARN("CRAG", "young_collection_no_region");
        return result;
    }

    auto pause_start = std::chrono::steady_clock::now();

    uintptr_t nursery_begin = reinterpret_cast<uintptr_t>(nursery->begin);
    // With shared young gen + TLABs, TlabClaimFromYoungGen() updates
    // G_YoungGen().bump, NOT nursery->current.  Read the actual allocation
    // frontier from the shared bump pointer so the nursery scan covers
    // all objects in all TLABs.
    uintptr_t nursery_used  = reinterpret_cast<uintptr_t>(
        G_YoungGen().bump.load(std::memory_order_acquire));

    CHAOS_IL2CPP_LOG_INFO_M("CRAG", "young_collection nursery=[{0}, {1}) usage={2}",
        static_cast<void*>(nursery->begin),
        static_cast<void*>(nursery->end),
        static_cast<unsigned long long>(nursery_used - nursery_begin));

    GcEtwFireGcYoungStart(static_cast<uint64_t>(nursery_used - nursery_begin));
    GcFireEvent(GcEvent::GC_YOUNG_START);

    // ── BFS worklist setup: initialize before any scavenge phase so that
    // GcScavengeObjectKnownNursery can enqueue promoted objects for
    // transitive closure in Phase 3.  Previously the worklist was set up
    // at Phase 3 entry, which meant Phases 1 and 2 could never populate it.
    {
        int worklist_cap = 256 * 1024;
        if (tls_bfs_worklist == nullptr) {
            tls_bfs_worklist = static_cast<void*>(std::malloc(
                static_cast<size_t>(worklist_cap) * sizeof(void*)));
            tls_bfs_capacity = (tls_bfs_worklist != nullptr) ? worklist_cap : 0;
        }
        if (tls_bfs_worklist != nullptr) {
            result.bfs_worklist = static_cast<void**>(tls_bfs_worklist);
            result.bfs_worklist_capacity = tls_bfs_capacity;
            result.bfs_worklist_count = 0;
        }
    }

    // ── Phase 0: All-thread conservative + precise stack root scan ──
    // Scan the stacks of ALL suspended threads for nursery pointers and
    // promote them, fixing up stack-local references after promotion.
    //
    // GcYoungCollection is always invoked under a global STW safepoint
    // (all callers in gc_region.cpp:201/217/362 + gc_api.cpp + gc_coordinator
    // acquire RequestGlobalSafepoint first), so every managed thread's stack
    // is frozen and consistent here.  Previous versions scanned only the
    // current thread's stack — other suspended threads' stacks holding
    // nursery references were invisible, so their objects could be collected
    // while still referenced -> use-after-free.  We now scan all threads via
    // GcScanAllThreadRoots (which both conservatively scans every stack AND
    // precisely scans registered T4 frames via GcSlotMap), promoting any
    // nursery pointer found and writing the tenured target back into the slot.
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("GC_Phase0_AllThreadStackRoots");
        struct RootScavengeCtx { YoungCollectionResult* result; } scavenge_ctx{ &result };
        threading::GcScanAllThreadRoots(
            [](void* root_addr, bool /*is_interior*/, void* user_data) {
                auto* slot = static_cast<void**>(root_addr);
                // root_addr points into ANOTHER thread's stack (conservative
                // all-thread scan).  The slot may fall in an ASan stack frame
                // redzone between the target thread's frames, which ASan poisons.
                // We only need to shed instrumentation for genuinely poisoned slots
                // (Probe gates on __asan_address_is_poisoned); live stack slots stay
                // instrumented so a real OOB/UAF write into a root is still caught
                // (review #2/#4) instead of masking all findings.  The promoted
                // pointer write-back below is similarly probe-gated.
                void* val = chaos::il2cpp::common::AsanReadPtrNoCheck(slot);
                if (val != nullptr && IsInNursery(val)) {
                    auto* r = static_cast<RootScavengeCtx*>(user_data)->result;
                    void* tenured = GcScavengeObjectKnownNursery(val, r);
                    if (tenured != nullptr && tenured != val) {
                        // NOTE (review #9): probe write into the foreign stack is
                        // only sound because GcYoungCollection runs under a global
                        // STW safepoint (all mutators suspended).
                        chaos::il2cpp::common::AsanWritePtrNoCheck(slot, tenured);
                    }
                }
            },
            &scavenge_ctx);
    }

    // ── Phase 1: Scan dirty cards for old→young cross-gen references ──
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("GC_Phase1_DirtyCards");
        result.dirty_cards_scanned = 0;
        // CoreCLR-aligned scan source: iterate REGISTERED L2 card segments (the
        // write barrier only writes into registered segments), so every card the
        // barrier recorded is reachable.  The prior allocator-page + LOH-segment
        // driven scans could miss a barrier-written card (page's L2 segment not
        // registered, or page range not covering the card index) → old→nursery
        // edge dropped → young object collected → dangling.
        auto phase1_scan_cb = [](uintptr_t range_start, uintptr_t range_end, void* ud) {
            auto* scav = static_cast<YoungCollectionResult*>(ud);
            for (uintptr_t slot = range_start; slot < range_end; slot += sizeof(void*)) {
                auto* ptr_slot = reinterpret_cast<void**>(slot);
                void* val = *ptr_slot;
                if (val != nullptr && IsInNursery(val)) {
                    void* tenured = GcScavengeObjectKnownNursery(val, scav);
                    if (tenured != nullptr) {
                        *ptr_slot = tenured;
                    }
                }
            }
        };
        ScanDirtyCardsInRegisteredSegments(&result.dirty_cards_scanned,
                                            phase1_scan_cb, &result);
    }
    pt.phase1_ns = static_cast<uint64_t>(std::chrono::duration_cast<
        std::chrono::nanoseconds>(std::chrono::steady_clock::now() - pause_start).count());

    // ── Phase 2: Precise object-by-object nursery scan ──
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("GC_Phase2_NurseryScan");
        // GC-N7 (YoungGcPauseUnderLoad-class) Phase-2 safety: the precise scan
        // derefs every slot in [scan_ptr, nursery_used) as a potential TypeInfo.
        // Under multi-cycle timed_out young GCs + Gen1 relocation/resize churn the
        // shared G_YoungGen().bump (or a recycled Region's begin) can desync from the
        // live frontier, so nursery_used can exceed the committed nursery range and
        // scan_ptr walks into unmapped memory → SEH.  Confine the walk ALWAYS to the
        // nursery Region's committed [begin, end) and advance only within it; a slot
        // beyond the true live frontier simply holds no valid typed header (safe skip),
        // never a wild deref.  This hardens against the corrupt-state crash; the deep
        // bump/region desync root is tracked separately (真机上 page-heap 定位).
        uintptr_t region_begin = reinterpret_cast<uintptr_t>(nursery->begin);
        uintptr_t region_end   = reinterpret_cast<uintptr_t>(nursery->end);
        if (nursery_used > region_end) nursery_used = region_end;
        if (nursery_begin < region_begin) nursery_begin = region_begin;
        if (nursery_begin > region_end) nursery_begin = region_end;

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
            // Phase-2 always advances within the committed nursery; if a corrupt
            // layout / size ever pushes scan_ptr past region_end (or below begin),
            // stop walking instead of dereferencing unmapped memory.
            if (scan_ptr < region_begin || scan_ptr >= region_end) break;
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

            if (layout == nullptr || layout->instance_size == 0) {
                // GC-N6 发现3 root fix: a registered layout with instance_size==0
                // (or a null lookup) must not advance scan_ptr by 0, which would
                // spin this Phase-2 walk forever — the typed young-GC hang.
                // Mirror PreciseObjectSize's guard: fall back to bounds estimation
                // so scan_ptr always advances.
                //
                // FIX-B alignment: when the _first_word_ IS a valid TypeInfo
                // pointer (the L621 gate passed) but the layout is unregistered,
                // this is a real typed object whose extent Phase-2 cannot
                // precisely determine.  Rather than advancing by EstimateObjectSize
                // (capped at 2048 bytes, which would phantom-re-walk into the
                // object interior for objects > 4 KB), we still need to advance
                // past this object before continuing the scan.  Use the slack
                // between scan_ptr and the end of the nursery as a conservative
                // upper bound, but advance by at least kMaxTlabAlloc (a reasonable
                // single-object cap) to avoid an unbounded single-step that
                // skips the entire remaining nursery for one unregistered object.
                //
                // NOTE: first_word is guaranteed valid here because the L621
                // IsValidTypeInfoPointer gate passed.
                {
                    uintptr_t slack = nursery_used - scan_ptr;
                    uintptr_t step = slack < kMaxTlabAlloc ? slack : kMaxTlabAlloc;
                    scan_ptr += step;
                }
                continue;
            }

            uint32_t obj_size = layout->instance_size;
            for (uint16_t i = 0; i < layout->pointer_count; i++) {
                uint16_t offset = layout->pointer_offsets[i].offset;
                // Never read an interior pointer slot beyond the committed nursery
                // (a corrupt layout could carry an oversize offset or size).
                if (scan_ptr + offset >= region_end) break;
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
            // Bounded advance: never step past the committed region.
            if (obj_size == 0 || scan_ptr >= region_end ||
                scan_ptr + obj_size > region_end) {
                scan_ptr = region_end - sizeof(void*);
            } else {
                scan_ptr += obj_size;
            }
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
        auto* gen1 = G_YoungGen().gen1_region.load(std::memory_order_acquire);
        if (gen1 != nullptr) {
            char* g1_bump = G_YoungGen().gen1_bump.load(std::memory_order_acquire);
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
    // Process the worklist populated during Phases 1 and 2.  Each promoted
    // object is scanned for nursery pointers; discovered nursery objects are
    // scavenged and appended for further BFS.  The worklist was set up and
    // result.bfs_worklist was assigned before Phase 1.
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("GcYoungCollection::CheneyBfs");
        if (result.bfs_worklist != nullptr) {
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
                        tls_bfs_worklist = new_wl;
                        tls_bfs_capacity = new_cap;
                    } else {
                        break;
                    }
                }

                void* promoted = result.bfs_worklist[idx++];
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

    // ── Phase 3d: Re-root BGC with promoted objects ──
    // If BGC was paused for this young GC, scan the promoted objects
    // (bfs_worklist) for old-gen references and push them to BGC's
    // worker deque.  This ensures BGC can trace these new old-gen
    // objects after resuming — without this, any old-gen object only
    // reachable through a promoted object would be incorrectly swept
    // by BGC when concurrent mark resumes.
    if (bgc_was_paused && result.bfs_worklist != nullptr) {
        auto& bgc_ctrl = BgcController::Instance();
        for (int i = 0; i < result.bfs_worklist_count; i++) {
            void* promoted = result.bfs_worklist[i];
            if (promoted != nullptr && G_OldGen().IsInOldGen(promoted)) {
                bgc_ctrl.PushToBgcMarkDeque(promoted);
            }
        }
        CHAOS_IL2CPP_LOG_DEBUG_M("CRAG",
            "bgc_reroot_promoted count={0}",
            static_cast<unsigned long long>(result.bfs_worklist_count));
    }

    // ── Phase 3b: Process weak GCHandles ──
    GcProcessWeakHandlesAfterYoungGC();

    // Phase 3c: Process dependent handles (ConditionalWeakTable).
    GcProcessDependentHandlesAfterYoungGC();
    pt.phase3b_ns = static_cast<uint64_t>(std::chrono::duration_cast<
        std::chrono::nanoseconds>(std::chrono::steady_clock::now() - pause_start).count())
        - pt.phase1_ns - pt.phase2_ns - pt.phase2b_ns - pt.phase3_ns;

    // ── Phase 4: Sweep young generation ──
    // Reset G_YoungGen().bump to the beginning of the region.
    // Clear all threads' TLAB ranges so they re-claim on next allocation.
    CHAOS_IL2CPP_SIZE nursery_used_bytes = static_cast<CHAOS_IL2CPP_SIZE>(
        nursery_used - nursery_begin);
    result.bytes_reclaimed = nursery_used_bytes;

    // Increment young GC counter for dynamic promotion threshold.
    int gc_count = G_YoungGen().young_gc_count_.fetch_add(1, std::memory_order_relaxed) + 1;

    // ── Phase 4: Gen1 collection ──
    // Trigger Gen1 (survivor) collection based on promotion_age_threshold.
    // When force_skip_gen1 is true, skip Gen1 entirely (gen=0 young-only path).
    if (!force_skip_gen1) {
        int threshold = G_YoungGen().promotion_age_threshold_.load(
            std::memory_order_acquire);
        bool should_collect_gen1 = (threshold <= 1) || (gc_count % threshold == 0);

        auto* gen1 = G_YoungGen().gen1_region.load(std::memory_order_acquire);
        if (gen1 != nullptr) {
            char* g1_bump = G_YoungGen().gen1_bump.load(std::memory_order_acquire);
            CHAOS_IL2CPP_SIZE g1_used = static_cast<CHAOS_IL2CPP_SIZE>(
                (g1_bump ? g1_bump : gen1->begin) - gen1->begin);
            constexpr CHAOS_IL2CPP_SIZE kGen1NearFullThreshold = 4096;
            CHAOS_IL2CPP_SIZE g1_capacity = static_cast<CHAOS_IL2CPP_SIZE>(
                G_YoungGen().gen1_end - gen1->begin);
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
                G_Scheduler().RecordGen1Collection(
                    gen1_result.bytes_promoted,
                    gen1_result.objects_in_gen1,
                    gen1_result.pause_ns);

                // ── Dynamic Gen1 resize after collection ──
                // Gen1 is now empty (bump was reset).  If the scheduler
                // recommends a significantly different size, reallocate.
                CHAOS_IL2CPP_SIZE recommended = G_Scheduler().RecommendedGen1Size();
                CHAOS_IL2CPP_SIZE current_size = static_cast<CHAOS_IL2CPP_SIZE>(
                    G_YoungGen().gen1_end - gen1->begin);
                CHAOS_IL2CPP_SIZE diff = (recommended > current_size)
                    ? (recommended - current_size)
                    : (current_size - recommended);
                if (diff > current_size / 6 && recommended > 0) {
                    ResizeGen1Region(recommended);
                    CHAOS_IL2CPP_LOG_DEBUG_M("CRAG",
                        "gen1_resize: {0} -> {1} bytes",
                        static_cast<unsigned long long>(current_size),
                        static_cast<unsigned long long>(recommended));
                }

                // ── Read scheduler-recommended promotion age ──
                // The scheduler computes the recommended threshold from
                // Gen1 EMA survival rate (smoothing out per-collection noise)
                // and pause-time cost awareness.  This replaces the previous
                // ad-hoc tuning using raw per-collection survival rate.
                int scheduler_threshold = G_Scheduler().GetRecommendedPromotionAge();

                // Gen1-occupancy override: if the Gen1 area is nearly
                // full after this collection, force threshold to 1 regardless
                // of the scheduler recommendation, preventing overflow.
                float occupancy = static_cast<float>(g1_used) /
                    static_cast<float>(g1_capacity);
                if (occupancy > 0.90f && scheduler_threshold > 1) {
                    scheduler_threshold = 1;
                }

                if (scheduler_threshold !=
                    G_YoungGen().promotion_age_threshold_.load(
                        std::memory_order_relaxed)) {
                    G_YoungGen().promotion_age_threshold_.store(
                        scheduler_threshold, std::memory_order_release);
                    CHAOS_IL2CPP_LOG_DEBUG_M("CRAG",
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
    G_YoungGen().bump.store(nursery->begin, std::memory_order_release);

    // SPB: restore region_end to emergency_start (re-carve emergency reserve)
    // and reset emergency bump so the next safepoint-bypass reuses the pool.
    char* emerg_start = G_YoungGen().emergency_start;
    G_YoungGen().region_end.store(emerg_start, std::memory_order_release);
    G_YoungGen().emergency_bump.store(emerg_start, std::memory_order_release);

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

    // ── Adaptive TLAB resizing ────────────────────────────────────
    // Snapshot TLAB utilization BEFORE the reset at the end of this block.
    // Utilization > 75% → double (up to 256 KB)
    // Utilization < 25% → halve (down to 16 KB)
    // Otherwise → keep
    if (tls_tlab.start != nullptr && tls_tlab.end > tls_tlab.start) {
        CHAOS_IL2CPP_SIZE used = static_cast<CHAOS_IL2CPP_SIZE>(
            (tls_tlab.current ? tls_tlab.current : tls_tlab.start) - tls_tlab.start);
        CHAOS_IL2CPP_SIZE total = static_cast<CHAOS_IL2CPP_SIZE>(tls_tlab.end - tls_tlab.start);
        if (total > 0) {
            double utilization = static_cast<double>(used) / static_cast<double>(total);
            if (utilization > 0.75 && tls_tlab_size < 256 * 1024) {
                tls_tlab_size = tls_tlab_size * 2;
            } else if (utilization < 0.25 && tls_tlab_size > 16 * 1024) {
                tls_tlab_size = tls_tlab_size / 2;
            }
        }
    }
    // Clamp to valid range.
    if (tls_tlab_size < 16 * 1024) tls_tlab_size = 16 * 1024;
    if (tls_tlab_size > 256 * 1024) tls_tlab_size = 256 * 1024;

    // Reset this thread's TLAB.
    tls_tlab = TLAB();

    // Clear card table entries covering the nursery range (and Gen1 range if
    // Gen1 objects exist).  This is more precise than ClearAllCards() — it
    // preserves old-gen card data that concurrent BGC mark may depend on for
    // STW re-mark (C2 fix: targeted range clear instead of global hammer).
    // When the promotion limit was reached (timed_out), keep cards dirty
    // so the next young GC correctly rescans old→nursery references from
    // partially-promoted objects.  Stale entries pointing to dead nursery
    // objects are skipped by IsInNursery.
    if (result.timed_out) {
        CHAOS_IL2CPP_LOG_WARN_M("CRAG",
            "young_collection timed_out: {0} objects promoted, "
            "keeping card table dirty",
            static_cast<unsigned long long>(result.objects_promoted));
    } else {
        // Clear nursery range — all nursery objects were promoted or dead.
        // When the nursery is empty (bump == begin), still clear the full
        // nursery range to handle manually-dirtied cards (e.g., test setups
        // where a card was dirtied before the second collection).
        uintptr_t clear_end = (nursery_used > nursery_begin)
            ? nursery_used
            : reinterpret_cast<uintptr_t>(nursery->end);
        ClearCardRange(nursery_begin, clear_end);
        // Clear Gen1 range — Phase 2b already scanned all Gen1→nursery refs.
        auto* gen1 = G_YoungGen().gen1_region.load(std::memory_order_acquire);
        if (gen1 != nullptr) {
            char* g1_bump = G_YoungGen().gen1_bump.load(std::memory_order_acquire);
            if (g1_bump > gen1->begin) {
                ClearCardRange(reinterpret_cast<uintptr_t>(gen1->begin),
                               reinterpret_cast<uintptr_t>(g1_bump));
            }
        }
    }

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

    G_Scheduler().RecordYoungCollection(
        nursery_used_bytes, result.bytes_promoted, pause_ns);

    // Report old-gen fragmentation for adaptive nursery sizing.
    // High fragmentation → shrink nursery to reduce promotion rate.
    G_Scheduler().SetOldGenFragmentation(G_OldGen().OverallFragmentation());

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

    GcEtwFireGcYoungEnd(pause_ns, result.objects_promoted, result.bytes_promoted, result.bytes_reclaimed);
    GcFireEvent(GcEvent::GC_YOUNG_DONE);

    // Resume BGC if it was paused for this young GC.
    if (bgc_was_paused) {
        BgcController::Instance().ResumeAfterYoungGc();
    }

    // M3/T8: verify every promoted BFS-worklist target landed in a legal
    // generation (OLD→old-gen page, Gen1→gen1 range).  Only at kFull (debug/CI);
    // cost is O(worklist) which is tiny next to the collection itself.  This makes
    // the previously-dead P1-A3 assertion actually run on the production young-GC
    // path instead of only the legacy PromoteNursery entry point.
    GcVerifyPromotedTracked(result);

    return result;
}

}  // namespace chaos::il2cpp::runtime_core