#ifndef CHAOS_IL2CPP_ARITHMETIC_CHAOS_BRIDGE_H_
#define CHAOS_IL2CPP_ARITHMETIC_CHAOS_BRIDGE_H_

// ── Bridge: chaos_wrap_add / chaos_div / chaos_rem / chaos_shift_* → ChaosIl2cpp::Common ──
//
// Generated C++ code (native-aot.generated.cpp) calls chaos_wrap_add(),
// chaos_div(), etc. as bare identifiers.  The actual implementations live
// in ChaosIl2cpp::Common namespace (arithmetic.h included via <chaos/common.h>).
//
// These thin inline wrappers bridge the naming gap so that no changes
// to the emitter are needed for older generated files.

#include <chaos/arithmetic.h>

inline CHAOS_IL2CPP_INT32 chaos_wrap_add(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right) noexcept
{
    return ChaosIl2cpp::Common::wrap_add(left, right);
}

inline CHAOS_IL2CPP_INT32 chaos_wrap_sub(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right) noexcept
{
    return ChaosIl2cpp::Common::wrap_sub(left, right);
}

inline CHAOS_IL2CPP_INT32 chaos_wrap_mul(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right) noexcept
{
    return ChaosIl2cpp::Common::wrap_mul(left, right);
}

inline CHAOS_IL2CPP_INT32 chaos_div(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right)
{
    return ChaosIl2cpp::Common::div(left, right);
}

inline CHAOS_IL2CPP_INT32 chaos_rem(CHAOS_IL2CPP_INT32 left, CHAOS_IL2CPP_INT32 right)
{
    return ChaosIl2cpp::Common::rem(left, right);
}

#endif  // CHAOS_IL2CPP_ARITHMETIC_CHAOS_BRIDGE_H_
