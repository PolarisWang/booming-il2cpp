#include "gc_bgc.h"

#include <chaos/log.h>
#include <chaos/native_types.h>

#include "gc_events.h"
#include "gc_layout.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_young_gen.h"
#include "thread_state.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <thread>

namespace chaos::il2cpp::runtime_core {

// ── Global state ─────────────────────────────────────────────────────

thread_local int tls_satb_buffer_index = -1;
thread_local bool tls_satb_registered = false;
std::atomic<bool> g_bgc_is_marking{false};

void BgcFlushSatbBuffer(const SatbEntry* entries, uint32_t count) {
    BgcController::Instance().FlushSatbBuffer(entries, count);
}

// ======================================================================
// BgcController implementation
// ======================================================================

void BgcController::Start() {
    if (bgc_running_.exchange(true, std::memory_order_acq_rel))
        return;
    bgc_thread_ = std::thread(&BgcController::BgcThreadMain, this);

    // Start dedicated finalizer thread.
    if (!finalizer_running_.exchange(true, std::memory_order_acq_rel)) {
        finalizer_thread_ = std::thread(&BgcController::FinalizerThreadMain, this);
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
        g_gc_scheduler.RequestFullGc();
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

    // DIAG: check 0xFF bytes immediately after safepoint release.
    {
        uint64_t ff_post_sp = g_old_gen.DiagCountOxFFBytes();
        if (ff_post_sp > 0) {
            CHAOS_IL2CPP_LOG_ERROR("BGC",
                "DIAG: 0xFF bytes = %llu immediately after safepoint release!",
                static_cast<unsigned long long>(ff_post_sp));
        }
    }

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

    // Scan dirty cards: any old-gen pages may have new cross-gen references
    // from concurrent mark phase that aren't captured by SATB alone
    // (e.g., a nursery object allocated during concurrent mark that points
    // to an unmarked old-gen object via a new field write).
    CHAOS_IL2CPP_SIZE cards_dirty = 0;
    g_old_gen.ScanDirtyCardsInPages(
        [&](uintptr_t /*card_idx*/, uintptr_t card_start, uintptr_t card_end) {
            cards_dirty++;
            // Scan every pointer slot in the dirty card range for old-gen refs.
            for (auto* slot = reinterpret_cast<void**>(card_start);
                 slot < reinterpret_cast<void**>(card_end);
                 slot++) {
                void* ref = *slot;
                if (ref != nullptr && g_old_gen.IsInOldGen(ref)) {
                    if (g_old_gen.BgcTryMark(ref)) {
                        std::lock_guard<std::mutex> lock(
                            BgcController::Instance().bgc_workers_[0].steal_mutex);
                        BgcController::Instance().bgc_workers_[0].deque.push_back(ref);
                    }
                }
            }
        });

    // Drain again after dirty cards.
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
    g_old_gen.BgcCompact();

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

void BgcController::ForceComplete() {
    // Called under safepoint.  Complete marking and sweep inline.
    auto p = phase_.load(std::memory_order_acquire);
    if (p == BgcPhase::IDLE)
        return;

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
        g_old_gen.BgcSweep();
        bgc_dead_finalizables_ = g_old_gen.CollectDeadFinalizables();
        bgc_dead_weak_handles_.clear();
        {
            std::vector<std::pair<uint64_t, void*>> flat;
            GcCollectDeadWeakHandles(flat);
            for (auto& f : flat) {
                bgc_dead_weak_handles_.push_back({f.first, f.second});
            }
        }
    }

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

    // Notify BGC thread to skip to finish.
    phase_.store(BgcPhase::FINISHED, std::memory_order_release);
    g_bgc_is_marking.store(false, std::memory_order_release);
    bgc_start_requested_.store(false, std::memory_order_release);
    cycle_complete_.store(true, std::memory_order_release);
    NotifyBgc();

    CHAOS_IL2CPP_LOG_DEBUG("BGC", "force_complete");
}

// ── Root set population (under safepoint) ────────────────────────────

void BgcController::PopulateRootSet() {
    // DIAG: check 0xFF bytes before any Phase 1 work.
    uint64_t ff_before = g_old_gen.DiagCountOxFFBytes();

    // Phase 1a: Mark pinned roots from MarkSweepOldGen.
    // Pinned roots are registered via g_old_gen.AddPinnedRoot().
    // They are stored in MarkSweepOldGen::pinned_roots_ (private),
    // so we rely on the scheduler/gc_old_gen to expose them.
    // For now, pinned roots are handled via TryMarkRoot in the stack
    // scanning path below, which will mark any old-gen object found.

        // Phase 1b: Scan the shared young generation for old-gen pointers.
    // The young region is scanned [begin, current) for any reference
    // to old-gen objects; those objects are marked and enqueued for
    // concurrent marking.
    {
        Region* young_region = g_young_gen.region.load(std::memory_order_acquire);
        if (young_region != nullptr) {
            auto& ctrl = BgcController::Instance();
            void* begin = young_region->begin;
            void* cur   = young_region->current;
            if (cur > begin) {
                for (auto* slot = static_cast<void**>(begin);
                     slot < static_cast<void**>(cur);
                     slot++) {
                    void* ref = *slot;
                    if (ref != nullptr && g_old_gen.IsInOldGen(ref)) {
                        if (g_old_gen.BgcTryMark(ref)) {
                            std::lock_guard<std::mutex> lock(
                                ctrl.bgc_workers_[0].steal_mutex);
                            ctrl.bgc_workers_[0].deque.push_back(ref);
                        }
                    }
                }
            }
        }
    }

    // Phase 1c:// Phase 1c: Scan all thread stacks as conservative roots.
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
                if (ref != nullptr && g_old_gen.IsInOldGen(ref)) {
                    s_in_oldgen++;
                    if (!IsValidManagedObject(ref)) return;
                    s_valid++;
                    if (g_old_gen.BgcTryMark(ref)) {
                        s_marked++;
                        std::lock_guard<std::mutex> lock(
                            BgcController::Instance().bgc_workers_[0].steal_mutex);
                        BgcController::Instance().bgc_workers_[0].deque.push_back(ref);
                    }
                }
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
                if (g_old_gen.BgcTryMark(object)) {
                    std::lock_guard<std::mutex> lock(
                        BgcController::Instance().bgc_workers_[0].steal_mutex);
                    BgcController::Instance().bgc_workers_[0].deque.push_back(object);
                }
            },
            nullptr);
    }

    // Phase 1e: Process initial mark stack to build transitive root closure.
    // This runs under safepoint, so it's fast (no concurrent interference).
    DrainWorkerDeque(0, 0);

    // DIAG Phase 1f: Verify all marked objects have valid GcLayout lookups.
    // Reads type_info from each marked object and checks if the layout lookup
    // succeeds.  A failed lookup means ScanObjectChildren would skip that
    // object's children, causing them to remain unmarked and be freed by sweep.
    {
        auto& layout_registry = GcLayoutRegistry::Instance();
        auto* arr = g_old_gen.GetPageArray();
        printf("  DIAG Phase1f: arr=%p arr_count=%d\n",
               (void*)arr, arr ? arr->count : -1);
        if (arr != nullptr) {
            int total_marked = 0, total_lookup_miss = 0;
            for (int pi = 0; pi < arr->count; pi++) {
                auto* page = arr->pages[pi];
                if (page == nullptr || !page->in_use.load(std::memory_order_acquire))
                    continue;
                if (page->is_oversized) continue;

                auto bm9 = GcMarkBitmap(page->MarkBitmap(), page->bitmap_bytes);
                auto* bitmap = bm9.Words();
                char* payload = page->Payload();
                CHAOS_IL2CPP_SIZE num_words = bm9.WordCount();
                CHAOS_IL2CPP_SIZE slot = 0;

                for (CHAOS_IL2CPP_SIZE w = 0; w < num_words; w++) {
                    uint64_t word = bitmap[w];
                    if (word == 0) { slot += 64; continue; }

                    for (int b = 0; b < 64; b++) {
                        if (word & (static_cast<uint64_t>(1) << b)) {
                            total_marked++;
                            CHAOS_IL2CPP_SIZE obj_slot = slot + b;
                            CHAOS_IL2CPP_SIZE obj_offset = obj_slot * sizeof(void*);
                            void* obj = payload + obj_offset;

                            const void* type_info_ptr = *static_cast<const void* const*>(obj);
                            if (type_info_ptr == nullptr) continue;
                            if (!layout_registry.IsValidTypeInfoPointer(type_info_ptr)) continue;

                            auto* hot = static_cast<const TypeInfoHot*>(type_info_ptr);
                            uint64_t stable_id = hot->stable_id;
                            const auto* layout = layout_registry.Lookup(stable_id);
                            if (layout == nullptr) {
                                total_lookup_miss++;
                                CHAOS_IL2CPP_LOG_ERROR("BGC",
                                    "DIAG: layout_lookup_miss page=%p offset=%zu obj=%p "
                                    "type_info=%p stable_id=0x%016llx",
                                    static_cast<void*>(page),
                                    static_cast<size_t>(obj_offset),
                                    static_cast<void*>(obj),
                                    static_cast<const void*>(type_info_ptr),
                                    static_cast<unsigned long long>(stable_id));
                            }
                        }
                    }
                    slot += 64;
                }
            }
            if (total_lookup_miss > 0) {
                CHAOS_IL2CPP_LOG_ERROR("BGC",
                    "DIAG: Phase1f layout lookup misses=%d (marked=%d) — "
                    "these objects' children will NOT be traced by BGC!",
                    total_lookup_miss, total_marked);
            } else {
                CHAOS_IL2CPP_LOG_DEBUG_M("BGC",
                    "DIAG: Phase1f all {0} marked objects have valid layout lookups",
                    total_marked);
            }
            printf("  DIAG Phase1f: total_marked=%d misses=%d\n", total_marked, total_lookup_miss);
        } else {
            printf("  DIAG Phase1f: arr is null — can't check marked objects\n");
        }
    }

    // DIAG: check 0xFF bytes after all Phase 1 work.
    uint64_t ff_after = g_old_gen.DiagCountOxFFBytes();
    if (ff_after > ff_before) {
        CHAOS_IL2CPP_LOG_ERROR("BGC",
            "DIAG: 0xFF bytes increased from %llu to %llu during PopulateRootSet!",
            static_cast<unsigned long long>(ff_before),
            static_cast<unsigned long long>(ff_after));
    }
    CHAOS_IL2CPP_LOG_DEBUG_M("BGC",
        "DIAG: 0xFF bytes before={0} after={1}",
        static_cast<unsigned long long>(ff_before),
        static_cast<unsigned long long>(ff_after));
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

    while (bgc_running_.load(std::memory_order_acquire)) {
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

            // DIAG: how many 0xFF bytes before concurrent mark processing?
            uint64_t ff_at_start = g_old_gen.DiagCountOxFFBytes();
            if (ff_at_start > 0) {
                CHAOS_IL2CPP_LOG_ERROR("BGC",
                    "DIAG: 0xFF bytes = %llu at concurrent_mark_begin (already corrupted!)",
                    static_cast<unsigned long long>(ff_at_start));
            } else {
                CHAOS_IL2CPP_LOG_DEBUG("BGC", "DIAG: 0xFF bytes = 0 at concurrent_mark_begin (clean)");
            }

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

            while (true) {
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
            g_old_gen.BgcSweep();

            // Collect dead finalizable objects using the mark bitmap
            // (still valid because BgcSweep preserves it via clear_bitmap=false).
            // The bitmap will be cleared by StwCompact() later, so we must
            // capture the list now.
            bgc_dead_finalizables_ = g_old_gen.CollectDeadFinalizables();
            if (!bgc_dead_finalizables_.empty()) {
                CHAOS_IL2CPP_LOG_DEBUG_M("BGC", "dead_finalizables count={0}",
                    static_cast<unsigned long long>(bgc_dead_finalizables_.size()));
            }

            // Collect dead weak handles while the mark bitmap is still valid.
            // These will be nulled after finalization so that WeakTrackResurrection
            // semantics are preserved (resurrected objects keep their handles).
            bgc_dead_weak_handles_.clear();
            {
                std::vector<std::pair<uint64_t, void*>> flat;
                GcCollectDeadWeakHandles(flat);
                for (auto& f : flat) {
                    bgc_dead_weak_handles_.push_back({f.first, f.second});
                }
            }
            if (!bgc_dead_weak_handles_.empty()) {
                CHAOS_IL2CPP_LOG_DEBUG_M("BGC", "dead_weak_handles count={0}",
                    static_cast<unsigned long long>(bgc_dead_weak_handles_.size()));
            }

            CHAOS_IL2CPP_LOG_DEBUG("BGC", "concurrent_sweep_complete");
        }

        // ── Phase 4: Signal compaction needed ────────────────────
        // After concurrent sweep, transition to COMPACT_NEEDED.
        // The BGC thread sleeps here until a mutator detects the
        // phase under an allocation slow path, enters a safepoint,
        // runs StwCompact(), and transitions to FINISHED.
        if (phase_.load(std::memory_order_acquire) == BgcPhase::CONCURRENT_SWEEP) {
            phase_.store(BgcPhase::COMPACT_NEEDED, std::memory_order_release);
            CHAOS_IL2CPP_LOG_DEBUG("BGC", "compact_needed_waiting");
        }

        // BGC thread waits while STW compaction happens (executed by the
        // requesting mutator under safepoint).  The phase will be set to
        // FINISHED by StwCompact() after compaction completes.
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

        // Reset for next cycle.
        if (phase_.load(std::memory_order_acquire) == BgcPhase::FINISHED) {
            g_bgc_is_marking.store(false, std::memory_order_release);
            bgc_start_requested_.store(false, std::memory_order_release);
            phase_.store(BgcPhase::IDLE, std::memory_order_release);
            cycle_complete_.store(true, std::memory_order_release);
            NotifyBgc();
            CHAOS_IL2CPP_LOG_DEBUG("BGC", "cycle_finished");
        }

        // ── Publish finalization work to finalizer thread ────────────
        // Instead of running finalizers on the BGC thread, publish them
        // (and their corresponding weak handles) to the dedicated finalizer
        // thread.  The finalizer thread processes finalizers first, then
        // nulls weak handles — preserving WeakTrackResurrection ordering.
        if (!bgc_dead_finalizables_.empty() || !bgc_dead_weak_handles_.empty()) {
            CHAOS_IL2CPP_LOG_DEBUG_M("BGC", "publish_finalization finalizers={0} weak={1}",
                static_cast<unsigned long long>(bgc_dead_finalizables_.size()),
                static_cast<unsigned long long>(bgc_dead_weak_handles_.size()));
            PublishFinalizationWork(bgc_dead_finalizables_, bgc_dead_weak_handles_);
            bgc_dead_finalizables_.clear();
            bgc_dead_weak_handles_.clear();
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
        pending_finalizers_.insert(pending_finalizers_.end(),
            finalizers.begin(), finalizers.end());
        pending_weak_handles_.insert(pending_weak_handles_.end(),
            weak_handles.begin(), weak_handles.end());
        finalizer_work_pending_.store(true, std::memory_order_release);
    }
    finalizer_cv_.notify_one();
}

