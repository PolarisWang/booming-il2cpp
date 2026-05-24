#ifndef CHAOS_IL2CPP_TIER_MANAGER_H_
#define CHAOS_IL2CPP_TIER_MANAGER_H_

// ── TierManager: global tier compilation state ──────────────────────────────
//
// Phase 3 of the tiered compilation system.  Responsibilities:
//   1. Module lifecycle tracking — register/unregister PatchMethod arrays
//      so that DestroyPatchContext can safely free tier data.
//   2. Background T2→T3 optimization queue — methods that exceed
//      kRegisterMappedThreshold (500 calls) are enqueued for background promotion
//      instead of blocking the calling thread.
//   3. Memory budget enforcement — 64 MB cap on optimized IR.
//   4. Statistics for diagnostics/benchmarking.
//
// The T3 promotion callback is set by the interpreter library at startup
// via SetTier3PromotionCallback().  This avoids a link-time dependency
// from runtime_core (which owns TierManager) to the interpreter library
// (which defines PromoteToTier3 / RebuildCallCacheForT3).

#include <chaos/native_types.h>

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>

#include <chaos/unordered_dense.h>

namespace chaos::il2cpp { namespace jit { struct JitPrecode; } }

namespace chaos::il2cpp::runtime_core {

struct PatchMethod;

// ── ModuleTierData ─────────────────────────────────────────────────────────
// Per-module tier tracking.  One entry per loaded .patchdata module.
// Maintains a compact array of PatchMethod pointers for fast iteration
// during module unload (set kT5Unloaded + free tier allocations).
struct ModuleTierData {
    uint32_t     module_id = 0;     // module index
    uint32_t     method_count = 0;  // total methods in the PatchContext
    PatchMethod** methods = nullptr; // array of PatchMethod pointers
    uint32_t     hot_count = 0;     // methods promoted to T2+
    uint64_t     epoch = 0;         // incremented on each patch apply
};

// ── OptimizationEntry ─────────────────────────────────────────────────────
// Single entry in the background optimization queue.
struct OptimizationEntry {
    PatchMethod* method = nullptr;
};

// ── JitRecompilationEntry ─────────────────────────────────────────────────
// Single entry in the JIT background recompilation queue.
// When a Tier 0 method's PGO call count exceeds kPgoTier1Threshold, it is
// enqueued for background Tier 1 recompilation via the TierManager thread.
struct JitRecompilationEntry {
    JitPrecode* precode;
};

// Queue depth limit to prevent unbounded memory growth.
static constexpr uint32_t kMaxOptimizationQueueSize = 1024;

// JIT recompilation queue depth limit.
static constexpr uint32_t kMaxJitRecompQueueSize = 256;

// Memory budget for T2/T3 optimized RegisterMethod IR.
static constexpr uint32_t kTierMemoryBudgetMB = 64;

// ── Global T3 promotion callback ───────────────────────────────────────────
// Registered by the interpreter library at startup.  Called by the
// background thread when it dequeues a T2→T3 promotion request.
using Tier3PromotionCallback = void (*)(PatchMethod*);
void SetTier3PromotionCallback(Tier3PromotionCallback cb) noexcept;
Tier3PromotionCallback GetTier3PromotionCallback() noexcept;

// ── TierManager ───────────────────────────────────────────────────────────
// Global singleton.  Manages module lifetime tracking, the background
// optimization queue, and statistics.
class TierManager {
public:
    static TierManager& Get() noexcept;

    // ── Module lifecycle ───────────────────────────────────────────────────
    // Register a module's PatchMethod array.  Called from ApplyPatchFromMemory
    // after all methods are populated.
    ModuleTierData* RegisterModule(uint32_t module_id,
                                   PatchMethod* methods,
                                   uint32_t method_count) noexcept;

    // Unregister a module.  Called from DestroyPatchContext.
    // Does NOT free tier data — that is done by the caller.
    void UnregisterModule(uint32_t module_id) noexcept;

    // Find module data (returns nullptr if not registered).
    ModuleTierData* FindModuleData(uint32_t module_id) noexcept;

    /// Find a PatchMethod by method_token and reset its call_count to 0.
    /// This allows tiering to re-trigger T4 compilation after a hotpatch.
    void ResetMethodCallCount(uint32_t method_token) noexcept;

    // ── Background optimization queue ──────────────────────────────────────
    // Enqueue a method for T2→T3 background promotion.
    // Returns true if enqueued, false if queue is full.
    // The background thread must be running (StartBackgroundThread).
    bool EnqueueOptimization(PatchMethod* method) noexcept;

    // ── JIT recompilation queue ───────────────────────────────────────────
    // Enqueue a Tier 0 method for background Tier 1 recompilation.
    // Called from JitStubDispatchImpl when enable_pgo is true and
    // pgo_call_count exceeds kPgoTier1Threshold.
    // @param precode   Pointer to JitPrecode*
    // @return true if enqueued, false if queue is full
    bool EnqueueJitRecompilation(JitPrecode* precode) noexcept;

    // Start/stop the background optimization thread.
    void StartBackgroundThread() noexcept;
    void StopBackgroundThread() noexcept;
    bool IsBackgroundThreadRunning() const noexcept {
        return running_.load(std::memory_order_acquire);
    }

    // ── Statistics ─────────────────────────────────────────────────────────
    std::atomic<uint32_t> total_optimized_methods{0};
    std::atomic<uint32_t> optimization_queue_depth{0};

    // ── Adaptive thresholds (Phase 4) ──────────────────────────────────────
    // T1→T2 threshold: fixed at 100 (T1→T2 is fast and cheap, no reason to delay).
    static uint32_t GetAdaptiveT1Threshold() noexcept { return 100; }

    // T2→T3 threshold: scales with total_optimized_methods to prevent
    // background queue overload when many methods go hot simultaneously.
    //   base = 500
    //   scale = min(total_optimized / 10 * 25, 500)
    //   effective range = 500..1000
    uint32_t GetAdaptiveT2Threshold() const noexcept;

private:
    TierManager() = default;
    ~TierManager() noexcept;
    TierManager(const TierManager&) = delete;
    TierManager& operator=(const TierManager&) = delete;

    void BackgroundLoop() noexcept;
    static void ThreadProc(void* arg) noexcept;

    // Module tracking (mutex-protected)
    mutable std::mutex mutex_;
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(uint32_t, ModuleTierData*) modules_;

    // Background thread
    std::thread                background_thread_;
    std::atomic<bool>          running_{false};
    CHAOS_IL2CPP_VECTOR(OptimizationEntry) queue_;
    CHAOS_IL2CPP_VECTOR(JitRecompilationEntry) jit_queue_;
    std::condition_variable    cv_;
};

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_TIER_MANAGER_H_
