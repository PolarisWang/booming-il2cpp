// Phase C compatibility header — provides symbols the C# emitter references
// that are missing from or namespaced differently in the native headers.
//
// These should eventually be emitted by the C# emitter preamble or fixed in
// the native headers. This file is a compilation bridge for verification.
// See CLAUDE.md: "all changes must originate in RuntimeHelperShapeRegistry"
// for the permanent fix path if any of these become load-bearing.

#ifndef CHAOS_IL2CPP_VERIFICATION_COMPAT_H_
#define CHAOS_IL2CPP_VERIFICATION_COMPAT_H_

#include <chaos/native_types.h>
#include <chaos/ptr_tag.h>

// -- Aliases for emitter-expected global names -------------------------------
//
// The C# emitter writes bare "chaos_managed_pointer_local_slot_tag" and
// "chaos_raw_int32_pointer_tag" into the generated preamble (ObjectModelEmission).
// ptr_tag.h defines them as ChaosIl2cpp::Common::k_* — provide global aliases.

constexpr auto chaos_managed_pointer_local_slot_tag =
    ChaosIl2cpp::Common::k_managed_pointer_local_slot_tag;

constexpr auto chaos_raw_int32_pointer_tag =
    ChaosIl2cpp::Common::k_raw_int32_pointer_tag;

// -- StringId helpers ---------------------------------------------------------
//
// StringIds are non-canonical x64 addresses (upper 16 bits set but not 0xFFFF)
// so they never collide with real heap pointers in the low 48-bit address space.
//
// The emitter writes these into generated preamble (ObjectEqualityEmission,
// ObjectModelEmission, RuntimeHelperShapeRegistry) but no native header
// defines them yet.

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

// -- CHAOS_IL2CPP_STRING_TYPE -------------------------------------------------
//
// Shorthand for the String struct generated into every native-aot.generated.cpp.
// The emitter uses this in runtime helper shapes (RuntimeHelperShapeRegistry).
// Since the struct is declared inside the anonymous namespace in each generated
// TU, this macro is defined per-TU after the struct declaration for now.

// NOTE: _compat.h is force-included before the generated TU, so the struct
// isn't visible yet. A separate local alias is emitted after the struct def.
#ifndef CHAOS_IL2CPP_STRING_TYPE
#define CHAOS_IL2CPP_STRING_TYPE chaos_type_System_Private_CoreLib_System_String
#endif

#endif // CHAOS_IL2CPP_VERIFICATION_COMPAT_H_
