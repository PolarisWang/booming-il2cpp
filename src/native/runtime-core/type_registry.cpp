// type_registry.cpp — HotUpdate dynamic TypeInfo registry
//
// Implements the runtime-registration side of the Hybrid TypeInfo* identity
// system: chaos_register_type() and chaos_find_type_by_stable_id().
//
// See type_info.h for the design rationale.

#include "runtime_core.h"
#include "runtime_vtable.h"
#include "type_registry.h"

#include <mutex>

// ── Dynamic type registry ───────────────────────────────────────
// Flat array bounded by kChaosMaxDynamicTypes (256).  A production
// runtime could replace this with a hash table for O(1) lookups.

namespace {

struct Registry {
    TypeInfo* types[kChaosMaxDynamicTypes];
    CHAOS_IL2CPP_SIZE count;
    std::mutex mtx;
};

Registry& GetRegistry() noexcept
{
    // C++11 function-local static is thread-safe (compiler-generated guard).
    // Zero-initialises POD members, calls default ctor for std::mutex.
    static Registry reg;
    return reg;
}

}  // anonymous namespace

// ── Public API ──────────────────────────────────────────────────

namespace chaos::il2cpp::runtime_core {

TypeInfo* chaos_register_type(
    const char* name,
    const TypeInfo* parent,
    CHAOS_IL2CPP_UINT8 type_shape,
    const InterfaceMapEntry* iface_map,
    CHAOS_IL2CPP_UINT32 iface_count,
    CHAOS_IL2CPP_UINT64* out_stable_id) noexcept
{
    if (name == nullptr)
    {
        if (out_stable_id) *out_stable_id = 0;
        return nullptr;
    }

    const CHAOS_IL2CPP_UINT64 stable_id = chaos_compute_type_stable_id(name);
    auto& reg = GetRegistry();

    // Optimistic lock-free check (fast path when registry is full).
    if (reg.count >= kChaosMaxDynamicTypes)
    {
        if (out_stable_id) *out_stable_id = 0;
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(reg.mtx);

    // Re-check under lock.
    if (reg.count >= kChaosMaxDynamicTypes)
    {
        if (out_stable_id) *out_stable_id = 0;
        return nullptr;
    }

    auto* ti = CHAOS_IL2CPP_NEW(TypeInfo);
    ti->parent     = parent;
    ti->stable_id  = stable_id;
    ti->type_shape = type_shape;
    ti->iface_map  = iface_map;
    ti->iface_count = iface_count;

    reg.types[reg.count] = ti;
    reg.count++;

    // Build vtable for the new HotUpdate type (copies parent vtable if registered).
    if (parent != nullptr) {
        runtime_vtable::BuildRuntimeVTable(stable_id, parent->stable_id);
    }

    if (out_stable_id) *out_stable_id = stable_id;
    return ti;
}

const TypeInfo* chaos_find_type_by_stable_id(
    CHAOS_IL2CPP_UINT64 stable_id) noexcept
{
    const auto& reg = GetRegistry();
    const CHAOS_IL2CPP_SIZE count = reg.count;  // snapshot (grow-safe)

    for (CHAOS_IL2CPP_SIZE i = 0; i < count; ++i)
    {
        if (reg.types[i]->stable_id == stable_id)
        {
            return reg.types[i];
        }
    }
    return nullptr;
}


bool ChaosTypeAddInterface(
    TypeInfo* ti,
    CHAOS_IL2CPP_UINT64 iface_stable_id,
    CHAOS_IL2CPP_UINT32 vtable_offset,
    CHAOS_IL2CPP_UINT32 method_count) noexcept
{
    if (ti == nullptr) return false;

    // Check if already present in AOT iface_map.
    for (CHAOS_IL2CPP_UINT32 i = 0; i < ti->iface_count; ++i)
    {
        if (ti->iface_map[i].iface_stable_id == iface_stable_id)
            return true;  // already mapped by AOT
    }

    // Check if already present in runtime_iface_map.
    for (CHAOS_IL2CPP_UINT32 i = 0; i < ti->runtime_iface_count; ++i)
    {
        if (ti->runtime_iface_map[i].iface_stable_id == iface_stable_id)
            return true;  // already added
    }

    auto& reg = GetRegistry();
    std::lock_guard<std::mutex> lock(reg.mtx);

    CHAOS_IL2CPP_UINT32 newCount = ti->runtime_iface_count + 1;
    auto* newMap = static_cast<InterfaceMapEntry*>(
        CHAOS_IL2CPP_REALLOC(
            const_cast<InterfaceMapEntry*>(ti->runtime_iface_map),
            newCount * sizeof(InterfaceMapEntry)));

    if (newMap == nullptr) return false;

    newMap[ti->runtime_iface_count].iface_stable_id = iface_stable_id;
    newMap[ti->runtime_iface_count].vtable_offset   = vtable_offset;
    newMap[ti->runtime_iface_count].method_count    = method_count;

    ti->runtime_iface_map  = newMap;
    ti->runtime_iface_count = newCount;
    return true;
}

}  // namespace chaos::il2cpp::runtime_core
