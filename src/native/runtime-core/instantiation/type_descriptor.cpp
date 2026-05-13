namespace chaos::il2cpp::runtime_instantiation {

// ════════════════════════════════════════════════════════════════════════════
// Public API
// ════════════════════════════════════════════════════════════════════════════

CHAOS_IL2CPP_UINT32 AllocateRuntimeToken() {
    return s_next_runtime_token.fetch_add(1u, std::memory_order_relaxed);
}

char* BuildClosedSubjectId(
    const chaos::il2cpp::runtime_core::ReflectionQueryTypeDescriptor* open_desc,
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

    auto* buf = static_cast<char*>(CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(result.size() + 1));
    if (buf == nullptr) return nullptr;
    CHAOS_IL2CPP_MEMCPY(buf, result.data(), result.size() + 1);
    return buf;
}

RuntimeInstantiatedType* BuildClosedDescriptor(
    TypeInfoHandle         open_type_definition,
    const TypeInfoHandle*  type_args,
    CHAOS_IL2CPP_UINT32   arg_count)
{
    if (open_type_definition == 0 || type_args == nullptr || arg_count == 0u) {
        return 0;
    }

    const auto* open_desc = chaos::il2cpp::runtime_core::TryDecodeReflectionQueryTypeHandle(
        open_type_definition);
    if (open_desc == nullptr) {
        return 0;  // Not a reflection-query type; cannot instantiate.
    }

    // ── Allocate RuntimeInstantiatedType ──
    auto* rt_type = static_cast<RuntimeInstantiatedType*>(
        CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(sizeof(RuntimeInstantiatedType)));
    if (rt_type == nullptr) {
        return 0;
    }
    CHAOS_IL2CPP_MEMSET(rt_type, 0, sizeof(RuntimeInstantiatedType));

    // ── Build closed subject_id ──
    char* subject_id = BuildClosedSubjectId(open_desc, type_args, arg_count);
    if (subject_id == nullptr) {
        CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(rt_type);
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
        auto* buf = static_cast<char*>(CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(display.size() + 1));
        if (buf) {
            CHAOS_IL2CPP_MEMCPY(buf, display.data(), display.size() + 1);
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
        CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(sizeof(TypeInfoHandle) * arg_count));
    if (args_buf == nullptr) {
        CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(const_cast<char*>(rt_type->descriptor.subject_id_utf8));
        CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(rt_type);
        return 0;
    }
    CHAOS_IL2CPP_MEMCPY(args_buf, type_args, sizeof(TypeInfoHandle) * arg_count);
    rt_type->type_args  = args_buf;
    rt_type->arg_count  = arg_count;
    rt_type->module_id  = 0u;  // AOT root by default

    // ── Track for cleanup ──
    {
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(s_runtime_types_mutex);
        s_runtime_types.push_back({rt_type, 0u});
    }

    // ── Build vtable for the closed type (copies open type vtable) ──
    if (rt_type->descriptor.subject_id_utf8 != nullptr &&
        open_desc->subject_id_utf8 != nullptr) {
        CHAOS_IL2CPP_UINT64 closed_sid = chaos_compute_type_stable_id(rt_type->descriptor.subject_id_utf8);
        CHAOS_IL2CPP_UINT64 open_sid = chaos_compute_type_stable_id(open_desc->subject_id_utf8);
        runtime_vtable::BuildRuntimeVTable(closed_sid, open_sid);
    }

    return rt_type;
}

void ComputeValueTypeLayout(RuntimeInstantiatedType* rt_type) {
    if (rt_type == nullptr || rt_type->descriptor.generic_type_definition == nullptr) {
        return;
    }

    // Delegate layout computation to the LayoutEngine.
    TypeInfoHandle closed_handle = EncodeReflectionQueryTypeHandle(&rt_type->descriptor);
    if (closed_handle == 0u) {
        return;
    }

    auto* engine = layout::GetLayoutEngine();
    const auto* layout = engine->GetOrComputeLayout(
        closed_handle, rt_type->type_args, rt_type->arg_count);

    if (layout == nullptr) {
        return;
    }

    // Copy layout results into the RuntimeInstantiatedType record
    // (LayoutEngine owns the layout memory; we copy field offsets and
    // resolved type handles into our own storage).
    rt_type->value_size = layout->value_size;
    rt_type->field_offset_count = layout->field_count;

    if (layout->field_count > 0u && layout->fields != nullptr) {
        // ── Copy field offsets ──
        auto* offsets = static_cast<CHAOS_IL2CPP_UINT32*>(
            CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(sizeof(CHAOS_IL2CPP_UINT32) * layout->field_count));
        if (offsets != nullptr) {
            for (CHAOS_IL2CPP_UINT32 i = 0u; i < layout->field_count; ++i) {
                offsets[i] = layout->fields[i].offset;
            }
            rt_type->field_offsets = offsets;
        }

        // ── Copy resolved field types (optional cache) ──
        auto* resolved = static_cast<TypeInfoHandle*>(
            CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(sizeof(TypeInfoHandle) * layout->field_count));
        if (resolved != nullptr) {
            for (CHAOS_IL2CPP_UINT32 i = 0u; i < layout->field_count; ++i) {
                resolved[i] = layout->fields[i].resolved_type;
            }
            rt_type->resolved_field_types = resolved;
            rt_type->resolved_field_count = layout->field_count;
        }
    }
}

const RuntimeInstantiationBridgeV0* GetBridgeV0() {
    return &g_bridge;
}

}  // namespace