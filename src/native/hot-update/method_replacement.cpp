#include "method_replacement.h"

#include <chaos/native_types.h>

#include <mutex>
#include <chaos/unordered_dense.h>

// Forward declaration: vtable_registry APIs used for VTable slot sync.
// Defined in runtime-core/vtable_registry.h.  We include the header here
// to call UpdateVTableSlotByMethodToken and FindMethodPointerByMethodToken.
#include <vtable_registry.h>

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
