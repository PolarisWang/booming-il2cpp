// reflection_metadata_impl.h — AOT constexpr metadata tables for core System.Private.CoreLib types
//
// This header provides constexpr ReflectionQueryTypeDescriptor instances for the most commonly
// referenced types in System.Private.CoreLib. These tables are compiled into runtime-core and
// shared across all AOT families. Per-family type metadata may be generated in a future
// per-family native-aot.metadata.h.
//
// Tables are in the chaos::il2cpp::runtime_core::aot_metadata namespace and are looked up
// by metadata token range or by name.

#ifndef CHAOS_IL2CPP_REFLECTION_METADATA_IMPL_H_
#define CHAOS_IL2CPP_REFLECTION_METADATA_IMPL_H_

#include "reflection_query_model.h"
#include "module_registry.h"

namespace chaos::il2cpp::runtime_core::aot_metadata {

// ── Forward declarations for the image descriptor ──
const ReflectionQueryTypeDescriptor* FindTypeByMetadataToken(uint32_t metadata_token);

// =====================================================================
// Type: System.Object (token: 0x02000001)
// =====================================================================
constexpr ReflectionQueryFieldDescriptor kObjectFields[] = {
    { 0x04000001, "System.Private.CoreLib/System.Object::_blank", "_blank", "System.Object" },
};

constexpr ReflectionQueryMethodDescriptor kObjectMethods[] = {
    { 0x06000001, "System.Private.CoreLib/System.Object::.ctor()", ".ctor", "System.Void", 0, nullptr, 0 },
    { 0x06000002, "System.Private.CoreLib/System.Object::Finalize()", "Finalize", "System.Void", 0, nullptr, 0 },
    { 0x06000003, "System.Private.CoreLib/System.Object::ToString()", "ToString", "System.String", 0, nullptr, 0 },
    { 0x06000004, "System.Private.CoreLib/System.Object::Equals(System.Object)", "Equals", "System.Boolean", 1, nullptr, 0 },
    { 0x06000005, "System.Private.CoreLib/System.Object::GetHashCode()", "GetHashCode", "System.Int32", 0, nullptr, 0 },
    { 0x06000006, "System.Private.CoreLib/System.Object::GetType()", "GetType", "System.Type", 0, nullptr, 0 },
};

constexpr ReflectionQueryTypeDescriptor kTypeObject = {
    0x02000001,
    "System.Private.CoreLib/System.Object",
    "System.Private.CoreLib/System.Object",
    "System",
    "Object",
    "Object",
    nullptr,
    kObjectFields, 1,
    nullptr, 0,
    kObjectMethods, 6,
};

// =====================================================================
// Type: System.ValueType (token: 0x02000002)
// =====================================================================
constexpr ReflectionQueryMethodDescriptor kValueTypeMethods[] = {
    { 0x06000007, "System.Private.CoreLib/System.ValueType::.ctor()", ".ctor", "System.Void", 0, nullptr, 0 },
    { 0x06000008, "System.Private.CoreLib/System.ValueType::Equals(System.Object)", "Equals", "System.Boolean", 1, nullptr, 0 },
    { 0x06000009, "System.Private.CoreLib/System.ValueType::GetHashCode()", "GetHashCode", "System.Int32", 0, nullptr, 0 },
    { 0x0600000a, "System.Private.CoreLib/System.ValueType::ToString()", "ToString", "System.String", 0, nullptr, 0 },
};

constexpr ReflectionQueryTypeDescriptor kTypeValueType = {
    0x02000002,
    "System.Private.CoreLib/System.ValueType",
    "System.Private.CoreLib/System.ValueType",
    "System",
    "ValueType",
    "ValueType",
    nullptr,
    nullptr, 0,
    nullptr, 0,
    kValueTypeMethods, 4,
};

// =====================================================================
// Type: System.Enum (token: 0x02000003)
// =====================================================================
constexpr ReflectionQueryMethodDescriptor kEnumMethods[] = {
    { 0x0600000b, "System.Private.CoreLib/System.Enum::.ctor()", ".ctor", "System.Void", 0, nullptr, 0 },
    { 0x0600000c, "System.Private.CoreLib/System.Enum::ToString()", "ToString", "System.String", 0, nullptr, 0 },
};

constexpr ReflectionQueryTypeDescriptor kTypeEnum = {
    0x02000003,
    "System.Private.CoreLib/System.Enum",
    "System.Private.CoreLib/System.Enum",
    "System",
    "Enum",
    "Enum",
    &kTypeValueType,
    nullptr, 0,
    nullptr, 0,
    kEnumMethods, 2,
    nullptr, 0,
    kFlagIsEnum | kFlagIsAbstract | kFlagIsPublic,
};

// =====================================================================
// Type: System.Byte (token: 0x0200000f — approximate)
// =====================================================================
constexpr ReflectionQueryFieldDescriptor kByteFields[] = {
    { 0x04000010, "System.Private.CoreLib/System.Byte::m_value", "m_value", "System.Byte" },
};

constexpr ReflectionQueryMethodDescriptor kByteMethods[] = {
    { 0x06000020, "System.Private.CoreLib/System.Byte::.ctor(System.Byte)", ".ctor", "System.Void", 1, nullptr, 0 },
    { 0x06000021, "System.Private.CoreLib/System.Byte::CompareTo(System.Object)", "CompareTo", "System.Int32", 1, nullptr, 0 },
    { 0x06000022, "System.Private.CoreLib/System.Byte::Equals(System.Object)", "Equals", "System.Boolean", 1, nullptr, 0 },
    { 0x06000023, "System.Private.CoreLib/System.Byte::GetHashCode()", "GetHashCode", "System.Int32", 0, nullptr, 0 },
    { 0x06000024, "System.Private.CoreLib/System.Byte::ToString()", "ToString", "System.String", 0, nullptr, 0 },
    { 0x06000025, "System.Private.CoreLib/System.Byte::Parse(System.String)", "Parse", "System.Byte", 1, nullptr, 0 },
};

constexpr ReflectionQueryTypeDescriptor kTypeByte = {
    0x0200000f,
    "System.Private.CoreLib/System.Byte",
    "System.Private.CoreLib/System.Byte",
    "System",
    "Byte",
    "Byte",
    &kTypeValueType,
    kByteFields, 1,
    nullptr, 0,
    kByteMethods, 6,
    nullptr, 0,
    kFlagIsValueType | kFlagIsSealed | kFlagIsPublic,
};

// =====================================================================
// Type: System.String (token: 0x0200000e)
// =====================================================================
constexpr ReflectionQueryMethodDescriptor kStringMethods[] = {
    { 0x06000030, "System.Private.CoreLib/System.String::get_Length()", "get_Length", "System.Int32", 0, nullptr, 0 },
    { 0x06000031, "System.Private.CoreLib/System.String::get_Chars(System.Int32)", "get_Chars", "System.Char", 1, nullptr, 0 },
    { 0x06000032, "System.Private.CoreLib/System.String::Equals(System.String)", "Equals", "System.Boolean", 1, nullptr, 0 },
};

constexpr ReflectionQueryTypeDescriptor kTypeString = {
    0x0200000e,
    "System.Private.CoreLib/System.String",
    "System.Private.CoreLib/System.String",
    "System",
    "String",
    "String",
    &kTypeObject,
    nullptr, 0,
    nullptr, 0,
    kStringMethods, 3,
};

// =====================================================================
// Type: System.Type (token: 0x02000020)
// =====================================================================
constexpr ReflectionQueryMethodDescriptor kTypeTypeMethods[] = {
    { 0x06000050, "System.Private.CoreLib/System.Type::get_Name()", "get_Name", "System.String", 0, nullptr, 0 },
    { 0x06000051, "System.Private.CoreLib/System.Type::get_FullName()", "get_FullName", "System.String", 0, nullptr, 0 },
    { 0x06000052, "System.Private.CoreLib/System.Type::get_Namespace()", "get_Namespace", "System.String", 0, nullptr, 0 },
    { 0x06000053, "System.Private.CoreLib/System.Type::get_IsValueType()", "get_IsValueType", "System.Boolean", 0, nullptr, 0 },
    { 0x06000054, "System.Private.CoreLib/System.Type::get_IsEnum()", "get_IsEnum", "System.Boolean", 0, nullptr, 0 },
    { 0x06000055, "System.Private.CoreLib/System.Type::get_BaseType()", "get_BaseType", "System.Type", 0, nullptr, 0 },
    { 0x06000056, "System.Private.CoreLib/System.Type::GetTypeFromHandle(System.RuntimeTypeHandle)", "GetTypeFromHandle", "System.Type", 1, nullptr, 0 },
};

constexpr ReflectionQueryTypeDescriptor kTypeType = {
    0x02000020,
    "System.Private.CoreLib/System.Type",
    "System.Private.CoreLib/System.Type",
    "System",
    "Type",
    "Type",
    &kTypeObject,
    nullptr, 0,
    nullptr, 0,
    kTypeTypeMethods, 7,
};

// =====================================================================
// Type: System.Exception (token: 0x02000030)
// =====================================================================
constexpr ReflectionQueryFieldDescriptor kExceptionFields[] = {
    { 0x04000040, "System.Private.CoreLib/System.Exception::_message", "_message", "System.Object" },
    { 0x04000041, "System.Private.CoreLib/System.Exception::_innerException", "_innerException", "System.Exception" },
    { 0x04000042, "System.Private.CoreLib/System.Exception::_stackTrace", "_stackTrace", "System.Object" },
    { 0x04000043, "System.Private.CoreLib/System.Exception::_HResult", "_HResult", "System.Int32" },
};

constexpr ReflectionQueryMethodDescriptor kExceptionMethods[] = {
    { 0x06000060, "System.Private.CoreLib/System.Exception::.ctor()", ".ctor", "System.Void", 0, nullptr, 0 },
    { 0x06000061, "System.Private.CoreLib/System.Exception::.ctor(System.String)", ".ctor", "System.Void", 1, nullptr, 0 },
    { 0x06000062, "System.Private.CoreLib/System.Exception::get_Message()", "get_Message", "System.String", 0, nullptr, 0 },
    { 0x06000063, "System.Private.CoreLib/System.Exception::get_InnerException()", "get_InnerException", "System.Exception", 0, nullptr, 0 },
    { 0x06000064, "System.Private.CoreLib/System.Exception::get_StackTrace()", "get_StackTrace", "System.String", 0, nullptr, 0 },
    { 0x06000065, "System.Private.CoreLib/System.Exception::get_HResult()", "get_HResult", "System.Int32", 0, nullptr, 0 },
};

constexpr ReflectionQueryTypeDescriptor kTypeException = {
    0x02000030,
    "System.Private.CoreLib/System.Exception",
    "System.Private.CoreLib/System.Exception",
    "System",
    "Exception",
    "Exception",
    &kTypeObject,
    kExceptionFields, 4,
    nullptr, 0,
    kExceptionMethods, 6,
    nullptr, 0,
    kFlagIsPublic,
};

// =====================================================================
// Type: System.DayOfWeek (token: 0x02000010 — approximate)
// Enum type with 7 named values: Sunday=0 through Saturday=6
// =====================================================================
constexpr ReflectionQueryFieldDescriptor kDayOfWeekFields[] = {
    { 0x04000050, "System.Private.CoreLib/System.DayOfWeek::value__", "value__", "System.Int32", 0LL },
    { 0x04000051, "System.Private.CoreLib/System.DayOfWeek::Sunday", "Sunday", "System.DayOfWeek", 0LL },
    { 0x04000052, "System.Private.CoreLib/System.DayOfWeek::Monday", "Monday", "System.DayOfWeek", 1LL },
    { 0x04000053, "System.Private.CoreLib/System.DayOfWeek::Tuesday", "Tuesday", "System.DayOfWeek", 2LL },
    { 0x04000054, "System.Private.CoreLib/System.DayOfWeek::Wednesday", "Wednesday", "System.DayOfWeek", 3LL },
    { 0x04000055, "System.Private.CoreLib/System.DayOfWeek::Thursday", "Thursday", "System.DayOfWeek", 4LL },
    { 0x04000056, "System.Private.CoreLib/System.DayOfWeek::Friday", "Friday", "System.DayOfWeek", 5LL },
    { 0x04000057, "System.Private.CoreLib/System.DayOfWeek::Saturday", "Saturday", "System.DayOfWeek", 6LL },
};

constexpr ReflectionQueryMethodDescriptor kDayOfWeekMethods[] = {
    { 0x06000070, "System.Private.CoreLib/System.DayOfWeek::.ctor()", ".ctor", "System.Void", 0, nullptr, 0 },
    { 0x06000071, "System.Private.CoreLib/System.DayOfWeek::ToString()", "ToString", "System.String", 0, nullptr, 0 },
};

constexpr ReflectionQueryTypeDescriptor kTypeDayOfWeek = {
    0x02000010,
    "System.Private.CoreLib/System.DayOfWeek",
    "System.Private.CoreLib/System.DayOfWeek",
    "System",
    "DayOfWeek",
    "DayOfWeek",
    &kTypeEnum,
    kDayOfWeekFields, 8,
    nullptr, 0,
    kDayOfWeekMethods, 2,
    nullptr, 0,
    kFlagIsEnum | kFlagIsSealed | kFlagIsPublic,
};

// =====================================================================
// Consolidated type lookup table — all types in this metadata set
// =====================================================================
constexpr ReflectionQueryTypeDescriptor const* kAllTypes[] = {
    &kTypeObject,
    &kTypeValueType,
    &kTypeEnum,
    &kTypeByte,
    &kTypeString,
    &kTypeType,
    &kTypeException,
    &kTypeDayOfWeek,
};

constexpr uint32_t kAllTypeCount = sizeof(kAllTypes) / sizeof(kAllTypes[0]);

// ── Image descriptor representing System.Private.CoreLib ──
constexpr ReflectionQueryImageDescriptor kImageCoreLib = {
    "System.Private.CoreLib",
    kAllTypes,
    kAllTypeCount,
};

// ── Lookup helpers ──

inline const ReflectionQueryTypeDescriptor* FindTypeByMetadataToken(uint32_t metadata_token) {
    for (uint32_t i = 0; i < kAllTypeCount; i++) {
        if (kAllTypes[i] != nullptr && kAllTypes[i]->metadata_token == metadata_token) {
            return kAllTypes[i];
        }
    }
    return nullptr;
}

inline const ReflectionQueryTypeDescriptor* FindTypeByName(const char* namespace_utf8, const char* name_utf8) {
    for (uint32_t i = 0; i < kAllTypeCount; i++) {
        auto* t = kAllTypes[i];
        if (t != nullptr &&
            ::chaos::il2cpp::runtime_core::NamesMatch(t->namespace_name_utf8, namespace_utf8) &&
            ::chaos::il2cpp::runtime_core::NamesMatch(t->name_utf8, name_utf8)) {
            return t;
        }
    }
    return nullptr;
}

}  // namespace chaos::il2cpp::runtime_core::aot_metadata

#endif  // CHAOS_IL2CPP_REFLECTION_METADATA_IMPL_H_
