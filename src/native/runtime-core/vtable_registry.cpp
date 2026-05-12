#include "vtable_registry.h"
#include "reflection_query_model.h"
#include "runtime_core.h"
#include "type_registry.h"

#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include <chaos/profile.h>

#include <cstdlib>
#include <cstring>
#include <mutex>
#include <new>
#include <unordered_map>
#include <vector>

namespace chaos::il2cpp::vtable_registry {

namespace {

struct VTableRegistryState {
    CHAOS_IL2CPP_SHARED_MUTEX                                         mutex;
    // Primary key: stable_id → vtable
    CHAOS_IL2CPP_UNORDERED_MAP(CHAOS_IL2CPP_UINT64, const TypeVTable*)       by_stable_id;
    // Secondary key: type_token → vtable
    CHAOS_IL2CPP_UNORDERED_MAP(CHAOS_IL2CPP_UINT32, const TypeVTable*)       by_type_token;
    // Flat vtable arrays (for AOT codegen direct dispatch)
    CHAOS_IL2CPP_UNORDERED_MAP(CHAOS_IL2CPP_UINT64, const void**)            flat_vtables;
    CHAOS_IL2CPP_UNORDERED_MAP(CHAOS_IL2CPP_UINT64, CHAOS_IL2CPP_UINT32)     flat_lengths;
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
    CHAOS_IL2CPP_UNIQUE_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);

    // Idempotent — re-registration of the same token is silently ignored.
    if (state.by_type_token.count(vtable->type_token)) {
        return true;
    }

    state.by_type_token[vtable->type_token] = vtable;
    if (vtable->stable_id != 0u) {
        state.by_stable_id[vtable->stable_id] = vtable;
    }
    return true;
}

void RegisterVTableArray(CHAOS_IL2CPP_UINT64 stable_id,
                         const void** vtable,
                         CHAOS_IL2CPP_UINT32 length) noexcept {
    if (stable_id == 0u || vtable == nullptr || length == 0u) return;
    auto& state = GetState();
    CHAOS_IL2CPP_UNIQUE_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);
    state.flat_vtables[stable_id] = vtable;
    state.flat_lengths[stable_id] = length;
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
    const auto* desc = chaos::il2cpp::runtime_core::TryDecodeReflectionQueryTypeHandle(type);
    if (desc != nullptr) {
        type_token = desc->metadata_token;
    }
    if (type_token == 0u) {
        return false;  // Cannot register without a valid token.
    }

    // Extract base token.
    CHAOS_IL2CPP_UINT32 base_token = 0u;
    if (base_type != 0) {
        const auto* base_desc = chaos::il2cpp::runtime_core::TryDecodeReflectionQueryTypeHandle(base_type);
        if (base_desc != nullptr) {
            base_token = base_desc->metadata_token;
        }
    }

    // Heap-allocate a TypeVTable that lives for the process lifetime.
    auto* vtable = static_cast<TypeVTable*>(CHAOS_IL2CPP_MALLOC(sizeof(TypeVTable)));
    if (vtable == nullptr) {
        return false;
    }
    vtable->type        = type;
    vtable->stable_id   = 0u;
    vtable->type_token  = type_token;
    vtable->base_type   = base_type;
    vtable->base_stable_id = 0u;
    vtable->base_token  = base_token;
    vtable->slot_count  = slot_count;
    vtable->slots       = slots;
    vtable->vtable_array    = nullptr;
    vtable->vtable_length   = 0u;

    auto& state = GetState();
    CHAOS_IL2CPP_UNIQUE_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);

    if (state.by_type_token.count(type_token)) {
        CHAOS_IL2CPP_FREE(vtable);
        return true;  // idempotent
    }

    state.by_type_token[type_token] = vtable;
    return true;
}

const TypeVTable* TryGetTypeVTable(CHAOS_IL2CPP_UINT32 type_token) {
    if (type_token == 0u) {
        return nullptr;
    }

    auto& state = GetState();
    CHAOS_IL2CPP_SHARED_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);

    auto it = state.by_type_token.find(type_token);
    if (it == state.by_type_token.end()) {
        return nullptr;
    }
    return it->second;
}

const TypeVTable* TryGetTypeVTableByStableId(CHAOS_IL2CPP_UINT64 stable_id) {
    if (stable_id == 0u) return nullptr;
    auto& state = GetState();
    CHAOS_IL2CPP_SHARED_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);
    auto it = state.by_stable_id.find(stable_id);
    return (it != state.by_stable_id.end()) ? it->second : nullptr;
}

