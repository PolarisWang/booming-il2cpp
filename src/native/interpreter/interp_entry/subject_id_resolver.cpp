namespace chaos::il2cpp::runtime_core {

// Forward declaration for ParseSubjectIdForHotpatchLookup (defined in hotpatch_resolve.cpp)
static void ParseSubjectIdForHotpatchLookup(
    const char* subject_id,
    std::string& out_ns,
    std::string& out_type_name,
    std::string& out_method_name) noexcept;

// ── Subject ID resolver callback for DeserializeAotCoreIr ────────────────
// Maps subject IDs to call_target pointers using PatchMetadataCache + AOT reflection query.
// This replaces the old PatchTokenResolver which resolved raw metadata tokens.

static void* ResolveSubjectId(
    const char* subject_id,
    void* user_data) noexcept
{
    if (subject_id == nullptr || user_data == nullptr) return nullptr;

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
                if (type_desc == nullptr) continue;
                if (type_desc->subject_id_utf8 != nullptr &&
                    std::strcmp(type_desc->subject_id_utf8, subject_id) == 0) {
                    return reinterpret_cast<void*>(
                        static_cast<CHAOS_IL2CPP_UINTPTR>(
                            EncodeReflectionQueryTypeHandle(type_desc)));
                }
                // Search methods in this type.
                if (type_desc->methods != nullptr) {
                    for (CHAOS_IL2CPP_INT32 mi = 0; mi < type_desc->method_count; ++mi) {
                        const auto* method_desc = &type_desc->methods[mi];
                        if (method_desc->subject_id_utf8 != nullptr &&
                            std::strcmp(method_desc->subject_id_utf8, subject_id) == 0) {
                            return reinterpret_cast<void*>(
                                static_cast<CHAOS_IL2CPP_UINTPTR>(
                                    EncodeReflectionQueryMethodHandle(method_desc)));
                        }
                    }
                }
            }
        }
    }

    return nullptr;
}

}  // namespace chaos::il2cpp::runtime_core