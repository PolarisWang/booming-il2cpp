#ifndef CHAOS_IL2CPP_COMMON_TYPE_INFO_H_
#define CHAOS_IL2CPP_COMMON_TYPE_INFO_H_

// A5-Trinity Type Identity System — Phase 0 CoreCLR Alignment
//
// Three-layer TypeInfo layout:
//   TypeInfoHot  (32B) — hot path: parent, vtable_array, stable_id
//   TypeInfoWarm (32B) — warm path: iface_map, runtime_iface_map
//   Cold section        — reflection metadata (future: EEClass)
//
// TypeInfoHot is always at offset [0] and stores warm_delta to reach
// TypeInfoWarm via GetWarmPtr().  The `using TypeInfo = TypeInfoHot`
// alias allows incremental migration of existing code.
//
// Static AOT types: inline constexpr across TUs (C++17 guarantees).
// Dynamic HotUpdate types: heap-allocated Hot+Warm pair.
//
// ── Migration to MethodTable ──────────────────────────────────────
// MethodTable (64B) is the unified type descriptor replacing TypeInfoV0.
// Its first 32B are bit-for-bit compatible with TypeInfoHot; the full
// 64B layout matches TypeInfoV0.  This allows incremental migration:
//   - Phase 0:  MethodTable = TypeInfoV0 alias (same memory layout)
//   - Phase 1+: MethodTable warm section restructured with eeclass ptr
//   - Existing code using TypeInfoHot* continues unchanged.

#include <chaos/native_types.h>

namespace chaos::il2cpp::common {

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

// ── Forward declarations ──────────────────────────────────────────
struct TypeInfoWarm;
struct MethodTable;

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

    /// Convert to MethodTable* when this TypeInfoHot is the hot section of one.
    MethodTable*       AsMethodTable() noexcept;
    const MethodTable* AsMethodTable() const noexcept;
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
    uint32_t                 iface_bitmap;         // [28] 4B — bloom filter: bit = 1 << (stable_id & 0x1F)
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

/// Fast interface presence test via bloom filter.
/// Returns false if `stable_id` is definitely NOT in the static iface_map.
/// Returns true if it MIGHT be (may false-positive — caller must verify).
inline bool IfaceBitmapMaybeContains(const TypeInfoWarm* warm, uint64_t stable_id) noexcept {
    return (warm->iface_bitmap & (1u << (stable_id & 0x1F))) != 0;
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

// ═══════════════════════════════════════════════════════════════════
// MethodTable — CoreCLR-aligned unified type descriptor (64B)
// ═══════════════════════════════════════════════════════════════════
// Replaces TypeInfoV0 as the codegen-emitted type descriptor.
// The first 32B are bit-for-bit compatible with TypeInfoHot, enabling
// reinterpret_cast between them. The warm section now carries the
// EEClass pointer (future lazy reflection metadata).
//
// Layout comparison:
//   TypeInfoV0:       [hot:32][warm:32]       total 64B
//   MethodTable:      [hot:32][warm:32]       total 64B (Phase 0: exact match)
//   MethodTable+EECL: [hot:32][eeclass:8]...  Phase 1+: warm section restructured
//
// Static AOT types: codegen emits inline constexpr MethodTable.
// Dynamic types:    heap-allocated MethodTable + EEClass pair.

struct MethodTable {
    // ── Hot section (32B, bit-compat with TypeInfoHot) ──────────
    const MethodTable*  parent_mt;             // [0]  8B — base type MethodTable
    const void**        vtable_array;          // [8]  8B — virtual method dispatch
    uint64_t            stable_id;             // [16] 8B — FNV-1a cross-module identity
    uint32_t            vtable_length;         // [24] 4B — number of vtable slots
    uint16_t            warm_delta;            // [28] 2B — to TypeInfoWarm (typically 32)
    uint8_t             type_shape;            // [30] 1B — 1=ref, 2=value, 3=interface
    uint8_t             flags;                 // [31] 1B — header_kind, has_finalizer

    // ── Warm section (32B) — Phase 0: matches TypeInfoWarm ─────
    // Phase 1+: eeclass pointer added here, other fields reshuffled.
    const InterfaceMapEntry* iface_map;            // [32] 8B — AOT iface_map
    const InterfaceMapEntry* runtime_iface_map;    // [40] 8B — HotUpdate (heap)
    uint32_t                 iface_count;          // [48] 4B — AOT iface count
    uint32_t                 runtime_iface_count;  // [52] 4B — HotUpdate iface count
    uint32_t                 cold_delta;           // [56] 4B — to cold section / EEClass
    uint32_t                 iface_bitmap;         // [60] 4B — bloom filter: bit = 1 << (stable_id & 0x1F)

    /// Cast to TypeInfoHot* (same pointer, first 32B compatible).
    inline const TypeInfoHot* AsTypeInfoHot() const noexcept {
        return reinterpret_cast<const TypeInfoHot*>(this);
    }
    inline TypeInfoHot* AsTypeInfoHot() noexcept {
        return reinterpret_cast<TypeInfoHot*>(this);
    }
};

static_assert(sizeof(MethodTable) == 64,
    "MethodTable: hot(32) + warm(32) = 64 bytes");

// Verify MethodTable hot section is bit-compatible with TypeInfoHot.
static_assert(offsetof(MethodTable, parent_mt) == offsetof(TypeInfoHot, parent),
    "MethodTable.parent_mt must match TypeInfoHot.parent offset");
static_assert(offsetof(MethodTable, vtable_array) == offsetof(TypeInfoHot, vtable_array),
    "MethodTable.vtable_array offset must match TypeInfoHot");
static_assert(offsetof(MethodTable, stable_id) == offsetof(TypeInfoHot, stable_id),
    "MethodTable.stable_id offset must match TypeInfoHot");
static_assert(offsetof(MethodTable, vtable_length) == offsetof(TypeInfoHot, vtable_length),
    "MethodTable.vtable_length offset must match TypeInfoHot");
static_assert(offsetof(MethodTable, warm_delta) == offsetof(TypeInfoHot, warm_delta),
    "MethodTable.warm_delta offset must match TypeInfoHot");
static_assert(offsetof(MethodTable, type_shape) == offsetof(TypeInfoHot, type_shape),
    "MethodTable.type_shape offset must match TypeInfoHot");
static_assert(offsetof(MethodTable, flags) == offsetof(TypeInfoHot, flags),
    "MethodTable.flags offset must match TypeInfoHot");

// ── TypeInfoHot → MethodTable conversion ───────────────────────────
// Only valid when TypeInfoHot is the hot section of a MethodTable.
inline const MethodTable* TypeInfoHot::AsMethodTable() const noexcept {
    return reinterpret_cast<const MethodTable*>(this);
}
inline MethodTable* TypeInfoHot::AsMethodTable() noexcept {
    return reinterpret_cast<MethodTable*>(this);
}

// ═══════════════════════════════════════════════════════════════════════════
// TypeInfoHandle is always a Module Registry handle: (module_id << 32 | token).
// See module_registry.h for encode/decode helpers (GetModuleId, GetTypeToken,
// MakeTypeHandle) and resolution helpers (ResolveMethodTable, ResolveTypeDescriptor).
//
// This is a single-canonical-representation design: every TypeInfoHandle uses
// exactly one encoding, eliminating tag-bit collisions and handle identity bugs.
// ═══════════════════════════════════════════════════════════════════════════

}  // namespace chaos::il2cpp::common

using namespace chaos::il2cpp::common;

#endif  // CHAOS_IL2CPP_COMMON_TYPE_INFO_H_
