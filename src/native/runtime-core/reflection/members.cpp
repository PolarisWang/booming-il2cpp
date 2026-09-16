// ── GetMembers ────────────────────────────────────────────────────
// Merges fields, methods, properties, and nested types into a single
// flat buffer: [total_count, (kind_0, handle_0), (kind_1, handle_1), ...]
// (kind = 0 field, 1 method, 2 property, 3 nested type)
// The managed wrapper iterates this buffer to construct MemberInfo[].

// Managed-array access (chaos_managed_array / ChaosArrayNew1D) for the
// Type::GetMembers managed-array wrapper below.
#include "../runtime_stubs/array_stubs.h"
#include "../ChaosGeneratedRuntimePrelude.h"

extern "C" {
namespace chaos::il2cpp::runtime_core {

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMembers(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;

    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr) return 0;

    uint32_t field_count    = (desc->fields    != nullptr) ? desc->field_count    : 0;
    uint32_t method_count   = (desc->methods   != nullptr) ? desc->method_count   : 0;
    uint32_t property_count = (desc->properties != nullptr) ? desc->property_count : 0;

    // Nested types via Module Registry Tier 0 (tag-bit-63 handles skip this)
    TypeRef tr;
    uint32_t nested_count = 0;
    uint32_t nested_module_id = 0;
    uint32_t nested_start = 0;
    if (ResolveTypeRef(type_handle, tr) && tr.module->nested_type_offset != nullptr) {
        nested_start      = tr.module->nested_type_offset[tr.type_index];
        uint32_t end      = tr.module->nested_type_offset[tr.type_index + 1];
        nested_count      = end - nested_start;
        nested_module_id  = tr.module_id;
    }

    uint32_t total = field_count + method_count + property_count + nested_count;
    if (total == 0 || total > 128) return 0;

    // Buffer entries: [count, (kind, handle) x total] = 1 + 2*total
    thread_local CHAOS_IL2CPP_INTPTR s_buffer[257];
    s_buffer[0] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<intptr_t>(total));

    constexpr CHAOS_IL2CPP_INTPTR kField      = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    constexpr CHAOS_IL2CPP_INTPTR kMethod     = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    constexpr CHAOS_IL2CPP_INTPTR kProperty   = static_cast<CHAOS_IL2CPP_INTPTR>(2);
    constexpr CHAOS_IL2CPP_INTPTR kNestedType = static_cast<CHAOS_IL2CPP_INTPTR>(3);

    uint32_t idx = 1;

    for (uint32_t i = 0; i < field_count; i++) {
        s_buffer[idx++] = kField;
        s_buffer[idx++] = static_cast<CHAOS_IL2CPP_INTPTR>(
            EncodeReflectionQueryFieldHandle(&desc->fields[i]));
    }
    for (uint32_t i = 0; i < method_count; i++) {
        s_buffer[idx++] = kMethod;
        s_buffer[idx++] = static_cast<CHAOS_IL2CPP_INTPTR>(
            EncodeReflectionQueryMethodHandle(&desc->methods[i]));
    }
    for (uint32_t i = 0; i < property_count; i++) {
        s_buffer[idx++] = kProperty;
        s_buffer[idx++] = static_cast<CHAOS_IL2CPP_INTPTR>(
            EncodeReflectionQueryPropertyHandle(&desc->properties[i]));
    }
    for (uint32_t i = 0; i < nested_count; i++) {
        s_buffer[idx++] = kNestedType;
        uint32_t child_token = tr.module->nested_type_children[nested_start + i];
        s_buffer[idx++] = static_cast<CHAOS_IL2CPP_INTPTR>(
            MakeTypeHandle(nested_module_id, child_token));
    }

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buffer);
}

