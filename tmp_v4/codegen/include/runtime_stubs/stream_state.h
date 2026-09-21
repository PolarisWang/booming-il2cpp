// ── Stream/IO side map state ───────────────────────────────────
#pragma once

#include <chaos/native_types.h>
#include <chaos/unordered_dense.h>
#include <cstdint>
#include <mutex>
#include <vector>
#include <string>

namespace chaos::il2cpp::runtime_core {

enum class StreamKind : uint8_t {
    None,
    MemoryStream,
    StringReader,
    StringWriter,
};

/// Per-object state for stream/IO stubs.
/// Stored in a global side map keyed by the managed object pointer.
struct StreamState {
    StreamKind kind = StreamKind::None;

    // MemoryStream state
    std::vector<uint8_t> buffer;
    int64_t position = 0;

    // StringReader state
    std::string text;
    int64_t read_pos = 0;

    // StringWriter state
    std::string accumulated;
};

/// Global side map: managed object pointer → StreamState.
extern CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(void*, StreamState) g_stream_state;
/// Mutex guarding g_stream_state.
extern std::mutex g_stream_state_mutex;

/// RAII lock for g_stream_state_mutex.
struct StreamStateLock {
    StreamStateLock() { g_stream_state_mutex.lock(); }
    ~StreamStateLock() { g_stream_state_mutex.unlock(); }
    StreamStateLock(const StreamStateLock&) = delete;
    StreamStateLock& operator=(const StreamStateLock&) = delete;
};

}  // namespace chaos::il2cpp::runtime_core
