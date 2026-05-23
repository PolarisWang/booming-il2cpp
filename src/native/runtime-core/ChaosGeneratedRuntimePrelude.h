#ifndef CHAOS_IL2CPP_GENERATED_RUNTIME_PRELUDE_H_
#define CHAOS_IL2CPP_GENERATED_RUNTIME_PRELUDE_H_

// ═══════════════════════════════════════════════════════════════════════
// Chaos Generated Runtime Prelude
//
// Common runtime helper functions shared by ALL generated native-aot
// translation units. Included at file scope before the generated
// namespace block so these definitions are visible from inside
// `namespace chaos::il2cpp::jit::<AssemblyName>` via unqualified
// lookup (they live in the parent `chaos::il2cpp::jit` namespace).
//
// These functions are `inline` to satisfy ODR — every TU that includes
// this header gets the same function body.
// ═══════════════════════════════════════════════════════════════════════

#include <chaos/common.h>
#include <chaos/type_info.h>
#include "generated_code_compat.h"

// ── Compatibility macros for generated array-store code ─────────────────
// GC_END_STUBBORN_CHANGE is a no-op under BGC (card-table + SATB replace
// the stubborn-region concept from Boehm GC).  Keep the macro so existing
// generated code compiles without modification.
#define GC_END_STUBBORN_CHANGE(obj) ((void)(obj))

namespace chaos::il2cpp::jit {

// ── Managed array type ───────────────────────────────────────────────
constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_managed_array = 1;
inline TypeInfoV0 chaos_type_info_managed_array = {{ nullptr, nullptr, 1ULL, 0, 32, 2, 0 }, { nullptr, nullptr, 0, 0, 0, 0 }};

struct chaos_managed_array
{
    ThinLockableHeader header{};
    CHAOS_IL2CPP_UINT8 element_type_shape = 0;
    const TypeInfo* element_type_info = nullptr;
    CHAOS_IL2CPP_INTPTR length = 0;
    CHAOS_IL2CPP_INTPTR* elements = nullptr;
};

// ── Type shape constants ─────────────────────────────────────────────
constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_reference = 1;
constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_value = 2;
constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_interface = 3;

// ── Managed pointer tag ──────────────────────────────────────────────
#ifdef CHAOS_IL2CPP_VERIFY_MODE
constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = 0;
#else
constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = 1;
#endif

// ── Normalize native int argument ────────────────────────────────────
inline CHAOS_IL2CPP_INTPTR chaos_normalize_native_int_argument(CHAOS_IL2CPP_INTPTR chaos_value) noexcept
{
    if ((chaos_value & chaos_managed_pointer_local_slot_tag) == 0)
    {
        return chaos_value;
    }

    auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_value & ~chaos_managed_pointer_local_slot_tag));
    return *chaos_slot;
}

// ── Resolve managed value pointer (template) ─────────────────────────
template <typename TValue>
TValue* chaos_resolve_managed_value_pointer(CHAOS_IL2CPP_INTPTR chaos_managed_pointer)
{
    if ((chaos_managed_pointer & chaos_managed_pointer_local_slot_tag) != 0)
    {
        auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_managed_pointer & ~chaos_managed_pointer_local_slot_tag));
        if (*chaos_slot == static_cast<CHAOS_IL2CPP_INTPTR>(0))
        {
            *chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NEW_GC(TValue));
        }
        return reinterpret_cast<TValue*>(*chaos_slot);
    }

    return reinterpret_cast<TValue*>(chaos_managed_pointer);
}

// ── Virtual method table resolver ────────────────────────────────────
inline void* chaos_vtable_resolve(const void** vtable, CHAOS_IL2CPP_UINT32 slot) noexcept
{
    if (vtable == nullptr) CHAOS_IL2CPP_FAIL();
    if (vtable[slot] == nullptr) CHAOS_IL2CPP_FAIL();
    return const_cast<void*>(vtable[slot]);
}

// ── Parent type info resolver ────────────────────────────────────────
inline const TypeInfo* chaos_get_parent_type_info(const TypeInfo* chaos_ti) noexcept
{
    if (chaos_ti == nullptr) return nullptr;
    return chaos_ti->parent;
}

// ── Type compatibility (TypeInfo* pointer based) ─────────────────────
inline bool chaos_is_type_compatible(const TypeInfo* chaos_actual_type_info, const TypeInfo* chaos_target_type_info) noexcept
{
    auto* chaos_current = chaos_actual_type_info;
    while (chaos_current != nullptr)
    {
        if (chaos_current == chaos_target_type_info || chaos_current->stable_id == chaos_target_type_info->stable_id)
        {
            return true;
        }

        chaos_current = chaos_current->parent;
    }

    return false;
}

