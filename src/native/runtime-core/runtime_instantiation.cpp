#include "runtime_instantiation.h"
#include "generic_context.h"
#include "reflection_query_model.h"

#include <chaos/native_types.h>

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>
#include <string>
#include <vector>

namespace chaos::il2cpp::runtime_instantiation {

namespace {

/* ── Runtime token allocator ────────────────────────────────────────────── */
/* Start at 0x80000000 to leave the AOT token space (0x00000000–0x7FFFFFFF)
 * uncontested.  Each allocation bumps the counter atomically. */

std::atomic<CHAOS_IL2CPP_UINT32> s_next_runtime_token{0x80000000u};

/* ── Track all heap-allocated RuntimeInstantiatedType instances so we can
 *    unregister + free them during hot-update unload.                     ── */
struct RuntimeInstantiatedTypeRecord {
    RuntimeInstantiatedType* type;
    CHAOS_IL2CPP_UINT32 module_id;
};

CHAOS_IL2CPP_MUTEX s_runtime_types_mutex;
CHAOS_IL2CPP_VECTOR(RuntimeInstantiatedTypeRecord) s_runtime_types;

/* ── Helper: compute a short display name for a type argument handle.    ── */

static const char* GetTypeDisplayName(TypeInfoHandle handle) {
    if (handle == nullptr) {
        return "?";
    }
    const auto* desc = TryDecodeReflectionQueryTypeHandle(handle);
    if (desc != nullptr && desc->display_name_utf8 != nullptr) {
        return desc->display_name_utf8;
    }
    if (desc != nullptr && desc->name_utf8 != nullptr) {
        return desc->name_utf8;
    }
    return "?";
}

/* ── Helper: copy a C string into a heap-allocated buffer.              ── */

static char* StrDup(const char* src) {
    if (src == nullptr) return nullptr;
    CHAOS_IL2CPP_SIZE len = std::strlen(src);
    auto* buf = static_cast<char*>(std::malloc(len + 1));
    if (buf == nullptr) return nullptr;
    std::memcpy(buf, src, len + 1);
    return buf;
}

/* ── Bridge function implementations ── */

TypeInfoHandle CHAOS_RUNTIME_ABI_CALL ResolveOrInstantiateType(
    TypeInfoHandle         open_type_definition,
    const TypeInfoHandle*  type_args,
    CHAOS_IL2CPP_UINT32   arg_count)
{
    if (open_type_definition == 0) {
        return 0;
    }

    /* Fast path: registry lookup. */
    TypeInfoHandle closed = generic_context::TryResolveClosedType(
        open_type_definition, type_args, arg_count);
    if (closed != 0) {
        return closed;
    }

    /* Slow path: build a RuntimeInstantiatedType descriptor. */
    auto* rt_type = BuildClosedDescriptor(
        open_type_definition, type_args, arg_count);
    if (rt_type == nullptr) {
        return 0;
    }

    /* Compute value type layout if needed. */
    if (rt_type->descriptor.subject_id_utf8 != nullptr) {
        // Check the open type to determine if it's a value type.
        // For now, we assume value types can be identified by convention
        // (the open type descriptor's fields are available).
        ComputeValueTypeLayout(rt_type);
    }

    /* Encode the closed descriptor as a TypeInfoHandle. */
    TypeInfoHandle closed_handle = EncodeReflectionQueryTypeHandle(
        &rt_type->descriptor);
    if (closed_handle == 0) {
        std::free(rt_type);
        return 0;
    }

    /* Register in GenericContextRegistry so subsequent lookups hit the fast path. */
    generic_context::RegisterGenericInstantiation(
        open_type_definition, closed_handle, rt_type->type_args, rt_type->arg_count);
    rt_type->is_registered = true;

    return closed_handle;
}

MethodInfoHandle CHAOS_RUNTIME_ABI_CALL ResolveOrInstantiateMethod(
    MethodInfoHandle       open_method_definition,
    const TypeInfoHandle*  type_args,
    CHAOS_IL2CPP_UINT32   arg_count)
{
    /* Phase 5 adds full method instantiation support.
     * For now, return nullptr (caller should handle the uninstantiated case). */
    (void)open_method_definition;
    (void)type_args;
    (void)arg_count;
    return 0;
}

void CHAOS_RUNTIME_ABI_CALL UnregisterModuleGenerics(
    CHAOS_IL2CPP_UINT32 module_id)
{
    generic_context::UnregisterModuleGenerics(module_id);

    /* Also free RuntimeInstantiatedType records for this module. */
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(s_runtime_types_mutex);
    for (CHAOS_IL2CPP_SIZE i = 0u; i < s_runtime_types.size(); ) {
        if (s_runtime_types[i].module_id == module_id) {
            auto* rt = s_runtime_types[i].type;
            std::free(rt->descriptor.subject_id_utf8);
            std::free(rt->descriptor.definition_subject_id_utf8);
            std::free(rt->descriptor.namespace_name_utf8);
            std::free(rt->descriptor.name_utf8);
            std::free(rt->descriptor.display_name_utf8);
            std::free(rt->type_args);
            std::free(rt->field_offsets);
            std::free(rt);
            s_runtime_types.erase(s_runtime_types.begin() +
                static_cast<CHAOS_IL2CPP_SIZE>(i));
        } else {
            ++i;
        }
    }
}

/* Process-wide bridge instance. */
const RuntimeInstantiationBridgeV0 g_bridge = {
    .abi_version                        = CHAOS_RUNTIME_INSTANTIATION_BRIDGE_V0,
    .struct_size                        = sizeof(RuntimeInstantiationBridgeV0),
    .resolve_or_instantiate_type        = ResolveOrInstantiateType,
    .resolve_or_instantiate_method      = ResolveOrInstantiateMethod,
    .unregister_module_generics         = UnregisterModuleGenerics,
    .runtime_instantiation_count        = 0u,
    .interpreted_method_call_count      = 0u,
};

}  // anonymous namespace

// ════════════════════════════════════════════════════════════════════════════
// Public API
// ════════════════════════════════════════════════════════════════════════════

CHAOS_IL2CPP_UINT32 AllocateRuntimeToken() {
    return s_next_runtime_token.fetch_add(1u, std::memory_order_relaxed);
}

char* BuildClosedSubjectId(
    const ReflectionQueryTypeDescriptor* open_desc,
    const TypeInfoHandle*                type_args,
    CHAOS_IL2CPP_UINT32                  arg_count)
{
    if (open_desc == nullptr) {
        return 0;
    }

    const char* base = open_desc->subject_id_utf8;
    if (base == nullptr) {
        return nullptr;
    }

    // Build: "OpenType`N[Arg1,Arg2,...]"
    std::string result;
    result += base;
    result += '[';

    for (CHAOS_IL2CPP_UINT32 i = 0u; i < arg_count; ++i) {
        if (i > 0u) result += ',';
        result += GetTypeDisplayName(type_args[i]);
    }
    result += ']';

    auto* buf = static_cast<char*>(std::malloc(result.size() + 1));
    if (buf == nullptr) return nullptr;
    std::memcpy(buf, result.data(), result.size() + 1);
    return buf;
}

RuntimeInstantiatedType* BuildClosedDescriptor(
    TypeInfoHandle         open_type_definition,
    const TypeInfoHandle*  type_args,
    CHAOS_IL2CPP_UINT32   arg_count)
{
    if (open_type_definition == nullptr || type_args == nullptr || arg_count == 0u) {
        return 0;
    }

    const auto* open_desc = TryDecodeReflectionQueryTypeHandle(
        open_type_definition);
    if (open_desc == nullptr) {
        return 0;  // Not a reflection-query type; cannot instantiate.
    }

    // ── Allocate RuntimeInstantiatedType ──
    auto* rt_type = static_cast<RuntimeInstantiatedType*>(
        std::calloc(1u, sizeof(RuntimeInstantiatedType)));
    if (rt_type == nullptr) {
        return 0;
    }

    // ── Build closed subject_id ──
    char* subject_id = BuildClosedSubjectId(open_desc, type_args, arg_count);
    if (subject_id == nullptr) {
        std::free(rt_type);
        return 0;
    }
    rt_type->descriptor.subject_id_utf8 = subject_id;

    // ── Copy other descriptor fields from the open type ──
    rt_type->descriptor.metadata_token           = AllocateRuntimeToken();
    rt_type->descriptor.definition_subject_id_utf8 = StrDup(open_desc->subject_id_utf8);
    rt_type->descriptor.namespace_name_utf8       = StrDup(open_desc->namespace_name_utf8);

    // Name: extract the simple type name (after last '.' or '/')
    const char* base_subject = open_desc->subject_id_utf8;
    const char* name_start = base_subject;
    if (const char* p = std::strrchr(base_subject, '/')) name_start = p + 1;
    else if (const char* p = std::strrchr(base_subject, '.')) name_start = p + 1;
    rt_type->descriptor.name_utf8 = StrDup(name_start);

    // Display name
    {
        std::string display;
        display += name_start;
        display += '[';
        for (CHAOS_IL2CPP_UINT32 i = 0u; i < arg_count; ++i) {
            if (i > 0u) display += ',';
            display += GetTypeDisplayName(type_args[i]);
        }
        display += ']';
        auto* buf = static_cast<char*>(std::malloc(display.size() + 1));
        if (buf) {
            std::memcpy(buf, display.data(), display.size() + 1);
            rt_type->descriptor.display_name_utf8 = buf;
        }
    }

    // Point generic_type_definition to the open type.
    rt_type->descriptor.generic_type_definition = open_desc;

    // Reference the open type's fields and methods directly.
    // (Phase 5 may substitute type arguments in field/method descriptors.)
    rt_type->descriptor.fields         = open_desc->fields;
    rt_type->descriptor.field_count    = open_desc->field_count;
    rt_type->descriptor.properties     = open_desc->properties;
    rt_type->descriptor.property_count = open_desc->property_count;
    rt_type->descriptor.methods        = open_desc->methods;
    rt_type->descriptor.method_count   = open_desc->method_count;

    // ── Copy type_args ──
    auto* args_buf = static_cast<TypeInfoHandle*>(
        std::malloc(sizeof(TypeInfoHandle) * arg_count));
    if (args_buf == nullptr) {
        std::free(rt_type->descriptor.subject_id_utf8);
        std::free(rt_type);
        return 0;
    }
    std::memcpy(args_buf, type_args, sizeof(TypeInfoHandle) * arg_count);
    rt_type->type_args  = args_buf;
    rt_type->arg_count  = arg_count;
    rt_type->module_id  = 0u;  // AOT root by default

    // ── Track for cleanup ──
    {
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(s_runtime_types_mutex);
        s_runtime_types.push_back({rt_type, 0u});
    }

    return rt_type;
}

void ComputeValueTypeLayout(RuntimeInstantiatedType* rt_type) {
    if (rt_type == nullptr || rt_type->descriptor.generic_type_definition == nullptr) {
        return;
    }

    const auto* open_desc = rt_type->descriptor.generic_type_definition;

    // For now, compute layout only if the type has fields.
    // Full value type layout requires:
    //   1. Knowing the size of each field's type (depends on type_args substitution)
    //   2. Alignment computation
    //   3. Padding insertion
    //
    // Phase 3 milestone: store field_count so consumers can compute offsets.
    if (open_desc->field_count == 0u) {
        return;
    }

    rt_type->field_offset_count = open_desc->field_count;
    rt_type->field_offsets = static_cast<CHAOS_IL2CPP_UINT32*>(
        std::calloc(open_desc->field_count, sizeof(CHAOS_IL2CPP_UINT32)));
    if (rt_type->field_offsets == nullptr) {
        rt_type->field_offset_count = 0u;
        return;
    }

    // Simplified layout: lay out fields sequentially at CHAOS_IL2CPP_INTPTR alignment.
    // A proper implementation would resolve each field's type handle → size via
    // reflection query, apply alignment rules, and compute padding.
    CHAOS_IL2CPP_UINT32 offset = 0u;
    for (CHAOS_IL2CPP_UINT32 i = 0u; i < open_desc->field_count; ++i) {
        rt_type->field_offsets[i] = offset;
        offset += static_cast<CHAOS_IL2CPP_UINT32>(sizeof(CHAOS_IL2CPP_INTPTR));
    }
    rt_type->value_size = offset;
}

const RuntimeInstantiationBridgeV0* GetBridgeV0() {
    return &g_bridge;
}

}  // namespace chaos::il2cpp::runtime_instantiation

extern "C" CHAOS_RUNTIME_ABI_EXPORT const RuntimeInstantiationBridgeV0*
CHAOS_RUNTIME_ABI_CALL ChaosRuntimeInstantiationGetBridgeV0(void) {
    return chaos::il2cpp::runtime_instantiation::GetBridgeV0();
}
