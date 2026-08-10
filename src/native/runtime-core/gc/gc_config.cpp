// gc_config.cpp — CRAG GC configuration knobs (env-driven, programmatic API)

#include "gc_config.h"

#include <chaos/log.h>

#include <cstdlib>   // std::getenv
#include <cstring>

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// Single process-wide config singleton.
// ======================================================================

static GcConfigImpl g_config;

GcConfigImpl& GcConfig() { return g_config; }

// ======================================================================
// Env-name table (one entry per INT_CONFIG key, matching header order).
// ======================================================================

namespace {

struct EnvOverride {
    CHAOS_IL2CPP_SIZE* dst;
    const char*         env_suffix;
    CHAOS_IL2CPP_SIZE   fallback;
};

/// Parse a non-negative integer from an env string.  Returns the parsed value
/// if the string is a valid non-negative integer, else the fallback.
static CHAOS_IL2CPP_SIZE ParseNonNegative(const char* s,
                                          CHAOS_IL2CPP_SIZE fallback) noexcept {
    if (s == nullptr || *s == '\0') return fallback;
    // Trim leading whitespace.
    while (*s == ' ' || *s == '\t') ++s;
    if (*s == '-') return fallback;  // reject negatives
    char* end = nullptr;
    unsigned long long v = std::strtoull(s, &end, 10);
    if (end == s || (*end != '\0' && *end != '\n')) return fallback;
    return static_cast<CHAOS_IL2CPP_SIZE>(v);
}

static float ParsePositiveFloat(const char* s, float fallback) noexcept {
    if (s == nullptr || *s == '\0') return fallback;
    char* end = nullptr;
    double v = std::strtod(s, &end);
    if (end == s || (*end != '\0' && *end != '\n')) return fallback;
    return static_cast<float>(v);
}

}  // anonymous namespace

void GcConfigImpl::Initialize() noexcept {
    // Set the compile-time fallbacks first (mirror the historical defaults).
#define INT_CONFIG(NAME, ENV, DEFAULT, DOC) NAME = (DEFAULT);
    CHAOS_GC_CONFIGURATION_KEYS
#undef INT_CONFIG

    // Any non-zero env override wins.  Names are CHAOS_GC_<EnvSuffix>.
#define INT_CONFIG(NAME, ENV, DEFAULT, DOC)                              \
    do {                                                                  \
        const char* _ev = std::getenv("CHAOS_GC_" ENV);                  \
        if (_ev != nullptr && *_ev != '\0') {                            \
            CHAOS_IL2CPP_SIZE _v = ParseNonNegative(_ev, (CHAOS_IL2CPP_SIZE)(DEFAULT)); \
            NAME = _v;                                                    \
        }                                                                 \
    } while (0);
    CHAOS_GC_CONFIGURATION_KEYS
#undef INT_CONFIG

    (void)ParsePositiveFloat;  // (reserved for future float knobs)
    CHAOS_IL2CPP_LOG_DEBUG_M("GCConfig",
        "default_nursery=%llu default_gen1=%llu max_tlab_alloc=%llu "
        "loh_threshold=%llu parallel_mark_workers=%llu hard_limit_mb=%llu soft_limit_mb=%llu",
        static_cast<unsigned long long>(DefaultNurserySize),
        static_cast<unsigned long long>(DefaultGen1Size),
        static_cast<unsigned long long>(MaxTlabAlloc),
        static_cast<unsigned long long>(LohThreshold),
        static_cast<unsigned long long>(ParallelMarkWorkers),
        static_cast<unsigned long long>(HeapHardLimitMB),
        static_cast<unsigned long long>(HeapSoftLimitMB));
}

}  // namespace chaos::il2cpp::runtime_core

// ======================================================================
// Native programmatic-tuning API (AOT & JIT callable).
// ======================================================================

extern "C" {

CHAOS_IL2CPP_SIZE CHAOS_RUNTIME_ABI_CALL chaos_gc_config_get_nursery_size() noexcept {
    return chaos::il2cpp::runtime_core::GcConfig().DefaultNurserySize;
}

void CHAOS_RUNTIME_ABI_CALL chaos_gc_config_set_nursery_size(
    CHAOS_IL2CPP_SIZE bytes) noexcept {
    if (bytes >= 4 * 1024) {  // sanity: >= 4 KB
        chaos::il2cpp::runtime_core::GcConfig().DefaultNurserySize = bytes;
    }
}

void CHAOS_RUNTIME_ABI_CALL chaos_gc_config_set_heap_hard_limit_mb(
    CHAOS_IL2CPP_INT32 mb) noexcept {
    if (mb >= 0) {
        chaos::il2cpp::runtime_core::GcConfig().HeapHardLimitMB =
            static_cast<CHAOS_IL2CPP_SIZE>(mb);
    }
}

void CHAOS_RUNTIME_ABI_CALL chaos_gc_config_set_heap_soft_limit_mb(
    CHAOS_IL2CPP_INT32 mb) noexcept {
    if (mb >= 0) {
        chaos::il2cpp::runtime_core::GcConfig().HeapSoftLimitMB =
            static_cast<CHAOS_IL2CPP_SIZE>(mb);
    }
}

}  // extern "C"
