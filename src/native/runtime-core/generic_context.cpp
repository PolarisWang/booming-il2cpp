#include "generic_context.h"

#include <chaos/native_types.h>

#include <mutex>
#include <unordered_map>
#include <vector>

namespace chaos::il2cpp::generic_context {

namespace {

/// Describes a single closed generic type instantiation.
struct GenericInstantiationEntry {
    uint32_t              open_token;
    uint32_t              closed_token;
    CHAOS_IL2CPP_VECTOR(uint32_t) type_arg_tokens;
};

/// A minimal generic context stored per method token.
/// Wraps class-level and method-level type argument arrays.
struct MethodGenericContextEntry {
    CHAOS_IL2CPP_VECTOR(TypeInfoHandle) class_type_args;
    CHAOS_IL2CPP_VECTOR(TypeInfoHandle) method_type_args;
};

// The opaque GenericContextHandle IS the MethodGenericContextEntry pointer.
// We use an owning store to keep entries alive for the process lifetime.

struct GenericContextRegistry {
    CHAOS_IL2CPP_MUTEX mutex;
    // Intermediate typedefs to avoid MSVC >> issue with nested macros
    using InstantiationVector = CHAOS_IL2CPP_VECTOR(GenericInstantiationEntry);
    CHAOS_IL2CPP_UNORDERED_MAP(uint32_t, InstantiationVector) by_open_token;
    // method generic context map: method_token -> context entry (heap-allocated)
    CHAOS_IL2CPP_UNORDERED_MAP(uint32_t, MethodGenericContextEntry*) by_method_token;
    // ownership store to prevent leaks
    using EntryPtrVector = CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_UNIQUE_PTR(MethodGenericContextEntry));
    EntryPtrVector owned_entries;
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
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(registry.mutex);

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
    entries.push_back(CHAOS_IL2CPP_MOVE(entry));
}

GenericContextHandle GetGenericContextForMethod(uint32_t method_token) {
    if (method_token == 0u) {
        return nullptr;
    }

    auto& registry = GetRegistry();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(registry.mutex);

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
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(registry.mutex);

    if (registry.by_method_token.count(method_token)) {
        return;  // already registered
    }

    auto entry = CHAOS_IL2CPP_MAKE_UNIQUE(MethodGenericContextEntry)();
    if (class_type_args != nullptr && class_arg_count > 0u) {
        entry->class_type_args.assign(class_type_args, class_type_args + class_arg_count);
    }
    if (method_type_args != nullptr && method_arg_count > 0u) {
        entry->method_type_args.assign(method_type_args, method_type_args + method_arg_count);
    }

    registry.by_method_token[method_token] = entry.get();
    registry.owned_entries.push_back(CHAOS_IL2CPP_MOVE(entry));
}

uint32_t GetClassTypeArgCount(GenericContextHandle generic_context) {
    if (generic_context == nullptr) {
        return 0u;
    }

    const auto* entry = reinterpret_cast<const MethodGenericContextEntry*>(generic_context);
    return static_cast<uint32_t>(entry->class_type_args.size());
}

TypeInfoHandle GetClassTypeArg(GenericContextHandle generic_context, uint32_t index) {
    if (generic_context == nullptr) {
        return nullptr;
    }

    const auto* entry = reinterpret_cast<const MethodGenericContextEntry*>(generic_context);
    if (index >= entry->class_type_args.size()) {
        return nullptr;
    }

    return entry->class_type_args[index];
}

uint32_t GetMethodTypeArgCount(GenericContextHandle generic_context) {
    if (generic_context == nullptr) {
        return 0u;
    }

    const auto* entry = reinterpret_cast<const MethodGenericContextEntry*>(generic_context);
    return static_cast<uint32_t>(entry->method_type_args.size());
}

TypeInfoHandle GetMethodTypeArg(GenericContextHandle generic_context, uint32_t index) {
    if (generic_context == nullptr) {
        return nullptr;
    }

    const auto* entry = reinterpret_cast<const MethodGenericContextEntry*>(generic_context);
    if (index >= entry->method_type_args.size()) {
        return nullptr;
    }

    return entry->method_type_args[index];
}

uint32_t GetRegisteredInstantiationCount() {
    auto& registry = GetRegistry();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(registry.mutex);
    uint32_t count = 0u;
    for (const auto& [_, v] : registry.by_open_token) {
        count += static_cast<uint32_t>(v.size());
    }
    return count;
}

}  // namespace chaos::il2cpp::generic_context
