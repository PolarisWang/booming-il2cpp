#ifndef CHAOS_IL2CPP_COMMON_LOG_H_
#define CHAOS_IL2CPP_COMMON_LOG_H_

// Unified logging system for Chaos IL2CPP.
//
// ============================================================================
// Architecture: Per-thread TLS ring buffer (64 entries x 256 B = 16 KB/thread).
// Hot path (tls_write) is lock-free. Buffer wrap triggers global-mutex flush.
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
// ── Output targets ─────────────────────────────────────────────────────────
// CHAOS_IL2CPP_LOG_OUTPUT_STDOUT = 1  (default)
// CHAOS_IL2CPP_LOG_OUTPUT_TRACE  = 2  (route to trace ring buffer)
// Runtime switchable via CHAOS_IL2CPP_LOG_SET_OUTPUT(mask).
//
// ── Usage ───────────────────────────────────────────────────────────────────
//   // Simple message (no formatting – pure strcpy, lowest overhead)
//   CHAOS_IL2CPP_LOG_ERROR("Memory", "allocation failed");
//   CHAOS_IL2CPP_LOG_WARN("GC", "heap near limit");
//
//   // Formatted message (fmtlib syntax {0}, {1})
//   CHAOS_IL2CPP_LOG_ERROR_M("Memory", "alloc {0} failed at {1}", size, loc);
//   CHAOS_IL2CPP_LOG_INFO_M("Init", "loaded {0} types in {1}ms", count, ms);
//
//   // Explicit flush (normally auto-flushed on buffer wrap)
//   CHAOS_IL2CPP_LOG_FLUSH();
//
//   // Switch output target at runtime
//   CHAOS_IL2CPP_LOG_SET_OUTPUT(CHAOS_IL2CPP_LOG_OUTPUT_STDOUT);

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <atomic>
#include <mutex>
#include <string>

#include <chaos/trace.h>
#include <fmt/format.h>

// ── Compile-time log level ──────────────────────────────────────────────────

#ifndef CHAOS_IL2CPP_LOG_LEVEL
#  ifdef CHAOS_TRACE_ENABLED
#    define CHAOS_IL2CPP_LOG_LEVEL 3  // DEBUG in debug builds
#  else
#    define CHAOS_IL2CPP_LOG_LEVEL 2  // INFO in ship builds
#  endif
#endif

#define CHAOS_IL2CPP_LOG_LEVEL_ERROR 0
#define CHAOS_IL2CPP_LOG_LEVEL_WARN  1
#define CHAOS_IL2CPP_LOG_LEVEL_INFO  2
#define CHAOS_IL2CPP_LOG_LEVEL_DEBUG 3

// ── Output target bitmask ───────────────────────────────────────────────────

#define CHAOS_IL2CPP_LOG_OUTPUT_STDOUT  1u
#define CHAOS_IL2CPP_LOG_OUTPUT_TRACE   2u

// ── Explicit flush ──────────────────────────────────────────────────────────
// CHAOS_IL2CPP_LOG_FLUSH()        — flush TLS buffer to configured outputs
// CHAOS_IL2CPP_LOG_FLUSH_STDOUT() — raw fflush(stdout) under flush mutex

// ── Raw write (machine protocol) ────────────────────────────────────────────
// CHAOS_IL2CPP_LOG_WRITE_RAW(msg) — write raw text directly to stdout.
//   Bypasses TLS buffer, no prefix formatting, under flush mutex.
//   Use for machine-consumed protocol output (e.g., benchmark JSON).

// ============================================================================
// Implementation (header-only, inline)
// ============================================================================

namespace ChaosIl2cpp::Common {
namespace log_internal {

constexpr int kLogRingCapacity = 64;
constexpr int kLogLineMax     = 256;

struct LogEntry {
    char line[kLogLineMax];
};

struct ThreadLogBuffer {
    LogEntry entries[kLogRingCapacity]{};
    uint32_t head = 0;
};

// ── Global state ────────────────────────────────────────────────────────────
inline std::atomic<uint32_t> g_log_output_mask{CHAOS_IL2CPP_LOG_OUTPUT_STDOUT};
inline std::mutex            g_log_flush_mutex{};

// ── TLS buffer ─────────────────────────────────────────────────────────────
inline ThreadLogBuffer& tls_buffer() {
    static thread_local ThreadLogBuffer buf;
    return buf;
}

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
#if defined(_WIN32)
        localtime_s(&buf, &sec);
#else
        localtime_r(&sec, &buf);
#endif
        std::strftime(s_buf, sizeof(s_buf), "%Y-%m-%dT%H:%M:%S", &buf);
    }
    return s_buf;
}

