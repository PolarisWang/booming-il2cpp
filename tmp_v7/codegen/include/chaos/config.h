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

// ── Platform detection ────────────────────────────────────
// CMakeLists.txt passes explicit CHAOS_IL2CPP_TARGET_PLATFORM_* via
// add_compile_definitions() for each preset target.
//
// When building outside the preset system (e.g. direct `cmake -B`),
// fall back to compiler built-in macros.
// ============================================================================

#if !defined(CHAOS_IL2CPP_TARGET_PLATFORM_WINDOWS) \
    && !defined(CHAOS_IL2CPP_TARGET_PLATFORM_ANDROID) \
    && !defined(CHAOS_IL2CPP_TARGET_PLATFORM_IOS) \
    && !defined(CHAOS_IL2CPP_TARGET_PLATFORM_MACOS) \
    && !defined(CHAOS_IL2CPP_TARGET_PLATFORM_LINUX)
#  if defined(__ANDROID__)
#    define CHAOS_IL2CPP_TARGET_PLATFORM_ANDROID 1
#  elif defined(__APPLE__) && defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#    define CHAOS_IL2CPP_TARGET_PLATFORM_IOS 1
#  elif defined(__APPLE__) && defined(__arm64__)
#    define CHAOS_IL2CPP_TARGET_PLATFORM_MACOS 1
#  elif defined(__APPLE__)
#    define CHAOS_IL2CPP_TARGET_PLATFORM_MACOS 1
#  elif defined(_WIN32) || defined(_WIN64)
#    define CHAOS_IL2CPP_TARGET_PLATFORM_WINDOWS 1
#  elif defined(__linux__)
#    define CHAOS_IL2CPP_TARGET_PLATFORM_LINUX 1
#  else
#    error "Unknown target platform — define CHAOS_IL2CPP_TARGET_PLATFORM_* via CMake"
#  endif
#endif

// ── Exception handling mode (dual-path + Win32 SEH) ─────────────────────────
//
// Three mutually exclusive EH strategies for managed exception propagation:
//
//   CHAOS_IL2CPP_EH_SETJMP (iOS/Android)
//     Uses setjmp/longjmp for ~5x faster throw (~300ns vs ~1700ns) and zero
//     LSDA/.ARM.extab tables.  Finally/fault semantics are handled by the
//     codegen emitting manual if/else blocks instead of RAII scope guards.
//     The runtime uses TLS exception_obj + nested jmp_buf stack.
//
//   CHAOS_IL2CPP_EH_WIN32_SEH (Windows)
//     Uses Win32 __try/__except/__finally + RaiseException.  Avoids C++ EH
//     RTTI matching and _CxxFrameHandler3 overhead.  Expect ~1200ns per
//     throw/catch vs ~2300ns for CPP_THROW on Windows.
//
//   CHAOS_IL2CPP_EH_CPP_THROW (macOS/Linux)
//     Uses C++ throw/catch with full RAII unwind.  Generates LSDA exception
//     tables (larger binary).  Simpler codegen — natural C++ try/catch/finally.
//
// If none is defined at build time, the platform-based default applies.
// ============================================================================

#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_CPP_THROW) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
  // Platform → EH mode mapping:
  //   Mobile (iOS, Android)  → SETJMP — zero EH tables, smaller binary
  //   Windows                → WIN32_SEH — SEH __except, skips C++ EH overhead
  //   macOS, Linux           → CPP_THROW — natural C++ EH
#  if defined(CHAOS_IL2CPP_TARGET_PLATFORM_IOS) \
      || defined(CHAOS_IL2CPP_TARGET_PLATFORM_ANDROID)
#    define CHAOS_IL2CPP_EH_SETJMP
#  elif defined(CHAOS_IL2CPP_TARGET_PLATFORM_WINDOWS)
#    define CHAOS_IL2CPP_EH_WIN32_SEH
#  else
#    define CHAOS_IL2CPP_EH_CPP_THROW
#  endif
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
// Only enabled in PROFILE builds (for performance analysis).  CHECK mode
// does NOT enable profiling because its ~30-cycle per-scope overhead (~60
// cycles per GC allocation) dominates benchmark results and creates an
// unfair comparison vs the fully-optimized managed JIT baseline.
// Any TU can override with `#define CHAOS_IL2CPP_PROFILE_ENABLED 1` before
// including config.h to force-enable profiling in CHECK mode for targeted
// hotspot analysis.
#if defined(CHAOS_IL2CPP_CONFIG_PROFILE) && !defined(CHAOS_IL2CPP_PROFILE_ENABLED)
#  define CHAOS_IL2CPP_PROFILE_ENABLED 1
#endif

// ── Platform identifier (compile-time, zero-overhead) ────────────────────
// Returns a canonical platform/arch string determined entirely by
// CHAOS_IL2CPP_TARGET_PLATFORM_* macros — no runtime detection, no
// branching.  The compiler inlines this to a pointer to a string literal.
// ============================================================================
inline const char* CHAOS_IL2CPP_GetPlatformIdentifier() {
#if defined(CHAOS_IL2CPP_TARGET_PLATFORM_WINDOWS)
    return "windows-x64";
#elif defined(CHAOS_IL2CPP_TARGET_PLATFORM_LINUX)
    return "linux-x64";
#elif defined(CHAOS_IL2CPP_TARGET_PLATFORM_MACOS)
#  if defined(__arm64__) || defined(__aarch64__)
    return "macos-arm64";
#  else
    return "macos-x64";
#  endif
#elif defined(CHAOS_IL2CPP_TARGET_PLATFORM_ANDROID)
    return "android-arm64";
#elif defined(CHAOS_IL2CPP_TARGET_PLATFORM_IOS)
    return "ios-arm64";
#else
    return "unknown";
#endif
}

#endif // CHAOS_IL2CPP_COMMON_CONFIG_H_