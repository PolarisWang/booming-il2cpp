// gc_config.h — CRAG GC runtime configuration knobs (align CoreCLR gcconfig)
//
// CoreCLR declares all GC tunables as a single GC_CONFIGURATION_KEYS macro list
// (gcconfig.h), expanded into per-key accessors driven by (a) environment
// variables and (b) managed switches.  CRAG adapts this to il2cpp:
//
//   - Environment override:  CHAOS_GC_<EnvSuffix> read via std::getenv
//     (cross-platform: std/env is identical on Windows/Linux/macOS/iOS/Android).
//   - Programmatic tuning:    native chaos_gc_config_set_<name>() API callable
//     from both AOT (static codegen) and JIT code, mirroring CoreCLR's
//     System.GC.* switches.
//   - Internal access:        GcConfig::Get<Name>().
//
// This keeps GC parameters runtime-tunable (no recompile) and platform-neutral,
// while preserving AOT/JIT compatibility through a plain C interface.

#ifndef CHAOS_IL2CPP_GC_CONFIG_H_
#define CHAOS_IL2CPP_GC_CONFIG_H_

#include <chaos/native_types.h>
#include <runtime_abi.h>

namespace chaos::il2cpp::runtime_core {

// ── Config key table (CoreCLR-style, single DRY source) ────────────────
// Each key:  KIND_CONFIG(Name, EnvSuffix, Default, Doc)
//   INT/BOOL kinds supported.  Internal defaults mirror the historical
//   compile-time constants; env override (CHAOS_GC_<EnvSuffix>) wins.
//
// Every key MUST be consumed by a real runtime path (read at init / GC-decision
// time, or latched into a hot-path cache).  Dead keys are not permitted — if a
// knob no longer drives any behavior it is removed, not left as a no-op.
//
// Note on float knobs: fractional tunables (trigger multipliers, fragmentation
// thresholds) are stored as fixed-point *1000 CHAOS_IL2CPP_SIZE to keep the
// config struct INT-only (mirrors GcScheduler::SetOldGenFragmentation fp*1000).
#define CHAOS_GC_CONFIGURATION_KEYS \
    /* ── existing core knobs (wired in InitYoungGeneration / diagnostics) ── */ \
    INT_CONFIG(DefaultNurserySize,      "NurserySize",          64 * 1024 * 1024, "nursery region size (bytes)")         \
    INT_CONFIG(DefaultGen1Size,         "Gen1Size",             0,               "gen1 survivor size (0=default=nursery)") \
    INT_CONFIG(MaxTlabAlloc,            "MaxTlabAlloc",         32 * 1024,       "largest in-TLAB allocation (bytes)")    \
    INT_CONFIG(LohThreshold,            "LOHThreshold",         85 * 1024,       "large-object-heap threshold (bytes)")   \
    INT_CONFIG(ParallelMarkWorkers,     "ParallelMarkWorkers",  8,               "max parallel-mark workers")             \
    INT_CONFIG(HeapHardLimitMB,         "HeapHardLimitMB",      0,               "hard heap limit (MB, 0=disabled)")       \
    INT_CONFIG(HeapSoftLimitMB,         "HeapSoftLimitMB",      0,               "soft heap limit (MB, 0=disabled)")        \
    INT_CONFIG(HeapVerify,              "HeapVerify",           0,               "GC self-verify level (0=off,1=region-gen,2=full)") \
    /* ── scheduler adaptive sizing (read at GC-decision time) ── */ \
    INT_CONFIG(YoungTriggerMultiplierFP, "YoungTriggerMultiplier", 2000,         "young-GC trigger mult (fp*1000; 2000=2.0x)/ last nursery") \
    INT_CONFIG(FullTriggerMultiplierFP,  "FullTriggerMultiplier",  2000,         "full-GC trigger mult (fp*1000; 2000=2.0x)/ est heap")   \
    INT_CONFIG(HighPressureTriggerMultiplierFP, "HighPressureTriggerMultiplier", 3000, "high-pressure trigger mult (fp*1000)") \
    INT_CONFIG(CooldownAllocations,     "CooldownAllocations",  256,             "alloc cooldown skips after each GC")      \
    INT_CONFIG(MinGcIntervalMs,         "MinGcIntervalMs",      50,              "min interval between GC completions (ms)") \
    INT_CONFIG(Gen1MinPromotionAge,     "Gen1MinPromotionAge",  1,               "min gen1 promotion age threshold")        \
    INT_CONFIG(Gen1MaxPromotionAge,     "Gen1MaxPromotionAge",  12,              "max gen1 promotion age threshold")         \
    INT_CONFIG(Gen1MaxNsPerByte,        "Gen1MaxNsPerByte",     10,              "max ns per promoted byte before age+nudge") \
    INT_CONFIG(MinNurserySize,          "MinNurserySize",       64 * 1024,       "adaptive nursery size floor (bytes)")     \
    INT_CONFIG(MaxNurserySize,          "MaxNurserySize",       4 * 1024 * 1024, "adaptive nursery size cap (bytes)")       \
    INT_CONFIG(MinGen1Size,             "MinGen1Size",          4 * 1024 * 1024, "adaptive gen1 size floor (bytes)")        \
    INT_CONFIG(MaxGen1Size,             "MaxGen1Size",          32 * 1024 * 1024,"adaptive gen1 size cap (bytes)")          \
    /* ── diagnostics ── */ \
    INT_CONFIG(DumpStatsIntervalSec,    "DumpStatsIntervalSec", 0,               "periodic GC stats dump interval (s, 0=off)") \
    /* ── BGC concurrent marking ── */ \
    INT_CONFIG(BgcWorkers,              "BgcWorkers",           8,               "max background-GC worker threads")        \
    INT_CONFIG(MarkSliceBudgetUs,       "MarkSliceBudgetUs",    2000,            "BGC mark slice budget (us)")              \
    /* ── old-gen compaction / reserve ── */ \
    INT_CONFIG(CrossPageFragThresholdFP,"CrossPageFragThreshold", 300,           "old-gen cross-page evacuate frag (fp*1000)") \
    INT_CONFIG(EmergencyReserveSize,    "EmergencyReserveSize", 64 * 1024,       "emergency reserve for OOM (bytes)")

// ── Internal config accessor ───────────────────────────────────────────
// A single struct populated once at init from env (or compile-time defaults).
// Avoids macro-generated method baggage while keeping the mental model of a
// key table.  Values are plain scalars (const after Initialize).
struct GcConfigImpl {
    // Fields are value-initialized to their DEFAULT at construction (not 0), so
    // reads before GcConfig().Initialize() still return the correct baked-in
    // default rather than a spurious 0.  This keeps consumers (scheduler sizing,
    // trigger logic, allocator latches) correct even when a code path reads
    // config before the GC is fully initialized.
#define INT_CONFIG(NAME, ENV, DEFAULT, DOC) CHAOS_IL2CPP_SIZE NAME = (DEFAULT);
    CHAOS_GC_CONFIGURATION_KEYS
#undef INT_CONFIG

