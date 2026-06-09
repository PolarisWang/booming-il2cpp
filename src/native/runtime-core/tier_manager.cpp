// tier_manager.cpp — Tier compilation manager (Phase 3)
//
// Provides:
//   1. Module lifecycle tracking — register/unregister PatchMethod arrays
//   2. Background optimization thread for T2→T3 promotion
//   3. Statistics for diagnostics
//
// The background thread uses a function pointer callback for T3 promotion
// (registered by the interpreter library) to avoid a link-time dependency
// from runtime_core to the interpreter library.

#include "tier_manager.h"
#include "patch_loader.h"  // PatchMethod tier_state constants

#include <jit/jit_precode.h>  // JitRecompileToTier1, HotpatchEntryV0
#include <jit/jit_engine.h>   // CompileTier

#include <chaos/log.h>

namespace chaos::il2cpp::runtime_core {

// ── Global T3 promotion callback ──────────────────────────────────────────
static Tier3PromotionCallback g_tier3_promotion_fn = nullptr;

void SetTier3PromotionCallback(Tier3PromotionCallback cb) noexcept {
    g_tier3_promotion_fn = cb;
}

Tier3PromotionCallback GetTier3PromotionCallback() noexcept {
    return g_tier3_promotion_fn;
}

// ── TierManager singleton ─────────────────────────────────────────────────

TierManager& TierManager::Get() noexcept {
    static TierManager instance;
    return instance;
}

TierManager::~TierManager() noexcept {
    StopBackgroundThread();

    // Free any remaining module data.
    for (auto& [id, data] : modules_) {
        (void)id;
        delete[] data->methods;
        delete data;
    }
    modules_.clear();
}

// ── Module lifecycle ──────────────────────────────────────────────────────

ModuleTierData* TierManager::RegisterModule(
    uint32_t module_id,
    PatchMethod* methods,
    uint32_t method_count) noexcept {

    auto* data = new ModuleTierData();
    if (data == nullptr) return nullptr;

    data->module_id = module_id;
    data->method_count = method_count;
    data->epoch = 0;

    // Allocate and populate the method pointer array.
    data->methods = new PatchMethod*[method_count];
    if (data->methods == nullptr) {
        delete data;
        return nullptr;
    }
    for (uint32_t i = 0; i < method_count; ++i) {
        data->methods[i] = &methods[i];
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = modules_.find(module_id);
        if (it != modules_.end()) {
            delete[] it->second->methods;
            delete it->second;
        }
        modules_[module_id] = data;
    }

    return data;
}

void TierManager::UnregisterModule(uint32_t module_id) noexcept {
    ModuleTierData* data = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = modules_.find(module_id);
        if (it == modules_.end()) return;
        data = it->second;
        modules_.erase(it);

        // Remove all token_map_ entries belonging to this module.
        for (uint32_t i = 0; i < data->method_count; ++i) {
            if (data->methods[i] != nullptr) {
                token_map_.erase(data->methods[i]->token);
            }
        }
    }

    if (data == nullptr) return;
    delete[] data->methods;
    delete data;
}

ModuleTierData* TierManager::FindModuleData(uint32_t module_id) noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = modules_.find(module_id);
    return (it != modules_.end()) ? it->second : nullptr;
}

void TierManager::ResetMethodCallCount(uint32_t method_token) noexcept {
    std::lock_guard<std::mutex> lock(mutex_);

    // Fast path: token_map_ hit (subsequent calls after first population)
    {
        auto it = token_map_.find(method_token);
        if (it != token_map_.end()) {
            auto* pm = it->second;
            pm->call_count.store(0, std::memory_order_release);
            uint32_t tier = pm->tier_state.load(std::memory_order_acquire);
            if (tier >= PatchMethod::kOptimizedRegister &&
                tier != PatchMethod::kQuickJitted) {
                pm->tier_state.store(PatchMethod::kStackInterpreted, std::memory_order_release);
            }
            return;
        }
    }

    // Slow path: linear scan to find the method, then cache the mapping for
    // subsequent lookups (Issue #10: amortized O(1) after first hit).
    for (auto& [id, data] : modules_) {
        (void)id;
        for (uint32_t i = 0; i < data->method_count; ++i) {
            if (data->methods[i] != nullptr &&
                data->methods[i]->token == method_token) {
                auto* pm = data->methods[i];
                token_map_[method_token] = pm;

                pm->call_count.store(0, std::memory_order_release);

                uint32_t tier = pm->tier_state.load(std::memory_order_acquire);
                if (tier >= PatchMethod::kOptimizedRegister &&
                    tier != PatchMethod::kQuickJitted) {
                    pm->tier_state.store(PatchMethod::kStackInterpreted, std::memory_order_release);
                }
                return;
            }
        }
    }
}

