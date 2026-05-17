#include "method_replacement.h"

#include <chaos/native_types.h>

#include <mutex>
#include <chaos/unordered_dense.h>

// Forward declaration: vtable_registry APIs used for VTable slot sync.
// Defined in runtime-core/vtable_registry.h.  We include the header here
// to call UpdateVTableSlotByMethodToken and FindMethodPointerByMethodToken.
#include <vtable_registry.h>

// Hotpatch dispatch entry activation — links method_replacement into
// the per-call-site hotpatch dispatch mechanism so that after Register(),
// all dispatch points that check HotpatchIsActive() will route through
// the replacement thunk instead of calling the original AOT native code.
#include <hotpatch_table.h>

namespace chaos::il2cpp::method_replacement {

namespace {

CHAOS_IL2CPP_SHARED_MUTEX g_method_replacement_mutex;
CHAOS_IL2CPP_UNORDERED_DENSE_MAP(CHAOS_IL2CPP_UINT32, MethodReplacementEntry) g_method_replacements;

}  // namespace

bool Register(CHAOS_IL2CPP_UINT32 method_token, void* thunk) {
    if (method_token == 0u || thunk == nullptr) {
        return false;
    }

    CHAOS_IL2CPP_UNIQUE_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(g_method_replacement_mutex);
    auto& entry = g_method_replacements[method_token];
    entry.method_token = method_token;
    entry.replacement_thunk = thunk;
    entry.active = true;

    // Capture the original pointer on first registration so Revert() can restore it.
    if (entry.original_pointer == nullptr) {
        entry.original_pointer =
            chaos::il2cpp::vtable_registry::FindMethodPointerByMethodToken(method_token);
    }

    // Sync VTable slots: update all TypeVTables that reference this method_token.
    lock.unlock();
    chaos::il2cpp::vtable_registry::UpdateVTableSlotByMethodToken(method_token, thunk);

    // Activate the hotpatch dispatch entry so per-call-site dispatch points
    // (the s_hotpatch_entries pattern-aware branches emitted by codegen) route
    // through the replacement instead of calling the original AOT native code.
    {
        auto& registry = chaos::il2cpp::runtime_core::GetHotpatchNameRegistry();
        uint64_t composite = registry.FindToken(method_token);
        if (composite != 0) {
            uint32_t mod_id = chaos::il2cpp::runtime_core::ExtractModuleId(composite);
            uint32_t slot = registry.TokenToSlot(mod_id, method_token);
            if (slot != ~0u) {
                registry.SetPatchedBySlot(mod_id, slot, true, thunk);
            }
        }
    }

    return true;
}

bool Revert(CHAOS_IL2CPP_UINT32 method_token) {
    CHAOS_IL2CPP_UNIQUE_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(g_method_replacement_mutex);

    auto it = g_method_replacements.find(method_token);
    if (it == g_method_replacements.end()) return false;

    // Restore original pointer in all VTable slots before erasing the entry.
    void* original = it->second.original_pointer;
    if (original != nullptr) {
        lock.unlock();
        chaos::il2cpp::vtable_registry::UpdateVTableSlotByMethodToken(method_token, original);
        lock.lock();
    }

    // Deactivate hotpatch dispatch entry even if no vtable was set up
    // (e.g., early bootstrap or test scenario without full vtable_registry).
    {
        auto& registry = chaos::il2cpp::runtime_core::GetHotpatchNameRegistry();
        uint64_t composite = registry.FindToken(method_token);
        if (composite != 0) {
            uint32_t mod_id = chaos::il2cpp::runtime_core::ExtractModuleId(composite);
            uint32_t slot = registry.TokenToSlot(mod_id, method_token);
            if (slot != ~0u) {
                registry.SetPatchedBySlot(mod_id, slot, false, nullptr);
            }
        }
    }

    g_method_replacements.erase(it);
    return true;
}

void RevertAll() {
    CHAOS_IL2CPP_UNIQUE_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(g_method_replacement_mutex);
    // Restore all original pointers before clearing.
    for (auto& [method_token, entry] : g_method_replacements) {
        if (entry.original_pointer != nullptr) {
            lock.unlock();
            chaos::il2cpp::vtable_registry::UpdateVTableSlotByMethodToken(
                method_token, entry.original_pointer);
            lock.lock();
        }

        // Deactivate hotpatch dispatch entry unconditionally.
        {
            auto& registry = chaos::il2cpp::runtime_core::GetHotpatchNameRegistry();
            uint64_t composite = registry.FindToken(method_token);
            if (composite != 0) {
                uint32_t mod_id = chaos::il2cpp::runtime_core::ExtractModuleId(composite);
                uint32_t slot = registry.TokenToSlot(mod_id, method_token);
                if (slot != ~0u) {
                    registry.SetPatchedBySlot(mod_id, slot, false, nullptr);
                }
            }
        }
    }
    g_method_replacements.clear();
}

void* Resolve(CHAOS_IL2CPP_UINT32 method_token) {
    CHAOS_IL2CPP_SHARED_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(g_method_replacement_mutex);
    const auto it = g_method_replacements.find(method_token);
    if (it == g_method_replacements.end() || !it->second.active) {
        return nullptr;
    }

    return it->second.replacement_thunk;
}

CHAOS_IL2CPP_UINT32 ActiveCount() {
    CHAOS_IL2CPP_SHARED_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(g_method_replacement_mutex);
    return static_cast<CHAOS_IL2CPP_UINT32>(g_method_replacements.size());
}

}  // namespace chaos::il2cpp::method_replacement
