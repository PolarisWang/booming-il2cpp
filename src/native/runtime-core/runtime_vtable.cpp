#include "runtime_vtable.h"

#include <chaos/trace.h>

#include <cstdlib>
#include <cstring>
#include <mutex>
#include <unordered_map>

namespace chaos::il2cpp::runtime_vtable {

namespace {

struct VTableEntry {
    const void**      vtable = nullptr;
    CHAOS_IL2CPP_UINT32 length = 0u;
};

struct Registry {
    CHAOS_IL2CPP_SHARED_MUTEX mtx;
    std::unordered_map<CHAOS_IL2CPP_UINT64, VTableEntry> entries;
};

Registry& GetRegistry() noexcept {
    static Registry reg;
    return reg;
}

}  // anonymous namespace

void RegisterVTable(CHAOS_IL2CPP_UINT64 stable_id,
                    const void** vtable,
                    CHAOS_IL2CPP_UINT32 length) noexcept {
    if (vtable == nullptr || length == 0u) return;

    auto& reg = GetRegistry();
    CHAOS_IL2CPP_UNIQUE_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(reg.mtx);
    reg.entries[stable_id] = {vtable, length};
}

const void** FindVTable(CHAOS_IL2CPP_UINT64 stable_id) noexcept {
    auto& reg = GetRegistry();
    CHAOS_IL2CPP_SHARED_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(reg.mtx);
    auto it = reg.entries.find(stable_id);
    return (it != reg.entries.end()) ? it->second.vtable : nullptr;
}

CHAOS_IL2CPP_UINT32 FindVTableLength(CHAOS_IL2CPP_UINT64 stable_id) noexcept {
    auto& reg = GetRegistry();
    CHAOS_IL2CPP_SHARED_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(reg.mtx);
    auto it = reg.entries.find(stable_id);
    return (it != reg.entries.end()) ? it->second.length : 0u;
}

const void** BuildRuntimeVTable(CHAOS_IL2CPP_UINT64 type_stable_id,
                                 CHAOS_IL2CPP_UINT64 base_stable_id) noexcept {
    CHAOS_IL2CPP_LOG_TRACE("runtime", "BuildRuntimeVTable",
        "\"type_sid\"=%llu,\"base_sid\"=%llu",
        (unsigned long long)type_stable_id,
        (unsigned long long)base_stable_id);
    // Find the base type's registered vtable.
    const void** base_vtable = FindVTable(base_stable_id);
    CHAOS_IL2CPP_UINT32 base_length = FindVTableLength(base_stable_id);
    if (base_vtable == nullptr || base_length == 0u) {
        return nullptr;
    }

    // Allocate a new vtable array.
    auto* new_vtable = static_cast<const void**>(
        std::malloc(sizeof(const void*) * base_length));
    if (new_vtable == nullptr) {
        return nullptr;
    }

    // Copy base vtable entries.
    std::memcpy(new_vtable, base_vtable, sizeof(const void*) * base_length);

    // Register the new vtable.
    RegisterVTable(type_stable_id, new_vtable, base_length);

    return new_vtable;
}

}  // namespace chaos::il2cpp::runtime_vtable
