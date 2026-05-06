/// Test stub — replaces vtable_registry.cpp for standalone interpreter tests.
/// Provides minimal implementations of the unified vtable registry API
/// without pulling in the full runtime-core dependency chain.
#include "vtable_registry.h"
#include "layout_engine.h"   // LayoutEngine stub

#include <cstdlib>
#include <cstring>
#include <unordered_map>
#include <vector>

namespace chaos::il2cpp::vtable_registry {

namespace {

// Minimal registry: type_token → TypeVTable
struct Registry {
    std::unordered_map<CHAOS_IL2CPP_UINT32, const TypeVTable*> by_token;
    std::unordered_map<CHAOS_IL2CPP_UINT64, const void**> flat_vtables;
    std::unordered_map<CHAOS_IL2CPP_UINT64, CHAOS_IL2CPP_UINT32> flat_lengths;
};

Registry& GetReg() {
    static Registry r;
    return r;
}

}  // anonymous namespace

bool RegisterTypeVTable(const TypeVTable* vtable) {
    if (vtable == nullptr || vtable->type_token == 0u || vtable->slots == nullptr) {
        return false;
    }
    auto& reg = GetReg();
    if (reg.by_token.count(vtable->type_token)) {
        return true;  // idempotent
    }
    reg.by_token[vtable->type_token] = vtable;
    return true;
}

bool RegisterRuntimeVTable(
    TypeInfoHandle /*type*/,
    TypeInfoHandle /*base_type*/,
    CHAOS_IL2CPP_UINT32 /*slot_count*/,
    const VTableSlot* /*slots*/)
{
    return false;  // not exercised by interpreter tests
}

void RegisterVTableArray(CHAOS_IL2CPP_UINT64 stable_id,
                          const void** vtable,
                          CHAOS_IL2CPP_UINT32 length) noexcept {
    if (stable_id == 0u || vtable == nullptr || length == 0u) return;
    auto& reg = GetReg();
    reg.flat_vtables[stable_id] = vtable;
    reg.flat_lengths[stable_id] = length;
}

const void** FindVTable(CHAOS_IL2CPP_UINT64 stable_id) noexcept {
    auto& reg = GetReg();
    auto it = reg.flat_vtables.find(stable_id);
    return (it != reg.flat_vtables.end()) ? it->second : nullptr;
}

CHAOS_IL2CPP_UINT32 FindVTableLength(CHAOS_IL2CPP_UINT64 stable_id) noexcept {
    auto& reg = GetReg();
    auto it = reg.flat_lengths.find(stable_id);
    return (it != reg.flat_lengths.end()) ? it->second : 0u;
}

const void** BuildRuntimeVTable(CHAOS_IL2CPP_UINT64 /*type_stable_id*/,
                                 CHAOS_IL2CPP_UINT64 /*base_stable_id*/) noexcept {
    return nullptr;  // not exercised by interpreter tests
}

const TypeVTable* TryGetTypeVTableByStableId(CHAOS_IL2CPP_UINT64 /*stable_id*/) {
    return nullptr;  // not exercised by interpreter tests
}

void* ResolveVirtualMethodPointer(
    CHAOS_IL2CPP_UINT32 instance_type_token,
    CHAOS_IL2CPP_UINT32 declared_method_token)
{
    auto& reg = GetReg();

    // Walk the inheritance chain upward.
    CHAOS_IL2CPP_UINT32 current = instance_type_token;
    while (current != 0u) {
        auto it = reg.by_token.find(current);
        if (it == reg.by_token.end()) {
            break;
        }
        const TypeVTable* vt = it->second;
        for (CHAOS_IL2CPP_UINT32 si = 0u; si < vt->slot_count; ++si) {
            if (vt->slots[si].method_token == declared_method_token) {
                return vt->slots[si].method_pointer;
            }
        }
        current = vt->base_token;
    }
    return nullptr;
}

void* ResolveVirtualMethodPointerByHandle(
    TypeInfoHandle /*instance_type*/,
    CHAOS_IL2CPP_UINT32 /*declared_method_token*/)
{
    return nullptr;  // not exercised by interpreter tests
}

CHAOS_IL2CPP_UINT32 GetRegisteredVTableCount() {
    return static_cast<CHAOS_IL2CPP_UINT32>(GetReg().by_token.size());
}

}  // namespace chaos::il2cpp::vtable_registry

// ════════════════════════════════════════════════════════════════════════════
// LayoutEngine stub — needed by token_resolver.cpp for field offset resolution.
// The full LayoutEngine requires runtime_core infrastructure; this stub
// returns nullptr (no layout available), which is sufficient for tests that
// don't exercise struct field access with generic type parameters.
// ════════════════════════════════════════════════════════════════════════════

namespace chaos::il2cpp::layout {

const TypeLayout* LayoutEngine::GetOrComputeLayout(
    TypeInfoHandle /*closed_type*/,
    const TypeInfoHandle* /*type_args*/,
    CHAOS_IL2CPP_UINT32 /*arg_count*/)
{
    return nullptr;  // stub: no layout available
}

}  // namespace chaos::il2cpp::layout
