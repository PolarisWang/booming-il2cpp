// dbg_instance.h — Debugger singleton management
//
// Central initialize/shutdown for the IL-level debugger subsystem.

#ifndef CHAOS_IL2CPP_DBG_INSTANCE_H_
#define CHAOS_IL2CPP_DBG_INSTANCE_H_

#include "dbg_config.h"

#if CHAOS_IL2CPP_DEBUGGER == 1

namespace chaos::il2cpp::diagnostics {

/// Initialize the debugger subsystem.
/// Starts the DAP transport and server thread.
/// @return true on success.
bool DbgInitialize() noexcept;

/// Shutdown the debugger subsystem.
void DbgShutdown() noexcept;

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_DEBUGGER

#endif  // CHAOS_IL2CPP_DBG_INSTANCE_H_
