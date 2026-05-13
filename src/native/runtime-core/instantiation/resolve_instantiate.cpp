namespace chaos::il2cpp::runtime_instantiation {

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
        CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(rt_type);
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
    if (open_method_definition == 0u) {
        return 0u;
    }

    /* Fast Path 1: AOT map query (generic method promotion).
     * Check for a pre-compiled AOT instance before falling through to the
     * GenericContextRegistry or building a new RuntimeInstantiatedMethod. */
    {
        CHAOS_IL2CPP_UINT32 open_token = 0u;
        CHAOS_IL2CPP_UINT32 module_id  = static_cast<CHAOS_IL2CPP_UINT32>(
            open_method_definition >> 32);

        const auto* open_desc =
            runtime_core::TryDecodeReflectionQueryMethodHandle(
                open_method_definition);
        if (open_desc != nullptr) {
            open_token = open_desc->metadata_token;
        }
        if (open_token != 0u) {
            const MethodInfoHandle aot_method = QueryAotMethod(
                module_id, open_token, type_args, arg_count);
            if (aot_method != 0u) return aot_method;
        }
    }

    /* Fast Path 2: GenericContextRegistry lookup. */
    MethodInfoHandle closed = generic_context::TryResolveClosedMethod(
        open_method_definition, type_args, arg_count);
    if (closed != 0u) {
        return closed;
    }

    /* Slow path: build a RuntimeInstantiatedMethod descriptor. */
    auto* rt_method = CreateClosedMethodDescriptor(
        open_method_definition, type_args, arg_count);
    if (rt_method == nullptr) {
        return 0u;
    }

    /* Populate token resolution context fields. */
    rt_method->bridge        = chaos::il2cpp::bootstrap::GetCodegenBridgeV0();
    rt_method->source_image  = 0u;  // set from module registration if available
    rt_method->layout_engine = layout::GetLayoutEngine();

    /* Encode the closed descriptor as a MethodInfoHandle. */
    MethodInfoHandle closed_handle = chaos::il2cpp::runtime_core::EncodeReflectionQueryMethodHandle(
        &rt_method->descriptor);
    if (closed_handle == 0u) {
        CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(const_cast<char*>(rt_method->descriptor.subject_id_utf8));
        CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(const_cast<char*>(rt_method->descriptor.name_utf8));
        CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(const_cast<char*>(rt_method->descriptor.member_type_utf8));
        CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(rt_method->type_args);
        CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(rt_method);
        return 0u;
    }

    /* Register in GenericContextRegistry so subsequent lookups hit the fast path. */
    generic_context::RegisterGenericMethodInstantiation(
        open_method_definition, closed_handle, rt_method->type_args, rt_method->arg_count);
    rt_method->is_registered = true;

    /* Track for module unload cleanup. */
    {
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(s_runtime_methods_mutex);
        s_runtime_methods.push_back({rt_method, rt_method->module_id});
    }

    return closed_handle;
}

void CHAOS_RUNTIME_ABI_CALL UnregisterModuleGenerics(
    CHAOS_IL2CPP_UINT32 module_id)
{
    generic_context::UnregisterModuleGenerics(module_id);

    /* Also free RuntimeInstantiatedType records for this module. */
    {
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(s_runtime_types_mutex);
        for (CHAOS_IL2CPP_SIZE i = 0u; i < s_runtime_types.size(); ) {
            if (s_runtime_types[i].module_id == module_id) {
                auto* rt = s_runtime_types[i].type;
                CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(const_cast<char*>(rt->descriptor.subject_id_utf8));
                CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(const_cast<char*>(rt->descriptor.definition_subject_id_utf8));
                CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(const_cast<char*>(rt->descriptor.namespace_name_utf8));
                CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(const_cast<char*>(rt->descriptor.name_utf8));
                CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(const_cast<char*>(rt->descriptor.display_name_utf8));
                CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(rt->type_args);
                CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(rt->field_offsets);
                CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(rt->resolved_field_types);
                CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(rt);
                s_runtime_types.erase(s_runtime_types.begin() +
                    static_cast<CHAOS_IL2CPP_SIZE>(i));
            } else {
                ++i;
            }
        }
    }

    /* Mark RuntimeInstantiatedMethod records as unloaded for this module.
     * We do NOT free the methods themselves because stale MethodInfoHandle
     * values may still be in use; is_unloaded blocks re-entry. */
    {
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(s_runtime_methods_mutex);
        for (CHAOS_IL2CPP_SIZE i = 0u; i < s_runtime_methods.size(); ) {
            if (s_runtime_methods[i].module_id == module_id) {
                s_runtime_methods[i].method->is_unloaded = true;
                s_runtime_methods.erase(s_runtime_methods.begin() +
                    static_cast<CHAOS_IL2CPP_SIZE>(i));
            } else {
                ++i;
            }
        }
    }

    /* Mark the module as tombstone in the Module Registry. */
    runtime_core::MarkModuleTombstone(module_id);
}

}  // namespace