#ifndef CHAOS_IL2CPP_MARSHAL_COPY_HELPERS_H_
#define CHAOS_IL2CPP_MARSHAL_COPY_HELPERS_H_

#include <chaos/native_types.h>
#include <runtime_abi.h>

namespace chaos::il2cpp::marshal_copy_helpers {

/// Minimal managed array layout for Marshal.Copy fast-path codegen helpers.
/// Layout matches the internal ArrayHeader in runtime_core.cpp.
struct ManagedArrayLayout {
    TypeInfoHandle element_type;
    CHAOS_IL2CPP_UINTPTR length;
};

/// Number of bytes to skip past the array header to reach element storage.
constexpr CHAOS_IL2CPP_SIZE kArrayHeaderSize = sizeof(ManagedArrayLayout);

/// Copy COUNT elements of type TElement from unmanaged SOURCE into managed
/// DEST_ARRAY at element offset START_INDEX. Bounds-checks against the array
/// length and aborts on overflow or null.
template<typename TElement>
inline void MarshalCopyPtrToArray(
    CHAOS_IL2CPP_INTPTR source,
    void* dest_array,
    CHAOS_IL2CPP_INT32 start_index,
    CHAOS_IL2CPP_INT32 length)
{
    if (dest_array == nullptr) { CHAOS_IL2CPP_ABORT(); }
    auto* header = static_cast<ManagedArrayLayout*>(dest_array);
    const auto start = static_cast<CHAOS_IL2CPP_UINTPTR>(start_index);
    const auto count = static_cast<CHAOS_IL2CPP_UINTPTR>(length);
    if (start > header->length || count > header->length - start) { CHAOS_IL2CPP_ABORT(); }
    auto* elements = reinterpret_cast<unsigned char*>(header) + sizeof(ManagedArrayLayout);
    CHAOS_IL2CPP_MEMCPY(
        elements + start * sizeof(TElement),
        reinterpret_cast<const void*>(source),
        count * sizeof(TElement));
}

/// Copy COUNT elements of type TElement from managed SOURCE_ARRAY at element
/// offset START_INDEX into unmanaged DEST. Bounds-checks against the array
/// length and aborts on overflow or null.
template<typename TElement>
inline void MarshalCopyArrayToPtr(
    void* source_array,
    CHAOS_IL2CPP_INT32 start_index,
    CHAOS_IL2CPP_INTPTR dest,
    CHAOS_IL2CPP_INT32 length)
{
    if (source_array == nullptr) { CHAOS_IL2CPP_ABORT(); }
    auto* header = static_cast<ManagedArrayLayout*>(source_array);
    const auto start = static_cast<CHAOS_IL2CPP_UINTPTR>(start_index);
    const auto count = static_cast<CHAOS_IL2CPP_UINTPTR>(length);
    if (start > header->length || count > header->length - start) { CHAOS_IL2CPP_ABORT(); }
    auto* elements = reinterpret_cast<unsigned char*>(header) + sizeof(ManagedArrayLayout);
    CHAOS_IL2CPP_MEMCPY(
        reinterpret_cast<void*>(dest),
        elements + start * sizeof(TElement),
        count * sizeof(TElement));
}

}  // namespace chaos::il2cpp::marshal_copy_helpers

#endif  // CHAOS_IL2CPP_MARSHAL_COPY_HELPERS_H_
