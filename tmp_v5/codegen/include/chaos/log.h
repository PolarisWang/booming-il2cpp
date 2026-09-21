#ifndef CHAOS_IL2CPP_COMMON_LOG_H_
#define CHAOS_IL2CPP_COMMON_LOG_H_

// Unified logging system for Chaos IL2CPP.
//
// ============================================================================
// Architecture: Direct-write with global mutex. Each CHAOS_IL2CPP_LOG_XXXXX
// call formats the message and writes it to stderr immediately — no ring
// buffer, no batching, no explicit fflush needed.
//
// stderr must be unbuffered for reliable immediate output. On Windows the
// C runtime default is unbuffered for stderr; on POSIX stderr defaults to
// line-buffered when connected to a TTY and unbuffered otherwise.
//
// Protocol/machine output (benchmark JSON, fact results) should go to stdout
// via CHAOS_IL2CPP_LOG_WRITE_RAW or direct printf/fputs so that diagnostic
// logs (stderr) do not pollute machine-consumed output.
// ============================================================================
//
// ── Log levels ──────────────────────────────────────────────────────────────
//   ERROR   = 0  (always compiled)
//   WARN    = 1  (compiled when CHAOS_IL2CPP_LOG_LEVEL >= 1)
//   INFO    = 2  (compiled when CHAOS_IL2CPP_LOG_LEVEL >= 2, default SHIP)
//   DEBUG   = 3  (compiled when CHAOS_IL2CPP_LOG_LEVEL >= 3)
//
// CHAOS_IL2CPP_LOG_LEVEL default:
//   CHAOS_TRACE_ENABLED → 3 (DEBUG in Debug build)
//   otherwise           → 2 (INFO in Ship build)
//
// ── Usage ───────────────────────────────────────────────────────────────────
//   // Simple message (no formatting)
//   CHAOS_IL2CPP_LOG_ERROR("Memory", "allocation failed");
//   CHAOS_IL2CPP_LOG_WARN("GC", "heap near limit");
//
//   // Formatted message (fmtlib syntax {0}, {1})
//   CHAOS_IL2CPP_LOG_ERROR_M("Memory", "alloc {0} failed at {1}", size, loc);
//   CHAOS_IL2CPP_LOG_INFO_M("Init", "loaded {0} types in {1}ms", count, ms);
//
//   // Raw protocol output (no log prefix, machine-consumed)
//   CHAOS_IL2CPP_LOG_WRITE_RAW("benchmark data\n");

#include <cstdint>
#include <cstdio>
#include <atomic>
#include <mutex>
#include <string>


#include <chaos/trace.h>
#include <chaos/config.h>
#include <chaos/pal/pal_time.h>
#include <fmt/format.h>

// ── Compile-time log level ──────────────────────────────────────────────────
// Default from config.h: CHECK=3, PROFILE=2, SHIP=0.
// Individual translation units may override before including this header.

#ifndef CHAOS_IL2CPP_LOG_LEVEL
#  define CHAOS_IL2CPP_LOG_LEVEL CHAOS_IL2CPP_DEFAULT_LOG_LEVEL
#endif

// ── Log level indicator constants ──────────────────────────────────────────

#define CHAOS_IL2CPP_LOG_LEVEL_ERROR 0
#define CHAOS_IL2CPP_LOG_LEVEL_WARN  1
#define CHAOS_IL2CPP_LOG_LEVEL_INFO  2
#define CHAOS_IL2CPP_LOG_LEVEL_DEBUG 3

// ============================================================================
// Implementation (header-only, inline)
// ============================================================================

