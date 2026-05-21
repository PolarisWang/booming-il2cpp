#include "gc_region.h"

#include <chaos/log.h>

#include <cstdio>

#include "gc_bgc.h"
#include "gc_events.h"
#include "gc_gen1.h"
#include "gc_layout.h"
#include "gc_old_gen.h"
#include "gc_loh.h"
#include "gc_scheduler.h"
#include "gc_stats.h"
#include "gc_stress.h"
#include "gc_api.h"
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

// ── Shared young generation + TLAB ──────────────────────────────
YoungGeneration g_young_gen;
thread_local TLAB tls_tlab;

// Per-thread allocation counter (TLS-local, flushed to scheduler in slow path).
thread_local CHAOS_IL2CPP_SIZE tls_alloc_since_last_gc = 0;

// ── TLS POH context ────────────────────────────────────────────
// Each thread has a fast bump-pointer path for POH allocation.
// POH regions are shared across threads (mutex-protected for allocation).
// For now, PohAllocate uses the process-wide POH context to avoid
// per-thread POH region proliferation (pinned objects are typically few).
static Region* s_poh_current = nullptr;  // current POH bump region
static std::mutex s_poh_mutex;

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
    CHAOS_IL2CPP_PROFILE_SCOPE("NurseryAllocateSlow");

    // GC Stress mode: force a full GC every kStressInterval allocations.
    if (GcStressShouldTrigger()) [[unlikely]] {
        tls_in_gc_stress = true;
        chaos_gc_collect();
        tls_in_gc_stress = false;
    }

    // Flush TLS allocation counter to scheduler before making any GC decision.
    FlushTlsAllocCounter();

    // Phase 1: Try to claim a new TLAB from the shared young generation.
    TLAB tlab = TlabClaimFromYoungGen();
    if (tlab.current != nullptr) {
        tls_tlab = tlab;
        return NurseryAllocate(size);
    }

    // Phase 2: Young region is exhausted — trigger a young GC.
    // When in NO_GC_REGION, skip young GC and go straight to old-gen fallback.
    if (!GcIsInNoGcRegion() && g_gc_scheduler.TryClaimGcSlot()) {
        uint32_t gen = threading::RequestGlobalSafepoint();
        auto* mt = threading::GetCurrentThread();
        if (mt) {
            mt->tlab_start = tls_tlab.start;
            mt->tlab_current = tls_tlab.current;
        }
        GcYoungCollection();
        if (mt) {
            mt->tlab_start = nullptr;
            mt->tlab_current = nullptr;
        }
        threading::ReleaseGlobalSafepoint(gen);
        GcAdvanceBgcCycle();
        tls_tlab = TLAB{};
    }

    // Phase 3: Retry from the fresh young region + new TLAB.
    tlab = TlabClaimFromYoungGen();
    if (tlab.current != nullptr) {
        tls_tlab = tlab;
        return NurseryAllocate(size);
    }

    // Phase 4: Young region full — fall back to old gen.
    // Switch to preemptive mode so old-gen allocation does not block
    // safepoint handshake (the old-gen allocator takes mutex_ internally,
    // which would deadlock against ScanDirtyCardsInPages).
    threading::EnterPreemptiveMode();
    void* old_result = g_old_gen.Allocate(size, true);
    threading::EnterCooperativeMode();  // SafepointPoll re-sync if pending
    return old_result;
}

