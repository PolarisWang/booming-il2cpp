// metadata_interface.h — Unified metadata resolution for AOT + HotUpdate paths
//
// Eliminates the 5-way architecture split between AOT and HotUpdate metadata:
//   1. TypeInfo* resolution:   module_registry type_info_ptrs[] ↔ chaos_find_type_by_stable_id()
//   2. Token space:            constexpr tables         ↔ PatchMetadataCache local tables
//   3. Method metadata:        CodegenBridgeV0          ↔ PatchMetadataCache::ResolveToken
//   4. Module lifecycle:       g_module_storage[1024]   ↔ ApplyPatchFromMemory allocation
//   5. Subject ID resolution:  CodegenBridgeV0 callbacks ↔ PatchMetadataCache + external_runtime
//
// Design principle: callers use ModuleAwareXxx() which dispatches internally based
// on module_id type (AOT module vs hotpatch module vs dynamic).

#ifndef CHAOS_IL2CPP_METADATA_INTERFACE_H_
#define CHAOS_IL2CPP_METADATA_INTERFACE_H_

#include "hotpatch_table.h"
#include "module_registry.h"
#include "patch_loader.h"
#include "type_registry.h"
#include "generic_context.h"
#include "reflection_query_model.h"

#include <cstdint>

namespace chaos::il2cpp::runtime_core {

// ── Module classification ─────────────────────────────────────────────
// metadata_type masks for the priority chain lookup.
enum MetadataSource : uint8_t {
    kMetadataAOT       = 0x01,  // CodegenBridgeV0 / module_registry
    kMetadataHotPatch  = 0x02,  // PatchMetadataCache
    kMetadataDynamic   = 0x04,  // chaos_register_type (dynamic type registry)
    kMetadataAny       = 0xFF,  // all sources
};

// ── ModuleAwareResolveTypeInfo ────────────────────────────────────────
// Resolve TypeInfoHandle → TypeInfoHot* using the best available source.
// Priority chain:
//   1. AOT module type_info_ptrs[] (if handle encodes a valid module_id + token)
//   2. Dynamic type registry by stable_id (if handle is a tag-encoded reflection query)
//   3. Fallback via CodegenBridgeV0 resolve_type_by_token (cold path)
//
// Returns nullptr when no source can resolve the handle.
const TypeInfoHot* ModuleAwareResolveTypeInfo(
    TypeInfoHandle handle,
    MetadataSource sources = kMetadataAny) noexcept;

// ── ModuleAwareResolveMethod ──────────────────────────────────────────
// Resolve a method token (0x06xxxxxx) within the given context (module_id or image)
// to a MethodInfoHandle.
//
// Priority chain:
//   1. Hotpatch PatchMetadataCache (if context provides a PatchMetadataCache*)
//   2. CodegenBridgeV0 resolve_method_by_token (if image is available)
//
// Returns 0 when no source can resolve the handle.
MethodInfoHandle ModuleAwareResolveMethod(
    ImageHandle image,
    uint32_t method_token,
    const PatchMetadataCache* patch_cache = nullptr) noexcept;

// ── ModuleAwareResolveField ───────────────────────────────────────────
// Resolve a field token (0x04xxxxxx) within the given context.
FieldInfoHandle ModuleAwareResolveField(
    ImageHandle image,
    uint32_t field_token) noexcept;

// ── ModuleAwareResolveToken ───────────────────────────────────────────
// Generic token resolver: dispatches by token high-byte to the appropriate
// metadata table.  Covers TypeDef, TypeRef, Method, Field, MemberRef tokens.
// Falls back through AOT bridge → patch cache → 0.
uint32_t ModuleAwareResolveToken(
    uint32_t token,
    ImageHandle image = 0,
    const PatchMetadataCache* patch_cache = nullptr) noexcept;

// ── LookupCallTarget ──────────────────────────────────────────────────
// Unified subject_id → call_target (MethodInfoHandle) resolution.
// Priority chain:
//   1. CodegenBridgeV0 reflection query model (search type descriptors by subjectId)
//   2. HotpatchNameRegistry (name-based lookup for hotpatch methods)
//   3. ExternalRuntimeDispatchTable (fallback for external/unresolved methods)
//
// Returns 0 when no source can resolve the subject_id.
MethodInfoHandle LookupCallTarget(
    const char* subject_id_utf8,
    const CodegenBridgeV0* bridge = nullptr,
    ImageHandle aot_image = 0,
    const PatchMetadataCache* patch_cache = nullptr) noexcept;

// ── ModuleLifecycleManager ───────────────────────────────────────────
// Three-in-one lifecycle manager: ModuleRegistry + GenericContextRegistry
// + HotpatchNameRegistry.  Provides atomic register/unregister operations.
//
// Usage during hotpatch apply:
//   auto* mgr = ModuleLifecycleManager::Get();
//   uint32_t mid = mgr->RegisterModule(name, descriptor, hotpatch_module, generics_reg);
//   ...
//   mgr->UnregisterModule(mid);  // atomically cleans up all three registries.
class ModuleLifecycleManager {
public:
    /// Global singleton (lazy-initialized).
    static ModuleLifecycleManager* Get() noexcept;

    /// Register a module across all three registries.
    /// @return module_id (from RegisterModule), or kInvalidModuleId on failure.
    uint32_t RegisterModule(
        const char* name,
        const ModuleDescriptor* module_desc,
        const HotpatchModuleV0* hotpatch_module = nullptr,
        const struct ModuleGenericRegistrationV0* generics_reg = nullptr) noexcept;

    /// Unregister (tombstone) a module across all three registries.
    /// Clears generic instantiations, marks module as tombstone.
    void UnregisterModule(uint32_t module_id) noexcept;

    /// Register hotpatch methods from a PatchContext.
    /// This is the runtime-side equivalent of RegisterModule for patch-loaded methods.
    /// Registers each method's module_id + token in the dispatch table.
    bool RegisterHotpatchContext(PatchContext* ctx) noexcept;

    /// Unregister a hotpatch context.
    bool UnregisterHotpatchContext(PatchContext* ctx) noexcept;

private:
    ModuleLifecycleManager() = default;
    ~ModuleLifecycleManager() = default;

    // Non-copyable, non-movable.
    ModuleLifecycleManager(const ModuleLifecycleManager&) = delete;
    ModuleLifecycleManager& operator=(const ModuleLifecycleManager&) = delete;
};

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_METADATA_INTERFACE_H_