namespace chaos::il2cpp::common {
namespace log_internal {

// ── Global mutex for thread-safe stdout access ──────────────────────────────
inline std::mutex g_log_mutex{};

// ── Cached timestamp (refreshed at most once per ms) ────────────────────────
inline const char* cached_log_timestamp() {
    using namespace std::chrono;
    static thread_local uint64_t  s_last_ms = 0;
    static thread_local char      s_buf[24] = {};
    auto now_ms = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()).count();
    if (now_ms != s_last_ms || s_buf[0] == '\0') {
        s_last_ms = now_ms;
        auto sec = static_cast<time_t>(now_ms / 1000);
        struct tm buf;
        chaos::il2cpp::pal::PalLocalTime(&sec, &buf);
        std::strftime(s_buf, sizeof(s_buf), "%Y-%m-%dT%H:%M:%S", &buf);
    }
    return s_buf;
}

// ── Format helpers ──────────────────────────────────────────────────────────

// Simple message: no format args
inline std::string format_simple(const char* level, const char* category,
                                  const char* message) {
    return fmt::format("[{0}][{1}][{2}] {3}",
                       cached_log_timestamp(), level, category, message);
}

// Formatted message: fmtlib-style {0} {1} ...
template <typename... Args>
inline std::string format_fmt(const char* level, const char* category,
                               fmt::format_string<Args...> fmt_str,
                               Args&&... args) {
    return fmt::format("[{0}][{1}][{2}] {3}",
                       cached_log_timestamp(), level, category,
                       fmt::format(fmt_str, std::forward<Args>(args)...));
}

// ── Global log-output flag (defined in runtime_core.cpp) ──────────────
// Non-inline, non-COMDAT variable in chaos_runtime_core.lib so there is
// exactly ONE instance across the entire binary.  The verification pipeline
// sets this flag to redirect diagnostic log output to stderr, keeping
// machine-consumed protocol output (benchmark JSON, fact results) clean on
// stdout.
//
// All inline log_write() calls in every prebuilt lib (interpreter, jit,
// bootstrap, etc.) reference this single extern variable — no ODR ambiguity.

extern bool g_log_use_stderr;

// ── Log write (lock-protected, immediate) ───────────────────────────────────
// Each call acquires g_log_mutex, writes the formatted line + newline to
// stdout (default) or stderr (when g_log_use_stderr is set by the verification
// pipeline entry point).  The output FILE* switch is checked at runtime so
// all prebuilt libs (interpreter, jit, bootstrap, etc.) benefit without
// recompilation — they all call the same inline function that references the
// single extern g_log_use_stderr flag in chaos_runtime_core.lib.

inline void log_write(const std::string& formatted) {
    std::lock_guard<std::mutex> guard(g_log_mutex);
    std::fputs(formatted.c_str(), g_log_use_stderr ? stderr : stdout);
    std::fputc('\n', g_log_use_stderr ? stderr : stdout);
}

// ── Raw write (bypasses log formatting, no prefix, no newline added) ────────
// Used for machine-consumed protocol output (e.g., benchmark JSON).
inline void write_raw(const char* msg) {
    std::lock_guard<std::mutex> guard(g_log_mutex);
    std::fputs(msg, stdout);
}

// Formatted raw write: fmtlib-style {0} {1} ...
template <typename... Args>
inline void write_raw_fmt(fmt::format_string<Args...> fmt_str,
                           Args&&... args) {
    auto formatted = fmt::format(fmt_str, std::forward<Args>(args)...);
    write_raw(formatted.c_str());
}

// ── Raw stdout flush (only needed for write_raw which omits '\n') ───────────
inline void flush_stdout() {
    std::lock_guard<std::mutex> guard(g_log_mutex);
    std::fflush(stdout);
}

} // namespace log_internal
} // namespace chaos::il2cpp::common

// ============================================================================
// Public macros
// ============================================================================

// ── ERROR (always compiled) ─────────────────────────────────────────────────

#define CHAOS_IL2CPP_LOG_ERROR(category, message)                     do { \
    CHAOS_IL2CPP_LOG_TRACE((category), "error", "message={0}", (message)); \
    ::chaos::il2cpp::common::log_internal::log_write(                        \
        ::chaos::il2cpp::common::log_internal::format_simple(                 \
            "ERROR", (category), (message)));                               \
} while (0)

#define CHAOS_IL2CPP_LOG_ERROR_M(category, fmt_str, ...)              do { \
    CHAOS_IL2CPP_LOG_TRACE((category), "error", "formatted=true");         \
    ::chaos::il2cpp::common::log_internal::log_write(                        \
        ::chaos::il2cpp::common::log_internal::format_fmt(                   \
            "ERROR", (category), (fmt_str), ##__VA_ARGS__));               \
} while (0)

// ── WARN (compiled when level >= 1) ────────────────────────────────────────

#if CHAOS_IL2CPP_LOG_LEVEL >= 1
#define CHAOS_IL2CPP_LOG_WARN(category, message)                       do { \
    CHAOS_IL2CPP_LOG_TRACE((category), "warn", "message={0}", (message));   \
    ::chaos::il2cpp::common::log_internal::log_write(                        \
        ::chaos::il2cpp::common::log_internal::format_simple(                 \
            "WARN", (category), (message)));                                \
} while (0)

