#ifndef CHAOS_IL2CPP_LAYOUT_ENGINE_H_
#define CHAOS_IL2CPP_LAYOUT_ENGINE_H_

#include <chaos/native_types.h>

#include <runtime_abi.h>
#include "module_registry.h"
#include "reflection_query_model.h"

#include <cstddef>
#include <cstdint>

namespace chaos::il2cpp::layout {

// ── Constants ──────────────────────────────────────────────────────────

/// Maximum recursion depth when resolving nested value type layouts.
constexpr uint32_t kLayoutMaxDepth = 64u;

/// Maximum visited types for the optional SizeAndAlignment cache.
constexpr uint32_t kLayoutMaxVisitedSize = 256u;

// ── RecursionGuard ─────────────────────────────────────────────────────
//
// Cycle detection for recursive struct layout computation.
// Tracks the set of TypeInfoHandle values currently being resolved.
// If we encounter a type already in the visiting set, a cycle is detected.

struct RecursionGuard {
    TypeInfoHandle visiting[kLayoutMaxDepth];  // cycle-detection path stack
    CHAOS_IL2CPP_UINT32 visit_count = 0u;
    CHAOS_IL2CPP_UINT32 depth = 0u;

    /// Push `type` onto the visiting set.  Returns false if `type` is
    /// already in the set (cycle detected).
    bool Enter(TypeInfoHandle type);

    /// Pop the most-recently entered type from the visiting set.
    void Leave();

    /// RAII helper: calls Enter() on construction, Leave() on destruction.
    struct ScopedEnter {
        RecursionGuard& guard;
        bool ok;
        ScopedEnter(RecursionGuard& g, TypeInfoHandle t);
        ~ScopedEnter();
    };
};

// ── FieldLayout ────────────────────────────────────────────────────────
//
// Resolved layout for a single field within a value type.

struct FieldLayout {
    TypeInfoHandle resolved_type;    ///< Resolved type handle (0 = unknown/resolution-failed)
    CHAOS_IL2CPP_UINT32 offset;     ///< Byte offset from struct start
    CHAOS_IL2CPP_UINT32 size;       ///< Field size in bytes
    CHAOS_IL2CPP_UINT32 alignment;  ///< Field alignment in bytes
    bool is_gc_reference;           ///< True if the field holds a GC object reference
};

// ── TypeLayout ─────────────────────────────────────────────────────────
//
// Complete layout for a closed value type.  Heap-allocated and owned
// by the LayoutEngine cache.  Callers must NOT free the returned pointer.

struct TypeLayout {
    TypeInfoHandle closed_type;             ///< The type this layout was computed for
    CHAOS_IL2CPP_UINT32 value_size;        ///< Total struct size in bytes (with padding)
    CHAOS_IL2CPP_UINT32 alignment;         ///< Struct alignment in bytes
    CHAOS_IL2CPP_UINT32 field_count;       ///< Number of fields
    FieldLayout* fields;                   ///< Heap-allocated array [field_count]
    CHAOS_IL2CPP_UINT32 gc_ref_count;     ///< Number of GC-reference fields
};

// ── LayoutEngine ───────────────────────────────────────────────────────
//
// Single-responsibility layout computation engine.
//
// Given a closed value type's TypeInfoHandle (and optional type arguments),
// computes the complete layout: field sizes, offsets, padding, total struct
// size, and alignment.  Results are cached on a per-TypeInfoHandle basis
// and indexed by module_id for bulk invalidation on hot-unload.
//
// Thread-safe: public methods acquire an internal mutex.  Recursive calls
// during layout computation assume the mutex is already held by the same
// thread (the engine uses a split internal / external API to avoid
// recursive-locking overhead).

class LayoutEngine {
public:
    LayoutEngine() = default;
    ~LayoutEngine();

    // Non-copyable, non-movable.
    LayoutEngine(const LayoutEngine&) = delete;
    LayoutEngine& operator=(const LayoutEngine&) = delete;

    // ── Public API ──────────────────────────────────────────────────────

    /// Return (compute or cached) the layout for a closed value type.
    ///
    /// @param closed_type  The TypeInfoHandle of the closed type.
    /// @param type_args    Type argument handles for generic substitution.
    ///                     Pass nullptr for non-generic types.
    /// @param arg_count    Number of type arguments (0 for non-generic).
    /// @return             Pointer to the layout, or nullptr on failure.
    const TypeLayout* GetOrComputeLayout(
        TypeInfoHandle closed_type,
        const TypeInfoHandle* type_args = nullptr,
        CHAOS_IL2CPP_UINT32 arg_count = 0u);

    /// Invalidate all cached layouts that belong to a given module.
    /// Called during hot-update package unloading.
    void InvalidateModuleCache(CHAOS_IL2CPP_UINT32 module_id);

    // ── Diagnostics ─────────────────────────────────────────────────────

    CHAOS_IL2CPP_UINT32 GetCacheHitCount() const { return cache_hits_; }
    CHAOS_IL2CPP_UINT32 GetComputeCount() const  { return compute_count_; }

private:
    // ── Internal types ──────────────────────────────────────────────────

