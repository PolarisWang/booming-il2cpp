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

    // Review #1-test: the no-CYCLE-state cleanup below MUST run regardless of
    // whether a BGC thread was actually running (i.e. not gated on the
    // bgc_running_ exchange above).  A controller that was never Start()ed, or
    // whose thread exited before Stop (phantom / mid-exit window described in
    // PauseForYoungGc), must STILL re-assert IDLE + not-marking so no stale
    // marking/phase flag poisons a later young-GC coordination.  These stores are
    // idempotent when the flags are already clear.
    //
    // DELIBERATELY NOT clearing bgc_pause_requested_/bgc_paused_ here: those are
    // the ACTIVE young-GC pause handshake, not cycle state.  Clearing them
    // unconditionally could race a PauseForYoungGc that is legitimately mid-flight
    // (wiping the ack a young GC is waiting on), which the pre-push review flagged.
    // The handshake is torn down by the BGC thread's own exit cleanup and by the
    // StopConcurrentMark/ResumeAfterYoungGc timeout escapes, not by Stop().
    g_bgc_is_marking.store(false, std::memory_order_release);
    phase_.store(BgcPhase::IDLE, std::memory_order_release);
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
    // ROOT-CAUSE GUARD (task#16): a BGC cycle is meaningless without a running
    // BGC thread.  The scheduler calls StartBgcCycle when g_bgc_enabled &&
    // !IsBusy(), but IsBusy() only checks phase_!=IDLE — NOT bgc_running_.  In
    // entrypoints that never call BgcController::Start() (e.g. the standalone
    // gc_stress_test whose main() skips RuntimeInit), the scheduler can start a
    // "phantom" cycle: phase_=CONCURRENT_MARK + g_bgc_is_marking=true with NO BGC
    // thread alive to ack PauseForYoungGc.  Every young GC then calls
    // PauseForYoungGc and spins on bgc_paused_ forever (the scenario-C 2/5
    // HANG / §十 3/10 HANG).  Bail here so the phantom concurrent mark never forms.
    if (!bgc_running_.load(std::memory_order_acquire)) {
        CHAOS_IL2CPP_LOG_DEBUG("BGC", "start_cycle_skipped_no_thread");
        return;
    }

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
    // GC-N11: fire BGC root-collect (STW) phase start.
    GcFireEvent(GcEvent::BGC_ROOT_COLLECT);
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

    // GC-N11: fire BGC concurrent-mark phase start.
    GcFireEvent(GcEvent::BGC_CONCURRENT_MARK);
    CHAOS_IL2CPP_LOG_DEBUG("BGC", "concurrent_mark_started");
}

