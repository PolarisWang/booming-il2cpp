// dbg_variables.h — Variable inspection for IL-level debugging
//
// Provides local variable and parameter inspection from the current
// FastFrame.  Maps DAP variablesReference IDs to frame slots.

#ifndef CHAOS_IL2CPP_DBG_VARIABLES_H_
#define CHAOS_IL2CPP_DBG_VARIABLES_H_

#include "dbg_config.h"

#if CHAOS_IL2CPP_DEBUGGER == 1

#include "dbg_protocol.h"

#include <vector>

namespace chaos::il2cpp::diagnostics {

/// Initialize the variable inspection module.
void DbgVariablesInitialize() noexcept;

/// Shutdown.
void DbgVariablesShutdown() noexcept;

/// Get the scopes for a stack frame.
/// @param frame_id  Stack frame ID (from stackTrace response).
/// @return List of DAP scopes.
std::vector<DapScope> DbgGetScopes(int frame_id) noexcept;

/// Get variables for a scope or variable reference.
/// @param variables_reference  Reference ID (from scopes or parent variable).
/// @return List of DAP variables.
std::vector<DapVariable> DbgGetVariables(int variables_reference) noexcept;

/// Convert a raw tag byte to a human-readable type name.
const char* TagToTypeName(uint8_t tag) noexcept;

/// Format a captured variable value as a display string.
void FormatValue(uint64_t val, uint8_t tag, char* buf, size_t buf_size) noexcept;

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_DEBUGGER

#endif  // CHAOS_IL2CPP_DBG_VARIABLES_H_
