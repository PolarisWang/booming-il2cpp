#include "gc_gen1.h"

#include <chaos/asan_interface.h>
#include <chaos/log.h>
#include <chaos/profile.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "gc_bgc.h"
#include "gc_events.h"
#include "gc_etw.h"
#include "gc_layout.h"
#include "gc_mark_bitmap.h"
#include "gc_old_gen.h"
#include "gc_scheduler.h"
#include "gc_young_gen.h"
#include "gc_heap.h"
#include "gc_static_roots.h"
#include "thread_state.h"

// Dense, cache-friendly address set for live_demoted (vs std::unordered_set,
// whose per-node heap allocations + poor locality inflate gen1 STW pause).
// Defined AFTER includes since gc_layout.h -> chaos/unordered_dense.h supplies
// the CHAOS_IL2CPP_UNORDERED_DENSE_SET macro.
using Gen1LiveDemotedSet = CHAOS_IL2CPP_UNORDERED_DENSE_SET(uintptr_t);

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// Global Gen1 state
// ======================================================================

Gen1State g_gen1_state;

// ======================================================================
// IsInGen1
// ======================================================================

bool IsInGen1(const void* ptr) {
    if (ptr == nullptr) return false;
    auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    if (gen1 == nullptr) return false;
    auto* bump = g_young_gen.gen1_bump.load(std::memory_order_acquire);
    if (bump == nullptr) return false;
    auto* cptr = static_cast<const char*>(ptr);
    return cptr >= gen1->begin && cptr < bump;
}

bool IsInGen1Range(const void* ptr) {
    if (ptr == nullptr) return false;
    auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    if (gen1 == nullptr) return false;
    if (g_young_gen.gen1_end == nullptr) return false;
    auto* cptr = static_cast<const char*>(ptr);
    return cptr >= gen1->begin && cptr < g_young_gen.gen1_end;
}

// ======================================================================
// TryAllocateInGen1
// ======================================================================

void* TryAllocateInGen1(CHAOS_IL2CPP_SIZE size) {
    char* current = g_young_gen.gen1_bump.load(std::memory_order_acquire);
    char* next;
    do {
        if (current == nullptr) return nullptr;
        next = current + size;
        if (next > g_young_gen.gen1_end) return nullptr;
    } while (!g_young_gen.gen1_bump.compare_exchange_weak(
        current, next, std::memory_order_release, std::memory_order_acquire));
    std::memset(current, 0, size);
    G_Scheduler().RecordGen1Allocation(size);
    return current;
}

// ======================================================================
// Gen1Fragmentation
// ======================================================================

float Gen1Fragmentation() {
    auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    if (gen1 == nullptr) return 0.0f;
    auto* bump = g_young_gen.gen1_bump.load(std::memory_order_acquire);
    CHAOS_IL2CPP_SIZE total = static_cast<CHAOS_IL2CPP_SIZE>(
        g_young_gen.gen1_end - gen1->begin);
    CHAOS_IL2CPP_SIZE used = static_cast<CHAOS_IL2CPP_SIZE>(
        (bump ? bump : gen1->begin) - gen1->begin);
    if (total == 0) return 0.0f;
    float frag = 1.0f - (static_cast<float>(used) / static_cast<float>(total));
    return (frag < 0.0f) ? 0.0f : frag;
}

// ======================================================================
// GcGen1ShouldCollect — decision helper for independent Gen1 collection
// ======================================================================

/// Base Gen1 occupancy threshold (fraction of Gen1 used) to trigger collection.
/// Adjusted downward when survival rate is low (most objects die quickly).
static constexpr float kGen1OccupancyThreshold = 0.80f;

/// Minimum occupancy threshold (safety floor — never trigger below this).
static constexpr float kGen1MinOccupancyThreshold = 0.40f;

/// Gen1 fragmentation threshold to trigger collection.
static constexpr float kGen1FragThreshold = 0.50f;

/// Fragmentation threshold: trigger collection when Gen1 fragmentation exceeds this.
/// trigger collection more eagerly (lower occupancy threshold).
static constexpr float kGen1LowSurvivalThreshold = 0.30f;

bool GcGen1ShouldCollect() {
    // 1. Check Gen1 has data.
    auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    if (gen1 == nullptr) return false;
    auto* bump = g_young_gen.gen1_bump.load(std::memory_order_acquire);
    if (bump == nullptr || bump <= gen1->begin) return false;

    // 2. Check BGC is not actively marking Gen1 (GEN1_GEN2 scope).
    if (BgcController::Instance().IsGen1MarkingActive()) return false;

    CHAOS_IL2CPP_SIZE used = static_cast<CHAOS_IL2CPP_SIZE>(bump - gen1->begin);
    CHAOS_IL2CPP_SIZE capacity = static_cast<CHAOS_IL2CPP_SIZE>(
        g_young_gen.gen1_end - gen1->begin);
    if (capacity == 0) return false;

    float occupancy = static_cast<float>(used) / static_cast<float>(capacity);

    // 3a. Survival-rate adaptive occupancy threshold.
    // When survival rate is low (<30%), most objects in Gen1 are dead —
    // promote eagerly at a lower occupancy threshold (50% instead of 80%).
    // When survival is high, use the default 80% threshold.
    float survival = G_Scheduler().Gen1SurvivalRate();
    float adaptive_threshold = kGen1OccupancyThreshold;
    if (survival < kGen1LowSurvivalThreshold) {
        // Linear interpolation: survival=0% -> 40%, survival=30% -> 80%
        adaptive_threshold = kGen1MinOccupancyThreshold +
            (kGen1OccupancyThreshold - kGen1MinOccupancyThreshold) *
            (survival / kGen1LowSurvivalThreshold);
    }
    if (occupancy > adaptive_threshold) return true;

    // 3b. Check fragmentation (only when Gen1 has meaningful data).
    // High fragmentation with <25% occupancy just means "barely used" —
    // not a real fragmentation problem.
    constexpr float kMinFragUsageFraction = 0.25f;
    float frag = Gen1Fragmentation();
    if (occupancy >= kMinFragUsageFraction && frag > kGen1FragThreshold) return true;

    return false;
}

