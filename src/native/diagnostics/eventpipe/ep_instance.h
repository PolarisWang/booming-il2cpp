// ep_instance.h — EventPipe singleton management
//
// Central initialize/shutdown for the entire EventPipe subsystem.
// Called from runtime-core startup and shutdown.

#ifndef CHAOS_IL2CPP_EP_INSTANCE_H_
#define CHAOS_IL2CPP_EP_INSTANCE_H_

#include "ep_config.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

namespace chaos::il2cpp::diagnostics {

/// Initialize the EventPipe subsystem.
/// Creates the named pipe, starts the listener thread, and registers
/// GC/ThreadPool event bridges.
/// Safe to call multiple times (idempotent via once_flag).
/// @return true on success, false on failure.
bool EpInitialize() noexcept;

/// Shutdown the EventPipe subsystem.
/// Disconnects clients, stops the listener thread, and unregisters bridges.
void EpShutdown() noexcept;

/// Check whether EventPipe is currently initialized.
bool EpIsInitialized() noexcept;

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EVENTPIPE

#endif  // CHAOS_IL2CPP_EP_INSTANCE_H_
