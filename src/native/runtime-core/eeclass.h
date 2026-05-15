#ifndef CHAOS_IL2CPP_EECLASS_H_
#define CHAOS_IL2CPP_EECLASS_H_

// ═══════════════════════════════════════════════════════════════════════
// EEClass — Cold reflection metadata container (lazy-filled)
// ═══════════════════════════════════════════════════════════════════════
//
// Mirrors CoreCLR's EEClass concept.  Holds the reflection metadata that
// is rarely accessed on hot paths: type name, method/field/property/event
// lists, generic metadata.  Linked from MethodTable::cold_delta as a byte
// offset from the MethodTable base address.
//
// Two backing modes:
//   AOT:     points into constexpr ReflectionQueryTypeDescriptor data.
//            Ensure*Filled() is a no-op — the data is always present.
//   Dynamic: heap-allocated by Ensure*Filled() from PatchMetadataCache.
//
// Layout is designed for cache-friendly cold access: all read-only after
// fill, never on the hot path.
// ═══════════════════════════════════════════════════════════════════════

#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include "reflection_query_model.h"

namespace chaos::il2cpp::runtime_core {

// ── Forward declarations ──────────────────────────────────────────
struct EEClass;

// ── Lightweight arrays for lazy-filled reflection data ─────────────
// These mirror the constexpr ReflectionQuery*Descriptor arrays but are
// also usable for heap-allocated dynamic type metadata.

template <typename T>
struct LazyArray {
    const T*    data   = nullptr;   // entries (constexpr or heap)
    uint32_t    count  = 0;         // entry count
    bool        filled = false;     // true once populated

    bool IsFilled() const noexcept { return filled; }
};

// ── Reflection entry types (canonical runtime representation) ─────
// These are what the managed reflection API returns after EEClass fills.
// For AOT types the pointers alias the constexpr descriptors directly.

using ReflectionMethodEntry   = ReflectionQueryMethodDescriptor;
using ReflectionFieldEntry    = ReflectionQueryFieldDescriptor;
using ReflectionPropertyEntry = ReflectionQueryPropertyDescriptor;
using ReflectionEventEntry    = ReflectionQueryPropertyDescriptor;  // events use same shape as properties

// ── EEClass (cold section header) ─────────────────────────────────
// Allocated either as constexpr (AOT, codegen-emitted) or on the
// domain heap (dynamic types via PatchMetadataCache).
//
// Invariant: once any Ensure*Filled() completes, the corresponding
// LazyArray is populated and immutable for the type's lifetime.

struct EEClass {
    // ── Identity (set at construction, never changes) ──────────────
    const char*            name_utf8          = nullptr;
    const char*            namespace_utf8     = nullptr;
    MethodTable*           mt                 = nullptr;  // backpointer to owning MethodTable

    // ── Lazy-filled reflection metadata arrays ─────────────────────
    // For AOT types: filled by pointing into the constexpr
    //   ReflectionQueryTypeDescriptor that the codegen emitted.
    // For dynamic types: domain-allocated and populated from the
    //   patch metadata cache on first access.
    LazyArray<ReflectionMethodEntry>   methods;
    LazyArray<ReflectionFieldEntry>    fields;
    LazyArray<ReflectionPropertyEntry> properties;
    LazyArray<ReflectionEventEntry>    events;

    // ── Generic metadata ───────────────────────────────────────────
    MethodTable*           generic_type_def     = nullptr;  // null for non-generic
    uint32_t               generic_param_count  = 0;
};

// ── EEClass accessor from MethodTable ─────────────────────────────
// Uses cold_delta as a byte offset from the MethodTable base address.
// Returns nullptr if cold_delta is 0 (no EEClass attached yet).
inline EEClass* GetEEClass(MethodTable* mt) noexcept {
    if (mt == nullptr || mt->cold_delta == 0) return nullptr;
    return reinterpret_cast<EEClass*>(
        reinterpret_cast<uint8_t*>(mt) + mt->cold_delta);
}

inline const EEClass* GetEEClass(const MethodTable* mt) noexcept {
    if (mt == nullptr || mt->cold_delta == 0) return nullptr;
    return reinterpret_cast<const EEClass*>(
        reinterpret_cast<const uint8_t*>(mt) + mt->cold_delta);
}

// ── EEClass lifecycle (declarations, defined in eeclass.cpp) ──────

/// Ensure the EEClass is attached to the given MethodTable.
/// For AOT types: resolves from the constexpr ReflectionQueryTypeDescriptor
///   and attaches a stable EEClass with zero allocation.
/// For dynamic types: allocates from the domain heap and fills from
///   the corresponding PatchMetadataCache entry.
/// @return true on success, false on allocation failure or unresolvable type.
bool EnsureEEClass(MethodTable* mt) noexcept;

/// Ensure the methods array is populated in the EEClass.
/// For AOT types this is a no-op (data already present).
/// For dynamic types this fills from patch metadata.
void EnsureMethodsFilled(EEClass* ee) noexcept;

/// Ensure the fields array is populated in the EEClass.
void EnsureFieldsFilled(EEClass* ee) noexcept;

/// Ensure the properties array is populated in the EEClass.
void EnsurePropertiesFilled(EEClass* ee) noexcept;

/// Ensure the events array is populated in the EEClass.
void EnsureEventsFilled(EEClass* ee) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_EECLASS_H_