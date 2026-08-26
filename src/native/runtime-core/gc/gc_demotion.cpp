#include "gc_demotion.h"

#include <chaos/asan_interface.h>
#include <chaos/log.h>

#include <algorithm>
#include <cstring>
#include <vector>

#include "gc_events.h"
#include "gc_card_table.h"   // DirtyCard (Phase 2.5 re-card)
#include "gc_young_gen.h"
#include "gc_gen1.h"
#include "gc_layout.h"
#include "gc_mark_bitmap.h"
#include "gc_old_gen.h"
#include "gc_root_scanner.h"
#include "gc_scheduler.h"
#include "thread_state.h"

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// ShouldDemote — decision helper
// ======================================================================

bool ShouldDemote(const OldGenPage* page, float frag_threshold) {
    if (page == nullptr || !page->in_use.load(std::memory_order_acquire)) {
        return false;
    }
    // Oversized pages (single object) are never demoted — they are handled
    // by oversized page reclamation in Collect() Phase 4.
    if (page->is_oversized) return false;

    // Compute fragmentation: 1.0 - (live_byte_estimate / payload_size).
    // Use the mark bitmap to count set bits.
    auto bm = GcMarkBitmap(const_cast<unsigned char*>(page->MarkBitmap()),
                           page->bitmap_bytes);
    CHAOS_IL2CPP_SIZE total_slots = page->payload_size / sizeof(void*);
    CHAOS_IL2CPP_SIZE live_slots = 0;
    for (CHAOS_IL2CPP_SIZE s = 0; s < total_slots; s++) {
        if (bm.TestSlot(s)) live_slots++;
    }
    float live_bytes = static_cast<float>(live_slots) * sizeof(void*);
    float frag = 1.0f - (live_bytes / static_cast<float>(page->payload_size));
    return frag >= frag_threshold;
}

// ======================================================================
// CollectDemotionCandidates
// ======================================================================

std::vector<DemotionEntry> CollectDemotionCandidates(
    MarkSweepOldGen& old_gen, CHAOS_IL2CPP_SIZE max_bytes) {

    std::vector<DemotionEntry> entries;
    CHAOS_IL2CPP_SIZE total_demoted = 0;

    // Lock the page list for a consistent snapshot.
    std::lock_guard<std::mutex> lock(old_gen.PageMutex());

    for (auto* page = old_gen.PageList(); page != nullptr; page = page->next) {
        if (total_demoted >= max_bytes) break;
        if (!ShouldDemote(page)) continue;

        auto bm = GcMarkBitmap(page->MarkBitmap(), page->bitmap_bytes);
        char* payload = page->Payload();
        CHAOS_IL2CPP_SIZE num_slots = page->payload_size / sizeof(void*);

        // Walk the mark bitmap to find live objects on this page.
        CHAOS_IL2CPP_SIZE s = 0;
        while (s < num_slots && total_demoted < max_bytes) {
            if (!bm.TestSlot(s)) {
                s++;
                continue;
            }

            // Found a marked slot — this is the start of a live object.
            // Use FindObjectContaining to determine the exact object bounds.
            void* obj_addr = payload + s * sizeof(void*);
            CHAOS_IL2CPP_SIZE obj_size = 0;

            // Read TypeInfo from the first word to determine instance size.
            const void* ti = *reinterpret_cast<const void* const*>(obj_addr);
            auto& layout_reg = GcLayoutRegistry::Instance();
            if (ti != nullptr && layout_reg.IsValidTypeInfoPointer(ti)) {
                uint64_t sid = layout_reg.ReadStableId(ti);
                const auto* layout = layout_reg.Lookup(sid);
                if (layout != nullptr && layout->instance_size > 0) {
                    obj_size = static_cast<CHAOS_IL2CPP_SIZE>(layout->instance_size);
                }
            }

            if (obj_size == 0) {
                // Fallback: scan forward to next marked bit or page end.
                CHAOS_IL2CPP_SIZE next_slot = s + 1;
                while (next_slot < num_slots && !bm.TestSlot(next_slot)) {
                    next_slot++;
                }
                obj_size = (next_slot - s) * sizeof(void*);
                if (obj_size == 0) { s++; continue; }
            }

            // Compute the slot range for this object.
            CHAOS_IL2CPP_SIZE obj_slots = (obj_size + sizeof(void*) - 1) / sizeof(void*);

            // IN-PLACE demotion (CoreCLR-aligned, GC-N6 #10): DO NOT move the
            // object to the gen1 region.  It stays resident in this old-gen page
            // at its original address, tracked in the page's demoted set so gen1
            // collection can scan it, full GC can root it, and classification
            // (GetRegionGen) reports it as gen1.  Its old-gen mark bit is KEPT SET
            // so old-gen sweep / BGC preserve it while it is gen1-owned.  Because
            // the address never changes, no external reference fix-up is needed —
            // eliminating the stale-reference class that crashed (probe mode2).
            // The old physically-moving demotion memcpy'd to TryAllocateInGen1 and
            // cleared the mark bit (bm.ClearRange), leaving a stale ref risk.
            if (!page->DemoteInPlace(static_cast<char*>(obj_addr), obj_size, /*must_promote=*/false)) {
                // Page's inline demoted array is full — skip this object (demotion
                // is best-effort; the object stays a normal gen2 object).
                s += obj_slots;
                continue;
            }
            entries.push_back({obj_addr, obj_addr, obj_size});
            total_demoted += obj_size;

            // Advance past this object.
            s += obj_slots;
        }
    }

    if (!entries.empty()) {
        CHAOS_IL2CPP_LOG_INFO_M("CRAG",
            "demotion: {0} objects, {1} bytes (in-place gen1-owned in old-gen pages)",
            static_cast<unsigned long>(entries.size()),
            static_cast<unsigned long long>(total_demoted));
    }

    return entries;
}