CHAOS_IL2CPP_SIZE BgcController::StwRemark() {
    // Must be called under safepoint.
    // GC-N11: fire BGC STW re-mark phase start.
    GcFireEvent(GcEvent::BGC_STW_REMARK);
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

        // ── In-place demoted objects as BGC roots ───────────────
        // A gen1-owned object physically resident in an old-gen page is NOT in
        // the gen1 region whose cards are scanned above, so the concurrent BGC
        // sweep would otherwise reclaim it.  Re-mark every demoted object (and
        // its transitive graph via the worker deque) so BGC sweep preserves it
        // while it is gen1-owned (CoreCLR-aligned in-place demotion, GC-N6 #10).
        {
            auto& ctrl = BgcController::Instance();
            const ScopedPreemptiveMode preempt;
            GcSpinLockGuard lock(G_OldGen().PageMutex());
            for (auto* page = G_OldGen().PageList(); page != nullptr; page = page->next) {
                if (!page->in_use.load(std::memory_order_acquire)) continue;
                for (int32_t i = 0; i < page->demoted_count.load(std::memory_order_acquire); i++) {
                    char* obj = page->demoted[i].addr;
                    if (obj == nullptr) continue;
                    if (G_OldGen().BgcTryMark(obj)) {
                        std::lock_guard<std::mutex> dl(ctrl.bgc_workers_[0].steal_mutex);
                        ctrl.bgc_workers_[0].deque.push_back(obj);
                    }
                }
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
    // GC-N11: fire BGC concurrent-sweep phase start.
    GcFireEvent(GcEvent::BGC_CONCURRENT_SWEEP);
}

void BgcController::StwCompact() {
    // Must be called under safepoint.
    // GC-N11: fire BGC STW compaction phase start.
    GcFireEvent(GcEvent::BGC_STW_COMPACT);
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
    // Review #2: fully clear the young-GC pause handshake too.  A young GC that
    // timed out waiting for the BGC to ack/clear a pause must not leave bgc_paused_
    // =true — otherwise the NEXT PauseForYoungGc would see it as an already-acked
    // pause and evacuate while a (not-yet-fully-stopped) BGC could still be
    // scanning.  Reset both sides of the handshake as part of forcing BGC to a
    // clean IDLE.  Safe: StopConcurrentMark runs under safepoint (full GC) or from
    // the young-GC pause/resume escape, so no handshake is legitimately mid-flight.
    bgc_pause_requested_.store(false, std::memory_order_release);
    bgc_paused_.store(false, std::memory_order_release);
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
                // root_addr is a slot on ANOTHER thread's stack (conservative
                // scan); it may sit in an ASan stack-frame redzone. Probe sheds
                // instrumentation only for genuinely poisoned slots, keeping live
                // root slots instrumented so a real OOB/UAF write into a root is
                // still surfaced (review #2 / #3) — instead of unconditionally
                // eliding for every slot and masking genuine findings.  (A prior
                // S2 pass changed this to blanket NoCheck; that masking is not
                // what the S2 SEGFAULT fix needed — the real fix was the self-
                // stack RelocateRoots in gc_old_gen, which keeps NoCheck.)
                void* ref = chaos::il2cpp::common::AsanReadPtrProbe(slot);
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

    // S3-A fast-path (CoreCLR background.cpp:3233 对照): if no BGC concurrent mark
    // is currently ACTIVE (g_bgc_is_marking==false — the precise "is a scan
    // running" signal), there is nothing to coordinate with.  Skipping the pause is
    // correct and lets a young GC that never started a BGC (unit fixture / BGC
    // disabled) proceed without spinning.
    //
    // KEY (review #1): do NOT key this on bgc_running_.  Stop() clears bgc_running_
    // (gc_bgc.cpp:85 exchange) BEFORE the exiting BGC thread clears g_bgc_is_marking
    // during its exit cleanup.  In that window a young GC reading bgc_running_==false
    // would skip coordination while the exiting thread could still be scanning a work
    // deque it had already started — a concurrent mark vs nursery-evacuation race.
    // g_bgc_is_marking is only set true under the safepoint inside StartBgcCycle
    // (which itself requires a running thread, line 153), so "marking==true ⟹ a
    // coordination partner exists / is required" holds unconditionally.
    if (!g_bgc_is_marking.load(std::memory_order_acquire)) {
        // Safety net (review #3): even with no mark currently active, a BGC thread
        // that marked once then exited could have left stale nursery entries in a
        // worker's work deque.  Those would later be treated as roots pointing at
        // already-moved addresses (dangling).  Drain unconditionally so neither the
        // no-mark fast path nor the full wait path can leak a stale nursery root.
        DrainNurseryFromWorkDeques();
        CHAOS_IL2CPP_LOG_DEBUG("BGC", "young_gc_pause_skipped_no_active_mark");
        return;
    }

    bgc_pause_requested_.store(true, std::memory_order_release);
    // M5-1: wake the BGC so it can ack even if it is parked in a phase-wait
    // (REMARK_NEEDED / COMPACT_NEEDED) — otherwise the wait predicate never
    // re-evaluates and the young GC spins on bgc_paused_ forever.
    bgc_cv_.notify_all();

    // S3-A bounded wait (CoreCLR wait_for_gc_done(timeOut) 对照): wait for the
    // BGC ack (bgc_paused_==true) up to a deadline, then give up.  If the ack is
    // never received the BGC is either dead or wedged; instead of spinning
    // forever (the L2-coordination deadlock family), force the BGC out so the
    // young GC can proceed safely without racing a possibly-still-scanning BGC.
    using namespace std::chrono;
    constexpr auto kPauseTimeout = milliseconds(2000);
    constexpr auto kPauseSleep   = microseconds(500);
    auto deadline = steady_clock::now() + kPauseTimeout;
    int log_count = 0;
    bool acked = false;
    while (!bgc_paused_.load(std::memory_order_acquire) &&
           steady_clock::now() < deadline) {
        auto remaining = duration_cast<milliseconds>(deadline - steady_clock::now()).count();
        if (remaining % 500 == 0 && log_count < 4) {  // throttled observability
            CHAOS_IL2CPP_LOG_WARN_M("BGC",
                "PauseForYoungGc waiting ack — phase={0} bgc_running={1} "
                "bgc_paused={2} bgc_marking={3}",
                static_cast<int>(phase_.load(std::memory_order_acquire)),
                static_cast<int>(bgc_running_.load(std::memory_order_acquire)),
                static_cast<int>(bgc_paused_.load(std::memory_order_acquire)),
                static_cast<int>(g_bgc_is_marking.load(std::memory_order_acquire)));
            ++log_count;
        }
        std::this_thread::sleep_for(kPauseSleep);
    }
    acked = bgc_paused_.load(std::memory_order_acquire);
    if (acked) {
        CHAOS_IL2CPP_LOG_DEBUG("BGC", "young_gc_paused_acknowledged");
    } else {
        // Deadline hit with no ack: the BGC is dead or wedged.  Force it out so
        // the young GC proceeds without a concurrent scan racing its evacuation.
        // StopConcurrentMark (safe under safepoint) drains workers + resets to
        // IDLE — CoreCLR's "revert to blocking" escape, bounded + deterministic.
        CHAOS_IL2CPP_LOG_WARN("BGC", "young_gc_pause_timeout — force-stopping BGC");
        StopConcurrentMark();
        bgc_pause_requested_.store(false, std::memory_order_release);
    }

    // Drain any stale nursery entries from work deques as a safety net.
    DrainNurseryFromWorkDeques();
}

void BgcController::ResumeAfterYoungGc() noexcept {
    CHAOS_IL2CPP_LOG_DEBUG("BGC", "young_gc_resume");
    bgc_pause_requested_.store(false, std::memory_order_release);
    // M5-1: wake the BGC so it can clear its pause-ack and continue, even if it
    // is parked in a phase-wait.
    bgc_cv_.notify_all();
    // S3-A bounded wait: the BGC must clear bgc_paused_ (ack resume).  Bound it
    // like the pause side — a wedged BGC must not strand the young GC forever.
    using namespace std::chrono;
    constexpr auto kResumeTimeout = milliseconds(2000);
    auto deadline = steady_clock::now() + kResumeTimeout;
    while (bgc_paused_.load(std::memory_order_acquire) &&
           steady_clock::now() < deadline) {
        std::this_thread::sleep_for(microseconds(500));
    }
    if (bgc_paused_.load(std::memory_order_acquire)) {
        // BGC didn't clear the pause-ack in time.  Force it to a clean IDLE so
        // the subsequent young GC phases (which assume BGC is not concurrently
        // scanning) hold.  Same escape as the pause side.  Review #2: explicitly
        // clear the pause-ack here too — StopConcurrentMark early-returns when
        // phase_ is already IDLE, so relying on it alone could leave bgc_paused_
        // =true to poison the NEXT PauseForYoungGc as a false "already acked".
        CHAOS_IL2CPP_LOG_WARN("BGC", "young_gc_resume_timeout — force-stopping BGC");
        StopConcurrentMark();
        bgc_pause_requested_.store(false, std::memory_order_release);
        bgc_paused_.store(false, std::memory_order_release);
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
