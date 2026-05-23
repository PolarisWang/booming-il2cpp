// dbg_transport.h — Debugger transport layer
//
// Provides a stdio-pipe transport for DAP communication.
// The debugger server reads from stdin and writes to stdout using the
// DAP wire format (Content-Length headers + JSON body).
//
// Thread-safety: The transport is single-threaded (message loop runs on
// the debugger thread).  Reads and writes must not be concurrent.

#ifndef CHAOS_IL2CPP_DBG_TRANSPORT_H_
#define CHAOS_IL2CPP_DBG_TRANSPORT_H_

#include "dbg_config.h"

#if CHAOS_IL2CPP_DEBUGGER == 1

#include <cstdint>
#include <string>

namespace chaos::il2cpp::diagnostics {

/// Initialize the debugger transport (stdio).
/// @return true on success.
bool DbgTransportInitialize() noexcept;

/// Shutdown the transport.
void DbgTransportShutdown() noexcept;

/// Read a DAP message from stdin.
/// Blocks until a complete message (header + body) is received.
/// @param buffer  Output buffer for the JSON body.
/// @param buffer_size  Size of output buffer.
/// @return true if a message was received, false on disconnect/error.
bool DbgTransportReadMessage(char* buffer, int buffer_size) noexcept;

/// Write a DAP message to stdout.
/// @param json_body  The JSON body to send.
/// @param body_len   Length of the JSON body.
/// @return true on success.
bool DbgTransportWriteMessage(const char* json_body, int body_len) noexcept;

/// Check if the transport is connected.
bool DbgTransportIsConnected() noexcept;

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_DEBUGGER

#endif  // CHAOS_IL2CPP_DBG_TRANSPORT_H_
