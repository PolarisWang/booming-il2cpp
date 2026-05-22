#include "gc_api.h"

#include <chaos/log.h>

#include "gc_helpers.h"
#include "gc_gen1.h"
#include "gc_heap.h"
#include "gc_loh.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_scheduler.h"
#include "gc_stats.h"
#include "gc_young_collector.h"
#include "gc_bgc.h"

#if defined(_WIN32)
#include <windows.h>
#endif

namespace chaos::il2cpp::runtime_core {

// ── Platform memory status ─────────────────────────────────────────

void GetPlatformMemoryStatus(MemoryStatusData& out) noexcept {
#if defined(_WIN32)
    MEMORYSTATUSEX ms;
    ms.dwLength = sizeof(ms);
    if (GlobalMemoryStatusEx(&ms)) {
        out.total_phys = static_cast<int64_t>(ms.ullTotalPhys);
        out.avail_phys = static_cast<int64_t>(ms.ullAvailPhys);
        return;
    }
#endif
    out.total_phys = 0;
    out.avail_phys = 0;
}

// ── External memory pressure tracking ─────────────────────────────
// Delegated to GcScheduler for unified GC triggering decisions.
// See gc_scheduler.h/cpp for AddExternalMemoryPressure /
// RemoveExternalMemoryPressure implementation.
namespace {
}  // anonymous namespace

// ── OOM handling ──────────────────────────────────────────────────

/// Re-entrancy guard: HandleOomCondition must not recurse when called
/// from within a GC promotion path (OldGen::Allocate during GcYoungCollection).
/// Set to true while HandleOomCondition is executing, checked on entry.
thread_local bool tls_in_oom_handler = false;

void* HandleOomCondition(void* (*retry_alloc)(void*), void* retry_context,
                         CHAOS_IL2CPP_SIZE size) noexcept {
    // Re-entrancy guard: if called from within a GC promotion path,
    // return nullptr immediately.  The GC cycle will complete and the
    // outer HandleOomCondition invocation will retry.
    if (tls_in_oom_handler) {
        CHAOS_IL2CPP_LOG_WARN("GC_API", "OOM: re-entrancy detected, skipping recursive handler");
        return nullptr;
    }
    tls_in_oom_handler = true;

    // Step 1: Try a blocking full STW GC (unless already in a GC or NoGcRegion).
    bool gc_ran = false;
    if (!GcIsInNoGcRegion() && G_Scheduler().TryClaimGcSlot()) {
        CHAOS_IL2CPP_LOG_WARN("GC_API", "OOM: triggering blocking full GC for size={0}",
            static_cast<unsigned long long>(size));
        uint32_t gen = threading::RequestGlobalSafepoint();
        chaos_gc_collect();
        threading::ReleaseGlobalSafepoint(gen);
        GcAdvanceBgcCycle();
        gc_ran = true;
    }

    // Step 2: Retry allocation after GC.
    if (retry_alloc) {
        void* ptr = retry_alloc(retry_context);
        if (ptr != nullptr) {
            CHAOS_IL2CPP_LOG_INFO("GC_API", "OOM recovery: allocation succeeded after full GC");
            tls_in_oom_handler = false;
            return ptr;
        }
    }

    // Step 3: Try the old-gen emergency reserve (protects finalizer thread).
    if (gc_ran) {
        void* reserve_ptr = G_OldGen().AllocateFromEmergencyReserve(size);
        if (reserve_ptr != nullptr) {
            CHAOS_IL2CPP_LOG_INFO("GC_API", "OOM recovery: allocated from emergency reserve");
            tls_in_oom_handler = false;
            return reserve_ptr;
        }
    }

    // Step 4: All attempts failed — fire OOM event.
    CHAOS_IL2CPP_LOG_ERROR("GC_API", "OOM: all recovery attempts failed for size={0}",
        static_cast<unsigned long long>(size));
    GcEtwFireGcOom();
    tls_in_oom_handler = false;
    return nullptr;
}

// ======================================================================
// chaos_gc_get_total_memory
// ======================================================================

CHAOS_IL2CPP_INT64 CHAOS_RUNTIME_ABI_CALL chaos_gc_get_total_memory(
    CHAOS_IL2CPP_INT32 force_full_collection) noexcept
{
    if (force_full_collection) {
        // Trigger a full blocking GC first.
        chaos_gc_collect();
    }

    // Return cumulative allocated bytes in old gen + LOH.
    // These are cumulative alloc counters (not live bytes), but after
    // a full GC they closely approximate live data size since freed
    // memory stays in the internal free lists.
    auto old_gen_bytes = static_cast<CHAOS_IL2CPP_INT64>(
        G_OldGen().TotalAllocated());
    auto loh_bytes = static_cast<CHAOS_IL2CPP_INT64>(
        G_Loh().TotalAllocated());
    return old_gen_bytes + loh_bytes;
}

// ======================================================================
// chaos_gc_add_memory_pressure
// ======================================================================

void CHAOS_RUNTIME_ABI_CALL chaos_gc_add_memory_pressure(
    CHAOS_IL2CPP_INT64 bytes) noexcept
{
    if (bytes < 0) {
        CHAOS_IL2CPP_LOG_WARN("GC_API", "add_memory_pressure_with_negative_value");
        return;
    }

    G_Scheduler().AddExternalMemoryPressure(bytes);
}

// ======================================================================
// chaos_gc_remove_memory_pressure
// ======================================================================

void CHAOS_RUNTIME_ABI_CALL chaos_gc_remove_memory_pressure(
    CHAOS_IL2CPP_INT64 bytes) noexcept
{
    if (bytes < 0) {
        CHAOS_IL2CPP_LOG_WARN("GC_API", "remove_memory_pressure_with_negative_value");
        return;
    }

    G_Scheduler().RemoveExternalMemoryPressure(bytes);
}

// ======================================================================
// chaos_gc_collect_with_mode
// ======================================================================

extern "C" void CHAOS_RUNTIME_ABI_CALL chaos_gc_collect_with_mode(
    CHAOS_IL2CPP_INT32 generation, CHAOS_IL2CPP_INT32 mode) noexcept
{
    CHAOS_IL2CPP_LOG_DEBUG("GC_API",
        "collect_with_mode gen=%d mode=%d", (int)generation, (int)mode);

    // Map mode to scheduler state.
    auto gc_mode = static_cast<GcCollectionMode>(mode);
    G_Scheduler().SetCollectionMode(gc_mode);

    // Map .NET semantics:
    //   Forced (1) / Aggressive (3) → immediate blocking collect
    //   Optimized (2) → let scheduler decide (BGC preference)
    //   Default (0) → same as Forced for GC.Collect(int)
    if (gc_mode == GcCollectionMode::OPTIMIZED) {
        // Optimized: trigger via scheduler (may defer to BGC).
        G_Scheduler().RequestFullGc();
        CHAOS_IL2CPP_LOG_DEBUG("GC_API", "collect_with_mode optimized (request deferred)");
    } else {
        // Forced / Aggressive / Default: immediate blocking collect.
        // Route by generation parameter:
        //   gen < 0 || gen >= 2 → full chain (young + gen1 + old + finalizers)
        //   gen == 1            → young + gen1 + finalizers (no old-gen collect)
        //   gen == 0            → young only + finalizers
        if (generation < 0 || generation >= 2) {
            // Full chain — matches existing chaos_gc_collect behavior.
            chaos_gc_collect();
        } else if (generation == 1) {
            // Mid chain: young + gen1 + finalizers.
            // Use force_skip_gen1 to prevent Phase 4 double-trigger;
            // call GcGen1Collection directly after young GC completes.
            Region* young_region = g_young_gen.region.load(std::memory_order_acquire);
            void* bump = g_young_gen.bump.load(std::memory_order_acquire);
            if (young_region != nullptr && bump > young_region->begin) {
                uint32_t gen = threading::RequestGlobalSafepoint();
                GcYoungCollection(true);
                threading::ReleaseGlobalSafepoint(gen);
            }
            // Always attempt Gen1 collection (gen1 may have data from
            // prior young GCs even if current young region was empty).
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
            G_OldGen().RunFinalizers();
        } else {
            // Young only: gen == 0.  Skip Gen1 entirely.
            Region* young_region = g_young_gen.region.load(std::memory_order_acquire);
            void* bump = g_young_gen.bump.load(std::memory_order_acquire);
            if (young_region != nullptr && bump > young_region->begin) {
                uint32_t gen = threading::RequestGlobalSafepoint();
                GcYoungCollection(true);
                threading::ReleaseGlobalSafepoint(gen);
            }
            G_OldGen().RunFinalizers();
        }
    }
}

// ======================================================================
// chaos_gc_get_latency_mode
// ======================================================================

extern "C" CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL chaos_gc_get_latency_mode() noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(G_Scheduler().GetLatencyMode());
}

