#include "vtable_registry.h"
#include "reflection_query_model.h"
#include "runtime_core.h"

#include <chaos/native_types.h>

#include <cstdlib>
#include <mutex>
#include <new>
#include <unordered_map>
#include <vector>

namespace chaos::il2cpp::vtable_registry {

namespace {

struct VTableRegistryState {
    CHAOS_IL2CPP_MUTEX                                            mutex;
    // Key: type_token → registered vtable
    CHAOS_IL2CPP_UNORDERED_MAP(CHAOS_IL2CPP_UINT32, const TypeVTable*)       by_type_token;
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

bool RegisterRuntimeVTable(
    TypeInfoHandle               type,
    TypeInfoHandle               base_type,
    CHAOS_IL2CPP_UINT32         slot_count,
    const VTableSlot*           slots)
{
    if (type == 0 || slots == nullptr || slot_count == 0u) {
        return false;
    }

    // Extract the runtime token from the TypeInfoHandle.
    CHAOS_IL2CPP_UINT32 type_token = 0u;
    const auto* desc = TryDecodeReflectionQueryTypeHandle(type);
    if (desc != nullptr) {
        type_token = desc->metadata_token;
    }
    if (type_token == 0u) {
        return false;  // Cannot register without a valid token.
    }

    // Extract base token.
    CHAOS_IL2CPP_UINT32 base_token = 0u;
    if (base_type != 0) {
        const auto* base_desc = TryDecodeReflectionQueryTypeHandle(base_type);
        if (base_desc != nullptr) {
            base_token = base_desc->metadata_token;
        }
    }

    // Heap-allocate a TypeVTable that lives for the process lifetime.
    auto* vtable = static_cast<TypeVTable*>(std::malloc(sizeof(TypeVTable)));
    if (vtable == nullptr) {
        return false;
    }
    vtable->type        = type;
    vtable->type_token  = type_token;
    vtable->base_type   = base_type;
    vtable->base_token  = base_token;
    vtable->slot_count  = slot_count;
    vtable->slots       = slots;

    auto& state = GetState();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(state.mutex);

    if (state.by_type_token.count(type_token)) {
        std::free(vtable);
        return true;  // idempotent
    }

    state.by_type_token[type_token] = vtable;
    return true;
}

void* ResolveVirtualMethodPointer(
    CHAOS_IL2CPP_UINT32 instance_type_token,
    CHAOS_IL2CPP_UINT32 declared_method_token) {
    if (instance_type_token == 0u || declared_method_token == 0u) {
        return nullptr;
    }

    auto& state = GetState();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(state.mutex);

    // Walk the inheritance chain starting from instance_type_token.
    CHAOS_IL2CPP_UINT32 current_token = instance_type_token;
    while (current_token != 0u) {
        auto it = state.by_type_token.find(current_token);
        if (it == state.by_type_token.end()) {
            break;
        }

        const TypeVTable* vtable = it->second;
        for (CHAOS_IL2CPP_UINT32 i = 0u; i < vtable->slot_count; ++i) {
            if (vtable->slots[i].method_token == declared_method_token) {
                return vtable->slots[i].method_pointer;
            }
        }

        // Move to base type
        current_token = vtable->base_token;
    }

    return nullptr;
}

CHAOS_IL2CPP_UINT32 GetRegisteredVTableCount() {
    auto& state = GetState();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(state.mutex);
    return static_cast<CHAOS_IL2CPP_UINT32>(state.by_type_token.size());
}

}  // namespace chaos::il2cpp::vtable_registry
