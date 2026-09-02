#include "gc_region.h"

#include <chaos/log.h>

#include <cstdint>

namespace chaos::il2cpp::runtime_core {

bool RegionManager::IsInDomain(CHAOS_IL2CPP_UINT32 domain_id, const void* ptr) const {
    if (ptr == nullptr) return false;
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);

    // Phase 1: lock-free range cache.  If the address is outside the
    // cached bounds for this domain, reject immediately without mutex.
    // Following the same pattern as IsNurseryPointer / IsPohPointer.
    {
        int count = domain_slot_count_.load(std::memory_order_acquire);
        for (int i = 0; i < count; i++) {
            uint32_t did = domain_range_slots_[i].domain_id.load(std::memory_order_acquire);
            if (did == domain_id) [[unlikely]] {
                uintptr_t b = domain_range_slots_[i].begin.load(std::memory_order_acquire);
                uintptr_t e = domain_range_slots_[i].end.load(std::memory_order_acquire);
                if (b >= e || addr < b || addr >= e) {
                    return false;  // Outside cached range.
                }
                // Within cached range — fall through to precise check.
                break;
            }
        }
    }

    // Phase 2: precise check under mutex.
    GcSpinLockGuard lock(mutex_);

    auto dit = domain_regions_.find(domain_id);
    if (dit == domain_regions_.end()) return false;

    for (int slot : dit->second) {
        const Region& r = region_table_[slot];
        if (r.id == kRegionIdInvalid) continue;
        uintptr_t r_begin = reinterpret_cast<uintptr_t>(r.begin);
        uintptr_t r_end   = reinterpret_cast<uintptr_t>(r.end);
        if (addr >= r_begin && addr < r_end) return true;
    }
    return false;
}

bool RegionManager::IsNurseryPointer(const void* ptr) const {
    if (ptr == nullptr) return false;
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);

    // Phase 1A: O(1) global bounds check.  If the pointer is outside the
    // conservative global nursery range, it cannot be in any nursery.
    // The global bounds monotonically expand on AddNurseryRange and never
    // shrink, so this is always a conservative (safe) filter.
    uintptr_t global_begin = nursery_global_begin_.load(std::memory_order_acquire);
    uintptr_t global_end   = nursery_global_end_.load(std::memory_order_acquire);
    if (global_begin < global_end) {
        if (addr < global_begin || addr >= global_end) {
            return false;  // Definitely not in any nursery.
        }
    }

    // Lock-free fast path: iterate the nursery range array with atomic loads.
    // No mutex needed — each slot is published with release ordering and never
    // modified after publication (removal zeros begin so the range is invalid).
    int count = nursery_slot_count_.load(std::memory_order_acquire);
    for (int i = 0; i < count; i++) {
        uintptr_t b = nursery_slots_[i].begin.load(std::memory_order_acquire);
        uintptr_t e = nursery_slots_[i].end.load(std::memory_order_acquire);
        if (b < e && addr >= b && addr < e) return true;
    }
    return false;
}

void RegionManager::AddNurseryRange(uintptr_t begin, uintptr_t end) {
    if (begin >= end) return;

    // Expand global nursery bounds (monotonic: only ever expands outward).
    // CAS loop ensures correctness under concurrent AddNurseryRange calls.
    {
        uintptr_t expected = nursery_global_begin_.load(std::memory_order_relaxed);
        while (begin < expected) {
            if (nursery_global_begin_.compare_exchange_weak(expected, begin,
                    std::memory_order_release, std::memory_order_relaxed)) {
                break;
            }
        }
    }
    {
        uintptr_t expected = nursery_global_end_.load(std::memory_order_relaxed);
        while (end > expected) {
            if (nursery_global_end_.compare_exchange_weak(expected, end,
                    std::memory_order_release, std::memory_order_relaxed)) {
                break;
            }
        }
    }

    // Try to reuse a previously-freed slot (begin == 0) first.
    int count = nursery_slot_count_.load(std::memory_order_acquire);
    for (int i = 0; i < count; i++) {
        uintptr_t b = nursery_slots_[i].begin.load(std::memory_order_acquire);
        if (b == 0) {
            nursery_slots_[i].begin.store(begin, std::memory_order_release);
            nursery_slots_[i].end.store(end, std::memory_order_release);
            return;
        }
    }

    // No reusable slot — extend the array.
    int idx = nursery_slot_count_.fetch_add(1, std::memory_order_acquire);
    if (idx >= kMaxNurserySlots) {
        CHAOS_IL2CPP_LOG_ERROR_M("CRAG", "nursery_slot_overflow idx={0}", idx);
        nursery_slot_count_.fetch_sub(1, std::memory_order_release);
        return;
    }
    nursery_slots_[idx].begin.store(begin, std::memory_order_release);
    nursery_slots_[idx].end.store(end, std::memory_order_release);
}

