#include "marshal_abi.h"
#include "engine_binding.h"
#include "runtime_core.h"

#include <cstring>

namespace chaos::il2cpp::struct_marshal {

using namespace marshal_abi;
using namespace runtime_core;

// ── Internal helpers ────────────────────────────────────────────────────────

/// Convert a 1-byte C# bool to a 4-byte Win32 BOOL.
static inline CHAOS_IL2CPP_INT32 BoolToWin32Bool(const unsigned char* src) noexcept
{
    return (*src != 0) ? 1 : 0;
}

/// Convert a 4-byte Win32 BOOL to a 1-byte C# bool.
static inline void Win32BoolToBool(unsigned char* dst, CHAOS_IL2CPP_INT32 win32_val) noexcept
{
    *dst = (win32_val != 0) ? 1 : 0;
}

/// Marshal a single element according to its NativeElementType.
static void MarshalElementManagedToNative(
    NativeElementType elem_type,
    unsigned char* dst,
    const unsigned char* src,
    CHAOS_IL2CPP_SIZE elem_size,
    RuntimeState* rs,
    ThreadState* ts) noexcept
{
    switch (elem_type) {
    case NativeElementType::U1:
    case NativeElementType::I1:
        *dst = *src;
        break;
    case NativeElementType::U2:
    case NativeElementType::I2:
        std::memcpy(dst, src, 2);
        break;
    case NativeElementType::U4:
    case NativeElementType::I4:
    case NativeElementType::R4:
        std::memcpy(dst, src, 4);
        break;
    case NativeElementType::U8:
    case NativeElementType::I8:
    case NativeElementType::R8:
        std::memcpy(dst, src, 8);
        break;
    case NativeElementType::Struct:
        // Element is itself a struct; copy the whole element as blittable.
        std::memcpy(dst, src, elem_size);
        break;
    default:
        break;
    }
}

static void MarshalElementNativeToManaged(
    NativeElementType elem_type,
    unsigned char* dst,
    const unsigned char* src,
    CHAOS_IL2CPP_SIZE elem_size) noexcept
{
    // For blittable element types, managed-to-native and native-to-managed are symmetric.
    MarshalElementManagedToNative(elem_type, dst, src, elem_size, nullptr, nullptr);
}

// ── Forward declarations for recursive calls ─────────────────────────────────

void MarshalStructManagedToNative(
    const StructMarshallingDescriptorV1* desc,
    unsigned char* native_blob,
    const unsigned char* managed_blob,
    RuntimeState* rs,
    ThreadState* ts) noexcept;

void MarshalStructNativeToManaged(
    const StructMarshallingDescriptorV1* desc,
    unsigned char* managed_blob,
    const unsigned char* native_blob,
    RuntimeState* rs,
    ThreadState* ts) noexcept;

void DestroyMarshalledStruct(
    const StructMarshallingDescriptorV1* desc,
    unsigned char* native_blob,
    RuntimeState* rs) noexcept;

// ── Managed-to-native struct marshalling ──────────────────────────────────
//
// Walks the descriptor tree and converts each field from managed
// representation to native representation.

void MarshalStructManagedToNative(
    const StructMarshallingDescriptorV1* desc,
    unsigned char* native_blob,
    const unsigned char* managed_blob,
    RuntimeState* rs,
    ThreadState* ts) noexcept
{
    for (uint16_t i = 0; i < desc->field_count; ++i) {
        const auto& field = desc->fields[i];
        auto* dst = native_blob + field.offset;
        const auto* src = managed_blob + field.offset;

        switch (field.kind) {
        case StructFieldKind::Blittable:
            std::memcpy(dst, src, field.size);
            break;

        case StructFieldKind::BoolField: {
            CHAOS_IL2CPP_INT32 native_val = BoolToWin32Bool(src);
            std::memcpy(dst, &native_val, sizeof(native_val));
            break;
        }

        case StructFieldKind::StringField: {
            CHAOS_IL2CPP_INTPTR managed_str = 0;
            std::memcpy(&managed_str, src, sizeof(managed_str));
            if (managed_str != 0) {
                auto* utf8 = MarshalStringToCoTaskMemUtf8(
                    rs, ts, reinterpret_cast<void*>(managed_str));
                std::memcpy(dst, &utf8, sizeof(utf8));
            } else {
                CHAOS_IL2CPP_INTPTR zero = 0;
                std::memcpy(dst, &zero, sizeof(zero));
            }
            break;
        }

        case StructFieldKind::NestedStruct:
            if (field.nested != nullptr) {
                MarshalStructManagedToNative(field.nested, dst, src, rs, ts);
            }
            break;

        case StructFieldKind::ByValArray: {
            CHAOS_IL2CPP_SIZE elem_size = field.size / field.array_count;
            for (uint16_t e = 0; e < field.array_count; ++e) {
                MarshalElementManagedToNative(
                    field.element_type,
                    dst + e * elem_size,
                    src + e * elem_size,
                    elem_size, rs, ts);
            }
            break;
        }

        case StructFieldKind::LPArray: {
            // LPArray: field holds a pointer-sized value (managed array reference).
            // For now, treat the pointer as opaque — copy the pointer value.
            // Full LPArray marshalling (element-by-element conversion) is a future enhancement.
            std::memcpy(dst, src, sizeof(CHAOS_IL2CPP_INTPTR));
            break;
        }

        case StructFieldKind::DecimalField:
            // .NET decimal (16 bytes) and COM DECIMAL have compatible layout.
            std::memcpy(dst, src, 16);
            break;

        case StructFieldKind::DateTimeField: {
            // .NET DateTime stores ticks as a int64 (1 tick = 100ns).
            // FILETIME is also a int64 of 100ns intervals since 1601-01-01.
            // Both use the same epoch and scale, so direct copy works.
            std::memcpy(dst, src, 8);
            break;
        }

        case StructFieldKind::ObjectField:
            // Marshal object reference as opaque IntPtr.
            std::memcpy(dst, src, sizeof(CHAOS_IL2CPP_INTPTR));
            break;

        case StructFieldKind::GuidField:
            // 16-byte GUID is effectively blittable.
            std::memcpy(dst, src, 16);
            break;
        }
    }
}

// ── Native-to-managed struct marshalling ──────────────────────────────────
//
// Post-call: converts native fields back to managed representation.

void MarshalStructNativeToManaged(
    const StructMarshallingDescriptorV1* desc,
    unsigned char* managed_blob,
    const unsigned char* native_blob,
    RuntimeState* rs,
    ThreadState* ts) noexcept
{
    for (uint16_t i = 0; i < desc->field_count; ++i) {
        const auto& field = desc->fields[i];
        auto* dst = managed_blob + field.offset;
        const auto* src = native_blob + field.offset;

        switch (field.kind) {
        case StructFieldKind::Blittable:
            std::memcpy(dst, src, field.size);
            break;

        case StructFieldKind::BoolField: {
            CHAOS_IL2CPP_INT32 native_val = 0;
            std::memcpy(&native_val, src, sizeof(native_val));
            Win32BoolToBool(dst, native_val);
            break;
        }

        case StructFieldKind::StringField: {
            CHAOS_IL2CPP_INTPTR native_str = 0;
            std::memcpy(&native_str, src, sizeof(native_str));
            if (native_str != 0) {
                auto* managed_str = MarshalPtrToStringUtf8(
                    rs, ts, native_str, -1, false);
                MarshalFreeCoTaskMem(rs, native_str);
                std::memcpy(dst, &managed_str, sizeof(managed_str));
            }
            break;
        }

        case StructFieldKind::NestedStruct:
            if (field.nested != nullptr) {
                MarshalStructNativeToManaged(field.nested, dst, src, rs, ts);
            }
            break;

        case StructFieldKind::ByValArray: {
            CHAOS_IL2CPP_SIZE elem_size = field.size / field.array_count;
            for (uint16_t e = 0; e < field.array_count; ++e) {
                MarshalElementNativeToManaged(
                    field.element_type,
                    dst + e * elem_size,
                    src + e * elem_size,
                    elem_size);
            }
            break;
        }

        case StructFieldKind::LPArray:
            std::memcpy(dst, src, sizeof(CHAOS_IL2CPP_INTPTR));
            break;

        case StructFieldKind::DecimalField:
            std::memcpy(dst, src, 16);
            break;

        case StructFieldKind::DateTimeField:
            std::memcpy(dst, src, 8);
            break;

        case StructFieldKind::ObjectField:
            std::memcpy(dst, src, sizeof(CHAOS_IL2CPP_INTPTR));
            break;

        case StructFieldKind::GuidField:
            std::memcpy(dst, src, 16);
            break;
        }
    }
}

// ── Destroy marshalled struct ─────────────────────────────────────────────
//
// Frees native resources (e.g., CoTaskMem string buffers) allocated during
// managed-to-native marshalling. Call this before freeing the native buffer
// or when Marshal.DestroyStructure is invoked.

void DestroyMarshalledStruct(
    const StructMarshallingDescriptorV1* desc,
    unsigned char* native_blob,
    RuntimeState* rs) noexcept
{
    for (uint16_t i = 0; i < desc->field_count; ++i) {
        const auto& field = desc->fields[i];
        auto* field_ptr = native_blob + field.offset;

        switch (field.kind) {
        case StructFieldKind::StringField: {
            CHAOS_IL2CPP_INTPTR native_str = 0;
            std::memcpy(&native_str, field_ptr, sizeof(native_str));
            if (native_str != 0) {
                MarshalFreeCoTaskMem(rs, native_str);
            }
            break;
        }

        case StructFieldKind::NestedStruct:
            if (field.nested != nullptr) {
                DestroyMarshalledStruct(field.nested, field_ptr, rs);
            }
            break;

        case StructFieldKind::ByValArray: {
            // If array elements are structs with string fields, recurse.
            if (field.element_type == NativeElementType::Struct && field.nested != nullptr) {
                CHAOS_IL2CPP_SIZE elem_size = field.size / field.array_count;
                for (uint16_t e = 0; e < field.array_count; ++e) {
                    DestroyMarshalledStruct(field.nested,
                        field_ptr + e * elem_size, rs);
                }
            }
            break;
        }

        // All other field kinds: no native resources to free.
        default:
            break;
        }
    }
}

}  // namespace chaos::il2cpp::struct_marshal