void BgcController::FinalizerThreadMain() noexcept {
    int thread_id = threading::AllocateThreadId();
    threading::RegisterThread(thread_id, nullptr);
    threading::EnterPreemptiveMode();
    CHAOS_IL2CPP_LOG_DEBUG("Finalizer", "thread_started id={0}", thread_id);

    while (finalizer_running_.load(std::memory_order_acquire)) {
        std::vector<FinalizerEntry> queue;
        std::vector<DeadWeakHandle> weak_handles;
        {
            std::unique_lock<std::mutex> lock(finalizer_mutex_);
            finalizer_cv_.wait(lock, [this]() {
                return finalizer_work_pending_.load(std::memory_order_acquire) ||
                       !finalizer_running_.load(std::memory_order_acquire);
            });
            if (!finalizer_running_.load(std::memory_order_acquire)) break;
            queue.swap(pending_finalizers_);
            weak_handles.swap(pending_weak_handles_);
            finalizer_work_pending_.store(false, std::memory_order_release);
        }

        if (!queue.empty()) {
            CHAOS_IL2CPP_LOG_DEBUG_M("Finalizer", "running {0} finalizers",
                static_cast<unsigned long long>(queue.size()));
            for (auto& entry : queue) {
                if (entry.finalizer != nullptr) {
                    entry.finalizer(entry.obj);
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
    }

    threading::UnregisterThread();
    CHAOS_IL2CPP_LOG_DEBUG("Finalizer", "thread_stopped");
}

// ── Parallel mark workers ──────────────────────────────────────────

int BgcController::SpawnParallelMarkWorkers() {
    int hw = static_cast<int>(std::thread::hardware_concurrency());
    int n_workers = std::min(hw, kMaxBgcWorkers);
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

    for (int i = 1; i < n_workers; i++) {
        bgc_parallel_workers_.emplace_back(&BgcController::BgcWorkerMain, this, i);
    }
    return n_workers;
}

void BgcController::StopParallelMarkWorkers() {
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

        auto* hot = static_cast<const TypeInfoHot*>(type_info_ptr);
        uint64_t stable_id = hot->stable_id;
        const auto* layout = layout_registry.Lookup(stable_id);

        if (layout == nullptr) {
            // Conservative fallback: type_info was recognized (valid pointer)
            // but no GcLayout is registered for this stable_id.  Scan all
            // pointer-aligned slots in the object and mark any old-gen refs.
            // This matches the same conservative path in DrainMarkStack.
            auto* page = g_old_gen.FindPage(obj);
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
                if (ref != nullptr && g_old_gen.IsInOldGen(ref)) {
                    if (g_old_gen.BgcTryMark(ref)) {
                        out_children.push_back(ref);
                    }
                }
            }
            return;
        }

        if (layout->pointer_count == 0) return;

        uintptr_t obj_base = reinterpret_cast<uintptr_t>(obj);
        for (uint16_t i = 0; i < layout->pointer_count; i++) {
            uint16_t offset = layout->pointer_offsets[i].offset;
            auto* slot = reinterpret_cast<void**>(obj_base + offset);
            void* ref = *slot;
            if (ref != nullptr && g_old_gen.IsInOldGen(ref)) {
                if (g_old_gen.BgcTryMark(ref)) {
                    out_children.push_back(ref);
                }
            }
        }
    }
}

void BgcController::BgcWorkerMain(int worker_idx) {
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
        if (entry != nullptr && g_old_gen.IsInOldGen(entry)) {
            if (g_old_gen.BgcTryMark(entry)) {
                std::lock_guard<std::mutex> lock(w0.steal_mutex);
                w0.deque.push_back(entry);
            }
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


void GcAdvanceBgcCycle() noexcept {
    auto& bgc = BgcController::Instance();
    auto phase = bgc.Phase();
    if (phase == BgcPhase::IDLE) {
        auto decision = g_gc_scheduler.DecideCollection();
        if (decision != GcCollectionKind::FULL_BGC) return;
        bgc.StartBgcCycle();
    } else if (phase == BgcPhase::REMARK_NEEDED) {
        uint32_t gen = threading::RequestGlobalSafepoint();
        bgc.StwRemark();
        bgc.StartConcurrentSweep();
        threading::ReleaseGlobalSafepoint(gen);
    } else if (phase == BgcPhase::COMPACT_NEEDED) {
        uint32_t gen = threading::RequestGlobalSafepoint();
        bgc.StwCompact();
        threading::ReleaseGlobalSafepoint(gen);
    }
}

}  // namespace chaos::il2cpp::runtime_core
