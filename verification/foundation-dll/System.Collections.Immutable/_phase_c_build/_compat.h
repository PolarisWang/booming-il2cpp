// Phase C compatibility header — force-included to provide symbols the C# emitter
// references that are missing from or namespaced differently in native headers.

#ifndef CHAOS_IL2CPP_VERIFICATION_COMPAT_H_
#define CHAOS_IL2CPP_VERIFICATION_COMPAT_H_

#include <chaos/native_types.h>
#include <chaos/ptr_tag.h>

// -- Global aliases for namespace-scoped constants ---------------------------
constexpr auto chaos_managed_pointer_local_slot_tag =
    ChaosIl2cpp::Common::k_managed_pointer_local_slot_tag;

constexpr auto chaos_raw_int32_pointer_tag =
    ChaosIl2cpp::Common::k_raw_int32_pointer_tag;

// -- StringId helpers --------------------------------------------------------
// StringIds use the upper 16 bits of a 64-bit value as a non-canonical tag.
inline bool chaos_is_string_id(CHAOS_IL2CPP_INTPTR value) noexcept
{
    auto raw = static_cast<CHAOS_IL2CPP_UINT64>(value);
    auto upper = static_cast<CHAOS_IL2CPP_UINT16>(raw >> 48);
    return upper != 0 && upper != 0xFFFF;
}

inline CHAOS_IL2CPP_UINT64 chaos_extract_string_id(CHAOS_IL2CPP_INTPTR value) noexcept
{
    return static_cast<CHAOS_IL2CPP_UINT64>(value);
}

// CHAOS_IL2CPP_STRING_TYPE is defined per-TU after the String struct decl.
#ifndef CHAOS_IL2CPP_STRING_TYPE
#define CHAOS_IL2CPP_STRING_TYPE chaos_type_System_Private_CoreLib_System_String
#endif

#endif
