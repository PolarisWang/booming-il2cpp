#include "generic_context.h"
#include "../bootstrap/bootstrap.h"
#include <codegen_bridge.h>
#include "reflection_query_model.h"

#include <chaos/native_types.h>
#include <chaos/trace.h>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace chaos::il2cpp::generic_context {

namespace {

/// Describes a single closed generic type instantiation (global index entry).
struct GenericInstantiationEntry {
    TypeInfoHandle                     open_type;
    TypeInfoHandle                     closed_type;
    CHAOS_IL2CPP_VECTOR(TypeInfoHandle) type_args;
    CHAOS_IL2CPP_UINT32                module_id;
};

/// Describes a single closed generic method instantiation (global index entry).
struct MethodInstantiationEntry {
    MethodInfoHandle                    open_method;
    MethodInfoHandle                    closed_method;
    CHAOS_IL2CPP_VECTOR(TypeInfoHandle) type_args;
    CHAOS_IL2CPP_UINT32                module_id;
};

/// Minimal generic context stored per method token.
struct MethodGenericContextEntry {
    CHAOS_IL2CPP_VECTOR(TypeInfoHandle) class_type_args;
    CHAOS_IL2CPP_VECTOR(TypeInfoHandle) method_type_args;
    CHAOS_IL2CPP_UINT32                module_id;
};

// ── Per-module shard: holds raw token data and owns entries ──

/// A single raw-token entry for a closed generic type whose resolution is deferred.
struct LazyTypeEntry {
    TypeInfoHandle         open_type;       // resolved open handle
    CHAOS_IL2CPP_UINT32    closed_token;    // unresolved raw token
    CHAOS_IL2CPP_UINT32    args_start;      // index into shard.token_pool
    CHAOS_IL2CPP_UINT32    arg_count;
};

/// A single raw-token entry for a method generic context whose resolution is deferred.
struct LazyMethodContextEntry {
    CHAOS_IL2CPP_UINT32    method_token;
    CHAOS_IL2CPP_UINT32    class_args_start;
    CHAOS_IL2CPP_UINT32    class_arg_count;
    CHAOS_IL2CPP_UINT32    method_args_start;
    CHAOS_IL2CPP_UINT32    method_arg_count;
};

/// All per-module data. Created during RegisterModuleGenerics, destroyed
/// during UnregisterModuleGenerics (O(1) shard drop).
struct ModuleShard {
    CHAOS_IL2CPP_MUTEX     shard_mutex;
    CHAOS_IL2CPP_UINT32    module_id;
    ImageHandle            source_image;    // for token→handle resolution

    CHAOS_IL2CPP_VECTOR(TypeInfoHandle)        open_definitions;   // resolved open handles
    CHAOS_IL2CPP_VECTOR(LazyTypeEntry)          lazy_types;
    CHAOS_IL2CPP_VECTOR(LazyMethodContextEntry) lazy_contexts;
    CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_UINT32)   token_pool;         // flat raw token storage

    using EntryPtrVector = CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_UNIQUE_PTR(MethodGenericContextEntry));
    EntryPtrVector owned_entries;   // destroyed with shard → no leak

    CHAOS_IL2CPP_VECTOR(bool) resolve_state;    // parallel to open_definitions; true = resolved

    explicit ModuleShard(CHAOS_IL2CPP_UINT32 mod_id, ImageHandle img)
        : module_id(mod_id), source_image(img) {}
};

// ── Lock-free routing table (open addressing, linear probing) ──

constexpr CHAOS_IL2CPP_UINT32 kRouteCapacity = 347;  // prime > 256 * 1.33

struct ShardEntry {
    std::atomic<CHAOS_IL2CPP_UINT32> module_id{0};
    ModuleShard* shard{nullptr};
};

ShardEntry s_routing_table[kRouteCapacity];

static CHAOS_IL2CPP_UINT32 RouteSlot(CHAOS_IL2CPP_UINT32 mid) {
    return mid % kRouteCapacity;
}

static bool RoutingTableInsert(CHAOS_IL2CPP_UINT32 module_id, ModuleShard* shard) {
    CHAOS_IL2CPP_UINT32 slot = RouteSlot(module_id);
    for (CHAOS_IL2CPP_UINT32 i = 0; i < kRouteCapacity; i++) {
        CHAOS_IL2CPP_UINT32 idx = (slot + i) % kRouteCapacity;
        CHAOS_IL2CPP_UINT32 expected = 0;
        if (s_routing_table[idx].module_id.compare_exchange_strong(expected, module_id)) {
            s_routing_table[idx].shard = shard;
            return true;
        }
        if (s_routing_table[idx].module_id.load(std::memory_order_relaxed) == module_id)
            return false;  // duplicate
    }
    return false;  // table full
}

