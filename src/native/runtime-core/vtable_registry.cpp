#include "vtable_registry.h"

#include <chaos/native_types.h>

#include <mutex>
#include <unordered_map>
#include <vector>

namespace chaos::il2cpp::vtable_registry {

namespace {

struct VTableRegistryState {
    CHAOS_IL2CPP_MUTEX                                            mutex;
    // Key: type_token → registered vtable
    CHAOS_IL2CPP_UNORDERED_MAP(uint32_t, const TypeVTable*)       by_type_token;
};

VTableRegistryState& GetState() {
    static VTableRegistryState s_state;
    return s_state;
}

}  // namespace

bool RegisterTypeVTable(const TypeVTable* vtable) {
    if (vtable == nullptr || vtable->type_token == 0u || vtable->slots == nullptr) {
        return false;
    }

    auto& state = GetState();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(state.mutex);

    // Idempotent — re-registration of the same token is silently ignored.
    if (state.by_type_token.count(vtable->type_token)) {
        return true;
    }

    state.by_type_token[vtable->type_token] = vtable;
    return true;
}

void* ResolveVirtualMethodPointer(uint32_t instance_type_token,
                                  uint32_t declared_method_token) {
    if (instance_type_token == 0u || declared_method_token == 0u) {
        return nullptr;
    }

    auto& state = GetState();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(state.mutex);

    // Walk the inheritance chain starting from instance_type_token.
    uint32_t current_token = instance_type_token;
    while (current_token != 0u) {
        auto it = state.by_type_token.find(current_token);
        if (it == state.by_type_token.end()) {
            break;
        }

        const TypeVTable* vtable = it->second;
        for (uint32_t i = 0u; i < vtable->slot_count; ++i) {
            if (vtable->slots[i].method_token == declared_method_token) {
                return vtable->slots[i].method_pointer;
            }
        }

        // Move to base type
        current_token = vtable->base_token;
    }

    return nullptr;
}

uint32_t GetRegisteredVTableCount() {
    auto& state = GetState();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(state.mutex);
    return static_cast<uint32_t>(state.by_type_token.size());
}

}  // namespace chaos::il2cpp::vtable_registry
