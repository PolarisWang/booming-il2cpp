#ifndef CHAOS_IL2CPP_ARITHMETIC_CHAOS_BRIDGE_H_
#define CHAOS_IL2CPP_ARITHMETIC_CHAOS_BRIDGE_H_

// ── Bridge: ChaosWrapAdd / ChaosDiv / ChaosRem → chaos::il2cpp::common ──
//
// Generated C++ code calls ChaosWrapAdd(), ChaosDiv(), etc. as bare
// identifiers from anonymous namespaces.  The actual implementations
// live in chaos::il2cpp::common namespace (arithmetic.h).
//
// These thin inline wrappers bridge the naming gap.  The wrapper
// functions are declared inside chaos::il2cpp::runtime_core and
// exposed at global scope via using declarations below.

#include <chaos/arithmetic.h>

namespace chaos::il2cpp::runtime_core {

inline CHAOS_IL2CPP_INT32 ChaosWrapAdd(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right) noexcept
{
    return chaos::il2cpp::common::wrap_add(left, right);
}

inline CHAOS_IL2CPP_INT32 ChaosWrapSub(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right) noexcept
{
    return chaos::il2cpp::common::wrap_sub(left, right);
}

inline CHAOS_IL2CPP_INT32 ChaosWrapMul(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right) noexcept
{
    return chaos::il2cpp::common::wrap_mul(left, right);
}

inline CHAOS_IL2CPP_INT32 ChaosDiv(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right)
{
    return chaos::il2cpp::common::div(left, right);
}

inline CHAOS_IL2CPP_INT32 ChaosRem(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right)
{
    return chaos::il2cpp::common::rem(left, right);
}

}  // namespace chaos::il2cpp::runtime_core

// Expose at global scope so generated code (inside anonymous namespaces)
// can call these as bare identifiers.
using chaos::il2cpp::runtime_core::ChaosWrapAdd;
using chaos::il2cpp::runtime_core::ChaosWrapSub;
using chaos::il2cpp::runtime_core::ChaosWrapMul;
using chaos::il2cpp::runtime_core::ChaosDiv;
using chaos::il2cpp::runtime_core::ChaosRem;

#endif  // CHAOS_IL2CPP_ARITHMETIC_CHAOS_BRIDGE_H_
