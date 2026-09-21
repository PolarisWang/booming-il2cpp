// mutex_stubs.cpp — System.Threading.Mutex stub declarations
//
// Platform-specific implementations are in mutex_stubs_win32.cpp and
// mutex_stubs_posix.cpp, selected at build time via CMake generator
// expressions.

#include <chaos/native_types.h>
#include "generated_code_compat.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ── Helper: extract UTF-8 string from a managed string pointer ───────────
static const char* GetManagedStringUtf8(CHAOS_IL2CPP_INTPTR name_ptr) noexcept {
    if (name_ptr == 0) return nullptr;
    auto* str = reinterpret_cast<const chaos_managed_string*>(name_ptr);
    return (str != nullptr && str->length > 0) ? str->utf8_data : nullptr;
}

// Mutex functions — implemented per-platform:
//   CHAOS_IL2CPP_INTPTR chaos_mutex_create(CHAS_IL2CPP_INT32 initially_owned, CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INTPTR* created_new);
//   CHAOS_IL2CPP_INTPTR chaos_mutex_open(CHAOS_IL2CPP_INTPTR name);
//   CHAOS_IL2CPP_INT32  chaos_mutex_release(CHAOS_IL2CPP_INTPTR handle);
//   CHAOS_IL2CPP_INT32  chaos_mutex_wait_one(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INT32 timeout_ms);
//   CHAOS_IL2CPP_INT32  chaos_mutex_close(CHAOS_IL2CPP_INTPTR handle);

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
