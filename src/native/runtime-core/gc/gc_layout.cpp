#include "gc_layout.h"

#include "gc_young_collector.h"

#include <chaos/log.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// GcLayoutRegistry singleton
// ======================================================================

GcLayoutRegistry::GcLayoutRegistry() {
    // Allocate initial table.
    auto* table = new GcLayoutTable(kGcLayoutMinCapacity);
    current_table_.store(table, std::memory_order_release);
}

GcLayoutRegistry& GcLayoutRegistry::Instance() {
    static GcLayoutRegistry instance;
    return instance;
}

// ======================================================================
// Registration
// ======================================================================

bool GcLayoutRegistry::InsertIntoTable(GcLayoutTable* table, uint64_t key, GcTypeLayout* value) {
    int idx = static_cast<int>(key % static_cast<uint64_t>(table->capacity));
    for (int probe = 0; probe < table->capacity; probe++) {
        uint64_t existing = table->keys[idx].load(std::memory_order_acquire);
        if (existing == kGcLayoutEmptySlot) {
            uint64_t expected = kGcLayoutEmptySlot;
            if (table->keys[idx].compare_exchange_strong(expected, key,
                    std::memory_order_release, std::memory_order_acquire)) {
                table->values[idx] = value;
                table->count++;
                return true;
            }
            // Race lost — retry.
        } else if (existing == key) {
            // Duplicate — update in-place.
            table->values[idx] = value;
            return true;
        }
        idx = (idx + 1) % table->capacity;
    }
    return false;  // table full
}

void GcLayoutRegistry::GrowTable() {
    auto* old_table = current_table_.load(std::memory_order_acquire);

    int new_capacity = old_table->capacity * 2;
    if (new_capacity > 1024 * 1024) {
        CHAOS_IL2CPP_LOG_WARN_M("GcLayout", "table capacity clamped at 1M slots");
        new_capacity = 1024 * 1024;
    }

    auto* new_table = new GcLayoutTable(new_capacity);

    // Rehash all existing entries.
    for (int i = 0; i < old_table->capacity; i++) {
        uint64_t key = old_table->keys[i].load(std::memory_order_acquire);
        if (key != kGcLayoutEmptySlot) {
            InsertIntoTable(new_table, key, old_table->values[i]);
            // GcTypeLayout* objects are re-linked — not freed.
        }
    }

    // Publish new table.
    current_table_.store(new_table, std::memory_order_release);

    // Retire old table (deferred free via ReclaimRetiredTables).
    RetireTable(old_table);
}

void GcLayoutRegistry::RetireTable(GcLayoutTable* table) {
    retired_tables_.push_back(table);
}

void GcLayoutRegistry::ReclaimRetiredTables() {
    // Must be called from a safepoint (no concurrent Lookup readers).
    size_t n = retired_tables_.size();
    for (auto* table : retired_tables_) {
        delete table;
    }
    retired_tables_.clear();

    if (n > 0) {
        CHAOS_IL2CPP_LOG_DEBUG_M("GcLayout", "reclaimed {0} retired tables",
            static_cast<unsigned long long>(n));
    }
}

void GcLayoutRegistry::Register(uint64_t stable_id, uint32_t instance_size,
                                const uint16_t* pointer_offsets,
                                uint16_t pointer_count) {
    if (stable_id == kGcLayoutEmptySlot) return;

    std::lock_guard<std::mutex> lock(register_mutex_);

    // Allocate and populate the layout.
    auto* layout = static_cast<GcTypeLayout*>(CHAOS_IL2CPP_MALLOC(sizeof(GcTypeLayout)));
    if (layout == nullptr) {
        CHAOS_IL2CPP_LOG_ERROR_M("GcLayout", "OOM registering stable_id={0:x}", stable_id);
        return;
    }

    layout->instance_size = instance_size;
    layout->pointer_count = (std::min)(pointer_count,
        static_cast<uint16_t>(kGcLayoutMaxInlinePointers));
    layout->_reserved = 0;
    layout->_reserved2 = 0;

    for (uint16_t i = 0; i < layout->pointer_count; i++) {
        layout->pointer_offsets[i].offset = pointer_offsets[i];
    }
    for (uint16_t i = layout->pointer_count; i < kGcLayoutMaxInlinePointers; i++) {
        layout->pointer_offsets[i].offset = 0;
    }

    // Insert into current table — grow if load factor exceeded.
    auto* table = current_table_.load(std::memory_order_acquire);
    if (!InsertIntoTable(table, stable_id, layout)) {
        // Table full — grow and retry.
        GrowTable();
        table = current_table_.load(std::memory_order_acquire);
        if (!InsertIntoTable(table, stable_id, layout)) {
            // Should not happen after growth, but handle gracefully.
            CHAOS_IL2CPP_LOG_ERROR_M("GcLayout", "insert failed after grow for stable_id={0:x}", stable_id);
            CHAOS_IL2CPP_FREE(layout);
            return;
        }
    }

    // Check load factor — grow proactively if >75%.
    if (static_cast<float>(table->count) / static_cast<float>(table->capacity) > kGcLayoutLoadFactorThreshold) {
        GrowTable();
    }
}

const GcTypeLayout* GcLayoutRegistry::Lookup(uint64_t stable_id) const {
    if (stable_id == kGcLayoutEmptySlot) return nullptr;

    auto* table = current_table_.load(std::memory_order_acquire);
    if (table == nullptr) return nullptr;

    int idx = static_cast<int>(stable_id % static_cast<uint64_t>(table->capacity));
    for (int probe = 0; probe < table->capacity; probe++) {
        uint64_t key = table->keys[idx].load(std::memory_order_acquire);
        if (key == kGcLayoutEmptySlot) {
            return nullptr;  // not found
        }
        if (key == stable_id) {
            return table->values[idx];
        }
        idx = (idx + 1) % table->capacity;
    }

    return nullptr;  // table full or not found
}

void GcLayoutRegistry::RegisterTypeInfoRange(uintptr_t range_begin,
                                              uintptr_t range_end) {
    if (range_begin >= range_end) return;

    for (int i = 0; i < typeinfo_range_count_.load(std::memory_order_acquire); i++) {
        auto& r = typeinfo_ranges_[i];
        if (range_begin <= r.end && range_end >= r.begin) {
            if (range_begin < r.begin) r.begin = range_begin;
            if (range_end > r.end) r.end = range_end;
            return;
        }
    }

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
    uint64_t stable_id = RegisterOrGetRawAllocType(instance_size);
    (void)stable_id;
}

uint64_t GcLayoutRegistry::RegisterOrGetRawAllocType(uint32_t instance_size) {
    uint64_t stable_id = kGcLayoutRawAllocStableId ^
        (static_cast<uint64_t>(instance_size) << 16);

    if (Lookup(stable_id) != nullptr) {
        return stable_id;
    }

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
            void* tenured = GcScavengeObjectKnownNursery(val, result);
            if (tenured != nullptr && tenured != val) {
                *slot = tenured;
            }
            found++;
        }
    }

    return found;
}
}  // namespace chaos::il2cpp::runtime_core