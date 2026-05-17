// struct_marshal.cpp — Descriptor-driven struct marshalling for P/Invoke
//
// Implements the three struct_marshal namespace functions that the codegen
// (RuntimeHelperShapeRegistry.cs) already references in its StructureToPtr<T>,
// PtrToStructure<T>, and DestroyStructure<T> shapes.

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
        // V1: only handle blittable element types via memcpy.
        if (field.element_type != NativeElementType::None && field.size > 0) {
            CHAOS_IL2CPP_MEMCPY(native_ptr + field.offset, managed_ptr + field.offset, field.size);
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
        // V1: store managed object pointer as IntPtr (simplified).
        {
            const void* obj = *reinterpret_cast<const void* const*>(managed_ptr + field.offset);
            *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(native_ptr + field.offset) =
                reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<void*>(obj));
        }
        break;

    case StructFieldKind::LPArray:
        // V1: no-op stub — pointer-to-array marshalling deferred.
        break;
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
        if (field.element_type != NativeElementType::None && field.size > 0) {
            CHAOS_IL2CPP_MEMCPY(managed_ptr + field.offset, native_ptr + field.offset, field.size);
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
            const CHAOS_IL2CPP_INTPTR raw = *reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(native_ptr + field.offset);
            *reinterpret_cast<void**>(managed_ptr + field.offset) = reinterpret_cast<void*>(raw);
        }
        break;

    case StructFieldKind::LPArray:
        break;
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

    case StructFieldKind::NestedStruct:
        if (field.nested != nullptr) {
            DestroyMarshalledFields(field.nested, native_ptr + field.offset, runtime);
        }
        break;

    case StructFieldKind::ByValArray:
        // V1: no per-element destruction (blittable-only assumption).
        break;

    case StructFieldKind::LPArray:
        // V1: no-op.
        break;

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
    RuntimeState* runtime) {
    if (desc == nullptr || native_ptr == nullptr) return;

    if (runtime == nullptr) {
        runtime = runtime_core::GetCurrentRuntimeState();
        if (runtime == nullptr) return;
    }

    DestroyMarshalledFields(desc, native_ptr, runtime);
}

}  // namespace chaos::il2cpp::struct_marshal
