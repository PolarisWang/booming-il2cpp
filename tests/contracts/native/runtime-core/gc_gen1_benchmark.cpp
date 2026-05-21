/// gc_gen1_benchmark — Gen1 collection performance baseline.
///
/// Measures Gen1 mark-sweep-promote throughput at various occupancy levels
/// and survival rates.  Calls GcGen1Collection() directly (not through
/// GcYoungCollection) to bypass promotion_age_threshold trigger logic,
/// ensuring every run captures a Gen1 collection cycle.
///
/// Run: artifacts/.../chaos_gc_gen1_benchmark.exe
///
/// Sample output:
///   Occupancy | Survival | Gen1 us | Promoted | Reclaimed | Objects
///      25%    |   100%   |   110   |  2048KB  |     0KB   |  32768
///      25%    |     0%   |    12   |     0KB  |  2048KB   |      0
///      90%    |    50%   |   580   |  3600KB  |  3600KB   |  59000

#include <chaos/native_types.h>

#include "gc_gen1.h"
#include "gc_scheduler.h"
#include "gc_young_gen.h"
#include "gc_young_collector.h"
#include "gc_region.h"
#include "gc_old_gen.h"
#include "gc_layout.h"
#include "gc_stats.h"
#include "thread_state.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace chaos::il2cpp::runtime_core;

// ── TestTypeInfo (same pattern as gc_gen1_test.cpp) ────────────────────

struct alignas(8) TestTypeInfo {
    uint64_t stable_id;
    uint64_t reserved[3];
};

static const void* SetupTestType(uint32_t instance_size) {
    uint64_t stable_id = GcLayoutRegistry::Instance()
        .RegisterOrGetRawAllocType(instance_size);
    static TestTypeInfo s_ti{};
    s_ti.stable_id = stable_id;
    auto* reg = &GcLayoutRegistry::Instance();
    uintptr_t ti_addr = reinterpret_cast<uintptr_t>(&s_ti);
    reg->RegisterTypeInfoRange(ti_addr, ti_addr + sizeof(TestTypeInfo));
    return &s_ti;
}

static const void* g_test_type_info = nullptr;

/// Object size for all Gen1 objects in this benchmark.
static constexpr CHAOS_IL2CPP_SIZE kObjSize = 64;

// ── Gen1 helpers ─────────────────────────────────────────────────────

/// Gen1 capacity (16 MB default).
static CHAOS_IL2CPP_SIZE Gen1Capacity() {
    auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    if (gen1 == nullptr) return 0;
    return static_cast<CHAOS_IL2CPP_SIZE>(
        g_young_gen.gen1_end - gen1->begin);
}

/// Clear nursery data to prevent stale Gen0→Gen1 references.
static void ClearNursery() {
    auto* nursery = g_young_gen.region.load(std::memory_order_acquire);
    if (nursery == nullptr) return;
    auto* n_end = g_young_gen.bump.load(std::memory_order_acquire);
    if (n_end != nullptr && n_end > nursery->begin) {
        std::memset(nursery->begin, 0,
                    static_cast<size_t>(n_end - nursery->begin));
    }
}

// ── Measurement ──────────────────────────────────────────────────────

struct Measurement {
    double occupancy_pct;        // e.g. 25.0
    double survival_pct;         // e.g. 50.0
    uint64_t gen1_us;            // Gen1 collection pause (us)
    uint64_t promoted_bytes;     // bytes promoted to Gen2
    uint64_t reclaimed_bytes;    // bytes reclaimed from Gen1
    int      objects_in_gen1;    // before collection
    int      objects_promoted;
};

