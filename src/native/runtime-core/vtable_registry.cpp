#include "vtable_registry.h"
#include "gc_region.h"           // RegionManager::IsInDomain
#include "reflection_query_model.h"
#include "runtime_core.h"
#include "type_registry.h"

#include <codegen_bridge.h>      // VTableDescriptorV0
#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include <chaos/profile.h>

#include <cstdlib>
#include <cstring>
#include <atomic>
#include <mutex>
#include <new>
#include <vector>

namespace chaos::il2cpp::vtable_registry {

namespace {

struct VTableRegistryState {
    CHAOS_IL2CPP_SHARED_MUTEX                                         mutex;
    // Primary key: stable_id → vtable
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP(CHAOS_IL2CPP_UINT64, const TypeVTable*)       by_stable_id;
    // Secondary key: type_token → vtable
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP(CHAOS_IL2CPP_UINT32, const TypeVTable*)       by_type_token;
    // Flat vtable arrays (for AOT codegen direct dispatch)
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP(CHAOS_IL2CPP_UINT64, const void**)            flat_vtables;
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP(CHAOS_IL2CPP_UINT64, CHAOS_IL2CPP_UINT32)     flat_lengths;
};

VTableRegistryState& GetState() {
    static VTableRegistryState s_state;
    return s_state;
}

// ── TCVC: Thread-local VTable Cache (Phase 9a) ──────────────────────
//
// ResolveVirtualMethodPointer() is called on EVERY virtual dispatch in
// the interpreter slow path.  The TCVC caches recent resolutions in a
// small thread-local ring buffer, bypassing the shared_lock + linear scan
// on cache hit when the global epoch has not changed.
//
// Global epoch is incremented by UpdateVTableSlotByMethodToken() when
// any slot is modified (method_replacement, hot-update revert).
//
// Cache entry layout:
//   key   = (instance_type_token << 32) | declared_method_token
//   value = resolved method_pointer
//   epoch = g_vtable_epoch at time of insertion

static constexpr CHAOS_IL2CPP_UINT32 kTcvcSize = 64;

struct TcvcEntry {
    CHAOS_IL2CPP_UINT64 key;
    void*               value;
    CHAOS_IL2CPP_UINT32 epoch;
};

struct TcvcState {
    TcvcEntry entries[kTcvcSize];
    CHAOS_IL2CPP_UINT32 next_index;  // ring buffer insertion point
};

// Global epoch counter.  Incremented (with release ordering) after every
// successful slot update in UpdateVTableSlotByMethodToken.  Initialized
// to 1 so that zero-initialized cache entries (epoch = 0) never match.
static std::atomic<CHAOS_IL2CPP_UINT32> g_vtable_epoch{1u};

// Per-thread cache state.  Zero-initialized on first access (all entries
// have key=0, epoch=0 — the first ResolveVirtualMethodPointer call will
// have a cache miss since g_vtable_epoch starts at 1 != 0, so a miss
// populates the cache correctly).
static thread_local TcvcState tls_tcvc{};

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
    tv->_pad[0] = tv->_pad[1] = tv->_pad[2] = 0;
    tv->iface_map      = vtd->iface_map;
    tv->iface_count    = vtd->iface_count;

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
    // to the slot position (which equals the flat vtable index).
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
    tv->_pad[0] = tv->_pad[1] = tv->_pad[2] = 0;
    tv->iface_map      = nullptr;
    tv->iface_count    = 0u;

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

CHAOS_IL2CPP_UINT32 UpdateVTableSlotByMethodToken(
    CHAOS_IL2CPP_UINT32       method_token,
    void*                     new_method) noexcept
{
    if (method_token == 0u || new_method == nullptr) return 0u;

    auto& state = GetState();
    CHAOS_IL2CPP_UNIQUE_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);

    CHAOS_IL2CPP_UINT32 updated = 0u;

    for (auto& [token, vtable_ref] : state.by_type_token) {
        (void)token;
        auto vtable_ptr = const_cast<TypeVTable*>(vtable_ref);
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
    // Release ordering ensures that all slot writes (both VTableSlot
    // method_pointer and vtable_array entries) are visible to any thread
    // that acquires the new epoch value in ResolveVirtualMethodPointer.
    if (updated > 0u) {
        g_vtable_epoch.fetch_add(1u, std::memory_order_release);
    }

    return updated;
}

void* FindMethodPointerByMethodToken(
    CHAOS_IL2CPP_UINT32       method_token) noexcept
{
    if (method_token == 0u) return nullptr;

    auto& state = GetState();
    CHAOS_IL2CPP_SHARED_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);