// ── GetMembers as a managed MemberInfo[] ─────────────────────────
// ATG subjects index the Type::GetMembers result as a managed array
// (chaos_managed_array: length @24, inline elements) and then invoke member
// accessors on the elements.  The flat [count, (kind, handle)...] buffer is
// NOT bit-compatible with that layout — indexing it trips the bounds check
// and FAIL_FASTs (uncatchable by any managed try/catch, B7 si=138-142).
// This wrapper rebuilds the member handles as a real managed INTPTR[].
//
// BindingFlags are accepted but deliberately NOT filtered: the Tier-2
// descriptor model carries no accessibility data, so the honest answer is
// the unfiltered superset rather than a silently-wrong filtered subset.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetMembersManagedArray(
    CHAOS_IL2CPP_INTPTR type_handle,
    CHAOS_IL2CPP_INT32 /*binding_flags*/) noexcept {
    using namespace chaos::il2cpp::runtime_core;

    CHAOS_IL2CPP_INTPTR flat = ChaosReflectionGetMembers(type_handle);
    if (flat == 0) return 0;

    const auto* buf = reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(flat);
    const auto total = static_cast<CHAOS_IL2CPP_INT32>(buf[0]);
    if (total <= 0 || total > 128) return 0;

    // Elements are encoded reflection-query handles (tagged integers), NOT
    // heap references — allocate with a non-reference element shape so the
    // GC never scans them.
    auto* arr = reinterpret_cast<chaos::il2cpp::jit::chaos_managed_array*>(
        ChaosArrayNew1D(
            &chaos::il2cpp::jit::chaos_type_info_managed_array.hot,
            &chaos::il2cpp::jit::chaos_type_info_managed_array.hot,
            chaos::il2cpp::jit::chaos_type_shape_value,
            static_cast<CHAOS_IL2CPP_INTPTR>(total)));
    if (arr == nullptr) return 0;

    auto* elements = chaos::il2cpp::jit::chaos_array_get_elements(arr);
    for (CHAOS_IL2CPP_INT32 i = 0; i < total; i++) {
        // flat layout: [count, (kind, handle) x total] — keep the handle slot.
        elements[i] = buf[2 + static_cast<CHAOS_IL2CPP_INTPTR>(2 * i)];
    }
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetNestedTypes(CHAOS_IL2CPP_INTPTR type_handle) {
    TypeRef tr;
    if (!ResolveTypeRef(type_handle, tr)) return 0;
    if (tr.module->nested_type_offset == nullptr) return 0;

    uint32_t start = tr.module->nested_type_offset[tr.type_index];
    uint32_t end = tr.module->nested_type_offset[tr.type_index + 1];
    uint32_t count = end - start;
    if (count == 0 || count > 32) return 0;

    thread_local CHAOS_IL2CPP_INTPTR buffer[33];
    buffer[0] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<intptr_t>(count));
    for (uint32_t i = 0; i < count; i++) {
        uint32_t child_token = tr.module->nested_type_children[start + i];
        buffer[1 + i] = static_cast<CHAOS_IL2CPP_INTPTR>(
            MakeTypeHandle(tr.module_id, child_token));
    }
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(buffer);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetField(
    CHAOS_IL2CPP_INTPTR type_handle,
    CHAOS_IL2CPP_INTPTR name_string_id)
{
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->fields == nullptr) return 0;

    const char* field_name = DecodeAndNullTerminateString(name_string_id);
    if (field_name == nullptr) return 0;

    auto* field = FindReflectionQueryField(desc, field_name);
    if (field == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryFieldHandle(field));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethod(
    CHAOS_IL2CPP_INTPTR type_handle,
    CHAOS_IL2CPP_INTPTR name_string_id,
    CHAOS_IL2CPP_INTPTR param_types)
{
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->methods == nullptr) return 0;

    const char* method_name = DecodeAndNullTerminateString(name_string_id);
    if (method_name == nullptr) return 0;

    if (param_types != 0) {
        // Managed array layout (chaos_managed_array, mirrored by
        // ManagedArrayAccessor in stub_common.h — both have length at offset 24):
        //   header(8) | element_type_shape(1)+pad(7) | element_type_info(8) | length(8)
        // The previous read used offset sizeof(ThinLockableHeader) == 8, which is
        // element_type_shape + padding — never a valid count.  The path was dead
        // until GetMethod(string, Type[]) was wired to it (no caller passed a real
        // array before), which is why the wrong offset went unnoticed.
        const auto* arr_bytes = reinterpret_cast<const uint8_t*>(
            static_cast<CHAOS_IL2CPP_INTPTR>(param_types));
        const auto managed_length =
            *reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(arr_bytes + 24);
        int32_t param_count = static_cast<int32_t>(managed_length);
        if (param_count < 0) param_count = 0;

        auto* method = FindReflectionQueryMethod(desc, method_name, param_count);
        if (method != nullptr) {
            return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryMethodHandle(method));
        }
    } else {
        // No param_types — return first method with matching name
        for (CHAOS_IL2CPP_UINT32 i = 0; i < desc->method_count; i++) {
            if (NamesMatch(desc->methods[i].name_utf8, method_name)) {
                return static_cast<CHAOS_IL2CPP_INTPTR>(
                    EncodeReflectionQueryMethodHandle(&desc->methods[i]));
            }
        }
    }

    return 0;
}

