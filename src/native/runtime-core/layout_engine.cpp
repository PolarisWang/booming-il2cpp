#include "layout_engine.h"

#include <chaos/native_types.h>

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>

namespace chaos::il2cpp::layout {

// ════════════════════════════════════════════════════════════════════════════
// Anonymous-namespace helpers
// ════════════════════════════════════════════════════════════════════════════

namespace {

// Make ReflectionQuery types accessible without qualification inside
// this anonymous namespace within chaos::il2cpp::layout.
using namespace chaos::il2cpp::runtime_core;

// ── Primitive type lookup table ──────────────────────────────────────────

struct PrimitiveEntry {
    const char* subject_id;   // e.g. "System.Int32"
    CHAOS_IL2CPP_UINT32 size;
    CHAOS_IL2CPP_UINT32 alignment;
};

static constexpr PrimitiveEntry kPrimitiveTypes[] = {
    {"System.Int8",    1u, 1u},
    {"System.Byte",    1u, 1u},
    {"System.SByte",   1u, 1u},
    {"System.Int16",   2u, 2u},
    {"System.UInt16",  2u, 2u},
    {"System.Char",    2u, 2u},
    {"System.Boolean", 1u, 1u},
    {"System.Int32",   4u, 4u},
    {"System.UInt32",  4u, 4u},
    {"System.Single",  4u, 4u},
    {"System.Int64",   8u, 8u},
    {"System.UInt64",  8u, 8u},
    {"System.Double",  8u, 8u},
    {"System.IntPtr",  static_cast<CHAOS_IL2CPP_UINT32>(sizeof(void*)), static_cast<CHAOS_IL2CPP_UINT32>(sizeof(void*))},
    {"System.UIntPtr", static_cast<CHAOS_IL2CPP_UINT32>(sizeof(void*)), static_cast<CHAOS_IL2CPP_UINT32>(sizeof(void*))},
};

static constexpr CHAOS_IL2CPP_UINT32 kPrimitiveCount =
    sizeof(kPrimitiveTypes) / sizeof(kPrimitiveTypes[0]);

/// Try to match a subject_id against the primitive type table.
/// Returns true and fills `out_size`, `out_alignment` on match.
static bool MatchPrimitiveType(
    const char* subject_id,
    CHAOS_IL2CPP_UINT32& out_size,
    CHAOS_IL2CPP_UINT32& out_alignment)
{
    if (subject_id == nullptr) {
        return false;
    }
    for (CHAOS_IL2CPP_UINT32 i = 0u; i < kPrimitiveCount; ++i) {
        if (CHAOS_IL2CPP_STRCMP(subject_id, kPrimitiveTypes[i].subject_id) == 0) {
            out_size      = kPrimitiveTypes[i].size;
            out_alignment = kPrimitiveTypes[i].alignment;
            return true;
        }
    }
    return false;
}

/// Try to get the ReflectionQueryTypeDescriptor for a TypeInfoHandle.
/// Handles both tag-encoded reflection-query handles and (module_id, token)
/// module-registry handles.  Returns nullptr on failure.
static const ReflectionQueryTypeDescriptor* ResolveTypeDescriptor(TypeInfoHandle type)
{
    if (type == 0u) {
        return nullptr;
    }

    // Try tag-encoded reflection-query handle first (fast path for runtime-instantiated types).
    const auto* desc = TryDecodeReflectionQueryTypeHandle(type);
    if (desc != nullptr) {
        return desc;
    }

    // Fall back to module registry lookup.
    using namespace runtime_core;
    const CHAOS_IL2CPP_UINT32 module_id = GetModuleId(type);
    const CHAOS_IL2CPP_UINT32 token     = GetTypeToken(type);

    if (module_id >= kMaxModules) {
        return nullptr;
    }

    const auto* module = LookupModule(module_id);
    if (module == nullptr || module->image == nullptr) {
        return nullptr;
    }

    return FindReflectionQueryTypeByToken(module->image, token);
}

/// Check whether the given TypeInfoHandle refers to a value type.
/// Uses module type_flags for module-registered types.
/// For tag-encoded handles, assumes value type (caller's responsibility).
static bool IsValueTypeHandle(TypeInfoHandle type)
{
    if (type == 0u) {
        return false;
    }

    // Tag-encoded handles come from runtime-instantiated value types.
    if (TryDecodeReflectionQueryTypeHandle(type) != nullptr) {
        return true;
    }

    // Module-registry handles — check type flags.
    using namespace runtime_core;
    const CHAOS_IL2CPP_UINT32 module_id = GetModuleId(type);
    const CHAOS_IL2CPP_UINT32 token     = GetTypeToken(type);

    if (module_id >= kMaxModules) {
        return false;
    }

    const auto* module = LookupModule(module_id);
    if (module == nullptr || module->type_flags == nullptr) {
        return false;
    }

    const CHAOS_IL2CPP_UINT32 type_index = TokenToIndex(token);
    if (type_index >= module->type_count) {
        return false;
    }

    return (module->type_flags[type_index] & kFlagIsValueType) != 0u;
}

}  // anonymous namespace

// ════════════════════════════════════════════════════════════════════════════
// RecursionGuard
// ════════════════════════════════════════════════════════════════════════════

bool RecursionGuard::Enter(TypeInfoHandle type)
{
    if (depth >= kLayoutMaxDepth) {
        return false;  // depth limit reached
    }

    // Check if `type` is already in the visiting set (cycle detection).
    for (CHAOS_IL2CPP_UINT32 i = 0u; i < visit_count; ++i) {
        if (visiting[i] == type) {
            return false;  // cycle detected
        }
    }

    // Push onto visiting set.
    visiting[visit_count++] = type;
    ++depth;
    return true;
}

void RecursionGuard::Leave()
{
    if (visit_count > 0u) {
        --visit_count;
    }
    if (depth > 0u) {
        --depth;
    }
}

RecursionGuard::ScopedEnter::ScopedEnter(RecursionGuard& g, TypeInfoHandle t)
    : guard(g)
    , ok(g.Enter(t))
{
}

RecursionGuard::ScopedEnter::~ScopedEnter()
{
    guard.Leave();
}

// ════════════════════════════════════════════════════════════════════════════
// LayoutEngine
// ════════════════════════════════════════════════════════════════════════════

// ── Destructor ──────────────────────────────────────────────────────────

LayoutEngine::~LayoutEngine()
{
    for (auto& entry : cache_) {
        if (entry.second != nullptr) {
            if (entry.second->fields != nullptr) {
                CHAOS_IL2CPP_FREE(entry.second->fields);
            }
            CHAOS_IL2CPP_FREE(entry.second);
        }
    }
    cache_.clear();
    module_index_.clear();
}

// ── Public: GetOrComputeLayout ─────────────────────────────────────────

const TypeLayout* LayoutEngine::GetOrComputeLayout(
    TypeInfoHandle         closed_type,
    const TypeInfoHandle*  type_args,
    CHAOS_IL2CPP_UINT32   arg_count)
{
    if (closed_type == 0u) {
        return nullptr;
    }

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(mutex_);

    // Check the primary cache.
    {
        auto it = cache_.find(closed_type);
        if (it != cache_.end()) {
            ++cache_hits_;
            return it->second;
        }
    }

    // Resolve the type descriptor.
    const auto* type_desc = ResolveTypeDescriptor(closed_type);
    if (type_desc == nullptr) {
        return nullptr;
    }

    // Compute the layout.
    RecursionGuard guard;
    auto* layout = ComputeLayoutInternal(closed_type, type_desc, type_args, arg_count, guard);
    if (layout == nullptr) {
        return nullptr;
    }

    // Store in cache and update the module index.
    cache_[closed_type] = layout;
    ++compute_count_;

    using namespace runtime_core;
    const CHAOS_IL2CPP_UINT32 module_id = GetModuleId(closed_type);
    if (module_id < kMaxModules) {
        module_index_[module_id].push_back(closed_type);
    }

    return layout;
}

// ── Public: InvalidateModuleCache ─────────────────────────────────────

void LayoutEngine::InvalidateModuleCache(CHAOS_IL2CPP_UINT32 module_id)
{
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(mutex_);

    auto index_it = module_index_.find(module_id);
    if (index_it == module_index_.end()) {
        return;  // no cached layouts for this module
    }

    // Free all cached TypeLayout entries belonging to this module.
    for (const auto& handle : index_it->second) {
        auto cache_it = cache_.find(handle);
        if (cache_it != cache_.end() && cache_it->second != nullptr) {
            if (cache_it->second->fields != nullptr) {
                CHAOS_IL2CPP_FREE(cache_it->second->fields);
            }
            CHAOS_IL2CPP_FREE(cache_it->second);
        }
        cache_.erase(handle);
    }

    module_index_.erase(index_it);
}

// ── Private: ComputeLayoutInternal ─────────────────────────────────────
//
// Assumes mutex_ is held by the caller.

TypeLayout* LayoutEngine::ComputeLayoutInternal(
    TypeInfoHandle                          closed_type,
    const ReflectionQueryTypeDescriptor*    type_desc,
    const TypeInfoHandle*                   type_args,
    CHAOS_IL2CPP_UINT32                    arg_count,
    RecursionGuard&                         guard)
{
    // Check the cache again (safe because caller holds the mutex).
    {
        auto it = cache_.find(closed_type);
        if (it != cache_.end()) {
            return it->second;
        }
    }

    // Cycle / depth-limit detection.
    RecursionGuard::ScopedEnter enter(guard, closed_type);
    if (!enter.ok) {
        // Cycle or excessive depth — return a conservative layout.
        auto* fallback = static_cast<TypeLayout*>(
            CHAOS_IL2CPP_MALLOC(sizeof(TypeLayout)));
        if (fallback == nullptr) {
            return nullptr;
        }
        fallback->closed_type  = closed_type;
        fallback->value_size   = static_cast<CHAOS_IL2CPP_UINT32>(sizeof(void*));
        fallback->alignment    = static_cast<CHAOS_IL2CPP_UINT32>(sizeof(void*));
        fallback->field_count  = 0u;
        fallback->fields       = nullptr;
        fallback->gc_ref_count = 0u;
        return fallback;
    }

    // ── Determine field set ──────────────────────────────────────────
    //
    // For generic types, use the open type definition's fields.
    // For non-generic types, use the type's own fields.

    const ReflectionQueryTypeDescriptor* field_source = type_desc;

    // If this is a closed generic, the fields live on the generic type definition.
    if (type_desc->generic_type_definition != nullptr &&
        type_desc->field_count == 0u &&
        type_desc->generic_type_definition->fields != nullptr)
    {
        field_source = type_desc->generic_type_definition;
    }

    const auto field_count    = field_source->field_count;
    const auto* field_descs   = field_source->fields;

    // ── Allocate FieldLayout array ─────────────────────────────────
    if (field_count == 0u || field_descs == nullptr) {
        // Empty struct — ECMA convention: size = 1, alignment = 1.
        auto* layout = static_cast<TypeLayout*>(
            CHAOS_IL2CPP_MALLOC(sizeof(TypeLayout)));
        if (layout == nullptr) {
            return nullptr;
        }
        layout->closed_type  = closed_type;
        layout->value_size   = 1u;
        layout->alignment    = 1u;
        layout->field_count  = 0u;
        layout->fields       = nullptr;
        layout->gc_ref_count = 0u;
        return layout;
    }

    auto* field_layouts = static_cast<FieldLayout*>(
        CHAOS_IL2CPP_MALLOC(sizeof(FieldLayout) * field_count));
    if (field_layouts == nullptr) {
        return nullptr;
    }

    // ── Layout each field ──────────────────────────────────────────
    CHAOS_IL2CPP_UINT32 current_offset = 0u;
    CHAOS_IL2CPP_UINT32 gc_ref_count   = 0u;

    for (CHAOS_IL2CPP_UINT32 i = 0u; i < field_count; ++i) {
        const auto& field_desc = field_descs[i];
        auto& out = field_layouts[i];

        // Resolve field type via generic parameter substitution.
        const TypeInfoHandle resolved_field_type =
            ResolveFieldType(field_desc, type_args, arg_count);

        // Resolve the field's size and alignment.
        SizeAndAlignment sa{};
        if (resolved_field_type != 0u) {
            sa = ResolveSizeAndAlignmentInternal(resolved_field_type, guard);
        } else {
            // Unknown field type — use conservative pointer-sized fallback.
            sa.size      = static_cast<CHAOS_IL2CPP_UINT32>(sizeof(void*));
            sa.alignment = static_cast<CHAOS_IL2CPP_UINT32>(sizeof(void*));
        }

        // Apply alignment to compute field offset.
        const CHAOS_IL2CPP_UINT32 field_offset = AlignUp(current_offset, sa.alignment);

        out.resolved_type  = resolved_field_type;
        out.offset         = field_offset;
        out.size           = sa.size;
        out.alignment      = sa.alignment;
        out.is_gc_reference = false;  // GC reference tracking TBD in later phase

        current_offset = field_offset + sa.size;

        // Track GC references — for now, mark any non-value-type,
        // non-primitive field as potential GC ref.  Refined in later phases.
        if (resolved_field_type != 0u &&
            !IsValueTypeHandle(resolved_field_type))
        {
            out.is_gc_reference = true;
            ++gc_ref_count;
        }
    }

    // ── Compute struct alignment and total size ────────────────────
    const CHAOS_IL2CPP_UINT32 struct_alignment =
        ComputeStructAlignment(field_layouts, field_count);

    // The total value size is the max of AlignUp(current_offset, struct_alignment)
    // and struct_alignment itself (per ECMA-335 II.12.2.4).
    CHAOS_IL2CPP_UINT32 value_size = AlignUp(current_offset, struct_alignment);
    if (value_size < struct_alignment) {
        value_size = struct_alignment;
    }

    // ── Allocate and populate TypeLayout ──────────────────────────
    auto* layout = static_cast<TypeLayout*>(
        CHAOS_IL2CPP_MALLOC(sizeof(TypeLayout)));
    if (layout == nullptr) {
        CHAOS_IL2CPP_FREE(field_layouts);
        return nullptr;
    }

    layout->closed_type  = closed_type;
    layout->value_size   = value_size;
    layout->alignment    = struct_alignment;
    layout->field_count  = field_count;
    layout->fields       = field_layouts;
    layout->gc_ref_count = gc_ref_count;

    return layout;
}

// ── Private: ResolveSizeAndAlignmentInternal ──────────────────────────
//
// Assumes mutex_ is held by the caller.

LayoutEngine::SizeAndAlignment LayoutEngine::ResolveSizeAndAlignmentInternal(
    TypeInfoHandle   type,
    RecursionGuard&  guard)
{
    SizeAndAlignment result{};
    result.size      = static_cast<CHAOS_IL2CPP_UINT32>(sizeof(void*));
    result.alignment = static_cast<CHAOS_IL2CPP_UINT32>(sizeof(void*));

    if (type == 0u) {
        return result;
    }

    // ── Cache check (safe because caller holds the mutex) ────────────
    {
        auto it = cache_.find(type);
        if (it != cache_.end() && it->second != nullptr) {
            result.size      = it->second->value_size;
            result.alignment = it->second->alignment;
            return result;
        }
    }

    // ── Resolve type descriptor ──────────────────────────────────────
    const auto* desc = ResolveTypeDescriptor(type);
    if (desc == nullptr) {
        return result;
    }

    // ── Primitive type check ─────────────────────────────────────────
    {
        CHAOS_IL2CPP_UINT32 prim_size = 0u;
        CHAOS_IL2CPP_UINT32 prim_align = 0u;
        if (MatchPrimitiveType(desc->subject_id_utf8, prim_size, prim_align)) {
            result.size      = prim_size;
            result.alignment = prim_align;
            return result;
        }
    }

    // ── Reference type check ─────────────────────────────────────────
    if (!IsValueTypeHandle(type)) {
        // Reference type fields occupy one pointer slot in a struct layout.
        result.size      = static_cast<CHAOS_IL2CPP_UINT32>(sizeof(void*));
        result.alignment = static_cast<CHAOS_IL2CPP_UINT32>(sizeof(void*));
        return result;
    }

    // ── Value type: compute layout recursively ───────────────────────
    //
    // For closed generic value types, we need type args.
    // If this is a runtime-instantiated type (tag-encoded handle), the
    // type args come from the original call.  For a nested value type
    // without pre-computed cache, we attempt to compute from the descriptor.
    //
    // If we cannot determine type args, we fall back to a conservative
    // pointer-sized layout.

    // Determine the type args for this sub-type.
    // For non-generic value types, type_args is not needed.
    const TypeInfoHandle* sub_type_args = nullptr;
    CHAOS_IL2CPP_UINT32 sub_arg_count = 0u;

    if (desc->generic_type_definition != nullptr) {
        // This is a closed generic — we need type args but can't recover
        // them from the handle alone.  Fall back to pointer-sized.
        // (Full recursive generic resolution is a Phase 4+ enhancement.)
        return result;
    }

    // Compute layout for this non-generic value type.
    auto* sub_layout = ComputeLayoutInternal(type, desc, sub_type_args, sub_arg_count, guard);
    if (sub_layout != nullptr) {
        // Store in cache (caller holds mutex, so this is safe).
        cache_[type] = sub_layout;
        using namespace runtime_core;
        const CHAOS_IL2CPP_UINT32 module_id = GetModuleId(type);
        if (module_id < kMaxModules) {
            module_index_[module_id].push_back(type);
        }

        result.size      = sub_layout->value_size;
        result.alignment = sub_layout->alignment;
    }

    return result;
}

// ── Private: ResolveFieldType ─────────────────────────────────────────

TypeInfoHandle LayoutEngine::ResolveFieldType(
    const ReflectionQueryFieldDescriptor& field,
    const TypeInfoHandle*                  type_args,
    CHAOS_IL2CPP_UINT32                  arg_count)
{
    if (field.member_type_utf8 == nullptr) {
        return 0u;
    }

    const char* member_type = field.member_type_utf8;

    // ── Generic parameter substitution ───────────────────────────────
    //
    // ECMA format: "!N" = class type arg N, "!!N" = method type arg N.
    if (member_type[0] == '!') {
        if (member_type[1] == '!') {
            // Method type parameter (!!N) — not yet supported in Phase 3.
            return 0u;
        }

        // Class type parameter (!N).
        const char* digits = member_type + 1;
        char* end = nullptr;
        const CHAOS_IL2CPP_UINT32 index =
            static_cast<CHAOS_IL2CPP_UINT32>(std::strtoul(digits, &end, 10));

        if (end == digits || (*end != '\0' && *end != ' ' && *end != ',' && *end != '>')) {
            return 0u;  // malformed index
        }

        if (type_args != nullptr && index < arg_count) {
            return type_args[index];
        }

        return 0u;  // index out of range
    }

    // ── Well-known type name ─────────────────────────────────────────
    //
    // For member_type_utf8 values like "System.Int32", look up the
    // TypeInfoHandle by scanning registered modules.
    return FindTypeByName(member_type);
}

// ── Private: FindTypeByName ──────────────────────────────────────────

TypeInfoHandle LayoutEngine::FindTypeByName(const char* fully_qualified_name)
{
    if (fully_qualified_name == nullptr || fully_qualified_name[0] == '\0') {
        return 0u;
    }

    using namespace runtime_core;

    // Scan all registered modules.
    for (CHAOS_IL2CPP_UINT32 module_id = 0u; module_id < kMaxModules; ++module_id) {
        const auto* module = LookupModule(module_id);
        if (module == nullptr || module->image == nullptr) {
            continue;
        }

        // Search by fully qualified name (namespace + "." + name).
        // The `fully_qualified_name` is expected in "Namespace.Type" form.
        // We split on the last '.' to get namespace and type name.
        const char* dot = std::strrchr(fully_qualified_name, '.');
        if (dot == nullptr) {
            // No namespace — search by name directly.
            const auto* type_desc = FindReflectionQueryTypeByName(
                module->image, "", fully_qualified_name);
            if (type_desc != nullptr) {
                return MakeTypeHandle(module_id, type_desc->metadata_token);
            }
        } else {
            // Namespace is everything before the last '.'; name is after.
            // Build a temporary null-terminated namespace string.
            const CHAOS_IL2CPP_SIZE ns_len =
                static_cast<CHAOS_IL2CPP_SIZE>(dot - fully_qualified_name);
            const char* type_name = dot + 1;

            // Stack buffer for most namespaces (up to 256 chars).
            char ns_buf[256];
            if (ns_len < sizeof(ns_buf)) {
                std::memcpy(ns_buf, fully_qualified_name, ns_len);
                ns_buf[ns_len] = '\0';

                const auto* type_desc = FindReflectionQueryTypeByName(
                    module->image, ns_buf, type_name);
                if (type_desc != nullptr) {
                    return MakeTypeHandle(module_id, type_desc->metadata_token);
                }
            } else {
                // Fallback for long namespaces.
                auto* ns_heap = static_cast<char*>(CHAOS_IL2CPP_MALLOC(ns_len + 1));
                if (ns_heap == nullptr) {
                    continue;
                }
                std::memcpy(ns_heap, fully_qualified_name, ns_len);
                ns_heap[ns_len] = '\0';

                const auto* type_desc = FindReflectionQueryTypeByName(
                    module->image, ns_heap, type_name);
                CHAOS_IL2CPP_FREE(ns_heap);

                if (type_desc != nullptr) {
                    return MakeTypeHandle(module_id, type_desc->metadata_token);
                }
            }
        }
    }

    return 0u;  // not found
}

// ── Static: AlignUp ───────────────────────────────────────────────────

CHAOS_IL2CPP_UINT32 LayoutEngine::AlignUp(
    CHAOS_IL2CPP_UINT32 offset,
    CHAOS_IL2CPP_UINT32 alignment)
{
    if (alignment == 0u) {
        return offset;
    }
    return (offset + alignment - 1u) & ~(alignment - 1u);
}

// ── Static: NaturalAlignment ──────────────────────────────────────────

CHAOS_IL2CPP_UINT32 LayoutEngine::NaturalAlignment(CHAOS_IL2CPP_UINT32 size)
{
    if (size == 0u) return 1u;
    if (size >= 8u) return 8u;
    if (size >= 4u) return 4u;
    if (size >= 2u) return 2u;
    return 1u;
}

// ── Static: ComputeStructAlignment ────────────────────────────────────
//
// ECMA-335 II.12.2.4: struct alignment is the maximum of its field
// alignments, capped at sizeof(void*).

CHAOS_IL2CPP_UINT32 LayoutEngine::ComputeStructAlignment(
    const FieldLayout* fields,
    CHAOS_IL2CPP_UINT32 field_count)
{
    if (fields == nullptr || field_count == 0u) {
        return 1u;
    }

    CHAOS_IL2CPP_UINT32 max_alignment = 1u;
    for (CHAOS_IL2CPP_UINT32 i = 0u; i < field_count; ++i) {
        if (fields[i].alignment > max_alignment) {
            max_alignment = fields[i].alignment;
        }
    }

    // Cap at pointer size per ECMA-335 II.12.2.4.
    const CHAOS_IL2CPP_UINT32 ptr_size =
        static_cast<CHAOS_IL2CPP_UINT32>(sizeof(void*));
    if (max_alignment > ptr_size) {
        max_alignment = ptr_size;
    }

    return max_alignment;
}

// ════════════════════════════════════════════════════════════════════════════
// VerifyModuleLayouts
// ════════════════════════════════════════════════════════════════════════════
//
// Placeholder — actual implementation requires ModuleDescriptor to carry
// LayoutExpectation data, which is emitted by codegen (Step 5).

void VerifyModuleLayouts(const runtime_core::ModuleDescriptor* /*module*/)
{
    // Step 5: iterate module->layout_expectations and assert each match.
    // For now, this is a no-op.
}

// ════════════════════════════════════════════════════════════════════════════
// GetLayoutEngine (singleton)
// ════════════════════════════════════════════════════════════════════════════

LayoutEngine* GetLayoutEngine()
{
    static LayoutEngine s_engine;
    return &s_engine;
}

}  // namespace chaos::il2cpp::layout
