#ifndef CHAOS_IL2CPP_COMMON_CONFIG_H_
#define CHAOS_IL2CPP_COMMON_CONFIG_H_

// ── Build configuration tiers ──────────────────────────────────────────
//
// Exactly one of these must be defined at build time:
//
//   CHAOS_IL2CPP_CONFIG_CHECK   — Full debug: all log levels, trace
//                                  events, runtime assertions.
//                                  Intended for daily development.
//
//   CHAOS_IL2CPP_CONFIG_PROFILE — Performance profiling: INFO+ logging,
//                                  trace events, no assertions.
//
//   CHAOS_IL2CPP_CONFIG_SHIP    — Release: ERROR-only logging, no trace
//                                  events, no assertions.
//
// Default: CHECK (when none defined).
// ============================================================================

#if !defined(CHAOS_IL2CPP_CONFIG_CHECK) && \
    !defined(CHAOS_IL2CPP_CONFIG_PROFILE) && \
    !defined(CHAOS_IL2CPP_CONFIG_SHIP)
// Safe default: full debug for day-to-day development.
#  define CHAOS_IL2CPP_CONFIG_CHECK
#endif

// ── Compile-time assertion macro ─────────────────────────────────────────
// CHAOS_IL2CPP_ASSERT(cond)  — abort if condition false (CHECK only).
//                              no-op in PROFILE and SHIP.

#if defined(CHAOS_IL2CPP_CONFIG_CHECK)
#  define CHAOS_IL2CPP_ASSERT(cond)  do { \
        if (!(cond)) { CHAOS_IL2CPP_ABORT(); } \
    } while (0)
#else
#  define CHAOS_IL2CPP_ASSERT(cond)  ((void)0)
#endif

// ── Log level ───────────────────────────────────────────────────────────
// Maps directly to CHAOS_IL2CPP_LOG_LEVEL in log.h.
//   CHECK   → 3 (DEBUG + INFO + WARN + ERROR)
//   PROFILE → 2 (INFO + WARN + ERROR)
//   SHIP    → 0 (ERROR only)

#if defined(CHAOS_IL2CPP_CONFIG_CHECK)
#  define CHAOS_IL2CPP_DEFAULT_LOG_LEVEL 3
#elif defined(CHAOS_IL2CPP_CONFIG_PROFILE)
#  define CHAOS_IL2CPP_DEFAULT_LOG_LEVEL 2
#else
#  define CHAOS_IL2CPP_DEFAULT_LOG_LEVEL 0
#endif

// ── Trace ring buffer ───────────────────────────────────────────────────
// CHAOS_IL2CPP_TRACE_ENABLED controls trace.h ring-buffer compilation.
//   CHECK and PROFILE compile trace; SHIP compiles it out.

#if defined(CHAOS_IL2CPP_CONFIG_CHECK) || defined(CHAOS_IL2CPP_CONFIG_PROFILE)
#  define CHAOS_IL2CPP_TRACE_ENABLED
#endif

// ── Profile scope accumulation ────────────────────────────────────────────
// CHAOS_IL2CPP_PROFILE_ENABLED controls profile.h RAII scope timers (RDTSC).
// Default to ON whenever trace is enabled (CHECK / PROFILE builds), giving
// allocation-path profile data at zero extra maintenance cost.
// Define CHAOS_IL2CPP_PROFILE_ENABLED=0 before including config.h to
// force-disable (e.g. in a TU that is extremely hot and cannot tolerate the
// ~30-cycle per-scope overhead).
#if defined(CHAOS_IL2CPP_TRACE_ENABLED) && !defined(CHAOS_IL2CPP_PROFILE_ENABLED)
#  define CHAOS_IL2CPP_PROFILE_ENABLED 1
#endif

#endif // CHAOS_IL2CPP_COMMON_CONFIG_H_