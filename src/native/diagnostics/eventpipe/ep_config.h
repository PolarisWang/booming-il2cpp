// ep_config.h — EventPipe feature flag and configuration
//
// Controls whether EventPipe diagnostic infrastructure is compiled in.
// When disabled (0), all EventPipe functions compile to no-ops with zero
// runtime cost via the preprocessor.
//
// Guard pattern (applied in every .cpp):
//   #if CHAOS_IL2CPP_EVENTPIPE == 1
//       ... implementation ...
//   #endif

#ifndef CHAOS_IL2CPP_EP_CONFIG_H_
#define CHAOS_IL2CPP_EP_CONFIG_H_

#include <cstdint>

#ifndef CHAOS_IL2CPP_EVENTPIPE
#define CHAOS_IL2CPP_EVENTPIPE 1  // default: enabled on all platforms
#endif

// Named pipe name template.
// Windows: \\.\pipe\chaos-il2cpp-diag-<pid>
// Linux:   /tmp/chaos-il2cpp-diag-<pid>.sock (AF_UNIX domain socket path)
#if defined(_WIN32)
#define CHAOS_IL2CPP_EP_PIPE_NAME_TEMPLATE L"\\\\.\\pipe\\chaos-il2cpp-diag-%d"
#else
#define CHAOS_IL2CPP_EP_PIPE_NAME_TEMPLATE "/tmp/chaos-il2cpp-diag-%d.sock"
#endif

// Connection timeout for the named pipe server (ConnectNamedPipe).
// In milliseconds. 0 means wait indefinitely for a client.
#define CHAOS_IL2CPP_EP_CONNECT_TIMEOUT_MS 0

// Write timeout for pipe writes (in milliseconds).
// If a write takes longer than this, it is aborted and the event is dropped.
static constexpr uint32_t kEpWriteTimeoutMs = 100;

// Maximum payload size for a single event (1 KB).
// Events with larger payloads will be truncated.
static constexpr uint32_t kEpMaxPayloadSize = 1024;

// Maximum number of outstanding (pending) writes in the transport buffer.
static constexpr int kEpMaxPendingWrites = 64;

#endif  // CHAOS_IL2CPP_EP_CONFIG_H_
