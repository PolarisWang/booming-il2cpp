#ifndef CHAOS_IL2CPP_REFLECTION_QUERY_MODEL_H_
#define CHAOS_IL2CPP_REFLECTION_QUERY_MODEL_H_

#include "codegen_bridge.h"

#include <chaos/native_types.h>

#include <cstdint>
#include <cstring>

// Declaration for codegen-visible custom attribute blob lookup.
// Defined in reflection/attributes.cpp, called from generated code.
extern "C" CHAOS_IL2CPP_INTPTR ChaosGetCustomAttributeFromBlob(
    CHAOS_IL2CPP_INTPTR member_kind,
    CHAOS_IL2CPP_INTPTR member_handle,
    CHAOS_IL2CPP_INTPTR attr_type_handle) noexcept;

namespace chaos::il2cpp::runtime_core {

struct ReflectionQueryParameterDescriptor {
    const char* subject_id_utf8;
    const char* name_utf8;
    CHAOS_IL2CPP_UINT32 parameter_index;
    const char* member_type_utf8;
    CHAOS_IL2CPP_INTPTR default_value_blob;   // 0 = no default value; raw ECMA Constant blob
    CHAOS_IL2CPP_UINT32 default_value_size;    // 0 = no default value
};

struct ReflectionQueryMethodDescriptor {
    CHAOS_IL2CPP_UINT32 metadata_token;
    const char* subject_id_utf8;
    const char* name_utf8;
    const char* member_type_utf8;
    CHAOS_IL2CPP_INT32 parameter_count;
    const ReflectionQueryParameterDescriptor* parameters;
    CHAOS_IL2CPP_UINT32 parameter_descriptor_count;
    const void* default_value_blob;
    CHAOS_IL2CPP_UINT32 flags;          // Method attribute flags (kMethodFlag*), must be last for ABI compat
};

// Method descriptor flags (populated from managed metadata)
static constexpr CHAOS_IL2CPP_UINT32 kMethodFlagIsPublic  = 1u << 0;
static constexpr CHAOS_IL2CPP_UINT32 kMethodFlagIsStatic  = 1u << 1;
static constexpr CHAOS_IL2CPP_UINT32 kMethodFlagIsVirtual = 1u << 2;

struct ReflectionQueryFieldDescriptor {
    CHAOS_IL2CPP_UINT32 metadata_token;
    const char* subject_id_utf8;
    const char* name_utf8;
    const char* member_type_utf8;
    CHAOS_IL2CPP_INT64 constant_value;  // For enum literal fields: the constant value; 0 otherwise
    CHAOS_IL2CPP_UINT32 flags;          // Field attribute flags (kFieldFlag*), must be last for ABI compat
};

// Field descriptor flags (populated from managed metadata)
static constexpr CHAOS_IL2CPP_UINT32 kFieldFlagIsPublic   = 1u << 0;
static constexpr CHAOS_IL2CPP_UINT32 kFieldFlagIsStatic   = 1u << 1;
static constexpr CHAOS_IL2CPP_UINT32 kFieldFlagIsInitOnly = 1u << 2;  // readonly
static constexpr CHAOS_IL2CPP_UINT32 kFieldFlagIsLiteral  = 1u << 3;  // const

struct ReflectionQueryPropertyDescriptor {
    const char* subject_id_utf8;
    const char* name_utf8;
    const char* member_type_utf8;
    CHAOS_IL2CPP_UINT32 flags;          // Property attribute flags (kPropertyFlag*), must be last for ABI compat
};

// Property descriptor flags (populated from managed metadata)
static constexpr CHAOS_IL2CPP_UINT32 kPropertyFlagIsStatic = 1u << 0;
static constexpr CHAOS_IL2CPP_UINT32 kPropertyFlagCanRead  = 1u << 1;
static constexpr CHAOS_IL2CPP_UINT32 kPropertyFlagCanWrite = 1u << 2;

struct ReflectionQueryEventDescriptor {
    const char* subject_id_utf8;
    const char* name_utf8;
    const char* member_type_utf8;  // EventHandler type
    CHAOS_IL2CPP_UINT32 flags;    // kEventFlag*, must be last for ABI compat
};

// Event descriptor flags
static constexpr CHAOS_IL2CPP_UINT32 kEventFlagIsStatic = 1u << 0;

struct ReflectionQueryTypeDescriptor {
    CHAOS_IL2CPP_UINT32 metadata_token;
    const char* subject_id_utf8;
    const char* definition_subject_id_utf8;
    const char* namespace_name_utf8;
    const char* name_utf8;
    const char* display_name_utf8;
    const ReflectionQueryTypeDescriptor* generic_type_definition;
    const ReflectionQueryFieldDescriptor* fields;
    CHAOS_IL2CPP_UINT32 field_count;
    const ReflectionQueryPropertyDescriptor* properties;
    CHAOS_IL2CPP_UINT32 property_count;
    const ReflectionQueryEventDescriptor* events;
    CHAOS_IL2CPP_UINT32 event_count;
    const ReflectionQueryMethodDescriptor* methods;
    CHAOS_IL2CPP_UINT32 method_count;
    const void* generic_parameters;
    CHAOS_IL2CPP_UINT32 generic_param_count;
    CHAOS_IL2CPP_UINT32 reserved_flags;
    /// Pointer to the AOT TypeInfoHot* for this type, or nullptr.
    /// Populated at startup by ChaosReflectionGetTypeFromHandle.
    /// Used by hierarchy.cpp fast path to skip O(n) module scan.
    const void* type_info_ptr;
};

struct ReflectionQueryImageDescriptor {
    const char* image_name_utf8;
    const ReflectionQueryTypeDescriptor* const* types;
    CHAOS_IL2CPP_UINT32 type_count;
    CHAOS_IL2CPP_UINT16 version_major;
    CHAOS_IL2CPP_UINT16 version_minor;
    CHAOS_IL2CPP_UINT16 version_build;
    CHAOS_IL2CPP_UINT16 version_revision;
};

constexpr CHAOS_IL2CPP_UINTPTR kReflectionQueryHandleTag =
    static_cast<CHAOS_IL2CPP_UINTPTR>(1) << ((sizeof(CHAOS_IL2CPP_UINTPTR) * 8u) - 1u);

template <typename THandle, typename TDescriptor>
inline THandle EncodeReflectionQueryHandle(const TDescriptor* descriptor) {
    return descriptor == nullptr
        ? static_cast<THandle>(0)
        : static_cast<THandle>(reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(descriptor) | kReflectionQueryHandleTag);
}

template <typename TDescriptor, typename THandle>
inline const TDescriptor* TryDecodeReflectionQueryHandle(THandle handle) {
    const CHAOS_IL2CPP_UINTPTR raw_handle = static_cast<CHAOS_IL2CPP_UINTPTR>(handle);
    if ((raw_handle & kReflectionQueryHandleTag) == 0u) {
        return nullptr;
    }

    return reinterpret_cast<const TDescriptor*>(raw_handle & ~kReflectionQueryHandleTag);
}

inline ImageHandle EncodeReflectionQueryImageHandle(const ReflectionQueryImageDescriptor* descriptor) {
    return EncodeReflectionQueryHandle<ImageHandle>(descriptor);
}

inline TypeInfoHandle EncodeReflectionQueryTypeHandle(const ReflectionQueryTypeDescriptor* descriptor) {
    return EncodeReflectionQueryHandle<TypeInfoHandle>(descriptor);
}

inline FieldInfoHandle EncodeReflectionQueryFieldHandle(const ReflectionQueryFieldDescriptor* descriptor) {
    return EncodeReflectionQueryHandle<FieldInfoHandle>(descriptor);
}

inline PropertyInfoHandle EncodeReflectionQueryPropertyHandle(const ReflectionQueryPropertyDescriptor* descriptor) {
    return EncodeReflectionQueryHandle<PropertyInfoHandle>(descriptor);
}

inline EventInfoHandle EncodeReflectionQueryEventHandle(const ReflectionQueryEventDescriptor* descriptor) {
    return EncodeReflectionQueryHandle<EventInfoHandle>(descriptor);
}

inline MethodInfoHandle EncodeReflectionQueryMethodHandle(const ReflectionQueryMethodDescriptor* descriptor) {
    return EncodeReflectionQueryHandle<MethodInfoHandle>(descriptor);
}

inline ParameterInfoHandle EncodeReflectionQueryParameterHandle(const ReflectionQueryParameterDescriptor* descriptor) {
    return EncodeReflectionQueryHandle<ParameterInfoHandle>(descriptor);
}

inline const ReflectionQueryImageDescriptor* TryDecodeReflectionQueryImageHandle(ImageHandle image) {
    return TryDecodeReflectionQueryHandle<ReflectionQueryImageDescriptor>(image);
}

inline const ReflectionQueryTypeDescriptor* TryDecodeReflectionQueryTypeHandle(TypeInfoHandle type) {
    return TryDecodeReflectionQueryHandle<ReflectionQueryTypeDescriptor>(type);
}

inline const ReflectionQueryMethodDescriptor* TryDecodeReflectionQueryMethodHandle(MethodInfoHandle method) {
    return TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(method);
}

inline bool NamesMatch(const char* left, const char* right) {
    const char* normalized_left = left != nullptr ? left : "";
    const char* normalized_right = right != nullptr ? right : "";
    return CHAOS_IL2CPP_STRCMP(normalized_left, normalized_right) == 0;
}

inline const ReflectionQueryTypeDescriptor* FindReflectionQueryTypeByToken(
    const ReflectionQueryImageDescriptor* image,
    CHAOS_IL2CPP_UINT32 metadata_token) {
    if (image == nullptr || image->types == nullptr || metadata_token == 0u) {
        return nullptr;
    }

    for (CHAOS_IL2CPP_UINT32 index = 0u; index < image->type_count; index++) {
        const ReflectionQueryTypeDescriptor* type = image->types[index];
        if (type != nullptr && type->metadata_token == metadata_token) {
            return type;
        }
    }

    return nullptr;
}

inline const ReflectionQueryTypeDescriptor* FindReflectionQueryTypeByName(
    const ReflectionQueryImageDescriptor* image,
    const char* namespace_utf8,
    const char* type_name_utf8) {
    if (image == nullptr || image->types == nullptr || namespace_utf8 == nullptr || type_name_utf8 == nullptr) {
        return nullptr;
    }

    for (CHAOS_IL2CPP_UINT32 index = 0u; index < image->type_count; index++) {
        const ReflectionQueryTypeDescriptor* type = image->types[index];
        if (type != nullptr &&
            NamesMatch(type->namespace_name_utf8, namespace_utf8) &&
            NamesMatch(type->name_utf8, type_name_utf8)) {
            return type;
        }
    }

    return nullptr;
}

inline const ReflectionQueryFieldDescriptor* FindReflectionQueryFieldByToken(
    const ReflectionQueryImageDescriptor* image,
    CHAOS_IL2CPP_UINT32 metadata_token) {
    if (image == nullptr || image->types == nullptr || metadata_token == 0u) {
        return nullptr;
    }

    for (CHAOS_IL2CPP_UINT32 type_index = 0u; type_index < image->type_count; type_index++) {
        const ReflectionQueryTypeDescriptor* type = image->types[type_index];
        if (type == nullptr || type->fields == nullptr) {
            continue;
        }

        for (CHAOS_IL2CPP_UINT32 field_index = 0u; field_index < type->field_count; field_index++) {
            const ReflectionQueryFieldDescriptor* field = &type->fields[field_index];
            if (field->metadata_token == metadata_token) {
                return field;
            }
        }
    }

    return nullptr;
}

inline const ReflectionQueryMethodDescriptor* FindReflectionQueryMethodByToken(
    const ReflectionQueryImageDescriptor* image,
    CHAOS_IL2CPP_UINT32 metadata_token) {
    if (image == nullptr || image->types == nullptr || metadata_token == 0u) {
        return nullptr;
    }

    for (CHAOS_IL2CPP_UINT32 type_index = 0u; type_index < image->type_count; type_index++) {
        const ReflectionQueryTypeDescriptor* type = image->types[type_index];
        if (type == nullptr || type->methods == nullptr) {
            continue;
        }

        for (CHAOS_IL2CPP_UINT32 method_index = 0u; method_index < type->method_count; method_index++) {
            const ReflectionQueryMethodDescriptor* method = &type->methods[method_index];
            if (method->metadata_token == metadata_token) {
                return method;
            }
        }
    }

    return nullptr;
}

inline const ReflectionQueryFieldDescriptor* FindReflectionQueryField(
    const ReflectionQueryTypeDescriptor* type,
    const char* field_name_utf8) {
    if (type == nullptr || type->fields == nullptr || field_name_utf8 == nullptr) {
        return nullptr;
    }

    for (CHAOS_IL2CPP_UINT32 index = 0u; index < type->field_count; index++) {
        const ReflectionQueryFieldDescriptor* field = &type->fields[index];
        if (NamesMatch(field->name_utf8, field_name_utf8)) {
            return field;
        }
    }

    return nullptr;
}

inline const ReflectionQueryPropertyDescriptor* FindReflectionQueryProperty(
    const ReflectionQueryTypeDescriptor* type,
    const char* property_name_utf8) {
    if (type == nullptr || type->properties == nullptr || property_name_utf8 == nullptr) {
        return nullptr;
    }

    for (CHAOS_IL2CPP_UINT32 index = 0u; index < type->property_count; index++) {
        const ReflectionQueryPropertyDescriptor* property = &type->properties[index];
        if (NamesMatch(property->name_utf8, property_name_utf8)) {
            return property;
        }
    }

    return nullptr;
}

inline const ReflectionQueryEventDescriptor* FindReflectionQueryEvent(
    const ReflectionQueryTypeDescriptor* type,
    const char* event_name_utf8) {
    if (type == nullptr || type->events == nullptr || event_name_utf8 == nullptr) {
        return nullptr;
    }

    for (CHAOS_IL2CPP_UINT32 index = 0u; index < type->event_count; index++) {
        const ReflectionQueryEventDescriptor* event = &type->events[index];
        if (NamesMatch(event->name_utf8, event_name_utf8)) {
            return event;
        }
    }

    return nullptr;
}

inline const ReflectionQueryMethodDescriptor* FindReflectionQueryMethod(
    const ReflectionQueryTypeDescriptor* type,
    const char* method_name_utf8,
    CHAOS_IL2CPP_INT32 parameter_count) {
    if (type == nullptr || type->methods == nullptr || method_name_utf8 == nullptr || parameter_count < 0) {
        return nullptr;
    }

    for (CHAOS_IL2CPP_UINT32 index = 0u; index < type->method_count; index++) {
        const ReflectionQueryMethodDescriptor* method = &type->methods[index];
        if (NamesMatch(method->name_utf8, method_name_utf8) && method->parameter_count == parameter_count) {
            return method;
        }
    }

    return nullptr;
}

inline const ReflectionQueryParameterDescriptor* FindReflectionQueryParameter(
    const ReflectionQueryMethodDescriptor* method,
    CHAOS_IL2CPP_UINT32 parameter_index) {
    if (method == nullptr || method->parameters == nullptr) {
        return nullptr;
    }

    for (CHAOS_IL2CPP_UINT32 index = 0u; index < method->parameter_descriptor_count; index++) {
        const ReflectionQueryParameterDescriptor* parameter = &method->parameters[index];
        if (parameter->parameter_index == parameter_index) {
            return parameter;
        }
    }

    return nullptr;
}

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_REFLECTION_QUERY_MODEL_H_