// ======================================================================
// chaos_gc_set_latency_mode
// ======================================================================

extern "C" void CHAOS_RUNTIME_ABI_CALL chaos_gc_set_latency_mode(
    CHAOS_IL2CPP_INT32 mode) noexcept
{
    auto lm = static_cast<GcLatencyMode>(mode);
    CHAOS_IL2CPP_LOG_DEBUG("GC_API", "set_latency_mode mode=%d", (int)lm);

    // Validate mode range.
    if (lm < GcLatencyMode::BATCH || lm > GcLatencyMode::NO_GC_REGION) {
        CHAOS_IL2CPP_LOG_WARN("GC_API", "set_latency_mode invalid_mode=%d", (int)mode);
        return;
    }

    G_Scheduler().SetLatencyMode(lm);

    // If switching away from NO_GC_REGION, trigger any deferred GC.
    if (lm != GcLatencyMode::NO_GC_REGION) {
        auto kind = G_Scheduler().DecideCollection();
        if (kind != GcCollectionKind::NONE) {
            G_Scheduler().RequestFullGc();
        }
    }
}

// ======================================================================
// chaos_gc_get_heap_size
// ======================================================================

extern "C" CHAOS_IL2CPP_INT64 CHAOS_RUNTIME_ABI_CALL chaos_gc_get_heap_size() noexcept
{
    // Return total allocated bytes in old gen + LOH.
    auto old_gen_bytes = static_cast<CHAOS_IL2CPP_INT64>(
        G_OldGen().TotalAllocated());
    auto loh_bytes = static_cast<CHAOS_IL2CPP_INT64>(
        G_Loh().TotalAllocated());
    return old_gen_bytes + loh_bytes;
}

