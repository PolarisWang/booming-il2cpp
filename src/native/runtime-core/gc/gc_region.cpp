#include "gc_region.h"

#include <chaos/log.h>

#include <cstdio>
#include <atomic>

#include "core/gc_alloc_stubs.h"
#include "gc_bgc.h"
#include "gc_coordinator.h"
#include "gc_diagnostics.h"
#include "gc_etw.h"
#include "gc_events.h"
#include "gc_gen1.h"
#include "gc_heap.h"
#include "gc_heap_manager.h"   // M3A-1: per-heap manager lifecycle
#include "gc_layout.h"
#include "gc_numa.h"
#include "gc_config.h"
#include "gc_old_gen.h"
#include "gc_loh.h"
#include "gc_parallel_mark.h"
#include "gc_scheduler.h"
#include "gc_stats.h"
#include "gc_api.h"
#include "gc_helpers.h"
#include "gc_young_collector.h"
#include "memory_domain.h"
#include "thread_state.h"
#include "profile_stats.h"

#include <chaos/pal/pal_mem.h>

namespace chaos::il2cpp::runtime_core {

// ── Region-to-generation skewed table  ───────────────────────
// Global so the inline GetRegionGen() (write-path fast lookup) can read it
// without invoking RegionManager.  Lazy-grown to cover the highest seen
// region address.  Guarded by RegionManager::mutex_ (allocated under it).
uint8_t* g_region_to_gen = nullptr;
CHAOS_IL2CPP_SIZE g_region_gen_bytes = 0;                 // table size in bytes (bounds guard)
static CHAOS_IL2CPP_SIZE s_region_gen_high_idx = 0;   // highest covered index (inclusive)

/// Ensure g_region_to_gen covers index = @a addr >> kRegionGenShift.
/// Called under RegionManager::mutex_ from AllocateRegion.  Grows by doubling.
static void EnsureRegionGenCoverage(uintptr_t addr) {
    CHAOS_IL2CPP_SIZE idx = addr >> kRegionGenShift;
    if (idx <= s_region_gen_high_idx) return;
    CHAOS_IL2CPP_SIZE need = idx + 1;
    CHAOS_IL2CPP_SIZE cap = (s_region_gen_high_idx == 0) ? 1024 : s_region_gen_high_idx + 1;
    while (cap < need) cap *= 2;
    auto* new_tab = static_cast<uint8_t*>(std::realloc(g_region_to_gen, cap));
    if (new_tab == nullptr) return;  // keep old coverage (fail-safe conservative)
    // Preserve existing entries; default new entries to kRegionGenOld (so an
    // uncovered region 查表 returns "old" → conservative card marking).
    for (CHAOS_IL2CPP_SIZE i = s_region_gen_high_idx + 1; i < cap; i++) new_tab[i] = kRegionGenOld;
    g_region_to_gen = new_tab;
    g_region_gen_bytes = cap;
    s_region_gen_high_idx = cap - 1;
}

}  // namespace chaos::il2cpp::runtime_core

#include <cstdlib>

// ── Large page allocation helpers ──────────────────────────────────
// These are only compiled when CHAOS_IL2CPP_GC_LARGE_PAGES == 1.
// On Windows: uses VirtualAlloc with MEM_LARGE_PAGES (requires
// SeLockMemoryPrivilege).  On Linux: uses mmap with MAP_HUGETLB.
// Both fall back gracefully — the callers in AllocateRegion check
// for nullptr and fall through to normal PalVirtualAlloc.
#if defined(CHAOS_IL2CPP_GC_LARGE_PAGES) && CHAOS_IL2CPP_GC_LARGE_PAGES == 1

static CHAOS_IL2CPP_SIZE GcGetLargePageMinimum() noexcept {
    return chaos::il2cpp::pal::PalGetLargePageSize();
}

static void* GcTryAllocLargePages(CHAOS_IL2CPP_SIZE alloc_size) noexcept {
    return chaos::il2cpp::pal::PalVirtualAllocLarge(alloc_size);
}

#endif  // CHAOS_IL2CPP_GC_LARGE_PAGES == 1
#include <cstring>

