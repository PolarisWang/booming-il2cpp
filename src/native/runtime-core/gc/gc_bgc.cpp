#include "gc_bgc.h"

#include <chaos/log.h>
#include <chaos/native_types.h>

#include "gc_events.h"
#include "gc_layout.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "thread_state.h"

#include <algorithm>
#include <chrono>
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
}

void BgcController::Stop() {
    if (!bgc_running_.exchange(false, std::memory_order_acq_rel))
        return;
    bgc_start_requested_.store(true, std::memory_order_release);
    if (bgc_thread_.joinable())
        bgc_thread_.join();
}

void BgcController::FlushSatbBuffer(const SatbEntry* entries, uint32_t count) {
    if (count == 0) return;
    std::lock_guard<std::mutex> lock(global_satb_mutex_);
    global_satb_.insert(global_satb_.end(), entries, entries + count);
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
        // Pool exhausted — reset alloc and fall back.
        satb_pool_alloc_.fetch_sub(1, std::memory_order_relaxed);
        CHAOS_IL2CPP_LOG_ERROR("BGC", "satb_pool_exhausted");
        return -1;
    }
    return idx;
}

// ── BGC cycle control ────────────────────────────────────────────────

void BgcController::StartBgcCycle() {
    // Under STW safepoint: populate roots, then signal BGC thread.
    CHAOS_IL2CPP_LOG_DEBUG("BGC", "start_cycle");

    phase_.store(BgcPhase::ROOT_COLLECT, std::memory_order_release);
    PopulateRootSet();
    CHAOS_IL2CPP_LOG_DEBUG("BGC", "root_set_populated");

    // Transition to concurrent mark phase.
    phase_.store(BgcPhase::CONCURRENT_MARK, std::memory_order_release);
    g_bgc_is_marking.store(true, std::memory_order_release);
    cycle_complete_.store(false, std::memory_order_release);
    bgc_start_requested_.store(true, std::memory_order_release);

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
    CHAOS_IL2CPP_SIZE marked = DrainMarkStack(0);

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
                        std::lock_guard<std::mutex> lock(mark_stack_mutex_);
                        mark_stack_.push_back(ref);
                    }
                }
            }
        });

    // Drain again after dirty cards.
    marked += DrainMarkStack(0);

    CHAOS_IL2CPP_LOG_DEBUG_M("BGC", "stw_remark_done marked={0} cards={1}",
        static_cast<unsigned long long>(marked),
        static_cast<unsigned long long>(cards_dirty));

    return marked;
}

void BgcController::StartConcurrentSweep() {
    phase_.store(BgcPhase::CONCURRENT_SWEEP, std::memory_order_release);
    bgc_start_requested_.store(true, std::memory_order_release);
    CHAOS_IL2CPP_LOG_DEBUG("BGC", "concurrent_sweep_started");
}