void* NurseryAllocateAtomicSlow(CHAOS_IL2CPP_SIZE size) {
    CHAOS_IL2CPP_PROFILE_SCOPE("NurseryAllocateAtomicSlow");

    // GC Stress mode: force a full GC.
    if (GcStressShouldTrigger()) [[unlikely]] {
        tls_in_gc_stress = true;
        chaos_gc_collect();
        tls_in_gc_stress = false;
    }

    FlushTlsAllocCounter();

    // Phase 1: Try to claim a new TLAB.
    TLAB tlab = TlabClaimFromYoungGen();
    if (tlab.current != nullptr) {
        tls_tlab = tlab;
        return NurseryAllocateAtomic(size);
    }

    // Phase 2: Young GC (scanning_required = false for pointer-free).
    // When in NO_GC_REGION, skip young GC and go straight to fallback.
    if (!GcIsInNoGcRegion() && g_gc_scheduler.TryClaimGcSlot()) {
        uint32_t gen = threading::RequestGlobalSafepoint();
        auto* mt = threading::GetCurrentThread();
        if (mt) {
            mt->tlab_start = tls_tlab.start;
            mt->tlab_current = tls_tlab.current;
        }
        GcYoungCollection();
        if (mt) {
            mt->tlab_start = nullptr;
            mt->tlab_current = nullptr;
        }
        threading::ReleaseGlobalSafepoint(gen);
        GcAdvanceBgcCycle();
        tls_tlab = TLAB{};
    }

    // Phase 3: Retry.
    tlab = TlabClaimFromYoungGen();
    if (tlab.current != nullptr) {
        tls_tlab = tlab;
        return NurseryAllocateAtomic(size);
    }

    // Phase 4: Fall back to old gen (no scanning needed).
    threading::EnterPreemptiveMode();
    void* old_result = g_old_gen.Allocate(size, false);
    threading::EnterCooperativeMode();
    return old_result;
}

// TeardownTlsNursery is now an inline no-op in gc_region.h


// ======================================================================
// DomainAllocate
// ======================================================================
void* DomainAllocate(CHAOS_IL2CPP_UINT32 domain_id, CHAOS_IL2CPP_SIZE size) {
    size = (size + 7) & ~static_cast<CHAOS_IL2CPP_SIZE>(7);

    // For now, delegate to the existing domain heap path.
    // In later M-stones, this will route through RegionManager.
    auto* domain = chaos::il2cpp::memory_domain::FindDomainById(domain_id);
    if (domain && domain->heap) {
        void* ptr = domain->heap->Allocate(size);
        if (ptr) GcRecordAlloc(size, false);
        return ptr;
    }

    // Fallback: tagged malloc
    void* ptr = CHAOS_IL2CPP_MALLOC(size);
    if (ptr) GcRecordAlloc(size, false);
    return ptr;
}

// ======================================================================
// PohAllocate — bump-pointer in POH region
// ======================================================================
void* PohAllocate(CHAOS_IL2CPP_SIZE size) noexcept {
    if (size == 0) return nullptr;
    size = (size + 7) & ~static_cast<CHAOS_IL2CPP_SIZE>(7);
    if (size > kPohRegionSize - sizeof(Region)) {
        // Single object larger than a POH region — allocate oversized via old gen.
        CHAOS_IL2CPP_LOG_DEBUG("CRAG", "poh_oversized");
        return g_old_gen.Allocate(size, true);
    }

    std::lock_guard<std::mutex> lock(s_poh_mutex);

    // Try bump from current POH region.
    if (s_poh_current != nullptr) {
        char* ptr = s_poh_current->current;
        char* next = ptr + size;
        if (next <= s_poh_current->end) {
            s_poh_current->current = next;
            std::memset(ptr, 0, size);
            g_gc_scheduler.RecordAllocation(size);
            return ptr;
        }
    }

    // Current POH region exhausted — allocate a new one.
    Region* new_poh = RegionManager::Instance().AllocateRegion(
        RegionKind::REGION_POH, kPohRegionSize);
    if (new_poh == nullptr) {
        CHAOS_IL2CPP_LOG_WARN("CRAG", "poh_oom_fallback");
        return g_old_gen.Allocate(size, true);
    }

    s_poh_current = new_poh;
    char* ptr = new_poh->current;
    new_poh->current = ptr + size;
    std::memset(ptr, 0, size);
    g_gc_scheduler.RecordAllocation(size);
    return ptr;
}

bool IsPohPointer(const void* ptr) noexcept {
    if (ptr == nullptr) return false;
    return RegionManager::Instance().IsPohPointer(ptr);
}

void TeardownTlsPoh() noexcept {
    // POH regions are shared across threads (process-wide), not per-thread.
    // Thread detach does not release POH regions.
}
// ======================================================================
// Shared young generation lifecycle
// ======================================================================

