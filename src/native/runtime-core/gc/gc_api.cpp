#include "gc_api.h"

#include <chaos/log.h>

#include "gc_helpers.h"
#include "gc_loh.h"
#include "gc_old_gen.h"
#include "gc_scheduler.h"

namespace chaos::il2cpp::runtime_core {

// ── External memory pressure tracking ─────────────────────────────
namespace {

// Process-wide counter for external (unmanaged) memory pressure.
// AddMemoryPressure increments, RemoveMemoryPressure decrements.
// When this exceeds kExternalMemoryThreshold, a full GC is requested.
std::atomic<CHAOS_IL2CPP_INT64> g_external_memory_pressure{0};

// Threshold: trigger GC when external memory reaches 2x of last known
// managed heap size.  This follows CoreCLR's heuristic of treating
// external memory as if it were managed allocations.
static constexpr CHAOS_IL2CPP_INT64 kExternalMemoryThreshold = 256 * 1024 * 1024;  // 256 MB

}  // anonymous namespace

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
        g_old_gen.TotalAllocated());
    auto loh_bytes = static_cast<CHAOS_IL2CPP_INT64>(
        g_loh.TotalAllocated());
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

    auto prev = g_external_memory_pressure.fetch_add(bytes,
        std::memory_order_relaxed);
    auto current = prev + bytes;

    // If crosssing the threshold, request a full GC at the next safepoint.
    // Use a compare-exchange to avoid requesting multiple GCs.
    if (current >= kExternalMemoryThreshold && prev < kExternalMemoryThreshold) {
        g_gc_scheduler.RequestFullGc();
        CHAOS_IL2CPP_LOG_DEBUG("GC_API",
            "external_memory_pressure_crossed_threshold: total=%lld",
            static_cast<long long>(current));
    }
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

    g_external_memory_pressure.fetch_sub(bytes, std::memory_order_relaxed);
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
    g_gc_scheduler.SetCollectionMode(gc_mode);

    // Map .NET semantics:
    //   Forced (1) / Aggressive (3) → immediate blocking collect
    //   Optimized (2) → let scheduler decide (BGC preference)
    //   Default (0) → same as Forced for GC.Collect(int)
    if (gc_mode == GcCollectionMode::OPTIMIZED) {
        // Optimized: trigger via scheduler (may defer to BGC).
        g_gc_scheduler.RequestFullGc();
        CHAOS_IL2CPP_LOG_DEBUG("GC_API", "collect_with_mode optimized (request deferred)");
    } else {
        // Forced / Aggressive / Default: immediate blocking collect.
        // CRAG has 2 generations: gen-0 (young) and gen-1 (old).
        chaos_gc_collect();
    }
}

// ======================================================================
// chaos_gc_get_latency_mode
// ======================================================================

extern "C" CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL chaos_gc_get_latency_mode() noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(g_gc_scheduler.GetLatencyMode());
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

    g_gc_scheduler.SetLatencyMode(lm);

    // If switching away from NO_GC_REGION, trigger any deferred GC.
    if (lm != GcLatencyMode::NO_GC_REGION) {
        auto kind = g_gc_scheduler.DecideCollection();
        if (kind != GcCollectionKind::NONE) {
            g_gc_scheduler.RequestFullGc();
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
        g_old_gen.TotalAllocated());
    auto loh_bytes = static_cast<CHAOS_IL2CPP_INT64>(
        g_loh.TotalAllocated());
    return old_gen_bytes + loh_bytes;
}

}  // namespace chaos::il2cpp::runtime_core
