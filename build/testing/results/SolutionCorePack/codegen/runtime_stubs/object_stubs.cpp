// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// object_stubs.cpp — Object / RuntimeHelpers stub implementations
#include <chaos/native_types.h>
#include <chaos/log.h>
#include "generated_code_compat.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

CHAOS_IL2CPP_INTPTR ChaosObjectEqualsStatic(CHAOS_IL2CPP_INTPTR /*left*/, CHAOS_IL2CPP_INTPTR /*right*/) noexcept
{
    CHAOS_IL2CPP_LOG_WARN("Stub", "object_stubs called");
    return 0;
}

void ChaosObjectCtor(CHAOS_IL2CPP_INTPTR /*obj*/) noexcept
{
}

CHAOS_IL2CPP_INTPTR ChaosRuntimeHelpersEquals(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) noexcept
{
    return (left == right) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosRuntimeHelpersGetHashCode(CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (value == 0) return 0;
    auto p = static_cast<CHAOS_IL2CPP_UINTPTR>(value);
    return static_cast<CHAOS_IL2CPP_INT32>(p ^ (p >> 32));
}

CHAOS_IL2CPP_INTPTR ChaosRuntimeHelpersGetObjectValue(CHAOS_IL2CPP_INTPTR value) noexcept
{
    return value;
}

CHAOS_IL2CPP_INT32 ChaosObjectGetHashCode(CHAOS_IL2CPP_INTPTR obj) noexcept
{
    if (obj == 0) return 0;
    auto p = static_cast<CHAOS_IL2CPP_UINTPTR>(obj);
    return static_cast<CHAOS_IL2CPP_INT32>(p ^ (p >> 32));
}
CHAOS_IL2CPP_INTPTR ChaosObjectToString(CHAOS_IL2CPP_INTPTR obj) noexcept {
    (void)obj;
    static CHAOS_IL2CPP_UINT8 s_sentinel = 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);
}
CHAOS_IL2CPP_INTPTR ChaosObjectGetType(CHAOS_IL2CPP_INTPTR obj) noexcept {
    (void)obj;
    static CHAOS_IL2CPP_UINT8 s_sentinel = 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core