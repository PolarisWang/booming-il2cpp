#include "vtable_registry_internal.h"
#include "reflection_query_model.h"
#include "runtime_core.h"
#include "type_registry.h"

#include <chaos/profile.h>
#include <cstdlib>
#include <cstring>

namespace chaos::il2cpp::vtable_registry {
using namespace internal;

bool RegisterTypeVTable(const TypeVTable* vtable) noexcept {
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

        // Also register the flat vtable array so that FindVTable() and
        // TypeInfoHot::vtable_array lookups work for codegen-emitted types.
        if (vtable->vtable_array != nullptr && vtable->vtable_length > 0u) {
            state.flat_vtables[vtable->stable_id] = vtable->vtable_array;
            state.flat_lengths[vtable->stable_id] = vtable->vtable_length;
        }
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

void RegisterCodegenVTable(const void* desc) noexcept {
    if (desc == nullptr) return;
    const auto* vtd = static_cast<const VTableDescriptorV0*>(desc);
    if (vtd->type_token == 0u || vtd->slots == nullptr || vtd->slot_count == 0u) {
        return;
    }

    // Heap-allocate a TypeVTable that lives for the process lifetime.
    // The VTableSlot and vtable_array data itself lives in .rodata (codegen-emitted)
    // or the caller's managed memory, so we only need to copy the TypeVTable struct.
    auto* tv = static_cast<TypeVTable*>(CHAOS_IL2CPP_MALLOC(sizeof(TypeVTable)));
    if (tv == nullptr) return;
    tv->type           = 0;   // Filled later via type_token → TypeInfoHandle mapping
    tv->stable_id      = vtd->stable_id;
    tv->type_token     = vtd->type_token;
    tv->base_type      = 0;   // Filled later
    tv->base_stable_id = 0;
    tv->base_token     = vtd->base_token;
    tv->slot_count     = vtd->slot_count;
    tv->slots          = static_cast<const VTableSlot*>(vtd->slots);
    tv->vtable_array   = vtd->vtable_array;
    tv->vtable_length  = vtd->vtable_length;
    tv->type_shape     = vtd->type_shape;
    tv->_owns_slots        = 0;  // .rodata, not owned
    tv->_owns_vtable_array = 0;  // .rodata, not owned
    tv->_pad[0] = tv->_pad[1] = 0;
    tv->iface_map      = vtd->iface_map;
    tv->iface_count    = vtd->iface_count;
    tv->runtime_iface_map  = nullptr;
    tv->runtime_iface_count = 0u;

    // RegisterTypeVTable now also populates flat_vtables when vtable_array is set.
    RegisterTypeVTable(tv);
}

bool RegisterHotUpdateVTable(
    CHAOS_IL2CPP_UINT64        stable_id,
    CHAOS_IL2CPP_UINT32        type_token,
    CHAOS_IL2CPP_UINT32        parent_token,
    const VTableSlot*          override_slots,
    CHAOS_IL2CPP_UINT32        override_count,
    CHAOS_IL2CPP_UINT8         type_shape) noexcept
{
    if (type_token == 0u) return false;

    auto& state = GetState();
    CHAOS_IL2CPP_UNIQUE_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);

    // Idempotent — skip if already registered.
    if (state.by_type_token.count(type_token) != 0u) return true;

    // Find parent TypeVTable (may be null).
    const TypeVTable* parent = nullptr;
    {
        auto pit = state.by_type_token.find(parent_token);
        if (pit != state.by_type_token.end()) parent = pit->second;
    }

    // ── Build flat vtable array: copy from parent, then apply overrides ──
    CHAOS_IL2CPP_UINT32 vtable_length = 0u;
    const void** vtable_array = nullptr;

    if (parent != nullptr && parent->vtable_array != nullptr) {
        vtable_length = parent->vtable_length;
        vtable_array = static_cast<const void**>(
            CHAOS_IL2CPP_MALLOC(vtable_length * sizeof(void*)));
        if (vtable_array == nullptr) return false;
        CHAOS_IL2CPP_MEMCPY(const_cast<void**>(vtable_array),
                            parent->vtable_array,
                            vtable_length * sizeof(void*));
    }

    // Apply overrides to the flat vtable array by matching method_token
    if (vtable_array != nullptr && override_slots != nullptr && override_count > 0u) {
        for (CHAOS_IL2CPP_UINT32 i = 0u; i < override_count; ++i) {
            CHAOS_IL2CPP_UINT32 mt = override_slots[i].method_token;
            for (CHAOS_IL2CPP_UINT32 j = 0u; j < parent->slot_count; ++j) {
                if (parent->slots[j].method_token == mt) {
                    const_cast<void**>(vtable_array)[j] = override_slots[i].method_pointer;
                    break;
                }
            }
        }
    }

    // ── Build VTableSlot[]: copy from parent, apply overrides ──
    CHAOS_IL2CPP_UINT32 slot_count = (parent != nullptr) ? parent->slot_count : override_count;
    auto* slots = static_cast<VTableSlot*>(
        CHAOS_IL2CPP_MALLOC(slot_count * sizeof(VTableSlot)));
    if (slots == nullptr) {
        if (vtable_array != nullptr) CHAOS_IL2CPP_FREE(const_cast<void**>(vtable_array));
        return false;
    }

    if (parent != nullptr) {
        CHAOS_IL2CPP_MEMCPY(slots, parent->slots, parent->slot_count * sizeof(VTableSlot));
        for (CHAOS_IL2CPP_UINT32 i = 0u; i < override_count; ++i) {
            for (CHAOS_IL2CPP_UINT32 j = 0u; j < parent->slot_count; ++j) {
                if (parent->slots[j].method_token == override_slots[i].method_token) {
                    slots[j].method_pointer = override_slots[i].method_pointer;
                    break;
                }
            }
        }
    } else {
        CHAOS_IL2CPP_MEMCPY(slots, override_slots, override_count * sizeof(VTableSlot));
    }

    // ── Allocate TypeVTable ──
    auto* tv = static_cast<TypeVTable*>(CHAOS_IL2CPP_MALLOC(sizeof(TypeVTable)));
    if (tv == nullptr) {
        CHAOS_IL2CPP_FREE(slots);
        if (vtable_array != nullptr) CHAOS_IL2CPP_FREE(const_cast<void**>(vtable_array));
        return false;
    }

    tv->type           = 0;
    tv->stable_id      = stable_id;
    tv->type_token     = type_token;
    tv->base_type      = 0;
    tv->base_stable_id = (parent != nullptr) ? parent->stable_id : 0u;
    tv->base_token     = parent_token;
    tv->slot_count     = slot_count;
    tv->slots          = slots;
    tv->vtable_array   = vtable_array;
    tv->vtable_length  = vtable_length;
    tv->type_shape     = type_shape;
    tv->_owns_slots        = 1;  // heap-allocated
    tv->_owns_vtable_array = (vtable_array != nullptr) ? 1 : 0;
    tv->_pad[0] = tv->_pad[1] = 0;
    tv->iface_map      = nullptr;
    tv->iface_count    = 0u;
    tv->runtime_iface_map  = nullptr;
    tv->runtime_iface_count = 0u;

    state.by_type_token[type_token] = tv;
    if (stable_id != 0u) {
        state.by_stable_id[stable_id] = tv;
    }
    if (vtable_array != nullptr && vtable_length > 0u) {
        state.flat_vtables[stable_id] = vtable_array;
        state.flat_lengths[stable_id] = vtable_length;
    }
    return true;
}

// ── RegisterTypeVTableRuntimeInterface ─────────────────────────────────
bool RegisterTypeVTableRuntimeInterface(
    CHAOS_IL2CPP_UINT32       type_token,
    CHAOS_IL2CPP_UINT64       iface_stable_id,
    CHAOS_IL2CPP_UINT32       vtable_offset,
    CHAOS_IL2CPP_UINT32       method_count) noexcept
{
    if (type_token == 0u || iface_stable_id == 0u) return false;

    auto& state = GetState();
    CHAOS_IL2CPP_UNIQUE_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);