static ModuleShard* RoutingTableLookup(CHAOS_IL2CPP_UINT32 module_id) {
    if (module_id == 0) return nullptr;
    CHAOS_IL2CPP_UINT32 slot = RouteSlot(module_id);
    for (CHAOS_IL2CPP_UINT32 i = 0; i < kRouteCapacity; i++) {
        CHAOS_IL2CPP_UINT32 idx = (slot + i) % kRouteCapacity;
        auto mid = s_routing_table[idx].module_id.load(std::memory_order_acquire);
        if (mid == module_id) return s_routing_table[idx].shard;
        if (mid == 0) break;  // end of probe chain
    }
    return nullptr;
}

static ModuleShard* RoutingTableRemove(CHAOS_IL2CPP_UINT32 module_id) {
    if (module_id == 0) return nullptr;
    CHAOS_IL2CPP_UINT32 slot = RouteSlot(module_id);
    for (CHAOS_IL2CPP_UINT32 i = 0; i < kRouteCapacity; i++) {
        CHAOS_IL2CPP_UINT32 idx = (slot + i) % kRouteCapacity;
        auto mid = s_routing_table[idx].module_id.load(std::memory_order_acquire);
        if (mid == module_id) {
            auto* shard = s_routing_table[idx].shard;
            s_routing_table[idx].shard = nullptr;
            s_routing_table[idx].module_id.store(0, std::memory_order_release);
            return shard;
        }
        if (mid == 0) break;
    }
    return nullptr;
}

// ── Slimmed global registry (shared indices only) ──

struct Registry {
    CHAOS_IL2CPP_MUTEX global_mutex;

    // Type instantiation index (open → closed vector) with shard owner for lazy resolve.
    using InstantiationVector = CHAOS_IL2CPP_VECTOR(GenericInstantiationEntry);
    struct TypeEntry {
        InstantiationVector closed_types;
        ModuleShard*        owner_shard;   // nullptr = no lazy data (standalone registration)
    };
    CHAOS_IL2CPP_UNORDERED_MAP(TypeInfoHandle, TypeEntry) by_open_type;

    // Method instantiation index.
    using MethodInstantiationVector = CHAOS_IL2CPP_VECTOR(MethodInstantiationEntry);
    CHAOS_IL2CPP_UNORDERED_MAP(MethodInfoHandle, MethodInstantiationVector) by_open_method;

    // Method generic context index (bare ptr owned by shard).
    CHAOS_IL2CPP_UNORDERED_MAP(CHAOS_IL2CPP_UINT32, MethodGenericContextEntry*) by_method_token;
    CHAOS_IL2CPP_UNORDERED_MAP(CHAOS_IL2CPP_UINT32, ModuleShard*) method_token_owners;

    // Orphan entries for AOT-root (module_id=0) standalone registrations.
    using EntryPtrVector = CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_UNIQUE_PTR(MethodGenericContextEntry));
    EntryPtrVector orphan_entries;
};

Registry& GetRegistry() {
    static Registry s_registry;
    return s_registry;
}

/// Batch-resolve all lazy type entries for a given open_type from a shard.
/// Must be called with shard->shard_mutex held (double-checked locking).
static void DoLazyResolveOpenType(Registry& registry, ModuleShard* shard,
                                   TypeInfoHandle open_type, CHAOS_IL2CPP_SIZE open_idx)
{
    const auto* bridge = chaos::il2cpp::bootstrap::GetCodegenBridgeV0();
    if (bridge == nullptr || bridge->resolve_type_by_token == nullptr)
        return;

    auto& type_entry = registry.by_open_type[open_type];
    for (const auto& lazy : shard->lazy_types) {
        if (lazy.open_type != open_type)
            continue;

        TypeInfoHandle closed_handle = bridge->resolve_type_by_token(
            shard->source_image, lazy.closed_token);
        if (closed_handle == 0)
            continue;

        CHAOS_IL2CPP_VECTOR(TypeInfoHandle) arg_handles;
        arg_handles.reserve(lazy.arg_count);
        for (CHAOS_IL2CPP_UINT32 j = 0; j < lazy.arg_count; j++) {
            arg_handles.push_back(bridge->resolve_type_by_token(
                shard->source_image,
                shard->token_pool[lazy.args_start + j]));
        }

        GenericInstantiationEntry inst;
        inst.open_type   = open_type;
        inst.closed_type = closed_handle;
        inst.module_id   = shard->module_id;
        inst.type_args   = CHAOS_IL2CPP_MOVE(arg_handles);
        type_entry.closed_types.push_back(CHAOS_IL2CPP_MOVE(inst));
    }

    shard->resolve_state[open_idx] = true;
}