// ── TLS write (lock-free) ───────────────────────────────────────────────────
inline void tls_write(const char* line) {
    auto& buf = tls_buffer();
    auto idx = buf.head % kLogRingCapacity;
    std::strncpy(buf.entries[idx].line, line, kLogLineMax - 1);
    buf.entries[idx].line[kLogLineMax - 1] = '\0';
    buf.head++;
}

// ── TLS flush to configured outputs ─────────────────────────────────────────
inline void flush_tls_buffer(bool force = false) {
    auto& buf = tls_buffer();
    auto count = buf.head;
    if (count == 0) return;

    auto mask = g_log_output_mask.load(std::memory_order_relaxed);

    // Determine range: if head hasn't wrapped, flush [0, head).
    // If wrapped, flush [head - kLogRingCapacity, head) = the latest ring.
    auto start = count >= kLogRingCapacity ? count - kLogRingCapacity : 0;
    auto end   = count;

    std::lock_guard<std::mutex> guard(g_log_flush_mutex);

    for (auto i = start; i < end; ++i) {
        auto idx = i % kLogRingCapacity;
        const char* line = buf.entries[idx].line;
        if (line[0] == '\0') continue;

        if (mask & CHAOS_IL2CPP_LOG_OUTPUT_STDOUT) {
            std::fputs(line, stdout);
            std::fputc('\n', stdout);
        }
    }

    if (mask & CHAOS_IL2CPP_LOG_OUTPUT_STDOUT) {
        std::fflush(stdout);
    }

    // Reset buffer (keep the unflushed tail if force=false and buffer wrapped)
    if (force || count < kLogRingCapacity) {
        buf.head = 0;
    }
}

// ── Raw write (bypasses TLS buffer, no prefix formatting) ───────────────────
// Used for machine-consumed protocol output (e.g., benchmark JSON).
inline void write_raw(const char* msg) {
    std::lock_guard<std::mutex> guard(g_log_flush_mutex);
    std::fputs(msg, stdout);
}

// Formatted raw write: fmtlib-style {0} {1} ...
template <typename... Args>
inline void write_raw_fmt(fmt::format_string<Args...> fmt_str,
                           Args&&... args) {
    auto formatted = fmt::format(fmt_str, std::forward<Args>(args)...);
    write_raw(formatted.c_str());
}

// ── Raw stdout flush ────────────────────────────────────────────────────────
inline void flush_stdout() {
    std::lock_guard<std::mutex> guard(g_log_flush_mutex);
    std::fflush(stdout);
}

// ── Format helpers ──────────────────────────────────────────────────────────

// Simple message: no format args, pure string copy
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

// Core log dispatch: format + TLS write + conditional flush
inline void log_dispatch(const std::string& formatted) {
    tls_write(formatted.c_str());
    // Auto-flush on every wrap
    if (tls_buffer().head % kLogRingCapacity == 0) {
        flush_tls_buffer();
    }
}

} // namespace log_internal
} // namespace ChaosIl2cpp::Common

// ============================================================================
// Public macros
// ============================================================================

// ── ERROR (always compiled) ─────────────────────────────────────────────────

#define CHAOS_IL2CPP_LOG_ERROR(category, message)                     do { \
    CHAOS_IL2CPP_LOG_TRACE((category), "error", "message={0}", (message)); \
    ::ChaosIl2cpp::Common::log_internal::log_dispatch(                     \
        ::ChaosIl2cpp::Common::log_internal::format_simple(                \
            "ERROR", (category), (message)));                               \
} while (0)

#define CHAOS_IL2CPP_LOG_ERROR_M(category, fmt_str, ...)              do { \
    CHAOS_IL2CPP_LOG_TRACE((category), "error", "formatted=true");         \
    ::ChaosIl2cpp::Common::log_internal::log_dispatch(                     \
        ::ChaosIl2cpp::Common::log_internal::format_fmt(                   \
            "ERROR", (category), (fmt_str), ##__VA_ARGS__));               \
} while (0)

// ── WARN (compiled when level >= 1) ────────────────────────────────────────

#if CHAOS_IL2CPP_LOG_LEVEL >= 1
#define CHAOS_IL2CPP_LOG_WARN(category, message)                       do { \
    CHAOS_IL2CPP_LOG_TRACE((category), "warn", "message={0}", (message));   \
    ::ChaosIl2cpp::Common::log_internal::log_dispatch(                     \
        ::ChaosIl2cpp::Common::log_internal::format_simple(                \
            "WARN", (category), (message)));                                \
} while (0)