void InitYoungGeneration() noexcept {
    auto* region = RegionManager::Instance().AllocateRegion(
        RegionKind::REGION_NURSERY, kDefaultYoungRegionSize);
    auto* region_begin = region ? region->begin : nullptr;
    g_young_gen.region.store(region, std::memory_order_release);
    if (region) {
        // Split the nursery region into young half (8 MB) and survivor half (8 MB).
        CHAOS_IL2CPP_SIZE half_size = kDefaultYoungRegionSize / 2;
        char* mid = region_begin + half_size;

        region->end = mid;  // young half ends at mid
        g_young_gen.region = region;
        g_young_gen.bump.store(region_begin, std::memory_order_release);
        g_young_gen.region_end.store(mid, std::memory_order_release);

        // Survivor area occupies the second half.
        g_young_gen.survivor_begin = mid;
        g_young_gen.survivor_end = mid + half_size;
        g_young_gen.survivor_bump.store(mid, std::memory_order_release);

        // Register nursery+survivor range with the card table so that
        // DirtyCard() write barrier covers writes to these regions.
        // Without this registration, the card table has no L2 segments
        // for the nursery/survivor address range and all writes are
        // silently dropped, creating a correctness hole for BGC
        // concurrent mark (Gen1→Gen2 references established during
        // concurrent mark are never recorded in dirty cards).
        GcRegisterHeapRange(
            reinterpret_cast<uintptr_t>(region_begin),
            reinterpret_cast<uintptr_t>(region_begin + kDefaultYoungRegionSize));

        // Set nursery range for DirtyCard fast skip (young GC Phase 2
        // scans nursery precisely, so card writes are unnecessary).
        GcSetCardTableNurseryRange(
            reinterpret_cast<uintptr_t>(region_begin),
            reinterpret_cast<uintptr_t>(mid));
    }
}

void DestroyYoungGeneration() noexcept {
    auto* region = g_young_gen.region.exchange(nullptr, std::memory_order_acq_rel);
    if (region) {
        g_young_gen.survivor_begin = nullptr;
        g_young_gen.survivor_end = nullptr;
        g_young_gen.survivor_bump.store(nullptr, std::memory_order_release);
        RegionManager::Instance().FreeRegion(region->id);
    }
    g_young_gen.bump.store(nullptr, std::memory_order_release);
    g_young_gen.region_end.store(nullptr, std::memory_order_release);
}

TLAB TlabClaimFromYoungGen() noexcept {
    // Atomically carve kDefaultTlabSize from the shared young region.
    char* bump = g_young_gen.bump.load(std::memory_order_acquire);
    while (bump != nullptr) {
        char* next_bump = bump + kDefaultTlabSize;
        const char* region_end = g_young_gen.region_end.load(std::memory_order_acquire);
        if (next_bump > region_end || region_end == nullptr) {
            return TLAB{};  // exhausted
        }
        if (g_young_gen.bump.compare_exchange_weak(bump, next_bump,
                std::memory_order_acq_rel, std::memory_order_acquire)) {
            TLAB tlab;
            tlab.start = bump;
            tlab.current = bump;
            tlab.end = next_bump;
            tlab.start_scan = bump;
            tlab.current_scan = bump;
            return tlab;
        }
        // CAS failed — retry with fresh bump value.
    }
    return TLAB{};
}



