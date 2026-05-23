// static_var_store.cpp — Indirect static field access implementation
//
// Maintains a registry mapping assembly_hash → (module_id, domain_id).
// Reads and writes delegate to AssemblyManager::GetStaticField().

#include "static_var_store.h"
#include "assembly_manager.h"

#include <mutex>
#include <map>
#include <unordered_map>

namespace {

struct AssemblyEntry {
    uint32_t module_id = 0;
    uint32_t domain_id = 0;
    bool registered = false;
};

std::mutex g_store_mutex;
std::unordered_map<uint32_t, AssemblyEntry> g_store;

}  // anonymous namespace

extern "C" void* static_var_store_read(uint32_t assembly_hash, uint32_t field_offset) {
    uint32_t module_id;
    {
        std::lock_guard<std::mutex> lock(g_store_mutex);
        auto it = g_store.find(assembly_hash);
        if (it == g_store.end() || !it->second.registered) {
            return nullptr;
        }
        module_id = it->second.module_id;
    }
    return chaos::il2cpp::runtime_core::AssemblyManager::Get()->GetStaticField(module_id, field_offset);
}

extern "C" void static_var_store_write(uint32_t assembly_hash, uint32_t field_offset, uintptr_t value) {
    void* ptr = static_var_store_read(assembly_hash, field_offset);
    if (ptr != nullptr) {
        *static_cast<uintptr_t*>(ptr) = value;
    }
}

extern "C" void static_var_store_register(uint32_t assembly_hash, uint32_t module_id, uint32_t domain_id) {
    std::lock_guard<std::mutex> lock(g_store_mutex);
    g_store[assembly_hash] = {module_id, domain_id, true};
}

extern "C" void static_var_store_unregister_domain(uint32_t domain_id) {
    std::lock_guard<std::mutex> lock(g_store_mutex);
    for (auto it = g_store.begin(); it != g_store.end(); ) {
        if (it->second.domain_id == domain_id) {
            it = g_store.erase(it);
        } else {
            ++it;
        }
    }
}

// ── Field-level store (AOT codegen indirect access) ──────────────────────
//
// Maintains a flat (assembly_hash, field_hash) → uintptr_t map for
// hot-update assembly static fields accessed via AOT codegen.
// Fields are lazily allocated on first access — no offset management needed.
//
// UnregisterAssembly clears all entries for a given assembly (called during
// AssemblyManager::UnloadAssembly).

namespace {

struct FieldEntry {
    uintptr_t value = 0;
    bool active = false;
};

// Key: (assembly_hash, field_hash).  Value: FieldEntry.
// Protected by g_store_mutex.
using FieldStore = std::map<std::pair<uint32_t, uint32_t>, FieldEntry>;
FieldStore g_field_store;

}  // anonymous namespace

extern "C" void* static_var_store_read_field(uint32_t assembly_hash, uint32_t field_hash) {
    std::lock_guard<std::mutex> lock(g_store_mutex);
    auto key = std::make_pair(assembly_hash, field_hash);
    auto& entry = g_field_store[key];
    entry.active = true;
    return &entry.value;
}

extern "C" void static_var_store_write_field(uint32_t assembly_hash, uint32_t field_hash, uintptr_t value) {
    std::lock_guard<std::mutex> lock(g_store_mutex);
    auto key = std::make_pair(assembly_hash, field_hash);
    g_field_store[key] = {value, true};
}

extern "C" void static_var_store_unregister_assembly(uint32_t assembly_hash) {
    std::lock_guard<std::mutex> lock(g_store_mutex);
    for (auto it = g_field_store.begin(); it != g_field_store.end(); ) {
        if (it->first.first == assembly_hash) {
            it = g_field_store.erase(it);
        } else {
            ++it;
        }
    }
}