    for (auto& [token, vtable] : state.by_type_token) {
        (void)token;
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

    // Free heap-allocated memory.
    if (tv->slots != nullptr) CHAOS_IL2CPP_FREE(const_cast<VTableSlot*>(tv->slots));
    if (tv->vtable_array != nullptr) CHAOS_IL2CPP_FREE(const_cast<void**>(tv->vtable_array));
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

    if (tv->slots != nullptr) CHAOS_IL2CPP_FREE(const_cast<VTableSlot*>(tv->slots));
    if (tv->vtable_array != nullptr) CHAOS_IL2CPP_FREE(const_cast<void**>(tv->vtable_array));
    CHAOS_IL2CPP_FREE(const_cast<TypeVTable*>(tv));
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

    // ── TCVC: Try thread-local cache (lock-free) ──────────────────────
    const CHAOS_IL2CPP_UINT32 epoch = g_vtable_epoch.load(std::memory_order_acquire);
    const CHAOS_IL2CPP_UINT64 key = (static_cast<CHAOS_IL2CPP_UINT64>(instance_type_token) << 32u) | declared_method_token;

    for (CHAOS_IL2CPP_UINT32 i = 0u; i < kTcvcSize; ++i) {
        const auto& entry = tls_tcvc.entries[i];
        if (entry.key == key && entry.epoch == epoch) {
            // Double-check epoch: if g_vtable_epoch has changed during our scan,
            // a concurrent slot update may have made this entry stale.
            if (g_vtable_epoch.load(std::memory_order_acquire) == epoch) {
                return entry.value;
            }
            break;  // epoch changed, cache is invalid -- fall through to full resolve
        }
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
            if (vtable->iface_map != nullptr && vtable->iface_count > 0u &&
                vtable->vtable_array != nullptr) {
                const auto* iface_entries = static_cast<const chaos::il2cpp::common::InterfaceMapEntry*>(vtable->iface_map);
                for (CHAOS_IL2CPP_UINT32 ifi = 0u; ifi < vtable->iface_count; ++ifi) {
                    if (declared_method_token < iface_entries[ifi].method_count) {
                        CHAOS_IL2CPP_UINT32 vtable_slot =
                            iface_entries[ifi].vtable_offset + declared_method_token;
                        if (vtable_slot < vtable->vtable_length) {
                            result = const_cast<void*>(vtable->vtable_array[vtable_slot]);
                            goto cache_and_return;
                        }
                    }
                }
            }
        }
    }

cache_and_return:
    // ── TCVC: Write result into thread-local cache ────────────────────
    {
        CHAOS_IL2CPP_UINT32 idx = tls_tcvc.next_index;
        tls_tcvc.entries[idx].key = key;
        tls_tcvc.entries[idx].value = result;
        tls_tcvc.entries[idx].epoch = epoch;
        tls_tcvc.next_index = (idx + 1u) % kTcvcSize;
    }

    return result;
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

void ClearDomainPointers(CHAOS_IL2CPP_UINT32 domain_id) {
    if (domain_id == 0u) return;

    auto& state = GetState();
    CHAOS_IL2CPP_UNIQUE_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(state.mutex);

    auto& mgr = chaos::il2cpp::runtime_core::RegionManager::Instance();

    // Scan all vtable entries — null iface_map / vtable_array if they point
    // into the domain being unloaded.  Both by_stable_id and by_type_token
    // may contain the same vtable, so we de-dup via a small set to avoid
    // redundant work when a vtable is reachable through both indexes.
    auto clear_vtable = [&](const TypeVTable* vtable) {
        if (vtable == nullptr) return;
        if (vtable->iface_map != nullptr &&
            mgr.IsInDomain(domain_id, vtable->iface_map)) {
            const_cast<TypeVTable*>(vtable)->iface_map = nullptr;
            const_cast<TypeVTable*>(vtable)->iface_count = 0u;
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
