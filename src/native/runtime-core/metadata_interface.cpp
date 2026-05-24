#include "metadata_interface.h"

#include <bootstrap/bootstrap.h>
#include "reflection_query_model.h"

#include <unified_metadata.h>

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

    // Attempt resolution through the AOT bridge for all table types.
    // If the bridge is unavailable or returns 0, fall back to the raw
    // token as an opaque handle (callers may resolve lazily).
    const auto* bridge = bootstrap::GetCodegenBridgeV0();
    if (bridge == nullptr) return token;

    switch (table) {
        case 0x02: // TypeDef
        case 0x01: // TypeRef
            if (bridge->resolve_type_by_token != nullptr && image != 0) {
                TypeInfoHandle th = bridge->resolve_type_by_token(image, token);
                if (th != 0) return static_cast<uint32_t>(th);
            }
            return token; // fallback: opaque handle

        case 0x06: // MethodDef
            if (bridge->resolve_method_by_token != nullptr && image != 0) {
                MethodInfoHandle mh = bridge->resolve_method_by_token(image, token);
                if (mh != 0) return static_cast<uint32_t>(mh);
            }
            // Try hotpatch cache fallback.
            if (patch_cache != nullptr) {
                // PatchMetadataCache may provide ResolveToken for method tokens
                // via its own internal tables; return token as opaque handle.
                return token;
            }
            return token;

        case 0x04: // FieldDef
            if (bridge->resolve_field_by_token != nullptr && image != 0) {
                FieldInfoHandle fh = bridge->resolve_field_by_token(image, token);
                if (fh != 0) return static_cast<uint32_t>(fh);
            }
            return token;

        case 0x0A: // MemberRef
            // MemberRef needs parent token resolution.  For now, return
            // the raw token — callers should have resolved parent first.
            return token;

        default:
            return token;
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

// ── MetadataRegistry ─────────────────────────────────────────────────────

MetadataRegistry& MetadataRegistry::Get() noexcept {
    static MetadataRegistry instance;
    return instance;
}

const TypeInfoHot* MetadataRegistry::ResolveType(
    uint32_t module_id, uint32_t type_token) noexcept
{
    if (type_token == 0) return nullptr;

    // Check cache first (once populated, never invalidated).
    uint64_t cache_key = (static_cast<uint64_t>(module_id) << 32) | type_token;
    if (cache_built_.load(std::memory_order_acquire)) {
        auto it = resolve_cache_.find(cache_key);
        if (it != resolve_cache_.end()) {
            return static_cast<const TypeInfoHot*>(it->second);
        }
    }

    // Build composite handle and resolve.
    TypeInfoHandle handle = MakeTypeHandle(module_id, type_token);
    const auto* result = ModuleAwareResolveTypeInfo(handle, kMetadataAny);

    // Populate cache (lazy, if not yet bulk-built).
    if (result != nullptr && !cache_built_.load(std::memory_order_relaxed)) {
        resolve_cache_.try_emplace(cache_key, const_cast<TypeInfoHot*>(result));
    }

    return result;
}

const char* MetadataRegistry::GetTypeName(
    uint64_t type_handle, const char** out_namespace) noexcept
{
    if (type_handle == 0) return nullptr;

    // Path 1: Handle encodes a composite key (module_id<<32 | token).
    uint32_t module_id = static_cast<uint32_t>(type_handle >> 32);
    uint32_t token = static_cast<uint32_t>(type_handle & 0xFFFFFFFFu);

    if (module_id < kMaxModules && token != 0) {
        const auto* mod = LookupModule(module_id);
        if (mod != nullptr && !mod->tombstone) {
            uint32_t idx = TokenToIndex(token);
            if (idx < mod->type_count) {
                if (out_namespace != nullptr && mod->type_namespaces != nullptr) {
                    *out_namespace = mod->type_namespaces[idx];
                }
                if (mod->type_names != nullptr) {
                    return mod->type_names[idx];
                }
            }
        }
    }

    // Path 2: Handle is a direct TypeInfoHot* pointer (from C API route).
    const auto* type_info = reinterpret_cast<const TypeInfoHot*>(type_handle);
    return LookupTypeNameByInfoPtr(type_info, out_namespace);
}

MethodInfoHandle MetadataRegistry::ResolveMethod(
    uint32_t module_id, uint32_t method_token) noexcept
{
    if (method_token == 0) return 0;

    // Resolve through AOT bridge with module image context.
    const auto* mod = LookupModule(module_id);
    if (mod == nullptr || mod->tombstone || mod->image == nullptr) return 0;

    ImageHandle image = EncodeReflectionQueryImageHandle(mod->image);
    return ModuleAwareResolveMethod(image, method_token);
}

const char* MetadataRegistry::GetMethodName(uint64_t method_handle) noexcept
{
    if (method_handle == 0) return nullptr;

    // MethodHandle composite: (module_id<<32) | method_token.
    uint32_t module_id = static_cast<uint32_t>(method_handle >> 32);
    uint32_t method_token = static_cast<uint32_t>(method_handle & 0xFFFFFFFFu);

    // Path 1: HotpatchNameRegistry — scan method_entries for matching token.
    auto& hp_registry = GetHotpatchNameRegistry();
    if (module_id < hp_registry.ModuleCount()) {
        const char* name = hp_registry.GetMethodName(module_id, method_token);
        if (name != nullptr) return name;
    }

    // Path 2: AOT module — scan ReflectionQueryModel method descriptors.
    const auto* mod = LookupModule(module_id);
    if (mod != nullptr && !mod->tombstone && mod->image != nullptr) {
        ImageHandle image = EncodeReflectionQueryImageHandle(mod->image);
        const auto* img = TryDecodeReflectionQueryImageHandle(image);
        if (img != nullptr) {
            const auto* md = FindReflectionQueryMethodByToken(img, method_token);
            if (md != nullptr && md->name_utf8 != nullptr) {
                return md->name_utf8;
            }
        }
    }

    return nullptr;
}

FieldInfoHandle MetadataRegistry::ResolveField(
    uint32_t module_id, uint32_t field_token) noexcept
{
    if (field_token == 0) return 0;

    const auto* mod = LookupModule(module_id);
    if (mod == nullptr || mod->tombstone) return 0;

    ImageHandle image = mod->image != nullptr
        ? EncodeReflectionQueryImageHandle(mod->image) : 0;
    return ModuleAwareResolveField(image, field_token);
}

uint64_t MetadataRegistry::FindToken(uint32_t method_token) noexcept
{
    if (method_token == 0) return 0;

    // 1. Try HotpatchNameRegistry first (covers both AOT and hotpatch).
    uint64_t result = GetHotpatchNameRegistry().FindToken(method_token);
    if (result != 0) return result;

    // 2. Scan ModuleRegistry for AOT-only modules not in HotpatchNameRegistry.
    uint32_t count = GetModuleCount();
    for (uint32_t mi = 0; mi < count; ++mi) {
        const auto* mod = GetModuleByIndex(mi);
        if (mod == nullptr || mod->tombstone) continue;
        // Module has types — check if any type contains this method token.
        // For now, AOT-only methods are found via HotpatchNameRegistry.
        // Full AOT method table scan would require a method token index.
        (void)mod;
    }

    return 0;
}

uint32_t MetadataRegistry::EnumerateMethods(
    uint32_t module_id, void** out_buf, uint32_t max) noexcept
{
    if (out_buf == nullptr || max == 0) return 0;

    uint32_t written = 0;

    // Enumerate from HotpatchNameRegistry if module has dispatch entries.
    auto& registry = GetHotpatchNameRegistry();
    if (module_id < registry.ModuleCount()) {
        for (uint32_t si = 0; si < UINT16_MAX && written < max; ++si) {
            auto* entry = registry.GetDispatchEntryBySlot(module_id, si);
            if (entry == nullptr) break;
            uint64_t mh = (static_cast<uint64_t>(module_id) << 32) | si;
            out_buf[written++] = reinterpret_cast<void*>(mh);
        }
    }

    // Fallback: enumerate AOT types and their methods from ModuleRegistry.
    if (written == 0) {
        const auto* mod = LookupModule(module_id);
        if (mod != nullptr && !mod->tombstone && mod->type_info_ptrs != nullptr) {
            for (uint32_t ti = 0; ti < mod->type_count && written < max; ++ti) {
                const auto* type_info = mod->type_info_ptrs[ti];
                if (type_info == nullptr) continue;
                // Each type contributes a placeholder — full method table
                // enumeration requires a method-per-type index.
                uint64_t mh = (static_cast<uint64_t>(module_id) << 32) | (0x02000000u | (ti + 1));
                out_buf[written++] = reinterpret_cast<void*>(mh);
            }
        }
    }

    return written;
}

// ── Unified registry V0 interface (C-callable wrappers) ────────────────

namespace {

TypeHandle UMR_ResolveType(uint32_t module_id, uint32_t type_token) {
    const auto* ti = MetadataRegistry::Get().ResolveType(module_id, type_token);
    return reinterpret_cast<TypeHandle>(ti);
}

const char* UMR_GetTypeName(TypeHandle type, const char** out_namespace) {
    return MetadataRegistry::Get().GetTypeName(type, out_namespace);
}

MethodHandle UMR_ResolveMethod(uint32_t module_id, uint32_t method_token) {
    return static_cast<MethodHandle>(
        MetadataRegistry::Get().ResolveMethod(module_id, method_token));
}

const char* UMR_GetMethodName(MethodHandle method) {
    return MetadataRegistry::Get().GetMethodName(method);
}

FieldHandle UMR_ResolveField(uint32_t module_id, uint32_t field_token) {
    return static_cast<FieldHandle>(
        MetadataRegistry::Get().ResolveField(module_id, field_token));
}

uint64_t UMR_FindToken(uint32_t method_token) {
    return MetadataRegistry::Get().FindToken(method_token);
}

uint32_t UMR_EnumerateMethods(uint32_t module_id, void** out_buf, uint32_t max) {
    return MetadataRegistry::Get().EnumerateMethods(module_id, out_buf, max);
}

}  // anonymous namespace

const UnifiedMetadataRegistryV0* MetadataRegistry::GetUnifiedRegistry() noexcept {
    static const UnifiedMetadataRegistryV0 s_registry = {
        0u,                                         // abi_version
        sizeof(UnifiedMetadataRegistryV0),          // struct_size
        UMR_ResolveType,
        UMR_GetTypeName,
        UMR_ResolveMethod,
        UMR_GetMethodName,
        UMR_ResolveField,
        UMR_FindToken,
        UMR_EnumerateMethods,
    };
    return &s_registry;
}

void MetadataRegistry::BuildCache() noexcept {
    if (cache_built_.load(std::memory_order_relaxed)) return;

    uint32_t count = GetModuleCount();
    for (uint32_t mi = 0; mi < count; ++mi) {
        PopulateModuleCache(mi);
    }

    // Also walk hotpatch modules.
    auto& registry = GetHotpatchNameRegistry();
    uint32_t hp_count = static_cast<uint32_t>(registry.ModuleCount());
    for (uint32_t mi = 0; mi < hp_count; ++mi) {
        for (uint32_t si = 0; si < UINT16_MAX; ++si) {
            auto* entry = registry.GetDispatchEntryBySlot(mi, si);
            if (entry == nullptr) break;
            uint64_t composite = (static_cast<uint64_t>(mi) << 32) | si;
            resolve_cache_.try_emplace(composite, entry->direct_ptr);
        }
    }

    cache_built_.store(true, std::memory_order_release);
}

void MetadataRegistry::PopulateModuleCache(uint32_t module_id) noexcept {
    const auto* mod = LookupModule(module_id);
    if (mod == nullptr || mod->tombstone) return;

    // Cache type_info_ptrs by token.
    if (mod->type_info_ptrs != nullptr) {
        for (uint32_t ti = 0; ti < mod->type_count; ++ti) {
            if (mod->type_info_ptrs[ti] == nullptr) continue;
            // TypeDef token encoding: 0x02000000 | (ti + 1)
            uint32_t token = 0x02000000u | (ti + 1);
            uint64_t key = (static_cast<uint64_t>(module_id) << 32) | token;
            resolve_cache_.try_emplace(key,
                const_cast<TypeInfoHot*>(mod->type_info_ptrs[ti]));
        }
    }
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

    // Activate each patched method's dispatch entry by calling SetPatchedBySlot.
    // This sets the kHotpatchActive flag so future calls go through the
    // interpreter rather than executing the AOT code directly.
    auto& registry = GetHotpatchNameRegistry();
    for (uint32_t i = 0; i < ctx->method_count; ++i) {
        const auto& method = ctx->methods[i];
        // Resolve the dispatch slot for this method's (module_id, token).
        // The slot is discovered during ApplyPatchFromMemory and stored
        // in the PatchMethod's token field.
        HotpatchEntryV0* entry = registry.GetDispatchEntry(method.module_id, method.token);
        if (entry == nullptr) continue;

        // Compute the slot index from the dispatch entry.
        // Walk back: entry - module->entry_table = slot.
        const HotpatchModuleV0* mod = nullptr;
        // We need the module to compute the slot.  GetDispatchEntry already
        // found the entry; the slot is stored in the PatchMethod metadata.
        // SetPatchedBySlot handles the release fence internally.
        // Since we don't have the slot index at this level, use token-based lookup.
        // ApplyPatchFromMemory already computed the slot — reuse the module-scoped path.
        //
        // For each method, we know its module_id and token.  TokenToSlot()
        // maps token → slot for hotpatch dispatch entries.  Use it directly.
        uint32_t slot = registry.TokenToSlot(method.module_id, method.token);
        if (slot == UINT32_MAX) continue;

        registry.SetPatchedBySlot(method.module_id, slot, true,
                                  const_cast<PatchMethod*>(&method));
    }

    // Record the patched method count for diagnostics.
    CHAOS_IL2CPP_LOG_DEBUG_M("MetadataRegistry", "activated {0} methods", ctx->method_count);
    return true;
}

bool ModuleLifecycleManager::UnregisterHotpatchContext(PatchContext* ctx) noexcept {
    if (ctx == nullptr) return false;

    // Delegates to Unpatch() which clears kHotpatchActive flags.
    return Unpatch(ctx);
}

void ModuleLifecycleManager::RegisterAotModuleData(
    const HotpatchModuleV0* hotpatch_module,
    const ModuleGenericRegistrationV0* generics_reg) noexcept
{
    // Attach hotpatch dispatch table to the AOT root (module_id=0).
    if (hotpatch_module != nullptr) {
        GetHotpatchNameRegistry().RegisterModule(hotpatch_module);
    }

    // Register AOT generic instantiations.
    if (generics_reg != nullptr) {
        generic_context::RegisterModuleGenerics(generics_reg);
    }
}

void ModuleLifecycleManager::RegisterHotUpdateGenerics(
    uint32_t module_id,
    const ModuleGenericRegistrationV0* generics_reg) noexcept
{
    if (generics_reg == nullptr) return;

    // Route through generic context, the same way RegisterAotModuleData
    // does for AOT root.  The module_id must match HotUpdatePackageHandle::module_id.
    generic_context::RegisterModuleGenerics(generics_reg);
}

void ModuleLifecycleManager::UnregisterHotUpdateGenerics(
    uint32_t module_id) noexcept
{
    if (module_id == 0 || module_id == kInvalidModuleId) return;

    generic_context::UnregisterModuleGenerics(module_id);
}

}  // namespace chaos::il2cpp::runtime_core