    struct SizeAndAlignment {
        CHAOS_IL2CPP_UINT32 size;
        CHAOS_IL2CPP_UINT32 alignment;
    };

    // ── Internal methods (caller MUST hold mutex_) ──────────────────────

    /// Compute layout for a type whose descriptor is already resolved.
    /// Assumes `mutex_` is held by the calling thread.
    TypeLayout* ComputeLayoutInternal(
        TypeInfoHandle closed_type,
        const runtime_core::ReflectionQueryTypeDescriptor* type_desc,
        const TypeInfoHandle* type_args,
        CHAOS_IL2CPP_UINT32 arg_count,
        RecursionGuard& guard);

    /// Resolve the size and alignment of a type handle.
    /// For value types, this recursively triggers ComputeLayoutInternal.
    /// Assumes `mutex_` is held by the calling thread.
    SizeAndAlignment ResolveSizeAndAlignmentInternal(
        TypeInfoHandle type,
        RecursionGuard& guard);

    /// Parse a field descriptor's member_type_utf8 and return the
    /// resolved TypeInfoHandle after generic parameter substitution.
    TypeInfoHandle ResolveFieldType(
        const runtime_core::ReflectionQueryFieldDescriptor& field,
        const TypeInfoHandle* type_args,
        CHAOS_IL2CPP_UINT32 arg_count);

    /// Scan all registered modules to find a type by fully qualified name.
    TypeInfoHandle FindTypeByName(const char* fully_qualified_name);

    /// Resolve a compound generic type field signature.
    ///
    /// Parses ECMA field signature format "Namespace.Type`N[arg1, arg2]"
    /// and resolves via the runtime instantiation bridge.
    /// e.g. "System.Collections.Generic.List`1[!0]" resolves the List<T>
    /// value type with T = type_args[0].
    TypeInfoHandle ResolveCompoundGenericType(
        const char* member_type,
        const TypeInfoHandle* type_args,
        CHAOS_IL2CPP_UINT32 arg_count);

    // ── Alignment helpers ───────────────────────────────────────────────

    static CHAOS_IL2CPP_UINT32 AlignUp(
        CHAOS_IL2CPP_UINT32 offset,
        CHAOS_IL2CPP_UINT32 alignment);

    static CHAOS_IL2CPP_UINT32 NaturalAlignment(
        CHAOS_IL2CPP_UINT32 size);

    static CHAOS_IL2CPP_UINT32 ComputeStructAlignment(
        const FieldLayout* fields,
        CHAOS_IL2CPP_UINT32 field_count);

    // ── Cache state ─────────────────────────────────────────────────────

    /// Primary cache: TypeInfoHandle → heap-allocated TypeLayout.
    CHAOS_IL2CPP_UNORDERED_MAP(TypeInfoHandle, TypeLayout*) cache_;

    /// Module index: module_id → list of TypeInfoHandle entries in `cache_`.
    /// Allows O(1) bulk invalidation when a module is unloaded.
    CHAOS_IL2CPP_UNORDERED_MAP(
        CHAOS_IL2CPP_UINT32,
        CHAOS_IL2CPP_VECTOR(TypeInfoHandle)) module_index_;

    /// Mutex protecting cache_ and module_index_.
    CHAOS_IL2CPP_MUTEX mutex_;

    // ── Counters ────────────────────────────────────────────────────────

    CHAOS_IL2CPP_UINT32 cache_hits_ = 0u;
    CHAOS_IL2CPP_UINT32 compute_count_ = 0u;
};

// ── LayoutExpectation ──────────────────────────────────────────────────
//
// Bootstrap verification data.  Codegen emits an array of these for each
// module, and runtime_core.cpp calls VerifyModuleLayouts() during module
// registration to assert that LayoutEngine produces the expected output.
// This catches layout-algorithm deviations early.

struct LayoutExpectation {
    CHAOS_IL2CPP_UINT32 type_token;
    CHAOS_IL2CPP_UINT32 expected_size;
    CHAOS_IL2CPP_UINT32 field_count;

    /// Per-field expectation (max 16 fields — sufficient for Phase 3).
    struct FieldExpectation {
        CHAOS_IL2CPP_UINT32 offset;
        CHAOS_IL2CPP_UINT32 size;
        CHAOS_IL2CPP_UINT32 alignment;
    } fields[16];
};

/// Verify that all LayoutExpectation entries for the given module match
/// the layouts computed by the LayoutEngine.  Asserts on mismatch.
/// Called during module registration in runtime_core.cpp.
void VerifyModuleLayouts(const runtime_core::ModuleDescriptor* module);

// ── Singleton accessor ─────────────────────────────────────────────────

/// Returns a pointer to the process-wide LayoutEngine singleton.
/// The singleton is lazily created on first access and lives for the
/// lifetime of the process.
LayoutEngine* GetLayoutEngine();

}  // namespace chaos::il2cpp::layout

#endif  // CHAOS_IL2CPP_LAYOUT_ENGINE_H_