// ── Background optimization queue ─────────────────────────────────────────

bool TierManager::EnqueueOptimization(PatchMethod* method) noexcept {
    if (method == nullptr) return false;

    // Ensure background thread is running.
    if (!running_.load(std::memory_order_acquire)) {
        StartBackgroundThread();
        if (!running_.load(std::memory_order_acquire)) return false;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.size() >= kMaxOptimizationQueueSize) {
            CHAOS_IL2CPP_LOG_WARN_M("Tier",
                "optimization queue full (max={})", kMaxOptimizationQueueSize);
            return false;
        }
        queue_.push_back({method});
        optimization_queue_depth.store(static_cast<uint32_t>(queue_.size()),
                                       std::memory_order_relaxed);
    }
    cv_.notify_one();
    return true;
}

bool TierManager::EnqueueJitRecompilation(jit::JitPrecode* precode) noexcept {
    if (precode == nullptr) return false;

    // Ensure background thread is running.
    if (!running_.load(std::memory_order_acquire)) {
        StartBackgroundThread();
        if (!running_.load(std::memory_order_acquire)) return false;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (jit_queue_.size() >= kMaxJitRecompQueueSize) {
            CHAOS_IL2CPP_LOG_WARN_M("Tier",
                "JIT recomp queue full (max={})", kMaxJitRecompQueueSize);
            return false;
        }
        jit_queue_.push_back({precode});
    }
    cv_.notify_one();
    return true;
}

void TierManager::StartBackgroundThread() noexcept {
    bool expected = false;
    if (!running_.compare_exchange_strong(expected, true,
            std::memory_order_acq_rel)) {
        return;  // Already running.
    }

    try {
        background_thread_ = std::thread(ThreadProc, this);
    } catch (...) {
        running_.store(false, std::memory_order_release);
        CHAOS_IL2CPP_LOG_ERROR_M("Tier",
            "failed to start background thread");
    }
}

void TierManager::StopBackgroundThread() noexcept {
    bool expected = true;
    if (!running_.compare_exchange_strong(expected, false,
            std::memory_order_acq_rel)) {
        return;  // Not running.
    }

    cv_.notify_all();
    if (background_thread_.joinable()) {
        background_thread_.join();
    }
}

// ── Background thread ─────────────────────────────────────────────────────

void TierManager::ThreadProc(void* arg) noexcept {
#if defined(_MSC_VER)
    __try {
#endif
    auto* self = static_cast<TierManager*>(arg);
    self->BackgroundLoop();
#if defined(_MSC_VER)
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        CHAOS_IL2CPP_LOG_ERROR_M("Tier",
            "background thread crashed (SEH 0x%x), restarting",
            GetExceptionCode());
        // Continue running — the background thread is not critical.
    }
#endif
}

