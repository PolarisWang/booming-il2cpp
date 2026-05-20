#include "gc_gen1.h"

#include <chaos/log.h>
#include <chaos/profile.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "gc_events.h"
#include "gc_layout.h"
#include "gc_mark_bitmap.h"
#include "gc_old_gen.h"
#include "gc_young_gen.h"
#include "thread_state.h"

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
    if (g_young_gen.survivor_begin == nullptr) return false;
    auto* s_bump = g_young_gen.survivor_bump.load(std::memory_order_acquire);
    if (s_bump == nullptr) return false;
    auto* cptr = static_cast<const char*>(ptr);
    return cptr >= g_young_gen.survivor_begin && cptr < s_bump;
}

// ======================================================================
// TryAllocateInGen1
// ======================================================================

void* TryAllocateInGen1(CHAOS_IL2CPP_SIZE size) {
    char* current = g_young_gen.survivor_bump.load(std::memory_order_acquire);
    char* next;
    do {
        if (current == nullptr) return nullptr;
        next = current + size;
        if (next > g_young_gen.survivor_end) return nullptr;
    } while (!g_young_gen.survivor_bump.compare_exchange_weak(
        current, next, std::memory_order_release, std::memory_order_acquire));
    std::memset(current, 0, size);
    return current;
}

// ======================================================================
// Gen1Fragmentation
// ======================================================================

