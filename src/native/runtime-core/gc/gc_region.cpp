#include "gc_region.h"

#include <chaos/log.h>

#include <cstdio>

#include "gc_bgc.h"
#include "gc_events.h"
#include "gc_layout.h"
#include "gc_old_gen.h"
#include "gc_loh.h"
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
    // Oversized: bypass nursery, allocate via old gen.
    if (size > kMaxNurseryAlloc) {
        CHAOS_IL2CPP_LOG_DEBUG("CRAG", "nursery_oversized");
        void* ptr = g_old_gen.Allocate(size, true);
        // g_old_gen.Allocate already zeroes the memory via std::memset internally.
        // GcRecordAlloc is handled inside OldGen::Allocate.
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

    // Flush TLS allocation counter to scheduler before making any GC decision.
    // All fast-path allocations since the last slow path are accumulated in
    // tls_alloc_since_last_gc (thread-local, no cross-core atomics).  Flushing
    // here gives DecideCollection accurate allocation volume without cache-line
    // bouncing on every allocation.
    if (tls_alloc_since_last_gc > 0) {
        g_gc_scheduler.RecordAllocation(tls_alloc_since_last_gc);
        tls_alloc_since_last_gc = 0;
    }

    // GC rate limiter: only one thread may proceed with GC decision per 500μs
    // window.  Threads that fail skip GC entirely and go straight to getting a
    // fresh nursery — this prevents cascading GC initiations (safepoint storms)
    // when 100 threads exhaust their nurseries simultaneously.
    bool claimed_gc_slot = g_gc_scheduler.TryClaimGcSlot();

    if (claimed_gc_slot) {
        // Check scheduler for collection decision.
        // Returns YOUNG (nursery GC), FULL (STW full GC), FULL_BGC (concurrent mark),
        // or NONE (no GC needed).
        auto gc_decision = g_gc_scheduler.DecideCollection();

        if (gc_decision == GcCollectionKind::FULL) {
            // Scheduler triggered full GC — run STW mark-sweep immediately.
            CHAOS_IL2CPP_LOG_DEBUG("CRAG", "full_gc_via_scheduler_in_nursery_alloc_slow");
            uint32_t gen = threading::RequestGlobalSafepoint();
            g_old_gen.Collect(nullptr, nullptr);
            threading::ReleaseGlobalSafepoint(gen);
            g_gc_scheduler.RecordGcCompleted();

            // Try bump from existing nursery after full GC freed memory.
            if (tls_nursery_ctx.nursery != nullptr) {
                char* ptr = tls_nursery_ctx.nursery->current;
                char* next = ptr + size;
                if (next <= tls_nursery_ctx.nursery->end) {
                    tls_nursery_ctx.nursery->current = next;
                    std::memset(ptr, 0, size);
                    g_gc_scheduler.RecordAllocation(size);
                    memory_domain::GcTrackDomainAlloc(size);
                    return ptr;
                }
            }
        } else if (gc_decision == GcCollectionKind::FULL_BGC) {
            // Scheduler triggered BGC — start concurrent mark cycle (non-blocking).
            CHAOS_IL2CPP_LOG_DEBUG("CRAG", "full_bgc_via_scheduler_in_nursery_alloc_slow");
            BgcController::Instance().StartBgcCycle();
            // BGC runs concurrently; continue with normal nursery allocation flow.
        }

        // Proactive young GC: run when the scheduler budget is exceeded.
        // Also run when nursery has live objects and we are about to recycle it.
        // This prevents a safepoint deadlock: without this, thread A (in the slow
        // path holding the RegionManager mutex) could call RequestGlobalSafepoint,
        // and thread B (waiting on the same mutex) can't reach SafepointPoll to
        // confirm, causing the confirmation loop to spin forever.
        bool do_young_gc = (gc_decision == GcCollectionKind::YOUNG);
        if (!do_young_gc && tls_nursery_ctx.nursery != nullptr &&
            tls_nursery_ctx.nursery->current > tls_nursery_ctx.nursery->begin) {
            do_young_gc = true;
        }


        if (do_young_gc) {
            // NOTE: no LOG_DEBUG here — must not block on I/O before
            // RequestGlobalSafepoint.  A blocking fputc (pipe full) while
            // holding g_log_mutex would prevent GC from starting.
            uint32_t gen = threading::RequestGlobalSafepoint();
            GcYoungCollection(tls_nursery_ctx.nursery);
            threading::ReleaseGlobalSafepoint(gen);
            g_gc_scheduler.RecordGcCompleted();

            char* ptr = tls_nursery_ctx.nursery->current;
            char* next = ptr + size;
            if (next <= tls_nursery_ctx.nursery->end) {
                tls_nursery_ctx.nursery->current = next;
                std::memset(ptr, 0, size);
                g_gc_scheduler.RecordAllocation(size);
                return ptr;
            }
        }

        // Check if BGC concurrent mark has completed and needs STW re-mark.
        if (BgcController::Instance().IsRemarkNeeded()) {
            CHAOS_IL2CPP_LOG_DEBUG("CRAG", "bgc_remark_in_nursery_alloc_slow");
            uint32_t gen = threading::RequestGlobalSafepoint();
            BgcController::Instance().StwRemark();
            threading::ReleaseGlobalSafepoint(gen);

            // After re-mark, signal BGC to begin concurrent sweep.
            BgcController::Instance().StartConcurrentSweep();

            // Record full collection (BGC cycle conserves old-gen memory).
            g_gc_scheduler.RecordFullCollection(g_old_gen.TotalAllocated());

            // Try bump from the existing nursery again.
            if (tls_nursery_ctx.nursery != nullptr) {
                char* ptr = tls_nursery_ctx.nursery->current;
                char* next = ptr + size;
                if (next <= tls_nursery_ctx.nursery->end) {
                    tls_nursery_ctx.nursery->current = next;
                    std::memset(ptr, 0, size);
                    g_gc_scheduler.RecordAllocation(size);
                    memory_domain::GcTrackDomainAlloc(size);
                    return ptr;
                }
            }
        }

        // Check if BGC concurrent sweep has completed and needs STW compaction.
        if (BgcController::Instance().IsCompactNeeded()) {
            CHAOS_IL2CPP_LOG_DEBUG("CRAG", "bgc_compact_in_nursery_alloc_slow");
            uint32_t gen = threading::RequestGlobalSafepoint();
            BgcController::Instance().StwCompact();
            threading::ReleaseGlobalSafepoint(gen);
            g_gc_scheduler.RecordFullCollection(g_old_gen.TotalAllocated());

            if (tls_nursery_ctx.nursery != nullptr) {
                char* ptr = tls_nursery_ctx.nursery->current;
                char* next = ptr + size;
                if (next <= tls_nursery_ctx.nursery->end) {
                    tls_nursery_ctx.nursery->current = next;
                    std::memset(ptr, 0, size);
                    g_gc_scheduler.RecordAllocation(size);
                    memory_domain::GcTrackDomainAlloc(size);
                    return ptr;
                }
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
            g_gc_scheduler.RecordGcCompleted();
            // RecordFullCollection is handled inside OldGen::Collect().

            // After full GC, try bump from the existing nursery again.
            if (tls_nursery_ctx.nursery != nullptr) {
                char* ptr = tls_nursery_ctx.nursery->current;
                char* next = ptr + size;
                if (next <= tls_nursery_ctx.nursery->end) {
                    tls_nursery_ctx.nursery->current = next;
                    std::memset(ptr, 0, size);
                    g_gc_scheduler.RecordAllocation(size);
                    memory_domain::GcTrackDomainAlloc(size);
                    return ptr;
                }
            }
        }
    }  // if (claimed_gc_slot) — end of GC decision block

    // Current nursery exhausted (or GC skipped due to rate limiting, or
    // from the manager, using the scheduler's recommended size.
    // CRITICAL: Clear TLS nursery pointer BEFORE FreeRegion, because another
    // thread's concurrent GcYoungCollection may scan this thread's stack via
    // GcScanAllThreadRoots and find the ManagedThread::nursery_ctx pointer.
    // If the Region is freed before we clear the TLS reference, the scanner
    // dereferences a dangling pointer → SIGSEGV.
    if (tls_nursery_ctx.nursery != nullptr) {
        RegionId old_id = tls_nursery_ctx.nursery->id;
        if (auto* mt = threading::GetCurrentThread()) {
            mt->nursery_ctx = nullptr;
        }
        tls_nursery_ctx.nursery = nullptr;
        tls_nursery_ctx.limit = nullptr;
        RegionManager::Instance().FreeRegion(old_id);
    }

    CHAOS_IL2CPP_SIZE nursery_size = g_gc_scheduler.RecommendedNurserySize();
    Region* new_nursery = RegionManager::Instance().AllocateNurseryOfSize(nursery_size);
    if (new_nursery == nullptr) {
        // Fallback to old gen if no region available.
        CHAOS_IL2CPP_LOG_WARN("CRAG", "nursery_oom_fallback");
        void* ptr = g_old_gen.Allocate(size, true);
        // g_old_gen.Allocate already zeroes memory AND calls GcRecordAlloc internally.
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
        // g_old_gen.Allocate already zeroes memory AND calls GcRecordAlloc internally.
        return ptr;
    }

    // Ensure the nursery context is registered in ManagedThread.
    if (auto* mt = threading::GetCurrentThread()) {
        if (mt->nursery_ctx == nullptr) {
            mt->nursery_ctx = &tls_nursery_ctx;
        }
    }

    // Flush TLS allocation counter to scheduler (same rationale as
    // NurseryAllocateSlow).
    if (tls_alloc_since_last_gc > 0) {
        g_gc_scheduler.RecordAllocation(tls_alloc_since_last_gc);
        tls_alloc_since_last_gc = 0;
    }

    // GC rate limiter (same design as NurseryAllocateSlow): only one thread
    // may proceed with GC decision per 500μs window.
    bool claimed_gc_slot = g_gc_scheduler.TryClaimGcSlot();

    if (claimed_gc_slot) {
        // Same as NurseryAllocateSlow but passes scanning_required=false for
        // old-gen fallback.  The nursery bump path is identical.
        auto gc_decision = g_gc_scheduler.DecideCollection();

        if (gc_decision == GcCollectionKind::FULL) {
            CHAOS_IL2CPP_LOG_DEBUG("CRAG", "full_gc_via_scheduler_in_nursery_alloc_atomic_slow");
            uint32_t gen = threading::RequestGlobalSafepoint();
            g_old_gen.Collect(nullptr, nullptr);
            threading::ReleaseGlobalSafepoint(gen);
            g_gc_scheduler.RecordGcCompleted();

            if (tls_nursery_ctx.nursery != nullptr) {
                char* ptr = tls_nursery_ctx.nursery->current;
                char* next = ptr + size;
                if (next <= tls_nursery_ctx.nursery->end) {
                    tls_nursery_ctx.nursery->current = next;
                    std::memset(ptr, 0, size);
                    g_gc_scheduler.RecordAllocation(size);
                    memory_domain::GcTrackDomainAlloc(size);
                    return ptr;
                }
            }
        } else if (gc_decision == GcCollectionKind::FULL_BGC) {
            CHAOS_IL2CPP_LOG_DEBUG("CRAG", "full_bgc_via_scheduler_in_nursery_alloc_atomic_slow");
            BgcController::Instance().StartBgcCycle();
        }

        // Proactive young GC: run when the scheduler budget is exceeded or the
        // nursery has live objects (prevents safepoint deadlock — see rationale
        // in NurseryAllocateSlow).
        bool do_young_gc = (gc_decision == GcCollectionKind::YOUNG);
        if (!do_young_gc && tls_nursery_ctx.nursery != nullptr &&
            tls_nursery_ctx.nursery->current > tls_nursery_ctx.nursery->begin) {
            do_young_gc = true;
        }


        if (do_young_gc) {
            // NOTE: no LOG_DEBUG — see rationale in NurseryAllocateSlow.
            uint32_t gen = threading::RequestGlobalSafepoint();
            GcYoungCollection(tls_nursery_ctx.nursery);
            threading::ReleaseGlobalSafepoint(gen);
            g_gc_scheduler.RecordGcCompleted();

            char* ptr = tls_nursery_ctx.nursery->current;
            char* next = ptr + size;
            if (next <= tls_nursery_ctx.nursery->end) {
                tls_nursery_ctx.nursery->current = next;
                std::memset(ptr, 0, size);
                g_gc_scheduler.RecordAllocation(size);
                return ptr;
            }
        }

        // Check if BGC concurrent mark has completed and needs STW re-mark.
        if (BgcController::Instance().IsRemarkNeeded()) {
            CHAOS_IL2CPP_LOG_DEBUG("CRAG", "bgc_remark_in_nursery_alloc_atomic_slow");
            uint32_t gen = threading::RequestGlobalSafepoint();
            BgcController::Instance().StwRemark();
            threading::ReleaseGlobalSafepoint(gen);
            BgcController::Instance().StartConcurrentSweep();
            g_gc_scheduler.RecordFullCollection(g_old_gen.TotalAllocated());

            if (tls_nursery_ctx.nursery != nullptr) {
                char* ptr = tls_nursery_ctx.nursery->current;
                char* next = ptr + size;
                if (next <= tls_nursery_ctx.nursery->end) {
                    tls_nursery_ctx.nursery->current = next;
                    std::memset(ptr, 0, size);
                    g_gc_scheduler.RecordAllocation(size);
                    memory_domain::GcTrackDomainAlloc(size);
                    return ptr;
                }
            }
        }

        // Check if BGC concurrent sweep has completed and needs STW compaction.
        if (BgcController::Instance().IsCompactNeeded()) {
            CHAOS_IL2CPP_LOG_DEBUG("CRAG", "bgc_compact_in_nursery_alloc_atomic_slow");
            uint32_t gen = threading::RequestGlobalSafepoint();
            BgcController::Instance().StwCompact();
            threading::ReleaseGlobalSafepoint(gen);
            g_gc_scheduler.RecordFullCollection(g_old_gen.TotalAllocated());

            if (tls_nursery_ctx.nursery != nullptr) {
                char* ptr = tls_nursery_ctx.nursery->current;
                char* next = ptr + size;
                if (next <= tls_nursery_ctx.nursery->end) {
                    tls_nursery_ctx.nursery->current = next;
                    std::memset(ptr, 0, size);
                    g_gc_scheduler.RecordAllocation(size);
                    memory_domain::GcTrackDomainAlloc(size);
                    return ptr;
                }
            }
        }

        // Check if a full GC has been requested by the scheduler.
        if (g_gc_scheduler.IsFullGcRequested()) {
            CHAOS_IL2CPP_LOG_DEBUG("CRAG", "full_gc_in_nursery_alloc_atomic_slow");
            uint32_t gen = threading::RequestGlobalSafepoint();
            g_old_gen.Collect(nullptr, nullptr);
            threading::ReleaseGlobalSafepoint(gen);
            g_gc_scheduler.RecordGcCompleted();
            // RecordFullCollection is handled inside OldGen::Collect().

            if (tls_nursery_ctx.nursery != nullptr) {
                char* ptr = tls_nursery_ctx.nursery->current;
                char* next = ptr + size;
                if (next <= tls_nursery_ctx.nursery->end) {
                    tls_nursery_ctx.nursery->current = next;
                    std::memset(ptr, 0, size);
                    g_gc_scheduler.RecordAllocation(size);
                    memory_domain::GcTrackDomainAlloc(size);
                    return ptr;
                }
            }
        }
    }  // if (claimed_gc_slot) — end of GC decision block

    // CRITICAL: Clear TLS nursery pointer BEFORE FreeRegion (same rationale as
    // NurseryAllocateSlow — concurrent GcYoungCollection may scan this thread's
    // nursery_ctx and dereference a freed Region → SIGSEGV).
    if (tls_nursery_ctx.nursery != nullptr) {
        RegionId old_id = tls_nursery_ctx.nursery->id;
        if (auto* mt = threading::GetCurrentThread()) {
            mt->nursery_ctx = nullptr;
        }
        tls_nursery_ctx.nursery = nullptr;
        tls_nursery_ctx.limit = nullptr;
        RegionManager::Instance().FreeRegion(old_id);
    }

    CHAOS_IL2CPP_SIZE nursery_size = g_gc_scheduler.RecommendedNurserySize();
    Region* new_nursery = RegionManager::Instance().AllocateNurseryOfSize(nursery_size);
    if (new_nursery == nullptr) {
        CHAOS_IL2CPP_LOG_WARN("CRAG", "nursery_oom_fallback_atomic");
        void* ptr = g_old_gen.Allocate(size, false);
        // g_old_gen.Allocate already zeroes memory AND calls GcRecordAlloc internally.
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
    YoungCollectionResult result = GcYoungCollection(nursery);

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

    // Step 1: Young collection on current TLS nursery (if any).
    if (tls_nursery_ctx.nursery != nullptr &&
        tls_nursery_ctx.nursery->current > tls_nursery_ctx.nursery->begin) {
        uint32_t gen = threading::RequestGlobalSafepoint();
        GcYoungCollection(tls_nursery_ctx.nursery);
        threading::ReleaseGlobalSafepoint(gen);
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