    auto it = state.by_type_token.find(type_token);
    if (it == state.by_type_token.end()) return false;

    auto* tv = const_cast<TypeVTable*>(it->second);

    // Check for duplicates in existing runtime map.
    if (tv->runtime_iface_map != nullptr) {
        const auto* existing = static_cast<const chaos::il2cpp::common::InterfaceMapEntry*>(tv->runtime_iface_map);
        for (CHAOS_IL2CPP_UINT32 i = 0u; i < tv->runtime_iface_count; ++i) {
            if (existing[i].iface_stable_id == iface_stable_id) {
                return true;  // already present, idempotent
            }
        }
    }

    // Realloc the runtime_iface_map (one past current count).
    CHAOS_IL2CPP_UINT32 new_count = tv->runtime_iface_count + 1u;
    auto* new_map = static_cast<chaos::il2cpp::common::InterfaceMapEntry*>(
        CHAOS_IL2CPP_MALLOC(new_count * sizeof(chaos::il2cpp::common::InterfaceMapEntry)));
    if (new_map == nullptr) return false;

    // Copy existing entries.
    if (tv->runtime_iface_map != nullptr && tv->runtime_iface_count > 0u) {
        CHAOS_IL2CPP_MEMCPY(new_map, tv->runtime_iface_map,
                            tv->runtime_iface_count * sizeof(chaos::il2cpp::common::InterfaceMapEntry));
    }

