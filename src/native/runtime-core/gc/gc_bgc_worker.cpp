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

            // M5-1: Ack any pending young-GC pause BEFORE leaving concurrent mark.
            // If a young GC requested a pause exactly as concurrent mark completed,
            // the BGC would otherwise break out of the mark loop and park in the
            // REMARK_NEEDED wait without acking bgc_paused_ — leaving the young GC
            // spinning forever in PauseForYoungGc (pre-existing phase-6/round-N race
            // in gc_bgc_race_test).  Ack + drain deques, then wait for the pause to
            // be lifted before proceeding to the phase transition.
            if (bgc_pause_requested_.load(std::memory_order_acquire)) [[unlikely]] {
                // Any work left in deques after workers stopped and dumps.
                for (int i = 0; i < kMaxBgcWorkers; i++) {
                    DrainWorkerDeque(i, 0);
                }
                bgc_paused_.store(true, std::memory_order_release);
                while (bgc_pause_requested_.load(std::memory_order_acquire)) {
                    std::this_thread::yield();
                }
                bgc_paused_.store(false, std::memory_order_release);
                CHAOS_IL2CPP_LOG_DEBUG("BGC", "young_gc_pause_acked_at_mark_complete");
            }

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
        // M5-1: loop servicing pending young-GC pauses.  PauseForYoungGc now
        // wakes us via bgc_cv_ even while parked here; if a pause is pending we
        // ack it and wait for resume, then re-evaluate the phase (so we do NOT
        // fall through to Finish on a pause wake).
        {
            bool pause_serviced = false;
            while (!pause_serviced) {
                {
                    std::unique_lock<std::mutex> lock(bgc_cv_mutex_);
                    bgc_cv_.wait(lock, [this]() {
                        auto p = phase_.load(std::memory_order_acquire);
                        return p != BgcPhase::REMARK_NEEDED ||
                               bgc_pause_requested_.load(std::memory_order_acquire) ||
                               !bgc_running_.load(std::memory_order_acquire);
                    });
                }
                if (bgc_pause_requested_.load(std::memory_order_acquire)) {
                    // Ack the pause and wait for the young GC to resume.
                    bgc_paused_.store(true, std::memory_order_release);
                    while (bgc_pause_requested_.load(std::memory_order_acquire)) {
                        std::this_thread::yield();
                    }
                    bgc_paused_.store(false, std::memory_order_release);
                    CHAOS_IL2CPP_LOG_DEBUG("BGC", "young_gc_pause_serviced_in_remark_wait");
                    // Re-evaluate: if the phase still hasn't advanced, re-wait.
                    if (phase_.load(std::memory_order_acquire) == BgcPhase::REMARK_NEEDED &&
                        bgc_running_.load(std::memory_order_acquire)) {
                        continue;
                    }
                }
                pause_serviced = true;
            }
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
            // M5-1: same pause-servicing loop as the REMARK_NEEDED wait — a
            // young-GC pause requested while parked here is acked and serviced,
            // then we re-evaluate the COMPACT_NEEDED phase (never falling through
            // to Finish on a pause wake).
            bool pause_serviced = false;
            while (!pause_serviced) {
                {
                    std::unique_lock<std::mutex> lock(bgc_cv_mutex_);
                    bgc_cv_.wait(lock, [this]() {
                        auto p = phase_.load(std::memory_order_acquire);
                        return p != BgcPhase::COMPACT_NEEDED ||
                               bgc_pause_requested_.load(std::memory_order_acquire) ||
                               !bgc_running_.load(std::memory_order_acquire);
                    });
                }
                if (bgc_pause_requested_.load(std::memory_order_acquire)) {
                    bgc_paused_.store(true, std::memory_order_release);
                    while (bgc_pause_requested_.load(std::memory_order_acquire)) {
                        std::this_thread::yield();
                    }
                    bgc_paused_.store(false, std::memory_order_release);
                    CHAOS_IL2CPP_LOG_DEBUG("BGC", "young_gc_pause_serviced_in_compact_wait");
                    if (phase_.load(std::memory_order_acquire) == BgcPhase::COMPACT_NEEDED &&
                        bgc_running_.load(std::memory_order_acquire)) {
                        continue;
                    }
                }
                pause_serviced = true;
            }
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
            // GC-N11: fire BGC cycle-complete (phase family ends here).
            GcFireEvent(GcEvent::BGC_FINISHED);
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

    // ── BGC thread exit cleanup (CoreCLR background.cpp:3233 对照) ──
    // bgc_running_ 变 false (Stop) 时线程从主循环退出。若退出时正处于
    // CONCURRENT_MARK / g_bgc_is_marking=true (Stop 不会走到 FINISHED 清位),
    // 不清会把"幽灵 marking"残留 —— 后续 young GC 见 g_bgc_is_marking=true 调
    // PauseForYoungGc 死转 (无线程可 ack)。线程退出必须无条件清,保证不变量:
    //   BGC 线程存活 ⇒ 可能 marking; 线程已死 ⇒ marking 必为 false。
    //   对照 CoreCLR gc_background_running 只在 BGC 线程主循环置/清 (本文件
    //   StartBgcCycle 改由线程在进入 mark 时置,见该函数注释)。
    g_bgc_is_marking.store(false, std::memory_order_release);
    bgc_pause_requested_.store(false, std::memory_order_release);
    bgc_paused_.store(false, std::memory_order_release);
    phase_.store(BgcPhase::IDLE, std::memory_order_release);

    threading::UnregisterThread();
    CHAOS_IL2CPP_LOG_DEBUG("BGC", "thread_stopped");
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

}  // namespace chaos::il2cpp::runtime_core
