// dbg_runtime.h — Runtime hooks for breakpoint checking
//
// Inline functions called from FastExecute/RegisterExecute main loops.
// These are the hot-path hooks — must be extremely lightweight.
// When no breakpoints are set, the check compiles to a single branch.

#ifndef CHAOS_IL2CPP_DBG_RUNTIME_H_
#define CHAOS_IL2CPP_DBG_RUNTIME_H_

#include "dbg_config.h"

#if CHAOS_IL2CPP_DEBUGGER == 1

#include "dbg_breakpoint.h"
#include "dbg_stepping.h"

#include <atomic>
#include <cstdint>
#include <string>
#include <vector>

namespace chaos::il2cpp::diagnostics {

/// Global flag: are any breakpoints set?
/// Fast path check — if false, skip the hash lookup entirely.
extern std::atomic<bool> g_dbg_any_breakpoints;

/// Global flag: has the DAP client requested a pause?
/// Set by HandlePause, checked in DbgShouldPause, cleared in DbgNotifyPaused.
extern std::atomic<bool> g_dbg_pause_requested;

/// Check if execution should pause at (method_token, il_offset).
/// Called before each IL instruction in FastExecute.
/// @param method_token  Metadata token of the current method.
/// @param il_offset     Current IL offset.
/// @param frame_depth   Current managed frame depth.
/// @return true if execution should stop (breakpoint hit, pause request, or step boundary).
inline bool DbgShouldPause(uint32_t method_token, uint32_t il_offset,
                           int frame_depth) noexcept
{
    // Fast path: no breakpoints, no pause request, and not stepping → skip.
    if (!g_dbg_any_breakpoints.load(std::memory_order_relaxed) &&
        !DbgIsStepping() &&
        !g_dbg_pause_requested.load(std::memory_order_relaxed)) {
        return false;
    }

    // Pause request has highest priority — stop immediately.
    if (g_dbg_pause_requested.load(std::memory_order_acquire)) {
        return true;
    }

    // Check breakpoints.
    if (g_dbg_any_breakpoints.load(std::memory_order_acquire)) {
        int bp_id = DbgCheckBreakpoint(method_token, il_offset);
        if (bp_id > 0) {
            return true;
        }
    }

    // Check stepping.
    if (DbgIsStepping()) {
        return DbgShouldStopAtCurrentPosition(frame_depth);
    }

    return false;
}

/// Notify the debugger that execution has paused.
/// Called after DbgShouldPause returns true.
void DbgNotifyPaused(uint32_t method_token, uint32_t il_offset) noexcept;

/// Signal the interpreter thread to resume execution.
/// Called by continue/next/stepIn/stepOut handlers after setting stepping state.
void DbgSignalResume() noexcept;

// ── Frame snapshot (opaque capture for stack/variable inspection) ──────
// The runtime code (fast_dispatch.cpp) fills this snapshot when a breakpoint
// is hit.  The debugger reads it in response to DAP stackTrace/scopes/
// variables requests.  This avoids coupling the debugger to FastFrame internals.

/// A single captured stack frame entry.
struct DbgCapturedFrame {
    uint32_t method_token = 0;
    uint32_t il_offset   = 0;
};

/// A single captured local variable value.
struct DbgCapturedLocal {
    uint32_t index = 0;
    uint64_t value = 0;
    uint8_t  tag   = 0;   // 0=unknown, 1=i32, 2=i64, 3=f32, 4=f64, 5=obj
};

static constexpr int kDbgMaxCapturedFrames = 64;
static constexpr int kDbgMaxCapturedLocals = 32;

/// Frame snapshot — filled by runtime, read by debugger.
struct DbgFrameSnapshot {
    int frame_count = 0;
    DbgCapturedFrame frames[kDbgMaxCapturedFrames]{};

    int local_count = 0;
    DbgCapturedLocal locals[kDbgMaxCapturedLocals]{};
};

/// Clear the frame snapshot.
void DbgClearFrameSnapshot() noexcept;

/// Get the current frame snapshot (for debugger handlers).
DbgFrameSnapshot& DbgGetFrameSnapshot() noexcept;

/// Result entry for DbgResolveMethodsForSource.
struct DbgMethodMatch {
    uint32_t method_token;
    const char* method_name;  // pointer to persistent metadata string
    uint32_t type_token;
    int line_number;          // the DAP line number the user requested
};

/// Resolve methods matching a source file name across all registered modules.
/// Iterates modules via ModuleRegistry, scans each module's type descriptors
/// for type names matching the filename stem, and collects their methods.
/// @param source_path  The source path from DAP (used for matching filename stem).
/// @param out_matches  Vector filled with matched (method_token, line) entries.
void DbgResolveMethodsForSource(const char* source_path,
                                std::vector<DbgMethodMatch>& out_matches) noexcept;

/// A single field read from a managed object (for variable expansion).
struct DbgObjectField {
    std::string name;
    std::string type_name;
    std::string value_str;       // formatted display value
    uint64_t child_object_addr;  // 0 = not an object reference, otherwise expandable
};

/// Read fields of a managed object at the given address.
/// Scans module type descriptors via ModuleRegistry to find the type,
/// then reads field values directly from the object's memory.
/// @param object_addr  Address of the managed object.
/// @param max_depth    Maximum nesting depth (3 is a safe default).
/// @return Vector of (name, type, formatted_value, child_object_addr) entries.
std::vector<DbgObjectField> DbgReadObjectFields(uint64_t object_addr,
                                                 int max_depth = 3) noexcept;

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_DEBUGGER

#endif  // CHAOS_IL2CPP_DBG_RUNTIME_H_
