#ifndef CHAOS_IL2CPP_COMMON_TRACE_H_
#define CHAOS_IL2CPP_COMMON_TRACE_H_

// Cross-language trace for C++ (CHECK/Debug mode only).
//
// Usage:
//   CHAOS_IL2CPP_LOG_TRACE("codegen", "emit_method", "method_count=%d", 18);
//   CHAOS_IL2CPP_LOG_TRACE_INIT();
//   CHAOS_IL2CPP_LOG_TRACE_FLUSH("/tmp/trace.jsonl");
//
// In SHIP builds all macros expand to ((void)0) — zero runtime overhead.
// Enabled automatically for CHECK and PROFILE builds via config.h.

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

namespace chaos::il2cpp::common {
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

// Timestamp cache using thread_local to avoid data races under concurrent
// trace writes. Each thread independently caches its last formatted timestamp
// and refreshes it at most once per millisecond.
inline const char* cached_iso8601() {
    thread_local uint64_t tls_last_ts_ms = 0;
    thread_local char tls_cached_timestamp[32] = {};
    using namespace std::chrono;
    auto now_ms = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()).count();
    if (now_ms != tls_last_ts_ms || tls_cached_timestamp[0] == '\0') {
        tls_last_ts_ms = now_ms;
        auto sec = static_cast<time_t>(now_ms / 1000);
        struct tm buf;
#if defined(_WIN32)
        localtime_s(&buf, &sec);
#else
        localtime_r(&sec, &buf);
#endif
        std::strftime(tls_cached_timestamp, sizeof(tls_cached_timestamp),
                      "%Y-%m-%dT%H:%M:%S", &buf);
    }
    return tls_cached_timestamp;
}

} // namespace detail
} // namespace chaos::il2cpp::common

// ============================================================================
// Public macros
// ============================================================================

#ifdef CHAOS_IL2CPP_TRACE_ENABLED

#define CHAOS_IL2CPP_LOG_TRACE(stage, op, fmt_str, ...) do {                       \
    const auto _ts_ = chaos::il2cpp::common::detail::cached_iso8601();              \
    const auto _tid_ = chaos::il2cpp::common::detail::g_trace_id;                   \
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
    chaos::il2cpp::common::detail::g_trace_ring.write(_buf_.c_str());                \
} while (0)

#define CHAOS_IL2CPP_LOG_TRACE_INIT() do {                                        \
    auto _trace_path_ = std::getenv("CHAOS_TRACE_PATH");                      \
    if (_trace_path_) {                                                        \
        std::strncpy(chaos::il2cpp::common::detail::g_trace_flush_path,          \
                     _trace_path_,                                             \
                     sizeof(chaos::il2cpp::common::detail::g_trace_flush_path) - 1); \
        std::atexit(chaos::il2cpp::common::detail::trace_atexit_flush);          \
    }                                                                          \
    auto _trace_id_ = std::getenv("CHAOS_TRACE_ID");                           \
    if (_trace_id_) {                                                           \
        std::strncpy(chaos::il2cpp::common::detail::g_trace_id,                  \
                     _trace_id_,                                               \
                     sizeof(chaos::il2cpp::common::detail::g_trace_id) - 1);     \
    }                                                                          \
} while (0)

#define CHAOS_IL2CPP_LOG_TRACE_FLUSH(path) do {                                   \
    chaos::il2cpp::common::detail::g_trace_ring.flush(path);                     \
} while (0)

#else // !CHAOS_IL2CPP_TRACE_ENABLED

#define CHAOS_IL2CPP_LOG_TRACE(stage, op, fmt, ...) ((void)0)
#define CHAOS_IL2CPP_LOG_TRACE_INIT()                ((void)0)
#define CHAOS_IL2CPP_LOG_TRACE_FLUSH(path)           ((void)0)

#endif // CHAOS_IL2CPP_TRACE_ENABLED

#endif // CHAOS_IL2CPP_COMMON_TRACE_H_
