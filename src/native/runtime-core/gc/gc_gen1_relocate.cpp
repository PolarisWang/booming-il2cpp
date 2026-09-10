#include "gc_gen1.h"

#include <chaos/asan_interface.h>
#include <chaos/log.h>

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

#include "gc_events.h"
#include "gc_heap.h"      // G_OldGen(), GcSpinLockGuard
#include "gc_layout.h"
#include "gc_old_gen.h"
#include "gc_static_roots.h"
#include "gc_young_gen.h"
#include "thread_state.h"

namespace chaos::il2cpp::runtime_core {

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

void RelocateGen1References(
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

}  // namespace chaos::il2cpp::runtime_core
