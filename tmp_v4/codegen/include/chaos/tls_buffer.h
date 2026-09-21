#ifndef CHAOS_IL2CPP_TLS_BUFFER_H_
#define CHAOS_IL2CPP_TLS_BUFFER_H_

#include <chaos/native_types.h>

#include <cstddef>
#include <mutex>

namespace chaos::il2cpp::common {

// ── TlsBuffer ────────────────────────────────────────────────────────
// Thread-local static buffer with compile-time iOS fallback.
// On platforms with CHAOS_IL2CPP_NO_TLS defined, falls back to a
// mutex-protected global static buffer (cold path — acceptable for
// reflection queries which are not on the hot execution path).
//
// Usage:
//   static TlsBuffer<char, 1024> s_name_buf;
//   char* buf = s_name_buf.data();
//   // buf is valid until the next call to s_name_buf.data() from
//   // the same thread (TLS) or until the mutex is re-acquired (NO_TLS).
//
// Template parameters:
//   T      — Element type (typically char or CHAOS_IL2CPP_INTPTR).
//   kSize  — Buffer capacity in elements.

template <typename T, std::size_t kSize>
class TlsBuffer {
public:
    T* data() {
#if defined(CHAOS_IL2CPP_NO_TLS)
        // iOS fallback: mutex-protected global static.
        // Reflection queries are cold path; lock contention is negligible.
        std::lock_guard<std::mutex> lock(s_mutex);
        return s_storage;
#else
        // Standard thread-local static.
        static thread_local T s_buffer[kSize];
        return s_buffer;
#endif
    }

    std::size_t size() const { return kSize; }

private:
#if defined(CHAOS_IL2CPP_NO_TLS)
    static T s_storage[kSize];
    static std::mutex s_mutex;
#endif
};

#if defined(CHAOS_IL2CPP_NO_TLS)
template <typename T, std::size_t kSize>
T TlsBuffer<T, kSize>::s_storage[kSize] = {};

template <typename T, std::size_t kSize>
std::mutex TlsBuffer<T, kSize>::s_mutex;
#endif

}  // namespace chaos::il2cpp::common

#endif  // CHAOS_IL2CPP_TLS_BUFFER_H_
