namespace chaos::il2cpp::runtime_core {

// Forward declaration for ResolveDirectFnSafe (defined in aot_direct_dispatch.cpp)
// Tier 1 + Tier 2 only — excludes kChaosExternalRuntimeFnTable entries whose
// calling conventions are incompatible with MethodInvoke.
void* ResolveDirectFnSafe(const char* subject_id) noexcept;

// ── Subject ID resolver callback for DeserializeAotCoreIr ────────────────
// Maps subject IDs to call_target pointers using PatchMetadataCache + AOT reflection query.
// This replaces the old PatchTokenResolver which resolved raw metadata tokens.

static void* ResolveSubjectId(const char* subject_id, void* user_data) noexcept {
    if (subject_id == nullptr || user_data == nullptr)
        return nullptr;

    auto* cache = static_cast<PatchMetadataCache*>(user_data);
    const auto* bridge = cache->GetBridge();
    ImageHandle aot_image = cache->GetAotImage();

    // Try to resolve through the AOT reflection query model.
    // Subject IDs follow the format: "AssemblyName/Namespace.TypeName:MethodName"
    // or just "SubjectId" for types.
    if (bridge != nullptr && aot_image != 0) {
        const auto* image = TryDecodeReflectionQueryImageHandle(aot_image);
        if (image != nullptr) {
            // Search types by subject_id.
            for (CHAOS_IL2CPP_UINT32 ti = 0; ti < image->type_count; ++ti) {
                const auto* type_desc = image->types[ti];
                if (type_desc == nullptr)
                    continue;
                if (type_desc->subject_id_utf8 != nullptr && std::strcmp(type_desc->subject_id_utf8, subject_id) == 0) {
                    return reinterpret_cast<void*>(
                        static_cast<CHAOS_IL2CPP_UINTPTR>(EncodeReflectionQueryTypeHandle(type_desc)));
                }
                // Search methods in this type.
                if (type_desc->methods != nullptr) {
                    for (CHAOS_IL2CPP_INT32 mi = 0; mi < type_desc->method_count; ++mi) {
                        const auto* method_desc = &type_desc->methods[mi];
                        if (method_desc->subject_id_utf8 != nullptr &&
                            std::strcmp(method_desc->subject_id_utf8, subject_id) == 0) {
                            // Return the encoded reflection query method handle
                            // (descriptor | tag bit 0x8000000000000000) so that:
                            //
                            //   1. PrecacheCallTarget (PatchMethodLowerIR) can
                            //      decode the handle → get method_desc → parse
                            //      subject_id → look up dispatch entry's
                            //      direct_ptr in HotpatchNameRegistry → fill
                            //      cache_info->direct_ptr → Handle_Call takes
                            //      the DoMIC path with the correct calling
                            //      convention fn(a0...a7) where args are raw
                            //      managed values.
                            //
                            //   2. If HotpatchNameRegistry misses (hotpatched
                            //      method, no dispatch entry), the cache stays
                            //      empty → Handle_Call falls through to DoRaw
                            //      → MethodInvoke.  MethodInvoke extracts
                            //      metadata_token=0 → FindInvokerPointer(0)
                            //      returns nullptr → returns NOT_FOUND
                            //      gracefully (no wrong-convention call).
                            //
                            // Why NOT return entry->direct_ptr directly:
                            //   Without the tag bit, PrecacheCallTarget can't
                            //   decode the handle, cache stays empty, and
                            //   Handle_Call falls to DoRaw → MethodInvoke.
                            //   MethodInvoke's raw pointer path calls the AOT
                            //   thunk as RawMethodInvokerFn(invoker(rs, ts,
                            //   this, argv, argc)), but AOT thunks expect
                            //   managed args directly (fn(a0...a7)). Passing
                            //   runtime_state/thread_state as managed args
                            //   corrupts the call.
                            return reinterpret_cast<void*>(
                                static_cast<CHAOS_IL2CPP_UINTPTR>(EncodeReflectionQueryMethodHandle(method_desc)));
                        }
                    }
                }
            }
        }
    }

    // Fallback: try to resolve cross-assembly calls using Tier 1 + Tier 2
    // (AotDirectDispatch + HotpatchNameRegistry), but NOT Tier 3
    // (kChaosExternalRuntimeFnTable).  Tier 3 stubs have managed calling
    // conventions incompatible with MethodInvoke.  Returns nullptr when
    // unresolvable → Handle_Call treats null call_target as a controlled
    // exception rather than a wrong-convention crash.
    //
    // Cross-assembly methods should primarily reach the JIT path through
    // instr.direct_fn (set during DeserializeAotCoreIr from the JSON), which
    // uses the correct fn(a0...a7) calling convention.
    return ResolveDirectFnSafe(subject_id);
}

} // namespace chaos::il2cpp::runtime_core