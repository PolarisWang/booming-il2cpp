#include "gc_layout.h"

#include "gc_young_collector.h"

#include <chaos/log.h>

#include <algorithm>
#include <cstdlib>

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// GcLayoutRegistry singleton
// ======================================================================

GcLayoutRegistry& GcLayoutRegistry::Instance() {
    static GcLayoutRegistry instance;
    return instance;
}

// ======================================================================
// Registration
// ======================================================================

void GcLayoutRegistry::Register(uint64_t stable_id, uint32_t instance_size,
                                const uint16_t* pointer_offsets,
                                uint16_t pointer_count) {
    if (stable_id == kEmptySlot) return;

    std::lock_guard<std::mutex> lock(register_mutex_);

    // Allocate and populate the layout.
    auto* layout = static_cast<GcTypeLayout*>(std::malloc(sizeof(GcTypeLayout)));
    if (layout == nullptr) {
        CHAOS_IL2CPP_LOG_ERROR_M("GcLayout", "OOM registering stable_id={0:x}", stable_id);
        return;
    }

    layout->instance_size = instance_size;
    layout->pointer_count = (std::min)(pointer_count,
        static_cast<uint16_t>(kGcLayoutMaxInlinePointers));
    layout->_reserved = 0;
    layout->_reserved2 = 0;

    // Copy pointer offsets (sorted by construction).
    for (uint16_t i = 0; i < layout->pointer_count; i++) {
        layout->pointer_offsets[i].offset = pointer_offsets[i];
    }
    // Zero remaining.
    for (uint16_t i = layout->pointer_count; i < kGcLayoutMaxInlinePointers; i++) {
        layout->pointer_offsets[i].offset = 0;
    }

    // Insert into hash table (linear probing).
    uint32_t idx = static_cast<uint32_t>(stable_id % kHashSize);
    for (int probe = 0; probe < kHashSize; probe++) {
        uint64_t key = hash_keys_[idx].load(std::memory_order_acquire);
        if (key == kEmptySlot) {
            // Empty slot — claim it.
            uint64_t expected = kEmptySlot;
            if (hash_keys_[idx].compare_exchange_strong(expected, stable_id,
                    std::memory_order_release, std::memory_order_acquire)) {
                hash_values_[idx] = layout;
                return;
            }
            // Race lost — slot was taken, try next.
        } else if (key == stable_id) {
            // Already registered — update.
            std::free(hash_values_[idx]);
            hash_values_[idx] = layout;
            return;
        }
        idx = (idx + 1) % kHashSize;
    }

    // Table full — leak the layout and log.
    CHAOS_IL2CPP_LOG_ERROR_M("GcLayout", "hash table full, dropping stable_id={0:x}", stable_id);
    std::free(layout);
}

const GcTypeLayout* GcLayoutRegistry::Lookup(uint64_t stable_id) const {
    if (stable_id == kEmptySlot) return nullptr;

    uint32_t idx = static_cast<uint32_t>(stable_id % kHashSize);
    for (int probe = 0; probe < kHashSize; probe++) {
        uint64_t key = hash_keys_[idx].load(std::memory_order_acquire);
        if (key == kEmptySlot) {
            // Not found (empty slot in probing chain).
            return nullptr;
        }
        if (key == stable_id) {
            return hash_values_[idx];
        }
        idx = (idx + 1) % kHashSize;
    }

    return nullptr;  // table full or not found
}

void GcLayoutRegistry::RegisterTypeInfoRange(uintptr_t range_begin,
                                              uintptr_t range_end) {
    if (range_begin >= range_end) return;

    // Use the lock-free fixed-size array.
    // Linear scan existing entries (max 64, one-time registration).
    for (int i = 0; i < typeinfo_range_count_.load(std::memory_order_acquire); i++) {
        auto& r = typeinfo_ranges_[i];
        if (range_begin <= r.end && range_end >= r.begin) {
            // Extend existing range (no need for atomic update since
            // registration is single-threaded at startup).
            if (range_begin < r.begin) r.begin = range_begin;
            if (range_end > r.end) r.end = range_end;
            return;
        }
    }

    // IMPORTANT: Write data into the slot BEFORE publishing via the count.
    // Since registration is single-threaded at startup, no concurrent
    // registration race exists.  However, GC workers may call
    // IsValidTypeInfoPointer concurrently — read the current count as
    // the insertion index, write data, then store the incremented count
    // with release ordering so that the data writes are visible.
    int idx = typeinfo_range_count_.load(std::memory_order_acquire);
    if (idx >= kMaxTypeInfoRanges) {
        CHAOS_IL2CPP_LOG_ERROR_M("GcLayout", "too many TypeInfo ranges ({0}), dropping", idx);
        return;
    }
    typeinfo_ranges_[idx].begin = range_begin;
    typeinfo_ranges_[idx].end = range_end;
    std::atomic_thread_fence(std::memory_order_release);
    typeinfo_range_count_.store(idx + 1, std::memory_order_release);
}

