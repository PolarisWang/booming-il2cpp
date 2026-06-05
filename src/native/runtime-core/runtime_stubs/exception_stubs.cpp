// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// exception_stubs.cpp — Exception helper stub implementations
#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include <chaos/log.h>
#include "generated_code_compat.h"  // ThinLockableHeader
#include "runtime_stubs/exception_stubs.h"

CHAOS_IL2CPP_INTPTR ChaosExceptionGetBaseException(CHAOS_IL2CPP_INTPTR exc) noexcept
{
    CHAOS_IL2CPP_LOG_WARN("Stub", "exception_stubs called");
    return exc;
}

CHAOS_IL2CPP_INTPTR ChaosExceptionGetInnerException(CHAOS_IL2CPP_INTPTR exc) noexcept
{
    (void)exc;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosExceptionGetHresult(CHAOS_IL2CPP_INTPTR exc) noexcept
{
    if (exc == 0) return 0;
    // Exception layout: ThinLockableHeader(16B) + _message(8B) +
    //   _innerException(8B) + _stackTrace(8B) + _HResult(4B)
    const auto hr_offset = sizeof(ThinLockableHeader) + 3 * sizeof(CHAOS_IL2CPP_INTPTR);
    return *reinterpret_cast<CHAOS_IL2CPP_INT32*>(
        static_cast<CHAOS_IL2CPP_UINT8*>(reinterpret_cast<void*>(exc)) + hr_offset);
}

CHAOS_IL2CPP_INTPTR ChaosRuntimewrappedGetWrappedException(CHAOS_IL2CPP_INTPTR exc) noexcept
{
    return exc;
}