#define CHAOS_IL2CPP_LOG_WARN_M(category, fmt_str, ...)               do { \
    CHAOS_IL2CPP_LOG_TRACE((category), "warn", "formatted=true");          \
    ::ChaosIl2cpp::Common::log_internal::log_dispatch(                     \
        ::ChaosIl2cpp::Common::log_internal::format_fmt(                   \
            "WARN", (category), (fmt_str), ##__VA_ARGS__));                \
} while (0)
#else
#define CHAOS_IL2CPP_LOG_WARN(category, message)       ((void)0)
#define CHAOS_IL2CPP_LOG_WARN_M(category, fmt_str, ...) ((void)0)
#endif

// ── INFO (compiled when level >= 2, default in SHIP) ───────────────────────

#if CHAOS_IL2CPP_LOG_LEVEL >= 2
#define CHAOS_IL2CPP_LOG_INFO(category, message)                      do { \
    ::ChaosIl2cpp::Common::log_internal::log_dispatch(                     \
        ::ChaosIl2cpp::Common::log_internal::format_simple(                \
            "INFO", (category), (message)));                                \
} while (0)

#define CHAOS_IL2CPP_LOG_INFO_M(category, fmt_str, ...)               do { \
    ::ChaosIl2cpp::Common::log_internal::log_dispatch(                     \
        ::ChaosIl2cpp::Common::log_internal::format_fmt(                   \
            "INFO", (category), (fmt_str), ##__VA_ARGS__));                \
} while (0)
#else
#define CHAOS_IL2CPP_LOG_INFO(category, message)       ((void)0)
#define CHAOS_IL2CPP_LOG_INFO_M(category, fmt_str, ...) ((void)0)
#endif

// ── DEBUG (compiled when level >= 3) ───────────────────────────────────────

#if CHAOS_IL2CPP_LOG_LEVEL >= 3
#define CHAOS_IL2CPP_LOG_DEBUG(category, message)                     do { \
    ::ChaosIl2cpp::Common::log_internal::log_dispatch(                     \
        ::ChaosIl2cpp::Common::log_internal::format_simple(                \
            "DEBUG", (category), (message)));                               \
} while (0)

#define CHAOS_IL2CPP_LOG_DEBUG_M(category, fmt_str, ...)              do { \
    ::ChaosIl2cpp::Common::log_internal::log_dispatch(                     \
        ::ChaosIl2cpp::Common::log_internal::format_fmt(                   \
            "DEBUG", (category), (fmt_str), ##__VA_ARGS__));               \
} while (0)
#else
#define CHAOS_IL2CPP_LOG_DEBUG(category, message)      ((void)0)
#define CHAOS_IL2CPP_LOG_DEBUG_M(category, fmt_str, ...) ((void)0)
#endif

// ── Control macros ─────────────────────────────────────────────────────────

#define CHAOS_IL2CPP_LOG_FLUSH()                                         do { \
    ::ChaosIl2cpp::Common::log_internal::flush_tls_buffer(true);              \
} while (0)

#define CHAOS_IL2CPP_LOG_SET_OUTPUT(mask)                                do { \
    ::ChaosIl2cpp::Common::log_internal::g_log_output_mask.store(             \
        (mask), std::memory_order_relaxed);                                    \
} while (0)

// ── Raw protocol output (bypasses log formatting, under flush mutex) ────────
// For machine-consumed output that must not get log prefixes, e.g. benchmark JSON.
// Write is pre-formatted by the caller.
#define CHAOS_IL2CPP_LOG_WRITE_RAW(msg)                                   do { \
    ::ChaosIl2cpp::Common::log_internal::write_raw((msg));                     \
} while (0)

// Formatted raw write: CHAOS_IL2CPP_LOG_WRITE_RAW_M("fmt {0}", arg)
#define CHAOS_IL2CPP_LOG_WRITE_RAW_M(fmt_str, ...)                        do { \
    ::ChaosIl2cpp::Common::log_internal::write_raw_fmt(                       \
        (fmt_str), ##__VA_ARGS__);                                             \
} while (0)

// ── Flush stdout under log flush mutex ──────────────────────────────────────
#define CHAOS_IL2CPP_LOG_FLUSH_STDOUT()                                   do { \
    ::ChaosIl2cpp::Common::log_internal::flush_stdout();                       \
} while (0)

#endif // CHAOS_IL2CPP_COMMON_LOG_H_