// ======================================================================
// chaos_gc_get_memory_info
// ======================================================================

extern "C" CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL chaos_gc_get_collection_count(
    CHAOS_IL2CPP_INT32 generation) noexcept
{
    auto snap = GcGetSnapshot();
    switch (generation) {
    case 0:
        return static_cast<CHAOS_IL2CPP_INT32>(snap.young_collections);
    case 1:
        return static_cast<CHAOS_IL2CPP_INT32>(snap.gen1_collections);
    default:
        return static_cast<CHAOS_IL2CPP_INT32>(snap.full_collections);
    }
}

extern "C" void CHAOS_RUNTIME_ABI_CALL chaos_gc_get_memory_info(
    CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INT32 kind) noexcept
{
    // obj is a managed GCMemoryInfoData object reference (points to MethodTable*).
    // Compute interior pointer past the MethodTable* to reach the first data field.
    auto* info = reinterpret_cast<GcMemoryInfoNative*>(
        reinterpret_cast<char*>(static_cast<std::intptr_t>(obj)) + sizeof(void*));
    auto snap = GcGetSnapshot();

    // ── Select per-kind data from the snapshot ────────────────────────
    // Different GCKind values provide different views:
    //   Any (0)       — latest overall snapshot (generation=last)
    //   Gen1 (1)      — Gen1-specific view (generation=1)
    //   Full (2)      — full STW marking GC view (generation=2)
    //   Background (3) — BGC concurrent mark view (generation=2, concurrent=1)
    int32_t generation = 2;
    int64_t gc_index = 0;
    int64_t promoted = 0;
    int64_t pause_ns_total = 0;
    int32_t compacted = 0;
    int32_t concurrent = 0;

    switch (kind) {
    case 1:  // GCKind.Gen1
        generation = 1;
        gc_index = static_cast<int64_t>(snap.gen1_collections);
        promoted = static_cast<int64_t>(snap.gen1_bytes_promoted);
        pause_ns_total = static_cast<int64_t>(snap.gen1_pause_ns_total);
        break;
    case 2:  // GCKind.Full
        generation = 2;
        gc_index = static_cast<int64_t>(snap.full_collections);
        promoted = static_cast<int64_t>(
            snap.young_bytes_promoted + snap.gen1_bytes_promoted);
        pause_ns_total = static_cast<int64_t>(snap.full_pause_ns_total);
        compacted = g_gc_stats.last_compacted.load(std::memory_order_relaxed);
        concurrent = g_gc_stats.last_concurrent.load(std::memory_order_relaxed);
        break;
    case 3:  // GCKind.Background
        generation = 2;
        gc_index = static_cast<int64_t>(snap.full_collections);
        promoted = 0;  // BGC does not promote; it marks concurrent
        pause_ns_total = static_cast<int64_t>(snap.full_pause_ns_total);
        concurrent = 1;  // BGC is always concurrent
        break;
    default:  // GCKind.Any (0) — latest overall snapshot
        generation = snap.last_gc_generation;
        gc_index = static_cast<int64_t>(snap.gc_index);
        promoted = static_cast<int64_t>(
            snap.young_bytes_promoted + snap.gen1_bytes_promoted);
        pause_ns_total = static_cast<int64_t>(
            snap.young_pause_ns_total + snap.gen1_pause_ns_total + snap.full_pause_ns_total);
        compacted = g_gc_stats.last_compacted.load(std::memory_order_relaxed);
        concurrent = g_gc_stats.last_concurrent.load(std::memory_order_relaxed);
        break;
    }

    auto heap_size = static_cast<CHAOS_IL2CPP_INT64>(
        G_OldGen().TotalAllocated() + G_Loh().TotalAllocated());
    auto fragmented = static_cast<CHAOS_IL2CPP_INT64>(
        snap.young_bytes_reclaimed + snap.full_bytes_reclaimed);

    MemoryStatusData mem;
    GetPlatformMemoryStatus(mem);

    info->high_memory_load_threshold_bytes = mem.total_phys / 2;
    info->total_available_memory_bytes = mem.total_phys;
    info->memory_load_bytes = mem.total_phys - mem.avail_phys;
    info->heap_size_bytes = heap_size;
    info->fragmented_bytes = fragmented;
    info->total_committed_bytes = heap_size;
    info->promoted_bytes = promoted;
    info->pinned_objects_count = 0;
    info->finalization_pending_count = static_cast<int64_t>(snap.finalization_pending_count);
    info->index = gc_index;
    info->generation = generation;
    {
        auto elapsed = std::chrono::steady_clock::now() - g_gc_start_time;
        uint64_t elapsed_ns = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count());
        info->pause_time_percentage = (elapsed_ns > 0)
            ? static_cast<int32_t>((static_cast<uint64_t>(pause_ns_total) * 100) / elapsed_ns)
            : 0;
    }
    info->compacted = static_cast<uint8_t>(compacted);
    info->concurrent = static_cast<uint8_t>(concurrent);

    // ── GenerationInfo array (5 entries: gen0, gen1, gen2, gen3, LOH) ──
    // Each entry: SizeBeforeBytes, SizeAfterBytes, FragBeforeBytes, FragAfterBytes
    // CRAG has 2 effective generations; map to BCL's 5-generation model.
    // This part is independent of GCKind — it reflects heap state at query time.
    CHAOS_IL2CPP_SIZE nursery_capacity = 0;
    {
        auto* nursery = g_young_gen.region.load(std::memory_order_acquire);
        if (nursery != nullptr) {
            nursery_capacity = static_cast<CHAOS_IL2CPP_SIZE>(
                nursery->end - nursery->begin);
        }
    }
    CHAOS_IL2CPP_SIZE survivor_occupancy = 0;
    Region* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    if (gen1 != nullptr) {
        char* s_bump = g_young_gen.gen1_bump.load(std::memory_order_acquire);
        if (s_bump > gen1->begin) {
            survivor_occupancy = static_cast<CHAOS_IL2CPP_SIZE>(
                s_bump - gen1->begin);
        }
    }

    // Gen0 (young/nursery)
    info->gen0_size_before = static_cast<int64_t>(nursery_capacity);
    info->gen0_size_after = 0;
    info->gen0_frag_before = 0;
    info->gen0_frag_after = 0;

    // Gen1 (survivor)
    int64_t gen1_reclaimed = static_cast<int64_t>(snap.gen1_bytes_reclaimed);
    info->gen1_size_before = static_cast<int64_t>(survivor_occupancy);
    info->gen1_size_after = (survivor_occupancy > gen1_reclaimed)
        ? static_cast<int64_t>(survivor_occupancy) - gen1_reclaimed : 0;
    info->gen1_frag_before = gen1_reclaimed;
    info->gen1_frag_after = 0;

    // Gen2 (old gen)
    int64_t old_alloc = static_cast<int64_t>(G_OldGen().TotalAllocated());
    int64_t old_reclaimed = static_cast<int64_t>(snap.full_bytes_reclaimed);
    info->gen2_size_before = old_alloc;
    info->gen2_size_after = (old_alloc > old_reclaimed) ? old_alloc - old_reclaimed : old_alloc;
    info->gen2_frag_before = old_reclaimed;
    info->gen2_frag_after = old_reclaimed;

    // Gen3 (not present in CRAG)
    info->gen3_size_before = 0;
    info->gen3_size_after = 0;
    info->gen3_frag_before = 0;
    info->gen3_frag_after = 0;

    // LOH
    int64_t loh_alloc = static_cast<int64_t>(G_Loh().TotalAllocated());
    info->loh_size_before = loh_alloc;
    info->loh_size_after = loh_alloc;
    info->loh_frag_before = 0;
    info->loh_frag_after = 0;
}

