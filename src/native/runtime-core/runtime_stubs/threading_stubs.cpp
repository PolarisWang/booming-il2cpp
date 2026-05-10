// threading_stubs.cpp — Threading stub implementations
#include <chaos/native_types.h>
#include "generated_code_compat.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

CHAOS_IL2CPP_INTPTR chaos_thread_get_current(void) noexcept
{
    return 0;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core