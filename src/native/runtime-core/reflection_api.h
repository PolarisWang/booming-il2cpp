#ifndef CHAOS_IL2CPP_REFLECTION_API_H_
#define CHAOS_IL2CPP_REFLECTION_API_H_

#include <chaos/native_types.h>

// ── Deferred stub declarations (reflection_api.cpp also includes remaining_stubs.cpp) ──
#include "runtime_stubs/reflection_stubs.h"

// ── extern "C" reflection API functions ──
// These are implemented in reflection_api.cpp and called from generated C++ code.
// Forward declarations are needed because the generated .cpp includes runtime_core.h
// but reflection_api.cpp is a separate compilation unit.

extern "C" {
namespace chaos::il2cpp::runtime_core {
CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeFromHandle(CHAOS_IL2CPP_INTPTR runtime_type_handle);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeByName(CHAOS_IL2CPP_INTPTR name_string_id, CHAOS_IL2CPP_INT32 throw_on_error, CHAOS_IL2CPP_INT32 ignore_case);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssembly(CHAOS_IL2CPP_INTPTR type_handle);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyName(CHAOS_IL2CPP_INTPTR assembly_handle);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyNameValue(CHAOS_IL2CPP_INTPTR assembly_name_handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetDeclaringType(CHAOS_IL2CPP_INTPTR type_handle);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetMemberName(CHAOS_IL2CPP_INTPTR member_handle);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetParameters(CHAOS_IL2CPP_INTPTR method_handle);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetParameterName(CHAOS_IL2CPP_INTPTR parameter_handle);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethodHandle(CHAOS_IL2CPP_INTPTR method_handle);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetMetadataToken(CHAOS_IL2CPP_INTPTR member_handle);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeHandle(CHAOS_IL2CPP_INTPTR type_handle);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetConstructors(CHAOS_IL2CPP_INTPTR type_handle, CHAOS_IL2CPP_INT32 binding_flags);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethods(CHAOS_IL2CPP_INTPTR type_handle);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetFields(CHAOS_IL2CPP_INTPTR type_handle);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetProperties(CHAOS_IL2CPP_INTPTR type_handle);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetInterfaces(CHAOS_IL2CPP_INTPTR type_handle);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetMembers(CHAOS_IL2CPP_INTPTR type_handle);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetNestedTypes(CHAOS_IL2CPP_INTPTR type_handle);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetField(CHAOS_IL2CPP_INTPTR type_handle, CHAOS_IL2CPP_INTPTR name_string_id);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethod(CHAOS_IL2CPP_INTPTR type_handle, CHAOS_IL2CPP_INTPTR name_string_id, CHAOS_IL2CPP_INTPTR param_types);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericArguments(CHAOS_IL2CPP_INTPTR type_handle);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericTypeDefinition(CHAOS_IL2CPP_INTPTR type_handle);
CHAOS_IL2CPP_INTPTR ChaosReflectionCreateInstance(CHAOS_IL2CPP_INTPTR type_handle, CHAOS_IL2CPP_INTPTR args);
CHAOS_IL2CPP_INTPTR ChaosReflectionInvokeMethod(CHAOS_IL2CPP_INTPTR method_handle, CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INTPTR args);
CHAOS_IL2CPP_INTPTR ChaosReflectionMakeGenericMethod(CHAOS_IL2CPP_INTPTR method_handle, CHAOS_IL2CPP_INTPTR type_args);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetCustomAttribute(CHAOS_IL2CPP_INTPTR member_handle, CHAOS_IL2CPP_INTPTR attribute_type_handle);
CHAOS_IL2CPP_INTPTR ChaosReflectionConcatStringPairValues(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeFullName(CHAOS_IL2CPP_INTPTR type_handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyQualifiedName(CHAOS_IL2CPP_INTPTR type_handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetReflectedType(CHAOS_IL2CPP_INTPTR member_handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetType(CHAOS_IL2CPP_INTPTR module_handle, CHAOS_IL2CPP_INTPTR name_string_id) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetTypes(CHAOS_IL2CPP_INTPTR module_handle) noexcept;
// ── Additional reflection API functions (implemented in reflection_api.cpp) ──
CHAOS_IL2CPP_INTPTR ChaosReflectionGetConstructorsDefault(CHAOS_IL2CPP_INTPTR type_handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetBaseType(CHAOS_IL2CPP_INTPTR type_handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetNamespace(CHAOS_IL2CPP_INTPTR type_handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionIsSubclassOf(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR candidate) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionIsAssignableFrom(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR candidate) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionIsInstanceOfType(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR obj) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionIsAssignableTo(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR candidate) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericParamConstraints(CHAOS_IL2CPP_INTPTR type_handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetContainsGenericParams(CHAOS_IL2CPP_INTPTR type_handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericParamPos(CHAOS_IL2CPP_INTPTR type_handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetModuleAssembly(CHAOS_IL2CPP_INTPTR module_handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetModuleName(CHAOS_IL2CPP_INTPTR module_handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetModuleNameOnly(CHAOS_IL2CPP_INTPTR module_handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyFullName(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetCallingAssembly(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetEntryAssembly(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetExecutingAssembly(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetImageRuntimeVersion(CHAOS_IL2CPP_INTPTR assembly) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyLocation(CHAOS_IL2CPP_INTPTR assembly) noexcept;
CHAOS_IL2CPP_INT32 ChaosReflectionGetCallingConvention(CHAOS_IL2CPP_INTPTR method) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetParamPosition(CHAOS_IL2CPP_INTPTR param_handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetParameterType(CHAOS_IL2CPP_INTPTR param_handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetFieldType(CHAOS_IL2CPP_INTPTR field_handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetPropertyType(CHAOS_IL2CPP_INTPTR property_handle) noexcept;
CHAOS_IL2CPP_INT32 ChaosReflectionGetParamAttributes(CHAOS_IL2CPP_INTPTR param) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetFieldsBindingflags(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INT32 flags) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetPropertiesBindingflags(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INT32 flags) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethodsBindingflags(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INT32 flags) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetTypes(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeFromAssemblyBool(CHAOS_IL2CPP_INTPTR assembly, CHAOS_IL2CPP_INTPTR type_name, CHAOS_IL2CPP_INT32 throw_on_error) noexcept;
CHAOS_IL2CPP_INT32 ChaosRuntimetypehandleGetHashCode(CHAOS_IL2CPP_INTPTR handle) noexcept;
CHAOS_IL2CPP_INT32 ChaosRuntimemethodhandleGetHashCode(CHAOS_IL2CPP_INT64 handle) noexcept;
CHAOS_IL2CPP_INT32 ChaosRuntimefieldhandleGetHashCode(CHAOS_IL2CPP_INTPTR handle) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionMakeGenericType(CHAOS_IL2CPP_INTPTR def, CHAOS_IL2CPP_INTPTR args) noexcept;
// ── Type flag stubs (implemented via CHAOS_DEFINE_TYPE_FLAG_STUB in reflection_api.cpp) ──
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsInterface(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsArray(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsEnum(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsAbstract(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsSealed(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsValueType(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsGenericType(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsConstructedGeneric(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsGenericTypeDef(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsPublic(CHAOS_IL2CPP_INTPTR method) noexcept;
}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"

// Expose at global scope for generated code.
using chaos::il2cpp::runtime_core::ChaosReflectionGetTypeFromHandle;
using chaos::il2cpp::runtime_core::ChaosReflectionGetTypeByName;
using chaos::il2cpp::runtime_core::ChaosReflectionGetAssembly;
using chaos::il2cpp::runtime_core::ChaosReflectionGetAssemblyName;
using chaos::il2cpp::runtime_core::ChaosReflectionGetAssemblyNameValue;
using chaos::il2cpp::runtime_core::ChaosReflectionGetDeclaringType;
using chaos::il2cpp::runtime_core::ChaosReflectionGetMemberName;
using chaos::il2cpp::runtime_core::ChaosReflectionGetParameters;
using chaos::il2cpp::runtime_core::ChaosReflectionGetParameterName;
using chaos::il2cpp::runtime_core::ChaosReflectionGetMethodHandle;
using chaos::il2cpp::runtime_core::ChaosReflectionGetMetadataToken;
using chaos::il2cpp::runtime_core::ChaosReflectionGetTypeHandle;
using chaos::il2cpp::runtime_core::ChaosReflectionGetConstructors;
using chaos::il2cpp::runtime_core::ChaosReflectionGetMethods;
using chaos::il2cpp::runtime_core::ChaosReflectionGetFields;
using chaos::il2cpp::runtime_core::ChaosReflectionGetProperties;
using chaos::il2cpp::runtime_core::ChaosReflectionGetInterfaces;
using chaos::il2cpp::runtime_core::ChaosReflectionGetMembers;
using chaos::il2cpp::runtime_core::ChaosReflectionGetNestedTypes;
using chaos::il2cpp::runtime_core::ChaosReflectionGetField;
using chaos::il2cpp::runtime_core::ChaosReflectionGetMethod;
using chaos::il2cpp::runtime_core::ChaosReflectionGetGenericArguments;
using chaos::il2cpp::runtime_core::ChaosReflectionGetGenericTypeDefinition;
using chaos::il2cpp::runtime_core::ChaosReflectionCreateInstance;
using chaos::il2cpp::runtime_core::ChaosReflectionInvokeMethod;
using chaos::il2cpp::runtime_core::ChaosReflectionMakeGenericMethod;
using chaos::il2cpp::runtime_core::ChaosReflectionGetCustomAttribute;
using chaos::il2cpp::runtime_core::ChaosReflectionConcatStringPairValues;
using chaos::il2cpp::runtime_core::ChaosReflectionGetTypeFullName;
using chaos::il2cpp::runtime_core::ChaosReflectionGetAssemblyQualifiedName;
using chaos::il2cpp::runtime_core::ChaosReflectionGetReflectedType;
using chaos::il2cpp::runtime_core::ChaosReflectionModuleGetType;
using chaos::il2cpp::runtime_core::ChaosReflectionModuleGetTypes;
// ── Global scope using declarations for additional reflection APIs ──
using chaos::il2cpp::runtime_core::ChaosReflectionGetConstructorsDefault;
using chaos::il2cpp::runtime_core::ChaosReflectionGetBaseType;
using chaos::il2cpp::runtime_core::ChaosReflectionGetNamespace;
using chaos::il2cpp::runtime_core::ChaosReflectionIsSubclassOf;
using chaos::il2cpp::runtime_core::ChaosReflectionIsAssignableFrom;
using chaos::il2cpp::runtime_core::ChaosReflectionIsInstanceOfType;
using chaos::il2cpp::runtime_core::ChaosReflectionIsAssignableTo;
using chaos::il2cpp::runtime_core::ChaosReflectionGetGenericParamConstraints;
using chaos::il2cpp::runtime_core::ChaosReflectionGetContainsGenericParams;
using chaos::il2cpp::runtime_core::ChaosReflectionGetGenericParamPos;
using chaos::il2cpp::runtime_core::ChaosReflectionGetModuleAssembly;
using chaos::il2cpp::runtime_core::ChaosReflectionGetModuleName;
using chaos::il2cpp::runtime_core::ChaosReflectionGetModuleNameOnly;
using chaos::il2cpp::runtime_core::ChaosReflectionGetAssemblyFullName;
using chaos::il2cpp::runtime_core::ChaosReflectionGetCallingAssembly;
using chaos::il2cpp::runtime_core::ChaosReflectionGetEntryAssembly;
using chaos::il2cpp::runtime_core::ChaosReflectionGetExecutingAssembly;
using chaos::il2cpp::runtime_core::ChaosReflectionGetImageRuntimeVersion;
using chaos::il2cpp::runtime_core::ChaosReflectionGetAssemblyLocation;
using chaos::il2cpp::runtime_core::ChaosReflectionGetCallingConvention;
using chaos::il2cpp::runtime_core::ChaosReflectionGetParamPosition;
using chaos::il2cpp::runtime_core::ChaosReflectionGetParameterType;
using chaos::il2cpp::runtime_core::ChaosReflectionGetFieldType;
using chaos::il2cpp::runtime_core::ChaosReflectionGetPropertyType;
using chaos::il2cpp::runtime_core::ChaosReflectionGetParamAttributes;
using chaos::il2cpp::runtime_core::ChaosReflectionGetFieldsBindingflags;
using chaos::il2cpp::runtime_core::ChaosReflectionGetPropertiesBindingflags;
using chaos::il2cpp::runtime_core::ChaosReflectionGetMethodsBindingflags;
using chaos::il2cpp::runtime_core::ChaosReflectionAssemblyGetTypes;
using chaos::il2cpp::runtime_core::ChaosReflectionGetTypeFromAssemblyBool;
using chaos::il2cpp::runtime_core::ChaosRuntimetypehandleGetHashCode;
using chaos::il2cpp::runtime_core::ChaosReflectionMakeGenericType;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsInterface;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsArray;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsEnum;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsAbstract;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsSealed;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsValueType;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsGenericType;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsConstructedGeneric;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsGenericTypeDef;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsPublic;
using chaos::il2cpp::runtime_core::chaos_reflection_get_is_static;
using chaos::il2cpp::runtime_core::ChaosReflectionFieldGetIsStatic;

#endif  // CHAOS_IL2CPP_REFLECTION_API_H_
