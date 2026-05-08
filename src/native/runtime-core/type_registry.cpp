// type_registry.cpp — HotUpdate dynamic TypeInfo registry
//
// Implements the runtime-registration side of the Hybrid TypeInfoHot* identity
// system: chaos_register_type() and chaos_find_type_by_stable_id().
//
// See type_info.h for the design rationale.

#include "runtime_core.h"
#include "runtime_vtable.h"
#include "type_registry.h"
#include "module_registry.h"
#include "reflection_query_model.h"

#include <mutex>
#include <cstring>

// ── Dynamic type registry ───────────────────────────────────────
// Flat array bounded by kChaosMaxDynamicTypes (256).  A production
// runtime could replace this with a hash table for O(1) lookups.

namespace {

struct Registry {
    TypeInfoHot* types[kChaosMaxDynamicTypes];
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

TypeInfoHot* chaos_register_type(
    const char* name,
    const TypeInfoHot* parent,
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

    // Allocate TypeInfoHot + TypeInfoWarm as a contiguous pair.
    auto* hot = CHAOS_IL2CPP_NEW(TypeInfoHot);
    auto* warm = CHAOS_IL2CPP_NEW(TypeInfoWarm);

    hot->parent        = parent;
    hot->vtable_array  = nullptr;
    hot->stable_id     = stable_id;
    hot->vtable_length = 0;
    hot->warm_delta    = static_cast<uint16_t>(
        reinterpret_cast<uint8_t*>(warm) - reinterpret_cast<uint8_t*>(hot));
    hot->type_shape    = type_shape;
    hot->flags         = 0;

    warm->iface_map          = iface_map;
    warm->runtime_iface_map  = nullptr;
    warm->iface_count        = iface_count;
    warm->runtime_iface_count = 0;
    warm->cold_delta         = 0;
    warm->_reserved          = 0;

    reg.types[reg.count] = hot;
    reg.count++;

    // Build vtable for the new HotUpdate type (copies parent vtable if registered).
    if (parent != nullptr) {
        runtime_vtable::BuildRuntimeVTable(stable_id, parent->stable_id);
    }

    if (out_stable_id) *out_stable_id = stable_id;
    return hot;
}

const TypeInfoHot* chaos_find_type_by_stable_id(
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
    TypeInfoHot* ti,
    CHAOS_IL2CPP_UINT64 iface_stable_id,
    CHAOS_IL2CPP_UINT32 vtable_offset,
    CHAOS_IL2CPP_UINT32 method_count) noexcept
{
    if (ti == nullptr) return false;

    auto* warm = GetWarmPtr(ti);

    // Check if already present in AOT iface_map.
    for (CHAOS_IL2CPP_UINT32 i = 0; i < warm->iface_count; ++i)
    {
        if (warm->iface_map[i].iface_stable_id == iface_stable_id)
            return true;  // already mapped by AOT
    }

    // Check if already present in runtime_iface_map.
    for (CHAOS_IL2CPP_UINT32 i = 0; i < warm->runtime_iface_count; ++i)
    {
        if (warm->runtime_iface_map[i].iface_stable_id == iface_stable_id)
            return true;  // already added
    }

    auto& reg = GetRegistry();
    std::lock_guard<std::mutex> lock(reg.mtx);

    CHAOS_IL2CPP_UINT32 newCount = warm->runtime_iface_count + 1;
    auto* newMap = static_cast<InterfaceMapEntry*>(
        CHAOS_IL2CPP_REALLOC(
            const_cast<InterfaceMapEntry*>(warm->runtime_iface_map),
            newCount * sizeof(InterfaceMapEntry)));

    if (newMap == nullptr) return false;

    newMap[warm->runtime_iface_count].iface_stable_id = iface_stable_id;
    newMap[warm->runtime_iface_count].vtable_offset   = vtable_offset;
    newMap[warm->runtime_iface_count].method_count    = method_count;

    warm->runtime_iface_map  = newMap;
    warm->runtime_iface_count = newCount;
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
        const auto* mod = LookupModule(module_id != 0u ? module_id : 0u);
        if (mod != nullptr && !mod->tombstone && mod->type_flags != nullptr) {
            uint32_t idx = TokenToIndex(token);
            if (idx < mod->type_count && mod->type_info_ptrs != nullptr) {
                return mod->type_info_ptrs[idx];
            }
        }
    }

    // ── Path 3: Raw metadata token or unknown encoding ──
    return nullptr;
}

}  // namespace chaos::il2cpp::runtime_core