/// Fill Gen1 to @a target_bytes (using @a kObjSize objects), keeping
/// @a survival_fraction of objects alive via Gen0 nursery references.
/// Returns the number of objects created.
static int FillGen1(CHAOS_IL2CPP_SIZE target_bytes, double survival_fraction) {
    int count = 0;
    auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    if (gen1 == nullptr) return 0;
    char* s_begin = gen1->begin;
    CHAOS_IL2CPP_SIZE used = static_cast<CHAOS_IL2CPP_SIZE>(
        g_young_gen.gen1_bump.load(std::memory_order_acquire) - s_begin);

    while (used < target_bytes) {
        void* obj = TryAllocateInGen1(kObjSize);
        if (obj == nullptr) break;  // Gen1 full

        // Write TypeInfo header.
        *static_cast<const void**>(obj) = g_test_type_info;

        // Create Gen0 nursery reference for a fraction of objects.
        // Use simple modulo for deterministic survival.
        // If survival_fraction == 1.0: keep all.
        // If survival_fraction == 0.5: keep every other.
        // If survival_fraction == 0.0: keep none.
        if (survival_fraction == 1.0 ||
            (survival_fraction == 0.5 && (count % 2 == 0))) {
            void* gen0_ref = NurseryAllocate(kObjSize);
            if (gen0_ref != nullptr) {
                std::memset(gen0_ref, 0, kObjSize);
                std::memcpy(static_cast<char*>(gen0_ref) + 8, &obj, sizeof(void*));
            }
        }

        count++;
        used = static_cast<CHAOS_IL2CPP_SIZE>(
            g_young_gen.gen1_bump.load(std::memory_order_acquire) - s_begin);
    }

    return count;
}

/// Run one measurement at the given occupancy and survival rate.
/// Ensures Gen1 is freshly filled (previous collection state is irrelevant).
///
/// Calls GcGen1Collection() directly (not through GcYoungCollection) to
/// bypass the promotion_age_threshold trigger logic.  This guarantees
/// every measurement captures a Gen1 mark-sweep cycle at the requested
/// occupancy and survival rate.
static Measurement RunOne(const char* occ_label, double occ_pct,
                          CHAOS_IL2CPP_SIZE target_bytes,
                          double survival_pct) {
    Measurement m{};
    m.occupancy_pct = occ_pct;
    m.survival_pct = survival_pct;

    // Fully reset Gen1 and nursery to eliminate interference from prior run.
    ClearNursery();
    // Also reset the nursery bump pointer so subsequent NurseryAllocate calls
    // in FillGen1 start from a clean nursery rather than the prior run's
    // bump position.  Without this, high-survival fills can overflow the
    // 8 MB nursery and trigger premature young GCs during the fill phase.
    {
        auto* nursery = g_young_gen.region.load(std::memory_order_acquire);
        if (nursery != nullptr) {
            g_young_gen.bump.store(nursery->begin, std::memory_order_release);
        }
    }
    GcGen1Collection();  // resets gen1_bump

    // Fill Gen1.
    int total_objects = FillGen1(target_bytes, survival_pct / 100.0);
    m.objects_in_gen1 = total_objects;

    // Snapshot before.
    GcSnapshot before = GcGetSnapshot();

    // Run Gen1 collection directly (not through GcYoungCollection) so every
    // measurement captures a Gen1 mark-sweep regardless of gc_count parity.
    // This is the correct approach for a microbenchmark — we want to measure
    // Gen1 throughput in isolation, not young-GC dispatch policy.
    Gen1CollectionResult gr = GcGen1Collection();

    // Record stats into the global GC stats so GcGetSnapshot picks them up.
    // (GcGen1Collection does NOT call GcRecordGen1Collection itself — that
    //  is normally done by GcYoungCollection's Phase 4 or the full-GC path.)
    GcRecordGen1Collection(
        gr.objects_promoted, gr.bytes_promoted,
        gr.bytes_reclaimed, gr.pause_ns);
    g_gc_scheduler.RecordGen1Collection(gr.bytes_promoted, gr.pause_ns);

    // Snapshot after.
    GcSnapshot after = GcGetSnapshot();

    // Compute Gen1 deltas.
    uint64_t gen1_ns = after.gen1_pause_ns_total - before.gen1_pause_ns_total;

    m.gen1_us         = gen1_ns / 1000;
    m.promoted_bytes  = after.gen1_bytes_promoted - before.gen1_bytes_promoted;
    m.reclaimed_bytes = after.gen1_bytes_reclaimed - before.gen1_bytes_reclaimed;
    m.objects_promoted = static_cast<int>(
        after.gen1_objects_promoted - before.gen1_objects_promoted);

    return m;
}