    /// Read CHAOS_GC_<ENV> for each key (std::getenv), or fall back to the
    /// compile-time default when absent/unparseable.
    void Initialize() noexcept;
};

/// Process-wide GC config singleton (read after runtime init).
GcConfigImpl& GcConfig();

namespace detail {
// Env-name + default helper for the Initialize() implementation.
inline const char* KeyEnvName(CHAOS_IL2CPP_UINT32 _ /*key_index*/) noexcept { return ""; }
}  // namespace detail

}  // namespace chaos::il2cpp::runtime_core

// ── Native programmatic-tuning API (AOT & JIT callable) ────────────────
extern "C" {

/// Set the GC nursery region size (bytes).  Callable from AOT codegen and
/// JIT helpers to override the env-driver default at runtime.
extern "C" CHAOS_IL2CPP_SIZE CHAOS_RUNTIME_ABI_CALL chaos_gc_config_get_nursery_size() noexcept;
extern "C" void CHAOS_RUNTIME_ABI_CALL chaos_gc_config_set_nursery_size(
    CHAOS_IL2CPP_SIZE bytes) noexcept;

/// Set the hard heap limit (MB; 0 = disabled).
extern "C" void CHAOS_RUNTIME_ABI_CALL chaos_gc_config_set_heap_hard_limit_mb(
    CHAOS_IL2CPP_INT32 mb) noexcept;

/// Set the soft heap limit (MB; 0 = disabled).
extern "C" void CHAOS_RUNTIME_ABI_CALL chaos_gc_config_set_heap_soft_limit_mb(
    CHAOS_IL2CPP_INT32 mb) noexcept;

}  // extern "C"

#endif  // CHAOS_IL2CPP_GC_CONFIG_H_
