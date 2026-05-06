#ifndef CHAOS_IL2CPP_COMMON_TRACE_H_
#define CHAOS_IL2CPP_COMMON_TRACE_H_

// Cross-language trace for C++ (CHECK/Debug mode only).
//
// Usage:
//   CHAOS_IL2CPP_LOG_TRACE("codegen", "emit_method", "method_count=%d", 18);
//   CHAOS_IL2CPP_LOG_TRACE_INIT();
//   CHAOS_IL2CPP_LOG_TRACE_FLUSH("/tmp/trace.jsonl");
//
// In PROFILE/SHIP builds all macros expand to ((void)0) — zero runtime overhead.
// Enable via CMake: target_compile_definitions(chaos_common PUBLIC CHAOS_TRACE_ENABLED)
// (Automatically set for Debug builds in chaos_common/CMakeLists.txt)

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <atomic>

#include <string>

#include <fmt/format.h>

// ============================================================================
// Ring buffer implementation (internal)
// ============================================================================

namespace ChaosIl2cpp::Common {
namespace detail {

constexpr int kTraceRingCapacity = 256;
constexpr int kTraceLineMax = 256;

struct TraceEntry {
    char line[kTraceLineMax];
};

class TraceRingBuffer {
public:
    void write(const char* line) {
        auto idx = head_.fetch_add(1, std::memory_order_relaxed) % kTraceRingCapacity;
        std::strncpy(entries_[idx].line, line, kTraceLineMax - 1);
        entries_[idx].line[kTraceLineMax - 1] = '\0';
    }

    void flush(const char* path) {
        auto count = head_.load(std::memory_order_relaxed);
        if (count == 0) return;

        auto* f = std::fopen(path, "a");
        if (!f) return;

        auto start = count >= kTraceRingCapacity ? count - kTraceRingCapacity : 0;
        auto end = count;
        for (auto i = start; i < end; ++i) {
            auto idx = i % kTraceRingCapacity;
            if (entries_[idx].line[0] != '\0') {
                std::fputs(entries_[idx].line, f);
                std::fputc('\n', f);
            }
        }
        std::fclose(f);
    }

    bool empty() const { return head_.load(std::memory_order_relaxed) == 0; }

private:
    TraceEntry entries_[kTraceRingCapacity]{};
    std::atomic<uint32_t> head_{0};
};

// Global ring buffer instance
inline TraceRingBuffer g_trace_ring;

// Global flush path (set by TRACE_INIT)
inline char g_trace_flush_path[512] = {};

// atexit handler
inline void trace_atexit_flush() {
    if (g_trace_flush_path[0] != '\0') {
        g_trace_ring.flush(g_trace_flush_path);
    }
}

// Global traceId (set from CHAOS_TRACE_ID env var by TRACE_INIT)
inline char g_trace_id[32] = {};

// Timestamp cache: refreshed at most once per ms to avoid repeated clock_gettime
inline uint64_t g_last_ts_ms = 0;
inline char g_cached_timestamp[32] = {};

inline const char* cached_iso8601() {
    using namespace std::chrono;
    auto now_ms = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()).count();
    if (now_ms != g_last_ts_ms || g_cached_timestamp[0] == '\0') {
        g_last_ts_ms = now_ms;
        auto sec = static_cast<time_t>(now_ms / 1000);
        struct tm buf;
#if defined(_WIN32)
        localtime_s(&buf, &sec);
#else
        localtime_r(&sec, &buf);
#endif
        std::strftime(g_cached_timestamp, sizeof(g_cached_timestamp),
                      "%Y-%m-%dT%H:%M:%S", &buf);
    }
    return g_cached_timestamp;
}

} // namespace detail
} // namespace ChaosIl2cpp::Common

// ============================================================================
// Public macros
// ============================================================================

#ifdef CHAOS_TRACE_ENABLED

#define CHAOS_IL2CPP_LOG_TRACE(stage, op, fmt_str, ...) do {                       \
    const auto _ts_ = ChaosIl2cpp::Common::detail::cached_iso8601();              \
    const auto _tid_ = ChaosIl2cpp::Common::detail::g_trace_id;                   \
    auto _buf_ = ::fmt::format(                                                    \
        "{{\"t\":\"{}Z\",\"l\":\"cpp\",\"traceId\":\"{}\","                       \
        "\"s\":\"{}\",\"o\":\"{}\",\"f\":\"{}:{}\"",                              \
        _ts_, _tid_, (stage), (op), __FILE__, __LINE__);                          \
    const auto _extra_ = ::fmt::format((fmt_str), ##__VA_ARGS__);                  \
    if (!_extra_.empty()) {                                                        \
        _buf_ += ",";                                                              \
        _buf_ += _extra_;                                                          \
    }                                                                              \
    _buf_ += "}";                                                                  \
    ChaosIl2cpp::Common::detail::g_trace_ring.write(_buf_.c_str());                \
} while (0)

#define CHAOS_IL2CPP_LOG_TRACE_INIT() do {                                        \
    auto _trace_path_ = std::getenv("CHAOS_TRACE_PATH");                      \
    if (_trace_path_) {                                                        \
        std::strncpy(ChaosIl2cpp::Common::detail::g_trace_flush_path,          \
                     _trace_path_,                                             \
                     sizeof(ChaosIl2cpp::Common::detail::g_trace_flush_path) - 1); \
        std::atexit(ChaosIl2cpp::Common::detail::trace_atexit_flush);          \
    }                                                                          \
    auto _trace_id_ = std::getenv("CHAOS_TRACE_ID");                           \
    if (_trace_id_) {                                                           \
        std::strncpy(ChaosIl2cpp::Common::detail::g_trace_id,                  \
                     _trace_id_,                                               \
                     sizeof(ChaosIl2cpp::Common::detail::g_trace_id) - 1);     \
    }                                                                          \
} while (0)

#define CHAOS_IL2CPP_LOG_TRACE_FLUSH(path) do {                                   \
    ChaosIl2cpp::Common::detail::g_trace_ring.flush(path);                     \
} while (0)

#else // !CHAOS_TRACE_ENABLED

#define CHAOS_IL2CPP_LOG_TRACE(stage, op, fmt, ...) ((void)0)
#define CHAOS_IL2CPP_LOG_TRACE_INIT()                ((void)0)
#define CHAOS_IL2CPP_LOG_TRACE_FLUSH(path)           ((void)0)

#endif // CHAOS_TRACE_ENABLED

#endif // CHAOS_IL2CPP_COMMON_TRACE_H_
