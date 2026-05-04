/// Test stub — replaces vtable_registry.cpp for standalone interpreter tests.
/// Provides the minimal implementation of RegisterTypeVTable and
/// ResolveVirtualMethodPointer without pulling in the full runtime-core
/// dependency chain (codegen_bridge, reflection_query_model, etc.).
#include "vtable_registry.h"
#include "layout_engine.h"   // LayoutEngine stub

#include <cstdlib>
#include <unordered_map>
#include <vector>

namespace chaos::il2cpp::vtable_registry {

namespace {

// Minimal registry: type_token → TypeVTable
struct Registry {
    std::unordered_map<CHAOS_IL2CPP_UINT32, const TypeVTable*> by_token;
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
