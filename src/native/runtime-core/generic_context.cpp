#include "generic_context.h"
#include "bootstrap.h"
#include "codegen_bridge.h"
#include "reflection_query_model.h"

#include <chaos/native_types.h>

#include <mutex>
#include <unordered_map>
#include <vector>

namespace chaos::il2cpp::generic_context {

namespace {

/// Describes a single closed generic type instantiation.
/// All type identities are TypeInfoHandle (tag-encoded ReflectionQueryTypeDescriptor*).
struct GenericInstantiationEntry {
    TypeInfoHandle                     open_type;     // open generic definition
    TypeInfoHandle                     closed_type;   // closed instantiation
    CHAOS_IL2CPP_VECTOR(TypeInfoHandle) type_args;    // type argument handles
    CHAOS_IL2CPP_UINT32                module_id;     // 0 = AOT, >0 = hotupdate
};

/// A minimal generic context stored per method token.
/// Wraps class-level and method-level type argument handle arrays.
struct MethodGenericContextEntry {
    CHAOS_IL2CPP_VECTOR(TypeInfoHandle) class_type_args;
    CHAOS_IL2CPP_VECTOR(TypeInfoHandle) method_type_args;
    CHAOS_IL2CPP_UINT32                module_id;
};

// The opaque GenericContextHandle IS the MethodGenericContextEntry pointer.
// We use an owning store to keep entries alive for the process lifetime.

struct GenericContextRegistry {
    CHAOS_IL2CPP_MUTEX mutex;

    // ── Type instantiation index ──
    // Keyed by open_type handle (tag-encoded pointer, O(1) comparison).
    using InstantiationVector = CHAOS_IL2CPP_VECTOR(GenericInstantiationEntry);
    CHAOS_IL2CPP_UNORDERED_MAP(TypeInfoHandle, InstantiationVector) by_open_type;

    // ── Method generic context index ──
    CHAOS_IL2CPP_UNORDERED_MAP(CHAOS_IL2CPP_UINT32, MethodGenericContextEntry*) by_method_token;
    using EntryPtrVector = CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_UNIQUE_PTR(MethodGenericContextEntry));
    EntryPtrVector owned_entries;

    // ── Module ownership index (for hot-update unregistration) ──
    CHAOS_IL2CPP_UNORDERED_MAP(CHAOS_IL2CPP_UINT32,
        CHAOS_IL2CPP_VECTOR(TypeInfoHandle)) types_by_module;
    CHAOS_IL2CPP_UNORDERED_MAP(CHAOS_IL2CPP_UINT32,
        CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_UINT32)) methods_by_module;
};

GenericContextRegistry& GetRegistry() {
    static GenericContextRegistry s_registry;
    return s_registry;
}

}  // namespace

// ═════════════════════════════════════════════════════════════
// Generic type instantiation (handle-based)
// ═════════════════════════════════════════════════════════════

void RegisterGenericInstantiation(
    TypeInfoHandle open_type,
    TypeInfoHandle closed_type,
    const TypeInfoHandle* type_args,
    CHAOS_IL2CPP_UINT32 arg_count)
{
    if (open_type == nullptr || closed_type == nullptr) {
        return;
    }

    auto& registry = GetRegistry();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(registry.mutex);

    // Idempotency: O(m) where m = instantiations for this open_type (typically 1-5).
    auto& entries = registry.by_open_type[open_type];
    for (const auto& entry : entries) {
        if (entry.closed_type == closed_type) {
            return;  // already registered
        }
    }

    GenericInstantiationEntry entry;
    entry.open_type   = open_type;
    entry.closed_type = closed_type;
    entry.module_id   = 0u;  // default: AOT root
    if (type_args != nullptr && arg_count > 0u) {
        entry.type_args.assign(type_args, type_args + arg_count);
    }
    entries.push_back(CHAOS_IL2CPP_MOVE(entry));
}

// ═════════════════════════════════════════════════════════════
// Method generic context
// ═════════════════════════════════════════════════════════════

GenericContextHandle GetGenericContextForMethod(CHAOS_IL2CPP_UINT32 method_token) {
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
    CHAOS_IL2CPP_UINT32 method_token,
    const TypeInfoHandle* class_type_args, CHAOS_IL2CPP_UINT32 class_arg_count,
    const TypeInfoHandle* method_type_args, CHAOS_IL2CPP_UINT32 method_arg_count)
{
    if (method_token == 0u) {
        return;
    }

    auto& registry = GetRegistry();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(registry.mutex);

    if (registry.by_method_token.count(method_token)) {
        return;  // already registered
    }

    auto entry = CHAOS_IL2CPP_MAKE_UNIQUE(MethodGenericContextEntry)();
    entry->module_id = 0u;
    if (class_type_args != nullptr && class_arg_count > 0u) {
        entry->class_type_args.assign(class_type_args, class_type_args + class_arg_count);
    }
    if (method_type_args != nullptr && method_arg_count > 0u) {
        entry->method_type_args.assign(method_type_args, method_type_args + method_arg_count);
    }

    registry.by_method_token[method_token] = entry.get();
    registry.owned_entries.push_back(CHAOS_IL2CPP_MOVE(entry));
}

