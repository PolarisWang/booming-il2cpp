// type_registry.cpp — HotUpdate dynamic TypeInfo registry
//
// Implements the runtime-registration side of the Hybrid TypeInfo* identity
// system: chaos_register_type() and chaos_find_type_by_stable_id().
//
// See src/native/common/chaos/type_info.h for the design rationale.

#include "runtime_core.h"

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

TypeInfo* chaos_register_type(
    const char* name,
    const TypeInfo* parent,
    CHAOS_IL2CPP_UINT8 type_shape,
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

    reg.types[reg.count] = ti;
    reg.count++;

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
