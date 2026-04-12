#include "method_replacement.h"

#include <mutex>
#include <unordered_map>

namespace chaos::il2cpp::method_replacement {

namespace {

std::mutex g_method_replacement_mutex;
std::unordered_map<uint32_t, MethodReplacementEntry> g_method_replacements;

}  // namespace

bool Register(uint32_t method_token, void* thunk) {
    if (method_token == 0u || thunk == nullptr) {
        return false;
    }

    std::lock_guard<std::mutex> lock(g_method_replacement_mutex);
    auto& entry = g_method_replacements[method_token];
    entry.method_token = method_token;
    entry.replacement_thunk = thunk;
    entry.active = true;
    return true;
}

bool Revert(uint32_t method_token) {
    std::lock_guard<std::mutex> lock(g_method_replacement_mutex);
    return g_method_replacements.erase(method_token) > 0u;
}

void RevertAll() {
    std::lock_guard<std::mutex> lock(g_method_replacement_mutex);
    g_method_replacements.clear();
}

void* Resolve(uint32_t method_token) {
    std::lock_guard<std::mutex> lock(g_method_replacement_mutex);
    const auto it = g_method_replacements.find(method_token);
    if (it == g_method_replacements.end() || !it->second.active) {
        return nullptr;
    }

    return it->second.replacement_thunk;
}

uint32_t ActiveCount() {
    std::lock_guard<std::mutex> lock(g_method_replacement_mutex);
    return static_cast<uint32_t>(g_method_replacements.size());
}

}  // namespace chaos::il2cpp::method_replacement
