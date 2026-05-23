// dbg_breakpoint.h — Breakpoint manager for IL-level debugging
//
// Manages breakpoints registered by the DAP client.  Maps from
// (method_token, il_offset) pairs to active breakpoints.
// Provides O(1) lookup for the FastExecute/RegisterExecute hooks.

#ifndef CHAOS_IL2CPP_DBG_BREAKPOINT_H_
#define CHAOS_IL2CPP_DBG_BREAKPOINT_H_

#include "dbg_config.h"

#if CHAOS_IL2CPP_DEBUGGER == 1

#include "dbg_protocol.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace chaos::il2cpp::diagnostics {

/// A breakpoint key: (method_token, il_offset).
struct BreakpointKey {
    uint32_t method_token;  // metadata token of the method
    uint32_t il_offset;     // IL offset within the method
};

/// Hash support for BreakpointKey.
struct BreakpointKeyHash {
    size_t operator()(const BreakpointKey& k) const {
        return (static_cast<size_t>(k.method_token) << 32) | k.il_offset;
    }
};

struct BreakpointKeyEqual {
    bool operator()(const BreakpointKey& a, const BreakpointKey& b) const {
        return a.method_token == b.method_token && a.il_offset == b.il_offset;
    }
};

/// Initialize the breakpoint manager.
void DbgBreakpointInitialize() noexcept;

/// Shutdown the breakpoint manager.
void DbgBreakpointShutdown() noexcept;

/// Set a breakpoint at (method_token, il_offset).
/// @return The breakpoint ID (assigned by the manager).
int DbgSetBreakpoint(uint32_t method_token, uint32_t il_offset,
                     const char* source_path, int line) noexcept;

/// Remove a breakpoint by ID.
void DbgRemoveBreakpoint(int breakpoint_id) noexcept;

/// Clear all breakpoints.
void DbgClearAllBreakpoints() noexcept;

/// Check if a breakpoint exists at (method_token, il_offset).
/// O(1) lookup — used by FastExecute/RegisterExecute hooks.
/// @return Breakpoint ID if found, -1 if not.
int DbgCheckBreakpoint(uint32_t method_token, uint32_t il_offset) noexcept;

/// Get all currently set breakpoints (for stackTrace response).
std::vector<DapBreakpoint> DbgGetAllBreakpoints() noexcept;

/// Get breakpoints for a specific source file (for setBreakpoints response).
std::vector<DapBreakpoint> DbgGetBreakpointsForSource(const char* source_path) noexcept;

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_DEBUGGER

#endif  // CHAOS_IL2CPP_DBG_BREAKPOINT_H_
