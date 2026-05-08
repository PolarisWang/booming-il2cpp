#ifndef CHAOS_IL2CPP_COMMON_TYPE_INFO_H_
#define CHAOS_IL2CPP_COMMON_TYPE_INFO_H_

// A5-Trinity Type Identity System
//
// Three-layer TypeInfo layout:
//   TypeInfoHot  (32B) — hot path: parent, vtable_array, stable_id
//   TypeInfoWarm (32B) — warm path: iface_map, runtime_iface_map
//   Cold section        — reflection metadata (future)
//
// TypeInfoHot is always at offset [0] and stores warm_delta to reach
// TypeInfoWarm via GetWarmPtr().  The `using TypeInfo = TypeInfoHot`
// alias allows incremental migration of existing code.
//
// Static AOT types: inline constexpr across TUs (C++17 guarantees).
// Dynamic HotUpdate types: heap-allocated Hot+Warm pair.

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
// Maps an interface to its method slots in the type's vtable.
// Used for interface dispatch and interface implementation checks.

struct InterfaceMapEntry {
    CHAOS_IL2CPP_UINT64 iface_stable_id;    // FNV-1a hash (8 bytes)
    CHAOS_IL2CPP_UINT32 vtable_offset;      // starting slot index (4 bytes)
    CHAOS_IL2CPP_UINT32 method_count;       // method count (4 bytes)
};

static_assert(sizeof(InterfaceMapEntry) == 16,
    "InterfaceMapEntry: stable_id(8) + vtable_offset(4) + method_count(4) = 16 bytes");

// ── Forward declaration ──────────────────────────────────────────
struct TypeInfoWarm;

// ── TypeInfoHot (32B) — Hot path ────────────────────────────────
// Accessed on every virtual dispatch, type check, and object creation.
// Must fit in half a cache line (32B) so two TypeInfoHot entries
// share one L1 cache line (64B), improving parent-chain locality.
//
// Layout:
//   [0]  parent         — base type (nullptr = System.Object)
//   [8]  vtable_array   — unified vtable access pointer
//   [16] stable_id      — FNV-1a 64-bit hash (cross-module identity)
//   [24] vtable_length  — number of vtable slots
//   [28] warm_delta     — byte offset: this + warm_delta = &TypeInfoWarm
//   [30] type_shape     — 1=reference, 2=value, 3=interface
//   [31] flags          — bit[0:1]=header_kind, bit[2]=has_finalizer

struct TypeInfoHot {
    const TypeInfoHot* parent;                  // [0]  8B
    const void**       vtable_array;            // [8]  8B
    CHAOS_IL2CPP_UINT64 stable_id;             // [16] 8B
    uint32_t           vtable_length;           // [24] 4B
    uint16_t           warm_delta;              // [28] 2B — &TypeInfoWarm = this + warm_delta
    uint8_t            type_shape;              // [30] 1B
    uint8_t            flags;                   // [31] 1B
};

static_assert(sizeof(TypeInfoHot) == 32,
    "TypeInfoHot: parent(8) + vtable_array(8) + stable_id(8) + "
    "vtable_length(4) + warm_delta(2) + type_shape(1) + flags(1) = 32 bytes");

// ── TypeInfoWarm (32B) — Warm path ──────────────────────────────
// Accessed during interface dispatch, reflection type checks,
// HotUpdate interface extension, and type compatibility queries.
// Lives immediately after TypeInfoHot (at hot + warm_delta).

struct TypeInfoWarm {
    const InterfaceMapEntry* iface_map;            // [0]  8B — AOT iface_map
    const InterfaceMapEntry* runtime_iface_map;    // [8]  8B — HotUpdate (heap)
    uint32_t                 iface_count;          // [16] 4B — AOT count
    uint32_t                 runtime_iface_count;  // [20] 4B — HotUpdate count
    uint32_t                 cold_delta;           // [24] 4B — to cold section
    uint32_t                 _reserved;            // [28] 4B
};

