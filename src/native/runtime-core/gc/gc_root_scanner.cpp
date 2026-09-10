#include "gc_root_scanner.h"

#include <chaos/log.h>
#include <chaos/unordered_dense.h>

#include "gc_card_table.h"

#include <cstdint>
#include <cstring>
#include <atomic>
#include <mutex>

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// GcSlotMap registry — maps code address → GcSlotMapV0
//
// Uses unordered_dense_map (swisstable) for O(1) insertion and lookup,
// replacing the previous sorted-array approach that had O(n) insertion
// via linear scan + memmove (O(n²) total at startup for large registries).
//
// Key is the method start address (pointer → identity hash, no wyhash).
// ======================================================================

namespace {

// Key: method code address.  Value: GcSlotMapV0 pointer.
// Identity hash (no wyhash) since the key is already a uniformly
// distributed pointer value from module layout.
using SlotMap = CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(
    const void*, const GcSlotMapV0*);

// Lock for thread-safe registration during module load.
std::mutex s_registry_mutex;

SlotMap s_slot_map;

}  // anonymous namespace

void GcRegisterSlotMap(const void* code_address, const GcSlotMapV0* slot_map) {
    if (code_address == nullptr || slot_map == nullptr) return;

    std::lock_guard<std::mutex> lock(s_registry_mutex);
    s_slot_map[code_address] = slot_map;
}

void GcUnregisterSlotMap(const void* code_address) noexcept {
    if (code_address == nullptr) return;
    std::lock_guard<std::mutex> lock(s_registry_mutex);
    s_slot_map.erase(code_address);
}

const GcSlotMapV0* GcLookupSlotMap(const void* code_address) {
    if (code_address == nullptr) return nullptr;

    // Lock-free read: the map is only mutated during module registration
    // (before GC starts), not during concurrent GC cycles.
    auto it = s_slot_map.find(code_address);
    return (it != s_slot_map.end()) ? it->second : nullptr;
}

void GcRegisterSlotMapsFromSection(const void* begin, const void* end) {
    // The .gc_slot_maps section contains entries laid out as:
    //   [entry_total_size:4][code_address:8][frame_size:4][num_gc_slots:4][slots:N*4]
    // The section is generated with #pragma pack(1) so fields are at byte
    // offsets with no padding — code_address is at offset 4, not offset 8.
    auto* ptr = static_cast<const uint8_t*>(begin);
    auto* end_ptr = static_cast<const uint8_t*>(end);

    while (ptr + static_cast<ptrdiff_t>(sizeof(uint32_t)) <= end_ptr) {
        uint32_t entry_total;
        std::memcpy(&entry_total, ptr, sizeof(entry_total));
        if (entry_total < 12u) {  // minimum: entry_total(4) + code_address(8)
            CHAOS_IL2CPP_LOG_WARN_M("CRAG", "slot_map_section_entry_too_small: %u", entry_total);
            break;
        }

        // Read code_address from ptr+4 (packed layout, no pointer alignment).
        const void* code_address;
        std::memcpy(&code_address, ptr + 4, sizeof(code_address));

        // GcSlotMapV0 body starts at ptr+12 (after entry_total + code_address).
        // Safe to reinterpret since GcSlotMapV0 has only uint32_t fields
        // (frame_size at +12, num_gc_slots at +16, slots at +20).
        auto* sm = reinterpret_cast<const GcSlotMapV0*>(ptr + 12);

        // Sanity-check: slot map must fit within the declared entry size.
        uint32_t expected_min = 12u + 8u  // packed header
            + sm->num_gc_slots * static_cast<uint32_t>(sizeof(uint32_t));
        if (entry_total < expected_min) {
            CHAOS_IL2CPP_LOG_WARN_M("CRAG",
                "slot_map_section_entry_size_mismatch: expected>=%u, got=%u",
                expected_min, entry_total);
            break;
        }

        if (code_address != nullptr) {
            GcRegisterSlotMap(code_address, sm);
        }

        // Advance by entry_total (includes the header).
        ptr += entry_total;
    }
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

        // Only report candidates that are non-null, pointer-aligned, and
        // within the managed heap range.  Values below g_heap_base are
        // definitely not managed object pointers (e.g., small integers,
        // string literals, code addresses, OS handles).
        uintptr_t val = reinterpret_cast<uintptr_t>(candidate);
        if (val != 0 && (val & (sizeof(void*) - 1)) == 0 &&
            val >= g_heap_base) {
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

// ── Interpreter frame scanner callback ───────────────────────────

namespace {
    std::atomic<GcScanInterpFramesFn> s_interp_frame_scanner{nullptr};
}

void GcSetInterpFrameScanner(GcScanInterpFramesFn fn) noexcept {
    s_interp_frame_scanner.store(fn, std::memory_order_release);
}

GcScanInterpFramesFn GcGetInterpFrameScanner() noexcept {
    return s_interp_frame_scanner.load(std::memory_order_acquire);
}

}  // namespace chaos::il2cpp::runtime_core