// ======================================================================
// ScanGen1ObjectPointers — scan a single Gen1 object for Gen2 pointers
// Used by full GC and BGC root scanning (not by Gen1 collection itself).
// ======================================================================

void ScanGen1ObjectPointers(const char* obj_addr, CHAOS_IL2CPP_SIZE obj_size,
                             void (*mark_callback)(void* child, void* ctx),
                             void* ctx) {
    auto& layout_registry = GcLayoutRegistry::Instance();
    const void* ti = *reinterpret_cast<const void* const*>(obj_addr);
    if (ti == nullptr) return;
    if (!layout_registry.IsValidTypeInfoPointer(ti)) {
        // Conservative fallback: scan all pointer-aligned slots.
        for (uintptr_t off = 0; off + sizeof(void*) <= obj_size; off += sizeof(void*)) {
            void* child = *reinterpret_cast<void* const*>(obj_addr + off);
            if (child != nullptr) {
                mark_callback(child, ctx);
            }
        }
        return;
    }
    uint64_t sid = layout_registry.ReadStableId(ti);
    const auto* layout = layout_registry.Lookup(sid);
    if (layout == nullptr) {
        // Fallback.
        for (uintptr_t off = 0; off + sizeof(void*) <= obj_size; off += sizeof(void*)) {
            void* child = *reinterpret_cast<void* const*>(obj_addr + off);
            if (child != nullptr) {
                mark_callback(child, ctx);
            }
        }
        return;
    }
    // Precise scan using layout pointer offsets.
    for (uint16_t i = 0; i < layout->pointer_count; i++) {
        uint16_t off = layout->pointer_offsets[i].offset;
        void* child = *reinterpret_cast<void**>(const_cast<char*>(obj_addr + off));
        if (child != nullptr) {
            mark_callback(child, ctx);
        }
    }
}

// ======================================================================
// RelocateGen1References — rewrite external refs after Gen1 moves
// ======================================================================
// GcGen1Collection relocates (promotes to Gen2 / compacts within Gen1) live
// Gen1 objects.  Any external reference (old-gen slot, static root, thread
// stack, another Gen1 object, GCHandle) still pointing at the OLD Gen1 address
// becomes stale after the move: the next Gen1 collection no longer sees it as
// Gen1-resident (IsInGen1 fails), drops the edge, and reclaims the target —
// a cross-generation use-after-free.  This rewrites every pointer that
// EXACTLY matches a moved object's old address to its new address.
//
// Exact-address match only (sorted map + binary search), mirroring
// DemotionRelocate: never rewrite values that merely fall inside a moved
// range, which would corrupt unrelated data and regress gen1_test demote.

struct Gen1MoveEntry {
    uintptr_t old_addr;
    uintptr_t new_addr;
};

static void RelocateGen1References(
    const std::vector<Gen1MoveEntry>& moves) noexcept {
    if (moves.empty()) return;

    // Sorted old→new address map for binary search.
    struct AddrPair { uintptr_t old_addr; uintptr_t new_addr; };
    std::vector<AddrPair> addr_map;
    addr_map.reserve(moves.size());
    for (auto& m : moves) {
        addr_map.push_back({m.old_addr, m.new_addr});
    }
    std::sort(addr_map.begin(), addr_map.end(),
        [](const AddrPair& a, const AddrPair& b) { return a.old_addr < b.old_addr; });

    auto rewrite_slot = [&](void** slot) {
        if (slot == nullptr || *slot == nullptr) return;
        uintptr_t val = reinterpret_cast<uintptr_t>(*slot);
        auto it = std::lower_bound(addr_map.begin(), addr_map.end(), val,
            [](const AddrPair& p, uintptr_t addr) { return p.old_addr < addr; });
        if (it != addr_map.end() && it->old_addr == val) {
            *slot = reinterpret_cast<void*>(it->new_addr);
        }
    };

    // Phase 1: Walk all old-gen pages, rewriting any slot that holds a moved
    // Gen1 object's old address.  (Old-gen pages are the primary cross-gen
    // edge source — an old object referencing a Gen1 survivor.)
    {
        const ScopedPreemptiveMode preempt;
        GcSpinLockGuard lock(G_OldGen().PageMutex());
        for (auto* page = G_OldGen().PageList(); page != nullptr; page = page->next) {
            if (!page->in_use.load(std::memory_order_acquire)) continue;
            char* payload = page->Payload();
            for (CHAOS_IL2CPP_SIZE s = 0; s < page->payload_size / sizeof(void*); s++) {
                rewrite_slot(reinterpret_cast<void**>(payload + s * sizeof(void*)));
            }
        }
    }

    // Phase 2: Walk static roots.
    GcScanStaticRoots(
        [](void* root_addr, bool /*is_interior*/, void* user_data) {
            auto* ra = static_cast<std::vector<AddrPair>*>(user_data);
            void* val = *static_cast<void* const*>(root_addr);
            if (val == nullptr) return;
            uintptr_t v = reinterpret_cast<uintptr_t>(val);
            auto it = std::lower_bound(ra->begin(), ra->end(), v,
                [](const AddrPair& p, uintptr_t addr) { return p.old_addr < addr; });
            if (it != ra->end() && it->old_addr == v) {
                *static_cast<void**>(root_addr) = reinterpret_cast<void*>(it->new_addr);
            }
        }, &addr_map);

    // Phase 3: Walk thread stacks (conservative).
    threading::GcScanAllThreadRoots(
        [](void* root_addr, bool /*is_interior*/, void* user_data) {
            if (root_addr == nullptr) return;
            auto& map = *static_cast<std::vector<AddrPair>*>(user_data);
            // root_addr is a slot on ANOTHER thread's stack (conservative scan);
            // may sit in an ASan stack-frame redzone → NoCheck read+write.
            uintptr_t val = reinterpret_cast<uintptr_t>(
                chaos::il2cpp::common::AsanReadPtrNoCheck(root_addr));
            if (val == 0) return;
            auto it = std::lower_bound(map.begin(), map.end(), val,
                [](const AddrPair& p, uintptr_t addr) { return p.old_addr < addr; });
            if (it != map.end() && it->old_addr == val) {
                chaos::il2cpp::common::AsanWritePtrNoCheck(
                    root_addr, reinterpret_cast<void*>(it->new_addr));
            }
        }, &addr_map);

    // Phase 4: Walk surviving Gen1 objects' interior pointers.  A surviving
    // Gen1 object may reference another moved Gen1 object; fix those up too.
    {
        auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
        if (gen1 != nullptr) {
            char* g1_bump = g_young_gen.gen1_bump.load(std::memory_order_acquire);
            if (g1_bump > gen1->begin) {
                char* s_cur = gen1->begin;
                auto& layout_registry = GcLayoutRegistry::Instance();
                while (s_cur < g1_bump) {
                    const void* ti = *reinterpret_cast<const void* const*>(s_cur);
                    CHAOS_IL2CPP_SIZE obj_size = kGen1MaxEstObjectSize;
                    if (ti != nullptr && layout_registry.IsValidTypeInfoPointer(ti)) {
                        uint64_t sid = layout_registry.ReadStableId(ti);
                        const auto* layout = layout_registry.Lookup(sid);
                        if (layout != nullptr && layout->instance_size > 0) {
                            obj_size = static_cast<CHAOS_IL2CPP_SIZE>(layout->instance_size);
                            for (uint16_t i = 0; i < layout->pointer_count; i++) {
                                uint16_t off = layout->pointer_offsets[i].offset;
                                rewrite_slot(reinterpret_cast<void**>(s_cur + off));
                            }
                        } else {
                            // No precise layout — conservative walk.
                            for (CHAOS_IL2CPP_SIZE off = 0;
                                 off + sizeof(void*) <= obj_size; off += sizeof(void*)) {
                                rewrite_slot(reinterpret_cast<void**>(s_cur + off));
                            }
                        }
                    } else {
                        // No valid TypeInfo — conservative walk.
                        for (CHAOS_IL2CPP_SIZE off = 0;
                             off + sizeof(void*) <= obj_size; off += sizeof(void*)) {
                            rewrite_slot(reinterpret_cast<void**>(s_cur + off));
                        }
                    }
                    s_cur += obj_size;
                }
            }
        }
    }

    // Phase 5: Relocate GCHandles.
    std::vector<std::pair<void*, void*>> handle_relocs;
    handle_relocs.reserve(moves.size());
    for (auto& m : moves) {
        handle_relocs.emplace_back(
            reinterpret_cast<void*>(m.old_addr), reinterpret_cast<void*>(m.new_addr));
    }
    GcRelocateHandles(handle_relocs);

    CHAOS_IL2CPP_LOG_DEBUG_M("CRAG", "gen1_relocate: {0} moves, {1} rewrites applied",
        static_cast<unsigned long long>(moves.size()),
        static_cast<unsigned long long>(moves.size()));
}

