// char_stubs.cpp — Char stub implementations
#include <chaos/native_types.h>
#include "generated_code_compat.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

CHAOS_IL2CPP_INT32 ChaosCharIsDigit(CHAOS_IL2CPP_INT32 c) noexcept { (void)c; return 0; }
CHAOS_IL2CPP_INT32 ChaosCharIsLetter(CHAOS_IL2CPP_INT32 c) noexcept { (void)c; return 0; }
CHAOS_IL2CPP_INT32 ChaosCharIsWhiteSpace(CHAOS_IL2CPP_INT32 c) noexcept { (void)c; return 0; }

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core