    // Append new entry.
    new_map[tv->runtime_iface_count].iface_stable_id = iface_stable_id;
    new_map[tv->runtime_iface_count].vtable_offset   = vtable_offset;
    new_map[tv->runtime_iface_count].method_count    = method_count;

    // Free old map (if any) and install new one.
    if (tv->runtime_iface_map != nullptr) {
        CHAOS_IL2CPP_FREE(const_cast<void*>(tv->runtime_iface_map));
    }
    tv->runtime_iface_map  = new_map;
    tv->runtime_iface_count = new_count;

    // Bump iface epoch to invalidate per-thread IOC entries.
    GetIfaceEpoch().fetch_add(1u, std::memory_order_release);

    // Also bump vtable epoch so that TCVC entries for this type are
    // invalidated (the previously cached nullptr must be re-resolved).
    GetVTableEpoch().fetch_add(1u, std::memory_order_release);

    return true;
}

CHAOS_IL2CPP_UINT32 UpdateVTableSlotByMethodToken(
    CHAOS_IL2CPP_UINT32       method_token,
    void*                     new_method) noexcept
{
    if (method_token == 0u || new_method == nullptr) return 0u;

    auto& state = GetState();
    CHAOS_IL2CPP_UNIQUE_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);

    CHAOS_IL2CPP_UINT32 updated = 0u;

    auto& values = state.by_type_token.values();
    CHAOS_IL2CPP_UINT32 const vtable_count = static_cast<CHAOS_IL2CPP_UINT32>(values.size());

    TypeVTable* snapshot_buf[128];
    std::vector<TypeVTable*> snapshot_heap;
    TypeVTable** snapshot = snapshot_buf;

    if (vtable_count > 128) {
        snapshot_heap.resize(vtable_count);
        snapshot = snapshot_heap.data();
    }

    for (CHAOS_IL2CPP_UINT32 vi = 0u; vi < vtable_count; ++vi) {
        snapshot[vi] = const_cast<TypeVTable*>(values[vi].second);
    }

    for (CHAOS_IL2CPP_UINT32 vi = 0u; vi < vtable_count; ++vi) {
        auto* vtable_ptr = snapshot[vi];
        if (vtable_ptr == nullptr || vtable_ptr->slots == nullptr) continue;

        auto* mutable_slots = const_cast<VTableSlot*>(vtable_ptr->slots);
        for (CHAOS_IL2CPP_UINT32 i = 0u; i < vtable_ptr->slot_count; ++i) {
            if (mutable_slots[i].method_token == method_token) {
                mutable_slots[i].method_pointer = new_method;

                // Also update the flat vtable array at the same index.
                if (vtable_ptr->vtable_array != nullptr && i < vtable_ptr->vtable_length) {
                    const_cast<void**>(vtable_ptr->vtable_array)[i] = new_method;
                }
                ++updated;
            }
        }
    }

    // ── TCVC: Bump global epoch to invalidate per-thread caches ─────
    if (updated > 0u) {
        GetVTableEpoch().fetch_add(1u, std::memory_order_release);
    }

    return updated;
}

void* FindMethodPointerByMethodToken(
    CHAOS_IL2CPP_UINT32       method_token) noexcept
{
    if (method_token == 0u) return nullptr;

    auto& state = GetState();
    CHAOS_IL2CPP_SHARED_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);

    auto& values = state.by_type_token.values();
    CHAOS_IL2CPP_UINT32 const vtable_count = static_cast<CHAOS_IL2CPP_UINT32>(values.size());

    TypeVTable* snapshot_buf[128];
    std::vector<TypeVTable*> snapshot_heap;
    TypeVTable** snapshot = snapshot_buf;

    if (vtable_count > 128) {
        snapshot_heap.resize(vtable_count);
        snapshot = snapshot_heap.data();
    }

    for (CHAOS_IL2CPP_UINT32 vi = 0u; vi < vtable_count; ++vi) {
        snapshot[vi] = const_cast<TypeVTable*>(values[vi].second);
    }

    for (CHAOS_IL2CPP_UINT32 vi = 0u; vi < vtable_count; ++vi) {
        auto* vtable = snapshot[vi];
        if (vtable == nullptr || vtable->slots == nullptr) continue;

        for (CHAOS_IL2CPP_UINT32 i = 0u; i < vtable->slot_count; ++i) {
            if (vtable->slots[i].method_token == method_token) {
                return vtable->slots[i].method_pointer;
            }
        }
    }

    return nullptr;
}