// ======================================================================
// DemotionRelocate — update all references after demotion
// ======================================================================

void DemotionRelocate(const std::vector<DemotionEntry>& entries,
                      MarkSweepOldGen& old_gen) {
    if (entries.empty()) return;

    // Build sorted old→new address map for binary search.
    struct AddrPair { uintptr_t old_addr; uintptr_t new_addr; };
    std::vector<AddrPair> addr_map;
    addr_map.reserve(entries.size());
    for (auto& e : entries) {
        addr_map.push_back({
            reinterpret_cast<uintptr_t>(e.old_addr),
            reinterpret_cast<uintptr_t>(e.new_addr)
        });
    }
    std::sort(addr_map.begin(), addr_map.end(),
        [](const AddrPair& a, const AddrPair& b) { return a.old_addr < b.old_addr; });

    // Phase 1: Walk all old-gen pages and update slot pointers.
    {
        std::lock_guard<std::mutex> lock(old_gen.PageMutex());
        for (auto* page = old_gen.PageList(); page != nullptr; page = page->next) {
            if (!page->in_use.load(std::memory_order_acquire)) continue;
            char* payload = page->Payload();
            CHAOS_IL2CPP_SIZE num_slots = page->payload_size / sizeof(void*);

            for (CHAOS_IL2CPP_SIZE s = 0; s < num_slots; s++) {
                auto* slot = reinterpret_cast<void**>(payload + s * sizeof(void*));
                void* val = *slot;
                if (val == nullptr) continue;
                uintptr_t val_addr = reinterpret_cast<uintptr_t>(val);

                auto it = std::lower_bound(addr_map.begin(), addr_map.end(), val_addr,
                    [](const AddrPair& p, uintptr_t addr) { return p.old_addr < addr; });
                if (it != addr_map.end() && it->old_addr == val_addr) {
                    *slot = reinterpret_cast<void*>(it->new_addr);
                }
            }
        }
    }

    // Phase 1.5: Scan the Gen1 region for demoted-object internal pointers.
    // Demoted objects were memcpy'd from Gen2 to Gen1, but any pointer fields
    // they contain still reference the old Gen2 addresses.  After sweep those
    // Gen2 addresses are reclaimed, creating dangling pointers.  Walk all
    // pointer-aligned slots in [gen1->begin, gen1_bump) and fix up any that
    // match a demoted old address.
    {
        auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
        if (gen1 != nullptr) {
            char* end = g_young_gen.gen1_bump.load(std::memory_order_acquire);
            if (end > gen1->begin) {
                for (char* cursor = gen1->begin; cursor < end;
                     cursor += sizeof(void*)) {
                    auto* slot = reinterpret_cast<void**>(cursor);
                    void* val = *slot;
                    if (val == nullptr) continue;
                    auto val_addr = reinterpret_cast<uintptr_t>(val);
                    auto it = std::lower_bound(addr_map.begin(), addr_map.end(),
                        val_addr,
                        [](const AddrPair& p, uintptr_t addr) {
                            return p.old_addr < addr; });
                    if (it != addr_map.end() && it->old_addr == val_addr) {
                        *slot = reinterpret_cast<void*>(it->new_addr);
                    }
                }
            }
        }
    }

    // Phase 2: Relocate thread-stack roots.
    threading::GcScanAllThreadRoots(
        [](void* root_addr, bool /*is_interior*/, void* user_data) {
            if (root_addr == nullptr) return;
            auto& map = *static_cast<std::vector<AddrPair>*>(user_data);
            // root_addr is a slot on ANOTHER thread's stack (conservative scan);
            // it may sit in an ASan stack-frame redzone.  Un-instrumented
            // read+write so relocation of foreign-stack roots doesn't trip a
            // false "unknown-crash" (access is byte-identical, ASan only elided).
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

    // Phase 2.5: Re-set the card table for each demoted object's NEW address.
    // GC-N6 finding (2026-08-25): demotion relocates old-gen objects — which
    // may hold interior cross-gen (old->nursery) references — into Gen1, but
    // the write barrier never carded the NEW Gen1 addresses.  The young GC's
    // Phase-2b Gen1 dirty-card scan therefore cannot see the demoted objects'
    // nursery references; the referenced nursery objects are collected while
    // still referenced (stale slots; exposed by the content-liveness check in
    // gc_region_barrier_stress_test).  Re-set the card for each new address so
    // Phase-2b rescans the demoted content (DirtyCard is lock-free).
    for (const auto& e : entries) {
        DirtyCard(e.new_addr);
    }

    // Phase 3: Relocate GCHandles.
    // Build a vector of {old, new} pairs in the format GcRelocateHandles expects.
    std::vector<std::pair<void*, void*>> handle_relocs;
    handle_relocs.reserve(entries.size());
    for (auto& e : entries) {
        handle_relocs.emplace_back(e.old_addr, e.new_addr);
    }
    GcRelocateHandles(handle_relocs);

    CHAOS_IL2CPP_LOG_DEBUG_M("CRAG", "demotion_relocate: {0} entries relocated",
        static_cast<unsigned long>(entries.size()));
}

}  // namespace chaos::il2cpp::runtime_core
