#include "vtable_registry_internal.h"
#include "reflection_query_model.h"
#include "runtime_core.h"

#include <chaos/profile.h>
#include <cstdlib>
#include <cstring>

namespace chaos::il2cpp::vtable_registry {
using namespace internal;

// ── ScanIfaceMapForMethod: Shared helper for iface_map/runtime_iface_map scan ──
// Returns the method_pointer if declared_method_token falls within any
// interface's method range, or nullptr if not found.  Defined here (at
// vtable_registry:: scope) and called only from ResolveVirtualMethodPointer.
void* ScanIfaceMapForMethod(
    const void* iface_map_raw,
    CHAOS_IL2CPP_UINT32 iface_count,
    const void* const* vtable_array,
    CHAOS_IL2CPP_UINT32 vtable_length,
    CHAOS_IL2CPP_UINT32 declared_method_token) noexcept
{
    if (iface_map_raw == nullptr || iface_count == 0u || vtable_array == nullptr)
        return nullptr;
    const auto* entries = static_cast<const chaos::il2cpp::common::InterfaceMapEntry*>(iface_map_raw);
    for (CHAOS_IL2CPP_UINT32 ifi = 0u; ifi < iface_count; ++ifi) {
        if (declared_method_token < entries[ifi].method_count) {
            CHAOS_IL2CPP_UINT32 slot = entries[ifi].vtable_offset + declared_method_token;
            if (slot < vtable_length)
                return const_cast<void*>(vtable_array[slot]);
        }
    }
    return nullptr;
}

const TypeVTable* TryGetTypeVTable(CHAOS_IL2CPP_UINT32 type_token) noexcept {
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

const TypeVTable* TryGetTypeVTableByStableId(CHAOS_IL2CPP_UINT64 stable_id) noexcept {
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
    CHAOS_IL2CPP_UINT32 declared_method_token) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("ResolveVirtualMethodPointer");
    if (instance_type_token == 0u) {
        return nullptr;
    }

    // ── TCVC: Try thread-local cache (lock-free) ──────────────────────
    const CHAOS_IL2CPP_UINT32 epoch = GetVTableEpoch().load(std::memory_order_acquire);
    const CHAOS_IL2CPP_UINT64 key = (static_cast<CHAOS_IL2CPP_UINT64>(instance_type_token) << 32u) | declared_method_token;

    // Direct-mapped: single entry per (key & mask) bucket.
    const CHAOS_IL2CPP_UINT32 idx = static_cast<CHAOS_IL2CPP_UINT32>(key) & kTcvcMask;
    const auto& entry = tls_tcvc.entries[idx];
    if (entry.key == key && entry.epoch == epoch) {
        // Double-check epoch: if GetVTableEpoch() has changed since our
        // acquire-load above, a concurrent slot update may have made this
        // entry stale.  (On x86 aqcuire is free; this check is ~0 cost.)
        if (GetVTableEpoch().load(std::memory_order_acquire) == epoch) {
            return entry.value;
        }
        // epoch changed → cache invalid, fall through to full resolve
    }

    // ── Cache miss: full resolve under shared lock ────────────────────
    auto& state = GetState();
    CHAOS_IL2CPP_SHARED_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);

