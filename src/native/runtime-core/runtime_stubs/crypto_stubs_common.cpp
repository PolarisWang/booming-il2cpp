// crypto_stubs_common.cpp — Shared alloc_byte_array helper for crypto stubs
// This file is compiled unconditionally and provides the managed byte[]
// allocation helper used by both the BCrypt (Windows) and OpenSSL (POSIX) paths.

#include "crypto_stubs_internal.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ── Managed byte[] allocation helper (shared: BCrypt AND OpenSSL paths) ──
// Allocates a managed byte[] of the given length via the GC, returns the handle
// (0 on failure).  The byte content is zeroed.  Defined in the SHARED region
// (before the _WIN32/_else platform split) so both the BCrypt and OpenSSL hash
// families can use it — a prior refactor (b7e079336) moved it into the #else
// branch only, leaving the BCrypt path with uses-but-no-definition (C3861).
CHAOS_IL2CPP_INTPTR alloc_byte_array(CHAOS_IL2CPP_SIZE length) noexcept
{
    const auto alloc_size = sizeof(ManagedArrayAccessor) + length;
    auto* storage = static_cast<CHAOS_IL2CPP_UINT8*>(GcAllocateAtomic(alloc_size));
    if (storage == nullptr) return 0;
    auto* arr = reinterpret_cast<ManagedArrayAccessor*>(storage);
    arr->element_type_shape = 2;  // value type
    arr->length = static_cast<CHAOS_IL2CPP_INTPTR>(length);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core