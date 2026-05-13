#include "domain_unloader.h"

#include "gc_region.h"
#include "gc_card_table.h"
#include "gc_young_collector.h"
#include "memory_domain.h"
#include "thread_state.h"

#include <chaos/log.h>

#include <cstdint>
#include <cstring>

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// Cross-domain reference scan
// ======================================================================

/// Scan the domain's regions for cross-domain references (domain→core).
/// At C4, these are cleared (set to null) since the domain is being unloaded.
static CHAOS_IL2CPP_SIZE ScanAndClearCrossDomainRefs(CHAOS_IL2CPP_UINT32 domain_id,
                                                        CHAOS_IL2CPP_SIZE* refs_found_out) {
    CHAOS_IL2CPP_SIZE refs_found = 0;
    CHAOS_IL2CPP_SIZE refs_cleared = 0;

    // Walk all domain regions for this domain_id.
    RegionManager& mgr = RegionManager::Instance();

    // Use the region manager to iterate regions.  We can't directly access
    // the region table from here, so we use FreeRegion scan as a proxy.
    // For C4, we scan card table entries for domain-id ownership.
    //
    // Implementation: for each dirty card that might belong to this domain,
    // scan pointer-aligned slots.  If a slot points outside the domain's
    // regions, it's a cross-domain reference (shouldn't exist in normal
    // operation).  Clear it.

    // At C4, we perform a card table sweep over the entire heap:
    // Look for cards that are dirty and belong to domain regions.
    // For each such card, scan slots.  Any slot pointing to a non-domain
    // address is a cross-domain ref.  Clear it.

    CHAOS_IL2CPP_LOG_INFO_M("CRAG", "scan_cross_domain id={0}", domain_id);

    // Walk the entire card table.  Dirty cards in domain regions are
    // candidates.
    uintptr_t heap_base = g_heap_base;
    if (heap_base == 0) return 0;

    // For each possible card, check if it's dirty and belongs to a
    // domain region.  At C4 this is simplified: we make a single pass
    // over the card table.  A production implementation would use the
    // region table for precise domain→card mapping.
    for (uintptr_t card_idx = 0; card_idx < kCardTableEntries; card_idx++) {
        if (g_card_table[card_idx] == 0) continue;  // clean card

        uintptr_t card_start = heap_base + (card_idx << kCardShift);
        uintptr_t card_end = card_start + kCardSize;

        // Scan pointer-aligned slots in this card.
        for (uintptr_t slot = card_start; slot < card_end; slot += sizeof(void*)) {
            auto* ptr_slot = reinterpret_cast<void**>(slot);
            void* val = *ptr_slot;

            // Check if the slot value points WITHIN the domain being unloaded.
            // If the target is inside this domain's regions, it's an intra-domain
            // reference (normal, should NOT be cleared — only cross-domain refs
            // become dangling after the domain is released).
            // If the target is outside this domain, it IS a cross-domain reference
            // and should be cleared (nulled) to prevent dangling pointers from
            // core into the released domain memory.
            if (val == nullptr) continue;

            // Check domain region table: is the value within the domain being unloaded?
            if (mgr.IsInDomain(domain_id, val)) {
                // The slot points INTO the domain that is being unloaded.
                // This is a core→domain dangling pointer — clear it to
                // prevent use-after-free after the domain memory is released.
                *ptr_slot = nullptr;
                refs_cleared++;
                refs_found++;
                continue;
            }

            // The slot points OUTSIDE the domain (core memory or another domain).
            // Safe to keep — not affected by this domain's unload.
        }
    }

    CHAOS_IL2CPP_LOG_INFO_M("CRAG", "cross_domain_scan id={0} found={1} cleared={2}",
        domain_id, refs_found, refs_cleared);
    if (refs_found_out) *refs_found_out = refs_found;
    return refs_cleared;
}

// ======================================================================
// UnloadDomain
// ======================================================================

DomainUnloadResult UnloadDomain(CHAOS_IL2CPP_UINT32 domain_id) {
    CHAOS_IL2CPP_LOG_INFO_M("CRAG", "unload_domain id={0}", domain_id);

    DomainUnloadResult result = {};
    result.success = false;

    if (domain_id == 0) {
        CHAOS_IL2CPP_LOG_WARN_M("CRAG", "unload_domain_invalid_id");
        return result;
    }

    // Phase 1: STW safepoint.
    uint32_t gen = threading::RequestGlobalSafepoint();
    CHAOS_IL2CPP_LOG_DEBUG_M("CRAG", "unload_safepoint gen={0}", gen);

    // Phase 2: Scan and clear cross-domain references.
    CHAOS_IL2CPP_SIZE refs_found = 0;
    result.cross_domain_refs_cleared = ScanAndClearCrossDomainRefs(domain_id, &refs_found);
    result.cross_domain_refs_found = refs_found;

    // Phase 3: Release all regions owned by this domain.
    RegionManager::Instance().ReleaseDomainRegions(domain_id);

    // Phase 4: Unregister the memory domain.
    bool md_unregistered = chaos::il2cpp::memory_domain::UnregisterMemoryDomain(domain_id);
    if (!md_unregistered) {
        CHAOS_IL2CPP_LOG_WARN_M("CRAG", "unload_unregister_md_failed id={0}", domain_id);
    }

    // Phase 5: Resume threads.
    threading::ReleaseGlobalSafepoint(gen);

    result.success = true;
    CHAOS_IL2CPP_LOG_INFO_M("CRAG", "unload_domain_done id={0}", domain_id);
    return result;
}

}  // namespace chaos::il2cpp::runtime_core