// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// char_stubs.cpp — Char stub implementations
#include <chaos/native_types.h>
#include "generated_code_compat.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

CHAOS_IL2CPP_INT32 ChaosCharIsDigit(CHAOS_IL2CPP_INT32 c) noexcept
{
    return (c >= 0x30 && c <= 0x39) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosCharIsLetter(CHAOS_IL2CPP_INT32 c) noexcept
{
    return ((c >= 0x41 && c <= 0x5A) || (c >= 0x61 && c <= 0x7A)) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosCharIsWhiteSpace(CHAOS_IL2CPP_INT32 c) noexcept
{
    return (c == 0x20 || c == 0x09 || c == 0x0A || c == 0x0D || c == 0x0C) ? 1 : 0;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core