// ═════════════════════════════════════════════════════════════
// Accessors (unchanged semantics, now operate on handle vectors)
// ═════════════════════════════════════════════════════════════

CHAOS_IL2CPP_UINT32 GetClassTypeArgCount(GenericContextHandle generic_context) {
    if (generic_context == nullptr) {
        return 0u;
    }

    const auto* entry = reinterpret_cast<const MethodGenericContextEntry*>(generic_context);
    return static_cast<CHAOS_IL2CPP_UINT32>(entry->class_type_args.size());
}

TypeInfoHandle GetClassTypeArg(GenericContextHandle generic_context, CHAOS_IL2CPP_UINT32 index) {
    if (generic_context == nullptr) {
        return 0;
    }

    const auto* entry = reinterpret_cast<const MethodGenericContextEntry*>(generic_context);
    if (index >= entry->class_type_args.size()) {
        return 0;
    }

    return entry->class_type_args[index];
}

CHAOS_IL2CPP_UINT32 GetMethodTypeArgCount(GenericContextHandle generic_context) {
    if (generic_context == nullptr) {
        return 0u;
    }

    const auto* entry = reinterpret_cast<const MethodGenericContextEntry*>(generic_context);
    return static_cast<CHAOS_IL2CPP_UINT32>(entry->method_type_args.size());
}

TypeInfoHandle GetMethodTypeArg(GenericContextHandle generic_context, CHAOS_IL2CPP_UINT32 index) {
    if (generic_context == nullptr) {
        return 0;
    }

    const auto* entry = reinterpret_cast<const MethodGenericContextEntry*>(generic_context);
    if (index >= entry->method_type_args.size()) {
        return 0;
    }

    return entry->method_type_args[index];
}

// ═════════════════════════════════════════════════════════════
// Bulk registration from codegen-emitted data
// ═════════════════════════════════════════════════════════════

void RegisterModuleGenerics(const struct ModuleGenericRegistrationV0* reg) {
    if (reg == nullptr) {
        return;
    }

    auto& registry = GetRegistry();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(registry.mutex);

    const auto* bridge = chaos::il2cpp::bootstrap::GetCodegenBridgeV0();
    if (bridge == nullptr || bridge->resolve_type_by_token == nullptr) {
        return;
    }

    TypeInfoHandle arg_buffer[8];  // sufficient for all .NET generic arities

    // ── Register type generic instantiations ──
    if (reg->generic_types != nullptr && reg->generic_type_count > 0u) {
        CHAOS_IL2CPP_VECTOR(TypeInfoHandle) module_types;
        module_types.reserve(reg->generic_type_count);

        for (CHAOS_IL2CPP_UINT32 i = 0u; i < reg->generic_type_count; i++) {
            const auto& entry = reg->generic_types[i];

            TypeInfoHandle open_type   = bridge->resolve_type_by_token(
                reg->source_image, entry.open_token);
            TypeInfoHandle closed_type = bridge->resolve_type_by_token(
                reg->source_image, entry.closed_token);

            const CHAOS_IL2CPP_UINT32 ac = entry.arg_count > 8u ? 8u : entry.arg_count;
            for (CHAOS_IL2CPP_UINT32 j = 0u; j < ac; j++) {
                arg_buffer[j] = bridge->resolve_type_by_token(
                    reg->source_image,
                    reg->generic_type_args[entry.args_start_index + j]);
            }

            // Idempotency check.
            auto& existing = registry.by_open_type[open_type];
            bool found = false;
            for (const auto& ex : existing) {
                if (ex.closed_type == closed_type) { found = true; break; }
            }
            if (found) continue;

            GenericInstantiationEntry inst;
            inst.open_type   = open_type;
            inst.closed_type = closed_type;
            inst.module_id   = reg->module_id;
            if (ac > 0u) {
                inst.type_args.assign(arg_buffer, arg_buffer + ac);
            }
            existing.push_back(CHAOS_IL2CPP_MOVE(inst));

            module_types.push_back(closed_type);
        }

        if (!module_types.empty()) {
            registry.types_by_module[reg->module_id] = CHAOS_IL2CPP_MOVE(module_types);
        }
    }

    // ── Register method generic contexts ──
    if (reg->generic_methods != nullptr && reg->generic_method_count > 0u) {
        CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_UINT32) module_methods;
        module_methods.reserve(reg->generic_method_count);

        for (CHAOS_IL2CPP_UINT32 i = 0u; i < reg->generic_method_count; i++) {
            const auto& entry = reg->generic_methods[i];

            if (registry.by_method_token.count(entry.method_token)) {
                continue;  // idempotent
            }

            // class_type_args occupy the first class_arg_count slots in the
            // arg pool, followed by method_arg_count method-level args.
            const CHAOS_IL2CPP_UINT32 total = entry.arg_count > 8u ? 8u : entry.arg_count;
            for (CHAOS_IL2CPP_UINT32 j = 0u; j < total; j++) {
                arg_buffer[j] = bridge->resolve_type_by_token(
                    reg->source_image,
                    reg->generic_method_args[entry.args_start_index + j]);
            }

            // Split into class/method args.
            const CHAOS_IL2CPP_UINT32 cc = total > entry.class_arg_count
                ? entry.class_arg_count : total;
            auto* class_args = cc > 0u ? arg_buffer : nullptr;
            auto* method_args = nullptr;
            CHAOS_IL2CPP_UINT32 mc = 0u;
            if (total > cc) {
                mc = total - cc;
                method_args = arg_buffer + cc;
            }

            auto ctx = CHAOS_IL2CPP_MAKE_UNIQUE(MethodGenericContextEntry)();
            ctx->module_id = reg->module_id;
            if (class_args != nullptr) {
                ctx->class_type_args.assign(class_args, class_args + cc);
            }
            if (method_args != nullptr) {
                ctx->method_type_args.assign(method_args, method_args + mc);
            }

            registry.by_method_token[entry.method_token] = ctx.get();
            registry.owned_entries.push_back(CHAOS_IL2CPP_MOVE(ctx));
            module_methods.push_back(entry.method_token);
        }

        if (!module_methods.empty()) {
            registry.methods_by_module[reg->module_id] = CHAOS_IL2CPP_MOVE(module_methods);
        }
    }
}