static void PrintHeader() {
    printf("  %-10s | %-9s | %-8s | %-9s | %-9s | %-7s\n",
           "Occupancy", "Survival", "Gen1 us", "Promoted",
           "Reclaimed", "ObjProm");
    printf("  %-10s-+-%-9s-+-%-8s-+-%-9s-+-%-9s-+-%-7s\n",
           "----------", "---------", "--------", "---------",
           "---------", "-------");
}

static void PrintMeasurement(const Measurement& m) {
    const char* occ_str = nullptr;
    if (m.occupancy_pct == 25.0) occ_str = "25%";
    else if (m.occupancy_pct == 50.0) occ_str = "50%";
    else if (m.occupancy_pct == 75.0) occ_str = "75%";
    else if (m.occupancy_pct == 90.0) occ_str = "90%";
    else if (m.occupancy_pct == 95.0) occ_str = "95%";
    else if (m.occupancy_pct == 98.0) occ_str = "98%";
    else occ_str = "???";

    const char* surv_str = nullptr;
    if (m.survival_pct == 0.0) surv_str = "0%";
    else if (m.survival_pct == 50.0) surv_str = "50%";
    else if (m.survival_pct == 100.0) surv_str = "100%";
    else surv_str = "???";

    // Format bytes as KB.
    uint64_t prom_kb = m.promoted_bytes / 1024;
    uint64_t recl_kb = m.reclaimed_bytes / 1024;

    printf("  %-10s | %-9s | %-8llu | %-4lluKB  | %-4lluKB  | %-7d\n",
           occ_str, surv_str,
           static_cast<unsigned long long>(m.gen1_us),
           static_cast<unsigned long long>(prom_kb),
           static_cast<unsigned long long>(recl_kb),
           m.objects_promoted);
}

// ═══════════════════════════════════════════════════════════════════════
// Drain Comparison: Mark-Sweep vs Promote-All
// ═══════════════════════════════════════════════════════════════════════

struct DrainRow {
    double occ_pct;
    double surv_pct;
    uint64_t mark_sweep_us;
    uint64_t drain_us;
    double ratio;
};

/// Measure a raw drain (promote-all) time for the current Gen1 content.
/// Walks [gen1->begin, gen1_bump), copies every object to Gen2.
/// Returns pause in ns.
static uint64_t MeasureDrain() {
    auto t0 = std::chrono::steady_clock::now();

    auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    if (gen1 == nullptr) return 0;
    char* s_begin = gen1->begin;
    char* s_bump  = g_young_gen.gen1_bump.load(std::memory_order_acquire);
    auto& layout_registry = GcLayoutRegistry::Instance();

    char* cur = s_begin;
    while (cur < s_bump) {
        const void* ti = *reinterpret_cast<const void* const*>(cur);
        CHAOS_IL2CPP_SIZE obj_size = kGen1MaxEstObjectSize;
        if (ti != nullptr && layout_registry.IsValidTypeInfoPointer(ti)) {
            uint64_t sid = layout_registry.ReadStableId(ti);
            const auto* layout = layout_registry.Lookup(sid);
            if (layout != nullptr && layout->instance_size > 0) {
                obj_size = static_cast<CHAOS_IL2CPP_SIZE>(layout->instance_size);
            }
        }
        void* gen2_addr = g_old_gen.Allocate(obj_size, true);
        if (gen2_addr != nullptr) {
            std::memcpy(gen2_addr, cur, obj_size);
        }
        cur += obj_size;
    }

    auto t1 = std::chrono::steady_clock::now();
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count());
}

