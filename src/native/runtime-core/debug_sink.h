// ── Debug Event Sink ──────────────────────────────────────────
// Lightweight ring buffer for runtime diagnostics.
// Enabled via CHAOS_DEBUG_SINK define (added in CMake build for debug builds).
// When enabled, generated code can write debug events that get dumped
// on assertion failure or explicit --debug-dump flag.

#include <chaos/native_types.h>
#include <cstdint>
#include <cstdio>
#include <chrono>

namespace chaos::il2cpp::runtime_core {

enum DebugEventKind : uint8_t {
    DEBUG_ASSERT_FAILURE,       // Assert.Equal/True/False/NotNull/Fail
    DEBUG_EXCEPTION_CAUGHT,     // catch(chaos_managed_exception) type matched
    DEBUG_EXCEPTION_MISMATCH,   // catch type check failed → rethrow
    DEBUG_EXCEPTION_THROWN,     // throw chaos_managed_exception
    DEBUG_HOTPATCH_DISPATCH,    // InterpreterEntryDirect called
    DEBUG_METHOD_ENTER,         // method entry (opt-in per method)
    DEBUG_METHOD_EXIT,          // method exit (opt-in per method)
    DEBUG_STATIC_READ,          // ExitCode static field read
    DEBUG_STATIC_WRITE,         // ExitCode static field write
};

struct DebugEvent {
    DebugEventKind kind;
    uint64_t timestamp;
    uint32_t method_token;
    int64_t data;
    const char* message;
};

static constexpr uint32_t kDebugRingBufferSize = 1024;

class DebugEventSink {
public:
    void Write(DebugEvent event) {
        if (!_enabled) return;
        event.timestamp = _now();
        _buffer[_head] = event;
        _head = (_head + 1) % kDebugRingBufferSize;
        if (_count < kDebugRingBufferSize) _count++;
    }

    void DumpAll() {
        if (_count == 0) {
            std::fprintf(stderr, "[DEBUG_SINK] (no events)\n");
            return;
        }
        uint32_t start = _head >= _count ? _head - _count : 0;
        for (uint32_t i = 0; i < _count; i++) {
            auto& e = _buffer[(start + i) % kDebugRingBufferSize];
            const char* kind_str = _kind_string(e.kind);
            std::fprintf(stderr, "[DEBUG_SINK] %s method_token=%u data=%lld msg=%s\n",
                kind_str, e.method_token, (long long)e.data,
                e.message ? e.message : "");
        }
    }

    void SetEnabled(bool enabled) { _enabled = enabled; }
    bool IsEnabled() const { return _enabled; }

private:
    static uint64_t _now() {
        return static_cast<uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
    }

    static const char* _kind_string(DebugEventKind kind) {
        switch (kind) {
            case DEBUG_ASSERT_FAILURE:      return "ASSERT_FAILURE";
            case DEBUG_EXCEPTION_CAUGHT:    return "EXCEPTION_CAUGHT";
            case DEBUG_EXCEPTION_MISMATCH:  return "EXCEPTION_MISMATCH";
            case DEBUG_EXCEPTION_THROWN:    return "EXCEPTION_THROWN";
            case DEBUG_HOTPATCH_DISPATCH:   return "HOTPATCH_DISPATCH";
            case DEBUG_METHOD_ENTER:        return "METHOD_ENTER";
            case DEBUG_METHOD_EXIT:         return "METHOD_EXIT";
            case DEBUG_STATIC_READ:         return "STATIC_READ";
            case DEBUG_STATIC_WRITE:        return "STATIC_WRITE";
            default:                        return "UNKNOWN";
        }
    }

    DebugEvent _buffer[kDebugRingBufferSize]{};
    uint32_t _head = 0;
    uint32_t _count = 0;
    bool _enabled = false;
};

extern DebugEventSink g_debug_sink;

#ifdef CHAOS_DEBUG_SINK
    #define DEBUG_WRITE(kind, token, data, msg) \
        do { if (::chaos::il2cpp::runtime_core::g_debug_sink.IsEnabled()) \
            ::chaos::il2cpp::runtime_core::g_debug_sink.Write({kind, 0, token, data, msg}); } while(0)
#else
    #define DEBUG_WRITE(kind, token, data, msg) ((void)0)
#endif

} // namespace chaos::il2cpp::runtime_core