void RegionManager::RemoveNurseryRange(uintptr_t begin, uintptr_t end) {
    // Scan the array and zero out the slot that matches.
    // V4-H5: Store end=0 FIRST, then begin=0.  The reader in
    // IsNurseryPointer loads begin then end — if begin is 0 but
    // end is still > 0, the slot looks valid (0 < old_end = true)
    // producing a false positive.  By zeroing end first, the reader
    // sees either (old_begin, 0) → 0 < 0 = false (skipped correctly),
    // or (0, 0) → also false.
    int count = nursery_slot_count_.load(std::memory_order_acquire);
    for (int i = 0; i < count; i++) {
        uintptr_t b = nursery_slots_[i].begin.load(std::memory_order_acquire);
        uintptr_t e = nursery_slots_[i].end.load(std::memory_order_acquire);
        if (b == begin && e == end) {
            nursery_slots_[i].end.store(0, std::memory_order_release);
            std::atomic_thread_fence(std::memory_order_release);
            nursery_slots_[i].begin.store(0, std::memory_order_release);
            return;
        }
    }
}

// ======================================================================
// POH range tracking (lock-free, same design as nursery slots)
// ======================================================================

// (IsPohPointer is now inline in gc_region.h)

void RegionManager::AddPohRange(uintptr_t begin, uintptr_t end) {
    if (begin >= end) return;

    int count = poh_slot_count_.load(std::memory_order_acquire);
    for (int i = 0; i < count; i++) {
        uintptr_t b = poh_slots_[i].begin.load(std::memory_order_acquire);
        if (b == 0) {
            poh_slots_[i].begin.store(begin, std::memory_order_release);
            poh_slots_[i].end.store(end, std::memory_order_release);
            return;
        }
    }

    int idx = poh_slot_count_.fetch_add(1, std::memory_order_acquire);
    if (idx >= kMaxPohSlots) {
        CHAOS_IL2CPP_LOG_ERROR_M("CRAG", "poh_slot_overflow idx={0}", idx);
        poh_slot_count_.fetch_sub(1, std::memory_order_release);
        return;
    }
    poh_slots_[idx].begin.store(begin, std::memory_order_release);
    poh_slots_[idx].end.store(end, std::memory_order_release);
}

void RegionManager::RemovePohRange(uintptr_t begin, uintptr_t end) {
    int count = poh_slot_count_.load(std::memory_order_acquire);
    for (int i = 0; i < count; i++) {
        uintptr_t b = poh_slots_[i].begin.load(std::memory_order_acquire);
        uintptr_t e = poh_slots_[i].end.load(std::memory_order_acquire);
        if (b == begin && e == end) {
            poh_slots_[i].end.store(0, std::memory_order_release);
            std::atomic_thread_fence(std::memory_order_release);
            poh_slots_[i].begin.store(0, std::memory_order_release);
            return;
        }
    }
}

void RegionManager::AddDomainRange(uint32_t domain_id, uintptr_t begin, uintptr_t end) {
    if (begin >= end) return;

    // Try to reuse a freed slot (domain_id == 0) or update existing.
    int count = domain_slot_count_.load(std::memory_order_acquire);
    for (int i = 0; i < count; i++) {
        uint32_t did = domain_range_slots_[i].domain_id.load(std::memory_order_acquire);
        if (did == domain_id) {
            // Update existing entry — expand bounds outward.
            uintptr_t cur_begin = domain_range_slots_[i].begin.load(std::memory_order_relaxed);
            while (begin < cur_begin) {
                if (domain_range_slots_[i].begin.compare_exchange_weak(cur_begin, begin,
                        std::memory_order_release, std::memory_order_relaxed)) {
                    break;
                }
            }
            uintptr_t cur_end = domain_range_slots_[i].end.load(std::memory_order_relaxed);
            while (end > cur_end) {
                if (domain_range_slots_[i].end.compare_exchange_weak(cur_end, end,
                        std::memory_order_release, std::memory_order_relaxed)) {
                    break;
                }
            }
            return;
        }
        if (did == 0) {
            // Reusable slot.
            domain_range_slots_[i].domain_id.store(domain_id, std::memory_order_release);
            domain_range_slots_[i].begin.store(begin, std::memory_order_release);
            std::atomic_thread_fence(std::memory_order_release);
            domain_range_slots_[i].end.store(end, std::memory_order_release);
            return;
        }
    }

    // Extend the array.
    int idx = domain_slot_count_.fetch_add(1, std::memory_order_acquire);
    if (idx >= kMaxDomainSlots) {
        CHAOS_IL2CPP_LOG_ERROR_M("CRAG", "domain_range_slot_overflow idx={0}", idx);
        domain_slot_count_.fetch_sub(1, std::memory_order_release);
        return;
    }
    domain_range_slots_[idx].domain_id.store(domain_id, std::memory_order_release);
    domain_range_slots_[idx].begin.store(begin, std::memory_order_release);
    std::atomic_thread_fence(std::memory_order_release);
    domain_range_slots_[idx].end.store(end, std::memory_order_release);
}

void RegionManager::RemoveDomainRange(uint32_t domain_id) {
    int count = domain_slot_count_.load(std::memory_order_acquire);
    for (int i = 0; i < count; i++) {
        uint32_t did = domain_range_slots_[i].domain_id.load(std::memory_order_acquire);
        if (did == domain_id) {
            // Zero end first, then domain_id (reader checks domain_id first via load → begin then end).
            domain_range_slots_[i].end.store(0, std::memory_order_release);
            std::atomic_thread_fence(std::memory_order_release);
            domain_range_slots_[i].begin.store(0, std::memory_order_release);
            domain_range_slots_[i].domain_id.store(0, std::memory_order_release);
            return;
        }
    }
}

}  // namespace chaos::il2cpp::runtime_core