static void PrintDrainHeader() {
    printf("\n%s\n", "  Mark-Sweep vs Drain (Promote-All) Comparison");
    printf("  %-10s | %-9s | %-11s | %-9s | %-6s\n",
           "Occupancy", "Survival", "MarkSweep_us", "Drain_us", "Ratio");
    printf("  %-10s-+-%-9s-+-%-11s-+-%-9s-+-%-6s\n",
           "----------", "---------", "-----------", "---------", "------");
}

static void PrintDrainRow(const DrainRow& r) {
    const char* occ_str = (r.occ_pct == 25.0) ? "25%" :
                          (r.occ_pct == 50.0) ? "50%" :
                          (r.occ_pct == 75.0) ? "75%" :
                          (r.occ_pct == 90.0) ? "90%" :
                          (r.occ_pct == 95.0) ? "95%" : "98%";
    const char* surv_str = (r.surv_pct == 0.0)   ? "0%" :
                           (r.surv_pct == 50.0)  ? "50%" : "100%";
    printf("  %-10s | %-9s | %-11llu | %-9llu | %-6.2f\n",
           occ_str, surv_str,
           static_cast<unsigned long long>(r.mark_sweep_us),
           static_cast<unsigned long long>(r.drain_us),
           r.ratio);
}

static void RunDrainComparison(CHAOS_IL2CPP_SIZE capacity) {
    PrintDrainHeader();

    struct { double pct; CHAOS_IL2CPP_SIZE bytes; } levels[] = {
        {25.0, static_cast<CHAOS_IL2CPP_SIZE>(capacity * 0.25)},
        {50.0, static_cast<CHAOS_IL2CPP_SIZE>(capacity * 0.50)},
        {75.0, static_cast<CHAOS_IL2CPP_SIZE>(capacity * 0.75)},
        {90.0, static_cast<CHAOS_IL2CPP_SIZE>(capacity * 0.90)},
        {95.0, static_cast<CHAOS_IL2CPP_SIZE>(capacity * 0.95)},
        {98.0, static_cast<CHAOS_IL2CPP_SIZE>(capacity * 0.98)},
    };
    double survivals[] = {0.0, 50.0, 100.0};

    for (auto& level : levels) {
        for (double surv : survivals) {
            // ── Mark-sweep measurement ──
            ClearNursery();
            {
                auto* nursery = g_young_gen.region.load(std::memory_order_acquire);
                if (nursery != nullptr)
                    g_young_gen.bump.store(nursery->begin, std::memory_order_release);
            }
            GcGen1Collection();
            FillGen1(level.bytes, surv / 100.0);
            Gen1CollectionResult gr = GcGen1Collection();
            uint64_t ms_ns = gr.pause_ns;

            // ── Drain measurement (refill + raw copy all) ──
            ClearNursery();
            {
                auto* nursery = g_young_gen.region.load(std::memory_order_acquire);
                if (nursery != nullptr)
                    g_young_gen.bump.store(nursery->begin, std::memory_order_release);
            }
            GcGen1Collection();
            FillGen1(level.bytes, surv / 100.0);
            uint64_t drain_ns = MeasureDrain();
            // Reset Gen1 after drain (MeasureDrain does NOT reset gen1_bump).
            g_young_gen.gen1_bump.store(s_begin,
                                        std::memory_order_release);

            DrainRow row{};
            row.occ_pct = level.pct;
            row.surv_pct = surv;
            row.mark_sweep_us = ms_ns / 1000;
            row.drain_us = drain_ns / 1000;
            row.ratio = (drain_ns > 0)
                ? static_cast<double>(ms_ns) / static_cast<double>(drain_ns)
                : 0.0;
            PrintDrainRow(row);
        }
        printf("\n");
    }

    puts("Notes:");
    puts("  - MarkSweep_us = GcGen1Collection pause (mark bitmap + root scan + promote-live)");
    puts("  - Drain_us = raw memcpy promote-all (no marking, copies ALL objects to Gen2)");
    puts("  - Ratio < 1.0 = mark-sweep faster (expected at low survival: fewer objects copied)");
    puts("  - Ratio ~1.0 = both similar (100% survival: same number of objects copied)");
    puts("  - Ratio > 1.0 possible if mark-sweep overhead is dominated by root scan cost\n");
}

