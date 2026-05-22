#ifndef CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_
#define CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_

// ── Runtime API declarations exposed to generated code ──
//
// This header provides declarations used by generated .cpp files via
// runtime_core.h. All object headers use PureType (8B) or ThinLockable (16B).

#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include "arithmetic_chaos_bridge.h"
#include "codegen_bridge.h"       // HotpatchEntryV0, Hotpatch structs, kHotpatchActive
// V5: <gc.h> removed — CRAG uses its own write barriers.

// ═══════════════════════════════════════════════════════════════════
// A5-Trinity Object Header Architecture (Dual-Variant)
// ═══════════════════════════════════════════════════════════════════
// Two header variants discriminated by TypeInfo.flags[1:0]:
//
//   PureType (00):    8B  {TypeInfo* type_info}
//                       — no sync, no vtable
//   ThinLockable (01): 16B {TypeInfo* type_info, uint64_t sync_state}
//                       — thin-lock capable, dispatch via type_info->vtable_array
//
// Both store TypeInfo* at offset [0], so chaos_object_get_type_info()
// is a single *(MethodTable**)obj read — no bit magic needed.
//
// PureType: value-type boxes, sealed types with 0 virtual methods,
//           compiler-verified no-sync.
// ThinLockable: all reference types (virtual dispatch via type_info->vtable_array,
//               sync via thin-lock / SyncBlock inflation).
// ═══════════════════════════════════════════════════════════════════

// ── PureType header (8B) ──────────────────────────────────────────
// Used for value-type boxes, sealed types with no virtual methods.
// No sync_state — compiler-verified no-sync.
struct PureTypeHeader {
    const TypeInfoHot* type_info = nullptr;
};

// ── ThinLockable header (16B) ─────────────────────────────────────
// Used for all reference types. sync_state at [8] for thin locking.
// Virtual dispatch goes through type_info->vtable_array.
struct ThinLockableHeader {
    const TypeInfoHot* type_info   = nullptr;  // [0]
    uint64_t        sync_state  = 0;        // [8] — thin lock / sync block index
};


// Verify all headers store TypeInfo* at offset 0 (required by chaos_object_get_type_info).
static_assert(offsetof(PureTypeHeader, type_info) == 0, "PureTypeHeader: type_info must be at offset 0");
static_assert(offsetof(ThinLockableHeader, type_info) == 0, "ThinLockableHeader: type_info must be at offset 0");

// ── Unified type_info accessor ─────────────────────────────────────
// All header kinds store TypeInfoHot* at offset [0].
// In Phase 0 migration, TypeInfoHot* and MethodTable* are interchangeable
// (MethodTable's first 32B are bit-compatible with TypeInfoHot).
inline const TypeInfoHot* chaos_object_get_type_info(const void* obj) noexcept {
    return *static_cast<const TypeInfoHot* const*>(obj);
}

/// Accessor that returns MethodTable* directly (preferred new code).
inline const MethodTable* chaos_object_get_method_table(const void* obj) noexcept {
    auto* ti = *static_cast<const TypeInfoHot* const*>(obj);
    return ti != nullptr ? ti->AsMethodTable() : nullptr;
}

// ── Managed string type ──────────────────────────────────────────
// Used by generated code for reinterpret_cast access to string length.
struct chaos_managed_string {
    ThinLockableHeader header{};
    CHAOS_IL2CPP_INT32 length = 0;
    const char* utf8_data = nullptr;
    CHAOS_IL2CPP_UINT64 string_id = 0u;
};

// ── Managed exception type ───────────────────────────────────────
// Used by generated code for catch(chaos_managed_exception&) blocks
// and throw chaos_managed_exception{obj} statements.
struct chaos_managed_exception
{
    CHAOS_IL2CPP_INTPTR object_value = 0;
};

// Sentinel values for chaos_managed_exception::object_value.
// These indicate special system exception kinds when thrown directly from
// native safepoint code (no managed exception object available).
constexpr CHAOS_IL2CPP_INTPTR kManagedExceptionNormal          = 0;   // normal managed exception
constexpr CHAOS_IL2CPP_INTPTR kManagedExceptionThreadAbort     = -1;  // Thread.Abort
constexpr CHAOS_IL2CPP_INTPTR kManagedExceptionThreadInterrupt = -2;  // Thread.Interrupt
constexpr CHAOS_IL2CPP_INTPTR kManagedExceptionComFailure      = -3;  // COMException (HRESULT via TLS g_com_failure_hr)