// ======================================================================
// NO_GC_REGION support
// ======================================================================

/// TLS nesting counter for NO_GC_REGION.
/// >0 means the current thread is inside a no-GC region.
thread_local int tls_no_gc_region_depth = 0;

extern "C" void CHAOS_RUNTIME_ABI_CALL chaos_gc_enter_no_gc_region() noexcept {
    ++tls_no_gc_region_depth;
    CHAOS_IL2CPP_LOG_DEBUG("GC_API", "enter_no_gc_region depth=%d", tls_no_gc_region_depth);
}

extern "C" void CHAOS_RUNTIME_ABI_CALL chaos_gc_leave_no_gc_region() noexcept {
    if (tls_no_gc_region_depth <= 0) {
        CHAOS_IL2CPP_LOG_WARN("GC_API", "leave_no_gc_region mismatched (depth already 0)");
        return;
    }
    --tls_no_gc_region_depth;
    CHAOS_IL2CPP_LOG_DEBUG("GC_API", "leave_no_gc_region depth=%d", tls_no_gc_region_depth);

    // When counter reaches zero, trigger any deferred GC.
    if (tls_no_gc_region_depth == 0) {
        auto kind = G_Scheduler().DecideCollection();
        if (kind != GcCollectionKind::NONE) {
            G_Scheduler().RequestFullGc();
        }
    }
}

