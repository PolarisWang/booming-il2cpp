// struct_marshal.cpp — Descriptor-driven struct marshalling for P/Invoke
//
// Implements the three struct_marshal namespace functions that the codegen
// (RuntimeHelperShapeRegistry.cs) already references in its StructureToPtr<T>,
// PtrToStructure<T>, and DestroyStructure<T> shapes.

#include "struct_marshal.h"
#include "runtime_core.h"

#include <chaos/type_info.h>

#include <cstring>

namespace chaos::il2cpp::struct_marshal {

namespace {

using marshal_abi::StructFieldKind;
using marshal_abi::NativeElementType;
using marshal_abi::StructFieldDescriptorV1;
using marshal_abi::StructMarshallingDescriptorV1;

// ── Forward declarations ─────────────────────────────────────────────
static void MarshalFieldsManagedToNative(
    const StructMarshallingDescriptorV1* desc,
    unsigned char* native_ptr,
    const unsigned char* managed_ptr,
    RuntimeState* runtime);

static void MarshalFieldsNativeToManaged(
    const StructMarshallingDescriptorV1* desc,
    unsigned char* managed_ptr,
    const unsigned char* native_ptr,
    RuntimeState* runtime);

static void DestroyMarshalledFields(
    const StructMarshallingDescriptorV1* desc,
    unsigned char* native_ptr,
    RuntimeState* runtime);

// ── Element-size helper for array marshalling ──────────────────────────

static uint32_t NativeElementSize(NativeElementType type) {
    switch (type) {
    case NativeElementType::U1: case NativeElementType::I1: return 1u;
    case NativeElementType::U2: case NativeElementType::I2: return 2u;
    case NativeElementType::U4: case NativeElementType::I4: return 4u;
    case NativeElementType::U8: case NativeElementType::I8: return 8u;
    case NativeElementType::R4: return 4u;
    case NativeElementType::R8: return 8u;
    default: return 0u;  // Struct or None — variable or unknown
    }
}

// ── Field-level helpers ─────────────────────────────────────────────

static void MarshalFieldManagedToNative(
    const StructFieldDescriptorV1& field,
    unsigned char* native_ptr,
    const unsigned char* managed_ptr,
    RuntimeState* runtime) {
    switch (field.kind) {
    case StructFieldKind::Blittable:
        CHAOS_IL2CPP_MEMCPY(native_ptr + field.offset, managed_ptr + field.offset, field.size);
        break;

    case StructFieldKind::GuidField:
        // System.Guid is 16 bytes and blittable in layout.
        CHAOS_IL2CPP_MEMCPY(native_ptr + field.offset, managed_ptr + field.offset, 16u);
        break;

    case StructFieldKind::BoolField:
        // C# bool (1 byte) → Win32 BOOL (4 bytes, 0/1).
        {
            const uint8_t val = *reinterpret_cast<const uint8_t*>(managed_ptr + field.offset);
            *reinterpret_cast<uint32_t*>(native_ptr + field.offset) = (val != 0) ? 1u : 0u;
        }
        break;

    case StructFieldKind::StringField:
        // Managed String object* → UTF-8 CoTaskMem pointer.
        {
            const void* managed_str = *reinterpret_cast<const void* const*>(managed_ptr + field.offset);
            if (managed_str != nullptr) {
                auto* ts = runtime_core::GetCurrentThreadState();
                CHAOS_IL2CPP_INTPTR native_buf = runtime_core::MarshalStringToCoTaskMemUtf8(runtime, ts, const_cast<void*>(managed_str));
                *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(native_ptr + field.offset) = native_buf;
            } else {
                *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(native_ptr + field.offset) = 0;
            }
        }
        break;

    case StructFieldKind::NestedStruct:
        if (field.nested != nullptr) {
            MarshalFieldsManagedToNative(
                field.nested,
                native_ptr + field.offset,
                managed_ptr + field.offset,
                runtime);
        }
        break;

    case StructFieldKind::ByValArray:
        if (field.size == 0 || field.array_count == 0) break;

        if (field.element_type != NativeElementType::Struct || field.nested == nullptr) {
            // Blittable or unknown element type: direct memcpy.
            CHAOS_IL2CPP_MEMCPY(native_ptr + field.offset, managed_ptr + field.offset, field.size);
        } else {
            // Non-blittable struct elements: iterate each with nested descriptor.
            const uint32_t elem_stride = field.size / field.array_count;
            for (uint16_t i = 0; i < field.array_count; ++i) {
                MarshalFieldsManagedToNative(
                    field.nested,
                    native_ptr + field.offset + i * elem_stride,
                    managed_ptr + field.offset + i * elem_stride,
                    runtime);
            }
        }
        break;

    case StructFieldKind::DecimalField:
        // V1: memcpy (16 bytes). Full Decimal↔COM DECIMAL conversion deferred.
        CHAOS_IL2CPP_MEMCPY(native_ptr + field.offset, managed_ptr + field.offset, 16u);
        break;

    case StructFieldKind::DateTimeField:
        // V1: memcpy (8 bytes). Ticks are effectively blittable.
        CHAOS_IL2CPP_MEMCPY(native_ptr + field.offset, managed_ptr + field.offset, 8u);
        break;

    case StructFieldKind::ObjectField:
        // Use GCHandle to keep the object rooted across the native call.
        {
            const void* obj = *reinterpret_cast<const void* const*>(managed_ptr + field.offset);
            if (obj != nullptr) {
                auto* abi = runtime_core::GetRuntimeAbiV0();
                if (abi != nullptr && abi->gc_handle_new != nullptr) {
                    const GCHandle handle = abi->gc_handle_new(runtime, const_cast<void*>(obj), true);
                    *reinterpret_cast<GCHandle*>(native_ptr + field.offset) = handle;
                } else {
                    *reinterpret_cast<CHAOS_IL2CPP_UINT64*>(native_ptr + field.offset) = 0;
                }
            } else {
                *reinterpret_cast<CHAOS_IL2CPP_UINT64*>(native_ptr + field.offset) = 0;
            }
        }
        break;

    case StructFieldKind::CustomMarshalerField:
        // Managed object → native IntPtr via ICustomMarshaler.
        {
            const void* managed_obj = *reinterpret_cast<const void* const*>(managed_ptr + field.offset);
            if (managed_obj != nullptr && field.custom_marshaler_cookie != nullptr) {
                const auto native_result = runtime_core::CustomMarshalerManagedToNative(
                    field.custom_marshaler_cookie,
                    reinterpret_cast<CHAOS_IL2CPP_INTPTR>(managed_obj));
                *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(native_ptr + field.offset) = native_result;
            } else {
                *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(native_ptr + field.offset) = 0;
            }
        }
        break;

    case StructFieldKind::LPArray: {
        // Managed array -> CoTaskMem pointer.
        const void* managed_arr = *reinterpret_cast<const void* const*>(managed_ptr + field.offset);
        if (managed_arr == nullptr) {
            *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(native_ptr + field.offset) = 0;
            break;
        }
        // Access array length via known offset: sizeof(TypeInfoHandle) = 8 bytes.
        const auto* arr_bytes = static_cast<const unsigned char*>(managed_arr);
        const auto elem_count = static_cast<uint32_t>(
            *reinterpret_cast<const CHAOS_IL2CPP_UINTPTR*>(arr_bytes + sizeof(CHAOS_IL2CPP_UINT64)));

        uint32_t elem_size = NativeElementSize(field.element_type);
        if (elem_size == 0 && field.nested != nullptr) {
            elem_size = field.nested->total_size;
        }
        if (elem_size == 0) {
            *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(native_ptr + field.offset) = 0;
            break;
        }

        const auto total_size = static_cast<CHAOS_IL2CPP_SIZE>(elem_count) * elem_size;
        const auto mem = runtime_core::MarshalAllocCoTaskMem(
            runtime, static_cast<CHAOS_IL2CPP_INT32>(total_size));
        if (mem == 0) {
            *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(native_ptr + field.offset) = 0;
            break;
        }

        auto* native_buf = reinterpret_cast<unsigned char*>(mem);
        const auto* managed_data = arr_bytes + sizeof(CHAOS_IL2CPP_UINT64) + sizeof(CHAOS_IL2CPP_UINTPTR);

        if (field.element_type == NativeElementType::Struct && field.nested != nullptr) {
            for (uint32_t i = 0; i < elem_count; ++i) {
                MarshalFieldsManagedToNative(
                    field.nested,
                    native_buf + i * elem_size,
                    managed_data + i * elem_size,
                    runtime);
            }
        } else {
            CHAOS_IL2CPP_MEMCPY(native_buf, managed_data, total_size);
        }
        *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(native_ptr + field.offset) = mem;
        break;
    }
    }
}

static void MarshalFieldNativeToManaged(
    const StructFieldDescriptorV1& field,
    unsigned char* managed_ptr,
    const unsigned char* native_ptr,
    RuntimeState* runtime) {
    switch (field.kind) {
    case StructFieldKind::Blittable:
        CHAOS_IL2CPP_MEMCPY(managed_ptr + field.offset, native_ptr + field.offset, field.size);
        break;

    case StructFieldKind::GuidField:
        CHAOS_IL2CPP_MEMCPY(managed_ptr + field.offset, native_ptr + field.offset, 16u);
        break;

    case StructFieldKind::BoolField:
        // Win32 BOOL (4 bytes) → C# bool (1 byte).
        {
            const uint32_t val = *reinterpret_cast<const uint32_t*>(native_ptr + field.offset);
            *reinterpret_cast<uint8_t*>(managed_ptr + field.offset) = (val != 0) ? 1u : 0u;
        }
        break;

    case StructFieldKind::StringField:
        // UTF-8 CoTaskMem pointer → managed String object.
        {
            const CHAOS_IL2CPP_INTPTR native_buf = *reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(native_ptr + field.offset);
            if (native_buf != 0) {
                auto* ts = runtime_core::GetCurrentThreadState();
                void* managed_str = runtime_core::MarshalPtrToStringUtf8(runtime, ts, native_buf, -1, false);
                *reinterpret_cast<void**>(managed_ptr + field.offset) = managed_str;
            } else {
                *reinterpret_cast<void**>(managed_ptr + field.offset) = nullptr;
            }
        }
        break;

    case StructFieldKind::NestedStruct:
        if (field.nested != nullptr) {
            MarshalFieldsNativeToManaged(
                field.nested,
                managed_ptr + field.offset,
                native_ptr + field.offset,
                runtime);
        }
        break;

    case StructFieldKind::ByValArray:
        if (field.size == 0 || field.array_count == 0) break;

        if (field.element_type != NativeElementType::Struct || field.nested == nullptr) {
            CHAOS_IL2CPP_MEMCPY(managed_ptr + field.offset, native_ptr + field.offset, field.size);
        } else {
            const uint32_t elem_stride = field.size / field.array_count;
            for (uint16_t i = 0; i < field.array_count; ++i) {
                MarshalFieldsNativeToManaged(
                    field.nested,
                    managed_ptr + field.offset + i * elem_stride,
                    native_ptr + field.offset + i * elem_stride,
                    runtime);
            }
        }
        break;

    case StructFieldKind::DecimalField:
        CHAOS_IL2CPP_MEMCPY(managed_ptr + field.offset, native_ptr + field.offset, 16u);
        break;

    case StructFieldKind::DateTimeField:
        CHAOS_IL2CPP_MEMCPY(managed_ptr + field.offset, native_ptr + field.offset, 8u);
        break;

    case StructFieldKind::ObjectField:
        {
            const GCHandle handle = *reinterpret_cast<const GCHandle*>(native_ptr + field.offset);
            if (handle != 0) {
                auto* abi = runtime_core::GetRuntimeAbiV0();
                if (abi != nullptr && abi->gc_handle_get != nullptr) {
                    void* obj = abi->gc_handle_get(runtime, handle);
                    *reinterpret_cast<void**>(managed_ptr + field.offset) = obj;
                } else {
                    *reinterpret_cast<void**>(managed_ptr + field.offset) = nullptr;
                }
            } else {
                *reinterpret_cast<void**>(managed_ptr + field.offset) = nullptr;
            }
        }
        break;

    case StructFieldKind::CustomMarshalerField:
        // Native IntPtr → managed object via ICustomMarshaler.
        // After conversion, call CleanUpNativeData to free the native resource.
        {
            const CHAOS_IL2CPP_INTPTR native_data =
                *reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(native_ptr + field.offset);
            if (native_data != 0 && field.custom_marshaler_cookie != nullptr) {
                void* managed_obj = reinterpret_cast<void*>(
                    runtime_core::CustomMarshalerNativeToManaged(
                        field.custom_marshaler_cookie, native_data));
                *reinterpret_cast<void**>(managed_ptr + field.offset) = managed_obj;

                // After the managed object is created, release the native data.
                runtime_core::CustomMarshalerCleanupNativeData(
                    field.custom_marshaler_cookie, native_data);
            } else {
                *reinterpret_cast<void**>(managed_ptr + field.offset) = nullptr;
            }
        }
        break;

    case StructFieldKind::LPArray: {
        // V2: Copy data back from native CoTaskMem buffer into the managed array.
        // The managed array was pre-allocated before the P/Invoke call (forward
        // marshalling direction), so its length is known from the array header.
        void* managed_arr = *reinterpret_cast<void**>(managed_ptr + field.offset);
        if (managed_arr == nullptr) break;

        const CHAOS_IL2CPP_INTPTR native_buf = *reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(native_ptr + field.offset);
        if (native_buf == 0) {
            *reinterpret_cast<void**>(managed_ptr + field.offset) = nullptr;
            break;
        }

        // Get element count from the managed array header.
        const auto* arr_bytes = static_cast<const unsigned char*>(managed_arr);
        const CHAOS_IL2CPP_UINTPTR elem_count = *reinterpret_cast<const CHAOS_IL2CPP_UINTPTR*>(
            arr_bytes + sizeof(CHAOS_IL2CPP_UINT64));

        // Determine element size from the marshalling descriptor.
        uint32_t elem_size = NativeElementSize(field.element_type);
        if (elem_size == 0 && field.nested != nullptr) {
            elem_size = field.nested->total_size;
        }
        if (elem_size == 0) break;

        const CHAOS_IL2CPP_SIZE total_bytes = static_cast<CHAOS_IL2CPP_SIZE>(elem_count) * elem_size;
        auto* native_data = reinterpret_cast<unsigned char*>(native_buf);
        auto* managed_data = static_cast<unsigned char*>(managed_arr)
            + sizeof(CHAOS_IL2CPP_UINT64) + sizeof(CHAOS_IL2CPP_UINTPTR);

        if (field.element_type == NativeElementType::Struct && field.nested != nullptr) {
            for (CHAOS_IL2CPP_UINTPTR i = 0; i < elem_count; ++i) {
                MarshalFieldsNativeToManaged(
                    field.nested,
                    managed_data + i * elem_size,
                    native_data + i * elem_size,
                    runtime);
            }
        } else {
            CHAOS_IL2CPP_MEMCPY(managed_data, native_data, total_bytes);
        }
        break;
    }
    }
}

static void DestroyFieldNative(
    const StructFieldDescriptorV1& field,
    unsigned char* native_ptr,
    RuntimeState* runtime) {
    switch (field.kind) {
    case StructFieldKind::StringField:
        {
            const CHAOS_IL2CPP_INTPTR buf = *reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(native_ptr + field.offset);
            if (buf != 0) {
                runtime_core::MarshalFreeCoTaskMem(runtime, buf);
                *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(native_ptr + field.offset) = 0;
            }
        }
        break;

    case StructFieldKind::ObjectField:
        {
            const GCHandle handle = *reinterpret_cast<const GCHandle*>(native_ptr + field.offset);
            if (handle != 0) {
                auto* abi = runtime_core::GetRuntimeAbiV0();
                if (abi != nullptr && abi->gc_handle_free != nullptr) {
                    abi->gc_handle_free(runtime, handle);
                }
                *reinterpret_cast<GCHandle*>(native_ptr + field.offset) = 0;
            }
        }
        break;

    case StructFieldKind::NestedStruct:
        if (field.nested != nullptr) {
            DestroyMarshalledFields(field.nested, native_ptr + field.offset, runtime);
        }
        break;

    case StructFieldKind::ByValArray:
        if (field.element_type == NativeElementType::Struct && field.nested != nullptr && field.array_count > 0) {
            const uint32_t elem_stride = field.size / field.array_count;
            for (uint16_t i = 0; i < field.array_count; ++i) {
                DestroyMarshalledFields(
                    field.nested,
                    native_ptr + field.offset + i * elem_stride,
                    runtime);
            }
        }
        break;

    case StructFieldKind::LPArray: {
        const CHAOS_IL2CPP_INTPTR buf = *reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(native_ptr + field.offset);
        if (buf != 0) {
            runtime_core::MarshalFreeCoTaskMem(runtime, buf);
            *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(native_ptr + field.offset) = 0;
        }
        break;
    }

    case StructFieldKind::CustomMarshalerField: {
        // Clean up native data allocated by the custom marshaler.
        const CHAOS_IL2CPP_INTPTR native_data =
            *reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(native_ptr + field.offset);
        if (native_data != 0 && field.custom_marshaler_cookie != nullptr) {
            runtime_core::CustomMarshalerCleanupNativeData(
                field.custom_marshaler_cookie, native_data);
            *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(native_ptr + field.offset) = 0;
        }
        break;
    }

    default:
        break;
    }
}

// ── Struct-level iteration ──────────────────────────────────────────

static void MarshalFieldsManagedToNative(
    const StructMarshallingDescriptorV1* desc,
    unsigned char* native_ptr,
    const unsigned char* managed_ptr,
    RuntimeState* runtime) {
    for (uint16_t i = 0; i < desc->field_count; ++i) {
        MarshalFieldManagedToNative(desc->fields[i], native_ptr, managed_ptr, runtime);
    }
}

static void MarshalFieldsNativeToManaged(
    const StructMarshallingDescriptorV1* desc,
    unsigned char* managed_ptr,
    const unsigned char* native_ptr,
    RuntimeState* runtime) {
    for (uint16_t i = 0; i < desc->field_count; ++i) {
        MarshalFieldNativeToManaged(desc->fields[i], managed_ptr, native_ptr, runtime);
    }
}

static void DestroyMarshalledFields(
    const StructMarshallingDescriptorV1* desc,
    unsigned char* native_ptr,
    RuntimeState* runtime) {
    for (uint16_t i = 0; i < desc->field_count; ++i) {
        DestroyFieldNative(desc->fields[i], native_ptr, runtime);
    }
}

}  // anonymous namespace

// ── Public API ───────────────────────────────────────────────────────

void MarshalStructManagedToNative(
    const StructMarshallingDescriptorV1* desc,
    unsigned char* native_ptr,
    const unsigned char* managed_ptr,
    RuntimeState* runtime,
    void* /*context*/) {
    if (desc == nullptr || native_ptr == nullptr || managed_ptr == nullptr) return;

    if (runtime == nullptr) {
        runtime = runtime_core::GetCurrentRuntimeState();
        if (runtime == nullptr) return;
    }

    MarshalFieldsManagedToNative(desc, native_ptr, managed_ptr, runtime);
}

void MarshalStructNativeToManaged(
    const StructMarshallingDescriptorV1* desc,
    unsigned char* managed_ptr,
    const unsigned char* native_ptr,
    RuntimeState* runtime,
    void* /*context*/) {
    if (desc == nullptr || managed_ptr == nullptr || native_ptr == nullptr) return;

    if (runtime == nullptr) {
        runtime = runtime_core::GetCurrentRuntimeState();
        if (runtime == nullptr) return;
    }

    MarshalFieldsNativeToManaged(desc, managed_ptr, native_ptr, runtime);
}

void DestroyMarshalledStruct(
    const StructMarshallingDescriptorV1* desc,
    unsigned char* native_ptr,
    RuntimeState* runtime) noexcept {
    if (desc == nullptr || native_ptr == nullptr) return;

    if (runtime == nullptr) {
        runtime = runtime_core::GetCurrentRuntimeState();
        if (runtime == nullptr) return;
    }

    DestroyMarshalledFields(desc, native_ptr, runtime);
}

}  // namespace chaos::il2cpp::struct_marshal