float Gen1Fragmentation() {
    if (g_young_gen.survivor_begin == nullptr) return 0.0f;
    auto* s_bump = g_young_gen.survivor_bump.load(std::memory_order_acquire);
    CHAOS_IL2CPP_SIZE total = static_cast<CHAOS_IL2CPP_SIZE>(
        g_young_gen.survivor_end - g_young_gen.survivor_begin);
    CHAOS_IL2CPP_SIZE used = static_cast<CHAOS_IL2CPP_SIZE>(
        (s_bump ? s_bump : g_young_gen.survivor_begin) - g_young_gen.survivor_begin);
    if (total == 0) return 0.0f;
    // Fragmentation = 1 - (used / total), capped at 0 (empty Gen1).
    float frag = 1.0f - (static_cast<float>(used) / static_cast<float>(total));
    return (frag < 0.0f) ? 0.0f : frag;
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
// GcGen1Collection
// ======================================================================

Gen1CollectionResult GcGen1Collection() {
    CHAOS_IL2CPP_PROFILE_SCOPE("GcGen1Collection");
    Gen1CollectionResult result = {};

    auto pause_start = std::chrono::steady_clock::now();

    // ── Phase 1: Determine Gen1 range ──
    if (g_young_gen.survivor_begin == nullptr) {
        return result;  // No Gen1 area configured.
    }
    char* s_begin = g_young_gen.survivor_begin;
    char* s_bump  = g_young_gen.survivor_bump.load(std::memory_order_acquire);
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
        CHAOS_IL2CPP_LOG_DEBUG("CRAG", "gen1_early_exit: span=%llu bytes",
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
            void* gen2_addr = g_old_gen.Allocate(obj_size, true);
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
        g_young_gen.survivor_bump.store(g_young_gen.survivor_begin, std::memory_order_release);
        auto pause_end = std::chrono::steady_clock::now();
        result.pause_ns = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(pause_end - pause_start).count());
        CHAOS_IL2CPP_LOG_INFO_M("CRAG",
            "gen1_early_exit done: promoted={0} objs, {1} bytes; pause={2} ns",
            static_cast<unsigned long long>(result.objects_promoted),
            static_cast<unsigned long long>(result.bytes_promoted),
            static_cast<unsigned long long>(result.pause_ns));
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
            CHAOS_IL2CPP_LOG_DEBUG("CRAG", "gen1_early_exit: objects=%llu span=%llu",
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
                void* gen2_addr = g_old_gen.Allocate(sz2, true);
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
            g_young_gen.survivor_bump.store(g_young_gen.survivor_begin, std::memory_order_release);
            auto pause_end = std::chrono::steady_clock::now();
            result.pause_ns = static_cast<uint64_t>(
                std::chrono::duration_cast<std::chrono::nanoseconds>(pause_end - pause_start).count());
            CHAOS_IL2CPP_LOG_INFO_M("CRAG",
                "gen1_early_exit done: promoted={0} objs, {1} bytes; pause={2} ns",
                static_cast<unsigned long long>(result.objects_promoted),
                static_cast<unsigned long long>(result.bytes_promoted),
                static_cast<unsigned long long>(result.pause_ns));
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
            void* gen2_addr = g_old_gen.Allocate(obj_size, true);
            if (gen2_addr != nullptr) {
                std::memcpy(gen2_addr, drain_cur, obj_size);
                result.objects_promoted++;
                result.bytes_promoted += obj_size;
            } else {
                result.promotion_failed = true;
                CHAOS_IL2CPP_LOG_ERROR("CRAG", "gen1 drain: Gen2 OOM at object offset={0}",
                    static_cast<unsigned long long>(drain_cur - s_begin));
            }
            drain_cur += obj_size;
        }
        // Reset Gen1 even if some promotions failed — better to lose Gen1 space
        // than to leave dangling pointers. Mutators have already been told their
        // objects are in Gen2 (via forwarding or reference updates in the caller).
        g_young_gen.survivor_bump.store(g_young_gen.survivor_begin, std::memory_order_release);
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
                            if (child != nullptr && IsInGen1(child)) {
                                uintptr_t child_addr = reinterpret_cast<uintptr_t>(child);
                                CHAOS_IL2CPP_SIZE slot_idx = (child_addr - gen1_begin) / sizeof(void*);
                                mark_bm.MarkRange(slot_idx, 1);
                            }
                        }
                    } else {
                        // Fallback: scan all slots.
                        for (CHAOS_IL2CPP_SIZE off = 0; off + sizeof(void*) <= nobj_size; off += sizeof(void*)) {
                            void* child = *reinterpret_cast<void**>(n_cur + off);
                            if (child != nullptr && IsInGen1(child)) {
                                uintptr_t child_addr = reinterpret_cast<uintptr_t>(child);
                                CHAOS_IL2CPP_SIZE slot_idx = (child_addr - gen1_begin) / sizeof(void*);
                                mark_bm.MarkRange(slot_idx, 1);
                            }
                        }
                    }
                } else {
                    // No valid TypeInfo; conservative scan.
                    for (CHAOS_IL2CPP_SIZE off = 0; off + sizeof(void*) <= nobj_size; off += sizeof(void*)) {
                        void* child = *reinterpret_cast<void**>(n_cur + off);
                        if (child != nullptr && IsInGen1(child)) {
                            uintptr_t child_addr = reinterpret_cast<uintptr_t>(child);
                            CHAOS_IL2CPP_SIZE slot_idx = (child_addr - gen1_begin) / sizeof(void*);
                            mark_bm.MarkRange(slot_idx, 1);
                        }
                    }
                }
                n_cur += nobj_size;
            }
        }
    }

    // 3b: Scan Gen2 dirty cards for pointers into Gen1.
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("Gen1_Root_DirtyCards");
        g_old_gen.ScanDirtyCardsInPages(
            [&](uintptr_t /*card_idx*/, uintptr_t card_start, uintptr_t card_end) {
                for (uintptr_t slot = card_start; slot < card_end; slot += sizeof(void*)) {
                    void* val = *reinterpret_cast<void**>(slot);
                    if (val != nullptr && IsInGen1(val)) {
                        uintptr_t child_addr = reinterpret_cast<uintptr_t>(val);
                        CHAOS_IL2CPP_SIZE slot_idx = (child_addr - gen1_begin) / sizeof(void*);
                        mark_bm.MarkRange(slot_idx, 1);
                    }
                }
            });
    }

    // 3c: Scan thread stacks (conservative).
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("Gen1_Root_Stacks");
        struct StackCtx { uintptr_t gen1_begin; GcMarkBitmap* bm; };
        StackCtx sctx{gen1_begin, &mark_bm};
        threading::GcScanAllThreadRoots(
            [](void* root_addr, bool /*is_interior*/, void* user_data) {
                auto* ctx = static_cast<StackCtx*>(user_data);
                void* val = *reinterpret_cast<void**>(root_addr);
                if (val != nullptr && IsInGen1(val)) {
                    uintptr_t child_addr = reinterpret_cast<uintptr_t>(val);
                    CHAOS_IL2CPP_SIZE slot_idx = (child_addr - ctx->gen1_begin) / sizeof(void*);
                    ctx->bm->MarkRange(slot_idx, 1);
                }
            }, &sctx);
    }

    // 3d: Scan GCHandles for Gen1 pointers.
    {
        CHAOS_IL2CPP_PROFILE_SCOPE("Gen1_Root_GCHandles");
        struct HandleCtx { uintptr_t gen1_begin; GcMarkBitmap* bm; };
        HandleCtx hctx{gen1_begin, &mark_bm};
        GcIterateTenuredHandles(
            [](void* obj, void* user_data) {
                if (obj != nullptr && IsInGen1(obj)) {
                    auto* context = static_cast<HandleCtx*>(user_data);
                    uintptr_t obj_addr = reinterpret_cast<uintptr_t>(obj);
                    CHAOS_IL2CPP_SIZE slot_idx = (obj_addr - context->gen1_begin) / sizeof(void*);
                    context->bm->MarkRange(slot_idx, 1);
                }
            }, &hctx);
    }

    // ── Phase 4: Walk Gen1 objects, promote marked / reclaim unmarked ──
    // (objects_in_gen1 was already counted in Phase 1b counting pass)

    {
        CHAOS_IL2CPP_PROFILE_SCOPE("Gen1_PromoteAndSweep");
        char* s_cur = s_begin;
        auto& sv_layout_registry = GcLayoutRegistry::Instance();
        // Track in local vars; only commit to result if no failures.
        CHAOS_IL2CPP_SIZE local_promoted = 0;
        CHAOS_IL2CPP_SIZE local_bytes_promoted = 0;
        CHAOS_IL2CPP_SIZE local_reclaimed = 0;
        bool local_failed = false;

        // ── Phase 4a: Collect live objects with their sizes ──
        // Using a stack-allocated batch buffer for the common case.
        // If the batch overflows (>256 live objects), fall through to
        // the direct interleaved promote path.
        static constexpr int kPromoteBatchMax = 256;
        struct PromoteEntry { char* src; CHAOS_IL2CPP_SIZE size; };
        PromoteEntry batch_buf[kPromoteBatchMax];
        int batch_count = 0;
        bool batch_overflow = false;

        while (s_cur < s_bump) {
            const void* ti = *reinterpret_cast<const void* const*>(s_cur);
            CHAOS_IL2CPP_SIZE sv_size = kGen1MaxEstObjectSize;
            if (ti != nullptr && sv_layout_registry.IsValidTypeInfoPointer(ti)) {
                uint64_t sv_sid = sv_layout_registry.ReadStableId(ti);
                const auto* sv_layout = sv_layout_registry.Lookup(sv_sid);
                if (sv_layout != nullptr && sv_layout->instance_size > 0) {
                    sv_size = static_cast<CHAOS_IL2CPP_SIZE>(sv_layout->instance_size);
                }
            }

            uintptr_t obj_addr = reinterpret_cast<uintptr_t>(s_cur);
            CHAOS_IL2CPP_SIZE slot_idx = (obj_addr - gen1_begin) / sizeof(void*);
            bool is_live = mark_bm.TestSlot(slot_idx);

            if (is_live) {
                if (batch_count < kPromoteBatchMax) {
                    batch_buf[batch_count++] = {s_cur, sv_size};
                } else {
                    batch_overflow = true;
                    break;
                }
            } else {
                local_reclaimed += sv_size;
            }

            s_cur += sv_size;
        }

        // ── Phase 4b: Promote from batch (or fall through to direct) ──
        if (!batch_overflow) {
            // Tight promote loop from batch — Allocate calls for the same
            // size class hit the free list with better locality.
            for (int i = 0; i < batch_count; i++) {
                void* gen2_addr = g_old_gen.Allocate(batch_buf[i].size, true);
                if (gen2_addr != nullptr) {
                    std::memcpy(gen2_addr, batch_buf[i].src, batch_buf[i].size);
                    local_promoted++;
                    local_bytes_promoted += batch_buf[i].size;
                } else {
                    local_failed = true;
                    CHAOS_IL2CPP_LOG_ERROR("CRAG",
                        "gen1 Gen2 OOM promoting batched object");
                }
            }
        } else {
            // Batch overflow (>256 live objects): promote remaining objects
            // including the one that triggered the overflow in interleaved mode.
            CHAOS_IL2CPP_LOG_DEBUG("CRAG",
                "gen1_batch_overflow: promoting %d batched + remaining", kPromoteBatchMax);

            // Promote what we collected so far.
            for (int i = 0; i < kPromoteBatchMax; i++) {
                void* gen2_addr = g_old_gen.Allocate(batch_buf[i].size, true);
                if (gen2_addr != nullptr) {
                    std::memcpy(gen2_addr, batch_buf[i].src, batch_buf[i].size);
                    local_promoted++;
                    local_bytes_promoted += batch_buf[i].size;
                } else {
                    local_failed = true;
                    CHAOS_IL2CPP_LOG_ERROR("CRAG",
                        "gen1 Gen2 OOM promoting batched object (overflow)");
                }
            }

            // Then continue with interleaved promote for the rest.
            while (s_cur < s_bump) {
                const void* ti2 = *reinterpret_cast<const void* const*>(s_cur);
                CHAOS_IL2CPP_SIZE sv2 = kGen1MaxEstObjectSize;
                if (ti2 != nullptr && sv_layout_registry.IsValidTypeInfoPointer(ti2)) {
                    uint64_t sid2 = sv_layout_registry.ReadStableId(ti2);
                    const auto* l2 = sv_layout_registry.Lookup(sid2);
                    if (l2 != nullptr && l2->instance_size > 0) {
                        sv2 = static_cast<CHAOS_IL2CPP_SIZE>(l2->instance_size);
                    }
                }
                uintptr_t oa2 = reinterpret_cast<uintptr_t>(s_cur);
                CHAOS_IL2CPP_SIZE si2 = (oa2 - gen1_begin) / sizeof(void*);
                if (mark_bm.TestSlot(si2)) {
                    void* dst = g_old_gen.Allocate(sv2, true);
                    if (dst != nullptr) {
                        std::memcpy(dst, s_cur, sv2);
                        local_promoted++;
                        local_bytes_promoted += sv2;
                    } else {
                        local_failed = true;
                        CHAOS_IL2CPP_LOG_ERROR("CRAG",
                            "gen1 Gen2 OOM (overflow)");
                    }
                } else {
                    local_reclaimed += sv2;
                }
                s_cur += sv2;
            }
        }

        // Commit only if no failures.  If any Gen2 alloc failed, we leave Gen1
        // content intact (survivor_bump NOT reset) for the next cycle to retry.
        // Reporting partial promotions would create duplicates on the next retry.
        if (!local_failed) {
            result.objects_promoted = local_promoted;
            result.bytes_promoted = local_bytes_promoted;
            result.bytes_reclaimed = local_reclaimed;
            result.promotion_failed = false;
        } else {
            result.promotion_failed = true;
            result.bytes_reclaimed = 0;  // Nothing reclaimed — Gen1 preserved
        }
    }

    // ── Phase 5: Reset Gen1 (only if all promotions succeeded) ──
    // If Gen2 OOM occurred, preserve Gen1 content for retry on next cycle.
    if (!result.promotion_failed) {
        g_young_gen.survivor_bump.store(g_young_gen.survivor_begin, std::memory_order_release);
    } else {
        CHAOS_IL2CPP_LOG_WARN("CRAG", "gen1_collection promotion_failed — survivor_bump NOT reset");
    }

    // ── Phase 6: Cleanup ──
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
        "gen1_collection done: promoted={0} objs, {1} bytes; reclaimed={2} bytes; pause={3} ns",
        static_cast<unsigned long long>(result.objects_promoted),
        static_cast<unsigned long long>(result.bytes_promoted),
        static_cast<unsigned long long>(result.bytes_reclaimed),
        static_cast<unsigned long long>(result.pause_ns));

    return result;
}

}  // namespace chaos::il2cpp::runtime_core
