#ifndef CHAOS_IL2CPP_REFLECTION_API_H_
#define CHAOS_IL2CPP_REFLECTION_API_H_

#include <chaos/native_types.h>

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

#endif  // CHAOS_IL2CPP_REFLECTION_API_H_
