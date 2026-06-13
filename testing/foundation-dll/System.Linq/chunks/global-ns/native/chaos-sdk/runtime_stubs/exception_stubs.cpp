// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// exception_stubs.cpp — Exception helper stub implementations
#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include <chaos/log.h>
#include "generated_code_compat.h"  // ThinLockableHeader
#include "runtime_stubs/exception_stubs.h"
#include <core/delegate_object.h>

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

// Invoke an Action delegate for Assert.Throws<T> verification blocks.
// NOT noexcept: the wrapped Action is expected to throw a managed exception
// (chaos_managed_exception) which must propagate to the caller's try/catch.
void ChaosInvokeAction(CHAOS_IL2CPP_INTPTR action)
{
    using namespace chaos::il2cpp::runtime_core;
    auto* del = reinterpret_cast<DelegateObject*>(action);
    auto* fn = reinterpret_cast<void(*)()>(del->chaos_delegate_method_ptr);
    if (del->chaos_delegate_target) {
        auto closed = reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(fn);
        closed(del->chaos_delegate_target);
    } else {
        fn();
    }
}

