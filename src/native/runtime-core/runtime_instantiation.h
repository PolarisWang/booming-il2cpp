#ifndef CHAOS_IL2CPP_RUNTIME_CORE_RUNTIME_INSTANTIATION_H_
#define CHAOS_IL2CPP_RUNTIME_CORE_RUNTIME_INSTANTIATION_H_

#include <runtime_abi.h>
#include "reflection_query_model.h"
#include <runtime_instantiation.h>

#include <cstdint>
#include <cstdlib>
#include <new>
#include <vector>

namespace chaos::il2cpp::runtime_instantiation {

// ── Runtime-instantiated type ─────────────────────────────────────────────

/// Heap-allocated descriptor for a MakeGenericType-created type.
/// Owns the ReflectionQueryTypeDescriptor + type argument storage.
/// Lifecycle: allocated on first miss, lives for process lifetime.
struct RuntimeInstantiatedType {
    /// Core type descriptor (heap-allocated, embedded).
    chaos::il2cpp::runtime_core::ReflectionQueryTypeDescriptor descriptor;

    /// Type arguments used to close the generic type.
    TypeInfoHandle* type_args = nullptr;
    CHAOS_IL2CPP_UINT32  arg_count = 0u;

    /// Value type layout (valid when the closed type is a value type).
    CHAOS_IL2CPP_UINT32  value_size = 0u;
    CHAOS_IL2CPP_UINT32* field_offsets = nullptr;
    CHAOS_IL2CPP_UINT32  field_offset_count = 0u;

    /// Resolved field type handles (heap-allocated, optional cache).
    /// Phase 3: each field's resolved TypeInfoHandle after generic parameter
    /// substitution.  Populated by the LayoutEngine during ComputeValueTypeLayout.
    TypeInfoHandle*       resolved_field_types = nullptr;
    CHAOS_IL2CPP_UINT32  resolved_field_count = 0u;

    /// Module ownership (0 = AOT root).
    CHAOS_IL2CPP_UINT32  module_id = 0u;

    /// True once registered in the GenericContextRegistry.
    bool is_registered = false;
};

// ── Token management ──────────────────────────────────────────────────────

/// Allocate a unique runtime metadata token (0x80000000 and above).
/// These tokens never collide with AOT-allocated tokens.
CHAOS_IL2CPP_UINT32 AllocateRuntimeToken();

// ── Closed type descriptor construction ────────────────────────────────────

/// Build a heap-allocated RuntimeInstantiatedType for a closed generic type.
///
/// Copies the open type's ReflectionQueryTypeDescriptor, replaces
/// metadata_token with a runtime-allocated token, sets generic_type_definition,
/// and stores type_args for later lookup and value-type layout computation.
///
/// Returns nullptr if open_type cannot be decoded (not a reflection-query type).
RuntimeInstantiatedType* BuildClosedDescriptor(
    TypeInfoHandle         open_type_definition,
    const TypeInfoHandle*  type_args,
    CHAOS_IL2CPP_UINT32   arg_count);

/// Build a subject_id string for the closed type: "OpenType[arg1,arg2,...]"
/// The returned buffer is heap-allocated; caller takes ownership.
char* BuildClosedSubjectId(
    const chaos::il2cpp::runtime_core::ReflectionQueryTypeDescriptor* open_desc,
    const TypeInfoHandle*                type_args,
    CHAOS_IL2CPP_UINT32                  arg_count);

// ── Value type layout computation ──────────────────────────────────────────

/// Compute value_size and field_offsets for a closed value type.
/// Uses the open type's field descriptors with type argument substitution.
void ComputeValueTypeLayout(RuntimeInstantiatedType* rt_type);

// ── Bridge access ──────────────────────────────────────────────────────────

/// Returns the process-wide RuntimeInstantiationBridgeV0 instance.
/// The struct is defined in contracts/native/v0/runtime_instantiation.h
/// (typedef struct inside extern "C", at global scope).  No local forward
/// declaration needed -- the include above provides the full definition.
const RuntimeInstantiationBridgeV0* GetBridgeV0();

// ── AOT method map (generic method promotion) ────────────────────────────

/// Register per-module AOT method entries for runtime query.
/// Stores raw pointers to the codegen-emitted static arrays (no copy).
/// @param module_id   The module owning these entries.
/// @param entries     Pointer to GenericMethodAotEntryV0[] (must live for
///                    process lifetime — emitted as static const C++ array).
/// @param count       Number of entries.
/// @param args        Flat type-arg token pool (uint32_t[]).
/// @param arg_count   Number of tokens in the args pool.
void RegisterMethodAotEntries(
    CHAOS_IL2CPP_UINT32                module_id,
    const GenericMethodAotEntryV0*     entries,
    CHAOS_IL2CPP_UINT32                count,
    const uint32_t*                    args,
    CHAOS_IL2CPP_UINT32                arg_count);

/// Query the per-module AOT method map for a pre-compiled generic method.
///
/// Per-module binary search by open_token, then token-based type_args scan.
/// Type argument matching compares GetTypeToken(type_args[i]) against the
/// stored metadata tokens from the AOT entries' flat token pool.  This works
/// correctly when the type arguments belong to the same module as the generic
/// method definition (the common case).  Cross-module type arguments may
/// produce false negatives (graceful interpreter fallback).
///
/// @param module_id          Module owning the entries.
/// @param open_method_token  Metadata token of the open generic method.
/// @param type_args          Concrete type arguments (TypeInfoHandle[]).
/// @param arg_count          Number of type arguments.
/// @return Token-based MethodInfoHandle suitable for MethodInvoke dispatch,
///         or 0u if not found in the AOT map.
MethodInfoHandle QueryAotMethod(
    CHAOS_IL2CPP_UINT32                module_id,
    CHAOS_IL2CPP_UINT32                open_method_token,
    const TypeInfoHandle*              type_args,
    CHAOS_IL2CPP_UINT32                arg_count);

}  // namespace chaos::il2cpp::runtime_instantiation

#endif  // CHAOS_IL2CPP_RUNTIME_CORE_RUNTIME_INSTANTIATION_H_