bool GcIsInNoGcRegion() noexcept {
    return tls_no_gc_region_depth > 0;
}

// ======================================================================
// chaos_gc_try_start_no_gc_region
// ======================================================================

extern "C" CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL chaos_gc_try_start_no_gc_region(
    CHAOS_IL2CPP_INT64 total_size, CHAOS_IL2CPP_INT32 disallow_full_blocking_gc) noexcept
{
    (void)disallow_full_blocking_gc;  // CRAG always allows full GC as safety net

    if (total_size < 0) return 0;

    // Estimate available capacity: remaining young gen bytes.
    auto* region = g_young_gen.region.load(std::memory_order_acquire);
    char* bump = g_young_gen.bump.load(std::memory_order_acquire);
    char* region_end = g_young_gen.region_end.load(std::memory_order_acquire);
    CHAOS_IL2CPP_INT64 available = 0;
    if (region != nullptr && bump != nullptr && region_end != nullptr) {
        available = static_cast<CHAOS_IL2CPP_INT64>(region_end - bump);
    }

    // Add Gen1 capacity as additional available space.
    auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    if (gen1 != nullptr) {
        char* gen1_bump = g_young_gen.gen1_bump.load(std::memory_order_acquire);
        if (gen1_bump != nullptr && gen1_bump < gen1->end) {
            available += static_cast<CHAOS_IL2CPP_INT64>(gen1->end - gen1_bump);
        }
    }

    // Add old gen free page capacity (estimate: use half of total allocated
    // as a conservative approximation of free-list capacity).
    CHAOS_IL2CPP_INT64 old_alloc = static_cast<CHAOS_IL2CPP_INT64>(
        G_OldGen().TotalAllocated());
    available += old_alloc / 2;

    // Apply 2x safety margin: require twice the requested budget.
    if (available < total_size * 2) return 0;

    // Budget sufficient — enter no-GC region.
    ++tls_no_gc_region_depth;
    CHAOS_IL2CPP_LOG_DEBUG("GC_API", "try_start_no_gc_region depth=%d budget=%lld available=%lld",
        tls_no_gc_region_depth,
        static_cast<long long>(total_size),
        static_cast<long long>(available));
    return 1;
}

// ======================================================================
// chaos_gc_end_no_gc_region
// ======================================================================