// ── EH catch type check ──────────────────────────────────────────
// Unified type compatibility check for catch blocks across all EH modes.
inline bool chaos_eh_match_type(CHAOS_IL2CPP_INTPTR exc_obj, const TypeInfo* expected) noexcept {
    if (exc_obj < 0) return false;
    if (expected == nullptr) return true;
    auto* h = reinterpret_cast<const ThinLockableHeader*>(reinterpret_cast<const void*>(exc_obj));
    if (h == nullptr) return true;
    return chaos_is_type_compatible(chaos_object_get_type_info(h), expected);
}

// ── Interface check (iface_map + runtime_iface_map linear scan via WarmPtr) ──
inline bool chaos_type_implements_interface(const TypeInfo* chaos_actual_type_info, const TypeInfo* chaos_target_interface_type_info) noexcept
{
    if (chaos_actual_type_info == nullptr || chaos_target_interface_type_info == nullptr)
    {
        return false;
    }

    const auto* chaos_warm = GetWarmPtr(chaos_actual_type_info);
    if (chaos_warm == nullptr) return false;

    if (chaos_warm->iface_count == 0 &&
        chaos_warm->runtime_iface_count == 0)
    {
        return false;
    }

    for (CHAOS_IL2CPP_UINT32 chaos_i = 0; chaos_i < chaos_warm->iface_count; chaos_i++)
    {
        if (chaos_warm->iface_map[chaos_i].iface_stable_id == chaos_target_interface_type_info->stable_id)
        {
            return true;
        }
    }

    for (CHAOS_IL2CPP_UINT32 chaos_i = 0; chaos_i < chaos_warm->runtime_iface_count; chaos_i++)
    {
        if (chaos_warm->runtime_iface_map[chaos_i].iface_stable_id == chaos_target_interface_type_info->stable_id)
        {
            return true;
        }
    }

    return false;
}

// ── Walk hierarchy to check interface implementation ─────────────────
inline bool chaos_does_type_implement_interface(const TypeInfo* chaos_actual_type_info, const TypeInfo* chaos_target_interface_type_info) noexcept
{
    auto* chaos_current = chaos_actual_type_info;
    while (chaos_current != nullptr)
    {
        if (chaos_type_implements_interface(chaos_current, chaos_target_interface_type_info))
        {
            return true;
        }

        chaos_current = chaos_current->parent;
    }

    return false;
}

// ── Array type compatibility (element shape + element type) ──────────
inline bool chaos_is_array_type_compatible(
    CHAOS_IL2CPP_UINT8 chaos_actual_element_shape,
    const TypeInfo* chaos_actual_element_type_info,
    CHAOS_IL2CPP_UINT8 chaos_target_element_shape,
    const TypeInfo* chaos_target_element_type_info) noexcept
{
    if (chaos_actual_element_shape == chaos_type_shape_reference)
    {
        if (chaos_target_element_shape == chaos_type_shape_reference)
        {
            return chaos_is_type_compatible(chaos_actual_element_type_info, chaos_target_element_type_info);
        }

        if (chaos_target_element_shape == chaos_type_shape_interface)
        {
            return chaos_does_type_implement_interface(chaos_actual_element_type_info, chaos_target_element_type_info);
        }

        return false;
    }

    return chaos_actual_element_shape == chaos_target_element_shape
        && chaos_actual_element_type_info == chaos_target_element_type_info;
}

}  // namespace chaos::il2cpp::jit

// File-scope aliases for the new codegen namespace pattern.
// Generated code now uses `namespace chaos::il2cpp::codegen::<Assembly>`
// with `using namespace chaos::il2cpp::runtime_core;` (not `jit`), so
// bring commonly-used jit-only symbols into global scope for unqualified
// lookup.  Type shape constants are excluded — they already exist in
// chaos::il2cpp::common (via type_info.h) and bringing the jit copies
// to file scope creates ambiguity.
using chaos::il2cpp::jit::chaos_managed_pointer_local_slot_tag;
using chaos::il2cpp::jit::chaos_is_type_compatible;
using chaos::il2cpp::jit::chaos_does_type_implement_interface;

#endif  // CHAOS_IL2CPP_GENERATED_RUNTIME_PRELUDE_H_