// ═══════════════════════════════════════════════════════════════════════
// Main
// ═══════════════════════════════════════════════════════════════════════

int main() {
    setbuf(stdout, NULL);
    puts("Gen1 Performance Baseline");
    puts("════════════════════════\n");

    // ── Setup ──
    uint32_t tid = threading::AllocateThreadId();
    threading::RegisterThread(tid, nullptr);
    threading::EnterCooperativeMode();

    InitYoungGeneration();
    g_test_type_info = SetupTestType(kObjSize);
    if (g_test_type_info == nullptr) {
        puts("FATAL: SetupTestType failed");
        return 1;
    }
    // Warm up old gen.
    g_old_gen.Allocate(8, true);

    CHAOS_IL2CPP_SIZE capacity = Gen1Capacity();
    printf("  Gen1 capacity: %llu bytes (%.1f MB)\n",
           static_cast<unsigned long long>(capacity),
           static_cast<double>(capacity) / (1024.0 * 1024.0));
    printf("  Object size: %llu bytes\n\n",
           static_cast<unsigned long long>(kObjSize));

    // ── Occupancy levels ──
    struct { double pct; CHAOS_IL2CPP_SIZE bytes; } levels[] = {
        {25.0, static_cast<CHAOS_IL2CPP_SIZE>(capacity * 0.25)},
        {50.0, static_cast<CHAOS_IL2CPP_SIZE>(capacity * 0.50)},
        {75.0, static_cast<CHAOS_IL2CPP_SIZE>(capacity * 0.75)},
        {90.0, static_cast<CHAOS_IL2CPP_SIZE>(capacity * 0.90)},
        {95.0, static_cast<CHAOS_IL2CPP_SIZE>(capacity * 0.95)},
        {98.0, static_cast<CHAOS_IL2CPP_SIZE>(capacity * 0.98)},
    };
    double survivals[] = {0.0, 50.0, 100.0};

    PrintHeader();

    // Warmup: fill Gen1 at 10% occupancy then collect to ensure all paths are
    // JIT-compiled and hardware caches are warm.
    ClearNursery();
    GcGen1Collection();
    int warmup_objs = FillGen1(capacity * 0.10, 1.0);
    (void)warmup_objs;
    GcGen1Collection();

    for (auto& level : levels) {
        for (double surv : survivals) {
            Measurement m = RunOne(
                nullptr, level.pct, level.bytes, surv);
            PrintMeasurement(m);

            // Extra newline between occupancy groups for readability.
        }
        printf("\n");
    }

    // ── Drain Comparison Table ──
    RunDrainComparison(capacity);

    // ── Teardown ──
    threading::UnregisterThread();

    puts("\nNotes:");
    puts("  - Gen1 us = Gen1 mark-sweep-promote pause (GcGen1Collection direct)");
    puts("  - Promoted = bytes copied from Gen1 to Gen2 (live objects)");
    puts("  - Reclaimed = bytes freed in Gen1 (dead objects)");
    puts("  - 0% survival may show false-positive promotion (~1-2 objects) due to");
    puts("    conservative stack scanning (not real liveness).");
    puts("  - Called GcGen1Collection() directly (not through GcYoungCollection)");
    puts("    to isolate Gen1 throughput from young-GC dispatch policy.\n");
    puts("OOM fallback (gc_gen1.cpp:149-195):");
    puts("  - If CHAOS_IL2CPP_MALLOC fails for the mark bitmap (128KB for full 16MB");
    puts("    Gen1), GcGen1Collection falls back to promote-all (drain). Every");
    puts("    object is unconditionally promoted to Gen2 via g_old_gen.Allocate +");
    puts("    memcpy. gen1_bump IS reset on this path even if Gen2 OOM occurs");
    puts("    (data loss is accepted over dangling pointers).");
    puts("  - In practice, a 128KB allocation never fails on desktop/server, so");
    puts("    this fallback is a safety net for constrained memory environments.\n");

    return 0;
}
