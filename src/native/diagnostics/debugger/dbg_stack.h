// dbg_stack.h — Call stack inspection for IL-level debugging
//
// Provides managed call stack unwinding for the DAP stackTrace request.
// Reads from the frame snapshot captured by FastExecute at breakpoint time.

#ifndef CHAOS_IL2CPP_DBG_STACK_H_
#define CHAOS_IL2CPP_DBG_STACK_H_

#include "dbg_config.h"

#if CHAOS_IL2CPP_DEBUGGER == 1

#include "dbg_protocol.h"

#include <cstdint>
#include <vector>

namespace chaos::il2cpp::diagnostics {

/// Initialize the stack inspection module.
void DbgStackInitialize() noexcept;

/// Shutdown.
void DbgStackShutdown() noexcept;

/// Get the current call stack as DAP stack frames.
/// @param thread_id  Managed thread ID (reserved, currently uses current thread).
/// @param start_frame  Starting frame index (for paging).
/// @param levels  Max number of frames to return.
/// @return List of DAP stack frames.
std::vector<DapStackFrame> DbgGetStackTrace(int thread_id, int start_frame, int levels) noexcept;

/// Get the current frame depth (number of managed frames on the stack).
int DbgGetFrameDepth() noexcept;

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_DEBUGGER

#endif  // CHAOS_IL2CPP_DBG_STACK_H_