const void** FindVTable(CHAOS_IL2CPP_UINT64 stable_id) noexcept {
    auto& state = GetState();
    CHAOS_IL2CPP_SHARED_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);
    auto it = state.flat_vtables.find(stable_id);
    return (it != state.flat_vtables.end()) ? it->second : nullptr;
}

CHAOS_IL2CPP_UINT32 FindVTableLength(CHAOS_IL2CPP_UINT64 stable_id) noexcept {
    auto& state = GetState();
    CHAOS_IL2CPP_SHARED_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);
    auto it = state.flat_lengths.find(stable_id);
    return (it != state.flat_lengths.end()) ? it->second : 0u;
}

const void** BuildRuntimeVTable(CHAOS_IL2CPP_UINT64 type_stable_id,
                                 CHAOS_IL2CPP_UINT64 base_stable_id) noexcept {
    if (type_stable_id == 0u || base_stable_id == 0u) return nullptr;

    auto& state = GetState();
    CHAOS_IL2CPP_UNIQUE_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);

    // Check if already built
    if (state.flat_vtables.count(type_stable_id)) {
        return state.flat_vtables[type_stable_id];
    }

    // Copy from base
    auto base_it = state.flat_vtables.find(base_stable_id);
    if (base_it == state.flat_vtables.end()) return nullptr;

    auto len_it = state.flat_lengths.find(base_stable_id);
    CHAOS_IL2CPP_UINT32 length = (len_it != state.flat_lengths.end()) ? len_it->second : 0u;
    if (length == 0u) return nullptr;

    auto* new_vtable = static_cast<const void**>(CHAOS_IL2CPP_MALLOC(length * sizeof(void*)));
    if (new_vtable == nullptr) return nullptr;

    CHAOS_IL2CPP_MEMCPY(new_vtable, base_it->second, length * sizeof(void*));
    state.flat_vtables[type_stable_id] = new_vtable;
    state.flat_lengths[type_stable_id] = length;
    return new_vtable;
}

void* ResolveVirtualMethodPointer(
    CHAOS_IL2CPP_UINT32 instance_type_token,
    CHAOS_IL2CPP_UINT32 declared_method_token) {
    CHAOS_IL2CPP_PROFILE_SCOPE("ResolveVirtualMethodPointer");
    if (instance_type_token == 0u) {
        return nullptr;
    }

    auto& state = GetState();
    CHAOS_IL2CPP_SHARED_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);

    // Walk the inheritance chain starting from instance_type_token.
    // When declared_method_token is 0 (interface slot 0), skip this walk
    // since no type has a slot with method_token == 0.
    if (declared_method_token != 0u) {
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
    }

    // Base chain walk failed (or skipped) — try interface vtable map.
    // The declared_method_token on interface dispatch is the slot index
    // within the interface's method table. Walk interface entries to find
    // the matching vtable offset.
    {
        auto it = state.by_type_token.find(instance_type_token);
        if (it != state.by_type_token.end()) {
            const TypeVTable* vtable = it->second;
            if (vtable->iface_map != nullptr && vtable->iface_count > 0u &&
                vtable->vtable_array != nullptr) {
                const auto* iface_entries = static_cast<const ChaosIl2cpp::Common::InterfaceMapEntry*>(vtable->iface_map);
                // When calling through an interface, declared_method_token is a small
                // zero-based slot index within that interface's method table.
                // Scan all interfaces to find one whose slots cover this index.
                for (CHAOS_IL2CPP_UINT32 ifi = 0u; ifi < vtable->iface_count; ++ifi) {
                    if (declared_method_token < iface_entries[ifi].method_count) {
                        // Slot is within this interface's range
                        CHAOS_IL2CPP_UINT32 vtable_slot =
                            iface_entries[ifi].vtable_offset + declared_method_token;
                        if (vtable_slot < vtable->vtable_length) {
                            return const_cast<void*>(vtable->vtable_array[vtable_slot]);
                        }
                    }
                }
            }
        }
    }

    return nullptr;
}

void* ResolveVirtualMethodPointerByHandle(
    TypeInfoHandle               instance_type,
    CHAOS_IL2CPP_UINT32         declared_method_token)
{
    if (instance_type == 0) return nullptr;
    const auto* desc = chaos::il2cpp::runtime_core::TryDecodeReflectionQueryTypeHandle(instance_type);
    if (desc == nullptr) return nullptr;
    return ResolveVirtualMethodPointer(desc->metadata_token, declared_method_token);
}

CHAOS_IL2CPP_UINT32 GetRegisteredVTableCount() {
    auto& state = GetState();
    CHAOS_IL2CPP_SHARED_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);
    return static_cast<CHAOS_IL2CPP_UINT32>(state.by_type_token.size());
}

}  // namespace chaos::il2cpp::vtable_registry