// ═════════════════════════════════════════════════════════════
// Module-level unregistration (hot-update)
// ═════════════════════════════════════════════════════════════

void UnregisterModuleGenerics(CHAOS_IL2CPP_UINT32 module_id) {
    auto& registry = GetRegistry();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(registry.mutex);

    // Remove type instantiations belonging to this module.
    auto type_it = registry.types_by_module.find(module_id);
    if (type_it != registry.types_by_module.end()) {
        // Build a set of closed_type handles to remove.
        const auto& closed_types = type_it->second;
        for (auto& [open_type, entries] : registry.by_open_type) {
            entries.erase(
                std::remove_if(entries.begin(), entries.end(),
                    [&](const GenericInstantiationEntry& e) {
                        for (auto ct : closed_types) {
                            if (e.closed_type == ct) return true;
                        }
                        return false;
                    }),
                entries.end());
        }
        registry.types_by_module.erase(type_it);
    }

    // Remove method contexts belonging to this module.
    auto meth_it = registry.methods_by_module.find(module_id);
    if (meth_it != registry.methods_by_module.end()) {
        const auto& tokens = meth_it->second;
        for (auto token : tokens) {
            registry.by_method_token.erase(token);
        }
        registry.methods_by_module.erase(meth_it);
    }
}

// ═════════════════════════════════════════════════════════════
// Diagnostics
// ═════════════════════════════════════════════════════════════

CHAOS_IL2CPP_UINT32 GetRegisteredInstantiationCount() {
    auto& registry = GetRegistry();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(registry.mutex);
    CHAOS_IL2CPP_UINT32 count = 0u;
    for (const auto& [_, v] : registry.by_open_type) {
        count += static_cast<CHAOS_IL2CPP_UINT32>(v.size());
    }
    return count;
}

// ═════════════════════════════════════════════════════════════
// Fast-path lookup for RuntimeInstantiationBridge
// ═════════════════════════════════════════════════════════════

TypeInfoHandle TryResolveClosedType(
    TypeInfoHandle open_type,
    const TypeInfoHandle* type_args,
    CHAOS_IL2CPP_UINT32 arg_count)
{
    if (open_type == 0) {
        return 0;
    }

    auto& registry = GetRegistry();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(registry.mutex);

    auto it = registry.by_open_type.find(open_type);
    if (it == registry.by_open_type.end()) {
        return 0;
    }

    for (const auto& entry : it->second) {
        if (entry.type_args.size() != arg_count) continue;

        bool match = true;
        for (CHAOS_IL2CPP_UINT32 i = 0u; i < arg_count; i++) {
            if (entry.type_args[i] != type_args[i]) {
                match = false;
                break;
            }
        }
        if (match) {
            return entry.closed_type;
        }
    }

    return nullptr;  // not found
}

}  // namespace chaos::il2cpp::generic_context