static_assert(sizeof(TypeInfoWarm) == 32,
    "TypeInfoWarm: iface_map(8) + runtime_iface_map(8) + "
    "iface_count(4) + runtime_iface_count(4) + cold_delta(4) + reserved(4) = 32 bytes");

// ── Backward compat alias ───────────────────────────────────────
// All existing `TypeInfo*` code continues to work as `TypeInfoHot*`.
// Phase migration: old code uses `TypeInfo`, new code uses `TypeInfoHot`.

using TypeInfo = TypeInfoHot;

// ── GetWarmPtr helper ───────────────────────────────────────────
// Returns pointer to the TypeInfoWarm that follows TypeInfoHot in memory.
// The warm_delta is computed at emission time and is guaranteed to be
// a small positive value (typically 32 for V0/V1, may be larger for V2).

inline const TypeInfoWarm* GetWarmPtr(const TypeInfoHot* hot) noexcept {
    return reinterpret_cast<const TypeInfoWarm*>(
        reinterpret_cast<const uint8_t*>(hot) + hot->warm_delta);
}

inline TypeInfoWarm* GetWarmPtr(TypeInfoHot* hot) noexcept {
    return reinterpret_cast<TypeInfoWarm*>(
        reinterpret_cast<uint8_t*>(hot) + hot->warm_delta);
}

// ── TypeInfo flags ──────────────────────────────────────────────
inline constexpr CHAOS_IL2CPP_UINT8 kTypeInfoHeaderKindMask   = 0x03;
inline constexpr CHAOS_IL2CPP_UINT8 kTypeInfoHeaderKindPure   = 0x00;  // PureType
inline constexpr CHAOS_IL2CPP_UINT8 kTypeInfoHeaderKindThin   = 0x01;  // ThinLockable
inline constexpr CHAOS_IL2CPP_UINT8 kTypeInfoHeaderKindFat    = 0x02;  // Fat
inline constexpr CHAOS_IL2CPP_UINT8 kTypeInfoHasFinalizer     = 0x04;  // bit[2]

// ── TypeInfoV0 — Hot + Warm pair (no inline vtable) ──────────
// For types with no virtual methods or with an external vtable array.
// warm_delta = 32 (= sizeof(TypeInfoHot)).

struct TypeInfoV0 {
    TypeInfoHot  hot;   // 32B
    TypeInfoWarm warm;  // 32B
};

static_assert(sizeof(TypeInfoV0) == 64,
    "TypeInfoV0: hot(32) + warm(32) = 64 bytes");

static_assert(offsetof(TypeInfoV0, warm) == sizeof(TypeInfoHot),
    "TypeInfoV0 warm must immediately follow hot");

// ── TypeInfoV2 — inline vtable ─────────────────────────────────
// For types with ≤6 virtual methods.  The vtable_array field in
// TypeInfoHot is set to &inline_slots[0] (= &hot + 64 = &warm + 32).

struct TypeInfoV2 {
    TypeInfoHot  hot;                  // 32B
    TypeInfoWarm warm;                 // 32B
    const void*  inline_slots[6];      // 48B
};

static_assert(sizeof(TypeInfoV2) == 112,
    "TypeInfoV2: hot(32) + warm(32) + inline_slots[6](48) = 112 bytes");

static_assert(offsetof(TypeInfoV2, warm) == 32,
    "TypeInfoV2 warm must immediately follow hot");

static_assert(offsetof(TypeInfoV2, inline_slots) == offsetof(TypeInfoV2, warm) + sizeof(TypeInfoWarm),
    "TypeInfoV2 inline_slots must immediately follow warm");

// ── Common type-shape constants ─────────────────────────────────
inline constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_reference = 1;
inline constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_value     = 2;
inline constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_interface = 3;

// ── HotUpdate dynamic type registration ─────────────────────────
inline constexpr CHAOS_IL2CPP_SIZE kChaosMaxDynamicTypes = 256;

}  // namespace ChaosIl2cpp::Common

using namespace ChaosIl2cpp::Common;

#endif  // CHAOS_IL2CPP_COMMON_TYPE_INFO_H_
