// chaos_monitor_stubs.cpp — extern "C" ABI stubs for managed monitor calls
// These are compiled as part of entry.exe to provide the extern "C" symbols
// that codegen generates calls to, while the actual logic lives in chaos_runtime_core.lib.

#include <chaos/native_types.h>

namespace chaos { namespace il2cpp { namespace runtime_core {
    bool MonitorEnter(void* monitor_target);
    bool MonitorExit(void* monitor_target);
} } }

extern "C" {

void chaos_monitor_enter(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INT32* lockTaken) noexcept
{
    if (obj == 0) {
        if (lockTaken) *lockTaken = 0;
        return;
    }
    void* monitor_target = reinterpret_cast<void*>(obj);
    bool result = chaos::il2cpp::runtime_core::MonitorEnter(monitor_target);
    if (lockTaken) *lockTaken = result ? 1 : 0;
}

void chaos_monitor_exit(CHAOS_IL2CPP_INTPTR obj) noexcept
{
    if (obj == 0) return;
    void* monitor_target = reinterpret_cast<void*>(obj);
    chaos::il2cpp::runtime_core::MonitorExit(monitor_target);
}

}  // extern "C"