// ── Type::GetProperty / GetField / GetEvent ──────────────────────────
//
// These resolve a member by name against the type's descriptor tables.  They
// exist because the members were previously unreachable: codegen emitted no
// property/field/event descriptors at all, so the codegen-side helpers were
// deliberate CHAOS_IL2CPP_FAIL() stubs and any member query aborted the
// process.  The descriptor tables are now emitted per closure-reachable type
// and registered through ChaosRegisterExternalType, whose dynamic table
// GetTypeDescriptorFromHandle already consults — so a plain descriptor walk
// is all that is needed here.
//
// Name-only matching (no signature): the callers are the 1-argument
// GetProperty(string)/GetField(string)/GetEvent(string) overloads.  Overload
// resolution among same-named members is not attempted — returning the first
// match mirrors the existing method path's no-param_types branch.

CHAOS_IL2CPP_INTPTR ChaosReflectionTypeGetProperty(
    CHAOS_IL2CPP_INTPTR type_handle,
    CHAOS_IL2CPP_INTPTR name_string_id) noexcept {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->properties == nullptr) return 0;

    const char* name = DecodeAndNullTerminateString(name_string_id);
    if (name == nullptr) return 0;

    for (CHAOS_IL2CPP_UINT32 i = 0; i < desc->property_count; i++) {
        if (NamesMatch(desc->properties[i].name_utf8, name)) {
            return static_cast<CHAOS_IL2CPP_INTPTR>(
                EncodeReflectionQueryPropertyHandle(&desc->properties[i]));
        }
    }
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionTypeGetField(
    CHAOS_IL2CPP_INTPTR type_handle,
    CHAOS_IL2CPP_INTPTR name_string_id) noexcept {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->fields == nullptr) return 0;

    const char* name = DecodeAndNullTerminateString(name_string_id);
    if (name == nullptr) return 0;

    for (CHAOS_IL2CPP_UINT32 i = 0; i < desc->field_count; i++) {
        if (NamesMatch(desc->fields[i].name_utf8, name)) {
            return static_cast<CHAOS_IL2CPP_INTPTR>(
                EncodeReflectionQueryFieldHandle(&desc->fields[i]));
        }
    }
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionTypeGetEvent(
    CHAOS_IL2CPP_INTPTR type_handle,
    CHAOS_IL2CPP_INTPTR name_string_id) noexcept {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->events == nullptr) return 0;

    const char* name = DecodeAndNullTerminateString(name_string_id);
    if (name == nullptr) return 0;

    for (CHAOS_IL2CPP_UINT32 i = 0; i < desc->event_count; i++) {
        if (NamesMatch(desc->events[i].name_utf8, name)) {
            return static_cast<CHAOS_IL2CPP_INTPTR>(
                EncodeReflectionQueryEventHandle(&desc->events[i]));
        }
    }
    return 0;
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"