namespace chaos::il2cpp::runtime_core {

/// Mark every 4MB region-gen byte covering [@a start, @a end) as OLD.  See the
/// header comment for rationale.  Called by old-gen / LOH page allocation under
/// their commit lock so updates are serialized with concurrent SetRegionGen
/// (nursery/Gen1 region creation).
void GcMarkRangeOld(uintptr_t start, uintptr_t end) noexcept {
    if (start >= end) return;
    EnsureRegionGenCoverage(start);
    EnsureRegionGenCoverage(end - 1);
    static constexpr CHAOS_IL2CPP_SIZE kRegionSize = CHAOS_IL2CPP_SIZE{1} << kRegionGenShift;
    uintptr_t a = start & ~(kRegionSize - 1);
    for (; a < end; a += kRegionSize) {
        SetRegionGen(a, kRegionGenOld);
    }
}

// ── Shared young generation + TLAB ──────────────────────────────
YoungGeneration g_young_gen;
thread_local TLAB tls_tlab;

// Physical nursery/Gen1 membership for GetRegionGen: authoritative over the
// coarse 4MB chunk table (see gc_region.h GetRegionGen rationale).  Reads the
// live nursery and Gen1 region ranges directly.  Returns young(0)/gen1(1) if
// physically inside, else kRegionGenInvalid.
uint8_t GcGetRegionGenPhysical(uintptr_t addr) noexcept {
    // Nursery: authoritative via RegionManager's registered nursery range array
    // (IsNurseryPointer) — this covers BOTH the shared young region (wired by
    // InitYoungGeneration) AND any independently-allocated REGION_NURSERY (tests).
    if (RegionManager::Instance().IsNurseryPointer(reinterpret_cast<const void*>(addr))) {
        return kRegionGenYoung;
    }
    auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    if (gen1 != nullptr) {
        auto* bump = g_young_gen.gen1_bump.load(std::memory_order_acquire);
        if (bump != nullptr && addr >= reinterpret_cast<uintptr_t>(gen1->begin) &&
            addr < reinterpret_cast<uintptr_t>(bump)) {
            return kRegionGenGen1;
        }
    }
    // In-place demotion (CoreCLR-aligned, GC-N6 #10): a gen1-owned object that
    // physically resides in an old-gen page (not the gen1 bump region) classifies
    // as gen1 so the write barrier's ref-gen, the scavenger's condemned filter,
    // and age tenuring treat it consistently.  Without this, it would read OLD
    // (the 4MB chunk byte) and the barrier would drop an old->demoted edge.
    if (IsInDemotedSet(reinterpret_cast<const void*>(addr))) {
        return kRegionGenGen1;
    }
    return kRegionGenInvalid;
}

// ── Forward declarations ───────────────────────────────────────
TLAB ClaimEmergencyTlab() noexcept;

// Per-thread allocation counter (TLS-local, flushed to scheduler in slow path).
thread_local CHAOS_IL2CPP_SIZE tls_alloc_since_last_gc = 0;

// Per-thread fast-path allocation counters (flushed to g_gc_stats at GC time).
thread_local CHAOS_IL2CPP_SIZE tls_alloc_fast_count = 0;
thread_local CHAOS_IL2CPP_SIZE tls_alloc_fast_bytes = 0;

// Per-thread adaptive TLAB size (initialized to default, adjusted by UpdateTlabSize).
thread_local CHAOS_IL2CPP_SIZE tls_tlab_size = kDefaultTlabSize;

// ── TLS POH context ────────────────────────────────────────────
// Each thread has a fast bump-pointer path for POH allocation.
// POH regions are shared across threads (mutex-protected for allocation).
// For now, PohAllocate uses the process-wide POH context to avoid
// per-thread POH region proliferation (pinned objects are typically few).
static Region* s_poh_current = nullptr;  // current POH bump region
// GcSpinLock (not std::mutex) for POH: the lock hold is a short bump-pointer
// critical section, and a std::mutex can park a thread in a kernel wait while
// in cooperative mode — leaving it unable to acknowledge a safepoint (a
// potential 3-party deadlock: thread waits mutex, coordinator waits thread's
// suspend_ack, mutex holder may wait on safepoint).  GcSpinLock never parks
// (spin-then-yield), and is wrapped in ScopedPreemptiveMode so the safepoint
// coordinator skips the thread while it may be spinning.
static GcSpinLock s_poh_lock;

// Platform virtual memory helpers for region recycling.
static void VirtualFreeRegion(void* ptr, CHAOS_IL2CPP_SIZE size) {
    if (ptr) chaos::il2cpp::pal::PalVirtualFree(ptr, size);
}

// ======================================================================
// NurseryAllocateSlow
// ======================================================================
void* NurseryAllocateSlow(CHAOS_IL2CPP_SIZE size) {
    CHAOS_IL2CPP_PROFILE_SCOPE("NurseryAllocateSlow");
    ProfileRecordSlowPath();

    if (size > kMaxTlabAlloc) {
        ProfileRecordLargeObjAlloc(static_cast<int64_t>(size));
    }

    // Flush TLS allocation counter to scheduler before making any GC decision.
    FlushTlsAllocCounter();
    // Flush fast-path TLS counters to global stats before any GC or stats read.
    FlushTlsFastStats();

    // Phase 0: Check soft memory limit (non-blocking).
    // When estimated heap usage exceeds the soft limit, request a full GC
    // at the next safepoint.  Does not block or fail the allocation.
    if (G_Scheduler().ExceedsSoftLimit()) [[unlikely]] {
        G_Scheduler().RequestFullGc();
    }

    // Phase 1: Try to claim a new TLAB from the shared young generation.
    TLAB tlab = TlabClaimFromYoungGen();
    if (tlab.current != nullptr) {
        tls_tlab = tlab;
        // Objects larger than kMaxTlabAlloc cannot use TLAB — NurseryAllocate
        // would redirect back to NurseryAllocateSlow, creating infinite recursion.
        // Fall through; the TLAB is set up for future small allocations.
        if (size <= kMaxTlabAlloc) {
            return NurseryAllocate(size);
        }
    }

    // SPB: If TLAB claim failed while a safepoint is active (e.g., BGC
    // background cycle), try the emergency TLAB reserve before attempting
    // GC or falling to old-gen.  This keeps allocation throughput at TLAB
    // level even when the nursery is locked by a BGC background cycle.
    if (tlab.current == nullptr && threading::SafepointRequested()) [[unlikely]] {
        TLAB emerg = ClaimEmergencyTlab();
        if (emerg.current != nullptr) {
            tls_tlab = emerg;
            if (size <= kMaxTlabAlloc) {
                return NurseryAllocate(size);
            }
        }
    }

    // Phase 2: Young region is exhausted — consult the GC scheduler.
    // When in NO_GC_REGION, skip collection and go straight to old-gen fallback.
    if (!GcIsInNoGcRegion() && G_Scheduler().TryClaimGcSlot()) {

        // Check if scheduler wants a full STW GC before doing a young GC.
        // DecideCollection() returns FULL when full_gc_requested_ is set or
        // page growth exceeds the trigger threshold.
        auto collection_kind = G_Scheduler().DecideCollection();
        if (collection_kind == GcCollectionKind::FULL) {
            uint32_t gen = threading::RequestGlobalSafepoint();
            auto* mt = threading::GetCurrentThread();
            if (mt) {
                mt->tlab_start = tls_tlab.start;
                mt->tlab_current = tls_tlab.current;
            }
            auto gc_start = std::chrono::high_resolution_clock::now();
            chaos_gc_collect();
            auto gc_end = std::chrono::high_resolution_clock::now();
            auto gc_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(gc_end - gc_start).count();
            ProfileRecordGcPause(static_cast<int64_t>(gc_ns));
            if (mt) {
                mt->tlab_start = nullptr;
                mt->tlab_current = nullptr;
            }
            threading::ReleaseGlobalSafepoint(gen);
            GcAdvanceBgcCycle();
            tls_tlab = TLAB{};
        } else {
        // Phase 2a: Check if Gen1 needs independent collection before young GC.
        // If Gen1 is near-full/fragmented, collect it independently — this avoids
        // the cost of Cheney-scavenging the nursery when only Gen1 needs cleanup.
        if (GcGen1ShouldCollect()) {
            uint32_t gen = threading::RequestGlobalSafepoint();
            auto* mt = threading::GetCurrentThread();
            if (mt) {
                mt->tlab_start = tls_tlab.start;
                mt->tlab_current = tls_tlab.current;
            }
            auto gc1_start = std::chrono::high_resolution_clock::now();
            GcGen1Collection();
            auto gc1_end = std::chrono::high_resolution_clock::now();
            auto gc1_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(gc1_end - gc1_start).count();
            ProfileRecordGcPause(static_cast<int64_t>(gc1_ns));
            if (mt) {
                mt->tlab_start = nullptr;
                mt->tlab_current = nullptr;
            }
            threading::ReleaseGlobalSafepoint(gen);
            GcAdvanceBgcCycle();
            tls_tlab = TLAB{};

            // Retry TLAB after Gen1 collection freed Gen2 capacity.
            TLAB tlab2 = TlabClaimFromYoungGen();
            if (tlab2.current != nullptr) {
                tls_tlab = tlab2;
                if (size <= kMaxTlabAlloc) {
                    return NurseryAllocate(size);
                }
                // Oversized: TLAB is set up for future small allocs, fall through to old gen.
            }

            // TLAB still exhausted — do young GC with force_skip_gen1=true
            // since Gen1 was just collected independently.
            uint32_t gen2 = threading::RequestGlobalSafepoint();
            auto* mt2 = threading::GetCurrentThread();
            if (mt2) {
                mt2->tlab_start = tls_tlab.start;
                mt2->tlab_current = tls_tlab.current;
            }
            GcYoungCollection(true);  // force_skip_gen1=true
            if (mt2) {
                mt2->tlab_start = nullptr;
                mt2->tlab_current = nullptr;
            }
            threading::ReleaseGlobalSafepoint(gen2);
            GcAdvanceBgcCycle();
            tls_tlab = TLAB{};
        } else {
            // Normal flow: young GC with default Gen1 handling (Phase 4).
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
    }
    } else if (!GcIsInNoGcRegion() && G_Scheduler().GcSlotIsHeld()) {
        // CoreCLR wait_for_gc_done alignment (a_state_retry_allocate):
        // another thread is in-flight with GC right now (it claimed the GC
        // slot but hasn't completed yet).  Wait for that GC to finish
        // (SafepointPoll blocks in cooperative mode until the safepoint is
        // released), then retry TLAB from the freshly-reset nursery.
        threading::SafepointPoll();
        tlab = TlabClaimFromYoungGen();
        if (tlab.current != nullptr) {
            tls_tlab = tlab;
            if (size <= kMaxTlabAlloc) {
                return NurseryAllocate(size);
            }
        }
    }

    // Phase 3: Retry from the fresh young region + new TLAB.
    tlab = TlabClaimFromYoungGen();
    if (tlab.current != nullptr) {
        tls_tlab = tlab;
        if (size <= kMaxTlabAlloc) {
            return NurseryAllocate(size);
        }
        // Oversized: TLAB is set up for future small allocs, fall through to old gen.
    }

    // SPB: If the post-GC TLAB retry failed while a safepoint is still active,
    // try the emergency TLAB reserve one more time before falling to old gen.
    if (tlab.current == nullptr && threading::SafepointRequested()) [[unlikely]] {
        TLAB emerg = ClaimEmergencyTlab();
        if (emerg.current != nullptr) {
            tls_tlab = emerg;
            if (size <= kMaxTlabAlloc) {
                return NurseryAllocate(size);
            }
        }
    }

    // Phase 4: Young region full — fall back to old gen.
    // Switch to preemptive mode so old-gen allocation does not block
    // safepoint handshake (the old-gen allocator takes mutex_ internally,
    // which would deadlock against ScanDirtyCardsInPages).
    threading::EnterPreemptiveMode();
    void* old_result = G_OldGen().Allocate(size, true);
    threading::EnterCooperativeMode();  // SafepointPoll re-sync if pending
    if (old_result) {
        tls_total_allocated_bytes += static_cast<CHAOS_IL2CPP_INT64>(size);
    } else if (BgcController::IsFinalizerThread()) {
        // Finalizer OOM guarantee: if the finalizer thread is trying to
        // allocate during heap exhaustion, use the emergency reserve to
        // prevent finalizer deadlock.  The reserve is replenished after
        // the next GC cycle completes.
        threading::EnterPreemptiveMode();
        old_result = G_OldGen().AllocateFromEmergencyReserve(size);
        threading::EnterCooperativeMode();
        if (old_result) {
            tls_total_allocated_bytes += static_cast<CHAOS_IL2CPP_INT64>(size);
        }
    }
    if (old_result == nullptr) {
        // All recovery attempts failed — invoke the full OOM recovery chain:
        // blocking full GC → retry → emergency reserve → OOM event.
        //
        // Fix-A (recovery routing): the retry lambda RE-ENTERS the original
        // allocation path after the full GC, rather than jumping straight to
        // old-gen.  The full GC just reset the nursery bump pointer, so
        // TlabClaimFromYoungGen() now succeeds — allocate from the fresh
        // nursery first, and only fall back to old-gen if the object is
        // oversized or the nursery retry fails.  This path runs only inside
        // HandleOomCondition's step-2 retry (never on the normal fast path).
        struct OomRetryCtx { CHAOS_IL2CPP_SIZE size; bool scanning; };
        OomRetryCtx ctx{size, true};
        old_result = HandleOomCondition(
            [](void* ctx) -> void* {
                auto* c = static_cast<OomRetryCtx*>(ctx);
                // Re-enter nursery first: full GC reset the bump pointer.
                if (c->size <= kMaxTlabAlloc) {
                    TLAB tlab = TlabClaimFromYoungGen();
                    if (tlab.current != nullptr) {
                        tls_tlab = tlab;
                        void* nursery_ptr = NurseryAllocate(c->size);
                        if (nursery_ptr != nullptr) {
                            return nursery_ptr;
                        }
                        // NurseryAllocate returned nullptr despite a fresh TLAB
                        // (e.g. oversized for remaining TLAB slack).  Clear the
                        // orphan TLAB so the next allocation re-claims rather than
                        // using a stale TLS pointer (review #6).
                        tls_tlab = TLAB{};
                    }
                }
                threading::EnterPreemptiveMode();
                void* p = G_OldGen().Allocate(c->size, c->scanning);
                threading::EnterCooperativeMode();
                return p;
            }, &ctx, size);
        if (old_result) {
            tls_total_allocated_bytes += static_cast<CHAOS_IL2CPP_INT64>(size);
        }
    }
    return old_result;
}

void* NurseryAllocateAtomicSlow(CHAOS_IL2CPP_SIZE size) {
    CHAOS_IL2CPP_PROFILE_SCOPE("NurseryAllocateAtomicSlow");
    ProfileRecordSlowPath();

    if (size > kMaxTlabAlloc) {
        ProfileRecordLargeObjAlloc(static_cast<int64_t>(size));
    }

    FlushTlsAllocCounter();

    // Phase 0: Check soft memory limit (non-blocking).
    if (G_Scheduler().ExceedsSoftLimit()) [[unlikely]] {
        G_Scheduler().RequestFullGc();
    }

    // Phase 1: Try to claim a new TLAB.
    TLAB tlab = TlabClaimFromYoungGen();
    if (tlab.current != nullptr) {
        tls_tlab = tlab;
        if (size <= kMaxTlabAlloc) {
            return NurseryAllocateAtomic(size);
        }
    }

    // SPB: If TLAB claim failed while a safepoint is active, try emergency.
    if (tlab.current == nullptr && threading::SafepointRequested()) [[unlikely]] {
        TLAB emerg = ClaimEmergencyTlab();
        if (emerg.current != nullptr) {
            tls_tlab = emerg;
            if (size <= kMaxTlabAlloc) {
                return NurseryAllocateAtomic(size);
            }
        }
    }

    // Phase 2: Young region is exhausted — consult the GC scheduler.
    // When in NO_GC_REGION, skip collection and go straight to old-gen fallback.
    if (!GcIsInNoGcRegion() && G_Scheduler().TryClaimGcSlot()) {

        // Check if scheduler wants a full STW GC before doing a young GC.
        // DecideCollection() returns FULL when full_gc_requested_ is set or
        // page growth exceeds the trigger threshold.
        auto collection_kind = G_Scheduler().DecideCollection();
        if (collection_kind == GcCollectionKind::FULL) {
            uint32_t gen = threading::RequestGlobalSafepoint();
            auto* mt = threading::GetCurrentThread();
            if (mt) {
                mt->tlab_start = tls_tlab.start;
                mt->tlab_current = tls_tlab.current;
            }
            auto gc_start = std::chrono::high_resolution_clock::now();
            chaos_gc_collect();
            auto gc_end = std::chrono::high_resolution_clock::now();
            auto gc_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(gc_end - gc_start).count();
            ProfileRecordGcPause(static_cast<int64_t>(gc_ns));
            if (mt) {
                mt->tlab_start = nullptr;
                mt->tlab_current = nullptr;
            }
            threading::ReleaseGlobalSafepoint(gen);
            GcAdvanceBgcCycle();
            tls_tlab = TLAB{};
        } else {
            // Young GC (scanning_required = false for pointer-free).
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
    } else if (!GcIsInNoGcRegion() && G_Scheduler().GcSlotIsHeld()) {
        // CoreCLR wait_for_gc_done alignment (mirror NurseryAllocateSlow):
        // another thread is in-flight with GC right now.  Wait for it to
        // complete, then retry TLAB from the freshly-reset nursery.
        threading::SafepointPoll();
        tlab = TlabClaimFromYoungGen();
        if (tlab.current != nullptr) {
            tls_tlab = tlab;
            if (size <= kMaxTlabAlloc) {
                return NurseryAllocateAtomic(size);
            }
        }
    }

    // Phase 3: Retry.
    tlab = TlabClaimFromYoungGen();
    if (tlab.current != nullptr) {
        tls_tlab = tlab;
        if (size <= kMaxTlabAlloc) {
            return NurseryAllocateAtomic(size);
        }
        // Oversized: TLAB is set up for future small allocs, fall through to old gen.
    }

    // SPB: If post-GC retry fails while safepoint still active, try emergency.
    if (tlab.current == nullptr && threading::SafepointRequested()) [[unlikely]] {
        TLAB emerg = ClaimEmergencyTlab();
        if (emerg.current != nullptr) {
            tls_tlab = emerg;
            if (size <= kMaxTlabAlloc) {
                return NurseryAllocateAtomic(size);
            }
        }
    }

    // Phase 4: Fall back to old gen (no scanning needed).
    threading::EnterPreemptiveMode();
    void* old_result = G_OldGen().Allocate(size, false);
    threading::EnterCooperativeMode();
    if (old_result) {
        tls_total_allocated_bytes += static_cast<CHAOS_IL2CPP_INT64>(size);
    }
    if (old_result == nullptr) {
        // Atomic variant OOM recovery: full GC → retry → emergency reserve.
        struct OomRetryCtx { CHAOS_IL2CPP_SIZE size; bool scanning; };
        OomRetryCtx ctx{size, false};
        old_result = HandleOomCondition(
            [](void* ctx) -> void* {
                auto* c = static_cast<OomRetryCtx*>(ctx);
                // Re-enter nursery first: full GC reset the bump pointer.
                if (c->size <= kMaxTlabAlloc) {
                    TLAB tlab = TlabClaimFromYoungGen();
                    if (tlab.current != nullptr) {
                        tls_tlab = tlab;
                        void* nursery_ptr = NurseryAllocateAtomic(c->size);
                        if (nursery_ptr != nullptr) {
                            return nursery_ptr;
                        }
                        // NurseryAllocateAtomic returned nullptr despite a fresh
                        // TLAB — clear the orphan TLAB so the TLS pointer does not
                        // point to a claimed-but-unused region (review #6).
                        tls_tlab = TLAB{};
                    }
                }
                threading::EnterPreemptiveMode();
                void* p = G_OldGen().Allocate(c->size, c->scanning);
                threading::EnterCooperativeMode();
                return p;
            }, &ctx, size);
        if (old_result) {
            tls_total_allocated_bytes += static_cast<CHAOS_IL2CPP_INT64>(size);
        }
    }
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
        void* result = G_OldGen().Allocate(size, true);
        if (result) {
            tls_total_allocated_bytes += static_cast<CHAOS_IL2CPP_INT64>(size);
        }
        return result;
    }

    // Switch to preemptive mode for the POH lock region (GcSpinLock spin-wait
    // in cooperative mode would block safepoint coordination).  The lock is
    // held only for the bump-pointer fast path + fallback, never across alloc.
    ScopedPreemptiveMode preempt;
    GcSpinLockGuard lock(s_poh_lock);

    // Try bump from current POH region.
    if (s_poh_current != nullptr) {
        char* ptr = s_poh_current->current;
        char* next = ptr + size;
        if (next <= s_poh_current->end) {
            s_poh_current->current = next;
            std::memset(ptr, 0, size);
            G_Scheduler().RecordAllocation(size);
            tls_total_allocated_bytes += static_cast<CHAOS_IL2CPP_INT64>(size);
            return ptr;
        }
    }

    // Current POH region exhausted — allocate a new one.
    Region* new_poh = RegionManager::Instance().AllocateRegion(
        RegionKind::REGION_POH, kPohRegionSize);
    if (new_poh == nullptr) {
        CHAOS_IL2CPP_LOG_WARN("CRAG", "poh_oom_fallback");
        void* result = G_OldGen().Allocate(size, true);
        if (result) {
            tls_total_allocated_bytes += static_cast<CHAOS_IL2CPP_INT64>(size);
        }
        return result;
    }

    s_poh_current = new_poh;
    char* ptr = new_poh->current;
    new_poh->current = ptr + size;
    std::memset(ptr, 0, size);
    G_Scheduler().RecordAllocation(size);
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
    // Initialize the GC config singleton (env overrides + programmatic knobs).
    GcConfig().Initialize();

    // M3A-1: initialize the per-heap manager.  Under the WKS default
    // (GC_SERVER=0) this is a no-op; when GC_SERVER=1 it allocates the per-heap
    // GcHeapContext array and per-heap old-gen, so Server-GC multi-heap is
    // actually prepared instead of crashing on a null/empty array (the previous
    // zero-production-call gap).
    GcHeapManager::Instance().Initialize();

    // Latch config-driven hot-path knobs once, so the allocation / LOH / mark
    // hot paths read a plain machine load (the latched values) instead of
    // consulting the config singleton per allocation.  kMaxTlabAlloc /
    // kLohThreshold / kMaxParallelMarkWorkers were converted from constexpr to
    // inline mutable values (defaulting to the historical constants); this
    // overwrites them from the env/API CHAOS_GC_* knobs so they actually drive
    // real behavior (not just the init log).
    kMaxTlabAlloc = GcConfig().MaxTlabAlloc;
    kLohThreshold = GcConfig().LohThreshold;
    kMaxParallelMarkWorkers = static_cast<int>(GcConfig().ParallelMarkWorkers);

    // Initialize scheduler memory limits from config (env-driven, replaces the
    // former compile-time #if CHAOS_IL2CPP_GC_HEAP_*_LIMIT_MB only).
    CHAOS_IL2CPP_SIZE hard_limit_mb = GcConfig().HeapHardLimitMB;
    if (hard_limit_mb > 0) {
        G_Scheduler().SetHardLimit(hard_limit_mb * 1024 * 1024);
    }
#if defined(CHAOS_IL2CPP_GC_HEAP_HARD_LIMIT_MB) && CHAOS_IL2CPP_GC_HEAP_HARD_LIMIT_MB > 0
    if (hard_limit_mb == 0) {  // fall back to compile-time when no env knob
        G_Scheduler().SetHardLimit(
            static_cast<CHAOS_IL2CPP_SIZE>(CHAOS_IL2CPP_GC_HEAP_HARD_LIMIT_MB) * 1024 * 1024);
    }
#endif
    CHAOS_IL2CPP_SIZE soft_limit_mb = GcConfig().HeapSoftLimitMB;
    if (soft_limit_mb > 0) {
        G_Scheduler().SetSoftLimit(soft_limit_mb * 1024 * 1024);
    }
#if defined(CHAOS_IL2CPP_GC_HEAP_SOFT_LIMIT_MB) && CHAOS_IL2CPP_GC_HEAP_SOFT_LIMIT_MB > 0
    if (soft_limit_mb == 0) {  // fall back to compile-time when no env knob
        G_Scheduler().SetSoftLimit(
            static_cast<CHAOS_IL2CPP_SIZE>(CHAOS_IL2CPP_GC_HEAP_SOFT_LIMIT_MB) * 1024 * 1024);
    }
#endif

    // Nursery / Gen1 sizes from config (env-tunable, no recompile).
    CHAOS_IL2CPP_SIZE nursery_size = GcConfig().DefaultNurserySize;
    CHAOS_IL2CPP_SIZE gen1_size = GcConfig().DefaultGen1Size > 0
        ? GcConfig().DefaultGen1Size : nursery_size;

    // Allocate an independent nursery region (no longer split 50/50 with survivor).
    auto* nursery = RegionManager::Instance().AllocateRegion(
        RegionKind::REGION_NURSERY, nursery_size);
    g_young_gen.region.store(nursery, std::memory_order_release);

    // Allocate an independent Gen1 survivor region.
    auto* gen1 = RegionManager::Instance().AllocateRegion(
        RegionKind::REGION_GEN1, gen1_size);
    g_young_gen.gen1_region.store(gen1, std::memory_order_release);

    if (nursery && gen1) {
        // ── Initialize SPB emergency TLAB reserve ──
        // Carve 256 KB from the nursery end as an emergency pool for threads
        // during BGC safepoints, so they don't degenerate to old-gen
        // (VirtualAlloc + mutex) while the nursery is locked by a BGC cycle.
        char* emergency_start = nursery->end - YoungGeneration::kEmergencyTlabSize;
        g_young_gen.emergency_start = emergency_start;
        g_young_gen.emergency_bump.store(emergency_start, std::memory_order_release);

        // Shrink region_end so TLAB claims stop before the emergency reserve,
        // and young GC skips it during nursery scanning.
        g_young_gen.region_end.store(emergency_start, std::memory_order_release);
        g_young_gen.bump.store(nursery->begin, std::memory_order_release);

        // Initialize Gen1 bump pointer and end.
        g_young_gen.gen1_bump.store(gen1->begin, std::memory_order_release);
        g_young_gen.gen1_end = gen1->end;

        // Register nursery range with the card table.
        GcRegisterHeapRange(
            reinterpret_cast<uintptr_t>(nursery->begin),
            reinterpret_cast<uintptr_t>(nursery->end));

        // Register Gen1 range with the card table.
        GcRegisterHeapRange(
            reinterpret_cast<uintptr_t>(gen1->begin),
            reinterpret_cast<uintptr_t>(gen1->end));

        // Set nursery range for DirtyCard fast skip (young GC Phase 2
        // scans nursery precisely, so card writes are unnecessary).
        GcSetCardTableNurseryRange(
            reinterpret_cast<uintptr_t>(nursery->begin),
            reinterpret_cast<uintptr_t>(nursery->end));
    }
}

void DestroyYoungGeneration() noexcept {
    auto* nursery = g_young_gen.region.exchange(nullptr, std::memory_order_acq_rel);
    if (nursery) {
        RegionManager::Instance().FreeRegion(nursery->id);
    }
    auto* gen1 = g_young_gen.gen1_region.exchange(nullptr, std::memory_order_acq_rel);
    if (gen1) {
        RegionManager::Instance().FreeRegion(gen1->id);
    }
    g_young_gen.bump.store(nullptr, std::memory_order_release);
    g_young_gen.region_end.store(nullptr, std::memory_order_release);
}

void ResizeGen1Region(CHAOS_IL2CPP_SIZE new_size) {
    auto* old_gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    if (old_gen1 == nullptr) return;

    auto* new_gen1 = RegionManager::Instance().AllocateRegion(
        RegionKind::REGION_GEN1, new_size);
    if (new_gen1 == nullptr) {
        CHAOS_IL2CPP_LOG_WARN_M("CRAG", "gen1_resize_oom size={0}",
            static_cast<unsigned long long>(new_size));
        return;
    }

    // Free the old Gen1 region (Gen1 is empty after collection, no migration
    // needed).  MUST happen BEFORE publishing the new pointer + bump: FreeRegion()
    // of a REGION_GEN1 unconditionally clears g_young_gen.{gen1_region,gen1_end,
    // gen1_bump} (gc_region.cpp:946-950).  If we published the new region first,
    // that clear would wipe it, silently disabling Gen1 for the rest of the
    // process (every subsequent survivor falls straight to old-gen).
    RegionManager::Instance().FreeRegion(old_gen1->id);

    // Publish new region atomically (after the old region is gone).
    g_young_gen.gen1_region.store(new_gen1, std::memory_order_release);
    g_young_gen.gen1_bump.store(new_gen1->begin, std::memory_order_release);
    g_young_gen.gen1_end = new_gen1->end;

    // Register new Gen1 range with the card table.
    // The old range's L2 segments remain registered but are harmless
    // (the old region's pages are freed, so no writes can dirty them).
    GcRegisterHeapRange(
        reinterpret_cast<uintptr_t>(new_gen1->begin),
        reinterpret_cast<uintptr_t>(new_gen1->end));
}

TLAB TlabClaimFromYoungGen() noexcept {
    // Lazy one-time initialization of the shared young generation.
    // RuntimeInit may not be called in all execution paths (e.g., benchmark
    // hosts call generated code directly).  Without this, g_young_gen.bump
    // is nullptr and every allocation falls through to OldGen.
    // Use compare_exchange for thread safety — only one thread initializes.
    static std::atomic<int> s_young_gen_state{0};  // 0=uninit, 1=initing, 2=ready
    //
    // GC-N7 / double-init: RuntimeInit OR a test fixture (GcTestBase::SetUp) that
    // calls InitYoungGeneration() directly leaves this private state at 0 (the
    // fixture doesn't touch it), so the FIRST NurseryAllocate here re-ran
    // InitYoungGeneration() — re-allocating BOTH the nursery and Gen1 regions and
    // pointing g_young_gen.region/gen1_region at fresh addresses.  Any object
    // allocated into the ORIGINAL regions before this point (e.g., a direct
    // TryAllocateInGen1 in a test) is left in an orphaned region the next
    // collection no longer scans → objects_promoted=0 / dangling (gen1
    // SingleLiveObject bytes_promoted=0).  Make the lazy-init idempotent: if a
    // region is already wired, InitYoungGeneration already ran — just mark ready,
    // never re-allocate and strand live region owner.
    if (g_young_gen.region.load(std::memory_order_acquire) != nullptr) {
        s_young_gen_state.store(2, std::memory_order_release);
    }
    int expected = 0;
    if (s_young_gen_state.compare_exchange_strong(expected, 1,
            std::memory_order_acq_rel, std::memory_order_acquire)) {
        InitYoungGeneration();
        s_young_gen_state.store(2, std::memory_order_release);
    } else if (expected == 1) {
        // Another thread is initializing — spin-wait.
        while (s_young_gen_state.load(std::memory_order_acquire) != 2) {
            CHAOS_IL2CPP_PAUSE_HINT();
        }
    }

    // Use the per-thread adaptive TLAB size (tuned by UpdateTlabSize).
    CHAOS_IL2CPP_SIZE tlab_sz = tls_tlab_size;
    // Clamp to valid range in case UpdateTlabSize produced an extreme value.
    if (tlab_sz < 16 * 1024) tlab_sz = 16 * 1024;
    if (tlab_sz > 256 * 1024) tlab_sz = 256 * 1024;

    // Atomically carve tlab_sz from the shared young region.
    char* bump = g_young_gen.bump.load(std::memory_order_acquire);
    while (bump != nullptr) {
        char* next_bump = bump + tlab_sz;
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

TLAB ClaimEmergencyTlab() noexcept {
    auto tlab_sz = (std::min)(static_cast<CHAOS_IL2CPP_SIZE>(kDefaultTlabSize),
                               YoungGeneration::kEmergencyTlabSize / 2);
    char* bump = g_young_gen.emergency_bump.load(std::memory_order_acquire);
    while (bump != nullptr) {
        char* next_bump = bump + tlab_sz;
        char* region_end = g_young_gen.emergency_start + YoungGeneration::kEmergencyTlabSize;
        if (next_bump > region_end) {
            return TLAB{};
        }
        if (g_young_gen.emergency_bump.compare_exchange_weak(bump, next_bump,
                std::memory_order_acq_rel, std::memory_order_acquire)) {
            TLAB tlab;
            tlab.start = bump;
            tlab.current = bump;
            tlab.end = next_bump;
            tlab.start_scan = nullptr;
            tlab.current_scan = nullptr;
            return tlab;
        }
    }
    return TLAB{};
}

Region* RegionManager::AllocateRegion(RegionKind kind, CHAOS_IL2CPP_SIZE min_size,
                                       CHAOS_IL2CPP_UINT32 domain_id) {
    GcSpinLockGuard lock(mutex_);

    CHAOS_IL2CPP_SIZE region_size = kDefaultRegionSize;
    switch (kind) {
    case RegionKind::REGION_NURSERY: region_size = kDefaultRegionSize; break;
    case RegionKind::REGION_GEN1:    region_size = kDefaultYoungRegionSize; break;
    case RegionKind::REGION_TENURED: region_size = SelectRegionSize(kind, min_size); break;
    case RegionKind::REGION_DOMAIN:  region_size = kDomainRegionSize;  break;
    case RegionKind::REGION_POH:     region_size = kPohRegionSize;     break;
    default:                  region_size = SelectRegionSize(kind, min_size); break;
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
        r->poh_next = nullptr;
        r->id = static_cast<RegionId>(region_count_ + 1);
        r->table_slot = -1;  // recycled region: not in region_table_, managed via free-list
        // Do NOT double-count: this region was already counted when first allocated.

        // If recycled nursery, re-publish to the lock-free range array.
        if (kind == RegionKind::REGION_NURSERY) {
            AddNurseryRange(reinterpret_cast<uintptr_t>(r->begin),
                            reinterpret_cast<uintptr_t>(r->end));
        }
        if (kind == RegionKind::REGION_GEN1) {
            g_young_gen.gen1_region.store(r, std::memory_order_release);
            g_young_gen.gen1_end = r->end;
            g_young_gen.gen1_bump.store(r->begin, std::memory_order_release);
        }
        if (kind == RegionKind::REGION_DOMAIN) {
            AddDomainRange(domain_id,
                reinterpret_cast<uintptr_t>(r->begin),
                reinterpret_cast<uintptr_t>(r->end));
        }
        if (kind == RegionKind::REGION_POH) {
            AddPohRange(reinterpret_cast<uintptr_t>(r->begin),
                         reinterpret_cast<uintptr_t>(r->end));
            r->poh_next = poh_region_list_;
            poh_region_list_ = r;
            poh_region_count_++;
        }
        return r;
    }

    // Allocate region memory directly via VirtualAlloc so regions are
    // separate from the old gen's address space (avoids nursery/old-gen overlap).
    void* mem = nullptr;
    {
        CHAOS_IL2CPP_SIZE alloc_size = sizeof(Region) + region_size;
#if defined(CHAOS_IL2CPP_GC_LARGE_PAGES) && CHAOS_IL2CPP_GC_LARGE_PAGES == 1
        // Try large pages for regions >= large page minimum (typically 2MB).
        // Most regions (512KB nursery, 64KB POH/domain) are below this
        // threshold; only large tenured or custom-size regions benefit.
        if (alloc_size >= GcGetLargePageMinimum()) {
            mem = GcTryAllocLargePages(alloc_size);
        }
        if (mem == nullptr)  // fallback to normal pages
#endif
        {
            mem = chaos::il2cpp::pal::PalVirtualAlloc(alloc_size);
        }
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
        G_OldGen().Free(mem);
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

    // If this is a POH region, publish to the POH range array
    // and prepend to the POH linked list for O(1) iteration.
    if (kind == RegionKind::REGION_POH) {
        AddPohRange(reinterpret_cast<uintptr_t>(r->begin),
                     reinterpret_cast<uintptr_t>(r->end));
        r->poh_next = poh_region_list_;
        poh_region_list_ = r;
        poh_region_count_++;
    }

    // If this is a DOMAIN region, publish to the lock-free domain range cache.
    if (kind == RegionKind::REGION_DOMAIN) {
        AddDomainRange(domain_id,
            reinterpret_cast<uintptr_t>(r->begin),
            reinterpret_cast<uintptr_t>(r->end));
    }

    // Initialize the region's generation.  Nursery AND Gen1
    // (survivor) are YOUNG-side regions — young GC scans them precisely
    // (Phase 2 covers nursery; Gen1's cross-gen refs are rescanned), so the
    // write barrier must treat them as gen0 (skip card: contents are scanned
    // wholesale).  Everything mature (tenured/LOH/Domain/POH) → old(2).
    // Keep the skewed region→gen table in sync for O(1) write-barrier lookups.
    //
    // M9-A1 (3-gen): GEN1 now gets a DISTINCT generation value (kRegionGenGen1=1)
    // instead of sharing kRegionGenYoung(0) with the nursery.  The write barrier
    // still treats both gen0 and gen1 as "young" for the wholesale-scan skip (via
    // `dst_gen <= kRegionGenGen1`), so this tag makes gen1's identity observable
    // without changing the young-scan semantics.
    uint8_t region_gen = (kind == RegionKind::REGION_NURSERY)  ? kRegionGenYoung
                       : (kind == RegionKind::REGION_GEN1)     ? kRegionGenGen1
                       : kRegionGenOld;
    r->gen = region_gen & kRegionGenMask;
    // Mark EVERY 4MB region-gen chunk the region spans (not just begin).  A
    // nursery/gen1 region is large (e.g. 64MB = 16 chunks); without this the
    // chunks beyond begin default to OLD(2), so a young object in the middle/end
    // of the nursery reads region-gen 2 → the write barrier treats it as mature
    // and skips carding a young→old edge, and a gen>condemned filter drops it.
    // This is the CoreCLR set_region_gen_num per-segment marking analog.
    EnsureRegionGenCoverage(reinterpret_cast<uintptr_t>(r->begin));
    EnsureRegionGenCoverage(reinterpret_cast<uintptr_t>(r->end) - 1);
    constexpr CHAOS_IL2CPP_SIZE kRgSize = CHAOS_IL2CPP_SIZE{1} << kRegionGenShift;
    uintptr_t a = reinterpret_cast<uintptr_t>(r->begin) & ~(kRgSize - 1);
    for (; a < reinterpret_cast<uintptr_t>(r->end); a += kRgSize) {
        SetRegionGen(a, region_gen);
    }

    total_allocated_bytes_.fetch_add(region_size, std::memory_order_relaxed);
    return r;
}

static constexpr int kFreeListTrimThreshold = 16;

void RegionManager::FreeRegion(RegionId id) {
    if (id == kRegionIdInvalid) return;
    GcSpinLockGuard lock(mutex_);

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

    // If this is the Gen1 region, clear the published pointer.
    if (r->kind == RegionKind::REGION_GEN1) {
        g_young_gen.gen1_region.store(nullptr, std::memory_order_release);
        g_young_gen.gen1_end = nullptr;
        g_young_gen.gen1_bump.store(nullptr, std::memory_order_release);
    }

    // Re-tag the freed region's 4MB region-gen chunks back to OLD.  The skewed
    // region→gen table is per-4MB-chunk (monotonic: a chunk tagged young/gen1
    // stays so until overwritten).  If this freed Gen1/Nursery region's chunks
    // kept their YOUNG/GEN1 tag, an old-gen page later allocated into that
    // address range would read GetRegionGen == young — misclassifying genuine
    // old-gen objects (dropping old→nursery cards, failing region-gen assertions;
    // GC-N6 mode3 gen1_resize exposes this when Gen1 shrinks).  Tag them OLD now
    // that the region no longer owns them.  (LOH/old-gen pages already tag chunks
    // OLD via GcMarkRangeOld at their own allocation/free.)
    if (r->kind == RegionKind::REGION_GEN1 ||
        r->kind == RegionKind::REGION_NURSERY) {
        GcMarkRangeOld(reinterpret_cast<uintptr_t>(r->begin),
                       reinterpret_cast<uintptr_t>(r->end));
    }

    // If this is a POH region, remove its range and unlink from POH list.
    if (r->kind == RegionKind::REGION_POH) {
        RemovePohRange(reinterpret_cast<uintptr_t>(r->begin),
                       reinterpret_cast<uintptr_t>(r->end));
        // Unlink from POH singly-linked list (O(n) but FreeRegion is rare).
        if (poh_region_list_ == r) {
            poh_region_list_ = r->poh_next;
        } else {
            Region* prev = poh_region_list_;
            while (prev != nullptr && prev->poh_next != r) {
                prev = prev->poh_next;
            }
            if (prev != nullptr) {
                prev->poh_next = r->poh_next;
            }
        }
        r->poh_next = nullptr;
        poh_region_count_--;
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
    GcSpinLockGuard lock(mutex_);

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

        // If this is a POH-kind region, remove its range and unlink from list.
        if (r->kind == RegionKind::REGION_POH) {
            RemovePohRange(reinterpret_cast<uintptr_t>(r->begin),
                           reinterpret_cast<uintptr_t>(r->end));
            if (poh_region_list_ == r) {
                poh_region_list_ = r->poh_next;
            } else {
                Region* prev = poh_region_list_;
                while (prev != nullptr && prev->poh_next != r) {
                    prev = prev->poh_next;
                }
                if (prev != nullptr) {
                    prev->poh_next = r->poh_next;
                }
            }
            r->poh_next = nullptr;
            poh_region_count_--;
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
    GcVerifyPromotedTracked(result);   // HeapVerify kFull: promoted objects in tracked old-gen

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

Region* RegionManager::GetFirstPohRegion() const {
    // O(1): returns cached POH list head instead of O(R) region table scan.
    // Mutex ensures caller sees a consistent list (mutators may be allocating).
    GcSpinLockGuard lock(mutex_);
    return poh_region_list_;
}

int RegionManager::GetPohRegionCount() const {
    // O(1): returns cached count instead of O(R) region table scan.
    GcSpinLockGuard lock(mutex_);
    return poh_region_count_;
}

Region* RegionManager::GetNextPohRegion(const Region* current) const {
    // O(1): follows poh_next pointer instead of scanning the region table.
    if (current == nullptr) return nullptr;
    GcSpinLockGuard lock(mutex_);
    return current->poh_next;
}

// 🔔 P2-3: RegionAllocate removed (dead code — callers use NurseryAllocate,
// G_OldGen().Allocate, or DomainAllocate directly).

// ======================================================================
// Extern "C" API — called from managed code via codegen inline body
// ======================================================================

/// Trigger a full blocking GC collection from managed code (System.GC.Collect()).
/// 1. Runs young collection on the calling thread's nursery (if any).
/// 2. Runs full old-gen mark-sweep (STW safepoint).
/// 3. Runs pending finalizers.
extern "C" void chaos_gc_collect() noexcept {
#if CHAOS_IL2CPP_GC_SERVER
    GcCoordinator::Instance().RequestGlobalGc();
#else
    CHAOS_IL2CPP_LOG_DEBUG("CRAG", "chaos_gc_collect requested");
    GcVerifyHeap();   // entry self-check (HeapVerify level > 0)

    // Step 1: Young collection on the shared young generation (if any).
    // Uses g_young_gen.bump to determine if there are live nursery objects,
    // not region->current (which is frozen at begin after each young GC reset).
    Region* young_region = g_young_gen.region.load(std::memory_order_acquire);
    void* bump = g_young_gen.bump.load(std::memory_order_acquire);
    if (young_region != nullptr && bump > young_region->begin) {
        uint32_t gen = threading::RequestGlobalSafepoint();
        GcYoungCollection(true);  // force_skip_gen1=true — Gen1 handled by step 1.5
        threading::ReleaseGlobalSafepoint(gen);
    }

    // Step 1.5: Gen1 collection (if Gen1 has objects).
    // Collects Gen1 and promotes live objects to Gen2.
    Region* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    if (gen1 != nullptr) {
        char* s_bump = g_young_gen.gen1_bump.load(std::memory_order_acquire);
        if (s_bump > gen1->begin) {
            uint32_t gen = threading::RequestGlobalSafepoint();
            auto gen1_result = GcGen1Collection();
            threading::ReleaseGlobalSafepoint(gen);
            GcRecordGen1Collection(
                gen1_result.objects_promoted,
                gen1_result.bytes_promoted,
                gen1_result.bytes_reclaimed,
                gen1_result.pause_ns);
            G_Scheduler().RecordGen1Collection(
                gen1_result.bytes_promoted,
                gen1_result.objects_in_gen1,
                gen1_result.pause_ns);
        }
    }

    // Step 2: Full old-gen collection (mark-sweep, potentially parallel).
    {
        uint32_t gen = threading::RequestGlobalSafepoint();
        G_OldGen().Collect(nullptr, nullptr);
        threading::ReleaseGlobalSafepoint(gen);
    }

    // Step 3: Run pending finalizers.
    G_OldGen().RunFinalizers();

    GcVerifyHeap();   // exit self-check (HeapVerify level > 0)

#endif  // CHAOS_IL2CPP_GC_SERVER
}

/// Wait for pending finalizers (System.GC.WaitForPendingFinalizers()).
extern "C" void chaos_gc_wait_for_pending_finalizers() noexcept {
    CHAOS_IL2CPP_LOG_DEBUG("CRAG", "chaos_gc_wait_for_pending_finalizers");
    G_OldGen().RunFinalizers();
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
    CHAOS_IL2CPP_LOG_DEBUG_M("CRAG", "chaos_gc_allocate_pinned size={0}",
        static_cast<unsigned long long>(size));
    return GcAllocatePinned(size);
}

// chaos_gc_get_total_memory, chaos_gc_add_memory_pressure, and
// chaos_gc_remove_memory_pressure are now defined in gc_api.cpp
// (declared in gc_api.h).

// ── TLS force-reference ────────────────────────────────────────────
// MSVC optimizes out unreferenced namespace-scope thread_local variables
// from the .obj, even with extern linkage. This function forces emission
// of tls_alloc_fast_count and tls_alloc_fast_bytes so that other
// translation units linking against chaos_runtime_core.lib can resolve
// the extern thread_local declarations in gc_alloc_stubs.h.
extern "C" CHAOS_IL2CPP_SIZE chaos_gc_tls_force_ref() noexcept {
    return tls_alloc_fast_count + tls_alloc_fast_bytes;
}

}  // namespace chaos::il2cpp::runtime_core