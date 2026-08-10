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

/// Find the GcSafepointV0 covering @a offset within the method.  Entries are
/// sorted by native_offset (set during serialization); binary search for the
/// last safepoint at or before @a offset.  Returns nullptr if @a offset is
/// before the first safepoint (should not happen for a valid GC stop, which
/// can only occur at a recorded safepoint).
static const GcSafepointV0* FindSafepoint(const GcPointMapV0& pm, uint32_t offset) noexcept {
    if (pm.num_safepoints == 0) return nullptr;
    if (offset > pm.code_size) offset = pm.code_size;
    // The array is packed as GcSafepointV0 (variable-length slots[]), so walk
    // by advancing each entry's size rather than indexing directly.
    const uint8_t* base = pm.safepoints;
    const GcSafepointV0* best = nullptr;
    uint32_t best_off = 0;
    size_t cursor = 0;
    for (uint32_t i = 0; i < pm.num_safepoints; ++i) {
        const auto* sp = reinterpret_cast<const GcSafepointV0*>(base + cursor);
        size_t sz = sizeof(GcSafepointV0) +
                    (static_cast<size_t>(sp->num_gc_slots) + sp->num_live_regs) * sizeof(uint32_t);
        if (sp->native_offset <= offset && sp->native_offset >= best_off) {
            best = sp;
            best_off = sp->native_offset;
        }
        cursor += sz;
    }
    return best;
}

void GcScanPreciseSafepoint(
    const ManagedFrameInfo& frame,
    const GcPointMapV0& point_map,
    const void* code_start,
    GcRootCallback callback,
    void* user_data) {

    auto* frame_base = static_cast<uint8_t*>(frame.frame_ptr);
    const uint8_t* code = static_cast<const uint8_t*>(code_start);
    const uint8_t* ra = static_cast<const uint8_t*>(frame.return_address);
    uint32_t offset = (ra >= code) ? static_cast<uint32_t>(ra - code) : 0;

    const GcSafepointV0* sp = FindSafepoint(point_map, offset);
    if (sp == nullptr) return;

    // Precise stack slots live at this safepoint.
    const uint32_t* it = sp->slots;
    for (uint32_t i = 0; i < sp->num_gc_slots; ++i, ++it) {
        uint32_t encoded = *it;
        uint32_t so = encoded & CHAOS_GC_SLOT_OFFSET_MASK;
        bool is_interior = (encoded & CHAOS_GC_SLOT_KIND_MASK) == CHAOS_GC_SLOT_KIND_INTERIOR;
        void* slot_addr = frame_base + so;
        callback(slot_addr, is_interior, user_data);
    }
    // Volatile-register roots are handled in Task B (num_live_regs populated
    // there); the register window needs the CPU context, which the stack-slot
    // walker here does not carry.
    (void)it;
}

/// Decode and report the live volatile-register roots of a safepoint from an
/// explicit physical-register value file.  The safepoint's slots[] array is
/// laid out as [num_gc_slots stack encodings][num_live_regs register
/// encodings] (see GcPointMapV0 serializer).  Register encodings use
/// CHAOS_GC_REG_MASK bits for the physical x64 register index; the register's
/// value (an object pointer) is reported as the root.
void GcScanSafepointRegisterRoots(
    const GcSafepointV0& safepoint,
    const void* const* gpr_values,   // [num_gprs] physical GPR values
    uint32_t num_gprs,
    GcRootCallback callback,
    void* user_data) {

    const uint32_t* it = safepoint.slots + safepoint.num_gc_slots;
    for (uint32_t i = 0; i < safepoint.num_live_regs; ++i, ++it) {
        uint32_t encoded = *it;
        uint32_t phys = encoded & CHAOS_GC_REG_MASK;
        bool is_interior = (encoded & CHAOS_GC_SLOT_KIND_MASK) == CHAOS_GC_SLOT_KIND_INTERIOR;
        if (phys >= num_gprs) continue;
        const void* value = gpr_values[phys];
        if (value == nullptr) continue;
        callback(const_cast<void*>(value), is_interior, user_data);
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