// ======================================================================
// GcGen1Collection
// ======================================================================

Gen1CollectionResult GcGen1Collection() {
    CHAOS_IL2CPP_PROFILE_SCOPE("GcGen1Collection");
    Gen1CollectionResult result = {};

    auto pause_start = std::chrono::steady_clock::now();

    // ── Phase 1: Determine Gen1 range ──
    auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    if (gen1 == nullptr) {
        return result;  // No Gen1 area configured.
    }
    char* s_begin = gen1->begin;
    char* s_bump  = g_young_gen.gen1_bump.load(std::memory_order_acquire);
    if (s_bump == nullptr || s_bump <= s_begin) {
        return result;  // Gen1 is empty.
    }

    uintptr_t gen1_begin = reinterpret_cast<uintptr_t>(s_begin);
    uintptr_t gen1_end   = reinterpret_cast<uintptr_t>(s_bump);
    CHAOS_IL2CPP_SIZE span = static_cast<CHAOS_IL2CPP_SIZE>(gen1_end - gen1_begin);

    CHAOS_IL2CPP_LOG_INFO_M("CRAG", "gen1_collection range=[{0}, {1}) span={2}",
        static_cast<void*>(s_begin), static_cast<void*>(s_bump),
        static_cast<unsigned long long>(span));

    GcFireEvent(GcEvent::GC_GEN1_COLLECT);

    // ── Phase 1b: Early exit for small survivors ──
    // Two-tier fast path: (a) span-based for minimal survivors,
    // (b) object-count-based to skip mark-sweep when total objects
    // are few (even if span is large due to sparse dead objects).
    // Both avoid bitmap allocation + root marking + sweep overhead.

    // Tier 1: Very small span — promote all with no counting needed.
    if (span <= 4096) {
        CHAOS_IL2CPP_LOG_DEBUG_M("CRAG", "gen1_early_exit: span=%llu bytes",
            static_cast<unsigned long long>(span));
        auto& tiny_layout = GcLayoutRegistry::Instance();
        char* tiny_cur = s_begin;
        while (tiny_cur < s_bump) {
            result.objects_in_gen1++;
            const void* ti = *reinterpret_cast<const void* const*>(tiny_cur);
            CHAOS_IL2CPP_SIZE obj_size = kGen1MaxEstObjectSize;
            if (ti != nullptr && tiny_layout.IsValidTypeInfoPointer(ti)) {
                uint64_t sid = tiny_layout.ReadStableId(ti);
                const auto* layout = tiny_layout.Lookup(sid);
                if (layout != nullptr && layout->instance_size > 0) {
                    obj_size = static_cast<CHAOS_IL2CPP_SIZE>(layout->instance_size);
                }
            }
            void* gen2_addr = G_OldGen().Allocate(obj_size, true);
            if (gen2_addr != nullptr) {
                std::memcpy(gen2_addr, tiny_cur, obj_size);
                result.objects_promoted++;
                result.bytes_promoted += obj_size;
            } else {
                result.promotion_failed = true;
                CHAOS_IL2CPP_LOG_ERROR("CRAG", "gen1_early_exit: Gen2 OOM");
            }
            tiny_cur += obj_size;
        }
        if (result.promotion_failed) {
            // Don't reset gen1_bump — data is still in Gen1 and needs retry.
            result.bytes_promoted = 0;
            result.objects_promoted = 0;
        } else {
            g_young_gen.gen1_bump.store(gen1->begin, std::memory_order_release);
            g_young_gen.gen1_prev_compact_end = nullptr;
        }
        auto pause_end = std::chrono::steady_clock::now();
        result.pause_ns = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(pause_end - pause_start).count());
        CHAOS_IL2CPP_LOG_INFO_M("CRAG",
            "gen1_early_exit done: promoted={0} objs, {1} bytes; pause={2} ns",
            static_cast<unsigned long long>(result.objects_promoted),
            static_cast<unsigned long long>(result.bytes_promoted),
            static_cast<unsigned long long>(result.pause_ns));
        g_gen1_state.collection_count.fetch_add(1, std::memory_order_relaxed);
        g_gen1_state.last_survived_bytes = result.bytes_promoted;
        g_gen1_state.total_allocated.store(0, std::memory_order_release);
        return result;
    }

    // Tier 2: Object-count-based early exit.  Walk the range counting
    // objects; if count stays below kGen1MinPromoteThreshold, promote all.
    // The counting pass also pre-computes objects_in_gen1, eliminating
    // the redundant recount in Phase 4 below.
    {
        auto& ec_layout = GcLayoutRegistry::Instance();
        char* ec_cur = s_begin;
        CHAOS_IL2CPP_SIZE ec_count = 0;
        while (ec_cur < s_bump) {
            ec_count++;
            const void* ti = *reinterpret_cast<const void* const*>(ec_cur);
            CHAOS_IL2CPP_SIZE sz = kGen1MaxEstObjectSize;
            if (ti != nullptr && ec_layout.IsValidTypeInfoPointer(ti)) {
                uint64_t sid = ec_layout.ReadStableId(ti);
                const auto* layout = ec_layout.Lookup(sid);
                if (layout != nullptr && layout->instance_size > 0) {
                    sz = static_cast<CHAOS_IL2CPP_SIZE>(layout->instance_size);
                }
            }
            ec_cur += sz;
        }
        result.objects_in_gen1 = ec_count;

        if (ec_count <= kGen1MinPromoteThreshold) {
            CHAOS_IL2CPP_LOG_DEBUG_M("CRAG", "gen1_early_exit: objects=%llu span=%llu",
                static_cast<unsigned long long>(ec_count),
                static_cast<unsigned long long>(span));
            char* ec2_cur = s_begin;
            while (ec2_cur < s_bump) {
                const void* ti2 = *reinterpret_cast<const void* const*>(ec2_cur);
                CHAOS_IL2CPP_SIZE sz2 = kGen1MaxEstObjectSize;
                if (ti2 != nullptr && ec_layout.IsValidTypeInfoPointer(ti2)) {
                    uint64_t sid2 = ec_layout.ReadStableId(ti2);
                    const auto* l2 = ec_layout.Lookup(sid2);
                    if (l2 != nullptr && l2->instance_size > 0) {
                        sz2 = static_cast<CHAOS_IL2CPP_SIZE>(l2->instance_size);
                    }
                }
                void* gen2_addr = G_OldGen().Allocate(sz2, true);
                if (gen2_addr != nullptr) {
                    std::memcpy(gen2_addr, ec2_cur, sz2);
                    result.objects_promoted++;
                    result.bytes_promoted += sz2;
                } else {
                    result.promotion_failed = true;
                    CHAOS_IL2CPP_LOG_ERROR("CRAG", "gen1_early_exit: Gen2 OOM");
                }
                ec2_cur += sz2;
            }
            if (result.promotion_failed) {
                result.bytes_promoted = 0;
                result.objects_promoted = 0;
            } else {
                g_young_gen.gen1_bump.store(gen1->begin, std::memory_order_release);
                g_young_gen.gen1_prev_compact_end = nullptr;
            }
            auto pause_end = std::chrono::steady_clock::now();
            result.pause_ns = static_cast<uint64_t>(
                std::chrono::duration_cast<std::chrono::nanoseconds>(pause_end - pause_start).count());
            CHAOS_IL2CPP_LOG_INFO_M("CRAG",
                "gen1_early_exit done: promoted={0} objs, {1} bytes; pause={2} ns",
                static_cast<unsigned long long>(result.objects_promoted),
                static_cast<unsigned long long>(result.bytes_promoted),
                static_cast<unsigned long long>(result.pause_ns));
            g_gen1_state.collection_count.fetch_add(1, std::memory_order_relaxed);
            g_gen1_state.last_survived_bytes = result.bytes_promoted;
            g_gen1_state.total_allocated.store(0, std::memory_order_release);
            return result;
        }
    }

    // ── Phase 2: Allocate temporary mark bitmap ──
    // Each bit covers one pointer-aligned (8-byte) slot.
    CHAOS_IL2CPP_SIZE num_slots = span / sizeof(void*);
    CHAOS_IL2CPP_SIZE bitmap_bytes = (num_slots / 8) + 1;  // +1 for remainder
    // Use stack allocation for small bitmaps to avoid malloc overhead.
    uint8_t stack_bitmap[128];  // covers 1024 slots = 8 KB span
    uint8_t* bitmap_raw = nullptr;
    bool used_stack = false;
    if (bitmap_bytes <= sizeof(stack_bitmap)) {
        bitmap_raw = stack_bitmap;
        used_stack = true;
    } else {
        bitmap_raw = static_cast<uint8_t*>(CHAOS_IL2CPP_MALLOC(bitmap_bytes));
    }
    if (bitmap_raw == nullptr) {
        CHAOS_IL2CPP_LOG_ERROR("CRAG", "gen1 OOM allocating mark bitmap, falling back to promote-all");
        // Fall back to promote-all (drain) — conservative: assume all objects are
        // live and promote each to Gen2. This avoids data loss when the mark bitmap
        // cannot be allocated (e.g., under extreme memory pressure).
        auto& drain_layout = GcLayoutRegistry::Instance();
        char* drain_cur = s_begin;
        while (drain_cur < s_bump) {
            result.objects_in_gen1++;
            const void* ti = *reinterpret_cast<const void* const*>(drain_cur);
            CHAOS_IL2CPP_SIZE obj_size = kGen1MaxEstObjectSize;
            if (ti != nullptr && drain_layout.IsValidTypeInfoPointer(ti)) {
                uint64_t sid = drain_layout.ReadStableId(ti);
                const auto* layout = drain_layout.Lookup(sid);
                if (layout != nullptr && layout->instance_size > 0) {
                    obj_size = static_cast<CHAOS_IL2CPP_SIZE>(layout->instance_size);
                }
            }
            void* gen2_addr = G_OldGen().Allocate(obj_size, true);
            if (gen2_addr != nullptr) {
                std::memcpy(gen2_addr, drain_cur, obj_size);
                result.objects_promoted++;
                result.bytes_promoted += obj_size;
            } else {
                result.promotion_failed = true;
                CHAOS_IL2CPP_LOG_ERROR_M("CRAG", "gen1 drain: Gen2 OOM at object offset={0}",
                    static_cast<unsigned long long>(drain_cur - s_begin));
            }
            drain_cur += obj_size;
        }
        if (result.promotion_failed) {
            result.bytes_promoted = 0;
            result.objects_promoted = 0;
        } else {
            g_young_gen.gen1_bump.store(gen1->begin, std::memory_order_release);
            g_young_gen.gen1_prev_compact_end = nullptr;
        }
        auto pause_end = std::chrono::steady_clock::now();
        result.pause_ns = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(pause_end - pause_start).count());
        CHAOS_IL2CPP_LOG_INFO_M("CRAG",
            "gen1_collection drain done: promoted={0} objs, {1} bytes; pause={2} ns",
            static_cast<unsigned long long>(result.objects_promoted),
            static_cast<unsigned long long>(result.bytes_promoted),
            static_cast<unsigned long long>(result.pause_ns));
        return result;
    }
    std::memset(bitmap_raw, 0, bitmap_bytes);
    GcMarkBitmap mark_bm(bitmap_raw, bitmap_bytes);

    // ── Phase 2.5: cross-page demoted-liveness tracking (GC-N6 #13 Phase2) ──
    // In-place demoted objects live in OLD-GEN pages, not the gen1 region, so the
    // gen1_begin-relative mark bitmap cannot index them.  Track live-demoted
    // addresses in a set as root scans discover references to them.  The gen1
    // collection then promotes / keeps / reclaims each demoted object in Phase 4.
    Gen1LiveDemotedSet live_demoted;
    // Mark a child pointer as gen1-live: if it's in the gen1 region, set the mark
    // bitmap bit; if it's an in-place demoted old-gen object, record it in
    // live_demoted (later promoted/kept/reclaimed in Phase 4).
    //
    // CRITICAL (base/interior asymmetry, review #1): conservative scans and
    // interior references record a RAW pointer that may be INTERIOR to the object
    // (IsInDemotedSet returns true for interior pointers, gc_old_gen.h:160).
    // Phase 4f keys liveness on the object BASE (page->demoted[i].addr), so every
    // demoted hit must be normalized to its base via IsInDemotedSetGetBase —
    // storing the raw interior value would make a base-address lookup MISS a live
    // demoted object, clear its old-gen mark bits, and the next sweep would
    // reclaim an object still referenced on a thread stack (use-after-free).
    auto mark_child = [&](void* child) {
        if (child == nullptr) return;
        if (IsInGen1(child)) {
            uintptr_t child_addr = reinterpret_cast<uintptr_t>(child);
            CHAOS_IL2CPP_SIZE slot_idx = (child_addr - gen1_begin) / sizeof(void*);
            mark_bm.MarkRange(slot_idx, 1);
        } else {
            char* demoted_base = IsInDemotedSetGetBase(child);
            if (demoted_base != nullptr) {
                live_demoted.insert(reinterpret_cast<uintptr_t>(demoted_base));
            }
        }
    };

    // ── Phase 3: Mark roots ──

    // 3a: Scan Gen0 (young half of nursery) for pointers into Gen1.
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("Gen1_Root_Gen0");
        auto* nursery = g_young_gen.region.load(std::memory_order_acquire);
        if (nursery != nullptr) {
            char* n_cur = nursery->begin;
            char* n_end = g_young_gen.bump.load(std::memory_order_acquire);
            auto& layout_registry = GcLayoutRegistry::Instance();
            while (n_cur < n_end) {
                const void* ti = *reinterpret_cast<const void* const*>(n_cur);
                CHAOS_IL2CPP_SIZE nobj_size = kGen1MaxEstObjectSize;
                if (ti != nullptr && layout_registry.IsValidTypeInfoPointer(ti)) {
                    uint64_t sid = layout_registry.ReadStableId(ti);
                    const auto* layout = layout_registry.Lookup(sid);
                    if (layout != nullptr && layout->instance_size > 0) {
                        nobj_size = static_cast<CHAOS_IL2CPP_SIZE>(layout->instance_size);
                        for (uint16_t i = 0; i < layout->pointer_count; i++) {
                            uint16_t off = layout->pointer_offsets[i].offset;
                            void* child = *reinterpret_cast<void**>(n_cur + off);
                            mark_child(child);
                        }
                    } else {
                        // Fallback: scan all slots.
                        for (CHAOS_IL2CPP_SIZE off = 0; off + sizeof(void*) <= nobj_size; off += sizeof(void*)) {
                            void* child = *reinterpret_cast<void**>(n_cur + off);
                            mark_child(child);
                        }
                    }
                } else {
                    // No valid TypeInfo; conservative scan.
                    for (CHAOS_IL2CPP_SIZE off = 0; off + sizeof(void*) <= nobj_size; off += sizeof(void*)) {
                        void* child = *reinterpret_cast<void**>(n_cur + off);
                        mark_child(child);
                    }
                }
                n_cur += nobj_size;
            }
        }
    }

    // 3b: Scan Gen2 dirty cards for pointers into Gen1.
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("Gen1_Root_DirtyCards");
        G_OldGen().ScanDirtyCardsInPages(
            [&](uintptr_t /*card_idx*/, uintptr_t card_start, uintptr_t card_end) {
                for (uintptr_t slot = card_start; slot < card_end; slot += sizeof(void*)) {
                    void* val = *reinterpret_cast<void**>(slot);
                    mark_child(val);
                }
            });
    }

    // 3c: Scan thread stacks (conservative).
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("Gen1_Root_Stacks");
        struct StackCtx { uintptr_t gen1_begin; GcMarkBitmap* bm; Gen1LiveDemotedSet* live_demoted; };
        StackCtx sctx{gen1_begin, &mark_bm, &live_demoted};
        threading::GcScanAllThreadRoots(
            [](void* root_addr, bool /*is_interior*/, void* user_data) {
                auto* ctx = static_cast<StackCtx*>(user_data);
                // root_addr is a slot on ANOTHER thread's stack (conservative
                // GcScanAllThreadRoots); it may sit in an ASan stack-frame
                // redzone.  Un-instrumented read avoids a false
                // stack-buffer-underflow (byte-identical access, ASan only
                // elided) — see gc_gen1.cpp:655 task#16.
                void* val = chaos::il2cpp::common::AsanReadPtrNoCheck(root_addr);
                if (val == nullptr) return;
                if (IsInGen1(val)) {
                    uintptr_t child_addr = reinterpret_cast<uintptr_t>(val);
                    CHAOS_IL2CPP_SIZE slot_idx = (child_addr - ctx->gen1_begin) / sizeof(void*);
                    ctx->bm->MarkRange(slot_idx, 1);
                } else {
                    // Normalize interior stack pointers to the demoted object base
                    // (same base-key rule as mark_child) — see review #1.
                    char* demoted_base = IsInDemotedSetGetBase(val);
                    if (demoted_base != nullptr) {
                        ctx->live_demoted->insert(reinterpret_cast<uintptr_t>(demoted_base));
                    }
                }
            }, &sctx);
    }

    // 3d: Scan GCHandles for Gen1 pointers.
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("Gen1_Root_GCHandles");
        struct HandleCtx { uintptr_t gen1_begin; GcMarkBitmap* bm; Gen1LiveDemotedSet* live_demoted; };
        HandleCtx hctx{gen1_begin, &mark_bm, &live_demoted};
        GcIterateTenuredHandles(
            [](void* obj, void* user_data) {
                if (obj == nullptr) return;
                auto* context = static_cast<HandleCtx*>(user_data);
                if (IsInGen1(obj)) {
                    uintptr_t obj_addr = reinterpret_cast<uintptr_t>(obj);
                    CHAOS_IL2CPP_SIZE slot_idx = (obj_addr - context->gen1_begin) / sizeof(void*);
                    context->bm->MarkRange(slot_idx, 1);
                } else {
                    // GCHandles store the object base, but normalize defensively —
                    // a handle may point at an interior offset for some projections.
                    // Key liveness on the base so Phase 4f lookup is consistent.
                    char* demoted_base = IsInDemotedSetGetBase(obj);
                    if (demoted_base != nullptr) {
                        context->live_demoted->insert(reinterpret_cast<uintptr_t>(demoted_base));
                    }
                }
            }, &hctx);
    }

    // ── Phase 3.5: demoted-object transitive closure (GC-N6 #13 Phase2) ──
    // A live demoted object may reference another demoted object (demoted→demoted
    // edge); those children must also be live.  BFS over live_demoted: scan each
    // live demoted object's interior pointers (precise layout) and mark any demoted
    // child live.  (Root scans in Phase 3 already added direct-reach demoted objs.)
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("Gen1_Demoted_Closure");
        // Every element is a demoted-object BASE address (all insert points
        // normalize via IsInDemotedSetGetBase), so dereferencing as an object
        // header is well-formed — a raw interior/false pointer can never reach
        // this loop.  IsValidTypeInfoPointer below remains a defensive fallback.
        std::vector<uintptr_t> worklist(live_demoted.begin(), live_demoted.end());
        while (!worklist.empty()) {
            uintptr_t addr = worklist.back();
            worklist.pop_back();
            const void* ti = *reinterpret_cast<const void* const*>(addr);
            auto& lreg = GcLayoutRegistry::Instance();
            if (ti == nullptr || !lreg.IsValidTypeInfoPointer(ti)) continue;
            uint64_t sid = lreg.ReadStableId(ti);
            const auto* layout = lreg.Lookup(sid);
            if (layout == nullptr || layout->pointer_count == 0) continue;
            for (uint16_t i = 0; i < layout->pointer_count; i++) {
                uint16_t off = layout->pointer_offsets[i].offset;
                void* child = *reinterpret_cast<void**>(addr + off);
                if (child == nullptr) continue;
                // Demoted child edge: normalize to base before inserting, so the
                // base-key invariant of live_demoted is preserved across closure.
                char* demoted_base = IsInDemotedSetGetBase(child);
                if (demoted_base != nullptr) {
                    uintptr_t child_addr = reinterpret_cast<uintptr_t>(demoted_base);
                    if (live_demoted.insert(child_addr).second) {
                        worklist.push_back(child_addr);
                    }
                } else if (IsInGen1(child)) {
                    uintptr_t child_addr = reinterpret_cast<uintptr_t>(child);
                    CHAOS_IL2CPP_SIZE slot_idx = (child_addr - gen1_begin) / sizeof(void*);
                    mark_bm.MarkRange(slot_idx, 1);
                }
            }
        }
    }

    // ── Phase 4: Walk Gen1 objects, promote old survivors / compact new ──

    {
        CHAOS_IL2CPP_PROFILE_SCOPE("Gen1_PromoteAndCompact");
        char* s_cur = s_begin;
        auto& sv_layout_registry = GcLayoutRegistry::Instance();
        // Track in local vars; only commit to result if no failures.
        CHAOS_IL2CPP_SIZE local_promoted_count = 0;
        CHAOS_IL2CPP_SIZE local_bytes_promoted = 0;
        CHAOS_IL2CPP_SIZE local_compacted = 0;
        CHAOS_IL2CPP_SIZE local_bytes_compacted = 0;
        CHAOS_IL2CPP_SIZE local_reclaimed = 0;
        bool local_failed = false;

        // ── Phase 4a: Determine old/new boundary ──
        // Objects below gen1_prev_compact_end have survived >=1 Gen1 collection
        // and are promoted to Gen2.  Objects above are freshly promoted from
        // Gen0 and stay in Gen1 via compaction.
        char* boundary = g_young_gen.gen1_prev_compact_end;
        if (boundary == nullptr || boundary < s_begin) {
            boundary = s_begin;  // No previous collection → all are "new"
        }
        if (boundary > s_bump) {
            boundary = s_bump;   // Clamp to current range.
        }

        // Allocate compaction buffer.  Max size = full Gen1 capacity.
        CHAOS_IL2CPP_SIZE gen1_capacity = static_cast<CHAOS_IL2CPP_SIZE>(
            g_young_gen.gen1_end - s_begin);
        char* compact_buf = static_cast<char*>(CHAOS_IL2CPP_MALLOC(gen1_capacity));
        if (compact_buf == nullptr) {
            CHAOS_IL2CPP_LOG_ERROR("CRAG", "gen1 OOM allocating compact buffer, falling back to promote-all");
            // Fall back to promote-all (drain Gen1) — conservative.
            auto& drain_layout = GcLayoutRegistry::Instance();
            char* drain_cur = s_begin;
            while (drain_cur < s_bump) {
                const void* ti = *reinterpret_cast<const void* const*>(drain_cur);
                CHAOS_IL2CPP_SIZE obj_size = kGen1MaxEstObjectSize;
                if (ti != nullptr && drain_layout.IsValidTypeInfoPointer(ti)) {
                    uint64_t sid = drain_layout.ReadStableId(ti);
                    const auto* layout = drain_layout.Lookup(sid);
                    if (layout != nullptr && layout->instance_size > 0) {
                        obj_size = static_cast<CHAOS_IL2CPP_SIZE>(layout->instance_size);
                    }
                }
                void* gen2_addr = G_OldGen().Allocate(obj_size, true);
                if (gen2_addr != nullptr) {
                    std::memcpy(gen2_addr, drain_cur, obj_size);
                    local_promoted_count++;
                    local_bytes_promoted += obj_size;
                } else {
                    local_failed = true;
                    CHAOS_IL2CPP_LOG_ERROR("CRAG", "gen1 drain: Gen2 OOM");
                }
                drain_cur += obj_size;
            }
            if (!local_failed) {
                g_young_gen.gen1_bump.store(gen1->begin, std::memory_order_release);
                g_young_gen.gen1_prev_compact_end = nullptr;
            }
            result.objects_promoted = local_promoted_count;
            result.bytes_promoted = local_bytes_promoted;
            result.bytes_reclaimed = local_reclaimed;
            result.promotion_failed = local_failed;
            goto phase6_cleanup;
        }

        // ── Phase 4b: Collect "old" live objects (promote batch) ──
        // and "new" live objects (compaction buffer).
        // Use a stack-allocated batch buffer for old-object promotion.
        static constexpr int kPromoteBatchMax = 256;
        struct PromoteEntry { char* src; CHAOS_IL2CPP_SIZE size; };
        PromoteEntry batch_buf[kPromoteBatchMax];
        int batch_count = 0;
        // Moves: old Gen1 address → new address (old-gen for promotion, Gen1
        // start for compaction), collected during 4b/4c and used to rewrite
        // external references after the objects physically move.
        std::vector<Gen1MoveEntry> moves;
        moves.reserve(512);

        while (s_cur < s_bump) {
            const void* ti = *reinterpret_cast<const void* const*>(s_cur);
            CHAOS_IL2CPP_SIZE obj_size = kGen1MaxEstObjectSize;
            if (ti != nullptr && sv_layout_registry.IsValidTypeInfoPointer(ti)) {
                uint64_t sid = sv_layout_registry.ReadStableId(ti);
                const auto* layout = sv_layout_registry.Lookup(sid);
                if (layout != nullptr && layout->instance_size > 0) {
                    obj_size = static_cast<CHAOS_IL2CPP_SIZE>(layout->instance_size);
                }
            }

            uintptr_t obj_addr = reinterpret_cast<uintptr_t>(s_cur);
            CHAOS_IL2CPP_SIZE slot_idx = (obj_addr - gen1_begin) / sizeof(void*);
            bool is_live = mark_bm.TestSlot(slot_idx);

            if (is_live) {
                if (s_cur < boundary) {
                    // "Old" survivor (survived >=1 Gen1 collection) → promote to Gen2.
                    if (batch_count < kPromoteBatchMax) {
                        batch_buf[batch_count++] = {s_cur, obj_size};
                    } else {
                        // Batch overflow: promote directly.
                        void* gen2_addr = G_OldGen().Allocate(obj_size, true);
                        if (gen2_addr != nullptr) {
                            std::memcpy(gen2_addr, s_cur, obj_size);
                            moves.push_back({reinterpret_cast<uintptr_t>(s_cur),
                                             reinterpret_cast<uintptr_t>(gen2_addr)});
                            local_promoted_count++;
                            local_bytes_promoted += obj_size;
                        } else {
                            local_failed = true;
                            CHAOS_IL2CPP_LOG_ERROR("CRAG",
                                "gen1 Gen2 OOM promoting old object");
                        }
                    }
                } else {
                    // "New" survivor (fresh from Gen0) → compact in Gen1.
                    std::memcpy(compact_buf + local_bytes_compacted, s_cur, obj_size);
                    // Record the move BEFORE advancing the running offset: the
                    // object at s_cur moves to s_begin + local_bytes_compacted.
                    moves.push_back({reinterpret_cast<uintptr_t>(s_cur),
                                     reinterpret_cast<uintptr_t>(s_begin + local_bytes_compacted)});
                    local_compacted++;
                    local_bytes_compacted += obj_size;
                }
            } else {
                local_reclaimed += obj_size;
            }

            s_cur += obj_size;
        }

        // ── Phase 4c: Promote batched old objects to Gen2 ──
        for (int i = 0; i < batch_count && !local_failed; i++) {
            void* gen2_addr = G_OldGen().Allocate(batch_buf[i].size, true);
            if (gen2_addr != nullptr) {
                std::memcpy(gen2_addr, batch_buf[i].src, batch_buf[i].size);
                moves.push_back({reinterpret_cast<uintptr_t>(batch_buf[i].src),
                                 reinterpret_cast<uintptr_t>(gen2_addr)});
                local_promoted_count++;
                local_bytes_promoted += batch_buf[i].size;
            } else {
                local_failed = true;
                CHAOS_IL2CPP_LOG_ERROR("CRAG",
                    "gen1 Gen2 OOM promoting batched old object");
            }
        }

        // Commit results (only if no failures).
        if (!local_failed) {
            // Phase 4d: Compact "new" survivors back to Gen1 start.
            std::memcpy(s_begin, compact_buf, local_bytes_compacted);
            g_young_gen.gen1_bump.store(s_begin + local_bytes_compacted,
                                         std::memory_order_release);
            g_young_gen.gen1_prev_compact_end = s_begin + local_bytes_compacted;

            result.objects_promoted = local_promoted_count;
            result.bytes_promoted = local_bytes_promoted;
            result.bytes_compacted = local_compacted;
            result.bytes_reclaimed = local_reclaimed;
            result.promotion_failed = false;

            // Phase 4e: Rewrite external references to the moved Gen1 objects.
            // After promotion/compaction the objects physically moved; any old-gen
            // slot / static root / thread / surviving-Gen1 interior / GCHandle that
            // still points at the OLD Gen1 address would reference recycled memory
            // on the next Gen1 collection (cross-gen UAF, GC-N6 mode3).  Relocate
            // exact old→new matches now, while the old addresses are still mapped.
            RelocateGen1References(moves);

            // ── Phase 4f: in-place demoted object liveness (GC-N6 #13 Phase2) ──
            // Gen1-owned objects physically resident in old-gen pages.  Do this
            // AFTER gen1 commit (gen1_bump/prev_compact_end stable).
            //   Live demoted (in live_demoted via Phase 3/3.5):
            //     - survived a prior gen1 collection (must_promote) -> PROMOTE to
            //       gen2: remove from demoted set (mark bit already set = gen2-live;
            //       already in old-gen, no move needed).
            //     - first survival -> keep as gen1-owned, set must_promote=true.
            //   Dead demoted (not in live_demoted) -> clear its old-gen mark bits so
            //     a later old-gen sweep reclaims the space; remove from demoted set.
            {
                const ScopedPreemptiveMode preempt;
                GcSpinLockGuard lock(G_OldGen().PageMutex());
                for (auto* page = G_OldGen().PageList(); page != nullptr; page = page->next) {
                    if (!page->in_use.load(std::memory_order_acquire)) continue;
                    // Iterate backwards-safe: DemoteRemove swaps-with-last.
                    for (int32_t i = 0; i < page->demoted_count.load(std::memory_order_acquire); ) {
                        auto& e = page->demoted[i];
                        if (live_demoted.find(reinterpret_cast<uintptr_t>(e.addr)) != live_demoted.end()) {
                            // Live.
                            if (e.must_promote) {
                                // Promote in place: become a normal gen2 object →
                                // just drop from the demoted set (mark bit already set).
                                page->DemoteRemove(e.addr);
                            } else {
                                e.must_promote = true;
                                i++;
                            }
                        } else {
                            // Dead: clear this object's old-gen mark bits so sweep reclaims it.
                            auto bm_d = GcMarkBitmap(page->MarkBitmap(), page->bitmap_bytes);
                            char* payload = page->Payload();
                            uintptr_t obj_addr = reinterpret_cast<uintptr_t>(e.addr);
                            uintptr_t payload_base = reinterpret_cast<uintptr_t>(payload);
                            if (obj_addr >= payload_base) {
                                CHAOS_IL2CPP_SIZE slot_off = (obj_addr - payload_base) / sizeof(void*);
                                CHAOS_IL2CPP_SIZE n_slots = (e.size + sizeof(void*) - 1) / sizeof(void*);
                                bm_d.ClearRange(slot_off, n_slots);
                            }
                            page->DemoteRemove(e.addr);
                        }
                    }
                }
            }
        } else {
            // Some promotions failed — preserve Gen1 state for retry.
            // Don't update gen1_bump or gen1_prev_compact_end.
            result.promotion_failed = true;
            result.bytes_reclaimed = 0;
        }

        CHAOS_IL2CPP_FREE(compact_buf);
    }

    // ── Phase 5: Skip (gen1_bump and gen1_prev_compact_end updated in Phase 4d) ──

    goto phase6_cleanup;

