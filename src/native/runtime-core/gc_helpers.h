#ifndef CHAOS_IL2CPP_GC_HELPERS_H_
#define CHAOS_IL2CPP_GC_HELPERS_H_

#include <chaos/native_types.h>
#include "runtime_abi.h"

namespace chaos::il2cpp::runtime_core {

/* GC allocation helpers for generated code. GcAllocate returns zeroed memory
   (scanned for pointers). GcAllocateAtomic returns zeroed memory that is NOT
   scanned for pointers — use for pointer-free data (e.g. string UTF-8 bytes). */
void* GcAllocate(CHAOS_IL2CPP_SIZE size);
void* GcAllocateAtomic(CHAOS_IL2CPP_SIZE size);
RuntimeStatus TypeQueryCapability(TypeInfoHandle type, RuntimeTypeCapabilityInfoV0* out_capability_info);
RuntimeMode GetRuntimeMode();
void SetRuntimeMode(RuntimeMode mode);
bool IsMixedMode();
void* BoxValueObject(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    TypeInfoHandle value_type,
    const void* value,
    CHAOS_IL2CPP_SIZE value_size);
RuntimeStatus UnboxValueObject(
    RuntimeState* runtime_state,
    void* boxed_object,
    void* out_value,
    CHAOS_IL2CPP_SIZE out_value_size);
bool ArrayStoreReference(
    void* array_instance,
    CHAOS_IL2CPP_UINTPTR index,
    void* value);
void* ArrayLoadReference(
    void* array_instance,
    CHAOS_IL2CPP_UINTPTR index);
bool ArrayCopyReferenceRange(
    void* source_array_instance,
    CHAOS_IL2CPP_UINTPTR source_index,
    void* target_array_instance,
    CHAOS_IL2CPP_UINTPTR target_index,
    CHAOS_IL2CPP_UINTPTR length);
bool ArrayClearReferenceRange(
    void* array_instance,
    CHAOS_IL2CPP_UINTPTR start_index,
    CHAOS_IL2CPP_UINTPTR length);
bool ArrayReverseReferenceRange(
    void* array_instance,
    CHAOS_IL2CPP_UINTPTR start_index,
    CHAOS_IL2CPP_UINTPTR length);

/// Mark the object as changed (for incremental GC write barrier).
/// No-op when bdwgc is not compiled with MANUAL_VDB.
void GcEndStubbornChange(const void* obj);

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_HELPERS_H_
