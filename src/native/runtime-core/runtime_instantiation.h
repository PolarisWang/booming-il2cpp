#ifndef CHAOS_IL2CPP_RUNTIME_INSTANTIATION_H_
#define CHAOS_IL2CPP_RUNTIME_INSTANTIATION_H_

#include "runtime_abi.h"
#include "reflection_query_model.h"
#include <chaos/runtime_instantiation.h>

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
    ReflectionQueryTypeDescriptor descriptor;

    /// Type arguments used to close the generic type.
    TypeInfoHandle* type_args = nullptr;
    CHAOS_IL2CPP_UINT32  arg_count = 0u;

    /// Value type layout (valid when the closed type is a value type).
    CHAOS_IL2CPP_UINT32  value_size = 0u;
    CHAOS_IL2CPP_UINT32* field_offsets = nullptr;
    CHAOS_IL2CPP_UINT32  field_offset_count = 0u;

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
    const ReflectionQueryTypeDescriptor* open_desc,
    const TypeInfoHandle*                type_args,
    CHAOS_IL2CPP_UINT32                  arg_count);

// ── Value type layout computation ──────────────────────────────────────────

/// Compute value_size and field_offsets for a closed value type.
/// Uses the open type's field descriptors with type argument substitution.
void ComputeValueTypeLayout(RuntimeInstantiatedType* rt_type);

// ── Bridge access ──────────────────────────────────────────────────────────

/// Returns the process-wide RuntimeInstantiationBridgeV0 instance.
const RuntimeInstantiationBridgeV0* GetBridgeV0();

}  // namespace chaos::il2cpp::runtime_instantiation

#endif  // CHAOS_IL2CPP_RUNTIME_INSTANTIATION_H_