phase6_cleanup:
    // ── Phase 6: Cleanup (shared with fallback paths above) ──
    if (!used_stack) {
        CHAOS_IL2CPP_FREE(bitmap_raw);
    }

    // ── Phase 7: Record statistics ──
    auto pause_end = std::chrono::steady_clock::now();
    result.pause_ns = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(pause_end - pause_start).count());

    g_gen1_state.total_allocated.store(0, std::memory_order_release);
    g_gen1_state.collection_count.fetch_add(1, std::memory_order_relaxed);
    g_gen1_state.last_survived_bytes = result.bytes_promoted;

    CHAOS_IL2CPP_LOG_INFO_M("CRAG",
        "gen1_collection done: promoted={0} objs, {1} bytes; compacted={2} bytes; reclaimed={3} bytes; pause={4} ns",
        static_cast<unsigned long long>(result.objects_promoted),
        static_cast<unsigned long long>(result.bytes_promoted),
        static_cast<unsigned long long>(result.bytes_compacted),
        static_cast<unsigned long long>(result.bytes_reclaimed),
        static_cast<unsigned long long>(result.pause_ns));

    GcEtwFireGcGen1Collect(result.pause_ns, result.objects_promoted, result.bytes_reclaimed);

    return result;
}

}  // namespace chaos::il2cpp::runtime_core
