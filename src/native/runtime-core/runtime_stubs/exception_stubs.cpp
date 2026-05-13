// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// exception_stubs.cpp — Exception helper stub implementations
#include <chaos/native_types.h>
#include "generated_code_compat.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

CHAOS_IL2CPP_INTPTR ChaosExceptionGetBaseException(CHAOS_IL2CPP_INTPTR exc) noexcept
{
    return exc;
}

CHAOS_IL2CPP_INTPTR ChaosExceptionGetInnerException(CHAOS_IL2CPP_INTPTR exc) noexcept
{
    (void)exc;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosExceptionGetHresult(CHAOS_IL2CPP_INTPTR exc) noexcept
{
    (void)exc;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosRuntimewrappedGetWrappedException(CHAOS_IL2CPP_INTPTR exc) noexcept
{
    return exc;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core