/// Batch-resolve a single method generic context from raw tokens.
static void DoLazyResolveMethodContext(Registry& registry, ModuleShard* shard,
                                        CHAOS_IL2CPP_UINT32 method_token,
                                        const LazyMethodContextEntry& lazy)
{
    const auto* bridge = chaos::il2cpp::bootstrap::GetCodegenBridgeV0();
    if (bridge == nullptr || bridge->resolve_type_by_token == nullptr)
        return;

    auto ctx = CHAOS_IL2CPP_MAKE_UNIQUE(MethodGenericContextEntry)();
    ctx->module_id = shard->module_id;

    for (CHAOS_IL2CPP_UINT32 j = 0; j < lazy.class_arg_count; j++) {
        ctx->class_type_args.push_back(bridge->resolve_type_by_token(
            shard->source_image,
            shard->token_pool[lazy.class_args_start + j]));
    }
    for (CHAOS_IL2CPP_UINT32 j = 0; j < lazy.method_arg_count; j++) {
        ctx->method_type_args.push_back(bridge->resolve_type_by_token(
            shard->source_image,
            shard->token_pool[lazy.method_args_start + j]));
    }

    registry.by_method_token[method_token] = ctx.get();
    shard->owned_entries.push_back(CHAOS_IL2CPP_MOVE(ctx));
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
    CHAOS_IL2CPP_TRACE("runtime", "RegisterGenericInstantiation",
        "\"arg_count\"=%u", arg_count);
    if (open_type == 0 || closed_type == 0)
        return;

    auto& registry = GetRegistry();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(registry.global_mutex);

    auto& type_entry = registry.by_open_type[open_type];
    for (const auto& entry : type_entry.closed_types) {
        if (entry.closed_type == closed_type)
            return;  // idempotent
    }

    GenericInstantiationEntry entry;
    entry.open_type   = open_type;
    entry.closed_type = closed_type;
    entry.module_id   = 0u;
    if (type_args != nullptr && arg_count > 0u) {
        entry.type_args.assign(type_args, type_args + arg_count);
    }
    type_entry.closed_types.push_back(CHAOS_IL2CPP_MOVE(entry));
}

// ═════════════════════════════════════════════════════════════
// Method generic context
// ═════════════════════════════════════════════════════════════

GenericContextHandle GetGenericContextForMethod(CHAOS_IL2CPP_UINT32 method_token) {
    if (method_token == 0u)
        return nullptr;

    auto& registry = GetRegistry();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(registry.global_mutex);

    auto it = registry.by_method_token.find(method_token);
    if (it == registry.by_method_token.end()) {
        // Not registered → check if we have a lazy entry pending.
        auto owner_it = registry.method_token_owners.find(method_token);
        if (owner_it == registry.method_token_owners.end())
            return nullptr;

        ModuleShard* shard = owner_it->second;
        // Find the lazy entry and resolve it.
        for (const auto& lazy : shard->lazy_contexts) {
            if (lazy.method_token == method_token) {
                DoLazyResolveMethodContext(registry, shard, method_token, lazy);
                break;
            }
        }
        // method_token_owners entry is intentionally kept (tombstone); we
        // re-check by_method_token after resolution.
        it = registry.by_method_token.find(method_token);
        if (it == registry.by_method_token.end())
            return nullptr;
    }

    return reinterpret_cast<GenericContextHandle>(it->second);
}