void TierManager::BackgroundLoop() noexcept {
    CHAOS_IL2CPP_LOG_DEBUG_M("tier", "background thread started");

    while (running_.load(std::memory_order_acquire)) {
        // ── Step 1: Process JIT recompilation queue (higher priority) ──────────
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (!jit_queue_.empty()) {
                auto je = jit_queue_.back();
                jit_queue_.pop_back();
                lock.unlock();  // Compilation must not hold the lock

                CHAOS_IL2CPP_LOG_DEBUG_M("tier",
                    "background JIT Tier0->Tier1 for precode=%p", static_cast<void*>(je.precode));

#if defined(_MSC_VER)
                __try {
#endif
                void* new_code = jit::JitRecompileToTier1(je.precode);
#if defined(_MSC_VER)
                } __except(EXCEPTION_EXECUTE_HANDLER) {
                    CHAOS_IL2CPP_LOG_WARN_M("tier",
                        "JIT recompilation crashed (SEH 0x%x), keeping Tier 0 code",
                        GetExceptionCode());
                    lock.lock();
                    continue;
                }
#endif

                lock.lock();
                if (new_code != nullptr) {
                    // Patch direct_ptr to Tier 1 code so future calls bypass
                    // the trampoline and go directly to the optimized code.
                    HotpatchEntryV0* entry = je.precode->entry;
                    if (entry != nullptr) {
                        entry->direct_ptr = new_code;
                        CHAOS_IL2CPP_LOG_DEBUG_M("tier",
                            "Tier1 complete: direct_ptr patched to %p", new_code);
                    }
                } else {
                    CHAOS_IL2CPP_LOG_WARN_M("tier",
                        "Tier1 recompilation failed — keeping Tier 0 code");
                }
                // Continue to next iteration to check for more work
                continue;
            }
        }

        // ── Step 2: Wait for work (condition variable with 200ms timeout) ──────
        OptimizationEntry entry;
        bool has_work = false;

        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait_for(lock, std::chrono::milliseconds(200), [this]() {
                return !queue_.empty() ||
                       !jit_queue_.empty() ||
                       !running_.load(std::memory_order_acquire);
            });

            if (!running_.load(std::memory_order_acquire)) break;
            if (queue_.empty()) continue;

            // Dequeue from back (LIFO — most recently hot method first).
            entry = queue_.back();
            queue_.pop_back();
            optimization_queue_depth.store(
                static_cast<uint32_t>(queue_.size()),
                std::memory_order_relaxed);
            has_work = true;
        }

        if (!has_work || entry.method == nullptr) continue;

        // Verify the method still expects T3 promotion (not unloaded).
        auto tier = entry.method->tier_state.load(std::memory_order_acquire);
        if (tier != PatchMethod::kOptimizeLowering) continue;

        // Check that the callback is registered (interpreter library loaded).
        auto fn = GetTier3PromotionCallback();
        if (fn == nullptr) {
            CHAOS_IL2CPP_LOG_WARN_M("Tier",
                "T3 promotion callback not registered — skipping token={}",
                entry.method->token);
            continue;
        }

        CHAOS_IL2CPP_LOG_DEBUG_M("tier",
            "background T2->T3 for token={}", entry.method->token);

        // Run T3 promotion via callback.
#if defined(_MSC_VER)
        __try {
#endif
        fn(entry.method);
#if defined(_MSC_VER)
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            CHAOS_IL2CPP_LOG_WARN_M("Tier",
                "T3 promotion crashed (SEH 0x%x), skipping token=%u",
                GetExceptionCode(), entry.method->token);
            // Mark as promoted to T3 to prevent retry.
            uint32_t expected = PatchMethod::kOptimizeLowering;
            entry.method->tier_state.compare_exchange_strong(
                expected, PatchMethod::kOptimizedRegister, std::memory_order_release);
            continue;
        }
#endif

        // Set T3 ready (only if still kOptimizeLowering — someone may have unloaded).
        uint32_t expected = PatchMethod::kOptimizeLowering;
        entry.method->tier_state.compare_exchange_strong(
            expected, PatchMethod::kOptimizedRegister, std::memory_order_release);

        total_optimized_methods.fetch_add(1, std::memory_order_relaxed);

        // Yield to avoid starving other threads.
        std::this_thread::yield();
    }

    CHAOS_IL2CPP_LOG_DEBUG_M("tier", "background thread stopped");
}

// ── Adaptive threshold ────────────────────────────────────────────────────

uint32_t TierManager::GetAdaptiveT2Threshold() const noexcept {
    // Base threshold: 50 calls.
    // Scale: add 5 per 10 optimized methods, capped at +50.
    // Effective range: 50 (cold) to 100 (200+ optimized methods).
    uint32_t optimized = total_optimized_methods.load(std::memory_order_relaxed);
    uint32_t adjustment = (optimized / 10) * 5;
    if (adjustment > 50) adjustment = 50;
    return 50 + adjustment;
}

