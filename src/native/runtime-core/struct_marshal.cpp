#include "marshal_abi.h"
#include "engine_binding.h"
#include "runtime_core.h"

#include <cstring>

namespace chaos::il2cpp::struct_marshal {

using namespace marshal_abi;
using namespace runtime_core;

// ── Managed-to-native struct marshalling ──────────────────────────────────
//
// Walks the descriptor tree and converts each field from managed
// representation (CHAOS_IL2CPP_INTPTR per field) to native representation.
//
// For blittable fields        : memcpy the value at field.offset.
// For string fields           : call MarshalStringToCoTaskMemUtf8.
// For nested struct fields    : recurse with the child descriptor.

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
        }
    }
}

// ── Native-to-managed struct marshalling ──────────────────────────────────
//
// Post-call: converts native fields back to managed representation.
// For string fields: reads the CoTaskMem UTF-8 pointer, creates a managed
// string, then frees the native buffer.

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
        }
    }
}

}  // namespace chaos::il2cpp::struct_marshal
