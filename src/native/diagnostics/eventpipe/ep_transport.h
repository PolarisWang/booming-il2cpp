// ep_transport.h — EventPipe IPC transport layer
//
// Provides a thread-safe interface for writing events to the diagnostic
// named pipe.  Uses Windows OVERLAPPED I/O for non-blocking writes.
//
// Thread-safety: EpTransportWrite is internally synchronized (mutex).
// Event emitters can call it from any thread including GC safepoints.

#ifndef CHAOS_IL2CPP_EP_TRANSPORT_H_
#define CHAOS_IL2CPP_EP_TRANSPORT_H_

#include "ep_config.h"
#include "ep_platform.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

#include <atomic>
#include <cstdint>
#include <mutex>

namespace chaos { namespace il2cpp { namespace diagnostics {

/// Initialize the transport layer.
/// Creates the named pipe server and starts listening for clients.
/// @param pid  Process ID to embed in the pipe name.
/// @return true on success, false on failure.
bool EpTransportInitialize(uint32_t pid) noexcept;

/// Shutdown the transport layer.
/// Disconnects any client and closes the pipe.
void EpTransportShutdown() noexcept;

/// Write an event to the pipe.
/// Thread-safe.  If no client is connected or the write fails,
/// the event is silently dropped (non-fatal).
/// @param data       Event data (header + payload) to write.
/// @param data_size  Total size in bytes.
void EpTransportWrite(const void* data, uint32_t data_size) noexcept;

/// Check whether a client is currently connected.
bool EpTransportIsConnected() noexcept;

}}}

#endif  // CHAOS_IL2CPP_EVENTPIPE

#endif  // CHAOS_IL2CPP_EP_TRANSPORT_H_
