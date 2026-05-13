#include "gc_region.h"

#include <chaos/log.h>

#include "gc_layout.h"
#include "gc_old_gen.h"
#include "gc_scheduler.h"
#include "gc_stats.h"
#include "gc_young_collector.h"
#include "memory_domain.h"
#include "thread_state.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#else
    #include <sys/mman.h>
#endif

#include <cstdlib>
#include <cstring>

namespace chaos::il2cpp::runtime_core {

// ── TLS nursery context ────────────────────────────────────────
thread_local NurseryContext tls_nursery_ctx{nullptr, nullptr};

// Platform virtual memory helpers for region recycling.
#if defined(_WIN32) || defined(_WIN64)
static void VirtualFreeRegion(void* ptr, CHAOS_IL2CPP_SIZE size) {
    if (ptr) VirtualFree(ptr, 0, MEM_RELEASE);
}
#else
static void VirtualFreeRegion(void* ptr, CHAOS_IL2CPP_SIZE size) {
    if (ptr) munmap(ptr, size);
}
#endif

// ======================================================================
// NurseryAllocateSlow
// ======================================================================
void* NurseryAllocateSlow(CHAOS_IL2CPP_SIZE size) {
    // Oversized: bypass nursery, allocate via old gen.
    if (size > kMaxNurseryAlloc) {
        CHAOS_IL2CPP_LOG_DEBUG("CRAG", "nursery_oversized");
        void* ptr = g_old_gen.Allocate(size, true);
        // g_old_gen.Allocate already zeroes the memory via std::memset internally.
        if (ptr) GcRecordAlloc(size, true);
        return ptr;
    }

    // Ensure the nursery context is registered in ManagedThread for cross-thread
    // scanning by full GC.  Register on every call — if already set this is a
    // cheap pointer store to the same value.
    if (auto* mt = threading::GetCurrentThread()) {
        if (mt->nursery_ctx == nullptr) {
            mt->nursery_ctx = &tls_nursery_ctx;
        }
    }

    // Check scheduler: if allocation budget is exceeded, run a young GC
    // on the current nursery before allocating a new one.
    // Also run a proactive young GC when the nursery is physically full and
    // has live objects — this prevents discarding potentially live data when
    // the scheduler's EMA-based threshold hasn't been exceeded yet.
    bool do_young_gc = false;
    if (tls_nursery_ctx.nursery != nullptr) {
        if (g_gc_scheduler.DecideCollection() == GcCollectionKind::YOUNG) {
            do_young_gc = true;
        } else if (tls_nursery_ctx.nursery->current > tls_nursery_ctx.nursery->begin) {
            // Scheduler didn't trigger, but nursery has live objects and we're
            // about to discard it — run young GC proactively to promote survivors.
            do_young_gc = true;
        }
    }
    if (do_young_gc) {
        CHAOS_IL2CPP_LOG_DEBUG("CRAG", "young_gc_in_nursery_alloc_slow");
        uint32_t gen = threading::RequestGlobalSafepoint();
        GcYoungCollection(tls_nursery_ctx.nursery);
        threading::ReleaseGlobalSafepoint(gen);

        char* ptr = tls_nursery_ctx.nursery->current;
        char* next = ptr + size;
        if (next <= tls_nursery_ctx.nursery->end) {
            tls_nursery_ctx.nursery->current = next;
            std::memset(ptr, 0, size);
            g_gc_scheduler.RecordAllocation(size);
            return ptr;
        }
    }

    // Check if a full GC has been requested by the scheduler.
    if (g_gc_scheduler.IsFullGcRequested()) {
        CHAOS_IL2CPP_LOG_DEBUG("CRAG", "full_gc_in_nursery_alloc_slow");
        // Full GC with STW safepoint: Collect() calls GcScanAllThreadRoots
        // internally for thread stack scanning, which requires the generation
        // to be odd so that other threads stop in SafepointPoll.
        uint32_t gen = threading::RequestGlobalSafepoint();
        g_old_gen.Collect(nullptr, nullptr);
        threading::ReleaseGlobalSafepoint(gen);
        g_gc_scheduler.RecordFullCollection(0);

        // After full GC, try bump from the existing nursery again.
        if (tls_nursery_ctx.nursery != nullptr) {
            char* ptr = tls_nursery_ctx.nursery->current;
            char* next = ptr + size;
            if (next <= tls_nursery_ctx.nursery->end) {
                tls_nursery_ctx.nursery->current = next;
                std::memset(ptr, 0, size);
                return ptr;
            }
        }
    }

    // Current nursery exhausted (or GC didn't free enough) — get a fresh one
    // from the manager, using the scheduler's recommended size.
    // Free the old nursery first so the lock-free range array doesn't leak slots.
    if (tls_nursery_ctx.nursery != nullptr) {
        RegionManager::Instance().FreeRegion(tls_nursery_ctx.nursery->id);
    }

    CHAOS_IL2CPP_SIZE nursery_size = g_gc_scheduler.RecommendedNurserySize();
    Region* new_nursery = RegionManager::Instance().AllocateNurseryOfSize(nursery_size);
    if (new_nursery == nullptr) {
        // Fallback to old gen if no region available.
        CHAOS_IL2CPP_LOG_WARN("CRAG", "nursery_oom_fallback");
        void* ptr = g_old_gen.Allocate(size, true);
        if (ptr) std::memset(ptr, 0, size);
        return ptr;
    }

    tls_nursery_ctx.nursery = new_nursery;
    tls_nursery_ctx.limit = new_nursery->end - kMaxNurseryAlloc;

    // Register nursery context in ManagedThread for cross-thread scanning.
    if (auto* mt = threading::GetCurrentThread()) {
        mt->nursery_ctx = &tls_nursery_ctx;
    }

    // Retry bump from the fresh nursery.
    char* ptr = new_nursery->current;
    new_nursery->current = ptr + size;
    std::memset(ptr, 0, size);
    g_gc_scheduler.RecordAllocation(size);
    return ptr;
}

void* NurseryAllocateAtomicSlow(CHAOS_IL2CPP_SIZE size) {
    // Oversized: bypass nursery, allocate via old gen (no scanning needed).
    if (size > kMaxNurseryAlloc) {
        CHAOS_IL2CPP_LOG_DEBUG("CRAG", "nursery_oversized_atomic");
        void* ptr = g_old_gen.Allocate(size, false);
        if (ptr) std::memset(ptr, 0, size);
        return ptr;
    }

    // Ensure the nursery context is registered in ManagedThread.
    if (auto* mt = threading::GetCurrentThread()) {
        if (mt->nursery_ctx == nullptr) {
            mt->nursery_ctx = &tls_nursery_ctx;
        }
    }

    // Same as NurseryAllocateSlow but passes scanning_required=false for
    // old-gen fallback.  The nursery bump path is identical.
    bool do_young_gc = false;
    if (tls_nursery_ctx.nursery != nullptr) {
        if (g_gc_scheduler.DecideCollection() == GcCollectionKind::YOUNG) {
            do_young_gc = true;
        } else if (tls_nursery_ctx.nursery->current > tls_nursery_ctx.nursery->begin) {
            do_young_gc = true;
        }
    }
    if (do_young_gc) {
        CHAOS_IL2CPP_LOG_DEBUG("CRAG", "young_gc_in_nursery_alloc_slow");
        uint32_t gen = threading::RequestGlobalSafepoint();
        GcYoungCollection(tls_nursery_ctx.nursery);
        threading::ReleaseGlobalSafepoint(gen);

        char* ptr = tls_nursery_ctx.nursery->current;
        char* next = ptr + size;
        if (next <= tls_nursery_ctx.nursery->end) {
            tls_nursery_ctx.nursery->current = next;
            std::memset(ptr, 0, size);
            g_gc_scheduler.RecordAllocation(size);
            return ptr;
        }
    }

    // Check if a full GC has been requested by the scheduler.
    if (g_gc_scheduler.IsFullGcRequested()) {
        CHAOS_IL2CPP_LOG_DEBUG("CRAG", "full_gc_in_nursery_alloc_atomic_slow");
        uint32_t gen = threading::RequestGlobalSafepoint();
        g_old_gen.Collect(nullptr, nullptr);
        threading::ReleaseGlobalSafepoint(gen);
        g_gc_scheduler.RecordFullCollection(0);

        if (tls_nursery_ctx.nursery != nullptr) {
            char* ptr = tls_nursery_ctx.nursery->current;
            char* next = ptr + size;
            if (next <= tls_nursery_ctx.nursery->end) {
                tls_nursery_ctx.nursery->current = next;
                std::memset(ptr, 0, size);
                return ptr;
            }
        }
    }

    // Free the old nursery first to avoid leaking the region and its
    // lock-free range slot.  (Matching what NurseryAllocateSlow does.)
    if (tls_nursery_ctx.nursery != nullptr) {
        RegionManager::Instance().FreeRegion(tls_nursery_ctx.nursery->id);
    }

    CHAOS_IL2CPP_SIZE nursery_size = g_gc_scheduler.RecommendedNurserySize();
    Region* new_nursery = RegionManager::Instance().AllocateNurseryOfSize(nursery_size);
    if (new_nursery == nullptr) {
        CHAOS_IL2CPP_LOG_WARN("CRAG", "nursery_oom_fallback_atomic");
        void* ptr = g_old_gen.Allocate(size, false);
        if (ptr) std::memset(ptr, 0, size);
        return ptr;
    }

    tls_nursery_ctx.nursery = new_nursery;
    tls_nursery_ctx.limit = new_nursery->end - kMaxNurseryAlloc;

    // Register nursery context in ManagedThread for cross-thread scanning.
    if (auto* mt = threading::GetCurrentThread()) {
        mt->nursery_ctx = &tls_nursery_ctx;
    }

    char* ptr = new_nursery->current;
    new_nursery->current = ptr + size;
    std::memset(ptr, 0, size);
    g_gc_scheduler.RecordAllocation(size);
    return ptr;
}

void TeardownTlsNursery() {
    // Clear nursery context from ManagedThread.
    if (auto* mt = threading::GetCurrentThread()) {
        mt->nursery_ctx = nullptr;
    }

    if (tls_nursery_ctx.nursery != nullptr) {
        RegionManager::Instance().FreeRegion(tls_nursery_ctx.nursery->id);
        tls_nursery_ctx.nursery = nullptr;
        tls_nursery_ctx.limit = nullptr;
    }
}

// ======================================================================
// DomainAllocate
// ======================================================================
void* DomainAllocate(CHAOS_IL2CPP_UINT32 domain_id, CHAOS_IL2CPP_SIZE size) {
    size = (size + 7) & ~static_cast<CHAOS_IL2CPP_SIZE>(7);

    // For now, delegate to the existing domain heap path.
    // In later M-stones, this will route through RegionManager.
    auto* domain = chaos::il2cpp::memory_domain::FindDomainById(domain_id);
    if (domain && domain->heap) {
        return domain->heap->Allocate(size);
    }

    // Fallback: tagged malloc
    return CHAOS_IL2CPP_MALLOC(size);
}

// ======================================================================
// RegionManager implementation
// ======================================================================

Region* RegionManager::AllocateRegion(RegionKind kind, CHAOS_IL2CPP_SIZE min_size,
                                       CHAOS_IL2CPP_UINT32 domain_id) {
    std::lock_guard<std::mutex> lock(mutex_);

    CHAOS_IL2CPP_SIZE region_size = kDefaultRegionSize;
    switch (kind) {
    case RegionKind::REGION_NURSERY: region_size = kDefaultRegionSize; break;
    case RegionKind::REGION_TENURED: region_size = kTenuredRegionSize; break;
    case RegionKind::REGION_DOMAIN:  region_size = kDomainRegionSize;  break;
    default:                  region_size = kDefaultRegionSize; break;
    }
    if (min_size > region_size) region_size = min_size;

    // Check free-list first.
    if (free_list_ != nullptr) {
        Region* r = free_list_;
        free_list_ = r->next;
        r->kind = kind;
        r->domain_id = domain_id;
        r->current = r->begin;
        r->gc_state = {};
        r->next = nullptr;
        r->id = static_cast<RegionId>(region_count_ + 1);
        // Do NOT double-count: this region was already counted when first allocated.

        // If recycled nursery, re-publish to the lock-free range array.
        if (kind == RegionKind::REGION_NURSERY) {
            AddNurseryRange(reinterpret_cast<uintptr_t>(r->begin),
                            reinterpret_cast<uintptr_t>(r->end));
        }
        return r;
    }

    // Allocate region memory directly via VirtualAlloc so regions are
    // separate from the old gen's address space (avoids nursery/old-gen overlap).
    void* mem = nullptr;
    {
        CHAOS_IL2CPP_SIZE alloc_size = sizeof(Region) + region_size;
#if defined(_WIN32) || defined(_WIN64)
        mem = VirtualAlloc(nullptr, alloc_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#else
        mem = mmap(nullptr, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (mem == MAP_FAILED) mem = nullptr;
#endif
    }
    if (mem == nullptr) return nullptr;

    auto* r = static_cast<Region*>(mem);
    r->id = static_cast<RegionId>(region_count_ + 1);  // 0 = invalid
    r->kind = kind;
    r->domain_id = domain_id;
    // Ensure 8-byte alignment for bump-pointer allocation.
    uintptr_t raw_begin = reinterpret_cast<uintptr_t>(mem) + sizeof(Region);
    raw_begin = (raw_begin + 7) & ~static_cast<uintptr_t>(7);
    r->begin = reinterpret_cast<char*>(raw_begin);
    r->end = r->begin + region_size;
    r->current = r->begin;
    r->gc_state = {};
    r->next = nullptr;

    // Add to region table.
    int slot = AllocSlot();
    if (slot < 0) {
        g_old_gen.Free(mem);
        return nullptr;
    }
    region_table_[slot] = *r;

    // If this is a nursery region, publish to the lock-free nursery range
    // array so IsNurseryPointer can check without taking the mutex.
    if (kind == RegionKind::REGION_NURSERY) {
        AddNurseryRange(reinterpret_cast<uintptr_t>(r->begin),
                        reinterpret_cast<uintptr_t>(r->end));
    }

    total_allocated_bytes_.fetch_add(region_size, std::memory_order_relaxed);
    return r;
}

static constexpr int kFreeListTrimThreshold = 16;

void RegionManager::FreeRegion(RegionId id) {
    if (id == kRegionIdInvalid) return;
    std::lock_guard<std::mutex> lock(mutex_);

    for (CHAOS_IL2CPP_UINT32 i = 0; i < region_count_; i++) {
        if (region_table_[i].id == id) {
            Region* r = &region_table_[i];

            // If this is a nursery region, remove its range from the
            // lock-free array so IsNurseryPointer stops finding it.
            if (r->kind == RegionKind::REGION_NURSERY) {
                RemoveNurseryRange(reinterpret_cast<uintptr_t>(r->begin),
                                   reinterpret_cast<uintptr_t>(r->end));
            }

            // Recycle into free list.
            r->next = free_list_;
            free_list_ = r;
            r->id = kRegionIdInvalid;
            free_list_size_++;

            // Trim free list when it grows too large to bound memory usage.
            if (free_list_size_ > kFreeListTrimThreshold) {
                // Walk to the last entry and free it.
                Region** pp = &free_list_;
                int count = 0;
                while ((*pp)->next != nullptr) {
                    pp = &(*pp)->next;
                    count++;
                }
                // Free the tail entry.
                Region* tail = *pp;
                *pp = nullptr;
                CHAOS_IL2CPP_SIZE region_size = static_cast<CHAOS_IL2CPP_SIZE>(tail->end - tail->begin)
                    + sizeof(Region);
                VirtualFreeRegion(tail, region_size);
                free_list_size_--;
            }
            return;
        }
    }
}

void RegionManager::ReleaseDomainRegions(CHAOS_IL2CPP_UINT32 domain_id) {
    std::lock_guard<std::mutex> lock(mutex_);

    CHAOS_IL2CPP_LOG_INFO_M("CRAG", "release_domain id={0}", domain_id);
    for (CHAOS_IL2CPP_UINT32 i = 0; i < region_count_; i++) {
        Region* r = &region_table_[i];
        if (r->domain_id == domain_id && r->id != kRegionIdInvalid) {
            r->next = free_list_;
            free_list_ = r;
            r->id = kRegionIdInvalid;
        }
    }
}

Region* RegionManager::AllocateNursery() {
    return AllocateRegion(RegionKind::REGION_NURSERY, kDefaultRegionSize);
}

Region* RegionManager::AllocateNurseryOfSize(CHAOS_IL2CPP_SIZE size) {
    return AllocateRegion(RegionKind::REGION_NURSERY, size);
}

CHAOS_IL2CPP_SIZE RegionManager::PromoteNursery(Region* nursery) {
    // Stub: in C2/C3, this scans the nursery, copies live objects to
    // a tenured region, and returns the total promoted bytes.
    // For M0, this is a no-op.
    (void)nursery;
    return 0;
}

void RegionManager::DumpRegions() const {
    CHAOS_IL2CPP_UINT32 active = 0;
    for (CHAOS_IL2CPP_UINT32 i = 0; i < region_count_; i++) {
        const Region& r = region_table_[i];
        if (r.id == kRegionIdInvalid) continue;
        active++;
        std::fprintf(stderr,
            "CRAG|region id=%u kind=%u domain=%u [%p-%p) cur=%p size=%llu\n",
            r.id, static_cast<unsigned>(r.kind), r.domain_id,
            (void*)r.begin, (void*)r.end, (void*)r.current,
            static_cast<unsigned long long>(r.end - r.begin));
    }
    std::fprintf(stderr, "CRAG|total regions=%u active=%u allocated=%llu\n",
        region_count_, active,
        static_cast<unsigned long long>(
            total_allocated_bytes_.load(std::memory_order_relaxed)));
}

int RegionManager::AllocSlot() {
    if (region_count_ >= region_capacity_) {
        CHAOS_IL2CPP_SIZE new_cap = region_capacity_ == 0 ? 64 : region_capacity_ * 2;
        auto* new_table = static_cast<Region*>(
            std::realloc(region_table_, new_cap * sizeof(Region)));
        if (new_table == nullptr) return -1;
        region_table_ = new_table;
        region_capacity_ = static_cast<CHAOS_IL2CPP_UINT32>(new_cap);
    }
    int idx = static_cast<int>(region_count_);
    region_count_++;
    return idx;
}

bool RegionManager::IsInDomain(CHAOS_IL2CPP_UINT32 domain_id, const void* ptr) const {
    if (ptr == nullptr) return false;
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    std::lock_guard<std::mutex> lock(mutex_);
    for (CHAOS_IL2CPP_UINT32 i = 0; i < region_count_; i++) {
        const Region& r = region_table_[i];
        if (r.id == kRegionIdInvalid) continue;
        if (r.domain_id != domain_id) continue;
        uintptr_t r_begin = reinterpret_cast<uintptr_t>(r.begin);
        uintptr_t r_end   = reinterpret_cast<uintptr_t>(r.end);
        if (addr >= r_begin && addr < r_end) return true;
    }
    return false;
}

bool RegionManager::IsNurseryPointer(const void* ptr) const {
    if (ptr == nullptr) return false;
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);

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
    int count = nursery_slot_count_.load(std::memory_order_acquire);
    for (int i = 0; i < count; i++) {
        uintptr_t b = nursery_slots_[i].begin.load(std::memory_order_acquire);
        uintptr_t e = nursery_slots_[i].end.load(std::memory_order_acquire);
        if (b == begin && e == end) {
            nursery_slots_[i].begin.store(0, std::memory_order_release);
            nursery_slots_[i].end.store(0, std::memory_order_release);
            return;
        }
    }
}

// 🔔 P2-3: RegionAllocate removed (dead code — callers use NurseryAllocate,
// g_old_gen.Allocate, or DomainAllocate directly).

}  // namespace chaos::il2cpp::runtime_core