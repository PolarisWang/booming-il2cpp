// dbg_stepping.h — Stepping controller for IL-level debugging
//
// Manages step state (Step Into / Step Over / Step Out) during
// interpreter execution.  The FastExecute hook checks the stepping
// state after each IL instruction and reports when a step boundary
// is reached.

#ifndef CHAOS_IL2CPP_DBG_STEPPING_H_
#define CHAOS_IL2CPP_DBG_STEPPING_H_

#include "dbg_config.h"

#if CHAOS_IL2CPP_DEBUGGER == 1

#include <cstdint>

namespace chaos::il2cpp::diagnostics {

/// Stepping mode.
enum class StepMode {
    None,       // not stepping
    StepInto,   // step by single IL instruction
    StepOver,   // skip call targets, stop at next instruction in same frame
    StepOut,    // continue until current frame returns
};

/// Initialize the stepping controller.
void DbgSteppingInitialize() noexcept;

/// Shutdown the stepping controller.
void DbgSteppingShutdown() noexcept;

/// Begin stepping in the given mode.
/// @param thread_id  Managed thread ID.
/// @param mode       Stepping mode.
/// @param frame_depth  Current frame depth (for StepOver/StepOut).
void DbgBeginStepping(int thread_id, StepMode mode, int frame_depth) noexcept;

/// End stepping (target reached).
void DbgEndStepping() noexcept;

/// Check if stepping is active.
bool DbgIsStepping() noexcept;

/// Get the current stepping mode.
StepMode DbgGetStepMode() noexcept;

/// Get the target frame depth for StepOver/StepOut.
int DbgGetStepTargetDepth() noexcept;

/// Check if we should stop at the current position.
/// Called after each IL instruction in FastExecute.
/// @param current_frame_depth  Current frame depth.
/// @return true if stepping should stop here.
bool DbgShouldStopAtCurrentPosition(int current_frame_depth) noexcept;

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_DEBUGGER

#endif  // CHAOS_IL2CPP_DBG_STEPPING_H_
