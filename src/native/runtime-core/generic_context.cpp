#include "generic_context.h"

#include <mutex>
#include <unordered_map>
#include <vector>

namespace chaos::il2cpp::generic_context {

namespace {

/// Describes a single closed generic type instantiation.
struct GenericInstantiationEntry {
    uint32_t              open_token;
    uint32_t              closed_token;
    std::vector<uint32_t> type_arg_tokens;
};

/// A minimal generic context stored per method token.
/// Wraps class-level and method-level type argument arrays.
struct MethodGenericContextEntry {
    std::vector<TypeInfoHandle> class_type_args;
    std::vector<TypeInfoHandle> method_type_args;
};

// The opaque GenericContextHandle IS the MethodGenericContextEntry pointer.
// We use an owning store to keep entries alive for the process lifetime.

struct GenericContextRegistry {
    std::mutex mutex;
    // type instantiation map: open_token → list of entries
    std::unordered_map<uint32_t, std::vector<GenericInstantiationEntry>> by_open_token;
    // method generic context map: method_token → context entry (heap-allocated)
    std::unordered_map<uint32_t, MethodGenericContextEntry*> by_method_token;
    // ownership store to prevent leaks
    std::vector<std::unique_ptr<MethodGenericContextEntry>> owned_entries;
};

GenericContextRegistry& GetRegistry() {
    static GenericContextRegistry s_registry;
    return s_registry;
}

}  // namespace

void RegisterGenericInstantiation(
    uint32_t open_token,
    uint32_t closed_token,
    const uint32_t* type_args,
    uint32_t arg_count) {
    if (open_token == 0u || closed_token == 0u) {
        return;
    }

    auto& registry = GetRegistry();
    std::lock_guard<std::mutex> lock(registry.mutex);

    auto& entries = registry.by_open_token[open_token];
    // Check for duplicate
    for (const auto& entry : entries) {
        if (entry.closed_token == closed_token) {
            return;  // already registered
        }
    }

    GenericInstantiationEntry entry;
    entry.open_token   = open_token;
    entry.closed_token = closed_token;
    if (type_args != nullptr && arg_count > 0u) {
        entry.type_arg_tokens.assign(type_args, type_args + arg_count);
    }
    entries.push_back(std::move(entry));
}

GenericContextHandle GetGenericContextForMethod(uint32_t method_token) {
    if (method_token == 0u) {
        return nullptr;
    }

    auto& registry = GetRegistry();
    std::lock_guard<std::mutex> lock(registry.mutex);

    auto it = registry.by_method_token.find(method_token);
    if (it == registry.by_method_token.end()) {
        return nullptr;
    }

    return reinterpret_cast<GenericContextHandle>(it->second);
}

void RegisterMethodGenericContext(
    uint32_t method_token,
    const TypeInfoHandle* class_type_args, uint32_t class_arg_count,
    const TypeInfoHandle* method_type_args, uint32_t method_arg_count) {
    if (method_token == 0u) {
        return;
    }

    auto& registry = GetRegistry();
    std::lock_guard<std::mutex> lock(registry.mutex);

    if (registry.by_method_token.count(method_token)) {
        return;  // already registered
    }

    auto entry = std::make_unique<MethodGenericContextEntry>();
    if (class_type_args != nullptr && class_arg_count > 0u) {
        entry->class_type_args.assign(class_type_args, class_type_args + class_arg_count);
    }
    if (method_type_args != nullptr && method_arg_count > 0u) {
        entry->method_type_args.assign(method_type_args, method_type_args + method_arg_count);
    }

    registry.by_method_token[method_token] = entry.get();
    registry.owned_entries.push_back(std::move(entry));
}

uint32_t GetRegisteredInstantiationCount() {
    auto& registry = GetRegistry();
    std::lock_guard<std::mutex> lock(registry.mutex);
    uint32_t count = 0u;
    for (const auto& [_, v] : registry.by_open_token) {
        count += static_cast<uint32_t>(v.size());
    }
    return count;
}

}  // namespace chaos::il2cpp::generic_context
