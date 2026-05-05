#ifndef CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_
#define CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_

// ── Runtime API declarations exposed to generated code ──
//
// This header provides declarations used by generated .cpp files via
// runtime_core.h. After the Hybrid TypeInfo* migration, all 28 passing
// families generate self-contained C++ code.

#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include "arithmetic_chaos_bridge.h"
#include "codegen_bridge.h"       // DispatchEntryV0, NameIndex structs, kDispatchPatched

// ── Object header definition ──
// type_info: type identity for GC, casting, reflection
// vtable: virtual method dispatch table
struct chaos_object_header {
    const void**    vtable      = nullptr;  // [0] virtual method table
    const TypeInfo* type_info   = nullptr;   // [8] type identity
};

// ── Managed string type ──────────────────────────────────────────
// Used by generated code for reinterpret_cast access to string length.
struct chaos_managed_string {
    chaos_object_header header{};
    CHAOS_IL2CPP_INT32 length = 0;
};

#define CHAOS_IL2CPP_STRING_TYPE chaos_managed_string

// ── Reflection struct type definitions ────────────────────────────
// Used by generated code for reflection object marshalling.
// The codegen emits extern runtime functions that work with these
// struct layouts; the definitions must stay in sync.

struct chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo {
    chaos_object_header header{};
    CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;
    CHAOS_IL2CPP_INTPTR runtime_method_handle = 0;
    CHAOS_IL2CPP_INTPTR generic_definition_method_handle = 0;
    CHAOS_IL2CPP_INTPTR generic_argument_type_handle = 0;
    CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
    CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo {
    chaos_object_header header{};
    CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;
    CHAOS_IL2CPP_INTPTR runtime_method_handle = 0;
    CHAOS_IL2CPP_INTPTR generic_definition_method_handle = 0;
    CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
    CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo {
    chaos_object_header header{};
    CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;
    CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
    CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_Assembly {
    chaos_object_header header{};
    CHAOS_IL2CPP_INTPTR runtime_assembly_handle = 0;
    CHAOS_IL2CPP_INTPTR runtime_assembly_name_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName {
    chaos_object_header header{};
    CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
};

// ── Exception metadata functions (extern C declarations) ──
// Implementations live in exception_api.cpp.
namespace chaos::il2cpp::runtime_core {
extern "C" {
void ChaosReflectionSetExceptionMetadata(CHAOS_IL2CPP_INTPTR exception_obj, CHAOS_IL2CPP_INTPTR message_value);
CHAOS_IL2CPP_INTPTR ChaosReflectionGetExceptionMessage(CHAOS_IL2CPP_INTPTR exception_obj);
}  // extern "C"

// ── Runtime stub declarations ───────────────────────────────────
// Stub implementations in runtime_stubs.cpp. These are thin wrappers
// called by generated extern "C" external-runtime-helper functions.
extern "C" {
// Array
void    ChaosArrayClear(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 count) noexcept;
CHAOS_IL2CPP_INT32 ChaosArrayGetLength(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 dimension) noexcept;
// Type marshalling
CHAOS_IL2CPP_INTPTR ChaosStoreInt64(CHAOS_IL2CPP_INT64 value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStoreFloat64(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_INT64  ChaosLoadInt64(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosLoadFloat64(CHAOS_IL2CPP_INTPTR value) noexcept;
// Buffer
CHAOS_IL2CPP_INT32  ChaosBufferByteLength(CHAOS_IL2CPP_INTPTR array) noexcept;
// DateTime
CHAOS_IL2CPP_INT64  ChaosDatetimeGetUtcNow(void) noexcept;
CHAOS_IL2CPP_INT32  ChaosDatetimeGetHashCode(CHAOS_IL2CPP_INTPTR datetime) noexcept;
// Math
CHAOS_IL2CPP_INT64  ChaosMathSqrt(CHAOS_IL2CPP_INT64 value) noexcept;
// Threading
void    ChaosInterlockedMemoryBarrier(void) noexcept;
// Exception
CHAOS_IL2CPP_INTPTR ChaosExceptionGetBaseException(CHAOS_IL2CPP_INTPTR exc) noexcept;
CHAOS_IL2CPP_INTPTR ChaosExceptionGetInnerException(CHAOS_IL2CPP_INTPTR exc) noexcept;
CHAOS_IL2CPP_INT32  ChaosExceptionGetHresult(CHAOS_IL2CPP_INTPTR exc) noexcept;
// Object
CHAOS_IL2CPP_INTPTR ChaosObjectEqualsStatic(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) noexcept;
void    ChaosObjectCtor(CHAOS_IL2CPP_INTPTR obj) noexcept;
// GUID / Random
CHAOS_IL2CPP_INTPTR ChaosGuidNewGuid(void) noexcept;
void    ChaosRandomNextBytes(CHAOS_IL2CPP_INTPTR rng, CHAOS_IL2CPP_INTPTR buffer) noexcept;
CHAOS_IL2CPP_INT64  ChaosRandomNextDouble(CHAOS_IL2CPP_INTPTR rng) noexcept;
// Culture
CHAOS_IL2CPP_INTPTR ChaosCultureGetCurrent(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetInvariant(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetCompareInfo(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetDateTimeFormat(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetDisplayName(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetName(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetNumberFormat(CHAOS_IL2CPP_INTPTR culture) noexcept;
// String
CHAOS_IL2CPP_INTPTR ChaosStringContains(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringJoinSs(CHAOS_IL2CPP_INTPTR separator, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStringStartsWith(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value) noexcept;
// Reflection
CHAOS_IL2CPP_INTPTR ChaosReflectionIsDefined(CHAOS_IL2CPP_INTPTR assembly, CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionIsSubclassOf(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR base) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsGenericType(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsValueType(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetNamespace(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetReflectedType(CHAOS_IL2CPP_INTPTR member) noexcept;
CHAOS_IL2CPP_INT32  ChaosReflectionGetCallingConvention(CHAOS_IL2CPP_INTPTR method) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsPublic(CHAOS_IL2CPP_INTPTR member) noexcept;
// Module reflection
CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetType(CHAOS_IL2CPP_INTPTR module, CHAOS_IL2CPP_INTPTR name) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetTypes(CHAOS_IL2CPP_INTPTR module) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetModuleAssembly(CHAOS_IL2CPP_INTPTR module) noexcept;
// Parameter reflection
CHAOS_IL2CPP_INTPTR ChaosReflectionGetRequiredCustomModifiers(CHAOS_IL2CPP_INTPTR param) noexcept;
CHAOS_IL2CPP_INT32  ChaosReflectionGetParamAttributes(CHAOS_IL2CPP_INTPTR param) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetDefaultValue(CHAOS_IL2CPP_INTPTR param) noexcept;
// Reflection image/constructed-generic
CHAOS_IL2CPP_INTPTR ChaosReflectionGetImageRuntimeVersion(CHAOS_IL2CPP_INTPTR assembly) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsConstructedGeneric(CHAOS_IL2CPP_INTPTR type) noexcept;
// Runtime helpers
CHAOS_IL2CPP_INTPTR ChaosFormattablestringFactoryCreate(CHAOS_IL2CPP_INTPTR format, CHAOS_IL2CPP_INTPTR args) noexcept;
CHAOS_IL2CPP_INTPTR ChaosRuntimeHelpersEquals(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) noexcept;
CHAOS_IL2CPP_INT32  ChaosRuntimeHelpersGetHashCode(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosRuntimeHelpersGetObjectValue(CHAOS_IL2CPP_INTPTR value) noexcept;
// Runtime method handle
CHAOS_IL2CPP_INT32  ChaosRuntimemethodhandleGetHashCode(CHAOS_IL2CPP_INT64 handle) noexcept;
// Runtime-wrapped exception
CHAOS_IL2CPP_INTPTR ChaosRuntimewrappedGetWrappedException(CHAOS_IL2CPP_INTPTR exc) noexcept;
// Volatile
CHAOS_IL2CPP_INT32  ChaosVolatileRead(CHAOS_IL2CPP_INTPTR ptr) noexcept;
	// Reflection supplementary
	CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsSealed(CHAOS_IL2CPP_INTPTR type) noexcept;
	CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsGenericTypeDef(CHAOS_IL2CPP_INTPTR type) noexcept;
	CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeFromAssemblyBool(CHAOS_IL2CPP_INTPTR assembly, CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INT32 throw_on_error);
	CHAOS_IL2CPP_INTPTR ChaosReflectionGetExecutingAssembly(void) noexcept;
	CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyLocation(CHAOS_IL2CPP_INTPTR assembly) noexcept;
	CHAOS_IL2CPP_INTPTR ChaosGetCustomAttributeFromBlob(CHAOS_IL2CPP_INTPTR member_kind, CHAOS_IL2CPP_INTPTR member_handle, CHAOS_IL2CPP_INTPTR attr_type_handle) noexcept;
// Float32 marshalling
CHAOS_IL2CPP_INTPTR ChaosStoreFloat32(CHAOS_IL2CPP_FLOAT32 value) noexcept;
// Reflection assignable
CHAOS_IL2CPP_INTPTR ChaosReflectionIsAssignableFrom(CHAOS_IL2CPP_INTPTR target, CHAOS_IL2CPP_INTPTR source) noexcept;
// Reflection generic
CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericParamConstraints(CHAOS_IL2CPP_INTPTR type) noexcept;
// Reflection member info
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsVirtual(CHAOS_IL2CPP_INTPTR member) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsInterface(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsArray(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyFullName(CHAOS_IL2CPP_INTPTR assembly) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericParamPos(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsEnum(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsAbstract(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyQualifiedName(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetTypes(CHAOS_IL2CPP_INTPTR assembly) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetBaseDefinition(CHAOS_IL2CPP_INTPTR member) noexcept;
// Reflection field handle
CHAOS_IL2CPP_INT32  ChaosRuntimefieldhandleGetHashCode(CHAOS_IL2CPP_INTPTR handle) noexcept;
// Reflection module
CHAOS_IL2CPP_INTPTR ChaosReflectionGetModuleName(CHAOS_IL2CPP_INTPTR module) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetModuleNameOnly(CHAOS_IL2CPP_INTPTR module) noexcept;
// Parameter reflection (supplementary)
CHAOS_IL2CPP_INTPTR ChaosReflectionHasDefaultValue(CHAOS_IL2CPP_INTPTR param) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetParameterType(CHAOS_IL2CPP_INTPTR param) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetParamPosition(CHAOS_IL2CPP_INTPTR param) noexcept;
// Reflection type handle
CHAOS_IL2CPP_INT32  ChaosRuntimetypehandleGetHashCode(CHAOS_IL2CPP_INTPTR handle) noexcept;
// Reflection constructors
CHAOS_IL2CPP_INTPTR ChaosReflectionGetConstructorsDefault(CHAOS_IL2CPP_INTPTR type) noexcept;
// Reflection assignable (supplementary)
CHAOS_IL2CPP_INTPTR ChaosReflectionIsAssignableTo(CHAOS_IL2CPP_INTPTR target, CHAOS_IL2CPP_INTPTR source) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionIsInstanceOfType(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INTPTR type) noexcept;
// Assembly reflection
CHAOS_IL2CPP_INTPTR ChaosReflectionGetCallingAssembly(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetEntryAssembly(void) noexcept;
// Generic type reflection
CHAOS_IL2CPP_INTPTR ChaosReflectionMakeGenericType(CHAOS_IL2CPP_INTPTR def, CHAOS_IL2CPP_INTPTR args) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetContainsGenericParams(CHAOS_IL2CPP_INTPTR type) noexcept;
// Parameter default value
CHAOS_IL2CPP_INTPTR ChaosReflectionGetRawDefaultValue(CHAOS_IL2CPP_INTPTR param) noexcept;
// Type reflection
CHAOS_IL2CPP_INTPTR ChaosReflectionGetFieldsBindingflags(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INT32 flags) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethodsBindingflags(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INT32 flags) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetBaseType(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeFullName(CHAOS_IL2CPP_INTPTR type) noexcept;
}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core

// Expose at global scope so generated code (inside anonymous namespaces)
// can call these as bare identifiers.
using chaos::il2cpp::runtime_core::ChaosArrayClear;
using chaos::il2cpp::runtime_core::ChaosArrayGetLength;
using chaos::il2cpp::runtime_core::ChaosBufferByteLength;
using chaos::il2cpp::runtime_core::ChaosCultureGetCompareInfo;
using chaos::il2cpp::runtime_core::ChaosCultureGetCurrent;
using chaos::il2cpp::runtime_core::ChaosCultureGetDateTimeFormat;
using chaos::il2cpp::runtime_core::ChaosCultureGetDisplayName;
using chaos::il2cpp::runtime_core::ChaosCultureGetInvariant;
using chaos::il2cpp::runtime_core::ChaosCultureGetName;
using chaos::il2cpp::runtime_core::ChaosCultureGetNumberFormat;
using chaos::il2cpp::runtime_core::ChaosDatetimeGetHashCode;
using chaos::il2cpp::runtime_core::ChaosDatetimeGetUtcNow;
using chaos::il2cpp::runtime_core::ChaosExceptionGetBaseException;
using chaos::il2cpp::runtime_core::ChaosExceptionGetHresult;
using chaos::il2cpp::runtime_core::ChaosExceptionGetInnerException;
using chaos::il2cpp::runtime_core::ChaosFormattablestringFactoryCreate;
using chaos::il2cpp::runtime_core::ChaosGuidNewGuid;
using chaos::il2cpp::runtime_core::ChaosInterlockedMemoryBarrier;
using chaos::il2cpp::runtime_core::ChaosLoadFloat64;
using chaos::il2cpp::runtime_core::ChaosLoadInt64;
using chaos::il2cpp::runtime_core::ChaosMathSqrt;
using chaos::il2cpp::runtime_core::ChaosObjectCtor;
using chaos::il2cpp::runtime_core::ChaosObjectEqualsStatic;
using chaos::il2cpp::runtime_core::ChaosRandomNextBytes;
using chaos::il2cpp::runtime_core::ChaosRandomNextDouble;
using chaos::il2cpp::runtime_core::ChaosReflectionAssemblyGetTypes;
using chaos::il2cpp::runtime_core::ChaosReflectionGetAssemblyFullName;
using chaos::il2cpp::runtime_core::ChaosReflectionGetAssemblyLocation;
using chaos::il2cpp::runtime_core::ChaosReflectionGetAssemblyQualifiedName;
using chaos::il2cpp::runtime_core::ChaosReflectionGetBaseDefinition;
using chaos::il2cpp::runtime_core::ChaosReflectionGetBaseType;
using chaos::il2cpp::runtime_core::ChaosReflectionGetCallingAssembly;
using chaos::il2cpp::runtime_core::ChaosReflectionGetCallingConvention;
using chaos::il2cpp::runtime_core::ChaosReflectionGetConstructorsDefault;
using chaos::il2cpp::runtime_core::ChaosReflectionGetContainsGenericParams;
using chaos::il2cpp::runtime_core::ChaosReflectionGetDefaultValue;
using chaos::il2cpp::runtime_core::ChaosReflectionGetEntryAssembly;
using chaos::il2cpp::runtime_core::ChaosReflectionGetExceptionMessage;
using chaos::il2cpp::runtime_core::ChaosReflectionGetExecutingAssembly;
using chaos::il2cpp::runtime_core::ChaosReflectionGetFieldsBindingflags;
using chaos::il2cpp::runtime_core::ChaosReflectionGetGenericParamConstraints;
using chaos::il2cpp::runtime_core::ChaosReflectionGetGenericParamPos;
using chaos::il2cpp::runtime_core::ChaosReflectionGetImageRuntimeVersion;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsAbstract;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsArray;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsConstructedGeneric;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsEnum;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsGenericType;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsGenericTypeDef;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsInterface;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsPublic;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsSealed;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsValueType;
using chaos::il2cpp::runtime_core::ChaosReflectionGetIsVirtual;
using chaos::il2cpp::runtime_core::ChaosReflectionGetMethodsBindingflags;
using chaos::il2cpp::runtime_core::ChaosReflectionGetModuleAssembly;
using chaos::il2cpp::runtime_core::ChaosReflectionGetModuleName;
using chaos::il2cpp::runtime_core::ChaosReflectionGetModuleNameOnly;
using chaos::il2cpp::runtime_core::ChaosReflectionGetNamespace;
using chaos::il2cpp::runtime_core::ChaosReflectionGetParamAttributes;
using chaos::il2cpp::runtime_core::ChaosReflectionGetParamPosition;
using chaos::il2cpp::runtime_core::ChaosReflectionGetParameterType;
using chaos::il2cpp::runtime_core::ChaosReflectionGetRawDefaultValue;
using chaos::il2cpp::runtime_core::ChaosReflectionGetReflectedType;
using chaos::il2cpp::runtime_core::ChaosReflectionGetRequiredCustomModifiers;
using chaos::il2cpp::runtime_core::ChaosReflectionGetTypeFromAssemblyBool;
using chaos::il2cpp::runtime_core::ChaosReflectionGetTypeFullName;
using chaos::il2cpp::runtime_core::ChaosReflectionHasDefaultValue;
using chaos::il2cpp::runtime_core::ChaosReflectionIsAssignableFrom;
using chaos::il2cpp::runtime_core::ChaosReflectionIsAssignableTo;
using chaos::il2cpp::runtime_core::ChaosReflectionIsDefined;
using chaos::il2cpp::runtime_core::ChaosReflectionIsInstanceOfType;
using chaos::il2cpp::runtime_core::ChaosReflectionIsSubclassOf;
using chaos::il2cpp::runtime_core::ChaosReflectionMakeGenericType;
using chaos::il2cpp::runtime_core::ChaosReflectionModuleGetType;
using chaos::il2cpp::runtime_core::ChaosReflectionModuleGetTypes;
using chaos::il2cpp::runtime_core::ChaosReflectionSetExceptionMetadata;
using chaos::il2cpp::runtime_core::ChaosRuntimeHelpersEquals;
using chaos::il2cpp::runtime_core::ChaosRuntimeHelpersGetHashCode;
using chaos::il2cpp::runtime_core::ChaosRuntimeHelpersGetObjectValue;
using chaos::il2cpp::runtime_core::ChaosRuntimefieldhandleGetHashCode;
using chaos::il2cpp::runtime_core::ChaosRuntimemethodhandleGetHashCode;
using chaos::il2cpp::runtime_core::ChaosRuntimetypehandleGetHashCode;
using chaos::il2cpp::runtime_core::ChaosRuntimewrappedGetWrappedException;
using chaos::il2cpp::runtime_core::ChaosStoreFloat32;
using chaos::il2cpp::runtime_core::ChaosStoreFloat64;
using chaos::il2cpp::runtime_core::ChaosStoreInt64;
using chaos::il2cpp::runtime_core::ChaosStringContains;
using chaos::il2cpp::runtime_core::ChaosStringJoinSs;
using chaos::il2cpp::runtime_core::ChaosStringStartsWith;
using chaos::il2cpp::runtime_core::ChaosVolatileRead;
using chaos::il2cpp::runtime_core::ChaosGetCustomAttributeFromBlob;

#endif  // CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_