#define CHAOS_IL2CPP_LOG_WARN_M(category, fmt_str, ...)               do { \
    CHAOS_IL2CPP_LOG_TRACE((category), "warn", "formatted=true");          \
    ::chaos::il2cpp::common::log_internal::log_write(                        \
        ::chaos::il2cpp::common::log_internal::format_fmt(                   \
            "WARN", (category), (fmt_str), ##__VA_ARGS__));                \
} while (0)
#else
#define CHAOS_IL2CPP_LOG_WARN(category, message)       ((void)0)
#define CHAOS_IL2CPP_LOG_WARN_M(category, fmt_str, ...) ((void)0)
#endif

// ── INFO (compiled when level >= 2, default in SHIP) ───────────────────────

#if CHAOS_IL2CPP_LOG_LEVEL >= 2
#define CHAOS_IL2CPP_LOG_INFO(category, message)                      do { \
    ::chaos::il2cpp::common::log_internal::log_write(                        \
        ::chaos::il2cpp::common::log_internal::format_simple(                 \
            "INFO", (category), (message)));                                \
} while (0)

#define CHAOS_IL2CPP_LOG_INFO_M(category, fmt_str, ...)               do { \
    ::chaos::il2cpp::common::log_internal::log_write(                        \
        ::chaos::il2cpp::common::log_internal::format_fmt(                   \
            "INFO", (category), (fmt_str), ##__VA_ARGS__));                \
} while (0)
#else
#define CHAOS_IL2CPP_LOG_INFO(category, message)       ((void)0)
#define CHAOS_IL2CPP_LOG_INFO_M(category, fmt_str, ...) ((void)0)
#endif

// ── DEBUG (compiled when level >= 3) ───────────────────────────────────────

#if CHAOS_IL2CPP_LOG_LEVEL >= 3
#define CHAOS_IL2CPP_LOG_DEBUG(category, message)                     do { \
    ::chaos::il2cpp::common::log_internal::log_write(                        \
        ::chaos::il2cpp::common::log_internal::format_simple(                 \
            "DEBUG", (category), (message)));                               \
} while (0)

#define CHAOS_IL2CPP_LOG_DEBUG_M(category, fmt_str, ...)              do { \
    ::chaos::il2cpp::common::log_internal::log_write(                        \
        ::chaos::il2cpp::common::log_internal::format_fmt(                   \
            "DEBUG", (category), (fmt_str), ##__VA_ARGS__));               \
} while (0)
#else
#define CHAOS_IL2CPP_LOG_DEBUG(category, message)      ((void)0)
#define CHAOS_IL2CPP_LOG_DEBUG_M(category, fmt_str, ...) ((void)0)
#endif

// ── Control macros ─────────────────────────────────────────────────────────

// CHAOS_IL2CPP_LOG_FLUSH is a no-op: _IOLBF flushes on every '\n'.
// Keep the macro for source compatibility.
#define CHAOS_IL2CPP_LOG_FLUSH()                     ((void)0)

// ── Raw protocol output (bypasses log formatting, under log mutex) ──────────
#define CHAOS_IL2CPP_LOG_WRITE_RAW(msg)                                   do { \
    ::chaos::il2cpp::common::log_internal::write_raw((msg));                     \
} while (0)

#define CHAOS_IL2CPP_LOG_WRITE_RAW_M(fmt_str, ...)                        do { \
    ::chaos::il2cpp::common::log_internal::write_raw_fmt(                       \
        (fmt_str), ##__VA_ARGS__);                                             \
} while (0)

// ── Flush stdout under log mutex ────────────────────────────────────────────
#define CHAOS_IL2CPP_LOG_FLUSH_STDOUT()                                   do { \
    ::chaos::il2cpp::common::log_internal::flush_stdout();                       \
} while (0)

#endif // CHAOS_IL2CPP_COMMON_LOG_H_