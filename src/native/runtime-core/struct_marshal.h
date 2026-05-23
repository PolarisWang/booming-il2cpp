#ifndef CHAOS_IL2CPP_STRUCT_MARSHAL_H_
#define CHAOS_IL2CPP_STRUCT_MARSHAL_H_

// struct_marshal.h — Descriptor-driven struct marshalling for P/Invoke.
//
// Provides StructureToPtr<T>, PtrToStructure<T>, and DestroyStructure<T>
// semantics via codegen-emitted StructMarshallingDescriptorV1 plans.
//
// Struct and field descriptor types are declared in <marshal_abi.h>
// (namespace chaos::il2cpp::marshal_abi).
//
// Descriptor registration/lookup functions are in
// chaos::il2cpp::runtime_core (declared in runtime_core.h).

#include <marshal_abi.h>

// Forward declarations at file scope (not inside namespace) to match the
// definition in header_layouts.cpp. When included in standalone TUs alongside
// marshal_internal.h (which also forward-declares these at file scope), the
// declarations refer to the same ::RuntimeState entity.
struct RuntimeState;

namespace chaos::il2cpp::struct_marshal {

/// Copy managed struct fields → native representation using descriptor.
/// Calls MarshalStringToCoTaskMemUtf8 for StringField, allocates CoTaskMem
/// for LPArray, and uses ICustomMarshaler for CustomMarshalerField.
/// Managed → native direction only; does NOT free previous native buffers.
void MarshalStructManagedToNative(
    const marshal_abi::StructMarshallingDescriptorV1* desc,
    unsigned char* native_ptr,
    const unsigned char* managed_ptr,
    RuntimeState* runtime = nullptr,
    void* context = nullptr);

/// Copy native representation → managed struct fields using descriptor.
/// Calls MarshalPtrToStringUtf8 for StringField, frees old CoTaskMem, and
/// calls CleanUpNativeData for CustomMarshalerField after conversion.
void MarshalStructNativeToManaged(
    const marshal_abi::StructMarshallingDescriptorV1* desc,
    unsigned char* managed_ptr,
    const unsigned char* native_ptr,
    RuntimeState* runtime = nullptr,
    void* context = nullptr);

/// Free native resources allocated during managed→native marshalling.
/// Frees CoTaskMem buffers (StringField, LPArray), GCHandles (ObjectField),
/// and calls CleanUpNativeData (CustomMarshalerField).
/// Safe to call on already-cleared native buffers (null/zero fields are no-op).
void DestroyMarshalledStruct(
    const marshal_abi::StructMarshallingDescriptorV1* desc,
    unsigned char* native_ptr,
    RuntimeState* runtime = nullptr) noexcept;

}  // namespace chaos::il2cpp::struct_marshal

#endif  // CHAOS_IL2CPP_STRUCT_MARSHAL_H_
