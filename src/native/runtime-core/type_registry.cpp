// type_registry.cpp — HotUpdate dynamic type registry
//
// Implements the runtime-registration side of the Hybrid TypeInfoHot* identity
// system: chaos_register_type() and chaos_find_type_by_stable_id().
//
// See type_info.h for the design rationale.

#include "runtime_core.h"
#include "runtime_vtable.h"
#include "type_registry.h"
#include <vtable_registry.h>
#include "module_registry.h"
#include "memory_domain.h"
#include "reflection_query_model.h"

#include <mutex>
#include <cstring>

// ── Dynamic type registry ───────────────────────────────────────
// Flat array bounded by kChaosMaxDynamicTypes (256).  A production
// runtime could replace this with a hash table for O(1) lookups.

namespace {

struct Registry {
    MethodTable* types[kChaosMaxDynamicTypes];
    CHAOS_IL2CPP_SIZE count;
    std::mutex mtx;
};

Registry& GetRegistry() noexcept
{
    // C++11 function-local static is thread-safe (compiler-generated guard).
    static Registry reg;
    return reg;
}

}  // anonymous namespace

// ── Public API ──────────────────────────────────────────────────

namespace chaos::il2cpp::runtime_core {

MethodTable* chaos_register_type(
    const char* name,
    const MethodTable* parent,
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

    // Allocate a single MethodTable (64B: hot + warm contiguous).
    auto* mt = CHAOS_IL2CPP_DOMAIN_CURRENT_NEW(MethodTable);

    // ── Hot section (32B, bit-compat with TypeInfoHot) ──
    mt->parent_mt        = parent;
    mt->vtable_array     = nullptr;
    mt->stable_id        = stable_id;
    mt->vtable_length    = 0;
    mt->warm_delta       = static_cast<uint16_t>(
        reinterpret_cast<uint8_t*>(&mt->iface_map) - reinterpret_cast<uint8_t*>(mt));
    mt->type_shape       = type_shape;
    mt->flags            = 0;

    // ── Warm section (32B) ──
    mt->iface_map          = iface_map;
    mt->runtime_iface_map  = nullptr;
    mt->iface_count        = iface_count;
    mt->runtime_iface_count = 0;
    mt->cold_delta         = 0;
    mt->_reserved          = 0;

    reg.types[reg.count] = mt;
    reg.count++;

    // Build vtable for the new HotUpdate type (copies parent vtable if registered).
    if (parent != nullptr) {
        runtime_vtable::BuildRuntimeVTable(stable_id, parent->stable_id);
    }

    if (out_stable_id) *out_stable_id = stable_id;
    return mt;
}

const MethodTable* chaos_find_type_by_stable_id(
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
    MethodTable* mt,
    CHAOS_IL2CPP_UINT64 iface_stable_id,
    CHAOS_IL2CPP_UINT32 vtable_offset,
    CHAOS_IL2CPP_UINT32 method_count) noexcept
{
    if (mt == nullptr) return false;

    // Check if already present in AOT iface_map.
    for (CHAOS_IL2CPP_UINT32 i = 0; i < mt->iface_count; ++i)
    {
        if (mt->iface_map[i].iface_stable_id == iface_stable_id)
            return true;  // already mapped by AOT
    }

    // Check if already present in runtime_iface_map.
    for (CHAOS_IL2CPP_UINT32 i = 0; i < mt->runtime_iface_count; ++i)
    {
        if (mt->runtime_iface_map[i].iface_stable_id == iface_stable_id)
            return true;  // already added
    }

    auto& reg = GetRegistry();
    std::lock_guard<std::mutex> lock(reg.mtx);

    CHAOS_IL2CPP_UINT32 newCount = mt->runtime_iface_count + 1;
    auto* newMap = static_cast<InterfaceMapEntry*>(
        CHAOS_IL2CPP_DOMAIN_CURRENT_REALLOC(
            const_cast<InterfaceMapEntry*>(mt->runtime_iface_map),
            newCount * sizeof(InterfaceMapEntry)));

    if (newMap == nullptr) return false;

    newMap[mt->runtime_iface_count].iface_stable_id = iface_stable_id;
    newMap[mt->runtime_iface_count].vtable_offset   = vtable_offset;
    newMap[mt->runtime_iface_count].method_count    = method_count;

    mt->runtime_iface_map  = newMap;
    mt->runtime_iface_count = newCount;

    // Sync with vtable_registry so that ResolveVirtualMethodPointer and
    // chaos_find_interface_offset can dispatch through this interface.
    if (mt->stable_id != 0u) {
        const auto* tv = chaos::il2cpp::vtable_registry::TryGetTypeVTableByStableId(mt->stable_id);
        if (tv != nullptr && tv->type_token != 0u) {
            chaos::il2cpp::vtable_registry::RegisterTypeVTableRuntimeInterface(
                tv->type_token, iface_stable_id, vtable_offset, method_count);
        }
    }

    return true;
}

const TypeInfoHot* TryResolveTypeInfo(TypeInfoHandle handle) noexcept
{
    if (handle == 0u || handle == TypeInfoHandle{}) return nullptr;

    // ── Path 1: Tag-encoded handles are RuntimeInstantiatedType pointers ──
    // These carry a ReflectionQueryTypeDescriptor, not a TypeInfo*, so we
    // cannot resolve to TypeInfo* from this path. The caller should compute
    // stable_id from the descriptor's subject_id_utf8 and use FindVTable().
    const auto* desc = TryDecodeReflectionQueryTypeHandle(handle);
    if (desc != nullptr) {
        return nullptr;
    }

    // ── Path 2: Module-registry handle ──
    // Encoding: [module_id:32 bits] [metadata_token:32 bits]
    uint32_t module_id = GetModuleId(handle);
    uint32_t token = GetTypeToken(handle);
    if (token != 0) {
        return LookupTypeInfoPtr(module_id != 0u ? module_id : 0u, token);
    }

    // ── Path 3: Raw metadata token or unknown encoding ──
    return nullptr;
}

}  // namespace chaos::il2cpp::runtime_core