void BgcController::WaitForCycleComplete() {
    while (!cycle_complete_.load(std::memory_order_acquire)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    cycle_complete_.store(false, std::memory_order_release);
}

void BgcController::ForceComplete() {
    // Called under safepoint.  Complete marking and sweep inline.
    if (phase_.load(std::memory_order_acquire) == BgcPhase::IDLE)
        return;

    // Drain remaining SATB + mark stack.
    DrainAllTlsSatbBuffers();
    DrainGlobalSatbQueue();
    DrainMarkStack(0);

    // Notify BGC thread to skip to finish.
    phase_.store(BgcPhase::FINISHED, std::memory_order_release);
    g_bgc_is_marking.store(false, std::memory_order_release);
    bgc_start_requested_.store(false, std::memory_order_release);
    cycle_complete_.store(true, std::memory_order_release);

    CHAOS_IL2CPP_LOG_DEBUG("BGC", "force_complete");
}

// ── Root set population (under safepoint) ────────────────────────────

void BgcController::PopulateRootSet() {
    // Phase 1a: Mark pinned roots from MarkSweepOldGen.
    // Pinned roots are registered via g_old_gen.AddPinnedRoot().
    // They are stored in MarkSweepOldGen::pinned_roots_ (private),
    // so we rely on the scheduler/gc_old_gen to expose them.
    // For now, pinned roots are handled via TryMarkRoot in the stack
    // scanning path below, which will mark any old-gen object found.

    // Phase 1b: Scan ALL registered threads' TLS nurseries.
    // For each thread, scan [nursery->begin, nursery->current) for
    // pointers to old-gen objects.
    {
        threading::EnumerateThreads(
            [](threading::ManagedThread* thread) -> bool {
                if (thread->nursery_ctx == nullptr) return true;
                auto* nursery = thread->nursery_ctx->nursery;
                if (nursery == nullptr) return true;

                // Snapshot nursery range (thread is paused at safepoint).
                void* begin = nursery->begin;
                void* cur   = nursery->current;
                if (cur <= begin) return true;

                // Scan every pointer slot.
                auto& ctrl = BgcController::Instance();
                for (auto* slot = static_cast<void**>(begin);
                     slot < static_cast<void**>(cur);
                     slot++) {
                    void* ref = *slot;
                    if (ref != nullptr && g_old_gen.IsInOldGen(ref)) {
                        if (g_old_gen.BgcTryMark(ref)) {
                            std::lock_guard<std::mutex> lock(ctrl.mark_stack_mutex_);
                            ctrl.mark_stack_.push_back(ref);
                        }
                    }
                }
                return true;
            });
    }

    // Phase 1c: Scan all thread stacks as conservative roots.
    // This catches old-gen references that live in thread-local stack
    // slots and are NOT in any TLS nursery.
    {
        threading::GcScanAllThreadRoots(
            [](void* root_addr, bool /*is_interior*/, void* /*user_data*/) {
                // Read the pointer at root_addr and try to mark it if in old-gen.
                auto* slot = static_cast<void**>(root_addr);
                void* ref = *slot;
                if (ref != nullptr && g_old_gen.IsInOldGen(ref)) {
                    if (g_old_gen.BgcTryMark(ref)) {
                        std::lock_guard<std::mutex> lock(
                            BgcController::Instance().mark_stack_mutex_);
                        BgcController::Instance().mark_stack_.push_back(ref);
                    }
                }
            },
            nullptr);
    }

    // Phase 1d: Scan GCHandle table.
    {
        GcIterateHandleTable(
            [](void* object, void* /*user_data*/) {
                if (object == nullptr) return;
                // GCHandles may point to nursery or old-gen objects.
                // Only mark old-gen objects here; nursery objects are
                // handled by the young collector.
                if (g_old_gen.IsInOldGen(object)) {
                    if (g_old_gen.BgcTryMark(object)) {
                        std::lock_guard<std::mutex> lock(
                            BgcController::Instance().mark_stack_mutex_);
                        BgcController::Instance().mark_stack_.push_back(object);
                    }
                }
            },
            nullptr);
    }

    // Phase 1e: Process initial mark stack to build transitive root closure.
    // This runs under safepoint, so it's fast (no concurrent interference).
    DrainMarkStack(0);
}

// ── BGC thread main ──────────────────────────────────────────────────

void BgcController::BgcThreadMain() {
    CHAOS_IL2CPP_LOG_DEBUG("BGC", "thread_started");

    while (bgc_running_.load(std::memory_order_acquire)) {
        // Wait for a start request.
        if (!bgc_start_requested_.load(std::memory_order_acquire)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        // ── Phase 2: Concurrent Mark ──────────────────────────────
        // The root set was already populated by StartBgcCycle under
        // safepoint.  Now trace transitively while mutators run.
        // ──────────────────────────────────────────────────────────

        if (phase_.load(std::memory_order_acquire) == BgcPhase::CONCURRENT_MARK) {
            CHAOS_IL2CPP_LOG_DEBUG("BGC", "concurrent_mark_begin");

            // Spawn parallel workers for faster mark stack draining.
            // Workers share the mutex-protected mark_stack_: each pops one
            // object at a time, calls ProcessGreyObject, and pushes
            // newly-marked children back.  The BGC thread handles SATB
            // draining AND participates in mark stack processing.
            int n_workers = SpawnParallelMarkWorkers();
            CHAOS_IL2CPP_LOG_DEBUG_M("BGC", "parallel_workers spawned={0}",
                static_cast<unsigned>(n_workers));

            constexpr CHAOS_IL2CPP_SIZE kBatchSize = 64;
            int idle_rounds = 0;

            while (true) {
                bool progressed = false;

                // Process a batch of grey objects.
                if (DrainMarkStack(kBatchSize) > 0) {
                    progressed = true;
                }

                // Drain global SATB queue.
                if (DrainGlobalSatbQueue() > 0) {
                    progressed = true;
                }

                if (progressed) {
                    idle_rounds = 0;
                    // Yield to avoid starving mutators.
                    std::this_thread::yield();
                } else {
                    idle_rounds++;
                    // After several idle rounds with no progress,
                    // check if we're truly done (mark stack + SATB both empty).
                    bool mark_done;
                    {
                        std::lock_guard<std::mutex> lock(mark_stack_mutex_);
                        mark_done = mark_stack_.empty();
                    }
                    bool satb_done;
                    {
                        std::lock_guard<std::mutex> lock(global_satb_mutex_);
                        satb_done = global_satb_.empty();
                    }

                    if (mark_done && satb_done) {
                        break;  // Concurrent mark complete.
                    }

                    // Brief sleep to avoid busy-waiting.
                    if (idle_rounds > 10) {
                        std::this_thread::sleep_for(std::chrono::microseconds(100));
                    } else {
                        std::this_thread::yield();
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

            // Final drain of any last items added by in-flight workers.
            DrainMarkStack(0);

            CHAOS_IL2CPP_LOG_DEBUG("BGC", "concurrent_mark_complete");
        }

        // Signal: concurrent mark done, waiting for STW re-mark.
        if (phase_.load(std::memory_order_acquire) == BgcPhase::CONCURRENT_MARK) {
            phase_.store(BgcPhase::REMARK_NEEDED, std::memory_order_release);
        }

        // BGC thread waits while STW re-mark happens (executed by the
        // requesting thread under safepoint).  The phase will be set to
        // CONCURRENT_SWEEP or FINISHED by the scheduler after re-mark.
        while (phase_.load(std::memory_order_acquire) == BgcPhase::REMARK_NEEDED &&
               bgc_running_.load(std::memory_order_acquire)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        // ── Phase 3: Concurrent Sweep ─────────────────────────────
        if (phase_.load(std::memory_order_acquire) == BgcPhase::CONCURRENT_SWEEP) {
            CHAOS_IL2CPP_LOG_DEBUG("BGC", "concurrent_sweep_begin");

            // Sweep pages uncovered by the mark bitmap.
            // Each page is swept under the old-gen mutex, with yields between
            // pages so that mutator allocations are not starved.
            g_old_gen.BgcSweep();

            CHAOS_IL2CPP_LOG_DEBUG("BGC", "concurrent_sweep_complete");
        }

        // ── Finish ────────────────────────────────────────────────
        if (phase_.load(std::memory_order_acquire) != BgcPhase::FINISHED) {
            phase_.store(BgcPhase::FINISHED, std::memory_order_release);
        }

        // Reset for next cycle.
        if (phase_.load(std::memory_order_acquire) == BgcPhase::FINISHED) {
            g_bgc_is_marking.store(false, std::memory_order_release);
            bgc_start_requested_.store(false, std::memory_order_release);
            cycle_complete_.store(true, std::memory_order_release);
            phase_.store(BgcPhase::IDLE, std::memory_order_release);
            CHAOS_IL2CPP_LOG_DEBUG("BGC", "cycle_finished");
        }
    }

    CHAOS_IL2CPP_LOG_DEBUG("BGC", "thread_stopped");
}

// ── Parallel mark workers ──────────────────────────────────────────

int BgcController::SpawnParallelMarkWorkers() {
    int hw = static_cast<int>(std::thread::hardware_concurrency());
    int n_workers = std::min(hw, kMaxBgcWorkers);
    if (n_workers < 2) return 1;  // No benefit from parallel.

    // Spawn N-1 additional workers (the BGC thread itself is worker 0).
    bgc_parallel_done_.store(false, std::memory_order_release);
    bgc_parallel_worker_count_.store(n_workers - 1, std::memory_order_relaxed);
    bgc_parallel_workers_.clear();

    for (int i = 1; i < n_workers; i++) {
        bgc_parallel_workers_.emplace_back(&BgcController::BgcWorkerMain, this);
    }
    return n_workers;
}

void BgcController::StopParallelMarkWorkers() {
    bgc_parallel_done_.store(true, std::memory_order_release);
    for (auto& w : bgc_parallel_workers_) {
        if (w.joinable()) w.join();
    }
    bgc_parallel_workers_.clear();
    bgc_parallel_worker_count_.store(0, std::memory_order_relaxed);
}

void BgcController::BgcWorkerMain() {
    // Worker loop: pop from shared mark_stack_ and process grey objects
    // until the BGC thread signals completion.
    while (!bgc_parallel_done_.load(std::memory_order_acquire)) {
        void* obj = nullptr;
        {
            std::lock_guard<std::mutex> lock(mark_stack_mutex_);
            if (!mark_stack_.empty()) {
                obj = mark_stack_.back();
                mark_stack_.pop_back();
            }
        }
        if (obj != nullptr) {
            ProcessGreyObject(obj);
        } else {
            // Mark stack empty — brief pause before re-checking.
            std::this_thread::yield();
        }
    }
}

// ── Grey object processing ───────────────────────────────────────────

void BgcController::ProcessGreyObject(void* obj) {
    const void* type_info_ptr = *static_cast<const void* const*>(obj);
    if (type_info_ptr == nullptr) return;

    auto& layout_registry = GcLayoutRegistry::Instance();
    if (!layout_registry.IsValidTypeInfoPointer(type_info_ptr)) {
        // Not a valid TypeInfo pointer — conservative fallback would scan
        // all pointer-aligned slots.  For P2, skip conservative scan and
        // rely on the SATB pre-write barrier to capture any references.
        return;
    }

    auto* hot = static_cast<const TypeInfoHot*>(type_info_ptr);
    uint64_t stable_id = hot->stable_id;
    const auto* layout = layout_registry.Lookup(stable_id);
    if (layout == nullptr || layout->pointer_count == 0) return;

    // Precise scan: iterate only declared pointer offsets.
    uintptr_t obj_base = reinterpret_cast<uintptr_t>(obj);
    for (uint16_t i = 0; i < layout->pointer_count; i++) {
        uint16_t offset = layout->pointer_offsets[i].offset;
        auto* slot = reinterpret_cast<void**>(obj_base + offset);
        void* ref = *slot;

        if (ref != nullptr && g_old_gen.IsInOldGen(ref)) {
            if (g_old_gen.BgcTryMark(ref)) {
                std::lock_guard<std::mutex> lock(mark_stack_mutex_);
                mark_stack_.push_back(ref);
            }
        }
    }
}

// ── Drain helpers ────────────────────────────────────────────────────

CHAOS_IL2CPP_SIZE BgcController::DrainMarkStack(CHAOS_IL2CPP_SIZE batch_limit) {
    CHAOS_IL2CPP_SIZE count = 0;
    while (true) {
        void* obj;
        {
            std::lock_guard<std::mutex> lock(mark_stack_mutex_);
            if (mark_stack_.empty()) break;
            obj = mark_stack_.back();
            mark_stack_.pop_back();
        }
        ProcessGreyObject(obj);
        ++count;
        if (batch_limit > 0 && count >= batch_limit) break;
    }
    return count;
}

CHAOS_IL2CPP_SIZE BgcController::DrainGlobalSatbQueue() {
    CHAOS_IL2CPP_SIZE count = 0;
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
                std::lock_guard<std::mutex> lock(mark_stack_mutex_);
                mark_stack_.push_back(entry);
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

}  // namespace chaos::il2cpp::runtime_core
