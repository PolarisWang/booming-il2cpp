#include "module_registry.h"

#include <chaos/native_types.h>
#include <chaos/trace.h>

#include <cstring>

namespace chaos::il2cpp::runtime_core {

// ── Global state ───────────────────────────────────────────────────────

// Sufficiently-aligned storage to avoid static initialization order issues.
static ModuleDescriptor g_module_storage[kMaxModules] = {};
static uint32_t g_module_count = 1;  // [0] = CoreLib fallback, always present

// Free list of recycled module_ids (from tombstone modules).
// RegisterModule checks this list before allocating a new linear slot.
static CHAOS_IL2CPP_VECTOR(uint32_t) g_free_list;

// ── Registry API ───────────────────────────────────────────────────────

uint32_t RegisterModule(const char* name, const ModuleDescriptor* descriptor) {
    if (name == nullptr || descriptor == nullptr) {
        return kInvalidModuleId;
    }

    uint32_t id = kInvalidModuleId;

    // Priority 1: Reuse a freed slot from the free list.
    if (!g_free_list.empty()) {
        id = g_free_list.back();
        g_free_list.pop_back();
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
            CHAOS_IL2CPP_PRINTF(
                "[runtime-core] WARN: module '%s' ABI manifest validation failed (code %d)\n",
                name, static_cast<int>(manifest_result));
            CHAOS_IL2CPP_FFLUSH(stdout);
        }
    }

    return id;
}

const ModuleDescriptor* LookupModule(uint32_t module_id) {
    if (module_id >= kMaxModules) {
        return nullptr;
    }
    // g_module_storage[id] is always populated for valid IDs (including
    // tombstone modules, where the entry is retained for handle safety).
    if (module_id >= g_module_count && !IsModuleTombstone(module_id)) {
        return nullptr;  // not yet allocated and not tombstone
    }
    return &g_module_storage[module_id];
}

const ModuleDescriptor* LookupModuleByName(const char* name) {
    if (name == nullptr) {
        return nullptr;
    }

    for (uint32_t i = 0; i < kMaxModules; i++) {
        // Skip unallocated slots and tombstone modules.
        if (i >= g_module_count && !IsModuleTombstone(i)) {
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
    CHAOS_IL2CPP_TRACE("runtime", "MarkModuleTombstone", "\"module_id\"=%u", module_id);
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
    // Keep name_utf8, type_names, type_namespaces (string literals from codegen).

    // Add to the free list so RegisterModule can reuse this slot.
    g_free_list.push_back(module_id);
}

bool IsModuleTombstone(uint32_t module_id) {
    if (module_id >= kMaxModules) {
        return false;
    }
    return g_module_storage[module_id].tombstone;
}

uint32_t GetModuleCount() {
    return g_module_count;
}

const ModuleDescriptor* GetModuleByIndex(uint32_t index) {
    if (index >= g_module_count) return nullptr;
    if (g_module_storage[index].tombstone) return nullptr;
    return &g_module_storage[index];
}

}  // namespace chaos::il2cpp::runtime_core
