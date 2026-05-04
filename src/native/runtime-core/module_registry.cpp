#include "module_registry.h"

#include <cstring>

namespace chaos::il2cpp::runtime_core {

// ── Global state ───────────────────────────────────────────────────────

// Sufficiently-aligned storage to avoid static initialization order issues.
static ModuleDescriptor g_module_storage[kMaxModules] = {};
static uint32_t g_module_count = 1;  // [0] = CoreLib fallback, always present

// ── Registry API ───────────────────────────────────────────────────────

uint32_t RegisterModule(const char* name, const ModuleDescriptor* descriptor) {
    if (name == nullptr || descriptor == nullptr) {
        return kInvalidModuleId;
    }

    if (g_module_count >= kMaxModules) {
        return kInvalidModuleId;
    }

    // AOT startup is serial — no lock needed for initial registration.
    uint32_t id = g_module_count;
    g_module_storage[id] = *descriptor;
    g_module_storage[id].name_utf8 = name;
    g_module_count++;
    return id;
}

const ModuleDescriptor* LookupModule(uint32_t module_id) {
    if (module_id >= g_module_count) {
        return nullptr;
    }
    return &g_module_storage[module_id];
}

const ModuleDescriptor* LookupModuleByName(const char* name) {
    if (name == nullptr) {
        return nullptr;
    }

    for (uint32_t i = 0; i < g_module_count; i++) {
        if (g_module_storage[i].name_utf8 != nullptr &&
            std::strcmp(g_module_storage[i].name_utf8, name) == 0) {
            return &g_module_storage[i];
        }
    }

    return nullptr;
}

}  // namespace chaos::il2cpp::runtime_core
