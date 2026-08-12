#include "gc_bgc.h"
#include "gc_bgc_inline.h"

#include <chaos/log.h>
#include <chaos/native_types.h>
#include <chaos/asan_interface.h>

#include "gc_events.h"
#include "gc_etw.h"
#include "gc_gen1.h"
#include "gc_layout.h"
#include "gc_loh.h"
#include "gc_config.h"
#include "gc_numa.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_root_change.h"
#include "gc_young_gen.h"
#include "gc_heap.h"
#include "thread_state.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <thread>

namespace chaos::il2cpp::runtime_core {

// Default: BGC enabled.  Verification/benchmark entrypoints may set to false
// before RuntimeInit() to disable BGC for short-lived processes where the
// concurrency race is not desired.
bool g_bgc_enabled = true;

// ── Global state ─────────────────────────────────────────────────────

thread_local int tls_satb_buffer_index = -1;
thread_local bool tls_satb_registered = false;
std::atomic<bool> g_bgc_is_marking{false};

/// Non-inline SATB write barrier wrapper for jit_helpers.cpp.
/// Compiled directly into test targets (MSVC-format .obj) to avoid
/// __tls_index issues with MSVC link.exe resolving TLS from GNU ar archives.
extern "C" void JitSatbPreWriteBarrier(void** slot) noexcept {
    BgcSatbPreWriteBarrier(slot);
}

void BgcFlushSatbBuffer(const SatbEntry* entries, uint32_t count) {
    BgcController::Instance().FlushSatbBuffer(entries, count);
}

// ======================================================================
// BgcController implementation
// ======================================================================

BgcController::~BgcController() {
    Stop();
}

void BgcController::Start() {
    if (bgc_running_.exchange(true, std::memory_order_acq_rel))
        return;
    bgc_thread_started_.store(false, std::memory_order_release);
    bgc_thread_ = std::thread(&BgcController::BgcThreadMain, this);

    // Start dedicated finalizer thread.
    if (!finalizer_running_.exchange(true, std::memory_order_acq_rel)) {
        finalizer_thread_started_.store(false, std::memory_order_release);
        finalizer_thread_ = std::thread(&BgcController::FinalizerThreadMain, this);
    }

    // Wait for both threads to complete their startup (RegisterThread +
    // EnterPreemptiveMode) before returning.  Without this barrier,
    // concurrent dispatch (benchmark loops, fact-json) races with thread
    // initialization, causing sporadic segfaults (~20-40% failure rate).
    while (!bgc_thread_started_.load(std::memory_order_acquire)) {
        std::this_thread::yield();
    }
    while (!finalizer_thread_started_.load(std::memory_order_acquire)) {
        std::this_thread::yield();
    }
}

void BgcController::Stop() {
    // Step 1: Stop BGC thread.
    if (bgc_running_.exchange(false, std::memory_order_acq_rel)) {
        bgc_start_requested_.store(true, std::memory_order_release);
        NotifyBgc();
        if (bgc_thread_.joinable())
            bgc_thread_.join();
    }

    // Step 2: Stop finalizer thread (after BGC — any pending work was published).
    if (finalizer_running_.exchange(false, std::memory_order_acq_rel)) {
        finalizer_cv_.notify_one();
        if (finalizer_thread_.joinable())
            finalizer_thread_.join();
    }
}

void BgcController::FlushSatbBuffer(const SatbEntry* entries, uint32_t count) {
    if (count == 0) return;
    {
        std::lock_guard<std::mutex> lock(global_satb_mutex_);
        global_satb_.insert(global_satb_.end(), entries, entries + count);
    }
    // New SATB entries mean new work for concurrent mark phase.
    NotifyBgc();
}

void BgcController::RegisterThreadSatbBuffer(SatbThreadBuffer* buf) {
    if (buf == nullptr) return;
    std::lock_guard<std::mutex> lock(satb_registry_mutex_);

    // Check if already registered.
    for (int i = 0; i < registered_satb_count_; i++) {
        if (registered_satb_buffers_[i] == buf) return;
    }

    if (registered_satb_count_ < kMaxSatbThreads) {
        registered_satb_buffers_[registered_satb_count_++] = buf;
    }
}

int BgcController::AllocateSatbBuffer() {
    int idx = satb_pool_alloc_.fetch_add(1, std::memory_order_relaxed);
    if (idx >= kMaxSatbPool) {
        // Pool exhausted — reset alloc and trigger emergency full GC fallback.
        satb_pool_alloc_.fetch_sub(1, std::memory_order_relaxed);
        CHAOS_IL2CPP_LOG_ERROR("BGC", "satb_pool_exhausted — requesting emergency full GC");
        G_Scheduler().RequestFullGc();
        return -1;
    }
    return idx;
}

// ── BGC cycle control ────────────────────────────────────────────────

void BgcController::StartBgcCycle() {
    // Guard against concurrent BGC start attempts.  Without this guard,
    // multiple threads calling StartBgcCycle simultaneously (e.g., when
    // 100 threads exhaust their nursery at the same time) will each call
    // PopulateRootSet() and NotifyBgc(), corrupting the root set and
    // creating duplicate concurrent mark threads.
    BgcPhase expected = BgcPhase::IDLE;
    if (!phase_.compare_exchange_strong(expected, BgcPhase::ROOT_COLLECT,
            std::memory_order_acq_rel, std::memory_order_acquire)) {
        CHAOS_IL2CPP_LOG_DEBUG("BGC", "start_cycle_skipped");
        return;
    }

    // Pre-allocate Gen1 mark bitmap BEFORE PopulateRootSet.
    // CHAOS_IL2CPP_MALLOC (raw malloc) does not need a safepoint, so this
    // is safe to run outside the safepoint region.  If allocation fails,
    // we fall back to GEN2_ONLY before doing any real work (root scanning),
    // avoiding wasted effort and the need for OOM handling during the mark
    // phase.  The BGC scope was already set by DecideBgcScope() in the
    // caller (GcAdvanceBgcCycle), so G_Scheduler().GetBgcScope() is current.
    if (G_Scheduler().GetBgcScope() == BgcScope::GEN1_GEN2) {
        if (!AllocateGen1MarkBitmap()) {
            CHAOS_IL2CPP_LOG_WARN("BGC",
                "gen1_bitmap OOM — falling back to GEN2_ONLY");
        }
    } else {
        FreeGen1MarkBitmap();  // Clean up any leftover from prior cycle.
    }

    // Acquire safepoint before PopulateRootSet.  Root scanning
    // (GcScanAllThreadRoots + TLS nursery enumeration) reads thread
    // stacks and nursery regions — doing this while threads are
    // actively modifying them yields an inconsistent root set that
    // can cause the concurrent mark loop to chase garbage pointers
    // indefinitely (the "BGC hang").
    CHAOS_IL2CPP_LOG_DEBUG("BGC", "start_cycle");
    uint32_t bgc_gen = threading::RequestGlobalSafepoint();
    PopulateRootSet();
    threading::ReleaseGlobalSafepoint(bgc_gen);
    CHAOS_IL2CPP_LOG_DEBUG("BGC", "root_set_populated");

    // Transition to concurrent mark phase.
    phase_.store(BgcPhase::CONCURRENT_MARK, std::memory_order_release);
    g_bgc_is_marking.store(true, std::memory_order_release);
    cycle_complete_.store(false, std::memory_order_release);
    bgc_start_requested_.store(true, std::memory_order_release);
    NotifyBgc();

    CHAOS_IL2CPP_LOG_DEBUG("BGC", "concurrent_mark_started");
}

CHAOS_IL2CPP_SIZE BgcController::StwRemark() {
    // Must be called under safepoint.
    CHAOS_IL2CPP_LOG_DEBUG("BGC", "stw_remark_start");

    // Drain all thread-local SATB buffers.
    DrainAllTlsSatbBuffers();

    // Process global SATB queue.
    DrainGlobalSatbQueue();

    // Drain mark stack to completion.
    CHAOS_IL2CPP_SIZE marked = DrainWorkerDeque(0, 0);

    // Drain all threads' root change buffers (G-25).
    // During concurrent mark, mutators may have overwritten root slots
    // (statics, GCHandles).  Re-mark any old-gen objects that were the
    // old value in those slots — they may have lost their last reference.
    threading::EnumerateThreads([](threading::ManagedThread* mt) {
        BgcDrainRootChangeBuffer(mt, [](void* obj) -> bool {
            if (G_OldGen().BgcTryMark(obj)) {
                BgcController::Instance().PushToBgcMarkDeque(obj);
                return true;
            }
            return false;
        });
        return true;  // continue enumeration
    });

    // Scan dirty cards: any old-gen pages may have new cross-gen references
    // from concurrent mark phase that aren't captured by SATB alone
    // (e.g., a nursery object allocated during concurrent mark that points
    // to an unmarked old-gen object via a new field write).
    // M5 (two-snapshot): clear each scanned card as it is consumed ("clear-as-
    // you-scan"), so the remark is idempotent — a later young GC / next remark
    // does not re-mark the same old-gen refs from a consumed card.
    CHAOS_IL2CPP_SIZE cards_dirty = 0;
    G_OldGen().ScanDirtyCardsInPages(
        [&](uintptr_t /*card_idx*/, uintptr_t card_start, uintptr_t card_end) {
            cards_dirty++;
            // Scan every pointer slot in the dirty card range for old-gen refs.
            // Skip any addresses that fall in the Gen1 range (separate region,
            // handled by its own re-scan below; this guard is belt-and-suspenders).
            for (auto* slot = reinterpret_cast<void**>(card_start);
                 slot < reinterpret_cast<void**>(card_end);
                 slot++) {
                void* ref = *slot;
                if (ref != nullptr && G_OldGen().IsInOldGen(ref)) {
                    if (G_OldGen().BgcTryMark(ref)) {
                        std::lock_guard<std::mutex> lock(
                            BgcController::Instance().bgc_workers_[0].steal_mutex);
                        BgcController::Instance().bgc_workers_[0].deque.push_back(ref);
                    }
                }
            }
            // Consume the card (two-snapshot: clear-as-scan).
            ClearCard(reinterpret_cast<const void*>(card_start));
        });

    // Drain again after dirty cards.
    marked += DrainWorkerDeque(0, 0);

    // ── Gen1 re-scan via card table ──────────────────────────
    // The Gen1 region is registered with the card table
    // via GcRegisterHeapRange, so DirtyCard() write barrier tracks
    // all pointer writes into Gen1 objects during concurrent mark.
    // Scan only dirty cards in the Gen1 region for efficiency,
    // replacing the prior full-walk re-scan.
    {
        Region* gen1 = G_YoungGen().gen1_region.load(std::memory_order_acquire);
        if (gen1 != nullptr) {
            char* sv_begin = gen1->begin;
            auto* sv_bump = G_YoungGen().gen1_bump.load(std::memory_order_acquire);
            if (sv_bump > sv_begin) {
                auto& ctrl = BgcController::Instance();
                ScanDirtyCards(
                    reinterpret_cast<uintptr_t>(sv_begin),
                    reinterpret_cast<uintptr_t>(sv_bump),
                    [&](uintptr_t /*card_idx*/, uintptr_t card_start, uintptr_t card_end) {
                        for (auto* slot = reinterpret_cast<void**>(card_start);
                             slot < reinterpret_cast<void**>(card_end);
                             slot++) {
                            void* ref = *slot;
                            if (ref != nullptr && G_OldGen().IsInOldGen(ref)) {
                                if (G_OldGen().BgcTryMark(ref)) {
                                    std::lock_guard<std::mutex> lock(
                                        ctrl.bgc_workers_[0].steal_mutex);
                                    ctrl.bgc_workers_[0].deque.push_back(ref);
                                }
                            }
                        }
                        // Consume the Gen1 card (two-snapshot clear-as-scan).
                        ClearCard(reinterpret_cast<const void*>(card_start));
                    });
            }
        }
    }

    // Drain again after Gen1 re-scan.
    marked += DrainWorkerDeque(0, 0);

    CHAOS_IL2CPP_LOG_DEBUG_M("BGC", "stw_remark_done marked={0} cards={1}",
        static_cast<unsigned long long>(marked),
        static_cast<unsigned long long>(cards_dirty));

    return marked;
}

void BgcController::StartConcurrentSweep() {
    phase_.store(BgcPhase::CONCURRENT_SWEEP, std::memory_order_release);
    bgc_start_requested_.store(true, std::memory_order_release);
    NotifyBgc();
    CHAOS_IL2CPP_LOG_DEBUG("BGC", "concurrent_sweep_started");
}

void BgcController::StwCompact() {
    // Must be called under safepoint.
    CHAOS_IL2CPP_LOG_DEBUG("BGC", "stw_compact_start");

    // Run compaction using the mark bitmap left intact by BgcSweep().
    // BgcCompact handles DecideCompactMode + compaction + bitmap clear.
    G_OldGen().BgcCompact();

    // ── Gen1 promote/keep decision (GEN1_GEN2 scope) ─────────────
    // After BGC concurrent mark + sweep, walk Gen1 using the BGC Gen1
    // mark bitmap.  If Gen1 survival is low (< 30%), promote live objects
    // to Gen2 and reset gen1_bump (Gen1 filtering was effective).  If
    // survival is high, leave Gen1 for the young GC to collect later.
    if (gen1_mark_bitmap_ != nullptr) {
        Region* gen1 = G_YoungGen().gen1_region.load(std::memory_order_acquire);
        char* sv_begin = (gen1 != nullptr) ? gen1->begin : nullptr;
        char* sv_bump  = G_YoungGen().gen1_bump.load(std::memory_order_acquire);
        if (sv_begin != nullptr && sv_bump > sv_begin) {
            GcEtwFireGcGen1Collect(0, 0, 0);  // placeholder — actual stats recorded in GcGen1Collection
            GcFireEvent(GcEvent::GC_GEN1_COLLECT);

            uintptr_t gen1_base = gen1_bitmap_base_;
            char* cur = sv_begin;
            CHAOS_IL2CPP_SIZE live_count = 0;
            CHAOS_IL2CPP_SIZE dead_count = 0;
            CHAOS_IL2CPP_SIZE total_bytes = static_cast<CHAOS_IL2CPP_SIZE>(sv_bump - sv_begin);
            CHAOS_IL2CPP_SIZE live_bytes = 0;
            CHAOS_IL2CPP_SIZE total_objects = 0;

            while (cur < sv_bump) {
                CHAOS_IL2CPP_SIZE obj_size = kGen1MaxEstObjectSize;
                const void* pti = *reinterpret_cast<const void* const*>(cur);
                if (pti != nullptr) {
                    auto& layout = GcLayoutRegistry::Instance();
                    if (layout.IsValidTypeInfoPointer(pti)) {
                        uint64_t sid = layout.ReadStableId(pti);
                        const auto* playout = layout.Lookup(sid);
                        if (playout != nullptr && playout->instance_size > 0) {
                            obj_size = static_cast<CHAOS_IL2CPP_SIZE>(playout->instance_size);
                        }
                    }
                }

                uintptr_t obj_addr = reinterpret_cast<uintptr_t>(cur);
                CHAOS_IL2CPP_SIZE slot_idx = (obj_addr - gen1_base) / sizeof(void*);
                CHAOS_IL2CPP_SIZE byte_idx = slot_idx / 8;
                int bit_off = static_cast<int>(slot_idx % 8);
                bool is_live = (byte_idx < gen1_bitmap_bytes_) &&
                    ((gen1_mark_bitmap_[byte_idx] >> bit_off) & 1u);

                total_objects++;
                if (is_live) {
                    live_count++;
                    live_bytes += obj_size;
                } else {
                    dead_count++;
                }
                cur += obj_size;
            }

            double survival = (total_objects > 0)
                ? static_cast<double>(live_count) / static_cast<double>(total_objects)
                : 0.0;

            CHAOS_IL2CPP_LOG_DEBUG_M("BGC",
                "gen1_sweep: objects={0} live={1} dead={2} live_bytes={3} "
                "total_bytes={4} survival={5:.3f}",
                static_cast<unsigned long long>(total_objects),
                static_cast<unsigned long long>(live_count),
                static_cast<unsigned long long>(dead_count),
                static_cast<unsigned long long>(live_bytes),
                static_cast<unsigned long long>(total_bytes),
                survival);

            // Decision: promote if survival < 30%.
            constexpr double kGen1PromoteThreshold = 0.30;
            if (survival < kGen1PromoteThreshold && live_bytes > 0) {
                auto pause_start = std::chrono::steady_clock::now();
                char* promote_cur = sv_begin;
                CHAOS_IL2CPP_SIZE promoted_bytes = 0;
                CHAOS_IL2CPP_SIZE promoted_objects = 0;
                bool oom = false;

                while (promote_cur < sv_bump) {
                    CHAOS_IL2CPP_SIZE psize = kGen1MaxEstObjectSize;
                    const void* pti = *reinterpret_cast<const void* const*>(promote_cur);
                    if (pti != nullptr) {
                        auto& playout = GcLayoutRegistry::Instance();
                        if (playout.IsValidTypeInfoPointer(pti)) {
                            uint64_t psid = playout.ReadStableId(pti);
                            const auto* pl = playout.Lookup(psid);
                            if (pl != nullptr && pl->instance_size > 0) {
                                psize = static_cast<CHAOS_IL2CPP_SIZE>(pl->instance_size);
                            }
                        }
                    }

                    uintptr_t paddr = reinterpret_cast<uintptr_t>(promote_cur);
                    CHAOS_IL2CPP_SIZE pslot = (paddr - gen1_base) / sizeof(void*);
                    CHAOS_IL2CPP_SIZE pbyte = pslot / 8;
                    int pbit = static_cast<int>(pslot % 8);
                    bool plive = (pbyte < gen1_bitmap_bytes_) &&
                        ((gen1_mark_bitmap_[pbyte] >> pbit) & 1u);

                    if (plive) {
                        void* gen2_addr = G_OldGen().Allocate(psize, true);
                        if (gen2_addr != nullptr) {
                            std::memcpy(gen2_addr, promote_cur, psize);
                            promoted_objects++;
                            promoted_bytes += psize;
                        } else {
                            oom = true;
                            CHAOS_IL2CPP_LOG_ERROR_M("BGC",
                                "gen1_promote OOM at offset={0}",
                                static_cast<unsigned long long>(
                                    promote_cur - sv_begin));
                            break;
                        }
                    }
                    promote_cur += psize;
                }

                if (!oom) {
                    G_YoungGen().gen1_bump.store(
                        gen1->begin, std::memory_order_release);
                    CHAOS_IL2CPP_LOG_DEBUG_M("BGC",
                        "gen1_promote_done: promoted={0} objs, {1} bytes",
                        static_cast<unsigned long long>(promoted_objects),
                        static_cast<unsigned long long>(promoted_bytes));

                    G_Scheduler().RecordGen1Collection(
                        promoted_bytes, total_objects,
                        static_cast<uint64_t>(std::chrono::duration_cast<
                            std::chrono::nanoseconds>(
                                std::chrono::steady_clock::now() - pause_start).count()));
                    G_Scheduler().RecordBgcGen1Promote(promoted_bytes);
                } else {
                    CHAOS_IL2CPP_LOG_WARN("BGC",
                        "gen1_promote OOM — leaving Gen1 intact");
                }
            } else {
                CHAOS_IL2CPP_LOG_DEBUG_M("BGC",
                    "gen1_keep: survival={0:.2f} above threshold",
                    survival);
                G_Scheduler().RecordBgcGen1Keep(
                    static_cast<CHAOS_IL2CPP_SIZE>(sv_bump - sv_begin));
            }
        }
    }

    // Transition to FINISHED, signaling BGC thread to reset to IDLE.
    phase_.store(BgcPhase::FINISHED, std::memory_order_release);
    NotifyBgc();

    CHAOS_IL2CPP_LOG_DEBUG("BGC", "stw_compact_done");
}

void BgcController::WaitForCycleComplete() {
    std::unique_lock<std::mutex> lock(bgc_cv_mutex_);
    bgc_cv_.wait(lock, [this]() { return cycle_complete_.load(std::memory_order_acquire); });
    cycle_complete_.store(false, std::memory_order_release);
}

void BgcController::CollectDeadWeakHandlesForBgc() {
    // Must be called after BgcSweep() while the mark bitmap is still valid.
    // Collects dead weak handles into bgc_dead_weak_handles_.
    // Fast path: skip if no handles are registered (avoids locking corrupted
    // handle table state under extreme stress).
    if (!GcHasAnyHandles()) {
        return;
    }
    std::vector<std::pair<uint64_t, void*>> flat;
    GcCollectDeadWeakHandles(flat);
    for (auto& f : flat) {
        bgc_dead_weak_handles_.push_back({f.first, f.second});
    }
}

void BgcController::ResetForTest() noexcept {
    // Stop parallel workers if any are running.
    StopParallelMarkWorkers();

    // Clear SATB pool.
    for (int i = 0; i < satb_pool_alloc_.load(std::memory_order_acquire); i++) {
        satb_pool_[i].count.store(0, std::memory_order_release);
    }
    satb_pool_alloc_.store(0, std::memory_order_release);

    // Clear global SATB queue.
    {
        std::lock_guard<std::mutex> lock(global_satb_mutex_);
        global_satb_.clear();
    }

    // Clear worker deques.
    for (int i = 0; i < kMaxBgcWorkers; i++) {
        std::lock_guard<std::mutex> lock(bgc_workers_[i].steal_mutex);
        bgc_workers_[i].deque.clear();
    }

    // Clear SATB thread registry.
    for (int i = 0; i < kMaxSatbThreads; i++) {
        registered_satb_buffers_[i] = nullptr;
    }
    registered_satb_count_ = 0;

    // Reset SATB freeze protocol.
    satb_freeze_requested_.store(false, std::memory_order_release);
    satb_freeze_remaining_.store(0, std::memory_order_release);

    // Free Gen1 mark bitmap if allocated.
    if (gen1_mark_bitmap_) {
        FreeGen1MarkBitmap();
    }

    // Reset phase.
    phase_.store(BgcPhase::IDLE, std::memory_order_release);
    cycle_complete_.store(false, std::memory_order_release);
    bgc_parallel_done_.store(false, std::memory_order_release);
}

void BgcController::ForceComplete() {
    // Called under safepoint.  Complete marking and sweep inline.
    auto p = phase_.load(std::memory_order_acquire);
    if (p == BgcPhase::IDLE)
        return;

    // Phase 0: Stop concurrent mark workers before draining or sweeping.
    // Without this, workers may still be processing objects (ScanObjectChildren)
    // while BgcSweep frees them — a use-after-free race detectable as segfault
    // in ForceComplete.  Safe to call even if workers weren't spawned yet.
    StopParallelMarkWorkers();

    // Phase 1: Drain remaining SATB + all workers' deques.
    DrainAllTlsSatbBuffers();
    DrainGlobalSatbQueue();
    for (int i = 0; i < kMaxBgcWorkers; i++) {
        DrainWorkerDeque(i, 0);
    }

    // Phase 2: If concurrent sweep hasn't run yet, run it inline so that
    // dead finalizables and weak handles are properly collected.
    // Without this, objects that died during the BGC cycle would have their
    // finalizers skipped (the BGC main loop skips CONCURRENT_SWEEP phase when
    // ForceComplete jumps straight to FINISHED).
    if (p == BgcPhase::CONCURRENT_MARK || p == BgcPhase::REMARK_NEEDED) {
        G_OldGen().BgcSweep();
        bgc_dead_finalizables_ = G_OldGen().CollectDeadFinalizables();
        bgc_dead_weak_handles_.clear();
        CollectDeadWeakHandlesForBgc();
    }

    // Phase 2b: Run BgcCompact under safepoint to handle deferred page
    // freeing (pages unlinked by BgcSweep Phase 4b but deferred for
    // safe VirtualFree) and mark bitmap clearing.
    // BgcCompact runs under safepoint — no concurrent readers exist.
    G_OldGen().BgcCompact();

    // Phase 3+4: Publish dead finalizables + weak handles to the finalizer
    // thread instead of running them inline.  The finalizer thread processes
    // finalizers first, then nulls weak handles (preserving WeakTrackResurrection
    // ordering) after the safepoint is released.
    if (!bgc_dead_finalizables_.empty() || !bgc_dead_weak_handles_.empty()) {
        CHAOS_IL2CPP_LOG_DEBUG_M("BGC", "force_complete_publish finalizers={0} weak={1}",
            static_cast<unsigned long long>(bgc_dead_finalizables_.size()),
            static_cast<unsigned long long>(bgc_dead_weak_handles_.size()));
        PublishFinalizationWork(bgc_dead_finalizables_, bgc_dead_weak_handles_);
        bgc_dead_finalizables_.clear();
        bgc_dead_weak_handles_.clear();
    }

    // Fully complete the cycle: set IDLE directly so that the call to
    // WaitForCycleComplete() returns with Phase() == IDLE.
    // The BGC thread will also process the IDLE transition when it wakes,
    // but since we're under safepoint it's safe to set both here.
    g_bgc_is_marking.store(false, std::memory_order_release);
    bgc_start_requested_.store(false, std::memory_order_release);
    phase_.store(BgcPhase::IDLE, std::memory_order_release);

    // Replenish the emergency reserve after a forced BGC completion.
    G_OldGen().ReplenishEmergencyReserve();

    cycle_complete_.store(true, std::memory_order_release);
    NotifyBgc();

    // Free Gen1 bitmap — StopConcurrentMark means a full GC is handling
    // all sweeping, so the bitmap is no longer valid.
    FreeGen1MarkBitmap();

    CHAOS_IL2CPP_LOG_DEBUG("BGC", "force_complete");
}

void BgcController::StopConcurrentMark() {
    // Called under safepoint from full GC. Stops BGC concurrent mark
    // without sweeping — the full GC's Collect() handles all sweeping.
    auto p = phase_.load(std::memory_order_acquire);
    if (p == BgcPhase::IDLE || p == BgcPhase::ROOT_COLLECT) return;

    // Drain SATB buffers and work deques so no pending mark work remains.
    DrainAllTlsSatbBuffers();
    DrainGlobalSatbQueue();
    for (int i = 0; i < kMaxBgcWorkers; i++) {
        DrainWorkerDeque(i, 0);
    }

    // Reset BGC to IDLE without sweeping or compacting.
    g_bgc_is_marking.store(false, std::memory_order_release);
    bgc_start_requested_.store(false, std::memory_order_release);
    phase_.store(BgcPhase::IDLE, std::memory_order_release);
    cycle_complete_.store(true, std::memory_order_release);
    NotifyBgc();

    // Free Gen1 bitmap — the full GC handles all sweeping.
    FreeGen1MarkBitmap();

    CHAOS_IL2CPP_LOG_DEBUG("BGC", "stop_concurrent_mark");
}

// ── Root set population (under safepoint) ────────────────────────────

void BgcController::PopulateRootSet() {
    // Phase 1a: Mark pinned roots from MarkSweepOldGen.
    // Pinned roots are registered via G_OldGen().AddPinnedRoot().
    // They are stored in MarkSweepOldGen::pinned_roots_ (private),
    // so we rely on the scheduler/gc_old_gen to expose them.
    // For now, pinned roots are handled via TryMarkRoot in the stack
    // scanning path below, which will mark any old-gen object found.

        // Phase 1b: Scan the shared young generation for old-gen pointers.
    // The young region is scanned [begin, bump) for any reference
    // to old-gen objects; those objects are marked and enqueued for
    // concurrent marking.  Uses G_YoungGen().bump (the true allocation
    // frontier advanced by TLAB claims) rather than region->current
    // (which is frozen at begin after each young GC reset).
    {
        Region* young_region = G_YoungGen().region.load(std::memory_order_acquire);
        if (young_region != nullptr) {
            auto& ctrl = BgcController::Instance();
            void* begin = young_region->begin;
            void* cur   = G_YoungGen().bump.load(std::memory_order_acquire);
            if (cur > begin) {
                for (auto* slot = static_cast<void**>(begin);
                     slot < static_cast<void**>(cur);
                     slot++) {
                    void* ref = *slot;
                    if (ref != nullptr && G_OldGen().IsInOldGen(ref)) {
                        if (G_OldGen().BgcTryMark(ref)) {
                            std::lock_guard<std::mutex> lock(
                                ctrl.bgc_workers_[0].steal_mutex);
                            ctrl.bgc_workers_[0].deque.push_back(ref);
                        }
                    }
                    // GEN1_GEN2 scope: mark live Gen1 references in the BGC
                    // Gen1 bitmap.  No-op when Gen1 bitmap is not allocated.
                    ctrl.BgcTryMarkGen1(ref);
                }
            }
        }
    }

    // Phase 1b2: Scan Gen1 region for Gen2 pointers.
    // Gen1 objects may reference Gen2 objects.  These Gen2 references
    // must be marked by BGC to prevent premature reclamation.
    {
        Region* gen1 = G_YoungGen().gen1_region.load(std::memory_order_acquire);
        if (gen1 != nullptr) {
            char* sv_begin = gen1->begin;
            auto* sv_bump = G_YoungGen().gen1_bump.load(std::memory_order_acquire);
            if (sv_bump > sv_begin) {
                auto& ctrl = BgcController::Instance();
                auto& layout_registry = GcLayoutRegistry::Instance();
                auto* s_cur = sv_begin;
                while (s_cur < sv_bump) {
                    const void* ti = *reinterpret_cast<const void* const*>(s_cur);
                    CHAOS_IL2CPP_SIZE obj_size = kGen1MaxEstObjectSize;
                    if (ti != nullptr && layout_registry.IsValidTypeInfoPointer(ti)) {
                        uint64_t sid = layout_registry.ReadStableId(ti);
                        const auto* layout = layout_registry.Lookup(sid);
                        if (layout != nullptr && layout->instance_size > 0) {
                            obj_size = static_cast<CHAOS_IL2CPP_SIZE>(layout->instance_size);
                            for (uint16_t i = 0; i < layout->pointer_count; i++) {
                                uint16_t off = layout->pointer_offsets[i].offset;
                                void* child = *reinterpret_cast<void**>(s_cur + off);
                                if (child != nullptr && G_OldGen().IsInOldGen(child)) {
                                    if (G_OldGen().BgcTryMark(child)) {
                                        std::lock_guard<std::mutex> lock(
                                            ctrl.bgc_workers_[0].steal_mutex);
                                        ctrl.bgc_workers_[0].deque.push_back(child);
                                    }
                                }
                            }
                        }
                    }
                    s_cur += obj_size;
                }
            }
        }
    }

    // Phase 1c: Scan all thread stacks as conservative roots.
    // This catches old-gen references that live in thread-local stack
    // slots and are NOT in any TLS nursery.
    {
        static int s_gte_heap = 0, s_in_oldgen = 0, s_valid = 0, s_marked = 0;
        s_gte_heap = s_in_oldgen = s_valid = s_marked = 0;
        threading::GcScanAllThreadRoots(
            [](void* root_addr, bool, void*) {
                auto* slot = static_cast<void**>(root_addr);
                void* ref = *slot;
                s_gte_heap++;
                if (ref == nullptr) return;
                if (G_OldGen().IsInOldGen(ref)) {
                    s_in_oldgen++;
                    if (!IsValidManagedObject(ref)) return;
                    s_valid++;
                    if (G_OldGen().BgcTryMark(ref)) {
                        s_marked++;
                        std::lock_guard<std::mutex> lock(
                            BgcController::Instance().bgc_workers_[0].steal_mutex);
                        BgcController::Instance().bgc_workers_[0].deque.push_back(ref);
                    }
                } else if (G_Loh().IsInLOH(ref)) {
                    if (G_Loh().MarkObject(ref)) {
                        std::lock_guard<std::mutex> lock(
                            BgcController::Instance().bgc_workers_[0].steal_mutex);
                        BgcController::Instance().bgc_workers_[0].deque.push_back(ref);
                    }
                }
                // GEN1_GEN2 scope: mark live Gen1 refs from stack roots.
                // No-op when Gen1 bitmap is not allocated.
                BgcController::Instance().BgcTryMarkGen1(ref);
            },
            nullptr);
        CHAOS_IL2CPP_LOG_DEBUG_M("BGC", "root_scan heap={0} oldgen={1} valid={2} marked={3}",
            s_gte_heap, s_in_oldgen, s_valid, s_marked);
    }

    // Phase 1d: Scan GCHandle table (tenured handles only — nursery
    // objects are handled by the young collector).
    {
        GcIterateTenuredHandles(
            [](void* object, void* /*user_data*/) {
                if (object == nullptr) return;
                if (G_OldGen().BgcTryMark(object)) {
                    std::lock_guard<std::mutex> lock(
                        BgcController::Instance().bgc_workers_[0].steal_mutex);
                    BgcController::Instance().bgc_workers_[0].deque.push_back(object);
                } else if (G_Loh().IsInLOH(object)) {
                    if (G_Loh().MarkObject(object)) {
                        std::lock_guard<std::mutex> lock(
                            BgcController::Instance().bgc_workers_[0].steal_mutex);
                        BgcController::Instance().bgc_workers_[0].deque.push_back(object);
                    }
                }
                // GEN1_GEN2 scope: mark Gen1-referenced handles in the
                // BGC Gen1 bitmap.  No-op when bitmap is not allocated.
                BgcController::Instance().BgcTryMarkGen1(object);
            },
            nullptr);
    }

    // Phase 1e: Scan POH regions for Gen2 pointers.
    // POH objects bypass young GC (never copied), so their references
    // to old-gen objects must be captured during BGC root scanning.
    // Without this, a POH object holding the only reference to a Gen2
    // object would cause that Gen2 object to be incorrectly swept.
    {
        auto& rm = RegionManager::Instance();
        int poh_count = rm.GetPohRegionCount();
        if (poh_count > 0) {
            auto& ctrl = BgcController::Instance();
            auto& layout_registry = GcLayoutRegistry::Instance();
            for (Region* r = rm.GetFirstPohRegion(); r != nullptr;
                 r = rm.GetNextPohRegion(r)) {
                if (r->current <= r->begin) continue;
                char* poh_cur = r->begin;
                while (poh_cur < r->current) {
                    const void* ti = *reinterpret_cast<const void* const*>(poh_cur);
                    if (ti != nullptr && layout_registry.IsValidTypeInfoPointer(ti)) {
                        uint64_t sid = layout_registry.ReadStableId(ti);
                        const auto* layout = layout_registry.Lookup(sid);
                        if (layout != nullptr && layout->instance_size > 0) {
                            CHAOS_IL2CPP_SIZE obj_size = static_cast<CHAOS_IL2CPP_SIZE>(
                                layout->instance_size);
                            for (uint16_t i = 0; i < layout->pointer_count; i++) {
                                uint16_t off = layout->pointer_offsets[i].offset;
                                void* child = *reinterpret_cast<void**>(poh_cur + off);
                                if (child != nullptr && G_OldGen().IsInOldGen(child)) {
                                    if (G_OldGen().BgcTryMark(child)) {
                                        std::lock_guard<std::mutex> lock(
                                            ctrl.bgc_workers_[0].steal_mutex);
                                        ctrl.bgc_workers_[0].deque.push_back(child);
                                    }
                                }
                            }
                            poh_cur += obj_size;
                            continue;
                        }
                    }
                    // Conservative fallback: scan all pointer-aligned slots.
                    for (uintptr_t off = 0; off + sizeof(void*) <=
                         static_cast<uintptr_t>(r->end - poh_cur);
                         off += sizeof(void*)) {
                        void* child = *reinterpret_cast<void**>(poh_cur + off);
                        if (child != nullptr && G_OldGen().IsInOldGen(child)) {
                            if (G_OldGen().BgcTryMark(child)) {
                                std::lock_guard<std::mutex> lock(
                                    ctrl.bgc_workers_[0].steal_mutex);
                                ctrl.bgc_workers_[0].deque.push_back(child);
                            }
                        }
                    }
                    break;
                }
            }
        }
    }

    // Phase 1f: Process initial mark stack to build transitive root closure.
    // This runs under safepoint, so it's fast (no concurrent interference).
    DrainWorkerDeque(0, 0);
}

// ── BGC thread main ──────────────────────────────────────────────────

void BgcController::BgcThreadMain() {
    CHAOS_IL2CPP_LOG_DEBUG("BGC", "thread_started");

    // Register as a managed thread in preemptive mode.
    // This makes the BGC thread visible to EnumerateThreads for diagnostics
    // and allows the safepoint initiator to wait for BGC to acknowledge
    // before performing STW work (e.g., ForceComplete drain).
    // Preemptive mode means BGC won't be blocked spinning during safepoints.
    int bgc_thread_id = threading::AllocateThreadId();
    threading::RegisterThread(bgc_thread_id, nullptr);
    threading::EnterPreemptiveMode();

    // Signal Start() that BGC thread startup is complete.
    bgc_thread_started_.store(true, std::memory_order_release);

    while (bgc_running_.load(std::memory_order_acquire)) {
        // ── Phase-independent BGC-YoungGC pause interception (GC fix) ──
        // The inner CONCURRENT_MARK loop also acks bgc_pause_requested_, but
        // that check only runs while the BGC thread is actively marking.  If a
        // young GC requests a pause while the BGC thread is between phases
        // (waiting for start, in sweep/compact, etc.), it would never be acked
        // and young GC would spin on bgc_paused_ forever (or race old-gen
        // pages).  Ack it here at the top of EVERY outer iteration so the
        // pause is honored regardless of which phase the BGC is in.
        if (bgc_pause_requested_.load(std::memory_order_acquire)) {
            bgc_paused_.store(true, std::memory_order_release);
            while (bgc_pause_requested_.load(std::memory_order_acquire)) {
                std::this_thread::yield();
            }
            bgc_paused_.store(false, std::memory_order_release);
            CHAOS_IL2CPP_LOG_DEBUG("BGC", "young_gc_pause_released(phase-independent)");
        }

        // Wait for a start request.  Uses condition_variable for event-driven
        // wake-up (P1-4: replaces sleep_for polling).
        {
            std::unique_lock<std::mutex> lock(bgc_cv_mutex_);
            bgc_cv_.wait(lock, [this]() {
                return bgc_start_requested_.load(std::memory_order_acquire) ||
                       !bgc_running_.load(std::memory_order_acquire);
            });
        }
        if (!bgc_start_requested_.load(std::memory_order_acquire))
            continue;

        // ── Phase 2: Concurrent Mark ──────────────────────────────
        // The root set was already populated by StartBgcCycle under
        // safepoint.  Now trace transitively while mutators run.
        // ──────────────────────────────────────────────────────────

        if (phase_.load(std::memory_order_acquire) == BgcPhase::CONCURRENT_MARK) {
            CHAOS_IL2CPP_LOG_DEBUG("BGC", "concurrent_mark_begin");

            // Spawn parallel workers for mark stack processing.
            // Uses per-worker deques with work-stealing (P1-1):
            // - Worker 0 = BGC thread (coordinator)
            // - Workers 1..N = parallel mark workers with steal support
            int n_workers = SpawnParallelMarkWorkers();
            CHAOS_IL2CPP_LOG_DEBUG_M("BGC", "parallel_workers spawned={0}",
                static_cast<unsigned>(n_workers));

            constexpr CHAOS_IL2CPP_SIZE kBatchSize = 64;
            int idle_rounds = 0;
            bool freeze_initiated = false;
            auto slice_start = std::chrono::steady_clock::now();

            while (true) {
                // ── BGC-YoungGC coordinated pause (G-3) ──
                // Young GC requests pause via bgc_pause_requested_ before
                // evacuating the nursery.  BGC acknowledges via bgc_paused_
                // and stops parallel workers (draining all deques) to avoid
                // races with forwarding pointer writes.
                if (bgc_pause_requested_.load(std::memory_order_acquire)) [[unlikely]] {
                    // Stop parallel workers before pausing so they don't race
                    // with young GC evacuation.  All deques are drained on stop.
                    StopParallelMarkWorkers();
                    bgc_paused_.store(true, std::memory_order_release);
                    // Spin-wait until the pause is lifted (young GC done).
                    while (bgc_pause_requested_.load(std::memory_order_acquire)) {
                        std::this_thread::yield();
                    }
                    bgc_paused_.store(false, std::memory_order_release);
                    CHAOS_IL2CPP_LOG_DEBUG("BGC", "young_gc_pause_released");
                }

                bool progressed = false;

                // Process from worker 0's deque (BGC thread's own work).
                if (DrainWorkerDeque(0, kBatchSize) > 0) {
                    progressed = true;
                }

                // Drain global SATB queue — pushes newly-marked entries to
                // worker 0's deque where they'll be picked up next round.
                if (DrainGlobalSatbQueue() > 0) {
                    progressed = true;
                }

                // If idle, sweep all workers' deques (coordinator drain).
                // The BGC thread helps idle workers by draining their
                // deques, acting as a natural load-balancing mechanism
                // that complements worker-initiated stealing.
                if (!progressed) {
                    for (int i = 1; i < n_workers; i++) {
                        if (DrainWorkerDeque(i, kBatchSize) > 0) {
                            progressed = true;
                            break;  // Found work — resume normal loop.
                        }
                    }
                }

                if (progressed) {
                    idle_rounds = 0;
                    freeze_initiated = false;
                    // Yield to avoid starving mutators.
                    std::this_thread::yield();

                    // Incremental marking: if we've exceeded the time budget,
                    // yield CPU to mutators by sleeping for the interval.
                    // NotifyBgc() from SATB flushes will wake us early.
                    auto slice_elapsed = std::chrono::steady_clock::now() - slice_start;
                    const auto slice_budget = std::chrono::microseconds(GcConfig().MarkSliceBudgetUs);
                    if (slice_elapsed >= slice_budget) {
                        slice_start = std::chrono::steady_clock::now();
                        std::unique_lock<std::mutex> lock(bgc_cv_mutex_);
                        bgc_cv_.wait_for(lock, kMarkSliceInterval);
                    }
                } else {
                    idle_rounds++;
                    // After several idle rounds with no progress,
                    // initiate SATB freeze protocol (CoreCLR-aligned
                    // convergence guarantee).  Ask all mutators to flush
                    // their SATB buffers and stop submitting new entries,
                    // then do a final drain.  If new work appears after
                    // the freeze+drain, unfreeze and continue.
                    if (idle_rounds > 20 && !freeze_initiated) {
                        freeze_initiated = true;
                        CHAOS_IL2CPP_LOG_DEBUG("BGC", "satb_freeze_initiating");

                        int n_ack;
                        {
                            std::lock_guard<std::mutex> lock(satb_registry_mutex_);
                            n_ack = registered_satb_count_;
                        }
                        satb_freeze_remaining_.store(n_ack, std::memory_order_release);
                        satb_freeze_requested_.store(true, std::memory_order_release);

                        constexpr int kFreezeSpinLimit = 100000;
                        for (int f = 0; f < kFreezeSpinLimit; f++) {
                            if (satb_freeze_remaining_.load(std::memory_order_acquire) <= 0)
                                break;
                            if (f < 1024) {
                                std::this_thread::yield();
                            } else {
                                std::this_thread::sleep_for(std::chrono::microseconds(100));
                            }
                        }

                        satb_freeze_requested_.store(false, std::memory_order_release);

                        DrainGlobalSatbQueue();
                        for (int i = 0; i < n_workers; i++) {
                            DrainWorkerDeque(i, 0);
                        }

                        bool after_freeze_progress = false;
                        for (int i = 0; i < n_workers; i++) {
                            std::lock_guard<std::mutex> lock(bgc_workers_[i].steal_mutex);
                            if (!bgc_workers_[i].deque.empty()) {
                                after_freeze_progress = true;
                                break;
                            }
                        }
                        if (!after_freeze_progress) {
                            bool satb_empty;
                            {
                                std::lock_guard<std::mutex> lock(global_satb_mutex_);
                                satb_empty = global_satb_.empty();
                            }
                            if (satb_empty) {
                                CHAOS_IL2CPP_LOG_DEBUG("BGC", "satb_freeze_converged");
                                break;
                            }
                        }

                        // New work appeared after freeze — unfreeze and continue.
                        idle_rounds = 0;
                        freeze_initiated = false;
                        CHAOS_IL2CPP_LOG_DEBUG("BGC", "satb_freeze_unfrozen_new_work");
                        continue;
                    }

                    // Check if we're truly done (all deques + SATB empty).
                    bool all_done = true;
                    for (int i = 0; i < n_workers; i++) {
                        std::lock_guard<std::mutex> lock(bgc_workers_[i].steal_mutex);
                        if (!bgc_workers_[i].deque.empty()) {
                            all_done = false;
                            break;
                        }
                    }

                    bool satb_done;
                    {
                        std::lock_guard<std::mutex> lock(global_satb_mutex_);
                        satb_done = global_satb_.empty();
                    }

                    if (all_done && satb_done) {
                        break;  // Concurrent mark complete.
                    }

                    // Brief sleep to avoid busy-waiting.
                    if (idle_rounds > 10) {
                        std::this_thread::sleep_for(std::chrono::microseconds(100));
                    } else {
                        std::this_thread::yield();
                    }

                    // Safety valve after ~10s idle with no convergence.
                    if (idle_rounds > 100000) {
                        CHAOS_IL2CPP_LOG_WARN("BGC", "concurrent_mark_convergence_timeout");
                        break;
                    }
                }

                // Check if forced to stop.
                if (!bgc_running_.load(std::memory_order_acquire))
                    return;
                if (phase_.load(std::memory_order_acquire) != BgcPhase::CONCURRENT_MARK)
                    break;
            }

            // Signal parallel workers to stop and join them.
            StopParallelMarkWorkers();

            // Safety drain: any work left in deques after workers stopped.
            for (int i = 0; i < n_workers; i++) {
                DrainWorkerDeque(i, 0);
            }

            CHAOS_IL2CPP_LOG_DEBUG("BGC", "concurrent_mark_complete");
        }

        // Signal: concurrent mark done, waiting for STW re-mark.
        if (phase_.load(std::memory_order_acquire) == BgcPhase::CONCURRENT_MARK) {
            phase_.store(BgcPhase::REMARK_NEEDED, std::memory_order_release);
        }

        // BGC thread waits while STW re-mark happens (executed by the
        // requesting thread under safepoint).  The phase will be set to
        // CONCURRENT_SWEEP or FINISHED by the scheduler after re-mark.
        {
            std::unique_lock<std::mutex> lock(bgc_cv_mutex_);
            bgc_cv_.wait(lock, [this]() {
                auto p = phase_.load(std::memory_order_acquire);
                return p != BgcPhase::REMARK_NEEDED ||
                       !bgc_running_.load(std::memory_order_acquire);
            });
        }

        // ── Phase 3: Concurrent Sweep ─────────────────────────────
        if (phase_.load(std::memory_order_acquire) == BgcPhase::CONCURRENT_SWEEP) {
            CHAOS_IL2CPP_LOG_DEBUG("BGC", "concurrent_sweep_begin");

            // Sweep pages uncovered by the mark bitmap.
            // Each page is swept under the old-gen mutex, with yields between
            // pages so that mutator allocations are not starved.
            G_OldGen().BgcSweep();

            // Collect dead finalizable objects using the mark bitmap
            // (still valid because BgcSweep preserves it via clear_bitmap=false).
            // The bitmap will be cleared by StwCompact() later, so we must
            // capture the list now.
            bgc_dead_finalizables_ = G_OldGen().CollectDeadFinalizables();
            if (!bgc_dead_finalizables_.empty()) {
                CHAOS_IL2CPP_LOG_DEBUG_M("BGC", "dead_finalizables count={0}",
                    static_cast<unsigned long long>(bgc_dead_finalizables_.size()));
            }

            // Collect dead weak handles while the mark bitmap is still valid.
            // These will be nulled after finalization so that WeakTrackResurrection
            // semantics are preserved (resurrected objects keep their handles).
            bgc_dead_weak_handles_.clear();
            CollectDeadWeakHandlesForBgc();
            CHAOS_IL2CPP_LOG_DEBUG("BGC", "concurrent_sweep_complete");
        }

        // ── Phase 4: Signal compaction needed ────────────────────
        if (phase_.load(std::memory_order_acquire) == BgcPhase::CONCURRENT_SWEEP) {
            phase_.store(BgcPhase::COMPACT_NEEDED, std::memory_order_release);
            CHAOS_IL2CPP_LOG_DEBUG("BGC", "compact_needed_waiting");
        }

        {
            std::unique_lock<std::mutex> lock(bgc_cv_mutex_);
            bgc_cv_.wait(lock, [this]() {
                auto p = phase_.load(std::memory_order_acquire);
                return p != BgcPhase::COMPACT_NEEDED ||
                       !bgc_running_.load(std::memory_order_acquire);
            });
        }

        // ── Finish ────────────────────────────────────────────────
        if (phase_.load(std::memory_order_acquire) != BgcPhase::FINISHED) {
            phase_.store(BgcPhase::FINISHED, std::memory_order_release);
        }

        // ── Publish finalization work to finalizer thread ────────────
        // Run BEFORE cycle_complete_ so that chaos_gc_wait_for_pending_finalizers
        // can observe the pending batch and wait for it to drain.
        if (!bgc_dead_finalizables_.empty() || !bgc_dead_weak_handles_.empty()) {
            CHAOS_IL2CPP_LOG_DEBUG_M("BGC", "publish_finalization finalizers={0} weak={1}",
                static_cast<unsigned long long>(bgc_dead_finalizables_.size()),
                static_cast<unsigned long long>(bgc_dead_weak_handles_.size()));
            PublishFinalizationWork(bgc_dead_finalizables_, bgc_dead_weak_handles_);
            bgc_dead_finalizables_.clear();
            bgc_dead_weak_handles_.clear();
        }

        // Reset for next cycle.
        if (phase_.load(std::memory_order_acquire) == BgcPhase::FINISHED) {
            // Free Gen1 bitmap before signaling completion so that
            // WaitForCycleComplete() observes a clean state.
            FreeGen1MarkBitmap();
            g_bgc_is_marking.store(false, std::memory_order_release);
            bgc_start_requested_.store(false, std::memory_order_release);
            phase_.store(BgcPhase::IDLE, std::memory_order_release);

            // Replenish the emergency reserve before signaling completion,
            // so the reserve is ready for the next cycle's allocations.
            G_OldGen().ReplenishEmergencyReserve();

            cycle_complete_.store(true, std::memory_order_release);
            NotifyBgc();
            CHAOS_IL2CPP_LOG_DEBUG("BGC", "cycle_finished");
        }

        // ── BGC dependent handle processing ─────────────────────────
        {
            int kept = GcProcessDependentHandlesAfterBgc();
            if (kept > 0) {
                CHAOS_IL2CPP_LOG_DEBUG_M("BGC", "dep_handles_kept={0}",
                    static_cast<unsigned>(kept));
            }
        }
    }  // end while(bgc_running_)

    threading::UnregisterThread();
    CHAOS_IL2CPP_LOG_DEBUG("BGC", "thread_stopped");
}

// ── Dedicated finalizer thread ──────────────────────────────────────

void BgcController::PublishFinalizationWork(
    std::vector<FinalizerEntry>& finalizers,
    std::vector<DeadWeakHandle>& weak_handles) noexcept
{
    if (finalizers.empty() && weak_handles.empty()) return;
    {
        std::lock_guard<std::mutex> lock(finalizer_mutex_);
        // Convert to timed entries with retry tracking.
        for (auto& f : finalizers) {
            pending_timed_finalizers_.push_back({f.obj, f.finalizer, 0, false});
        }
        pending_weak_handles_.insert(pending_weak_handles_.end(),
            weak_handles.begin(), weak_handles.end());
        finalizer_work_pending_.store(true, std::memory_order_release);
    }
    bgc_finalizer_batches_pending_.fetch_add(1, std::memory_order_release);
    finalizer_cv_.notify_one();
}

void BgcController::FinalizerThreadMain() noexcept {
    int thread_id = threading::AllocateThreadId();
    threading::RegisterThread(thread_id, nullptr);
    threading::EnterPreemptiveMode();
    finalizer_thread_id_ = std::this_thread::get_id();
    CHAOS_IL2CPP_LOG_DEBUG_M("Finalizer", "thread_started id={0}", thread_id);

    // Signal Start() that finalizer thread startup is complete.
    finalizer_thread_started_.store(true, std::memory_order_release);

    while (finalizer_running_.load(std::memory_order_acquire)) {
        std::vector<TimedFinalizerEntry> queue;
        std::vector<DeadWeakHandle> weak_handles;
        {
            std::unique_lock<std::mutex> lock(finalizer_mutex_);
            finalizer_cv_.wait(lock, [this]() {
                return finalizer_work_pending_.load(std::memory_order_acquire) ||
                       !finalizer_running_.load(std::memory_order_acquire);
            });
            if (!finalizer_running_.load(std::memory_order_acquire)) break;
            queue.swap(pending_timed_finalizers_);
            weak_handles.swap(pending_weak_handles_);
            finalizer_work_pending_.store(false, std::memory_order_release);
        }

        if (queue.empty()) continue;

        CHAOS_IL2CPP_LOG_DEBUG_M("Finalizer", "running {0} finalizers",
            static_cast<unsigned long long>(queue.size()));

        for (auto& entry : queue) {
            if (entry.finalizer == nullptr) continue;
            if (entry.is_dead) continue;

            // Run finalizer inline on the finalizer thread (matching CoreCLR).
            // A hung finalizer blocks the finalizer thread — this is an
            // intentional design choice favoring simplicity and thread-safety
            // over per-finalizer timeout isolation.
            try {
                entry.finalizer(entry.obj);
            } catch (...) {
                entry.retry_count++;
                CHAOS_IL2CPP_LOG_WARN_M("Finalizer",
                    "finalizer exception obj={0} retry={1}/{2}",
                    entry.obj, entry.retry_count, kFinalizerMaxRetries);

                if (entry.retry_count >= kFinalizerMaxRetries) {
                    entry.is_dead = true;
                    CHAOS_IL2CPP_LOG_ERROR_M("Finalizer",
                        "finalizer permanently skipped obj={0}", entry.obj);
                } else {
                    // Re-queue for retry.
                    std::lock_guard<std::mutex> lock(finalizer_mutex_);
                    pending_timed_finalizers_.push_back(entry);
                    finalizer_work_pending_.store(true, std::memory_order_release);
                }
            }
        }

        // Process weak handles AFTER finalization (WeakTrackResurrection).
        if (!weak_handles.empty()) {
            std::vector<std::pair<uint64_t, void*>> flat;
            flat.reserve(weak_handles.size());
            for (auto& dwh : weak_handles) {
                flat.emplace_back(dwh.handle_id, dwh.old_object);
            }
            GcProcessCollectedWeakHandles(flat);
            CHAOS_IL2CPP_LOG_DEBUG_M("Finalizer", "weak_handles_processed count={0}",
                static_cast<unsigned long long>(flat.size()));
        }

        // Signal drain completion.
        int prev = bgc_finalizer_batches_pending_.fetch_sub(1, std::memory_order_acq_rel);
        if (prev == 1) {
            std::lock_guard<std::mutex> lock(bgc_finalizer_drain_mutex_);
            bgc_finalizer_drain_cv_.notify_all();
        }
    }

    threading::UnregisterThread();
    CHAOS_IL2CPP_LOG_DEBUG("Finalizer", "thread_stopped");
}

void BgcController::WaitForFinalizerDrain() noexcept {
    std::unique_lock<std::mutex> lock(bgc_finalizer_drain_mutex_);
    bgc_finalizer_drain_cv_.wait(lock, [this]() {
        return bgc_finalizer_batches_pending_.load(std::memory_order_acquire) == 0;
    });
}

// ── BGC-YoungGC coordinated pause protocol (G-3) ─────────────────────

void BgcController::PauseForYoungGc() noexcept {
    CHAOS_IL2CPP_LOG_DEBUG("BGC", "young_gc_pause_requested");
    bgc_pause_requested_.store(true, std::memory_order_release);
    // Wait for BGC thread to acknowledge (bgc_paused_ == true).
    while (!bgc_paused_.load(std::memory_order_acquire)) {
        std::this_thread::yield();
    }
    CHAOS_IL2CPP_LOG_DEBUG("BGC", "young_gc_paused_acknowledged");
    // Drain any stale nursery entries from work deques as a safety net.
    DrainNurseryFromWorkDeques();
}

void BgcController::ResumeAfterYoungGc() noexcept {
    CHAOS_IL2CPP_LOG_DEBUG("BGC", "young_gc_resume");
    bgc_pause_requested_.store(false, std::memory_order_release);
    // Wait for BGC thread to clear bgc_paused_ (acknowledge resume).
    while (bgc_paused_.load(std::memory_order_acquire)) {
        std::this_thread::yield();
    }
    CHAOS_IL2CPP_LOG_DEBUG("BGC", "young_gc_resumed");
}

void BgcController::DrainNurseryFromWorkDeques() noexcept {
    // Scan all worker deques and remove any entries pointing to nursery
    // regions.  BGC deques should normally never contain nursery pointers
    // (BGC only marks old-gen/LOH objects, filtering by IsInOldGen before
    // pushing).  This is a safety net for any edge case that might have
    // introduced a nursery reference.
    auto& rm = RegionManager::Instance();
    for (int i = 0; i < kMaxBgcWorkers; i++) {
        std::lock_guard<std::mutex> lock(bgc_workers_[i].steal_mutex);
        auto& dq = bgc_workers_[i].deque;
        for (auto it = dq.begin(); it != dq.end(); ) {
            if (*it != nullptr && rm.IsNurseryPointer(*it)) {
                it = dq.erase(it);
            } else {
                ++it;
            }
        }
    }
    CHAOS_IL2CPP_LOG_DEBUG("BGC", "nursery_deque_drain_done");
}

// ── Parallel mark workers ──────────────────────────────────────────

int BgcController::SpawnParallelMarkWorkers() {
    int hw = static_cast<int>(std::thread::hardware_concurrency());
    // Cap by both the compile-time array bound (kMaxBgcWorkers) and the
    // config-driven CHAOS_GC_BgcWorkers knob (env/API tunable).
    int cfg_workers = static_cast<int>(GcConfig().BgcWorkers);
    int n_workers = (std::min)(hw, kMaxBgcWorkers);
    n_workers = (std::min)(n_workers, cfg_workers > 0 ? cfg_workers : kMaxBgcWorkers);
    if (n_workers < 2) return 1;  // No benefit from parallel.

    // Initialize per-worker deques (worker 0 = BGC thread).
    for (int i = 0; i < n_workers; i++) {
        std::lock_guard<std::mutex> lock(bgc_workers_[i].steal_mutex);
        bgc_workers_[i].deque.clear();
    }
    bgc_worker_count_.store(n_workers, std::memory_order_release);

    // Spawn N-1 additional workers (the BGC thread itself is worker 0).
    bgc_parallel_done_.store(false, std::memory_order_release);
    bgc_parallel_workers_.clear();

    // Bind worker 0 (BGC thread) to NUMA node 0; spawned workers bind in BgcWorkerMain.
    int numa_count = GcNumaNodeCount();
    if (numa_count > 1) {
        GcNumaBindThread(0);
    }

    for (int i = 1; i < n_workers; i++) {
        bgc_parallel_workers_.emplace_back(&BgcController::BgcWorkerMain, this, i);
    }
    return n_workers;
}

void BgcController::StopParallelMarkWorkers() {
    // Guards against concurrent calls from ForceComplete and BgcThreadMain.
    // Without this, both can join the same worker — the first succeeds and
    // CloseHandle's the handle, the second fails with ESRCH ("no such process").
    std::lock_guard<std::mutex> lock(stop_workers_mutex_);

    bgc_parallel_done_.store(true, std::memory_order_release);
    for (auto& w : bgc_parallel_workers_) {
        if (w.joinable()) w.join();
    }
    bgc_parallel_workers_.clear();
    bgc_worker_count_.store(0, std::memory_order_relaxed);
}

namespace {
    /// Scan pointer slots of a grey object and collect newly-marked children.
    /// Does NOT acquire any lock — results are stored in @a out_children for
    /// batch push by the caller.
    void ScanObjectChildren(void* obj, std::vector<void*>& out_children) {
        const void* type_info_ptr = *static_cast<const void* const*>(obj);
        if (type_info_ptr == nullptr) return;

        auto& layout_registry = GcLayoutRegistry::Instance();
        if (!layout_registry.IsValidTypeInfoPointer(type_info_ptr)) return;

        uint64_t stable_id = layout_registry.ReadStableId(type_info_ptr);
        const auto* layout = layout_registry.Lookup(stable_id);

        if (layout == nullptr) {
            // Conservative fallback: type_info was recognized (valid pointer)
            // but no GcLayout is registered for this stable_id.  Scan all
            // pointer-aligned slots in the object and mark any old-gen refs.
            // This matches the same conservative path in DrainMarkStack.
            auto* page = G_OldGen().FindPage(obj);
            if (page == nullptr) return;
            auto obj_addr = reinterpret_cast<uintptr_t>(obj);
            auto payload_start = reinterpret_cast<uintptr_t>(page->Payload());
            CHAOS_IL2CPP_SIZE offset = static_cast<CHAOS_IL2CPP_SIZE>(obj_addr - payload_start);
            CHAOS_IL2CPP_SIZE payload_remaining = page->payload_size - offset;
            CHAOS_IL2CPP_SIZE max_size = kOldGenSizeClasses[kOldGenNumSizeClasses - 1];
            if (payload_remaining < max_size) max_size = payload_remaining;

            for (CHAOS_IL2CPP_SIZE slot_off = 0;
                 slot_off + sizeof(void*) <= max_size;
                 slot_off += sizeof(void*)) {
                auto* slot = reinterpret_cast<void**>(static_cast<char*>(obj) + slot_off);
                void* ref = *slot;
                if (ref == nullptr) continue;
                if (G_OldGen().IsInOldGen(ref)) {
                    if (G_OldGen().BgcTryMark(ref)) {
                        out_children.push_back(ref);
                    }
                } else if (G_Loh().IsInLOH(ref)) {
                    if (G_Loh().MarkObject(ref)) {
                        out_children.push_back(ref);
                    }
                }
                // GEN1_GEN2 scope: mark live Gen1 refs during transitive
                // tracing.  No-op when Gen1 bitmap is not allocated.
                BgcController::Instance().BgcTryMarkGen1(ref);
            }
            return;
        }

        if (layout->pointer_count == 0) return;

        uintptr_t obj_base = reinterpret_cast<uintptr_t>(obj);
        for (uint16_t i = 0; i < layout->pointer_count; i++) {
            uint16_t offset = layout->pointer_offsets[i].offset;
            auto* slot = reinterpret_cast<void**>(obj_base + offset);
            void* ref = *slot;
            if (ref == nullptr) continue;
            if (G_OldGen().IsInOldGen(ref)) {
                if (G_OldGen().BgcTryMark(ref)) {
                    out_children.push_back(ref);
                }
            } else if (G_Loh().IsInLOH(ref)) {
                if (G_Loh().MarkObject(ref)) {
                    out_children.push_back(ref);
                }
            }
            // GEN1_GEN2 scope: mark live Gen1 refs during transitive
            // tracing.  No-op when Gen1 bitmap is not allocated.
            BgcController::Instance().BgcTryMarkGen1(ref);
        }
    }
}

void BgcController::BgcWorkerMain(int worker_idx) {
    // Bind to NUMA node for locality.
    int numa_count = GcNumaNodeCount();
    if (numa_count > 1) {
        GcNumaBindThread(worker_idx % numa_count);
    }

    // Worker loop: per-worker deque with work-stealing (P1-1).
    // Each worker pops from its own deque (under steal_mutex).
    // When empty, attempts up to 3 random steals from other workers.
    // Newly-marked children are pushed to the worker's own deque.
    auto& ws = bgc_workers_[worker_idx];

    // Simple deterministic PRNG seed for random victim selection.
    // Not thread_local (MSVC rejects capturing thread_local in lambdas).
    // Simple linear congruential is sufficient — we don't need cryptographic
    // randomness, just distributed victim selection.
    uint32_t prng = static_cast<uint32_t>(
        reinterpret_cast<uintptr_t>(&ws) ^
        static_cast<uint32_t>(worker_idx * 0x9E3779B9));

    while (!bgc_parallel_done_.load(std::memory_order_acquire)) {
        void* obj = nullptr;

        // Phase 1: Try local pop from own deque.
        {
            std::lock_guard<std::mutex> lock(ws.steal_mutex);
            if (!ws.deque.empty()) {
                obj = ws.deque.back();
                ws.deque.pop_back();
            }
        }

        // Phase 2: If local empty, try steal (up to 3 random attempts).
        if (obj == nullptr) {
            int n = bgc_worker_count_.load(std::memory_order_acquire);
            for (int attempt = 0; attempt < 3 && n > 1; attempt++) {
                // Simple LCG instead of XorShift32 (avoids thread_local capture issues on MSVC).
                prng = prng * 1103515245u + 12345u;
                int victim = static_cast<int>(prng % n);
                if (victim == worker_idx) continue;

                auto& vw = bgc_workers_[victim];
                std::lock_guard<std::mutex> lock(vw.steal_mutex);
                if (!vw.deque.empty()) {
                    // Steal from front (oldest work) — victim continues
                    // from back (newest), preserving temporal locality.
                    obj = vw.deque.front();
                    vw.deque.erase(vw.deque.begin());
                    break;
                }
            }
        }

        if (obj == nullptr) {
            // Brief sleep to avoid starving mutators when no work available.
            // CoreCLR workers use a condition variable; we use a short sleep
            // to avoid rebuilding the entire wake-up protocol.
            std::this_thread::sleep_for(std::chrono::microseconds(50));
            continue;
        }

        // Process object: scan children, push to own deque.
        std::vector<void*> children;
        ScanObjectChildren(obj, children);
        if (!children.empty()) {
            std::lock_guard<std::mutex> lock(ws.steal_mutex);
            ws.deque.insert(ws.deque.end(), children.begin(), children.end());
        }
    }
}

// ── Grey object processing ───────────────────────────────────────────

void BgcController::ProcessGreyObject(void* obj) {
    std::vector<void*> children;
    ScanObjectChildren(obj, children);
    if (!children.empty()) {
        std::lock_guard<std::mutex> lock(bgc_workers_[0].steal_mutex);
        bgc_workers_[0].deque.insert(bgc_workers_[0].deque.end(),
                                     children.begin(), children.end());
    }
}

// ── Drain helpers ────────────────────────────────────────────────────

CHAOS_IL2CPP_SIZE BgcController::DrainWorkerDeque(int idx, CHAOS_IL2CPP_SIZE batch_limit) {
    /// Drain up to @a batch_limit entries from worker @a idx's deque.
    /// Processes each entry and pushes newly-marked children to the same deque.
    CHAOS_IL2CPP_SIZE count = 0;
    auto& ws = bgc_workers_[idx];
    std::vector<void*> batch;
    std::vector<void*> children;
    batch.reserve(kBgcPopBatchSize * 2);
    children.reserve(kBgcPopBatchSize * 2);

    while (true) {
        batch.clear();
        children.clear();
        {
            std::lock_guard<std::mutex> lock(ws.steal_mutex);
            int n = 0;
            while (!ws.deque.empty() && n < kBgcPopBatchSize) {
                batch.push_back(ws.deque.back());
                ws.deque.pop_back();
                ++n;
            }
        }

        if (batch.empty()) break;

        for (void* obj : batch) {
            ScanObjectChildren(obj, children);
            ++count;
        }

        if (!children.empty()) {
            std::lock_guard<std::mutex> lock(ws.steal_mutex);
            ws.deque.insert(ws.deque.end(), children.begin(), children.end());
        }

        if (batch_limit > 0 && count >= batch_limit) break;
    }
    return count;
}

CHAOS_IL2CPP_SIZE BgcController::DrainGlobalSatbQueue() {
    CHAOS_IL2CPP_SIZE count = 0;
    auto& w0 = bgc_workers_[0];
    while (true) {
        SatbEntry entry;
        {
            std::lock_guard<std::mutex> lock(global_satb_mutex_);
            if (global_satb_.empty()) break;
            entry = global_satb_.back();
            global_satb_.pop_back();
        }
        if (entry != nullptr) {
            if (G_OldGen().IsInOldGen(entry)) {
                if (G_OldGen().BgcTryMark(entry)) {
                    std::lock_guard<std::mutex> lock(w0.steal_mutex);
                    w0.deque.push_back(entry);
                }
            } else if (G_Loh().IsInLOH(entry)) {
                if (G_Loh().MarkObject(entry)) {
                    std::lock_guard<std::mutex> lock(w0.steal_mutex);
                    w0.deque.push_back(entry);
                }
            }
            // GEN1_GEN2 scope: mark live Gen1 SATB entries in the BGC
            // Gen1 bitmap.  No-op when Gen1 bitmap is not allocated.
            BgcTryMarkGen1(entry);
        }
        ++count;
    }
    return count;
}

CHAOS_IL2CPP_SIZE BgcController::DrainAllTlsSatbBuffers() {
    // SAFE ONLY UNDER SAFEPOINT.
    // Drain all registered thread-local SATB buffers.
    // During STW re-mark, all threads are paused, so their TLS is stable.
    CHAOS_IL2CPP_SIZE total = 0;
    std::lock_guard<std::mutex> lock(satb_registry_mutex_);
    for (int i = 0; i < registered_satb_count_; i++) {
        auto* buf = registered_satb_buffers_[i];
        if (buf == nullptr) continue;
        uint32_t count = buf->count.load(std::memory_order_acquire);
        if (count == 0) continue;
        FlushSatbBuffer(buf->entries, count);
        buf->count.store(0, std::memory_order_release);
        total += count;
    }
    return total;
}

// ── Gen1 concurrent mark bitmap ───────────────────────────────────

bool BgcController::AllocateGen1MarkBitmap() noexcept {
    if (gen1_mark_bitmap_ != nullptr) return true;

    Region* gen1 = G_YoungGen().gen1_region.load(std::memory_order_acquire);
    char* s_begin = (gen1 != nullptr) ? gen1->begin : nullptr;
    char* s_end   = G_YoungGen().gen1_end;
    if (s_begin == nullptr || s_end == nullptr || s_begin >= s_end) {
        CHAOS_IL2CPP_LOG_DEBUG("BGC", "gen1_bitmap: no Gen1 range");
        return false;
    }

    uintptr_t base = reinterpret_cast<uintptr_t>(s_begin);
    CHAOS_IL2CPP_SIZE span = static_cast<CHAOS_IL2CPP_SIZE>(s_end - s_begin);
    // 1 bit per pointer-sized slot: span / sizeof(void*) bits → bytes
    CHAOS_IL2CPP_SIZE total_slots = span / sizeof(void*);
    CHAOS_IL2CPP_SIZE bitmap_bytes = (total_slots / 8) + 1;

    auto* bitmap = static_cast<uint8_t*>(CHAOS_IL2CPP_MALLOC(bitmap_bytes));
    if (bitmap == nullptr) {
        CHAOS_IL2CPP_LOG_ERROR_M("BGC", "gen1_bitmap OOM span=%llu",
            static_cast<unsigned long long>(span));
        return false;
    }
    std::memset(bitmap, 0, bitmap_bytes);

    gen1_mark_bitmap_ = bitmap;
    gen1_bitmap_bytes_ = bitmap_bytes;
    gen1_bitmap_base_ = base;
    gen1_bitmap_span_ = span;

    CHAOS_IL2CPP_LOG_DEBUG_M("BGC", "gen1_bitmap allocated base=0x{0:x} span={1} bytes={2}",
        static_cast<unsigned long long>(base),
        static_cast<unsigned long long>(span),
        static_cast<unsigned long long>(bitmap_bytes));
    return true;
}

void BgcController::FreeGen1MarkBitmap() noexcept {
    if (gen1_mark_bitmap_ != nullptr) {
        CHAOS_IL2CPP_FREE(gen1_mark_bitmap_);
        gen1_mark_bitmap_ = nullptr;
    }
    gen1_bitmap_bytes_ = 0;
    gen1_bitmap_base_ = 0;
    gen1_bitmap_span_ = 0;
}

bool BgcController::BgcTryMarkGen1(void* obj) noexcept {
    if (gen1_mark_bitmap_ == nullptr) return false;
    uintptr_t addr = reinterpret_cast<uintptr_t>(obj);
    if (addr < gen1_bitmap_base_) return false;
    uintptr_t offset = addr - gen1_bitmap_base_;
    if (offset >= gen1_bitmap_span_) return false;

    CHAOS_IL2CPP_SIZE slot_idx = offset / sizeof(void*);
    CHAOS_IL2CPP_SIZE byte_idx = slot_idx / 8;
    uint8_t mask = static_cast<uint8_t>(1u << (slot_idx % 8));
    if (byte_idx >= gen1_bitmap_bytes_) return false;

#if defined(_MSC_VER)
    auto prev = _InterlockedOr8(reinterpret_cast<volatile char*>(&gen1_mark_bitmap_[byte_idx]), mask);
    return (prev & mask) == 0;
#else
    auto prev = __atomic_fetch_or(&gen1_mark_bitmap_[byte_idx], mask, __ATOMIC_RELAXED);
    return (prev & mask) == 0;
#endif
}

void BgcController::ResetGen1MarkBitmap() noexcept {
    if (gen1_mark_bitmap_ != nullptr) {
        std::memset(gen1_mark_bitmap_, 0, gen1_bitmap_bytes_);
        // Rebase to current Gen1 range in case it changed.
        Region* gen1 = G_YoungGen().gen1_region.load(std::memory_order_acquire);
        if (gen1 != nullptr) {
            gen1_bitmap_base_ = reinterpret_cast<uintptr_t>(gen1->begin);
        }
        CHAOS_IL2CPP_LOG_DEBUG("BGC", "gen1_bitmap reset");
    }
}

void GcAdvanceBgcCycle() noexcept {
    auto& bgc = BgcController::Instance();
    auto phase = bgc.Phase();
    if (phase == BgcPhase::IDLE) {
        auto decision = G_Scheduler().DecideCollection();
        if (decision != GcCollectionKind::FULL_BGC) return;
        // Decide BGC scope (GEN2_ONLY or GEN1_GEN2) before starting the cycle.
        // StartBgcCycle reads the scope from the scheduler to allocate the
        // Gen1 mark bitmap if needed.
        G_Scheduler().DecideBgcScope();
        bgc.StartBgcCycle();
    } else if (phase == BgcPhase::REMARK_NEEDED) {
        auto remark_start = std::chrono::steady_clock::now();
        uint32_t gen = threading::RequestGlobalSafepoint();
        bgc.StwRemark();
        bgc.StartConcurrentSweep();
        threading::ReleaseGlobalSafepoint(gen);
        uint64_t remark_ns = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now() - remark_start).count());
        CHAOS_IL2CPP_LOG_INFO_M("BGC", "remark_done pause_us={0}",
            remark_ns / 1000);
    } else if (phase == BgcPhase::COMPACT_NEEDED) {
        auto compact_start = std::chrono::steady_clock::now();
        uint32_t gen = threading::RequestGlobalSafepoint();
        bgc.StwCompact();
        threading::ReleaseGlobalSafepoint(gen);
        uint64_t compact_ns = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now() - compact_start).count());
        CHAOS_IL2CPP_LOG_INFO_M("BGC", "compact_done pause_us={0}",
            compact_ns / 1000);
    }
}

}  // namespace chaos::il2cpp::runtime_core
