#include "module_registry.h"

#include <chaos/native_types.h>
#include <chaos/trace.h>
#include <chaos/log.h>

#include <cstring>

#include <shared_mutex>

namespace chaos::il2cpp::runtime_core {

// ── Global state ───────────────────────────────────────────────────────

// Sufficiently-aligned storage to avoid static initialization order issues.
// kMaxModules = 4096 supports 200+ DLL scenarios.
static ModuleDescriptor g_module_storage[kMaxModules] = {};
static uint32_t g_module_count = 1;  // [0] = CoreLib fallback, always present

// Free list of recycled module_ids (from tombstone modules).
// RegisterModule checks this list before allocating a new linear slot.
// Function-local static to avoid cross-TU static init ordering fiasco:
// std::vector requires dynamic initialization; callers may run before
// file-scope statics in this TU are constructed.
static auto& g_free_list() {
    static CHAOS_IL2CPP_VECTOR(uint32_t) list;
    return list;
}

// Reader-writer lock guarding all module registry state:
//   g_module_storage[], g_module_count, g_free_list()
// Read paths (LookupModule, LookupModuleByName, etc.) acquire shared_lock.
// Write paths (RegisterModule, MarkModuleTombstone) acquire unique_lock.
// Must NOT be recursively acquired -- internal callers must not call
// another locked function while holding the lock.
// Function-local static to avoid cross-TU static init ordering fiasco:
// callers (generated static initializers) may run before file-scope statics
// in this TU are constructed.
static std::shared_mutex& g_module_mutex() {
    static std::shared_mutex mutex;
    return mutex;
}

// ── Registry API ───────────────────────────────────────────────────────

uint32_t RegisterModule(const char* name, const ModuleDescriptor* descriptor) {
    if (name == nullptr || descriptor == nullptr) {
        return kInvalidModuleId;
    }

    std::unique_lock<std::shared_mutex> lock(g_module_mutex());

    uint32_t id = kInvalidModuleId;

    // Priority 1: Reuse a freed slot from the free list.
    if (!g_free_list().empty()) {
        id = g_free_list().back();
        g_free_list().pop_back();
    }

    // Priority 2: Allocate a new linear slot.
    if (id == kInvalidModuleId) {
        if (g_module_count >= kMaxModules) {
            return kInvalidModuleId;
        }
        id = g_module_count;
        g_module_count++;
    }

    g_module_storage[id] = *descriptor;
    g_module_storage[id].name_utf8 = name;
    g_module_storage[id].tombstone = false;

    // Validate ABI manifest if present (fail-open during development).
    if (descriptor->abi_manifest != nullptr)
    {
        ChaosAbiManifestResult manifest_result = ChaosAbiManifestValidate(descriptor->abi_manifest);
        if (manifest_result != CHAOS_ABI_MANIFEST_OK)
        {
            CHAOS_IL2CPP_LOG_WARN_M("ABI", "module '{0}' ABI manifest validation failed (code {1})",
                name, static_cast<int>(manifest_result));
        }
    }

    return id;
}

const ModuleDescriptor* LookupModule(uint32_t module_id) {
    std::shared_lock<std::shared_mutex> lock(g_module_mutex());

    if (module_id >= kMaxModules) {
        return nullptr;
    }
    // Check if slot is actually occupied by comparing name_utf8 (set to
    // nullptr on tombstone, never set for unallocated slots).  Using
    // g_module_count + tombstone fails when RegisterModule reuses a
    // freed slot with index >= g_module_count.
    if (g_module_storage[module_id].name_utf8 == nullptr) {
        return nullptr;  // slot not occupied
    }
    return &g_module_storage[module_id];
}

const ModuleDescriptor* LookupModuleByName(const char* name) {
    std::shared_lock<std::shared_mutex> lock(g_module_mutex());

    if (name == nullptr) {
        return nullptr;
    }

    for (uint32_t i = 0; i < kMaxModules; i++) {
        // Skip unallocated slots and tombstone modules.
        if (i >= g_module_count && !g_module_storage[i].tombstone) {
            break;
        }
        if (g_module_storage[i].tombstone) {
            continue;
        }
        if (g_module_storage[i].name_utf8 != nullptr &&
            std::strcmp(g_module_storage[i].name_utf8, name) == 0) {
            return &g_module_storage[i];
        }
    }

    return nullptr;
}

void MarkModuleTombstone(uint32_t module_id) {
    CHAOS_IL2CPP_LOG_TRACE("runtime", "MarkModuleTombstone", "\"module_id\"=%u", module_id);

    std::unique_lock<std::shared_mutex> lock(g_module_mutex());

    if (module_id >= kMaxModules) {
        return;
    }
    if (module_id == 0u) {
        return;  // CoreLib fallback — never tombstone.
    }
    if (g_module_storage[module_id].tombstone) {
        return;  // already a tombstone
    }

    // Mark as tombstone and null out fields that reference freed memory.
    g_module_storage[module_id].tombstone    = true;
    g_module_storage[module_id].type_count   = 0u;
    g_module_storage[module_id].image        = nullptr;
    g_module_storage[module_id].type_flags   = nullptr;
    g_module_storage[module_id].type_parent_tokens = nullptr;
    g_module_storage[module_id].custom_attribute_blob    = nullptr;
    g_module_storage[module_id].custom_attribute_offset  = nullptr;
    g_module_storage[module_id].custom_attribute_entity_count = 0u;
    g_module_storage[module_id].custom_attribute_materializer = nullptr;
    // Keep name_utf8, type_names, type_namespaces (string literals from codegen).

    // Add to the free list so RegisterModule can reuse this slot.
    g_free_list().push_back(module_id);
}

bool IsModuleTombstone(uint32_t module_id) {
    std::shared_lock<std::shared_mutex> lock(g_module_mutex());

    if (module_id >= kMaxModules) {
        return false;
    }
    return g_module_storage[module_id].tombstone;
}

uint32_t GetModuleCount() {
    std::shared_lock<std::shared_mutex> lock(g_module_mutex());
    return g_module_count;
}

const ModuleDescriptor* GetModuleByIndex(uint32_t index) {
    std::shared_lock<std::shared_mutex> lock(g_module_mutex());

    if (index >= g_module_count) return nullptr;
    if (g_module_storage[index].tombstone) return nullptr;
    return &g_module_storage[index];
}

const TypeInfoHot* LookupTypeInfoPtr(uint32_t module_id, uint32_t token) {
    std::shared_lock<std::shared_mutex> lock(g_module_mutex());

    if (module_id >= kMaxModules) return nullptr;
    if (g_module_storage[module_id].name_utf8 == nullptr) return nullptr;
    if (g_module_storage[module_id].tombstone) return nullptr;
    if (g_module_storage[module_id].type_flags == nullptr) return nullptr;
    if (g_module_storage[module_id].type_info_ptrs == nullptr) return nullptr;

    uint32_t idx = TokenToIndex(token);
    if (idx >= g_module_storage[module_id].type_count) return nullptr;

    return g_module_storage[module_id].type_info_ptrs[idx];
}

const char* LookupTypeNameByInfoPtr(const TypeInfoHot* type_info, const char** out_namespace) {
    if (type_info == nullptr) return nullptr;

    std::shared_lock<std::shared_mutex> lock(g_module_mutex());

    for (uint32_t m = 0; m < g_module_count; m++) {
        auto& mod = g_module_storage[m];
        if (mod.name_utf8 == nullptr || mod.tombstone) continue;
        if (mod.type_info_ptrs == nullptr || mod.type_names == nullptr) continue;

        for (uint32_t t = 0; t < mod.type_count; t++) {
            if (mod.type_info_ptrs[t] == type_info) {
                if (out_namespace != nullptr) {
                    *out_namespace = mod.type_namespaces != nullptr ? mod.type_namespaces[t] : nullptr;
                }
                return mod.type_names[t];
            }
        }
    }
    return nullptr;
}

}  // namespace chaos::il2cpp::runtime_core
