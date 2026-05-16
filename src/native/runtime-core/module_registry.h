#ifndef CHAOS_IL2CPP_MODULE_REGISTRY_H_
#define CHAOS_IL2CPP_MODULE_REGISTRY_H_

#include <runtime_abi.h>
#include "reflection_query_model.h"
#include "abi_manifest.h"
#include "hotpatch_table.h"
#include <chaos/type_info.h>

#include <cstdint>

namespace chaos::il2cpp::runtime_core {

// ── Constants ──────────────────────────────────────────────────────────

constexpr uint32_t kModuleIdBits = 32;
constexpr uint32_t kMaxModules = 4096;
constexpr uint32_t kInvalidModuleId = 0xFFFFFFFFu;

// ── Type flag bit definitions ──────────────────────────────────────────

constexpr uint32_t kFlagIsValueType          = 1u << 0;
constexpr uint32_t kFlagIsEnum               = 1u << 1;
constexpr uint32_t kFlagIsAbstract           = 1u << 2;
constexpr uint32_t kFlagIsSealed             = 1u << 3;
constexpr uint32_t kFlagIsInterface          = 1u << 4;
constexpr uint32_t kFlagIsArray              = 1u << 5;
constexpr uint32_t kFlagIsGenericType        = 1u << 6;
constexpr uint32_t kFlagIsGenericTypeDef     = 1u << 7;
constexpr uint32_t kFlagIsConstructedGeneric = 1u << 8;
constexpr uint32_t kFlagIsNested             = 1u << 9;
constexpr uint32_t kFlagIsPublic             = 1u << 10;
constexpr uint32_t kFlagIsByRef              = 1u << 11;
constexpr uint32_t kFlagIsPointer            = 1u << 12;

// ── ModuleDescriptor ───────────────────────────────────────────────────

struct ModuleDescriptor {
    const char* name_utf8;                  // Module name (e.g. "System.Linq")
    const ReflectionQueryImageDescriptor* image;  // Tier 2 cold-path descriptor
    const uint32_t* type_flags;             // Tier 1: per-type bit flags
    const char* const* type_names;          // Tier 1: per-type names
    const char* const* type_namespaces;     // Tier 1: per-type namespaces
    const uint32_t* type_parent_tokens;     // Tier 1: per-type parent tokens
    const TypeInfoHot* const* type_info_ptrs;   // Tier 1: per-type TypeInfoHot* pointers (Phase 3+)
    const uint32_t* nested_type_children;    // Tier 1: flat array of child tokens, grouped by parent type
    const uint32_t* nested_type_offset;      // Tier 1: [type_count+1] prefix-sum into nested_type_children
    const uint32_t* generic_param_constraint_data;   // Tier 1: flat array of (param_index<<28|token) per constraint
    const uint32_t* generic_param_constraint_offset; // Tier 1: [type_count+1] prefix-sum into constraint_data
    uint32_t type_count;                    // Number of types managed by this module
    const uint8_t* custom_attribute_blob;                    // CustomAttribute compact binary data (Tier 1)
    const uint32_t* custom_attribute_offset;                 // [type_count+1] prefix-sum into blob by TokenToIndex
    uint32_t custom_attribute_entity_count;                  // type_count for validation (0 = no attributes)
    CHAOS_IL2CPP_INTPTR (*custom_attribute_materializer)(   // Per-module materializer: resolves TypeInfo*/VTable*
        uint32_t attr_type_token,                            //   by attr type token, creates + assigns from field_data
        const uint8_t* field_data);                          //   Returns object pointer or 0
    const ChaosAbiManifestV0* abi_manifest; // Per-module ABI manifest (null = no validation)
    bool tombstone = false;                 // true after hot-unload (module entry retained for handle safety)
};

// ── TypeInfoHandle encode/decode ───────────────────────────────────────
// Encoding: [module_id:32 bits] [token:32 bits]
//   module_id = index into g_modules[] (0 = CoreLib fallback)
//   token     = ECMA metadata token (e.g. 0x02000001)

inline uint32_t GetModuleId(TypeInfoHandle handle) {
    return static_cast<uint32_t>(handle >> 32);
}

inline uint32_t GetTypeToken(TypeInfoHandle handle) {
    return static_cast<uint32_t>(handle & 0xFFFFFFFFu);
}

inline TypeInfoHandle MakeTypeHandle(uint32_t module_id, uint32_t token) {
    return (static_cast<uint64_t>(module_id) << 32) | token;
}

// ── Token → index conversion ──────────────────────────────────────────
// ECMA metadata token: 0x02TTTTTT — low 24 bits = table index
// TypeDef table starts at 1 (0 reserved), so type index = raw_index - 1.

inline uint32_t TokenToIndex(uint32_t token) {
    uint32_t raw_index = token & 0x00FFFFFFu;
    return raw_index > 0 ? raw_index - 1 : 0;
}

// ── Registry API ──────────────────────────────────────────────────────

uint32_t RegisterModule(const char* name, const ModuleDescriptor* descriptor);
const ModuleDescriptor* LookupModule(uint32_t module_id);
const ModuleDescriptor* LookupModuleByName(const char* name);

/// Mark a module entry as tombstone (hot-unloaded).
/// The module_id is recycled: subsequent RegisterModule calls may reuse it.
/// After marking, LookupModule(module_id) still returns a valid pointer,
/// but type_count is 0 and image/type_flags/type_parent_tokens are nulled
/// to prevent access to freed memory.
void MarkModuleTombstone(uint32_t module_id);

/// Returns true if the given module_id refers to a tombstone (unloaded) module.
bool IsModuleTombstone(uint32_t module_id);

/// Returns the ABI manifest for the given module, or nullptr if none registered.
inline const ChaosAbiManifestV0* LookupModuleAbiManifest(uint32_t module_id) {
    const auto* desc = LookupModule(module_id);
    return desc != nullptr ? desc->abi_manifest : nullptr;
}

/// Returns the total number of registered module slots (including slot 0).
/// Iterate [0, GetModuleCount()) with GetModuleByIndex().
uint32_t GetModuleCount();

/// Returns the module descriptor for the given index, or nullptr if the slot
/// is unallocated or tombstoned.
const ModuleDescriptor* GetModuleByIndex(uint32_t index);

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_MODULE_REGISTRY_H_