void RegisterMethodGenericContext(
    CHAOS_IL2CPP_UINT32 method_token,
    const TypeInfoHandle* class_type_args, CHAOS_IL2CPP_UINT32 class_arg_count,
    const TypeInfoHandle* method_type_args, CHAOS_IL2CPP_UINT32 method_arg_count)
{
    if (method_token == 0u)
        return;

    auto& registry = GetRegistry();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(registry.global_mutex);

    if (registry.by_method_token.count(method_token))
        return;  // already registered

    auto entry = CHAOS_IL2CPP_MAKE_UNIQUE(MethodGenericContextEntry)();
    entry->module_id = 0u;
    if (class_type_args != nullptr && class_arg_count > 0u)
        entry->class_type_args.assign(class_type_args, class_type_args + class_arg_count);
    if (method_type_args != nullptr && method_arg_count > 0u)
        entry->method_type_args.assign(method_type_args, method_type_args + method_arg_count);

    registry.by_method_token[method_token] = entry.get();
    registry.orphan_entries.push_back(CHAOS_IL2CPP_MOVE(entry));
}

// ═════════════════════════════════════════════════════════════
// Accessors
// ═════════════════════════════════════════════════════════════

CHAOS_IL2CPP_UINT32 GetClassTypeArgCount(GenericContextHandle generic_context) {
    if (generic_context == nullptr) return 0u;
    const auto* entry = reinterpret_cast<const MethodGenericContextEntry*>(generic_context);
    return static_cast<CHAOS_IL2CPP_UINT32>(entry->class_type_args.size());
}

TypeInfoHandle GetClassTypeArg(GenericContextHandle generic_context, CHAOS_IL2CPP_UINT32 index) {
    if (generic_context == nullptr) return 0;
    const auto* entry = reinterpret_cast<const MethodGenericContextEntry*>(generic_context);
    return index < entry->class_type_args.size() ? entry->class_type_args[index] : 0;
}

CHAOS_IL2CPP_UINT32 GetMethodTypeArgCount(GenericContextHandle generic_context) {
    if (generic_context == nullptr) return 0u;
    const auto* entry = reinterpret_cast<const MethodGenericContextEntry*>(generic_context);
    return static_cast<CHAOS_IL2CPP_UINT32>(entry->method_type_args.size());
}

TypeInfoHandle GetMethodTypeArg(GenericContextHandle generic_context, CHAOS_IL2CPP_UINT32 index) {
    if (generic_context == nullptr) return 0;
    const auto* entry = reinterpret_cast<const MethodGenericContextEntry*>(generic_context);
    return index < entry->method_type_args.size() ? entry->method_type_args[index] : 0;
}

// ═════════════════════════════════════════════════════════════
// Fast-path: resolve closed type from registry (with lazy trigger)
// ═════════════════════════════════════════════════════════════

TypeInfoHandle TryResolveClosedType(
    TypeInfoHandle open_type,
    const TypeInfoHandle* type_args,
    CHAOS_IL2CPP_UINT32 arg_count)
{
    CHAOS_IL2CPP_TRACE("runtime", "TryResolveClosedType",
        "\"arg_count\"=%u", arg_count);
    if (open_type == 0)
        return 0;

    auto& registry = GetRegistry();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(registry.global_mutex);

    auto it = registry.by_open_type.find(open_type);
    if (it == registry.by_open_type.end())
        return 0;

    auto& type_entry = it->second;

    // If the closed_types vector is empty but we have an owner shard and
    // the open type hasn't been resolved yet → trigger lazy batch resolve.
    if (type_entry.closed_types.empty() && type_entry.owner_shard != nullptr) {
        // Find the open index in the shard.
        for (CHAOS_IL2CPP_SIZE i = 0; i < type_entry.owner_shard->open_definitions.size(); i++) {
            if (type_entry.owner_shard->open_definitions[i] == open_type) {
                // Double-checked locking: already checked resolve_state without lock,
                // now do it under the shard mutex.
                {
                    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) slock(type_entry.owner_shard->shard_mutex);
                    if (!type_entry.owner_shard->resolve_state[i]) {
                        DoLazyResolveOpenType(registry, type_entry.owner_shard, open_type, i);
                    }
                }
                break;
            }
        }
        // After resolution, the shard owner is no longer needed for this open type.
        type_entry.owner_shard = nullptr;
    }

    // Linear scan of now-populated (or previously populated) entries.
    for (const auto& entry : type_entry.closed_types) {
        if (entry.type_args.size() != arg_count)
            continue;
        bool match = true;
        for (CHAOS_IL2CPP_UINT32 i = 0; i < arg_count; i++) {
            if (entry.type_args[i] != type_args[i]) {
                match = false;
                break;
            }
        }
        if (match)
            return entry.closed_type;
    }

    return 0;  // not found
}

// ═════════════════════════════════════════════════════════════
// Bulk registration from codegen-emitted data (Eager + Lazy)
// ═════════════════════════════════════════════════════════════