    void* result = nullptr;

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
                    result = vtable->slots[i].method_pointer;
                    goto cache_and_return;
                }
            }

            // Move to base type
            current_token = vtable->base_token;
        }
    }

    // Base chain walk failed (or skipped) -- try interface vtable map.
    {
        auto it = state.by_type_token.find(instance_type_token);
        if (it != state.by_type_token.end()) {
            const TypeVTable* vtable = it->second;
            if (vtable->vtable_array == nullptr) goto cache_and_return;

            // ── Interface method dispatch (strategy 1) ──
            // declared_method_token is a full metadata token (e.g. 0x700).
            // Scan all registered interface vtables to find which one
            // declares this method_token, then resolve through the
            // instance type's iface_map using the relative slot index.
            {
                CHAOS_IL2CPP_UINT64 iface_stable_id = 0u;
                CHAOS_IL2CPP_UINT32 relative_index = ~0u;

                for (const auto& [token, ivt] : state.by_type_token) {
                    if (ivt->type_shape == chaos::il2cpp::common::chaos_type_shape_interface) {
                        for (CHAOS_IL2CPP_UINT32 si = 0u; si < ivt->slot_count; ++si) {
                            if (ivt->slots[si].method_token == declared_method_token) {
                                iface_stable_id = ivt->stable_id;
                                relative_index = si;
                                break;
                            }
                        }
                        if (iface_stable_id != 0u) break;
                    }
                }

                if (iface_stable_id != 0u && relative_index != ~0u) {
                    // ── Try AOT iface_map ──
                    if (vtable->iface_map != nullptr) {
                        const auto* entries = static_cast<const chaos::il2cpp::common::InterfaceMapEntry*>(vtable->iface_map);
                        for (CHAOS_IL2CPP_UINT32 ifi = 0u; ifi < vtable->iface_count; ++ifi) {
                            if (entries[ifi].iface_stable_id == iface_stable_id &&
                                relative_index < entries[ifi].method_count) {
                                CHAOS_IL2CPP_UINT32 slot = entries[ifi].vtable_offset + relative_index;
                                if (slot < vtable->vtable_length) {
                                    result = const_cast<void*>(vtable->vtable_array[slot]);
                                    goto cache_and_return;
                                }
                            }
                        }
                    }

                    // ── Try runtime_iface_map (hot-update additions) ──
                    if (vtable->runtime_iface_map != nullptr) {
                        const auto* rentries = static_cast<const chaos::il2cpp::common::InterfaceMapEntry*>(vtable->runtime_iface_map);
                        for (CHAOS_IL2CPP_UINT32 ifi = 0u; ifi < vtable->runtime_iface_count; ++ifi) {
                            if (rentries[ifi].iface_stable_id == iface_stable_id &&
                                relative_index < rentries[ifi].method_count) {
                                CHAOS_IL2CPP_UINT32 slot = rentries[ifi].vtable_offset + relative_index;
                                if (slot < vtable->vtable_length) {
                                    result = const_cast<void*>(vtable->vtable_array[slot]);
                                    goto cache_and_return;
                                }
                            }
                        }
                    }
                }
            }

            // ── Interface method dispatch (strategy 2) ──
            // declared_method_token is already an interface-relative
            // method index (e.g. 0 for slot 0).  Used by codegen paths
            // where the method token is not the metadata token but the
            // slot index within the interface vtable.
            if (result == nullptr) {
                result = ScanIfaceMapForMethod(
                    vtable->iface_map, vtable->iface_count,
                    vtable->vtable_array, vtable->vtable_length,
                    declared_method_token);
                if (result != nullptr) goto cache_and_return;

                result = ScanIfaceMapForMethod(
                    vtable->runtime_iface_map, vtable->runtime_iface_count,
                    vtable->vtable_array, vtable->vtable_length,
                    declared_method_token);
                if (result != nullptr) goto cache_and_return;
            }
        }
    }

cache_and_return:
    // ── TCVC: Write result into thread-local cache ────────────────────
    {
        CHAOS_IL2CPP_UINT32 idx = static_cast<CHAOS_IL2CPP_UINT32>(key) & kTcvcMask;
        tls_tcvc.entries[idx].key = key;
        tls_tcvc.entries[idx].value = result;
        tls_tcvc.entries[idx].epoch = epoch;
    }

    return result;
}

void* ResolveVirtualMethodPointerByHandle(
    TypeInfoHandle               instance_type,
    CHAOS_IL2CPP_UINT32         declared_method_token) noexcept
{
    if (instance_type == 0) return nullptr;
    const auto* desc = chaos::il2cpp::runtime_core::TryDecodeReflectionQueryTypeHandle(instance_type);
    if (desc == nullptr) return nullptr;
    return ResolveVirtualMethodPointer(desc->metadata_token, declared_method_token);
}

