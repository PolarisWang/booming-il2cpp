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

namespace ChaosIl2cpp::Common {

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

// ── InterfaceMapEntry ───────────────────────────────────────────
// Each entry maps an interface type to its method slots in the type's vtable.
// Used for O(1) interface dispatch: vtable[entry.vtable_offset + method_index].
// Linear scanned during dispatch (interface count is typically 1-5).

struct InterfaceMapEntry {
    CHAOS_IL2CPP_UINT64 iface_stable_id;    // FNV-1a hash of interface subject ID (8 bytes)
    CHAOS_IL2CPP_UINT32 vtable_offset;      // starting vtable slot index for this interface (4 bytes)
    CHAOS_IL2CPP_UINT32 method_count;       // number of methods in this interface (4 bytes)
};

static_assert(sizeof(InterfaceMapEntry) == 16,
              "InterfaceMapEntry: stable_id(8) + vtable_offset(4) + method_count(4) = 16 bytes");

// ── TypeInfo ────────────────────────────────────────────────────
// Base TypeInfo (64B) shared by all type variants:
//   V0: vtable_array=nullptr, vtable_length=0 (interfaces, primitives)
//   V1: vtable_array points to external VTable_symbol[] (regular types)
//   V2: TypeInfoV2 extends with inline_slots[6], vtable_array=&inline_slots[0]

struct TypeInfo {
    const TypeInfo* parent;                  // base type, nullptr = System.Object (8 bytes)
    CHAOS_IL2CPP_UINT64 stable_id;           // FNV-1a hash of type subject ID (8 bytes)
    const InterfaceMapEntry* iface_map;      // AOT compile-time iface_map (8 bytes)
    const InterfaceMapEntry* runtime_iface_map; // HotUpdate-追加的接口映射, heap 分配 (8 bytes)
    CHAOS_IL2CPP_UINT32 iface_count;         // number of entries in iface_map (4 bytes)
    CHAOS_IL2CPP_UINT32 runtime_iface_count; // number of entries in runtime_iface_map (4 bytes)
    CHAOS_IL2CPP_UINT8  type_shape;          // 1=reference, 2=value, 3=interface (1 byte)
    CHAOS_IL2CPP_UINT8  flags;               // bit[0:1]=header_kind, bit[2]=has_finalizer (1 byte)
    // 2 bytes padding
    const void**        vtable_array;        // unified vtable access ptr (8 bytes)
    CHAOS_IL2CPP_UINT32 vtable_length;       // number of vtable slots (4 bytes)
    // 4 bytes padding
};

static_assert(sizeof(TypeInfo) == 64,
              "TypeInfo layout: parent(8) + stable_id(8) + iface_map(8) + runtime_iface_map(8) + "
              "iface_count(4) + runtime_iface_count(4) + type_shape(1) + flags(1) + padding(2) + "
              "vtable_array(8) + vtable_length(4) + padding(4) = 64 bytes");

// ── TypeInfo flags ──────────────────────────────────────────────
inline constexpr CHAOS_IL2CPP_UINT8 kTypeInfoHeaderKindMask   = 0x03;
inline constexpr CHAOS_IL2CPP_UINT8 kTypeInfoHeaderKindPure   = 0x00;  // PureType (no sync)
inline constexpr CHAOS_IL2CPP_UINT8 kTypeInfoHeaderKindThin   = 0x01;  // ThinLockable
inline constexpr CHAOS_IL2CPP_UINT8 kTypeInfoHeaderKindFat    = 0x02;  // Fat (vtable* + sync_state)
inline constexpr CHAOS_IL2CPP_UINT8 kTypeInfoHasFinalizer     = 0x04;  // bit[2]: has finalizer

// ── TypeInfoV2 — inline vtable slots ───────────────────────────
// Extends TypeInfo with inline_slots[6] for types with ≤6 virtual methods.
// vtable_array is set to &inline_slots[0] at emission time so the unified
// vtable_array[slot] access path works identically for V1 and V2.

struct TypeInfoV2 {
    TypeInfo                base;               // 64B (vtable_array = &inline_slots[0])
    const void*             inline_slots[6];    // 48B: inline vtable slots
};

static_assert(sizeof(TypeInfoV2) == 112,
              "TypeInfoV2: TypeInfo(64) + inline_slots[6](48) = 112 bytes");

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
/// @param iface_map   Pointer to array of InterfaceMapEntry for this type (optional).
/// @param iface_count Number of entries in iface_map (0 if none).
/// @param out_stable_id  Optional output parameter; receives the computed
///                       stable_id on success, or 0 on failure.
///
/// @return Pointer to the newly allocated TypeInfo, or nullptr if the
///         registry is full.
}  // namespace ChaosIl2cpp::Common

// Bring all common type identities into global scope so that all
// translation units (including generated code inside anonymous
// namespaces) can reference TypeInfo/InterfaceMapEntry etc. without
// namespace qualification.
using namespace ChaosIl2cpp::Common;

// Note: chaos_register_type() and chaos_find_type_by_stable_id() are
// now declared in src/native/runtime-core/type_registry.h under the
// chaos::il2cpp::runtime_core namespace.

#endif  // CHAOS_IL2CPP_COMMON_TYPE_INFO_H_
