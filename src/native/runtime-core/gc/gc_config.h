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
#define CHAOS_GC_CONFIGURATION_KEYS \
    INT_CONFIG(DefaultNurserySize,   "NurserySize",      64 * 1024 * 1024, "nursery region size (bytes)")            \
    INT_CONFIG(DefaultGen1Size,      "Gen1Size",         0,                "gen1 survivor size (0=default=nursery)")  \
    INT_CONFIG(MaxTlabAlloc,         "MaxTlabAlloc",     32 * 1024,        "largest in-TLAB allocation (bytes)")     \
    INT_CONFIG(LohThreshold,         "LOHThreshold",     85 * 1024,        "large-object-heap threshold (bytes)")    \
    INT_CONFIG(ParallelMarkWorkers,  "ParallelMarkWorkers", 8,             "max parallel-mark workers")              \
    INT_CONFIG(HeapHardLimitMB,      "HeapHardLimitMB",  0,                "hard heap limit (MB, 0=disabled)")        \
    INT_CONFIG(HeapSoftLimitMB,      "HeapSoftLimitMB",  0,                "soft heap limit (MB, 0=disabled)")         \
    INT_CONFIG(HeapVerify,           "HeapVerify",       0,                "GC self-verify level (0=off,1=region-gen,2=full)")

// ── Internal config accessor ───────────────────────────────────────────
// A single struct populated once at init from env (or compile-time defaults).
// Avoids macro-generated method baggage while keeping the mental model of a
// key table.  Values are plain scalars (const after Initialize).
struct GcConfigImpl {
#define INT_CONFIG(NAME, ENV, DEFAULT, DOC) CHAOS_IL2CPP_SIZE NAME;
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