void UnregisterTypeVTable(CHAOS_IL2CPP_UINT32 type_token) noexcept {
    if (type_token == 0u) return;

    auto& state = GetState();
    CHAOS_IL2CPP_UNIQUE_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);

    auto it = state.by_type_token.find(type_token);
    if (it == state.by_type_token.end()) return;

    const TypeVTable* tv = it->second;

    // Remove from secondary index.
    if (tv->stable_id != 0u) {
        state.by_stable_id.erase(tv->stable_id);
        state.flat_vtables.erase(tv->stable_id);
        state.flat_lengths.erase(tv->stable_id);
    }

    state.by_type_token.erase(it);

    // Free heap-allocated memory (but not .rodata / caller-owned).
    if (tv->slots != nullptr && tv->_owns_slots) CHAOS_IL2CPP_FREE(const_cast<VTableSlot*>(tv->slots));
    if (tv->vtable_array != nullptr && tv->_owns_vtable_array) CHAOS_IL2CPP_FREE(const_cast<void**>(tv->vtable_array));
    if (tv->runtime_iface_map != nullptr) CHAOS_IL2CPP_FREE(const_cast<void*>(tv->runtime_iface_map));
    CHAOS_IL2CPP_FREE(const_cast<TypeVTable*>(tv));
}

void UnregisterTypeVTableByStableId(CHAOS_IL2CPP_UINT64 stable_id) noexcept {
    if (stable_id == 0u) return;

    auto& state = GetState();
    CHAOS_IL2CPP_UNIQUE_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);

    auto it = state.by_stable_id.find(stable_id);
    if (it == state.by_stable_id.end()) return;

    const TypeVTable* tv = it->second;

    state.by_type_token.erase(tv->type_token);
    state.by_stable_id.erase(it);
    state.flat_vtables.erase(stable_id);
    state.flat_lengths.erase(stable_id);

    if (tv->slots != nullptr && tv->_owns_slots) CHAOS_IL2CPP_FREE(const_cast<VTableSlot*>(tv->slots));
    if (tv->vtable_array != nullptr && tv->_owns_vtable_array) CHAOS_IL2CPP_FREE(const_cast<void**>(tv->vtable_array));
    if (tv->runtime_iface_map != nullptr) CHAOS_IL2CPP_FREE(const_cast<void*>(tv->runtime_iface_map));
    CHAOS_IL2CPP_FREE(const_cast<TypeVTable*>(tv));
}

bool RegisterRuntimeVTable(
    TypeInfoHandle               type,
    TypeInfoHandle               base_type,
    CHAOS_IL2CPP_UINT32         slot_count,
    const VTableSlot*           slots) noexcept
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
    vtable->type_shape      = 0;
    vtable->_owns_slots        = 0;
    vtable->_owns_vtable_array = 0;
    vtable->_pad[0] = vtable->_pad[1] = 0;

    auto& state = GetState();
    CHAOS_IL2CPP_UNIQUE_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);

    if (state.by_type_token.count(type_token)) {
        CHAOS_IL2CPP_FREE(vtable);
        return true;  // idempotent
    }

    state.by_type_token[type_token] = vtable;
    return true;
}

CHAOS_IL2CPP_UINT32 GetRegisteredVTableCount() noexcept {
    auto& state = GetState();
    CHAOS_IL2CPP_SHARED_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);
    return static_cast<CHAOS_IL2CPP_UINT32>(state.by_type_token.size());
}

void ClearDomainPointers(CHAOS_IL2CPP_UINT32 domain_id) noexcept {
    if (domain_id == 0u) return;

    auto& state = GetState();
    CHAOS_IL2CPP_UNIQUE_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);

    auto& mgr = chaos::il2cpp::runtime_core::RegionManager::Instance();

    auto clear_vtable = [&](const TypeVTable* vtable) {
        if (vtable == nullptr) return;
        if (vtable->iface_map != nullptr &&
            mgr.IsInDomain(domain_id, vtable->iface_map)) {
            const_cast<TypeVTable*>(vtable)->iface_map = nullptr;
            const_cast<TypeVTable*>(vtable)->iface_count = 0u;
        }
        if (vtable->runtime_iface_map != nullptr &&
            mgr.IsInDomain(domain_id, vtable->runtime_iface_map)) {
            const_cast<TypeVTable*>(vtable)->runtime_iface_map = nullptr;
            const_cast<TypeVTable*>(vtable)->runtime_iface_count = 0u;
        }
        if (vtable->vtable_array != nullptr &&
            mgr.IsInDomain(domain_id, vtable->vtable_array)) {
            const_cast<TypeVTable*>(vtable)->vtable_array = nullptr;
            const_cast<TypeVTable*>(vtable)->vtable_length = 0u;
        }
    };

    for (auto& [stable_id, vtable] : state.by_stable_id) {
        (void)stable_id;
        clear_vtable(vtable);
    }
    for (auto& [token, vtable] : state.by_type_token) {
        (void)token;
        clear_vtable(vtable);
    }
}

}  // namespace chaos::il2cpp::vtable_registry