bool GcLayoutRegistry::IsValidTypeInfoPointer(const void* ptr) const {
    if (ptr == nullptr) return false;
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);

    int count = typeinfo_range_count_.load(std::memory_order_acquire);
    for (int i = 0; i < count; i++) {
        const auto& r = typeinfo_ranges_[i];
        if (addr >= r.begin && addr < r.end) {
            return true;
        }
    }
    return false;
}

void GcLayoutRegistry::RegisterRawAllocType(uint32_t instance_size) {
    // Register a pointer-free layout.
    uint64_t stable_id = RegisterOrGetRawAllocType(instance_size);
    (void)stable_id;
}

uint64_t GcLayoutRegistry::RegisterOrGetRawAllocType(uint32_t instance_size) {
    // Generate a deterministic stable_id for this size.
    // The formula ensures that same-size allocs share the same layout.
    uint64_t stable_id = kGcLayoutRawAllocStableId ^
        (static_cast<uint64_t>(instance_size) << 16);

    // Check if already registered.
    if (Lookup(stable_id) != nullptr) {
        return stable_id;
    }

    // Register pointer-free layout.
    Register(stable_id, instance_size, nullptr, 0);
    return stable_id;
}

// ======================================================================
// Scanner helpers
// ======================================================================

int ScanObjectPointers(void* obj, const GcTypeLayout* layout,
                       Region* nursery, YoungCollectionResult* result) {
    if (obj == nullptr || layout == nullptr) return 0;

    int found = 0;
    uintptr_t obj_base = reinterpret_cast<uintptr_t>(obj);

    for (uint16_t i = 0; i < layout->pointer_count; i++) {
        uint16_t offset = layout->pointer_offsets[i].offset;
        auto* slot = reinterpret_cast<void**>(obj_base + offset);
        void* val = *slot;

        if (val == nullptr) continue;

        if (IsInNursery(val)) {
            // This pointer references a nursery object — scavenge it.
            void* tenured = GcScavengeObject(val, result);
            if (tenured != nullptr && tenured != val) {
                *slot = tenured;  // Update the reference.
            }
            found++;
        }
    }

    return found;
}

// ======================================================================
// Cheney BFS — walk promoted objects using precise layouts
// ======================================================================

void CheneyBfsPrecise(void* tenured_begin, void* tenured_end,
                      Region* nursery, YoungCollectionResult* result) {
    if (tenured_begin == nullptr || tenured_end == nullptr) return;
    if (tenured_begin >= tenured_end) return;

    auto& registry = GcLayoutRegistry::Instance();

    // Walk the promoted region object-by-object.
    uintptr_t scan = reinterpret_cast<uintptr_t>(tenured_begin);
    uintptr_t end  = reinterpret_cast<uintptr_t>(tenured_end);

    while (scan < end) {
        auto* obj = reinterpret_cast<void*>(scan);

        // Read TypeInfo* from first word.
        const void* type_info_ptr = *static_cast<const void* const*>(obj);
        if (type_info_ptr == nullptr) {
            // End of valid objects (padding).
            break;
        }

        // Look up the GC layout.
        // For promoted objects, the TypeInfo* is the canonical TIB pointer.
        // We need to derive the stable_id from it.
        auto* hot = static_cast<const TypeInfoHot*>(type_info_ptr);
        uint64_t stable_id = hot->stable_id;

        const auto* layout = registry.Lookup(stable_id);
        if (layout == nullptr) {
            // No layout registered — fall back to instance_size from TypeInfo
            // (or skip if we can't determine size).
            // At C2/C3, this path is minimal since most types have layouts.
            CHAOS_IL2CPP_LOG_DEBUG("CRAG", "cheney_bfs_no_layout");
            break;
        }

        // Scan this object's pointer fields.
        ScanObjectPointers(obj, layout, nursery, result);

        // Advance to next object.
        scan += layout->instance_size;
    }
}

}  // namespace chaos::il2cpp::runtime_core