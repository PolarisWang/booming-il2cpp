// monitor_stubs.cpp — Monitor enter/exit stubs
//
// SEPARATE from threading_stubs.cpp because MSVC generates C-linkage
// (undecorated) references to threading::SafepointPoll and
// threading::tls_this_thread when inline functions from gc_transition.h
// (GcTransitionToPreemptive / GcTransitionToCooperative) are emitted as
// COMDAT bodies in translation units that also contain extern "C" blocks.
//
// This file contains NO extern "C" code, so the inline functions from
// gc_transition.h are emitted with proper C++ name decoration.
#include <chaos/native_types.h>
#include "thread_state.h"
#include "gc_transition.h"
#include "runtime_core.h"
#include "engine_binding.h"

namespace chaos::il2cpp::runtime_core {

static void MonitorEnterImpl(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INTPTR lockTaken) noexcept
{
    if (obj == 0) return;
    void* monitor_target = reinterpret_cast<void*>(obj);

    GC_TRANSITION_TO_PREEMPTIVE();
    bool result = MonitorEnter(monitor_target);
    GC_TRANSITION_TO_COOPERATIVE();

    if (lockTaken != 0) {
        *reinterpret_cast<CHAOS_IL2CPP_INT32*>(lockTaken) = result ? 1 : 0;
    }
}

static void MonitorExitImpl(CHAOS_IL2CPP_INTPTR obj) noexcept
{
    if (obj == 0) return;
    void* monitor_target = reinterpret_cast<void*>(obj);

    GC_TRANSITION_TO_PREEMPTIVE();
    MonitorExit(monitor_target);
    GC_TRANSITION_TO_COOPERATIVE();
}

}  // namespace chaos::il2cpp::runtime_core

extern "C" {

void chaos_monitor_enter(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INTPTR lockTaken) noexcept
{
    chaos::il2cpp::runtime_core::MonitorEnterImpl(obj, lockTaken);
}

void chaos_monitor_exit(CHAOS_IL2CPP_INTPTR obj) noexcept
{
    chaos::il2cpp::runtime_core::MonitorExitImpl(obj);
}

}  // extern "C"
