#ifndef CHAOS_IL2CPP_STRUCT_MARSHAL_H_
#define CHAOS_IL2CPP_STRUCT_MARSHAL_H_

#include "marshal_abi.h"
#include "runtime_abi.h"

namespace chaos::il2cpp::struct_marshal {

/// Marshal a struct from managed representation to native representation
/// by walking the descriptor tree.
/// @param desc      Descriptor tree for the struct type.
/// @param native_blob  Output: native-side blob (same offset layout as managed).
/// @param managed_blob Input: managed-side blob to convert.
/// @param rs        Runtime state (for string marshalling).
/// @param ts        Thread state (for string marshalling).
void MarshalStructManagedToNative(
    const marshal_abi::StructMarshallingDescriptorV1* desc,
    unsigned char* native_blob,
    const unsigned char* managed_blob,
    RuntimeState* rs,
    ThreadState* ts) noexcept;

/// Marshal a struct from native representation back to managed representation.
/// This is the post-call counterpart that converts the native result
/// back to managed strings and copies blittable fields.
/// @param desc      Descriptor tree for the struct type.
/// @param managed_blob Output: managed-side blob to restore.
/// @param native_blob  Input: native-side blob to read from.
/// @param rs        Runtime state (for string marshalling).
/// @param ts        Thread state (for string marshalling).
void MarshalStructNativeToManaged(
    const marshal_abi::StructMarshallingDescriptorV1* desc,
    unsigned char* managed_blob,
    const unsigned char* native_blob,
    RuntimeState* rs,
    ThreadState* ts) noexcept;

}  // namespace chaos::il2cpp::struct_marshal

#endif  // CHAOS_IL2CPP_STRUCT_MARSHAL_H_