#define CHAOS_IL2CPP_STRING_TYPE chaos_managed_string

// ── Exception metadata functions (extern C declarations) ──
// Implementations live in exception_api.cpp.
namespace chaos::il2cpp::runtime_core {
extern "C" {
void ChaosReflectionSetExceptionMetadata(CHAOS_IL2CPP_INTPTR exception_obj, CHAOS_IL2CPP_INTPTR message_value);
void ChaosReflectionInitDefaultException(CHAOS_IL2CPP_INTPTR exception_obj);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetExceptionMessage(CHAOS_IL2CPP_INTPTR exception_obj);
}  // extern "C"

// ── Runtime stub declarations ───────────────────────────────────
// Stub implementations in runtime_stubs/*.cpp. Each per-domain header
// declares its functions in namespace+extern "C" context.
//
// NOTE: Functions already declared in reflection_api.h (included via
// runtime_core.h) are NOT duplicated here to avoid redefinition errors.
}  // namespace chaos::il2cpp::runtime_core

// Move stubs.h include outside the namespace to avoid MSVC extern "C" +
// namespace ambiguity (C2883 / C2039).  All function declarations in
// stubs.h are extern "C" and belong at file scope.
#include "runtime_stubs/stubs.h"

// g_chaos_fail_hook: fail hook for generated code verification (setjmp/longjmp).
// Declared in chaos::il2cpp::common namespace (thread.cpp), brought to file scope
// here for generated code that references it as chaos::il2cpp::common::g_chaos_fail_hook.
namespace chaos::il2cpp::common {
extern void (*g_chaos_fail_hook)();
}  // namespace chaos::il2cpp::common

// ── Inline List<T> field layout ───────────────────────────────────────
// Used by generated code that emits direct field operations for List<T>
// methods instead of routing through CollectionList* runtime stubs.
// These fields are embedded in the GC object right after ThinLockableHeader
// (immediately after offset 16). Must match the field emission order in
// ObjectModelEmission.cs for generic List<T> types.
struct chaos_list_fields {
    CHAOS_IL2CPP_INTPTR items_array;   // [0] pointer to chaos_list_array_header + elements
    CHAOS_IL2CPP_INT32 size;           // [8] logical element count
    CHAOS_IL2CPP_INT32 version;        // [12] modification counter
};

// ── Inline List<T> internal buffer header ─────────────────────────────
// items_array points to a heap-allocated buffer where the first
// CHAOS_IL2CPP_INT32 is the capacity, followed by element data.
// Allocated via CHAOS_IL2CPP_MALLOC, freed via CHAOS_IL2CPP_FREE.
struct chaos_list_array_header {
    CHAOS_IL2CPP_INT32 capacity;  // [0] element capacity (elements start at [1])
};

// ── Enum metadata fast path types ───────────────────────────────────
// Pre-computed enum field tables generated at codegen time.
// Generated by EnumMetadataExtractor; consumed by enum_stubs.cpp.
struct EnumFieldEntry {
    const char* name;
    CHAOS_IL2CPP_INT64 value;
};
struct EnumMetadataTable {
    const EnumFieldEntry* fields;
    CHAOS_IL2CPP_UINT32 count;
};

// ── Enum dispatch table (sorted FNV-24 array for binary search) ──────
// Generated by EnumMetadataExtractor; registered via ChaosEnumRegisterDispatchTable.
// The dispatch table maps FNV-1a 24-bit hashes to EnumMetadataTable pointers,
// enabling O(log n) lookup without the large switch-case in the generated header.
struct EnumDispatchEntry {
    CHAOS_IL2CPP_UINT32 fnv24;
    const EnumMetadataTable* table;
};

// Function pointer set by ChaosEnumRegisterDispatchTable for runtime metadata lookup.
// Replaces g_chaos_resolve_enum_metadata_by_fnv24 for the common codegen-metadata case.
// Default nullptr → enum_resolve_meta falls back to g_chaos_resolve_enum_metadata_by_fnv24.
extern "C" const EnumMetadataTable* (*g_chaos_enum_dispatch_lookup)(CHAOS_IL2CPP_UINT32 fnv24) noexcept;

#endif // CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_
