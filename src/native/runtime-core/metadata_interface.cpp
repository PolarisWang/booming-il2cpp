#include "metadata_interface.h"

#include <bootstrap/bootstrap.h>
#include "reflection_query_model.h"

#include <cstring>

namespace chaos::il2cpp::runtime_core {

// ── ModuleAwareResolveTypeInfo ───────────────────────────────────────────

const TypeInfoHot* ModuleAwareResolveTypeInfo(
    TypeInfoHandle handle, MetadataSource sources) noexcept
{
    if (handle == 0u) return nullptr;

    // ── Path 1: Try module-registry type_info_ptrs[] ──
    if (sources & kMetadataAOT) {
        uint32_t module_id = GetModuleId(handle);
        uint32_t token = GetTypeToken(handle);
        if (token != 0) {
            const auto* mod = LookupModule(module_id != 0u ? module_id : 0u);
            if (mod != nullptr && !mod->tombstone && mod->type_info_ptrs != nullptr) {
                uint32_t idx = TokenToIndex(token);
                if (idx < mod->type_count) {
                    return mod->type_info_ptrs[idx];
                }
            }
        }
    }

    // ── Path 2: Tag-encoded handles — try dynamic type registry ──
    if (sources & kMetadataDynamic) {
        const auto* desc = TryDecodeReflectionQueryTypeHandle(handle);
        if (desc != nullptr && desc->subject_id_utf8 != nullptr) {
            uint64_t stable_id = chaos_compute_type_stable_id(desc->subject_id_utf8);
            const auto* dyn_type = chaos_find_type_by_stable_id(stable_id);
            if (dyn_type != nullptr) {
                return dyn_type->AsTypeInfoHot();
            }
        }
    }

    // ── Path 3: CodegenBridgeV0 resolve_type_by_token — cold path ──
    if (sources & kMetadataAOT) {
        const auto* bridge = bootstrap::GetCodegenBridgeV0();
        if (bridge != nullptr && bridge->resolve_type_by_token != nullptr) {
            uint32_t module_id = GetModuleId(handle);
            uint32_t token = GetTypeToken(handle);
            if (token != 0) {
                const auto* mod = LookupModule(module_id != 0u ? module_id : 0u);
                if (mod != nullptr && mod->image != nullptr) {
                    TypeInfoHandle resolved = bridge->resolve_type_by_token(
                        reinterpret_cast<ImageHandle>(mod->image), token);
                    if (resolved != 0) {
                        // Recursive call with narrowed scope.
                        return ModuleAwareResolveTypeInfo(resolved, kMetadataAOT);
                    }
                }
            }
        }
    }

    return nullptr;
}

// ── ModuleAwareResolveMethod ─────────────────────────────────────────────

MethodInfoHandle ModuleAwareResolveMethod(
    ImageHandle image, uint32_t method_token,
    const PatchMetadataCache* patch_cache) noexcept
{
    if (method_token == 0) return 0;

    // 1. Hotpatch cache first (if available).
    if (patch_cache != nullptr) {
        // PatchMetadataCache doesn't expose method resolution as MethodInfoHandle —
        // it provides AotCoreIr JSON and MethodDef entries.  For resolved call_target,
        // the subject_id resolver (LookupCallTarget) is used instead.
        // Return 0 here so callers fall through to the AOT bridge.
        (void)patch_cache;
    }

    // 2. AOT bridge resolve_method_by_token.
    const auto* bridge = bootstrap::GetCodegenBridgeV0();
    if (bridge != nullptr && bridge->resolve_method_by_token != nullptr && image != 0) {
        return bridge->resolve_method_by_token(image, method_token);
    }

    return 0;
}

// ── ModuleAwareResolveField ──────────────────────────────────────────────

FieldInfoHandle ModuleAwareResolveField(
    ImageHandle image, uint32_t field_token) noexcept
{
    if (field_token == 0) return 0;

    const auto* bridge = bootstrap::GetCodegenBridgeV0();
    if (bridge != nullptr && bridge->resolve_field_by_token != nullptr && image != 0) {
        return bridge->resolve_field_by_token(image, field_token);
    }

    return 0;
}

// ── ModuleAwareResolveToken ──────────────────────────────────────────────

uint32_t ModuleAwareResolveToken(
    uint32_t token, ImageHandle image,
    const PatchMetadataCache* patch_cache) noexcept
{
    if (token == 0) return 0;

    uint8_t table = static_cast<uint8_t>(token >> 24);

    // TypeDef / TypeRef / Method / Field / MemberRef
    switch (table) {
        case 0x02: // TypeDef
        case 0x01: // TypeRef
        case 0x06: // Method
        case 0x04: // Field
        case 0x0A: // MemberRef
            // For now, return 0 — token resolution is handled by DeserializeAotCoreIr
            // which carries subject_id references from the managed side.  Direct token
            // resolution is only needed for generic instantiation tables (handled in
            // RegisterModuleGenerics via CodegenBridgeV0 callbacks).
            (void)image;
            (void)patch_cache;
            return 0;
        default:
            return 0;
    }
}

// ── LookupCallTarget ─────────────────────────────────────────────────────

MethodInfoHandle LookupCallTarget(
    const char* subject_id_utf8,
    const CodegenBridgeV0* bridge,
    ImageHandle aot_image,
    const PatchMetadataCache* patch_cache) noexcept
{
    if (subject_id_utf8 == nullptr || subject_id_utf8[0] == '\0') return 0;

    // 1. Try AOT reflection query model (search type descriptors by subjectId).
    if (bridge != nullptr && aot_image != 0) {
        const auto* image = TryDecodeReflectionQueryImageHandle(aot_image);
        if (image != nullptr) {
            for (uint32_t ti = 0; ti < image->type_count; ++ti) {
                const auto* type_desc = image->types[ti];
                if (type_desc == nullptr) continue;

                // Check type subject_id.
                if (type_desc->subject_id_utf8 != nullptr &&
                    std::strcmp(type_desc->subject_id_utf8, subject_id_utf8) == 0) {
                    return static_cast<MethodInfoHandle>(
                        static_cast<uintptr_t>(
                            EncodeReflectionQueryTypeHandle(type_desc)));
                }

                // Check methods in this type.
                if (type_desc->methods != nullptr) {
                    for (int32_t mi = 0; mi < type_desc->method_count; ++mi) {
                        const auto* method_desc = &type_desc->methods[mi];
                        if (method_desc->subject_id_utf8 != nullptr &&
                            std::strcmp(method_desc->subject_id_utf8, subject_id_utf8) == 0) {
                            return static_cast<MethodInfoHandle>(
                                static_cast<uintptr_t>(
                                    EncodeReflectionQueryMethodHandle(method_desc)));
                        }
                    }
                }
            }
        }
    }

    // 2. Try HotpatchNameRegistry for name-based lookup.
    // Parse "Assembly/Namespace.TypeName:MethodName" format.
    if (patch_cache != nullptr) {
        // PatchMetadataCache has its own subject_id resolver via
        // cache lookup of AotCoreIr.  The patch method's call_target
        // is resolved during PatchMethodLowerIR by the ResolveSubjectId
        // callback, which uses the AOT bridge + image from SetAotBridge.
        (void)patch_cache;
    }

    return 0;
}

// ── ModuleLifecycleManager ───────────────────────────────────────────────

ModuleLifecycleManager* ModuleLifecycleManager::Get() noexcept {
    static ModuleLifecycleManager instance;
    return &instance;
}

uint32_t ModuleLifecycleManager::RegisterModule(
    const char* name,
    const ModuleDescriptor* module_desc,
    const HotpatchModuleV0* hotpatch_module,
    const ModuleGenericRegistrationV0* generics_reg) noexcept
{
    if (name == nullptr || module_desc == nullptr) {
        return kInvalidModuleId;
    }

    // 1. Register in ModuleRegistry (gets a module_id).
    uint32_t module_id = runtime_core::RegisterModule(name, module_desc);
    if (module_id == kInvalidModuleId) {
        return kInvalidModuleId;
    }

    // 2. Register hotpatch dispatch table data (if provided).
    if (hotpatch_module != nullptr) {
        auto& registry = GetHotpatchNameRegistry();
        registry.RegisterModule(hotpatch_module);
    }

    // 3. Register generic instantiations (if provided).
    if (generics_reg != nullptr) {
        generic_context::RegisterModuleGenerics(generics_reg);
    }

    return module_id;
}

void ModuleLifecycleManager::UnregisterModule(uint32_t module_id) noexcept {
    if (module_id == kInvalidModuleId || module_id == 0u) return;

    // 1. Unregister generic instantiations.
    generic_context::UnregisterModuleGenerics(module_id);

    // 2. Mark module as tombstone in ModuleRegistry.
    runtime_core::MarkModuleTombstone(module_id);

    // 3. Evict ICustomMarshaler cache — stale marshaler instances from the
    //    unloaded module will be re-resolved on next P/Invoke.
    runtime_core::ClearMarshalerCache();

    // Note: HotpatchNameRegistry entries are not removed (they use
    // .rodata pointers from codegen data sections which persist
    // for the lifetime of the process).  Module ID recycling via
    // tombstone handles name collisions on re-registration.
}

bool ModuleLifecycleManager::RegisterHotpatchContext(PatchContext* ctx) noexcept {
    if (ctx == nullptr || ctx->method_count == 0) return false;

    // PatchContext methods are already registered via SetPatchedBySlot
    // during ApplyPatchFromMemory.  This hook exists for post-processing
    // such as inlining-map population and call-count tracking.
    return true;
}

bool ModuleLifecycleManager::UnregisterHotpatchContext(PatchContext* ctx) noexcept {
    if (ctx == nullptr) return false;

    // Delegates to Unpatch() which clears kHotpatchActive flags.
    return Unpatch(ctx);
}

}  // namespace chaos::il2cpp::runtime_core