CHAOS_IL2CPP_UINT32 chaos_find_interface_offset(
    CHAOS_IL2CPP_UINT32       type_token,
    CHAOS_IL2CPP_UINT64       iface_stable_id) noexcept
{
    if (type_token == 0u || iface_stable_id == 0u)
        return CHAOS_IL2CPP_UINT32_MAX;

    // ── IOC: Try thread-local cache (lock-free) ──────────────────────
    const CHAOS_IL2CPP_UINT32 iface_epoch = GetIfaceEpoch().load(std::memory_order_acquire);
    const CHAOS_IL2CPP_UINT64 ioc_key = (static_cast<CHAOS_IL2CPP_UINT64>(type_token) << 32u)
                                       | (static_cast<CHAOS_IL2CPP_UINT32>(iface_stable_id & 0xFFFFFFFF));

    // Direct-mapped: single entry per (key & mask) bucket.
    const CHAOS_IL2CPP_UINT32 ioc_idx = static_cast<CHAOS_IL2CPP_UINT32>(ioc_key) & kIocMask;
    const auto& entry = tls_ioc.entries[ioc_idx];
    if (entry.key == ioc_key && entry.epoch == iface_epoch) {
        // Double-check epoch.
        if (GetIfaceEpoch().load(std::memory_order_acquire) == iface_epoch) {
            return entry.vtable_offset;
        }
    }

    // ── Cache miss: full lookup under shared lock ────────────────────
    auto& state = GetState();
    CHAOS_IL2CPP_SHARED_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);

    auto it = state.by_type_token.find(type_token);
    if (it == state.by_type_token.end())
        return CHAOS_IL2CPP_UINT32_MAX;

    const TypeVTable* tv = it->second;

    // Check AOT iface_map.
    if (tv->iface_map != nullptr) {
        const auto* entries = static_cast<const chaos::il2cpp::common::InterfaceMapEntry*>(tv->iface_map);
        for (CHAOS_IL2CPP_UINT32 i = 0u; i < tv->iface_count; ++i) {
            if (entries[i].iface_stable_id == iface_stable_id) {
                // IOC write-back (direct-mapped).
                CHAOS_IL2CPP_UINT32 idx = static_cast<CHAOS_IL2CPP_UINT32>(ioc_key) & kIocMask;
                tls_ioc.entries[idx].key = ioc_key;
                tls_ioc.entries[idx].vtable_offset = entries[i].vtable_offset;
                tls_ioc.entries[idx].method_count = entries[i].method_count;
                tls_ioc.entries[idx].epoch = iface_epoch;
                return entries[i].vtable_offset;
            }
        }
    }

    // Check runtime_iface_map.
    if (tv->runtime_iface_map != nullptr) {
        const auto* entries = static_cast<const chaos::il2cpp::common::InterfaceMapEntry*>(tv->runtime_iface_map);
        for (CHAOS_IL2CPP_UINT32 i = 0u; i < tv->runtime_iface_count; ++i) {
            if (entries[i].iface_stable_id == iface_stable_id) {
                CHAOS_IL2CPP_UINT32 idx = static_cast<CHAOS_IL2CPP_UINT32>(ioc_key) & kIocMask;
                tls_ioc.entries[idx].key = ioc_key;
                tls_ioc.entries[idx].vtable_offset = entries[i].vtable_offset;
                tls_ioc.entries[idx].method_count = entries[i].method_count;
                tls_ioc.entries[idx].epoch = iface_epoch;
                return entries[i].vtable_offset;
            }
        }
    }

    return CHAOS_IL2CPP_UINT32_MAX;
}

}  // namespace chaos::il2cpp::vtable_registry
