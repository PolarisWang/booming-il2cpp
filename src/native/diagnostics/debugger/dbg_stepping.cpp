// dbg_stepping.cpp — Stepping controller implementation

#include "dbg_stepping.h"

#if CHAOS_IL2CPP_DEBUGGER == 1

#include <atomic>

namespace chaos::il2cpp::diagnostics {

namespace {

std::atomic<StepMode> g_step_mode{StepMode::None};
std::atomic<int> g_step_thread_id{0};
std::atomic<int> g_step_target_depth{0};

}  // anonymous namespace

void DbgSteppingInitialize() noexcept {
    g_step_mode.store(StepMode::None, std::memory_order_release);
}

void DbgSteppingShutdown() noexcept {
    g_step_mode.store(StepMode::None, std::memory_order_release);
}

void DbgBeginStepping(int thread_id, StepMode mode, int frame_depth) noexcept {
    g_step_thread_id.store(thread_id, std::memory_order_relaxed);
    g_step_mode.store(mode, std::memory_order_release);
    g_step_target_depth.store(frame_depth, std::memory_order_relaxed);
}

void DbgEndStepping() noexcept {
    g_step_mode.store(StepMode::None, std::memory_order_release);
}

bool DbgIsStepping() noexcept {
    return g_step_mode.load(std::memory_order_acquire) != StepMode::None;
}

StepMode DbgGetStepMode() noexcept {
    return g_step_mode.load(std::memory_order_acquire);
}

int DbgGetStepTargetDepth() noexcept {
    return g_step_target_depth.load(std::memory_order_relaxed);
}

bool DbgShouldStopAtCurrentPosition(int current_frame_depth) noexcept {
    auto mode = g_step_mode.load(std::memory_order_acquire);
    if (mode == StepMode::None) return false;

    switch (mode) {
    case StepMode::StepInto:
        // Step Into: stop after every IL instruction.
        return true;

    case StepMode::StepOver:
        // Step Over: stop when back at or above the target frame depth.
        if (current_frame_depth <= g_step_target_depth.load(std::memory_order_relaxed)) {
            return true;
        }
        return false;

    case StepMode::StepOut:
        // Step Out: stop when above the target frame depth (caller frame).
        if (current_frame_depth < g_step_target_depth.load(std::memory_order_relaxed)) {
            return true;
        }
        return false;

    default:
        return false;
    }
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_DEBUGGER
