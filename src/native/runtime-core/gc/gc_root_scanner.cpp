#include "gc_root_scanner.h"

#include <chaos/log.h>

#include <cstdint>
#include <cstring>
#include <mutex>
#include <unordered_map>

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// GcSlotMap registry — maps code address → GcSlotMapV0
//
// Uses a simple sorted-array approach (populated at module registration).
// Lookup is binary search.  For the M0/C1 effort this is adequate; a
// fancier trie or paging structure can be added later if the registry
// grows beyond ~100k entries.
// ======================================================================

namespace {

struct SlotMapEntry {
    const void*     code_start;   // Method start address (PC range begin)
    const GcSlotMapV0* slot_map;  // Associated GcSlotMap
};

// Lock for thread-safe registration during module load.
// Lookup is read-only after startup and does not need the lock.
std::mutex s_registry_mutex;

// Flat array of registered mappings.  Sorted by code_start for binary search.
// Grows only during module registration (not on the GC path).
SlotMapEntry* s_entries = nullptr;
uint32_t      s_entry_count = 0;
uint32_t      s_entry_capacity = 0;

}  // anonymous namespace

void GcRegisterSlotMap(const void* code_address, const GcSlotMapV0* slot_map) {
    if (code_address == nullptr || slot_map == nullptr) return;

    std::lock_guard<std::mutex> lock(s_registry_mutex);

    // Grow the array if needed.
    if (s_entry_count >= s_entry_capacity) {
        uint32_t new_cap = s_entry_capacity == 0 ? 256 : s_entry_capacity * 2;
        auto* new_entries = static_cast<SlotMapEntry*>(
            std::realloc(s_entries, new_cap * sizeof(SlotMapEntry)));
        if (new_entries == nullptr) {
            CHAOS_IL2CPP_LOG_ERROR("CRAG", "OOM growing SlotMap registry");
            return;
        }
        s_entries = new_entries;
        s_entry_capacity = new_cap;
    }

    // Insert sorted by code_address.
    // This is O(n) per insert; total O(n^2) at startup.  Acceptable for
    // tens of thousands of methods (<< 1s).  Can switch to batch-sort later.
    uint32_t pos = 0;
    while (pos < s_entry_count && s_entries[pos].code_start < code_address) {
        pos++;
    }

    // Shift existing entries right.
    if (pos < s_entry_count) {
        std::memmove(&s_entries[pos + 1], &s_entries[pos],
                     (s_entry_count - pos) * sizeof(SlotMapEntry));
    }

    s_entries[pos] = { code_address, slot_map };
    s_entry_count++;
}

const GcSlotMapV0* GcLookupSlotMap(const void* code_address) {
    if (code_address == nullptr || s_entry_count == 0) return nullptr;

    // Binary search for the matching entry.
    int lo = 0;
    int hi = static_cast<int>(s_entry_count) - 1;

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        const void* addr = s_entries[mid].code_start;

        if (addr == code_address) {
            return s_entries[mid].slot_map;
        }
        if (addr < code_address) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }

    return nullptr;
}

void GcRegisterSlotMapsFromSection(const GcSlotMapV0* begin, const GcSlotMapV0* end) {
    // The section contains flat GcSlotMapV0 instances packed sequentially.
    // Each instance is followed by its slot data.  We use a heuristic:
    // each method's slot map occupies sizeof(GcSlotMapV0) + num_gc_slots * 4 bytes.
    // The section cannot be iterated without per-entry size metadata, so this
    // function requires the caller (codegen) to register each method individually
    // via GcRegisterSlotMap().
    (void)begin;
    (void)end;
    // This is a placeholder for future batch-registration optimization.
}

// ======================================================================
// Root scanning
// ======================================================================

void GcScanPreciseFrame(
    const ManagedFrameInfo& frame,
    const GcSlotMapV0& slot_map,
    GcRootCallback callback,
    void* user_data) {

    auto* frame_base = static_cast<uint8_t*>(frame.frame_ptr);

    for (uint32_t i = 0; i < slot_map.num_gc_slots; i++) {
        uint32_t encoded = slot_map.slots[i];
        uint32_t offset = encoded & CHAOS_GC_SLOT_OFFSET_MASK;
        bool is_interior = (encoded & CHAOS_GC_SLOT_KIND_MASK) == CHAOS_GC_SLOT_KIND_INTERIOR;

        if (offset >= slot_map.frame_size) {
            CHAOS_IL2CPP_LOG_WARN("CRAG", "slot_offset_out_of_range");
            continue;
        }

        void* slot_addr = frame_base + offset;
        callback(slot_addr, is_interior, user_data);
    }
}

void GcScanConservativeFrame(
    const ManagedFrameInfo& frame,
    GcConservativeRootCallback callback,
    void* user_data) {

    auto* frame_base = static_cast<uint8_t*>(frame.frame_ptr);
    uint32_t num_slots = frame.frame_size / sizeof(void*);

    for (uint32_t i = 0; i < num_slots; i++) {
        auto* slot = reinterpret_cast<void**>(frame_base + i * sizeof(void*));
        void* candidate = *slot;

        // Only report non-null, reasonably aligned candidates.
        // BDWGC's own conservative scanner applies more sophisticated
        // heuristics; this is a minimal filter.
        uintptr_t val = reinterpret_cast<uintptr_t>(candidate);
        if (val != 0 && (val & (sizeof(void*) - 1)) == 0) {
            callback(candidate, user_data);
        }
    }
}

void GcScanFrameHybrid(
    const ManagedFrameInfo& frame,
    GcRootCallback precise_callback,
    GcConservativeRootCallback conservative_callback,
    void* user_data) {

    const GcSlotMapV0* slot_map = GcLookupSlotMap(frame.return_address);

    if (slot_map != nullptr) {
        // Precise scan for known frames.
        GcScanPreciseFrame(frame, *slot_map, precise_callback, user_data);
    } else {
        // Conservative fallback for unknown frames.
        CHAOS_IL2CPP_LOG_DEBUG("CRAG", "conservative_scan_fallback");
        GcScanConservativeFrame(frame, conservative_callback, user_data);
    }
}

}  // namespace chaos::il2cpp::runtime_core