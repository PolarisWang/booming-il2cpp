#include "method_replacement.h"

#include <chaos/native_types.h>

#include <mutex>
#include <unordered_map>

namespace chaos::il2cpp::method_replacement {

namespace {

CHAOS_IL2CPP_SHARED_MUTEX g_method_replacement_mutex;
CHAOS_IL2CPP_UNORDERED_MAP(CHAOS_IL2CPP_UINT32, MethodReplacementEntry) g_method_replacements;

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
    return true;
}

bool Revert(CHAOS_IL2CPP_UINT32 method_token) {
    CHAOS_IL2CPP_UNIQUE_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(g_method_replacement_mutex);
    return g_method_replacements.erase(method_token) > 0u;
}

void RevertAll() {
    CHAOS_IL2CPP_UNIQUE_LOCK(CHAOS_IL2CPP_SHARED_MUTEX) lock(g_method_replacement_mutex);
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
