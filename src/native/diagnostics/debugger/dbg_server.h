// dbg_server.h — DAP message server
//
// Implements the DAP message loop: receives requests, dispatches to
// handlers, and sends responses/events.  Runs in a dedicated thread
// (not the interpreter execution thread).
//
// The execution flow is:
//   1. VSCode sends "launch" → debugger thread starts message loop
//   2. Interpreter hits breakpoint → sends "stopped" event
//   3. VSCode sends "stackTrace", "scopes", "variables" → thread responds
//   4. VSCode sends "continue" → signals interpreter to resume
//   5. VSCode sends "disconnect" → shutdown

#ifndef CHAOS_IL2CPP_DBG_SERVER_H_
#define CHAOS_IL2CPP_DBG_SERVER_H_

#include "dbg_config.h"

#if CHAOS_IL2CPP_DEBUGGER == 1

#include "dbg_protocol.h"

namespace chaos::il2cpp::diagnostics {

/// Initialize the DAP server.
void DbgServerInitialize() noexcept;

/// Shutdown the DAP server.
void DbgServerShutdown() noexcept;

/// Run the DAP message loop (blocking).
/// Reads messages from transport, dispatches to handlers, sends responses.
void DbgServerRun() noexcept;

/// Send a "stopped" event to the client.
/// Called from the interpreter hook (DbgNotifyPaused).
void DbgServerSendStopped(DapStoppedReason reason, int thread_id) noexcept;

/// Signal the server to stop (called from hooks during shutdown).
void DbgServerSignalStop() noexcept;

/// Dispatch a DAP request directly (test support, bypasses transport).
/// Parses the JSON body, dispatches to the appropriate handler,
/// and sends the response via DbgTransportWriteMessage.
void DbgServerDispatchTestRequest(const char* json_body, int body_len) noexcept;

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_DEBUGGER

#endif  // CHAOS_IL2CPP_DBG_SERVER_H_