Region* RegionManager::AllocateRegion(RegionKind kind, CHAOS_IL2CPP_SIZE min_size,
                                       CHAOS_IL2CPP_UINT32 domain_id) {
    std::lock_guard<std::mutex> lock(mutex_);

    CHAOS_IL2CPP_SIZE region_size = kDefaultRegionSize;
    switch (kind) {
    case RegionKind::REGION_NURSERY: region_size = kDefaultRegionSize; break;
    case RegionKind::REGION_TENURED: region_size = kTenuredRegionSize; break;
    case RegionKind::REGION_DOMAIN:  region_size = kDomainRegionSize;  break;
    case RegionKind::REGION_POH:     region_size = kPohRegionSize;     break;
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
        r->table_slot = -1;  // recycled region: not in region_table_, managed via free-list
        // Do NOT double-count: this region was already counted when first allocated.

        // If recycled nursery, re-publish to the lock-free range array.
        if (kind == RegionKind::REGION_NURSERY) {
            AddNurseryRange(reinterpret_cast<uintptr_t>(r->begin),
                            reinterpret_cast<uintptr_t>(r->end));
        }
        if (kind == RegionKind::REGION_DOMAIN) {
            AddDomainRange(domain_id,
                reinterpret_cast<uintptr_t>(r->begin),
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
    r->table_slot = slot;

    // Register in O(1) indexes.
    region_index_[r->id] = slot;
    domain_regions_[domain_id].push_back(slot);

    // If this is a nursery region, publish to the lock-free nursery range
    // array so IsNurseryPointer can check without taking the mutex.
    if (kind == RegionKind::REGION_NURSERY) {
        AddNurseryRange(reinterpret_cast<uintptr_t>(r->begin),
                        reinterpret_cast<uintptr_t>(r->end));
    }

    // If this is a POH region, publish to the POH range array.
    if (kind == RegionKind::REGION_POH) {
        AddPohRange(reinterpret_cast<uintptr_t>(r->begin),
                     reinterpret_cast<uintptr_t>(r->end));
    }

    // If this is a DOMAIN region, publish to the lock-free domain range cache.
    if (kind == RegionKind::REGION_DOMAIN) {
        AddDomainRange(domain_id,
            reinterpret_cast<uintptr_t>(r->begin),
            reinterpret_cast<uintptr_t>(r->end));
    }

    total_allocated_bytes_.fetch_add(region_size, std::memory_order_relaxed);
    return r;
}

static constexpr int kFreeListTrimThreshold = 16;

void RegionManager::FreeRegion(RegionId id) {
    if (id == kRegionIdInvalid) return;
    std::lock_guard<std::mutex> lock(mutex_);

    // O(1) lookup via region_index_.
    auto it = region_index_.find(id);
    if (it == region_index_.end()) return;
    int slot = it->second;
    region_index_.erase(it);

    Region* r = &region_table_[slot];

    // If this is a nursery region, remove its range from the
    // lock-free array so IsNurseryPointer stops finding it.
    if (r->kind == RegionKind::REGION_NURSERY) {
        RemoveNurseryRange(reinterpret_cast<uintptr_t>(r->begin),
                           reinterpret_cast<uintptr_t>(r->end));
    }

    // If this is a POH region, remove its range.
    if (r->kind == RegionKind::REGION_POH) {
        RemovePohRange(reinterpret_cast<uintptr_t>(r->begin),
                       reinterpret_cast<uintptr_t>(r->end));
    }

    // If this is a DOMAIN region, remove its range from the lock-free cache.
    if (r->kind == RegionKind::REGION_DOMAIN) {
        RemoveDomainRange(r->domain_id);
    }

    // Remove from domain index.
    auto dit = domain_regions_.find(r->domain_id);
    if (dit != domain_regions_.end()) {
        auto& vec = dit->second;
        for (size_t i = 0; i < vec.size(); ) {
            if (vec[i] == slot) {
                vec[i] = vec.back();
                vec.pop_back();
                break;
            } else {
                ++i;
            }
        }
        if (vec.empty()) {
            domain_regions_.erase(dit);
        }
    }

    // Recycle into free list.
    r->next = free_list_;
    free_list_ = r;
    r->id = kRegionIdInvalid;
    r->table_slot = -1;
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
}

void RegionManager::ReleaseDomainRegions(CHAOS_IL2CPP_UINT32 domain_id) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto dit = domain_regions_.find(domain_id);
    if (dit == domain_regions_.end()) return;

    CHAOS_IL2CPP_LOG_INFO_M("CRAG", "release_domain id={0} regions={1}",
        domain_id, static_cast<unsigned>(dit->second.size()));

    for (int slot : dit->second) {
        Region* r = &region_table_[slot];
        if (r->id == kRegionIdInvalid) continue;

        // Remove from region_index_.
        region_index_.erase(r->id);

        // If this is a nursery-kind region, remove its range from the
        // lock-free nursery lookup table so IsInNursery doesn't return
        // stale true for the freed region's address range.
        if (r->kind == RegionKind::REGION_NURSERY) {
            RemoveNurseryRange(reinterpret_cast<uintptr_t>(r->begin),
                               reinterpret_cast<uintptr_t>(r->end));
        }

        // If this is a POH-kind region, remove its range.
        if (r->kind == RegionKind::REGION_POH) {
            RemovePohRange(reinterpret_cast<uintptr_t>(r->begin),
                           reinterpret_cast<uintptr_t>(r->end));
        }

        // If this is a DOMAIN-kind region, remove its range.
        if (r->kind == RegionKind::REGION_DOMAIN) {
            RemoveDomainRange(r->domain_id);
        }

        // Recycle into free list.
        r->next = free_list_;
        free_list_ = r;
        r->id = kRegionIdInvalid;
        r->table_slot = -1;
    }
    domain_regions_.erase(dit);
}

Region* RegionManager::AllocateNursery() {
    return AllocateRegion(RegionKind::REGION_NURSERY, kDefaultRegionSize);
}

Region* RegionManager::AllocateNurseryOfSize(CHAOS_IL2CPP_SIZE size) {
    return AllocateRegion(RegionKind::REGION_NURSERY, size);
}

CHAOS_IL2CPP_SIZE RegionManager::PromoteNursery(Region* nursery) {
    if (nursery == nullptr) return 0;

    // Delegate to the young collector which performs precise GcLayout scanning
    // of the nursery, copies live objects to old-gen via GcScavengeObject, and
    // sets forwarding pointers for transitive closure via Cheney BFS.
    //
    // The return value is the total bytes promoted from this nursery.
    YoungCollectionResult result = GcYoungCollection();

    if (result.objects_promoted > 0) {
        CHAOS_IL2CPP_LOG_DEBUG_M("CRAG", "promote_nursery objects={0} bytes={1}",
            static_cast<unsigned long long>(result.objects_promoted),
            static_cast<unsigned long long>(result.bytes_promoted));
    }

    return result.bytes_promoted;
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
    std::lock_guard<std::mutex> lock(mutex_);

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

bool RegionManager::IsPohPointer(const void* ptr) const {
    if (ptr == nullptr) return false;
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);

    int count = poh_slot_count_.load(std::memory_order_acquire);
    for (int i = 0; i < count; i++) {
        uintptr_t b = poh_slots_[i].begin.load(std::memory_order_acquire);
        uintptr_t e = poh_slots_[i].end.load(std::memory_order_acquire);
        if (b < e && addr >= b && addr < e) return true;
    }
    return false;
}

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

Region* RegionManager::GetFirstPohRegion() const {
    std::lock_guard<std::mutex> lock(mutex_);
    for (CHAOS_IL2CPP_UINT32 i = 0; i < region_count_; i++) {
        const Region& r = region_table_[i];
        if (r.id != kRegionIdInvalid && r.kind == RegionKind::REGION_POH) {
            // We need a mutable pointer.  Since we're under mutex_ and
            // this is the only writer, const_cast is safe for internal use.
            return const_cast<Region*>(&region_table_[i]);
        }
    }
    return nullptr;
}

int RegionManager::GetPohRegionCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    int count = 0;
    for (CHAOS_IL2CPP_UINT32 i = 0; i < region_count_; i++) {
        if (region_table_[i].id != kRegionIdInvalid &&
            region_table_[i].kind == RegionKind::REGION_POH) {
            count++;
        }
    }
    return count;
}

Region* RegionManager::GetNextPohRegion(const Region* current) const {
    std::lock_guard<std::mutex> lock(mutex_);
    for (CHAOS_IL2CPP_UINT32 i = current->table_slot + 1; i < region_count_; i++) {
        const Region& r = region_table_[i];
        if (r.id != kRegionIdInvalid && r.kind == RegionKind::REGION_POH) {
            return const_cast<Region*>(&region_table_[i]);
        }
    }
    return nullptr;
}

// 🔔 P2-3: RegionAllocate removed (dead code — callers use NurseryAllocate,
// g_old_gen.Allocate, or DomainAllocate directly).

// ======================================================================
// Extern "C" API — called from managed code via codegen inline body
// ======================================================================

/// Trigger a full blocking GC collection from managed code (System.GC.Collect()).
/// 1. Runs young collection on the calling thread's nursery (if any).
/// 2. Runs full old-gen mark-sweep (STW safepoint).
/// 3. Runs pending finalizers.
extern "C" void chaos_gc_collect() noexcept {
    CHAOS_IL2CPP_LOG_DEBUG("CRAG", "chaos_gc_collect requested");

    // Step 1: Young collection on the shared young generation (if any).
    // Uses g_young_gen.bump to determine if there are live nursery objects,
    // not region->current (which is frozen at begin after each young GC reset).
    Region* young_region = g_young_gen.region.load(std::memory_order_acquire);
    void* bump = g_young_gen.bump.load(std::memory_order_acquire);
    if (young_region != nullptr && bump > young_region->begin) {
        uint32_t gen = threading::RequestGlobalSafepoint();
        GcYoungCollection();
        threading::ReleaseGlobalSafepoint(gen);
    }

    // Step 1.5: Gen1 collection (if Gen1/survivor has objects).
    // Collects the survivor area and promotes live objects to Gen2.
    if (g_young_gen.survivor_begin != nullptr) {
        char* s_bump = g_young_gen.survivor_bump.load(std::memory_order_acquire);
        if (s_bump > g_young_gen.survivor_begin) {
            uint32_t gen = threading::RequestGlobalSafepoint();
            auto gen1_result = GcGen1Collection();
            threading::ReleaseGlobalSafepoint(gen);
            GcRecordGen1Collection(
                gen1_result.objects_promoted,
                gen1_result.bytes_promoted,
                gen1_result.bytes_reclaimed,
                gen1_result.pause_ns);
            g_gc_scheduler.RecordGen1Collection(
                gen1_result.bytes_promoted,
                gen1_result.objects_in_gen1,
                gen1_result.pause_ns);
        }
    }

    // Step 2: Full old-gen collection (mark-sweep, potentially parallel).
    {
        uint32_t gen = threading::RequestGlobalSafepoint();
        g_old_gen.Collect(nullptr, nullptr);
        threading::ReleaseGlobalSafepoint(gen);
    }

    // Step 3: Run pending finalizers.
    g_old_gen.RunFinalizers();

    CHAOS_IL2CPP_LOG_DEBUG("CRAG", "chaos_gc_collect completed");
}

/// Wait for pending finalizers (System.GC.WaitForPendingFinalizers()).
extern "C" void chaos_gc_wait_for_pending_finalizers() noexcept {
    CHAOS_IL2CPP_LOG_DEBUG("CRAG", "chaos_gc_wait_for_pending_finalizers");
    g_old_gen.RunFinalizers();
    // Also drain any BGC finalizer thread batches.
    BgcController::Instance().WaitForFinalizerDrain();
}

/// GC.KeepAlive — prevents the GC from collecting the object before
/// this call (extern "C" for codegen direct call).  In CRAG, no-op,
/// but the call site in generated code provides the observable
/// side-effect boundary that keeps the argument alive.
extern "C" void chaos_gc_keepalive(CHAOS_IL2CPP_INTPTR obj) noexcept {
    (void)obj;
}

/// Allocate memory in the Pinned Object Heap (POH).
/// POH objects never participate in young GC copying and are suitable
/// for callers that know the allocation will be pinned (e.g., fixed
/// buffers, P/Invoke buffers, GCHandleType.Pinned targets).
/// ABI export: callable from managed/NativeAOT code.
extern "C" void* chaos_gc_allocate_pinned(CHAOS_IL2CPP_SIZE size) noexcept {
    CHAOS_IL2CPP_LOG_DEBUG("CRAG", "chaos_gc_allocate_pinned size={0}",
        static_cast<unsigned long long>(size));
    return GcAllocatePinned(size);
}

// chaos_gc_get_total_memory, chaos_gc_add_memory_pressure, and
// chaos_gc_remove_memory_pressure are now defined in gc_api.cpp
// (declared in gc_api.h).

}  // namespace chaos::il2cpp::runtime_core