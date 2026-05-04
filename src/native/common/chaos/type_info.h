#ifndef CHAOS_IL2CPP_COMMON_TYPE_INFO_H_
#define CHAOS_IL2CPP_COMMON_TYPE_INFO_H_

// Hybrid TypeInfo* type identity system.
//
// Replaces the old integer-based type_id with a TypeInfo* pointer in
// chaos_object_header.  Each type has a static constexpr TypeInfo instance
// that carries:
//   - parent  : pointer to base type's TypeInfo (nullptr for System.Object)
//   - stable_id : FNV-1a 64-bit hash — stable across builds / translation units
//   - type_shape : reference / value / interface discriminator
//
// Static AOT-compiled types are `inline constexpr` (C++17), guaranteeing
// a unique address across all TUs.  Dynamic (HotUpdate) types register at
// runtime via chaos_register_type().
//
// Pointer equality (current == target) is the identity check — no switch,
// no string compare.  This works because:
//   - Static types: inline constexpr ⇒ one address per type across all TUs
//   - Dynamic types: registered once, pointer returned from registry

#include <chaos/native_types.h>

// ── 64-bit FNV-1a hash ─────────────────────────────────────────
// The C# codegen side (NativeAotLoweringPlanner.ObjectModelUtilities.cs)
// implements the identical algorithm in ComputeStableTypeId() so that
// static and dynamic type IDs match.

inline constexpr CHAOS_IL2CPP_UINT64 chaos_compute_type_stable_id(const char* name) noexcept
{
    CHAOS_IL2CPP_UINT64 hash = 14695981039346656037ULL;  // FNV_offset_basis_64
    while (*name)
    {
        hash ^= static_cast<CHAOS_IL2CPP_UINT8>(*name++);
        hash *= 1099511628211ULL;  // FNV_prime_64
    }
    return hash;
}

// ── TypeInfo ────────────────────────────────────────────────────

struct TypeInfo {
    const TypeInfo* parent;                  // base type, nullptr = System.Object (8 bytes)
    CHAOS_IL2CPP_UINT64 stable_id;           // FNV-1a hash of type subject ID (8 bytes)
    const CHAOS_IL2CPP_UINT64* iface_map;    // sorted array of implemented interface stable_ids (8 bytes)
    CHAOS_IL2CPP_UINT32 iface_count;         // number of entries in iface_map (4 bytes)
    CHAOS_IL2CPP_UINT8  type_shape;          // 1=reference, 2=value, 3=interface (1 byte)
    // 3 bytes padding
};

static_assert(sizeof(TypeInfo) == 32,
              "TypeInfo layout: parent(8) + stable_id(8) + iface_map(8) + iface_count(4) + type_shape(1) + padding(3) = 32 bytes");

// ── Common type-shape constants ─────────────────────────────────

inline constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_reference = 1;
inline constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_value     = 2;
inline constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_interface = 3;

// ── HotUpdate dynamic type registration ─────────────────────────
//
// These functions allow registering new types at runtime for HotUpdate
// scenarios.  Dynamic TypeInfo instances are heap-allocated and stored
// in a fixed-capacity global registry.  The stable_id is computed via
// FNV-1a hash of the type name (same algorithm used at compile time by
// the C# codegen), so &TypeInfo pointer comparisons work for both
// static and dynamically-loaded types.
//
// The implementation lives in type_registry.cpp (runtime-core) to keep
// this header lightweight and avoid dragging <mutex> into all TUs.
//
// Thread safety: writes serialised via mutex; reads (find) are
// lock-free on already-published entries.

/// Maximum number of dynamically registered types (HotUpdate).
inline constexpr CHAOS_IL2CPP_SIZE kChaosMaxDynamicTypes = 256;

/// Register a new dynamic type.
///
/// Allocates a TypeInfo on the heap, populates it from the arguments,
/// computes its stable_id via FNV-1a hash of @p name, and stores it in
/// the global dynamic type registry.
///
/// @param name        Fully qualified type name (e.g. "MyAssembly/MyType").
///                    Used only for stable_id computation.
/// @param parent      Pointer to the base type's TypeInfo (nullptr for
///                    System.Object).
/// @param type_shape  One of chaos_type_shape_reference/value/interface.
/// @param out_stable_id  Optional output parameter; receives the computed
///                       stable_id on success, or 0 on failure.
///
/// @return Pointer to the newly allocated TypeInfo, or nullptr if the
///         registry is full.
TypeInfo* chaos_register_type(
    const char* name,
    const TypeInfo* parent,
    CHAOS_IL2CPP_UINT8 type_shape,
    const CHAOS_IL2CPP_UINT64* iface_map = nullptr,
    CHAOS_IL2CPP_UINT32 iface_count = 0,
    CHAOS_IL2CPP_UINT64* out_stable_id = nullptr) noexcept;

/// Find a registered dynamic type by its stable_id.
///
/// Searches the dynamic type registry.  Does NOT search static
/// (inline constexpr) TypeInfo instances — static types already have
/// known addresses accessed via `&chaos_type_info_X` symbols.
///
/// @return Pointer to the matching TypeInfo, or nullptr if not found.
const TypeInfo* chaos_find_type_by_stable_id(
    CHAOS_IL2CPP_UINT64 stable_id) noexcept;

#endif  // CHAOS_IL2CPP_COMMON_TYPE_INFO_H_