// ── EvaluateTierPromotion ─────────────────────────────────────────────────
//
// Shared tier promotion decision function.  Called by both TryTierUpgrade
// (entry_direct) and TryFastOsrPromotion (fast_dispatch) to eliminate
// duplicated CAS + threshold logic.
//
// Handles all CAS transitions internally via acq_rel ordering.
// The caller executes the returned action — no compilation happens here.

TierManager::PromotionAction TierManager::EvaluateTierPromotion(
    PatchMethod* pm, uint32_t call_count) noexcept {

    // Step 1: read current tier state
    auto tier = pm->tier_state.load(std::memory_order_acquire);

    // Step 2: T0→QuickJIT — immediately compile with no-optimizer for hotpatch
    // methods.  Threshold is 1 so first call triggers Quick JIT.
    if (tier == PatchMethod::kStackInterpreted &&
        call_count >= PatchMethod::kQuickJitThreshold) {
        uint32_t expected = PatchMethod::kStackInterpreted;
        if (pm->tier_state.compare_exchange_strong(
                expected, PatchMethod::kQuickJitted,
                std::memory_order_acq_rel)) {
            return PromotionAction::kQuickJit;
        }
        // CAS failed — another thread won, re-read for next check
        tier = pm->tier_state.load(std::memory_order_acquire);
    }

    // Step 3: kQuickJitted→kJitted — Quick JIT already compiled, check if
    // it's time for Full JIT (optimized compilation).
    if (tier == PatchMethod::kQuickJitted) {
        uint32_t backoff_base = PatchMethod::kJitThreshold +
                                pm->codegen_fail_count * 1000;
        if (call_count >= backoff_base) {
            uint32_t expected = PatchMethod::kQuickJitted;
            if (pm->tier_state.compare_exchange_strong(
                    expected, PatchMethod::kJitted,
                    std::memory_order_acq_rel)) {
                return PromotionAction::kCompileToNative;
            }
            // CAS failed — re-read for next check
            tier = pm->tier_state.load(std::memory_order_acquire);
        }
    }

    // Step 4: T1→T2 — stack-interpreted → register-lowering
    if (tier == PatchMethod::kStackInterpreted &&
        call_count >= GetAdaptiveT1Threshold()) {
        uint32_t expected = PatchMethod::kStackInterpreted;
        if (pm->tier_state.compare_exchange_strong(
                expected, PatchMethod::kRegisterLowering,
                std::memory_order_acq_rel)) {
            return PromotionAction::kPromoteToTier2;
        }
        // CAS failed — another thread won, re-read for next check
        tier = pm->tier_state.load(std::memory_order_acquire);
    }

    // Step 5: T2→T3 — register-mapped → background optimization
    if (tier == PatchMethod::kRegisterMapped &&
        call_count >= Get().GetAdaptiveT2Threshold()) {
        uint32_t expected = PatchMethod::kRegisterMapped;
        if (pm->tier_state.compare_exchange_strong(
                expected, PatchMethod::kOptimizeLowering,
                std::memory_order_acq_rel)) {
            return PromotionAction::kPromoteToTier3;
        }
        tier = pm->tier_state.load(std::memory_order_acquire);
    }

    // Step 6: T3→T4 — optimized-register → native (JIT) with codegen backoff
    if (tier == PatchMethod::kOptimizedRegister) {
        uint32_t backoff_base = PatchMethod::kJitThreshold +
                                pm->codegen_fail_count * 1000;
        if (call_count >= backoff_base) {
            uint32_t expected = PatchMethod::kOptimizedRegister;
            if (pm->tier_state.compare_exchange_strong(
                    expected, PatchMethod::kJitted,
                    std::memory_order_acq_rel)) {
                return PromotionAction::kCompileToNative;
            }
            // CAS failed — re-read for kJitted/kJitSkip check below
            tier = pm->tier_state.load(std::memory_order_acquire);
        }
    }

    // Step 7: already at native tier — check OSR availability
    if (tier >= PatchMethod::kJitted) {
        auto* nm = pm->cached_native_method;
        if (nm != nullptr && nm->code != nullptr) {
            if (nm->osr_entry_offset != 0) {
                return PromotionAction::kTransferToOsr;
            }
            return PromotionAction::kContinueInterpreting;
        }
    }

    // Step 6: no promotion needed
    return PromotionAction::kNoAction;
}

}  // namespace chaos::il2cpp::runtime_core