void RegisterModuleGenerics(const struct ModuleGenericRegistrationV0* reg) {
    if (reg == nullptr)
        return;

    auto& registry = GetRegistry();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(registry.global_mutex);

    const auto* bridge = chaos::il2cpp::bootstrap::GetCodegenBridgeV0();
    if (bridge == nullptr || bridge->resolve_type_by_token == nullptr)
        return;

    // ── Create shard ──
    auto shard = CHAOS_IL2CPP_MAKE_UNIQUE(ModuleShard)(reg->module_id, reg->source_image);

    // ── Phase 1 (Eager): Register open definitions only ──
    if (reg->generic_types != nullptr && reg->generic_type_count > 0u) {
        shard->lazy_types.reserve(reg->generic_type_count);

        for (CHAOS_IL2CPP_UINT32 i = 0u; i < reg->generic_type_count; i++) {
            const auto& entry = reg->generic_types[i];

            TypeInfoHandle open_type = bridge->resolve_type_by_token(
                reg->source_image, entry.open_token);
            if (open_type == 0)
                continue;

            // Store in global by_open_type with empty vector + shard owner.
            auto& type_entry = registry.by_open_type[open_type];
            if (type_entry.owner_shard == nullptr) {
                type_entry.owner_shard = shard.get();
                shard->open_definitions.push_back(open_type);
                shard->resolve_state.push_back(false);
            }

            // Record lazy entry (raw tokens, no resolution yet).
            LazyTypeEntry lazy;
            lazy.open_type    = open_type;
            lazy.closed_token = entry.closed_token;
            lazy.args_start   = static_cast<CHAOS_IL2CPP_UINT32>(shard->token_pool.size());
            lazy.arg_count    = entry.arg_count;

            // Copy raw arg tokens into the token pool.
            for (CHAOS_IL2CPP_UINT32 j = 0u; j < entry.arg_count; j++) {
                shard->token_pool.push_back(
                    reg->generic_type_args[entry.args_start_index + j]);
            }

            shard->lazy_types.push_back(lazy);
        }
    }

    // ── Phase 2 (Eager): Register method generic contexts (raw tokens) ──
    //
    // Note: GenericMethodRegistrationEntryV0 reuses GenericTypeRegistrationEntryV0:
    //   entry.open_token    = methodToken
    //   entry.closed_token  = classArgCount
    //   entry.arg_count     = methodArgCount
    //   entry.args_start_index = argsStartIndex
    if (reg->generic_methods != nullptr && reg->generic_method_count > 0u) {
        shard->lazy_contexts.reserve(reg->generic_method_count);

        for (CHAOS_IL2CPP_UINT32 i = 0u; i < reg->generic_method_count; i++) {
            const auto& entry = reg->generic_methods[i];

            CHAOS_IL2CPP_UINT32 method_token  = entry.open_token;
            CHAOS_IL2CPP_UINT32 class_arg_cnt = entry.closed_token;
            CHAOS_IL2CPP_UINT32 method_arg_cnt = entry.arg_count;

            if (registry.by_method_token.count(method_token) ||
                registry.method_token_owners.count(method_token))
            {
                continue;  // idempotent
            }

            LazyMethodContextEntry lazy;
            lazy.method_token = method_token;

            // class_type_args occupy the first class_arg_cnt slots.
            lazy.class_args_start = static_cast<CHAOS_IL2CPP_UINT32>(shard->token_pool.size());
            lazy.class_arg_count  = class_arg_cnt;
            for (CHAOS_IL2CPP_UINT32 j = 0u; j < class_arg_cnt; j++) {
                shard->token_pool.push_back(
                    reg->generic_method_args[entry.args_start_index + j]);
            }

            // method_type_args follow.
            lazy.method_args_start = static_cast<CHAOS_IL2CPP_UINT32>(shard->token_pool.size());
            lazy.method_arg_count  = method_arg_cnt;
            for (CHAOS_IL2CPP_UINT32 j = 0u; j < method_arg_cnt; j++) {
                shard->token_pool.push_back(
                    reg->generic_method_args[entry.args_start_index + class_arg_cnt + j]);
            }

            shard->lazy_contexts.push_back(lazy);
            registry.method_token_owners[method_token] = shard.get();
        }
    }

    // ── Insert shard into routing table ──
    if (!RoutingTableInsert(reg->module_id, shard.get())) {
        // Duplicate module_id — shard is discarded.
        return;
    }

    // Release ownership to the routing table.
    static_cast<void>(shard.release());
}