extern "C" CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL chaos_gc_end_no_gc_region() noexcept {
    if (tls_no_gc_region_depth <= 0) {
        CHAOS_IL2CPP_LOG_WARN("GC_API", "end_no_gc_region mismatched (depth already 0)");
        return 0;  // Success (no GC triggered — nothing to clean up)
    }

    --tls_no_gc_region_depth;
    CHAOS_IL2CPP_LOG_DEBUG("GC_API", "end_no_gc_region depth=%d", tls_no_gc_region_depth);

    if (tls_no_gc_region_depth == 0) {
        auto kind = G_Scheduler().DecideCollection();
        if (kind != GcCollectionKind::NONE) {
            G_Scheduler().RequestFullGc();
            return 1;  // GCTriggered
        }
    }

    return 0;  // Success
}

void chaos_gc_register_finalizable(void* obj) noexcept {
    if (obj == nullptr) return;

    // Read TypeInfoHot* from the object's first word.
    auto* type_info = *static_cast<const TypeInfoHot**>(obj);
    if (type_info == nullptr) return;

    // Only finalize types with the has_finalizer flag.
    if ((type_info->flags & kTypeInfoHasFinalizer) == 0) return;

    // Look up the finalizer callback from the side map (stable_id → callback).
    auto& registry = GcLayoutRegistry::Instance();
    auto* callback = registry.LookupFinalizer(type_info->stable_id);
    if (callback == nullptr) return;

    // Register with old gen so RunFinalizers() can invoke the callback
    // when the object becomes unreachable.
    G_OldGen().RegisterFinalizer(obj, callback);
}

// ======================================================================
// chaos_gc_get_total_pause_duration
// ======================================================================

extern "C" CHAOS_IL2CPP_INT64 CHAOS_RUNTIME_ABI_CALL chaos_gc_get_total_pause_duration() noexcept
{
    auto snap = GcGetSnapshot();
    uint64_t total = snap.young_pause_ns_total
                   + snap.gen1_pause_ns_total
                   + snap.full_pause_ns_total;
    return static_cast<CHAOS_IL2CPP_INT64>(total);
}

// ======================================================================
// chaos_gc_get_allocated_bytes_for_current_thread
// ======================================================================

/// Per-thread total allocated bytes counter (monotonically increasing, never reset).
/// Incremented by all allocation paths: NurseryAllocate, NurseryAllocateAtomic,
/// PohAllocate, and fallback paths to old gen / LOH.
thread_local CHAOS_IL2CPP_INT64 tls_total_allocated_bytes = 0;

extern "C" CHAOS_IL2CPP_INT64 CHAOS_RUNTIME_ABI_CALL chaos_gc_get_allocated_bytes_for_current_thread() noexcept
{
    return tls_total_allocated_bytes;
}

// ======================================================================
// Full GC Notification API
// ======================================================================

extern "C" void CHAOS_RUNTIME_ABI_CALL chaos_gc_enable_full_gc_notification(
    CHAOS_IL2CPP_INT32 /*max_generation_threshold*/,
    CHAOS_IL2CPP_INT32 /*large_object_heap_threshold*/) noexcept
{
    G_Scheduler().EnableFullGcNotification();
}

extern "C" void CHAOS_RUNTIME_ABI_CALL chaos_gc_disable_full_gc_notification() noexcept
{
    G_Scheduler().DisableFullGcNotification();
}

extern "C" CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL chaos_gc_wait_for_full_gc_approach(
    CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    bool signaled = G_Scheduler().WaitForFullGcApproach(timeout_ms);
    if (!G_Scheduler().IsNotificationEnabled()) return -1;  // not registered
    return signaled ? 0 : 1;  // 0=success, 1=timeout
}

extern "C" CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL chaos_gc_wait_for_full_gc_complete(
    CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    bool signaled = G_Scheduler().WaitForFullGcComplete(timeout_ms);
    if (!G_Scheduler().IsNotificationEnabled()) return -1;  // not registered
    return signaled ? 0 : 1;  // 0=success, 1=timeout
}

}  // namespace chaos::il2cpp::runtime_core

// ── ABI export wrapper (C-linkage for generated AOT code) ─────────────

extern "C" bool CHAOS_RUNTIME_ABI_CALL chaos_is_gc_pointer(const void* ptr) noexcept {
    return chaos::il2cpp::runtime_core::chaos_is_gc_pointer(ptr);
}