// ═════════════════════════════════════════════════════════════
// Module-level unregistration (O(1) shard drop)
// ═════════════════════════════════════════════════════════════

void UnregisterModuleGenerics(CHAOS_IL2CPP_UINT32 module_id) {
    if (module_id == 0u)
        return;

    auto& registry = GetRegistry();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(registry.global_mutex);

    ModuleShard* shard = RoutingTableRemove(module_id);
    if (shard == nullptr)
        return;

    // ── Remove all open_type entries owned by this shard ──
    for (CHAOS_IL2CPP_SIZE i = 0; i < shard->open_definitions.size(); i++) {
        TypeInfoHandle open_type = shard->open_definitions[i];
        auto type_it = registry.by_open_type.find(open_type);
        if (type_it == registry.by_open_type.end())
            continue;

        auto& closed_types = type_it->second.closed_types;

        // Remove entries whose closed_type was lazily resolved from this shard.
        // We cannot use module_id directly because multiple modules could register
        // instantiations for the same open_type. Instead, we remove entries that
        // were added during lazy resolve of this shard's open_type.
        // The only entries that belong to this shard are those added by
        // DoLazyResolveOpenType, which always sets module_id = shard->module_id.
        closed_types.erase(
            std::remove_if(closed_types.begin(), closed_types.end(),
                [&](const GenericInstantiationEntry& e) {
                    return e.module_id == shard->module_id;
                }),
            closed_types.end());

        if (closed_types.empty() && type_it->second.owner_shard == nullptr) {
            registry.by_open_type.erase(type_it);
        }
    }

    // ── Remove method token entries owned by this shard ──
    for (const auto& lazy : shard->lazy_contexts) {
        registry.by_method_token.erase(lazy.method_token);
        registry.method_token_owners.erase(lazy.method_token);
    }

    // ── Discard shard (owned_entries freed by unique_ptr, token_pool freed by vector) ──
    delete shard;
}

// ═════════════════════════════════════════════════════════════
// Method instantiation registration and lookup
// ═════════════════════════════════════════════════════════════

void RegisterGenericMethodInstantiation(
    MethodInfoHandle       open_method,
    MethodInfoHandle       closed_method,
    const TypeInfoHandle*  type_args,
    CHAOS_IL2CPP_UINT32   arg_count)
{
    if (open_method == 0u || closed_method == 0u)
        return;

    auto& registry = GetRegistry();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(registry.global_mutex);

    auto& entries = registry.by_open_method[open_method];
    for (const auto& entry : entries) {
        if (entry.closed_method == closed_method)
            return;  // idempotent
    }

    MethodInstantiationEntry entry;
    entry.open_method   = open_method;
    entry.closed_method = closed_method;
    entry.module_id     = 0u;
    if (type_args != nullptr && arg_count > 0u)
        entry.type_args.assign(type_args, type_args + arg_count);
    entries.push_back(CHAOS_IL2CPP_MOVE(entry));
}

MethodInfoHandle TryResolveClosedMethod(
    MethodInfoHandle       open_method,
    const TypeInfoHandle*  type_args,
    CHAOS_IL2CPP_UINT32   arg_count)
{
    if (open_method == 0u)
        return 0u;

    auto& registry = GetRegistry();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(registry.global_mutex);

    auto it = registry.by_open_method.find(open_method);
    if (it == registry.by_open_method.end())
        return 0u;

    for (const auto& entry : it->second) {
        if (entry.type_args.size() != arg_count)
            continue;
        bool match = true;
        for (CHAOS_IL2CPP_UINT32 i = 0u; i < arg_count; i++) {
            if (entry.type_args[i] != type_args[i]) {
                match = false;
                break;
            }
        }
        if (match)
            return entry.closed_method;
    }

    return 0u;
}

// ═════════════════════════════════════════════════════════════
// Diagnostics
// ═════════════════════════════════════════════════════════════

CHAOS_IL2CPP_UINT32 GetRegisteredInstantiationCount() {
    auto& registry = GetRegistry();
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(registry.global_mutex);
    CHAOS_IL2CPP_UINT32 count = 0u;
    for (const auto& [_, v] : registry.by_open_type) {
        count += static_cast<CHAOS_IL2CPP_UINT32>(v.closed_types.size());
    }
    return count;
}

}  // namespace chaos::il2cpp::generic_context
